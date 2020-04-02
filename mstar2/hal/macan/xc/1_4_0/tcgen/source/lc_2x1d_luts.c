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
#include "DMFxPtQs.h"
#include "lc_2x1d_luts.h"
#include "L2PQ_LUTs.h"
#include "PQ2L_LUT_9b.h"
#include "PQ2L_LUT_11b.h"




// Function Tuned for the folloing ranges in floaing point
// Smax 100-10000
// Smin 0.005 to 40
// TGamma 1 to 4

void calcTaTb(S32 Sgamma , U32  uSmax , U32 uSmin , S32 *Ta , S32 *Tb)
{
    S32  lfactMax , lfactMin ,  lTgamma_n, ldenom , log2T , lfactDiff , TgammaS , ldiff_n  , ltemp;
    S32  Smax ,  Smin;

    Smax = uSmax >> (QLINEAR-QTMAXTMIN);
    Smin = uSmin >> (QLINEAR-QTMAXTMIN);

    lTgamma_n   = unorm_s(Sgamma);
    TgammaS     = sshl_su(Sgamma,lTgamma_n);
    ldenom      = sdiv_sls((S64)0x1<<(30+31),TgammaS);//result in 1/Sgamma * 2^(30+31-lTgamma_n-QGAMMA)


    /* (Smax.^(1/Sgamma) */
    log2T = fxLOG2Taylor(Smax,QLOG2OUT);            //log2(2^(QTMAXTMIN)*Smax)*2^QLOG2OUT
    log2T = ssub_ss(log2T,(QTMAXTMIN) << QLOG2OUT);
    lfactMax = smpy_ss(ldenom,log2T);                   // 1/Sgamma * log2(Smax)*2^(30-lTgamma_n-QGAMMA+QLOG2OUT)
    lfactMax = fxPOW2Taylor(lfactMax,30-lTgamma_n-QGAMMA + QLOG2OUT,QDIFF); // result in Q_TA_TB

    /* (Smin.^(1/Sgamma) */
    log2T = fxLOG2Taylor(Smin,QLOG2OUT);            //log2(2^(QTMAXTMIN)*Smax)*2^QLOG2OUT
    log2T = ssub_ss(log2T,(QTMAXTMIN) << QLOG2OUT);
    lfactMin = smpy_ss(ldenom,log2T);                   // 1/Sgamma * log2(Smax)*2^(30-lTgamma_n-QGAMMA+QLOG2OUT)
    lfactMin = fxPOW2Taylor(lfactMin,30-lTgamma_n-QGAMMA + QLOG2OUT,QDIFF); // result in Q_TA_TB

    /*(Smax.^(1/Sgamma)-Smin.^(1/Sgamma))*/
    lfactDiff = ssub_ss(lfactMax,lfactMin);

    /* (Smax.^(1/Sgamma)-Smin.^(1/Sgamma)).^Sgamma; */
    log2T = fxLOG2Taylor(lfactDiff,QLOG2OUT);            //log2(2^(QTMAXTMIN)*Smax)*2^QLOG2OUT
    log2T = ssub_ss(log2T,QDIFF << QLOG2OUT);
    ltemp = smpy_ss(TgammaS,log2T);                   // 1/Sgamma * log2(Smax)*2^(30-lTgamma_n-QGAMMA+QLOG2OUT)
    *Ta = fxPOW2Taylor(ltemp,lTgamma_n+QGAMMA + QLOG2OUT-31 , QTA); // result in QTA


    /*  Smin.^(1/Sgamma) / (Smax.^(1/Sgamma) - Smin.^(1/Sgamma)) */
    ldiff_n   = unorm_s(lfactDiff);
    ldenom    = sshl_su(lfactDiff,ldiff_n);
    ldenom    = sdiv_sls((S64)1<<(30+31),ldenom);//30+31-ln-Q_TA_TB/lfactDiff
    ltemp        = smpy_ss(ldenom,lfactMin);        //30-ln-Q_TA_TB/lfactDiff * lfactMin & 2^Q_TA_TB
    *Tb        = sshr_sss(ltemp,30-ldiff_n-QTB);

}

