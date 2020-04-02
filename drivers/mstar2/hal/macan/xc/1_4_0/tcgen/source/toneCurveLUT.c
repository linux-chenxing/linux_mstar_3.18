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

#include "generateLutlogXTable.h"
#include <asm/div64.h>


void CalcIMappedParams  (S32 *Mid,S32 Clip,S32 TminPQ,S32 TmaxPQ,S32 SminPQ,S32 SmaxPQ,
                         S32 *Shift,S32 *Min,S32 *Max , S32 *Slope)
{


    S32 ltemp1,ltemp2,ldenom,ldenom_n1,key , ldenom2,ldenom_n2;
    S64 lltemp1;
    S32 s2t_ratio,ShiftFactor;
    S32 Crush , sdrDisplayMaxPQ, sdrContentMidPQ;

    *Mid            = sminss(*Mid,SmaxPQ);
    sdrDisplayMaxPQ = Clip;
    sdrContentMidPQ = *Mid;
    Clip            = TmaxPQ;
    Crush           = TminPQ;

    // DMParams.S2Tratio = min(1, sqrt((SmaxPQ-SminPQ)/(TmaxPQ-TminPQ)));
    ltemp1      = SmaxPQ - SminPQ;
    ltemp2      = TmaxPQ - TminPQ;
    ldenom_n1   = unorm_s(ltemp2);
    ldenom      = sshl_su(ltemp2,ldenom_n1);//(diff) * 2^(ldenom_n1)
    ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);//(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
    lltemp1     = lmpy_ss(ldenom,ltemp1);
    ltemp1      = sshr_slu(lltemp1,30-ldenom_n1);
    s2t_ratio   = ssqrt_s(ltemp1);

    //Mid = sdrContentMidPQ / S2Tratio;
    ldenom_n2   = unorm_s(s2t_ratio);
    ldenom2     = sshl_su(s2t_ratio,ldenom_n2);//(s2t_ratio) * 2^(ldenom_n2)
    ldenom2     = sdiv_sls((S64)0x1<<(30+31),ldenom2);//(1/s2t_ratio) = ldenom2 * 2^(30+31-ldenom2);
    lltemp1     = lmpy_ss(ldenom2,sdrContentMidPQ);//sdrContentMidPQ / S2Tratio *2^(30-ldenom2);
    *Mid        = sshr_slu(lltemp1,(30-ldenom_n2));

    //  SlopeIn = sqrt(1/DMParams.S2Tratio) * DMParams.ScreenRatio;
    ldenom_n1   = unorm_s(s2t_ratio);
    ldenom      = sshl_su(s2t_ratio,ldenom_n1);
    ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);
    ldenom      = sshr_ssu(ldenom,30-ldenom_n1-23);
    ldenom      = ssqrt_s(ldenom);
    ldenom      = sshr_ssu(ldenom,-((23-31) + ((31-23)/2)));
    *Slope      = sshl_ssu(ldenom , QSLOPEIN-23);//lltemp1,(23+QSCREEN_RATIO)-QSLOPEIN);

    // DMParams.ShiftFactor = (1 - DMParams.S2Tratio) ;
    ShiftFactor = SCF(1.0) -  s2t_ratio;



    // (Mid-TminPQ)
    ltemp1      = ssub_ss(*Mid,TminPQ);


    //1/(TMax-TMin);
    ltemp2      = ssub_ss(TmaxPQ,TminPQ);
    ldenom_n1   = unorm_s(ltemp2);
    ldenom      = sshl_su(ltemp2,ldenom_n1);
    ldenom      = sdiv_sls((S64)1<<(30+31),ldenom);//(1/double(Clip-Crush)) = double(ldenom)* 2^(double(ldenom_n1)-31-30)

    //Key = (Mid-Crush)/(Clip-Crush);
    lltemp1     = lmpy_ss(ldenom,ltemp1);
    ldenom_n2   = unorm_ul( lltemp1);
    lltemp1     <<= ldenom_n2;
    key         = sshr_slu(lltemp1,32);

    //(Key/0.5) * DMParams.ShiftFactor
    lltemp1      = lmpy_ss(key,ShiftFactor);
    ltemp1      =  sshr_slu(lltemp1,30-ldenom_n1-(32-ldenom_n2)+31-1) ; // /0.5  (Key/0.5) * DMParams.ShiftFactor * 2^31

    // Shift = Mid * (Key/0.5) * DMParams.ShiftFactor;
    *Shift       = smpy_ss(ltemp1,*Mid);

    *Min = smaxss(Crush-*Shift, SminPQ );
    *Max = sminss(Clip-*Shift,sdrDisplayMaxPQ);

    ltemp1 = sminss(*Max,*Mid-*Shift);
    ltemp1 = smaxss(*Min,ltemp1);
    *Mid = sadd_sss(ltemp1,*Shift);


    return ;
}