U16 Linear_to_PQ14(U32 Y)
{
    S32 l2_pq_index =0;
    S32 ltemp;
    U64 lltemp;


    while(1)
    {
        if( Y <=L2PQ_Offset1[l2_pq_index])
        {
            break;
        }
        else
        {
            l2_pq_index++;
        }
    }

    ltemp   = Y-L2PQ_Offset1[l2_pq_index-1];                    // in QL2PQ_OFFSET1
    lltemp  = lmpy_ss(ltemp,L2PQ_Scale[l2_pq_index-1]);         // in QL2PQ_OFFSET1 + QL2PQ_SCALE
    ltemp   = sshr_slu(lltemp,(QLINEAR + QL2PQ_SCALE)-QL2PQ_OFFSET2);
    ltemp   = sadd_ss(ltemp,L2PQ_Offset2[l2_pq_index-1]);
    ltemp   = (ltemp + (1<<(QL2PQ_OFFSET2-QGAMMA2PQ-1))) >> (QL2PQ_OFFSET2-QGAMMA2PQ) ;
    ltemp   = sminss((1<<QGAMMA2PQ)-1,ltemp);
    return (U16)ltemp;
}

void Gamma2PQ(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax , U16 *pGamma2PQLut )
{
    S32 iloop;
    S32  Ta , Tb  , log2T , lTgamma_n , TgammaS;
    S32 l2_pq_index ;
    S32 divFactor;




    calcTaTb(Sgamma ,  uSmax ,  uSmin , &Ta , &Tb);

    divFactor = 0x7FFFFFFF/(SRangeMax-SRangeMin);


    lTgamma_n   = unorm_s(Sgamma);
    TgammaS     = sshl_su(Sgamma,lTgamma_n);

    l2_pq_index = 1;
    for(iloop = 0 ; iloop < GAMMA2PQ_LUTSIZE ; iloop++)
    {
        S32 X , ltemp ,ln1;
        S32 index;
        U32 Y ;
        U64 lltemp;

        index = (iloop - SRangeMin);
        index = smaxss(0,index);
        lltemp = lmpy_ss(index,divFactor);
        X  = sshr_slu(lltemp, 31-QTB);
        X  = sminss(X , (1<<QTB)-1);

        /* Gamma to Linear */
        X = sadd_ss(X,Tb);
        X = smaxss(0,X);


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

        /* Linear to PQ */
        Y     =  Y > 2621440000U ? 2621440000U : Y;        // last point in the LUt

        while(1)
        {
            if( Y <=L2PQ_Offset1[l2_pq_index])
            {
                break;
            }
            else
            {
                l2_pq_index++;
            }
        }

        ltemp   = Y-L2PQ_Offset1[l2_pq_index-1];                    // in QL2PQ_OFFSET1
        lltemp  = lmpy_ss(ltemp,L2PQ_Scale[l2_pq_index-1]);         // in QL2PQ_OFFSET1 + QL2PQ_SCALE
        ltemp   = sshr_slu(lltemp,(QLINEAR + QL2PQ_SCALE)-QL2PQ_OFFSET2);
        ltemp   = sadd_ss(ltemp,L2PQ_Offset2[l2_pq_index-1]);
        ltemp   = (ltemp + (1<<(QL2PQ_OFFSET2-QGAMMA2PQ-1))) >> (QL2PQ_OFFSET2-QGAMMA2PQ) ;
        ltemp   = sminss((1<<QGAMMA2PQ)-1,ltemp);
        pGamma2PQLut[iloop] = (U16)ltemp;

    }// end of iloop




}/* end of Gamma2PQ */