void CalcMappedParams   (S32 Crush,S32 Mid,S32 Clip,
                         displayPqTuning_t_    *pDisplayTuning,
                         S32 SminPQ,S32 SmaxPQ,
                         S32 Sdiagonalinches,S32 *TMin,S32 *TMid,S32 *TMax ,S32 *SMid, S32 *Slope)
{


    S32 ltemp1,ltemp2,ldenom,ldenom_n1,key,ldenom_n2;
    S64 lltemp1;
    S32 s2t_ratio,screen_ratio,ShiftFactor,Shift;
    S32 limitUp,limitDown;


    // DMParams.S2Tratio = min(1, sqrt((TmaxPQ-TminPQ)/(SmaxPQ-SminPQ)));
    lltemp1      = (S64)pDisplayTuning->tMaxPQ + (S64)pDisplayTuning->tMinPQ+1;
    ltemp1       = (S32)(lltemp1>>1);
    lltemp1      = (S64)SmaxPQ + (S64)SminPQ+1;
    ltemp2       = (S32)(lltemp1>>1);

    ldenom_n1   = unorm_s(ltemp2);
    ldenom      = sshl_su(ltemp2,ldenom_n1);//(diff) * 2^(ldenom_n1)
    ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);//(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
    lltemp1     = lmpy_ss(ldenom,ltemp1);
    ltemp1      = sshr_slu(lltemp1,30-ldenom_n1);
    s2t_ratio   = ssqrt_s(ltemp1);

    // ScreenRatio = sqrt(log2(Tdiagonalinches) / log2(Sdiagonalinches));
    if(pDisplayTuning->tDiagonalInches==Sdiagonalinches)
    {
        screen_ratio =  1<<QSCREEN_RATIO;
    }
    else
    {
        ltemp1      = fxLOG2Taylor(pDisplayTuning->tDiagonalInches,QLOG2OUT);
        ltemp2      = fxLOG2Taylor(Sdiagonalinches,QLOG2OUT);//log2(Sdiagonalinches) * 2^QLOG2OUT
        ldenom_n1   = unorm_s(ltemp2);
        ldenom      = sshl_su(ltemp2,ldenom_n1);//log2(Sdiagonalinches) * 2^(QLOG2OUT+ldenom_n1)
        ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);//%(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
        lltemp1     = lmpy_ss(ldenom,ltemp1);
        ldenom      = sshr_slu(lltemp1,31+30-ldenom_n1-QSCREEN_RATIO);

        // input to sqrt root is in Q21(ratioQ)
        ldenom       = ssqrt_s(ldenom);
        //sqrt accepts input in Q31 ..so compensate for the difference
        screen_ratio = sshr_sss(ldenom,((31-QSCREEN_RATIO)/2)); // first term to get output in ratioQ ,
        // second term to go compensate the input to sqrt
    }

    //  SlopeIn = sqrt(1/DMParams.S2Tratio) * DMParams.ScreenRatio;
    ldenom_n1   = unorm_s(s2t_ratio);
    ldenom      = sshl_su(s2t_ratio,ldenom_n1);
    ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);
    ldenom      = sshr_ssu(ldenom,30-ldenom_n1-23);
    ldenom      = ssqrt_s(ldenom);
    ldenom      = sshr_ssu(ldenom,-((23-31) + ((31-23)/2)));
    lltemp1     = lmpy_ss(ldenom,screen_ratio);
    ltemp1      = sshr_slu(lltemp1,(23+QSCREEN_RATIO)-QSLOPEIN);
    ltemp2      = smpy_ss(ltemp1,pDisplayTuning->tContrast); //Slope *=  (1 + DMParams.DContrast);
    *Slope      = sadd_sss(ltemp1,ltemp2);

    // DMParams.ShiftFactor = (1 - DMParams.S2Tratio) * DMParams.ScreenRatio;
    ShiftFactor = SCF(1.0) -  s2t_ratio;
    ShiftFactor = smpy_ss(ShiftFactor,screen_ratio);


    // (Mid-SminPQ)
    ltemp1      = ssub_ss(Mid,SminPQ);

    //1/(SMax-SMin);
    ltemp2      = ssub_ss(SmaxPQ,SminPQ);
    ldenom_n1   = unorm_s(ltemp2);
    ldenom      = sshl_su(ltemp2,ldenom_n1);
    ldenom      = sdiv_sls((S64)1<<(30+31),ldenom);//(1/double(Clip-Crush)) = double(ldenom)* 2^(double(ldenom_n1)-31-30)

    //Key = (Mid-Crush)/(Clip-Crush);
    lltemp1     = lmpy_ss(ldenom,ltemp1);
    ldenom_n2   = unorm_ul( lltemp1);
    lltemp1     <<= ldenom_n2;
    key         = sshr_slu(lltemp1,32);

    //(Key/0.5) * DMParams.ShiftFactor
    lltemp1      = lmpy_ss(key,ShiftFactor);
    ltemp1      =  sshr_slu(lltemp1,30-ldenom_n1-32+ldenom_n2+QSCREEN_RATIO-1) ; // /0.5  (Key/0.5) * DMParams.ShiftFactor * 2^31

    // Shift = Mid * (Key/0.5) * DMParams.ShiftFactor;
    //Shift = Mid * (1 - S2Tratio) * Key * ScreenRatio + DMParams.DBrightness;
    ltemp1  = smpy_ss(ltemp1,Mid);
    Shift  = sadd_sss(ltemp1,pDisplayTuning->tBrightness);

    *TMin = sadd_sss(smaxss(ssub_sss(Crush,Shift), pDisplayTuning->tMinPQ ),pDisplayTuning->tMinPQBias);
    *TMax = sadd_sss(sminss(ssub_sss(Clip,Shift), pDisplayTuning->tMaxPQ ),pDisplayTuning->tMaxPQBias);


    ltemp1 = sminss(*TMax,sadd_sss(Mid-Shift,pDisplayTuning->tMidPQBias));
    ltemp1 = smaxss(*TMin,ltemp1);
    *SMid = sadd_sss(ltemp1,Shift-pDisplayTuning->tMidPQBias);

    ///LUP = (TMax-TMin)*0.85+TMin;
    limitUp = smpy_ss(*TMax-*TMin,SCF(0.85));
    limitUp = sadd_sss(limitUp,*TMin);

    //LDOWN = (TMax-TMin)*0.15+TMin;
    limitDown = smpy_ss(*TMax-*TMin,SCF(0.15));
    limitDown = sadd_sss(limitDown,*TMin);

    //SMid = max(LDOWN, min(LUP, SMid));
    *SMid = smaxss(limitDown,sminss(limitUp,*SMid));

    //TMid = SMid - Shift + DMParams.TMidPQBias;
    ltemp1 = *SMid - Shift;
    *TMid = sadd_sss(ltemp1,pDisplayTuning->tMidPQBias);

    //TMid = max(LDOWN, min(LUP, TMid));
    *TMid = smaxss(limitDown, sminss(limitUp, *TMid));

    return ;
}


void CalcCurveParams    (S32 SMin,S32 SMid,S32 SMax ,S32 TMin,S32 TMid,S32 TMax,S32 Slope,
                         displayPqTuning_t_     *pDisplayTuning,
                         val_norm_pair_t        *cParam1,
                         val_norm_pair_t        *cParam2,
                         val_norm_pair_t        *cParam3,
                         S32                    *slopeOverRollOff)
{

    S32 ltemp2,ltemp_x1,ltemp_xn1,ltemp_x2,ltemp_xn2,ltemp_x3,ltemp_xn3,ltempS_R , ltemp_n;
    S32 ltemp_y1, ltemp_yn1,ltemp_y2,ltemp_yn2,ltemp_y3,ltemp_yn3;
    S32 ltemp_xs1,ltemp_xs2,ltemp_xs3;
    S32 ltemp_ys1,ltemp_ys2,ltemp_ys3;
    S32 ldenom,ldenom_n1;
    S64 lltemp2;
    S64 lltemp_x3y1  , lltemp_x3y2   , lltemp_x2y3;
    S32 iSign;

    S32 ltemp_x1x2,ltemp_x1x3,ltemp_x2x3;
    S64 lltemp1;

    // Slope*RolloffInv
    lltemp2  =  lmpy_ss(Slope,sshl_su(pDisplayTuning->tRolloffInv,29));
    ltempS_R  =  sshr_slu(lltemp2 ,31-(QSLOPEROLLOFF - QSLOPEIN +2));

    // log2(Crush)
    ltemp2   = fxLOG2Taylor(SMin,QLOG2OUT);
    ltemp2   = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));//log2(Crush)*2^QLOG2OUT

    ltemp_x1 = smpy_ss(ltemp2, ltempS_R );
    ltemp_xs1 = fxPOW2Taylor(ltemp_x1,QLOG2OUT+QSLOPEROLLOFF-31,31);
    ltemp_xn1= unorm_s(ltemp_xs1);
    ltemp_x1 = sshl_su( ltemp_xs1,ltemp_xn1);


    // log2(Mid)
    ltemp2   = fxLOG2Taylor(SMid,QLOG2OUT);
    ltemp2   = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));

    ltemp_x2 = smpy_ss(ltemp2, ltempS_R );
    ltemp_xs2 = fxPOW2Taylor(ltemp_x2,QLOG2OUT+QSLOPEROLLOFF-31,31);
    ltemp_xn2= unorm_s(ltemp_xs2);
    ltemp_x2 = sshl_su( ltemp_xs2,ltemp_xn2);


    // log2(Clip)
    ltemp2   = fxLOG2Taylor(SMax,QLOG2OUT);
    ltemp2   = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));
    ltemp_x3 = smpy_ss(ltemp2, ltempS_R );
    ltemp_xs3 = fxPOW2Taylor(ltemp_x3,QLOG2OUT+QSLOPEROLLOFF-31,31);
    ltemp_xn3= unorm_s(ltemp_xs3);
    ltemp_x3 = sshl_su( ltemp_xs3,ltemp_xn3);


    // log2(TMin)
    iSign = 1;
    if(TMin < 0)
    {
        iSign = -1;
        TMin = -TMin;
    }
    ltemp2 = fxLOG2Taylor(TMin,QLOG2OUT);
    ltemp2 = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));
    ltemp_y1 = smpy_ss(ltemp2,sshl_su(pDisplayTuning->tRolloffInv,31-(QLOG2OUT-QPOW2IN)));
    ltemp_ys1 = fxPOW2Taylor(ltemp_y1,QPOW2IN,31);
    ltemp_ys1 *= iSign;
    ltemp_yn1= unorm_s(ltemp_ys1);
    ltemp_y1 = sshl_su( ltemp_ys1,ltemp_yn1);

    // log2(TMid)
    iSign = 1;
    if(TMid<0)
    {
        iSign = -1;
        TMid = -TMid;
    }
    ltemp2   = fxLOG2Taylor(TMid,QLOG2OUT);
    ltemp2   = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));
    ltemp_y2 = smpy_ss(ltemp2,sshl_su(pDisplayTuning->tRolloffInv,31-(QLOG2OUT-QPOW2IN)));
    ltemp_ys2 = fxPOW2Taylor(ltemp_y2,QPOW2IN,31);
    ltemp_ys2 *=iSign;
    ltemp_yn2= unorm_s(ltemp_ys2);
    ltemp_y2 = sshl_su( ltemp_ys2,ltemp_yn2);

    // log2(TMax)
    iSign = 1;
    if(TMax < 0)
    {
        TMax = -TMax;
        iSign = -1;
    }
    ltemp2   = fxLOG2Taylor(TMax,QLOG2OUT);
    ltemp2   = ssub_ss(ltemp2,sshl_su(QB02DATAIN,QLOG2OUT));
    ltemp_y3 = smpy_ss(ltemp2,sshl_su(pDisplayTuning->tRolloffInv,31-(QLOG2OUT-QPOW2IN)));
    ltemp_ys3 = fxPOW2Taylor(ltemp_y3,QPOW2IN,31);
    ltemp_ys3 *=iSign;
    ltemp_yn3= unorm_s(ltemp_ys3);
    ltemp_y3 = sshl_su( ltemp_ys3,ltemp_yn3);


    //// temp = x3*y3*(x1-x2)+x2*y2*(x3-x1)+x1*y1*(x2-x3);
    ltemp_x1x2  = smpy_ss(ltemp_x1,ltemp_x2);
    ltemp_x1x3  = smpy_ss(ltemp_x1,ltemp_x3);
    ltemp_x2x3  = smpy_ss(ltemp_x2,ltemp_x3);

    //x3y3x1
    lltemp2 = lmpy_ss(ltemp_x1x3,ltemp_ys3);
    lltemp1 = lltemp2 >> (ltemp_xn1+ltemp_xn3);     //x3*y3*x1 * 2^62
    //x3y3x2
    lltemp2 = lmpy_ss(ltemp_x2x3,ltemp_ys3);
    lltemp1 -= lltemp2 >> (ltemp_xn2+ltemp_xn3);
    //x2x3y2
    lltemp2 = lmpy_ss(ltemp_x2x3,ltemp_ys2);
    lltemp1 += lltemp2 >> (ltemp_xn2+ltemp_xn3);
    //x2x1y2
    lltemp2 = lmpy_ss(ltemp_x1x2,ltemp_ys2);
    lltemp1 -= lltemp2 >> (ltemp_xn1+ltemp_xn2);
    //x2x1y1
    lltemp2 = lmpy_ss(ltemp_x1x2,ltemp_ys1);
    lltemp1 += lltemp2 >> (ltemp_xn1+ltemp_xn2);
    //x3x1y1
    lltemp2 = lmpy_ss(ltemp_x1x3,ltemp_ys1);
    lltemp1 -= lltemp2 >> (ltemp_xn1+ltemp_xn3); //temp * 2^62

    ldenom_n1 = unorm_sl(lltemp1);
    lltemp1   >>= (32-ldenom_n1);
    ldenom    = (S32)lltemp1;//temp * 2^(62-32+ln)
    ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);//result in 1/temp * 2^(61-(30+ldenom_n1))

    //// c1 = (x2*x3*(y2-y3)*y1-x1*x3*(y1-y3)*y2+x1*x2*(y1-y2)*y3)/temp;
    ltemp2  = smpy_ss(ltemp_x2x3,ltemp_y1);
    lltemp2 = lmpy_ss(ltemp2,ssub_sss(ltemp_ys2,ltemp_ys3));
    lltemp1 = lltemp2 >> (ltemp_xn2+ltemp_xn3+ltemp_yn1);

    ltemp2  = smpy_ss(ltemp_x1x3,ltemp_y2);
    lltemp2 = lmpy_ss(ltemp2,ssub_sss(ltemp_ys1,ltemp_ys3));
    lltemp1 -= lltemp2 >> (ltemp_xn1+ltemp_xn3+ltemp_yn2);

    ltemp2  = smpy_ss(ltemp_x1x2,ltemp_y3);
    lltemp2 = lmpy_ss(ltemp2,ssub_sss(ltemp_ys1,ltemp_ys2));
    lltemp1 += lltemp2 >> (ltemp_xn1+ltemp_xn2+ltemp_yn3); //(x2*x3*(y2-y3)*y1-x1*x3*(y1-y3)*y2+x1*x2*(y1-y2)*y3) * 2^62
    ltemp_n = unorm_sl(lltemp1);
    lltemp1 <<=ltemp_n;
    ltemp2   = sshr_slu(lltemp1,32);
    lltemp2  = lmpy_ss(ldenom,ltemp2);//(x2*x3*(y2-y3)*y1-x1*x3*(y1-y3)*y2+x1*x2*(y1-y2)*y3)  * 1/temp * 2^(31-ldenom_n1)* 2^(30+ltemp_n)
    ltemp2   = unorm_sl(lltemp2);
    lltemp2  <<=ltemp2;
    lltemp2  >>=32;
    cParam1->val  = (S32)lltemp2;//(61-ldenom_n1+ltemp_n)
    cParam1->norm = (U16)(61-ldenom_n1+ltemp_n+ltemp2-32);


    //// c2(-(x2*y2-x3*y3)*y1+(x1*y1-x3*y3)*y2-(x1*y1-x2*y2)*y3)/temp;
    //x3y3y1
    ltemp2 = smpy_ss(ltemp_x3,ltemp_y3);
    lltemp1 = lmpy_ss(ltemp2,ltemp_ys1);
    lltemp1 >>= (ltemp_xn3+ltemp_yn3);

    //x2y2y1
    ltemp2 = smpy_ss(ltemp_x2,ltemp_y2);
    lltemp2 = lmpy_ss(ltemp2,ltemp_ys1);
    lltemp2 >>= (ltemp_xn2+ltemp_yn2);
    lltemp1 -= lltemp2;

    //x1y1y2
    ltemp2 = smpy_ss(ltemp_x1,ltemp_y1);
    lltemp2 = lmpy_ss(ltemp2,ltemp_ys2);
    lltemp2 >>= (ltemp_xn1+ltemp_yn1);
    lltemp1 +=  lltemp2;

    //x3y3y2
    ltemp2 = smpy_ss(ltemp_x3,ltemp_y3);
    lltemp2 = lmpy_ss(ltemp2,ltemp_ys2);
    lltemp2 >>= (ltemp_xn3+ltemp_yn3);
    lltemp1 -=  lltemp2;

    //x1y1y3
    ltemp2 = smpy_ss(ltemp_x1,ltemp_y1);
    lltemp2 = lmpy_ss(ltemp2,ltemp_ys3);
    lltemp2 >>= (ltemp_xn1+ltemp_yn1);
    lltemp1 -=  lltemp2;

    //x2y2y3
    ltemp2 = smpy_ss(ltemp_x2,ltemp_y2);
    lltemp2 = lmpy_ss(ltemp2,ltemp_ys3);
    lltemp2 >>= (ltemp_xn2+ltemp_yn2);
    lltemp1 +=  lltemp2;


    ltemp_n = unorm_sl(lltemp1);
    lltemp1 <<=ltemp_n;
    ltemp2   = sshr_slu(lltemp1,32);
    lltemp2  = lmpy_ss(ldenom,ltemp2);//(-(x2*y2-x3*y3)*y1+(x1*y1-x3*y3)*y2-(x1*y1-x2*y2)*y3)  * 1/temp * 2^(31-ldenom_n1)* 2^(30+ltemp_n)

    ltemp2   = unorm_sl(lltemp2);
    lltemp2  <<=ltemp2;
    lltemp2  >>=32;
    cParam2->val  = (S32)lltemp2;//(61-ldenom_n1+ltemp_n)
    cParam2->norm = (U16)(61-ldenom_n1+ltemp_n+ltemp2-32);


    //// c3 = ((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3)/temp;
    lltemp_x3y1 = lmpy_ss(ltemp_xs3-ltemp_xs2,ltemp_ys1);
    lltemp_x3y2 = lmpy_ss(ltemp_xs3-ltemp_xs1,ltemp_ys2);
    lltemp_x2y3 = lmpy_ss(ltemp_xs2-ltemp_xs1,ltemp_ys3);

    lltemp2  = lltemp_x3y1-lltemp_x3y2;
    lltemp2 += lltemp_x2y3;
    //((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3) * 2^(31+31)
    ltemp_n  = unorm_sl(lltemp2);
    ltemp2   = sshr_slu(lltemp2,(32-ltemp_n));//((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3) * 2^(62+ltemp2-32)
    lltemp2  = lmpy_ss(ldenom,ltemp2);

    ltemp2   = unorm_sl(lltemp2);
    lltemp2  <<=ltemp2;
    lltemp2  >>=32;
    cParam3->val  = (S32)lltemp2;//(61-ldenom_n1+ltemp_n)
    cParam3->norm = (U16)(61-ldenom_n1+ltemp_n+ltemp2-32);


    *slopeOverRollOff = ltempS_R;

}