void PQ2Gamma(S32 Tgamma , U32  uTmax , U32 uTmin , U16 TRangeMin , U16 TRangeMax, U16 *pPQ2GammaLut )
{
    S32 iloop;
    S32  Ta , Tb  ,  Tgn ,  Tan ,divFactor ;


    calcTaTb(Tgamma ,  uTmax ,  uTmin , &Ta , &Tb);
    divFactor = 0x7FFFFFFF/(TRangeMax-TRangeMin);

    Tan   = unorm_s(Ta);
    Ta    = sshl_su(Ta,Tan);
    Ta    = sdiv_sls((S64)0x1<<(30+31),Ta);//result in 1/Ta * 2^(30+31-Tan-QTA)

    Tgn   = unorm_s(Tgamma);
    Tgamma    = sshl_su(Tgamma,Tgn);
    Tgamma    = sdiv_sls((S64)0x1<<(30+31),Tgamma);//result in 1/Tg * 2^(30+31-Tgn-QTGAMMA)

    Tb +=  (1<<(QTB-QPQ2GAMMA-1));
    Tb >>= (QTB-QPQ2GAMMA);

#define QLOCALLOG2OUT 24

    for(iloop = 0 ; iloop < (1<<PQ2L_LOG22LUTSIZE) ; iloop++)
    {
        S32 lin,lscale , index;
        U32 ulin;
        U64 lltemp;

        index  = (iloop - TRangeMin);
        index  = smaxss(0,index);
        lltemp = lmpy_ss(index,divFactor);
        lltemp += (1<<(31-PQ2L_LOG22LUTSIZE-1));
        index  = sshr_slu(lltemp, 31-PQ2L_LOG22LUTSIZE);
        index  = sminss(index , (1<<PQ2L_LOG22LUTSIZE)-1);


        ulin = PQ2L_LUT_9b[index] << PQ2L_LUT_9b_norm[index];
        lltemp = (S64)ulin * Ta;                                // 30+31-Tan-TQA+ln1+PQ2L_LUTFRACBITS
        lin = lltemp >> 32;                                     // 30+31-32-Tan-TQA+ln1 +PQ2L_LUTFRACBITS

        lin     = fxLOG2Taylor(lin,QLOCALLOG2OUT);
        lscale  = (PQ2L_LUT_9b_norm[index]+QLINEAR+30+31-32-QTA-Tan) << QLOCALLOG2OUT;
        lin     = ssub_ss(lin,lscale);     //log2(X./Ta)*2^QLOG2OUT
        lin     = smpy_ss(lin,Tgamma);     //1/Tgamma * log2(X./Ta)*2^(QLOG2OUT+30-Tgn-TGAMMA)
        lin     = fxPOW2Taylor(lin,QLOCALLOG2OUT+30-Tgn-QGAMMA,QPQ2GAMMA);
        lin     = ssub_ss(lin,Tb);
        //lin     = sadd_sss(lin , 1<<(QTB-QPQ2GAMMA-1));
        //lin     = sshr_ssu(lin ,QTB-QPQ2GAMMA);
        lin     = sminss(0xfff,smaxss(lin,0));

        pPQ2GammaLut[iloop] = (U16)lin;
    }





}/* end of PQ2Gamma */

U32 PQ14_to_Linear(U16 inPQ)
{
    S32 index ,  xDiffNum ,  yDiff;
    U32 lin_Out;


    index = inPQ >> (QPQIN-PQ2L_LOG22LUTSIZE); // PQin 10b

    // Interpolate
    xDiffNum    =   inPQ % (1<<(QPQIN-PQ2L_LOG22LUTSIZE));
    yDiff       =   PQ2L_LUT_9b[index+1] - PQ2L_LUT_9b[index];

    lin_Out     =   (yDiff * xDiffNum) ;
    lin_Out     +=  (1<<(QPQIN-PQ2L_LOG22LUTSIZE-1));
    lin_Out     >>= (QPQIN-PQ2L_LOG22LUTSIZE);
    lin_Out     +=  PQ2L_LUT_9b[index];

    return lin_Out;

}/* end of PQ14_to_Linear() */

U32 PQ14_to_Linear_11bLut(U16 inPQ)
{
    S32 index ,  xDiffNum ,  yDiff;
    U32 lin_Out;


    index = inPQ >> (QPQIN-11); // PQin 10b

    // Interpolate
    xDiffNum    =   inPQ % (1<<(QPQIN-11));
    yDiff       =   PQ2L_LUT_11b[index+1] - PQ2L_LUT_11b[index];

    lin_Out     =   (yDiff * xDiffNum) ;
    lin_Out     +=  (1<<(QPQIN-11-1));
    lin_Out     >>= (QPQIN-11);
    lin_Out     +=  PQ2L_LUT_11b[index];

    return lin_Out;

}/* end of PQ14_to_Linear() */