void  GenerateToneCurveLut(S32 Crush,S32 Clip ,
                           val_norm_pair_t *cParam1,
                           val_norm_pair_t *cParam2,
                           val_norm_pair_t *cParam3,
                           S32 slopeOverRollOff,S32 Rolloff,
                           S32 sTrimSlope ,S32 sTrimOffset , S32 sTrimPower,
                           U16 *toneCurve515LUT , U16 *toneCurve4KLUT)

{


    int iloop;
    S32 ltemp1,lratio;
    S64 llratio,llsum1,llsum2;
    const S32 *plogTable = toneCurveXlogTable;
    U16 *toneCurveLUT   ;
    U32 TrimSlope ,  TrimPower;
    U16 uLutMaxValue = 0;
    U16 uOut;
    U16 uCommonNorm1,uCommonNorm2;


    TrimSlope = (sTrimSlope + (1<<QTRIMS));
    TrimPower = (sTrimPower + (1<<QTRIMS));

    toneCurveLUT = toneCurve515LUT;

    uCommonNorm1  = sminss(cParam1->norm,cParam2->norm);
    uCommonNorm2 = sminss(31,cParam3->norm);

    for(iloop =0 ; iloop<4096; iloop+=8)
    {
        ltemp1 = smpy_ss(slopeOverRollOff,plogTable[iloop]);// logTable is created assuming SoR is Q29
        ltemp1 = fxPOW2Taylor(ltemp1,QPOW2IN+(QSLOPEROLLOFF-29),31);


        llsum1   = lmpy_ss(cParam2->val,ltemp1);
        llsum1 >>= cParam2->norm-uCommonNorm1;
        llsum1  += lmpy_ss(cParam1->val,0x7FFFFFFF>>(cParam1->norm-uCommonNorm1));//2^(31+uCommonNorm)

        llsum2   = lmpy_ss(cParam3->val,ltemp1);
        llsum2 >>= cParam3->norm-uCommonNorm2;
        llsum2  += (S64)1 <<(31+uCommonNorm2);
        llsum2  >>=uCommonNorm2;

        llsum2  = llsum1/(llsum2+1);
        ltemp1  = unorm_sl(llsum2);
        llsum2  <<=ltemp1;
        llsum2  >>=32;
        llsum2  = smaxss(0,llsum2);
        lratio = (S32)llsum2;
        ltemp1 += uCommonNorm1-32;

        if(lratio != 0)
        {
            lratio = fxLOG2Taylor(lratio,QLOG2OUT);
            lratio = ssub_ss(lratio,(ltemp1) << QLOG2OUT);
            llratio = lmpy_ss(lratio,Rolloff);
            lratio = sshr_slu(llratio,31+QLOG2OUT-QPOW2IN);
            lratio = fxPOW2Taylor(lratio,QPOW2IN,29);
        }


        /// Trim Pass
        lratio = smpy_ss(lratio,TrimSlope<<(31-29));         //result in QTRIM , previous output in Q29
        lratio = sadd_sss(lratio,sTrimOffset);       //result in QTRIM
        lratio = smaxss(0,lratio);
        if(lratio>0)
        {
            lratio = fxLOG2Taylor(lratio,QLOG2OUT);
            lratio = ssub_ss(lratio,QTRIMS << QLOG2OUT);
            llratio = lmpy_ss(lratio,TrimPower);
            lratio = sshr_slu(llratio,QTRIMS+QLOG2OUT-QPOW2IN);
            lratio = fxPOW2Taylor(lratio,QPOW2IN,31);
        }

        ///

        lratio = sminss(Clip,smaxss(Crush,lratio));
        lratio = sadd_sss(lratio , (unsigned)(0x1) << ((31-QTCLUT-1)));
        //lratio = (lratio & (signed)0x80000000>>QTCLUT);
        uOut = (U16) ((lratio) >> (31-QTCLUT));


        if(uOut<uLutMaxValue)
        {
            uOut = uLutMaxValue;
        }
        else
        {
            uLutMaxValue = uOut;
        }

        *toneCurveLUT++  =  uOut;
    }
}

void CreateToneCurve(S32 Crush,S32 Mid,S32 Clip,
                     S32 SminPQ,S32 SmaxPQ,S32 Sdiagonalinches,
                     S32 sTrimSlope ,S32 sTrimOffset , S32 sTrimPower,
                     displayPqTuning_t_    *pDisplayTuning,
                     val_norm_pair_t *cParam1,
                     val_norm_pair_t *cParam2,
                     val_norm_pair_t *cParam3,
                     S32 *slopeOverRollOff,
                     U16 *toneCurve515LUT , U16 *toneCurveLUT)
{

    S32 TMin,Shift,TMid, TMax,SMid,Slope;
//    S32 RolloffInv;
    S32 Rolloff;
    U16 Mode;

#define TRIMSMAX  (1<<(QTRIMS-1)) //0.5
#define TRIMSMIN -(1<<(QTRIMS-1)) //0.5

//    RolloffInv      = pDisplayTuning->tRolloffInv;
    Rolloff         = pDisplayTuning->tRolloff;
    Mode            = pDisplayTuning->tMode;



    /* Limit Trim Values */
    sTrimSlope  = sminss(TRIMSMAX,smaxss(sTrimSlope,TRIMSMIN));
    sTrimPower  = sminss(TRIMSMAX,smaxss(sTrimPower,TRIMSMIN));
    sTrimOffset = sminss(TRIMSMAX,smaxss(sTrimOffset,TRIMSMIN));



    /* Mode = 0 , TC
       Mode = 1 , Inverse TC
       Mode = 2 , Bypass
       Mode = 3 , SDR TC  */

    if(0 == Mode)       /* Tone Curve */
    {
        CalcMappedParams(Crush,Mid,Clip,
                         pDisplayTuning,
                         SminPQ,SmaxPQ,
                         Sdiagonalinches,
                         &TMin,&TMid,&TMax,&SMid,&Slope);

        CalcCurveParams(Crush, SMid,Clip , TMin,
                        TMid, TMax, Slope, pDisplayTuning,
                        cParam1,cParam2,cParam3,slopeOverRollOff);

        //printk("%d %d\n", cParam1->norm, cParam1->val);
        //printk("%d %d\n", cParam2->norm, cParam2->val);
        //printk("%d %d\n", cParam3->norm, cParam3->val);
        //printk("%d %d %d\n", *slopeOverRollOff, Rolloff, pDisplayTuning->tRolloffInv);
        //printk("%d %d %d\n", sTrimSlope,sTrimOffset,sTrimPower);
        GenerateToneCurveLut(0,0x7fffffff,cParam1,cParam2,cParam3,*slopeOverRollOff,Rolloff,
                             sTrimSlope,sTrimOffset,sTrimPower,toneCurve515LUT,toneCurveLUT);

    }
    else if(1==Mode)    /* Inverse Tone Curve */
    {
        CalcMappedParams(Crush,Mid,Clip,
                         pDisplayTuning,
                         SminPQ,SmaxPQ,
                         Sdiagonalinches,
                         &TMin,&TMid,&TMax,&SMid,&Slope);

        CalcCurveParams(Crush, SMid,Clip , TMin,
                        TMid, TMax, Slope, pDisplayTuning,
                        cParam1,cParam2,cParam3,slopeOverRollOff);

        //      GenerateInvToneCurveLut(*cParam1,*cParam2,*cParam3,*slopeOverRollOff,
        //       RolloffInv << QSLOPEROLLOFF,sTrimSlope,sTrimOffset,sTrimPower,toneCurve515LUT,toneCurveLUT);
    }
    else if(3==Mode)   /* SDR Tone Curve */
    {
        S32 TminPQ, TmaxPQ;
        TminPQ          = pDisplayTuning->tMinPQ;
        TmaxPQ          = pDisplayTuning->tMaxPQ;


        CalcIMappedParams(&Mid,Clip,TminPQ,TmaxPQ,SminPQ,SmaxPQ,
                          &Shift,&TMin,&TMax,&Slope);

        CalcCurveParams   (TminPQ, Mid,TmaxPQ , TMin,
                           Shift, TMax, Slope, pDisplayTuning,
                           cParam1,cParam2,cParam3,slopeOverRollOff);

        //    GenerateInvToneCurveLut(*cParam1,*cParam2,*cParam3,*slopeOverRollOff,
        //                                 RolloffInv << QSLOPEROLLOFF,0,0,0,toneCurve515LUT,toneCurveLUT);
    }
    else           /* bypass */
    {
        U32  iLoop;
        for(iLoop=0 ; iLoop<4096; iLoop++)
        {
            toneCurveLUT[iLoop] = (U16)iLoop;
        }
    }      /* end of if Mode */
}