void Gamma2PQX(S32 Sgamma , U32  uSmax , U32 uSmin , U16 SRangeMin , U16 SRangeMax , S16 *pGamma2PQLut )
{
    S32 iloop;//,kloop;
    S32  Ta , Tb  , log2T , lTgamma_n , TgammaS;
    S32 l2_pq_index ;
    S32 divFactor;
    //S32 offset;
    U32 G2LX_Offset;



    calcTaTb(Sgamma ,  uSmax ,  uSmin , &Ta , &Tb);

    divFactor = 0x7FFFFFFF/(SRangeMax-SRangeMin);


    lTgamma_n   = unorm_s(Sgamma);
    TgammaS     = sshl_su(Sgamma,lTgamma_n);

    G2LX_Offset = 0;
    l2_pq_index = 1;
    for(iloop = SRangeMin ; iloop < GAMMA2PQX_LUTSIZE ; iloop++)
    {
        S32 X , ltemp ,ln1;
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
        Y     =  Y > 2621440000U ? 2621440000U : Y;        // last point in the LUt

        while(1)
        {
            if( Y <=L2PQ_Offset1[l2_pq_index])
            {
                break;
            }
            else
            {
                l2_pq_index++;
            }
        }

        ltemp   = Y-L2PQ_Offset1[l2_pq_index-1];                    // in QL2PQ_OFFSET1
        lltemp  = lmpy_ss(ltemp,L2PQ_Scale[l2_pq_index-1]);         // in QL2PQ_OFFSET1 + QL2PQ_SCALE
        ltemp   = sshr_slu(lltemp,(QLINEAR + QL2PQ_SCALE)-QL2PQ_OFFSET2);
        ltemp   = sadd_ss(ltemp,L2PQ_Offset2[l2_pq_index-1]);
        ltemp   = (ltemp + (1<<(QL2PQ_OFFSET2-QGAMMA2PQ-1))) >> (QL2PQ_OFFSET2-QGAMMA2PQ) ;
        ltemp   = sminss(0x3fff,ltemp);
        pGamma2PQLut[iloop] = (S16)(ltemp);

    }// end of iloop

#if 0
    offset = pGamma2PQLut[SRangeMin]*2; //offset

    for(iloop = SRangeMin-1 , kloop = SRangeMin+1; iloop >=0 ; iloop--)
    {
        S32 ltemp;
        ltemp = -pGamma2PQLut[kloop++];
        ltemp = sadd_ss(ltemp,offset);
        ltemp = smaxss(0,ltemp);
        pGamma2PQLut[iloop] = (U16)(ltemp);
    }// end of iloop
#else

    for(iloop = 0 ; iloop < SRangeMin ; iloop++)
    {
        S32 X , ltemp ,ln1;
        S32 index;
        U32 Y ;
        S32 sign;
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

        sign = 0;
        if(2*G2LX_Offset < Y )
        {
            sign = 1;
            Y = Y - 2*G2LX_Offset;
        }
        else
        {
            Y = 2*G2LX_Offset - Y;
        }
        /* Linear to PQ */

        l2_pq_index = 1;
        while(1)
        {
            if( Y <=L2PQ_Offset1[l2_pq_index])
            {
                break;
            }
            else
            {
                l2_pq_index++;
            }
        }

        ltemp   = Y-L2PQ_Offset1[l2_pq_index-1];                    // in QL2PQ_OFFSET1
        lltemp  = lmpy_ss(ltemp,L2PQ_Scale[l2_pq_index-1]);         // in QL2PQ_OFFSET1 + QL2PQ_SCALE
        ltemp   = sshr_slu(lltemp,(QLINEAR + QL2PQ_SCALE)-QL2PQ_OFFSET2);
        ltemp   = sadd_ss(ltemp,L2PQ_Offset2[l2_pq_index-1]);
        ltemp   = (ltemp + (1<<(QL2PQ_OFFSET2-QGAMMA2PQ-1))) >> (QL2PQ_OFFSET2-QGAMMA2PQ) ;
        ltemp   = sminss((1<<QGAMMA2PQ)-1,ltemp);
        ltemp   = smaxss(-((1<<QGAMMA2PQ)-1),ltemp);
        if(sign) ltemp = -ltemp;
        pGamma2PQLut[iloop] = (S16)(ltemp);

    }// end of iloop

#endif


}/* end of Gamma2PQX */