////////////////////////////////////////////////////////////////////////

typedef struct DoVi_TmoInfo_t_
{
    U16 u16Mode;
    U16 u16Crush;
    U16 u16Mid;
    U16 u16Clip;
    S16 s16Slope;
    S16 s16Offset;
    S16 s16Power;
    U16 u16SminPq;
    U16 u16SmaxPq;
    U16 u16SdiagInches;
    U16 u16TminPq;
    U16 u16TmaxPq;
    U16 u16TdiagInches;
    S16 s16TminPqBias;
    S16 s16TmidPqBias;
    S16 s16TmaxPqBias;
    S16 s16Contrast;
    S16 s16Brightness;
    S32 s32Rolloff;
} DoVi_TmoInfo_t;

void DoVi_CreateToneCurve(const DoVi_TmoInfo_t* pTmoInfo, U16 *toneCurve512LUT)
{
#if 0
    printk("//%d //u16Mode;\n", pTmoInfo->u16Mode);
    printk("//%d //u16Crush;\n", pTmoInfo->u16Crush);
    printk("//%d //u16Mid;\n", pTmoInfo->u16Mid);
    printk("//%d //u16Clip;\n", pTmoInfo->u16Clip);
    printk("//%d //s16Slope;\n", pTmoInfo->s16Slope);
    printk("//%d //s16Offset;\n", pTmoInfo->s16Offset);
    printk("//%d //s16Power;\n", pTmoInfo->s16Power);
    printk("//%d //u16SminPq;\n", pTmoInfo->u16SminPq);
    printk("//%d //u16SmaxPq;\n", pTmoInfo->u16SmaxPq);
    printk("//%d //u16SdiagInches;\n", pTmoInfo->u16SdiagInches);
    printk("//%d //u16TminPq;\n", pTmoInfo->u16TminPq);
    printk("//%d //u16TmaxPq;\n", pTmoInfo->u16TmaxPq);
    printk("//%d //u16TdiagInches;\n", pTmoInfo->u16TdiagInches);
    printk("//%d //s16TminPqBias;\n", pTmoInfo->s16TminPqBias);
    printk("//%d //s16TmidPqBias;\n", pTmoInfo->s16TmidPqBias);
    printk("//%d //s16TmaxPqBias;\n", pTmoInfo->s16TmaxPqBias);
    printk("//%d //s16Contrast;\n", pTmoInfo->s16Contrast);
    printk("//%d //s16Brightness;\n", pTmoInfo->s16Brightness);
    printk("//%d //s32Rolloff;\n", pTmoInfo->s32Rolloff);
#endif

    // PQ value is defined as 12bits in MdsExt_t
    int32_t Crush  = (S32)(pTmoInfo->u16Crush) << (QB02DATAIN-12);
    int32_t Mid    = (S32)(pTmoInfo->u16Mid)   << (QB02DATAIN-12);
    int32_t Clip   = (S32)(pTmoInfo->u16Clip)  << (QB02DATAIN-12);
    int32_t SminPQ = (S32)(pTmoInfo->u16SminPq) << (QB02DATAIN-12);
    int32_t SmaxPQ = (S32)(pTmoInfo->u16SmaxPq) << (QB02DATAIN-12);
    int32_t trimSlope  = (S32)(pTmoInfo->s16Slope)  << (QTRIMS-12);
    int32_t trimOffset = (S32)(pTmoInfo->s16Offset) << (QTRIMS-12);
    int32_t trimPower  = (S32)(pTmoInfo->s16Power)  << (QTRIMS-12);
    int32_t Sdiagonalinches = (S32)(pTmoInfo->u16SdiagInches);

    displayPqTuning_t_ pqt;
    pqt.tMode = 0; // MM/HDMI SINK
    pqt.tMinPQ = (S32)(pTmoInfo->u16TminPq) << (QB02DATAIN-12);
    pqt.tMaxPQ = (S32)(pTmoInfo->u16TmaxPq) << (QB02DATAIN-12);
    pqt.tDiagonalInches = (S32)(pTmoInfo->u16TdiagInches);
    pqt.tRolloff = (S32)(pTmoInfo->s32Rolloff);
    pqt.tRolloffInv = ((uint32_t)1 << QB02DATAIN) / pqt.tRolloff;
    pqt.tMinPQBias = (S32)(pTmoInfo->s16TminPqBias) << (QB02DATAIN-12);
    pqt.tMidPQBias = (S32)(pTmoInfo->s16TmidPqBias) << (QB02DATAIN-12);
    pqt.tMaxPQBias = (S32)(pTmoInfo->s16TmaxPqBias) << (QB02DATAIN-12);
    pqt.tContrast = (S32)(pTmoInfo->s16Contrast) << (QB02DATAIN-12);
    pqt.tBrightness = (S32)(pTmoInfo->s16Brightness) << (QB02DATAIN-12);

    val_norm_pair_t cParam1, cParam2, cParam3;
    int32_t slopeOverRollOff;
    CreateToneCurve(Crush, Mid, Clip,
                    SminPQ, SmaxPQ, Sdiagonalinches,
                    trimSlope, trimOffset, trimPower,
                    &pqt,
                    &cParam1, &cParam2, &cParam3, &slopeOverRollOff,
                    toneCurve512LUT, NULL);
#if 0
    int i = 0;
    for (i = 0; i < 30; i++)
        printk(KERN_CRIT "//TC[%d] = %x\n", i,toneCurve512LUT[i]);
#endif

}