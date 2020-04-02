/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/

#ifdef WIN32
#pragma optimize( "", off )
#endif

#include "fx_math.h"
#include "DolbyDisplayManagement.h"
#include "generateLutlogXTable.h"



void CalcMappedParams3pxx(  int32_t SMax,
                            srcMetadata_t_     *pSrcMetadata,
                            displayPqTuning_t_ *pDisplayTuning,
                            int32_t *pTMin,int32_t *pTMid,int32_t *pTMax,int32_t *pSMid,
                            int32_t *pSlope,int32_t *ps2t_ratio, int32_t *pscreen_ratio)
    {


        int32_t ltemp1,ltemp2,ldenom,ldenom_n1,key;
        int64_t lltemp1;
        int32_t Shift;
        int32_t limitUp,limitDown;
        int32_t sDisplayMidPQ,tDisplayMidPQ;
        int32_t TminPQ,TmaxPQ,Tdiagonalinches;
        int32_t SminPQ,SmaxPQ,Sdiagonalinches;
        int32_t Crush, Mid, Clip,KeyWeight;
        int32_t SignalSimpleMid , NewS2Tratio ,s2t_ratio;

        Crush       = pSrcMetadata->Crush;
        Mid         = pSrcMetadata->Mid;
        Clip        = SMax;//pSrcMetadata->Clip;
        KeyWeight   = pSrcMetadata->KeyWeight;
      /* Swap source and target for inverse DM */
       if(DM_NORMAL_TC == pDisplayTuning->tMode)
       {
           SminPQ           = pSrcMetadata->SminPQ;
           SmaxPQ           = pSrcMetadata->SmaxPQ;
           Sdiagonalinches  = pSrcMetadata->Sdiagonalinches;
           TminPQ           = pDisplayTuning->tMinPQ;
           TmaxPQ           = pDisplayTuning->tMaxPQ;
           Tdiagonalinches  = pDisplayTuning->tDiagonalInches;
       }
       else if(DM_INVERSE_TC == pDisplayTuning->tMode)
       {
           TminPQ           = pSrcMetadata->SminPQ;
           TmaxPQ           = pSrcMetadata->SmaxPQ;
           SminPQ           = pDisplayTuning->tMinPQ;
           SmaxPQ           = pDisplayTuning->tMaxPQ;
           Tdiagonalinches  = pSrcMetadata->Sdiagonalinches;
           Sdiagonalinches  = pDisplayTuning->tDiagonalInches;
       }
       else
       { /* Use default */
           SminPQ           = pSrcMetadata->SminPQ;
           SmaxPQ           = pSrcMetadata->SmaxPQ;
           Sdiagonalinches  = pSrcMetadata->Sdiagonalinches;
           TminPQ           = pDisplayTuning->tMinPQ;
           TmaxPQ           = pDisplayTuning->tMaxPQ;
           Tdiagonalinches  = pDisplayTuning->tDiagonalInches;
       }

    //SourceDisplayMid = (SmaxPQ+SminPQ)/2;
    sDisplayMidPQ         = (SmaxPQ>>1) + (SminPQ>>1);

    //TargetDisplayMid = (TmaxPQ+TminPQ)/2;
    tDisplayMidPQ         = (TmaxPQ>>1) + (TminPQ>>1);

    //S2Tratio = SourceDisplayMid - TargetDisplayMid;
    s2t_ratio   = sDisplayMidPQ - tDisplayMidPQ;

    if(pDisplayTuning->tBrightPreserve !=0)
    {
        SignalSimpleMid = (Clip>>1) + (Crush>>1);

        if(SignalSimpleMid < tDisplayMidPQ){
            NewS2Tratio = 0;
        }
        else if(SignalSimpleMid > sDisplayMidPQ){
            NewS2Tratio = s2t_ratio;
        }
        else if (s2t_ratio > 0){
            #define Q_EASE 20
            int32_t t;
            int32_t norm , ease_offset;
            int64_t i64;

            norm = unorm_s(s2t_ratio);
            t = sshl_ssu(s2t_ratio ,  norm);

            t = (int32_t)(((int64_t)(SignalSimpleMid - tDisplayMidPQ)<<(30))/t);//Q = z = 30-norm

            ease_offset = 0;
            norm = 30-norm;
            if( t >= (1<<(norm-1))){
                //1+
                ease_offset = 1<<Q_EASE;
                //t = t-1
                t = t - (1<<(norm));
            }

            i64 = ( (int64_t)t *  t) >>31;//t^2 2z-31
            i64 = (i64 * i64) >>31;//t^4 4z-62-31
            i64 = (i64 *  t) >> (5*norm-124+31-4-Q_EASE);//t^5 // 16 *5z-93-31
            i64 += ease_offset;

            i64= ((s2t_ratio * i64) >> Q_EASE);
            i64 = smaxss(-2147483648LL,sminss(0x7FFFFFFF,i64));

            NewS2Tratio = (int32_t)i64;
        }
        else {
            NewS2Tratio = s2t_ratio;
        }

        lltemp1  =  (int64_t)NewS2Tratio * pDisplayTuning->tBrightPreserve + (int64_t)s2t_ratio * (4096-pDisplayTuning->tBrightPreserve);
        s2t_ratio = (int32_t)(lltemp1 >> 12);
    }




    *ps2t_ratio  = s2t_ratio;

    // ScreenRatio = sqrt(log2(Tdiagonalinches) / log2(Sdiagonalinches));
    if (Tdiagonalinches==Sdiagonalinches) {
        *pscreen_ratio =  1<<QSCREEN_RATIO;
    }
    else {
        ltemp1      = fxLOG2Taylor(Tdiagonalinches,QLOG2OUT);
        ltemp2      = fxLOG2Taylor(Sdiagonalinches,QLOG2OUT);//log2(Sdiagonalinches) * 2^QLOG2OUT
        ldenom_n1   = unorm_s(ltemp2);
        ldenom      = sshl_su(ltemp2,ldenom_n1);//log2(Sdiagonalinches) * 2^(QLOG2OUT+ldenom_n1)
        ldenom      = sdiv_sls((int64_t)0x1<<(30+31),ldenom);//%(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
        lltemp1     = lmpy_ss(ldenom,ltemp1);
        ldenom      = sshr_slu(lltemp1,31+30-ldenom_n1-QSCREEN_RATIO);

        // input to sqrt root is in Q21(ratioQ)
        ldenom       = ssqrt_s(ldenom);
        //sqrt accepts input in Q31 ..so compensate for the difference
        *pscreen_ratio = sshr_sss(ldenom,((31-QSCREEN_RATIO)/2)); // first term to get output in ratioQ ,
                                                                     // second term to go compensate the input to sqrt
    }
    //Slope = (1+S2Tratio) * ScreenRatio * (1 + DMParams.DContrast);

    ltemp1      = smpy_ss(*pscreen_ratio,*ps2t_ratio);
    ltemp1      = sadd_sss(*pscreen_ratio,ltemp1);
    ltemp1      = sshl_ssu(ltemp1,QSLOPEIN-QSCREEN_RATIO);

    ltemp2      = smpy_ss(ltemp1,pDisplayTuning->tContrast);
    *pSlope      = sadd_sss(ltemp1,ltemp2);

    //Key = DMParams.KeyWeight * (SMid - SourceDisplayMid) + 1;
    //Key = Key/2; key in Q30
    ltemp1 = Mid - sDisplayMidPQ;
    lltemp1 = lmpy_ss(ltemp1 , KeyWeight);
    ltemp1  = sshr_slu(lltemp1,QLVL3MD+1);
    key = sadd_sss(ltemp1,1<<(QB02DATAIN-1));


    //Shift = S2Tratio * Key/2 * ScreenRatio + DMParams.DBrightness;
    lltemp1 = lmpy_ss(*ps2t_ratio,key); // S2Tratio/2 * Key * 2^62
    ltemp1  = unorm_sl(lltemp1);
    lltemp1   <<= ltemp1;//S2Tratio/2 * Key * 2^(62+n-32)
    lltemp1   >>= (32);
    ltemp2  = (int32_t)lltemp1;
    lltemp1 = lmpy_ss(ltemp2,*pscreen_ratio);// 2^(30+n+QSCREEN_RATIO)
    ltemp1 = sshr_slu(lltemp1,ltemp1+30+QSCREEN_RATIO-31);
    Shift = sadd_sss(ltemp1,pDisplayTuning->tBrightness);

    //TMin = max(SMin-Shift, TminPQ) + DMParams.TMinBias;
    //TMax = min(SMax-Shift, TmaxPQ) + DMParams.TMaxBias;
    *pTMin = sadd_sss(smaxss(ssub_sss(Crush,Shift), TminPQ ),pDisplayTuning->tMinPQBias);
    *pTMax = sadd_sss(sminss(ssub_sss(Clip,Shift), TmaxPQ),pDisplayTuning->tMaxPQBias);

    //LUP = (SMax-SMin)*0.85+SMin;
    limitUp = smpy_ss(Clip-Crush,SCF(0.85));
    limitUp = sadd_sss(limitUp,Crush);

    //LDOWN = (SMax-SMin)*0.15+SMin;
    limitDown = smpy_ss(Clip-Crush,SCF(0.15));
    limitDown = sadd_sss(limitDown,Crush);

    //SMid = max(LDOWN, min(LUP, SMid));
    *pSMid = smaxss(limitDown,sminss(limitUp,Mid));

    ///LUP = (TMax-TMin)*0.85+TMin;
    limitUp = smpy_ss(*pTMax-*pTMin,SCF(0.85));
    limitUp = sadd_sss(limitUp,*pTMin);
    //LDOWN = (TMax-TMin)*0.15+TMin;
    limitDown = smpy_ss(*pTMax-*pTMin,SCF(0.15));
    limitDown = sadd_sss(limitDown,*pTMin);
    //TMid = max(LDOWN, min(LUP, SMid - Shift + DMParams.TMidBias));
    //TMid = SMid - Shift + DMParams.TMidPQBias;
    ltemp1 = *pSMid - Shift;
    ltemp1 = sadd_sss(ltemp1,pDisplayTuning->tMidPQBias);
    //TMid = max(LDOWN, min(LUP, TMid));
    *pTMid = smaxss(limitDown, sminss(limitUp, ltemp1));


    }

void CalcMappedParams2pxx(int32_t Crush,int32_t Mid,int32_t Clip,
                         displayPqTuning_t_    *pDisplayTuning,
                         int32_t SminPQ,int32_t SmaxPQ,
                         int32_t Sdiagonalinches,int32_t *TMin,int32_t *TMid,int32_t *TMax ,int32_t *SMid, int32_t *Slope){


    int32_t ltemp1,ltemp2,ldenom,ldenom_n1,key,ldenom_n2;
    int64_t lltemp1;
    int32_t s2t_ratio,screen_ratio,ShiftFactor,Shift;
    int32_t limitUp,limitDown;


    /* Swap source and target for inverse DM */
    if(DM_INVERSE_TC == pDisplayTuning->tMode)
    {
        int32_t tMinPq, tMaxPq, tDiagonalInches;
        tMinPq = pDisplayTuning->tMinPQ;
        tMaxPq = pDisplayTuning->tMaxPQ;
        tDiagonalInches = pDisplayTuning->tDiagonalInches;
        pDisplayTuning->tMinPQ = SminPQ;
        pDisplayTuning->tMaxPQ = SmaxPQ;
        SminPQ = tMinPq;
        SmaxPQ = tMaxPq;
        pDisplayTuning->tDiagonalInches  = Sdiagonalinches;
        Sdiagonalinches  = tDiagonalInches;
    }

    // DMParams.S2Tratio = min(1, sqrt((TmaxPQ-TminPQ)/(SmaxPQ-SminPQ)));
    lltemp1      = (int64_t)pDisplayTuning->tMaxPQ + (int64_t)pDisplayTuning->tMinPQ+1;
    ltemp1       = (int32_t)(lltemp1>>1);
    lltemp1      = (int64_t)SmaxPQ + (int64_t)SminPQ+1;
    ltemp2       = (int32_t)(lltemp1>>1);

    ldenom_n1   = unorm_s(ltemp2);
    ldenom      = sshl_su(ltemp2,ldenom_n1);//(diff) * 2^(ldenom_n1)
    ldenom      = sdiv_sls((int64_t)0x1<<(30+31),ldenom);//(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
    lltemp1     = lmpy_ss(ldenom,ltemp1);
    ltemp1      = sshr_slu(lltemp1,30-ldenom_n1);
    s2t_ratio   = ssqrt_s(ltemp1);

    // ScreenRatio = sqrt(log2(Tdiagonalinches) / log2(Sdiagonalinches));
    if(pDisplayTuning->tDiagonalInches==Sdiagonalinches){
        screen_ratio =  1<<QSCREEN_RATIO;
    }
    else{
        ltemp1      = fxLOG2Taylor(pDisplayTuning->tDiagonalInches,QLOG2OUT);
        ltemp2      = fxLOG2Taylor(Sdiagonalinches,QLOG2OUT);//log2(Sdiagonalinches) * 2^QLOG2OUT
        ldenom_n1   = unorm_s(ltemp2);
        ldenom      = sshl_su(ltemp2,ldenom_n1);//log2(Sdiagonalinches) * 2^(QLOG2OUT+ldenom_n1)
        ldenom      = sdiv_sls((int64_t)0x1<<(30+31),ldenom);//%(1/log2(Sdiagonalinches)) = double(ldenom)* 2^(double(ldenom_n1+26)-31-30)
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
    ldenom      = sdiv_sls((int64_t)0x1<<(30+31),ldenom);
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
    ldenom      = sdiv_sls((int64_t)1<<(30+31),ldenom);//(1/double(Clip-Crush)) = double(ldenom)* 2^(double(ldenom_n1)-31-30)

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

    //LUP = (SMax-SMin)*0.85+SMin;
    limitUp = smpy_ss(Clip-Crush,SCF(0.85));
    limitUp = sadd_sss(limitUp,Crush);

    //LDOWN = (SMax-SMin)*0.15+SMin;
    limitDown = smpy_ss(Clip-Crush,SCF(0.15));
    limitDown = sadd_sss(limitDown,Crush);

    //SMid = max(LDOWN, min(LUP, SMid));
    *SMid = smaxss(limitDown,sminss(limitUp,*SMid));

    ///LUP = (TMax-TMin)*0.85+TMin;
    limitUp = smpy_ss(*TMax-*TMin,SCF(0.85));
    limitUp = sadd_sss(limitUp,*TMin);

    //LDOWN = (TMax-TMin)*0.15+TMin;
    limitDown = smpy_ss(*TMax-*TMin,SCF(0.15));
    limitDown = sadd_sss(limitDown,*TMin);

    //TMid = SMid - Shift + DMParams.TMidPQBias;
    ltemp1 = *SMid - Shift;
    *TMid = sadd_sss(ltemp1,pDisplayTuning->tMidPQBias);

    //TMid = max(LDOWN, min(LUP, TMid));
    *TMid = smaxss(limitDown, sminss(limitUp, *TMid));

    return ;
}


void CalcCurveParams    (int32_t SMin,int32_t SMid,int32_t SMax ,int32_t TMin,int32_t TMid,int32_t TMax,int32_t Slope,
                         displayPqTuning_t_     *pDisplayTuning,
                         val_norm_pair_t        *pParam1,
                         val_norm_pair_t        *pParam2,
                         val_norm_pair_t        *pParam3,
                         int32_t                *pSoR)
{

    int32_t ltemp2,ltemp_x1,ltemp_xn1,ltemp_x2,ltemp_xn2,ltemp_x3,ltemp_xn3,ltempS_R , ltemp_n;
    int32_t ltemp_y1, ltemp_yn1,ltemp_y2,ltemp_yn2,ltemp_y3,ltemp_yn3;
    int32_t ltemp_xs1,ltemp_xs2,ltemp_xs3;
    int32_t ltemp_ys1,ltemp_ys2,ltemp_ys3;
    int32_t ldenom,ldenom_n1;
    int64_t lltemp2;
    int64_t lltemp_x3y1  , lltemp_x3y2   , lltemp_x2y3;
    int32_t iSign;

    int32_t ltemp_x1x2,ltemp_x1x3,ltemp_x2x3;
    int64_t lltemp1;

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
        lltemp1   <<= ldenom_n1;
        lltemp1   >>= (32);
        ldenom    = (int32_t)lltemp1;//temp * 2^(62-32+ln)
        ldenom      = sdiv_sls((int64_t)0x1<<(30+31),ldenom);//result in 1/temp * 2^(61-(30+ldenom_n1))

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
        pParam1->val  = (int32_t)lltemp2;//(61-ldenom_n1+ltemp_n)
        pParam1->norm = (uint16_t)(61-ldenom_n1+ltemp_n+ltemp2-32);


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
        pParam2->val  = (int32_t)lltemp2;//(61-ldenom_n1+ltemp_n)
        pParam2->norm = (uint16_t)(61-ldenom_n1+ltemp_n+ltemp2-32);


        //// c3 = ((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3)/temp;
        lltemp_x3y1 = lmpy_ss(ltemp_xs3-ltemp_xs2,ltemp_ys1);
        lltemp_x3y2 = lmpy_ss(ltemp_xs3-ltemp_xs1,ltemp_ys2);
        lltemp_x2y3 = lmpy_ss(ltemp_xs2-ltemp_xs1,ltemp_ys3);

        lltemp2  = lltemp_x3y1-lltemp_x3y2;
        lltemp2 += lltemp_x2y3;
                    //((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3) * 2^(31+31)
        ltemp_n  = unorm_sl(lltemp2);
        lltemp2  <<=ltemp_n;
        ltemp2   = (lltemp2>>32);
        //ltemp2   = sshr_slu(lltemp2,(32-ltemp_n));//((x3-x2)*y1-(x3-x1)*y2+(x2-x1)*y3) * 2^(62+ltemp2-32)
        lltemp2  = lmpy_ss(ldenom,ltemp2);

        ltemp2   = unorm_sl(lltemp2);
        lltemp2  <<=ltemp2;
        lltemp2  >>=32;
        pParam3->val  = (int32_t)lltemp2;//(61-ldenom_n1+ltemp_n)
        pParam3->norm = (uint16_t)(61-ldenom_n1+ltemp_n+ltemp2-32);

        if(pParam1->norm>31)
        {
            pParam1->val >>= sminss(31,pParam1->norm-31);
            pParam1->norm = 31;
        }
        if(pParam2->norm>31)
        {
            pParam2->val >>= sminss(31,pParam2->norm-31);
            pParam2->norm = 31;
        }
        if(pParam3->norm>31)
        {
            pParam3->val >>= sminss(31,pParam3->norm-31);
            pParam3->norm = 31;
        }


        *pSoR = ltempS_R;

}

/* Properly handled integer part of fxPOW2Taylor() */
int64_t fxPOW2TaylorInt(int32_t sInput, int32_t sScaleIn, int32_t sScaleOut)
{
    int64_t lltemp11;
    int32_t intCount = 0;

    if (sInput > (1<<QLOG2OUT)) { // > 1.0
        do {
            sInput -= (1<<QLOG2OUT);
            intCount++;
        } while (sInput > (1<<QLOG2OUT));

        lltemp11 = fxPOW2Taylor(sInput, sScaleIn, sScaleOut) * (int64_t)(1<<intCount);
    }
    else
        lltemp11 = fxPOW2Taylor(sInput, sScaleIn, sScaleOut);

    return lltemp11;
}


// NOTE: this function assumes rolloffInv=3.0
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
                           )


{


    int32_t iloop, istep;
    int32_t ltemp1,lratio , lnorm;
    int64_t llratio,llsum1,llsum2 , lltemp1,lltemp2;
    const int32_t *plogTable = toneCurveXlogTable;
    int16_t *toneCurveLUT   ;
    uint32_t TrimSlope ,  TrimPower;
    uint16_t uLutMaxValue = 0;
    uint16_t uOut;
    uint16_t uCommonNorm1,uCommonNorm2;

    istep = 4096 / lutSize;

    TrimSlope = (sTrimSlope + (1<<QTRIMS));
    TrimPower = (sTrimPower + (1<<QTRIMS));

    if(toneCurve4KLUT == NULL)
    {
        // Output 512 points without interpolation
        toneCurveLUT = toneCurve515LUT;
    }
    else
    {
         // Output 4k points after interpolation
        toneCurveLUT = toneCurve515LUT+1;
    }



     xMin  = xMin>>(QB02DATAIN-12-1); // To convert 31 bits to 12 bits
     xMin  +=1;
     xMin  >>=1;
     xMax   = xMax>>(QB02DATAIN-12-1);
     xMax  +=1;
     xMax  >>=1;

    if(DM_NORMAL_TC == pDisplayTuning->tMode)
    {
        uCommonNorm1  = sminss(pParam1->norm,pParam2->norm);
        uCommonNorm2 = sminss(31,pParam3->norm);

        for(iloop =0 ; iloop<4096; iloop+=istep)
        {
            int32_t loopId = iloop;
           // if(loopId < xMin)
           //     loopId = xMin;
           // if(loopId > xMax)
           //     loopId = xMax;

            ltemp1 = smpy_ss(SoR,plogTable[loopId]);// logTable is created assuming SoR is Q29
            ltemp1 = fxPOW2Taylor(ltemp1,QPOW2IN+(QSLOPEROLLOFF-29),31);


            llsum1   = lmpy_ss(pParam2->val,ltemp1);
            llsum1 >>= pParam2->norm-uCommonNorm1;
            llsum1  += lmpy_ss(pParam1->val,sshr_ssu(0x7FFFFFFF,(pParam1->norm-uCommonNorm1)));//2^(31+uCommonNorm)

            llsum2   = lmpy_ss(pParam3->val,ltemp1);
            llsum2 >>= pParam3->norm-uCommonNorm2;
            llsum2  += (int64_t)1 <<(31+uCommonNorm2);
            llsum2  >>=uCommonNorm2;

            llsum2  = llsum1/(llsum2+1);
            ltemp1  = unorm_sl(llsum2);
            llsum2  <<=ltemp1;
            llsum2  >>=32;
            llsum2  = smaxss(0,llsum2);
            lratio = (int32_t)llsum2;
            ltemp1 += uCommonNorm1-32;


            //max(0,((c1Ref+c2Ref*tcLutRef)./(1+c3Ref*tcLutRef))) = double(lratio).*2^-(ltemp1))

            if(lratio != 0){
            lratio = fxLOG2Taylor(lratio,QLOG2OUT);
            lratio = ssub_ss(lratio,(ltemp1) << QLOG2OUT);
            llratio = lmpy_ss(lratio,pDisplayTuning->tRolloff);
            lratio = sshr_slu(llratio,31+QLOG2OUT-QPOW2IN);
            lratio = fxPOW2Taylor(lratio,QPOW2IN,29);
            }


            /// Trim Pass
            lratio = smpy_ss(lratio,TrimSlope<<(31-29));         //result in QTRIM , previous output in Q29
            lratio = sadd_sss(lratio,sTrimOffset);       //result in QTRIM
            lratio = smaxss(0,lratio);
            if(lratio>0)     {
            lratio = fxLOG2Taylor(lratio,QLOG2OUT);
            lratio = ssub_ss(lratio,QTRIMS << QLOG2OUT);
            llratio = lmpy_ss(lratio,TrimPower);
            lratio = sshr_slu(llratio,QTRIMS+QLOG2OUT-QPOW2IN);
            lratio = fxPOW2Taylor(lratio,QPOW2IN,31);
            }

            ///

            lratio = sminss(0x7fffffff,smaxss(0,lratio));
            lratio = sadd_sss(lratio , (unsigned)(0x1) << ((31-QTCLUT-1)));
            uOut = (uint16_t) ((lratio) >> (31-QTCLUT));

            if (clampOut)
              uOut = sminss(pDisplayTuning->tMaxPQ >> (QB02DATAIN-12), smaxss(pDisplayTuning->tMinPQ >> (QB02DATAIN-12), uOut));

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
    else if(DM_INVERSE_TC == pDisplayTuning->tMode)
    {
        uCommonNorm1  = sminss(pParam2->norm,pParam3->norm);
        SoR = sdiv_sls((int64_t)1<<(QSLOPEROLLOFF+31),SoR);
        TrimPower        = sdiv_sls((int64_t)1<<(QTRIMS+QTRIMS),TrimPower);//Inverse is in QTRIMS too
        TrimSlope        = sdiv_sls((int64_t)1<<(QTRIMS+QTRIMS),TrimSlope);//Inverse is in QTRIMS too

        for(iloop = 0 ; iloop<4096; iloop+=istep)
        {
            int32_t flag;
            int64_t lltemp11;
            int32_t loopId = iloop;
            //if(loopId < xMin)
            //    loopId = xMin;
            //if(loopId > xMax)
            //    loopId = xMax;

            flag = 0;

            ltemp1 = smpy_ss(TrimPower,plogTable[loopId]);
            ltemp1 = fxPOW2Taylor(ltemp1,QLOG2OUT+QTRIMS-31,QTRIMS);        //(power(max(0,I),1/(TrimPower+1)))* 2^31

            ltemp1 = ssub_sss(ltemp1,sTrimOffset);     //(power(max(0,I),1/(TrimPower+1)) - TrimOffset) * 2^31
            lltemp1 = lmpy_ss(ltemp1,TrimSlope);            // resut is in 31+QTRIMS
            ltemp1 = sshr_slu(lltemp1,QTRIMS+QTRIMS-13);
            ltemp1 += 1;
            ltemp1 >>= 1;
            if (ltemp1 >= 4095) {
                flag = 1;
                ltemp1 >>= 1;
            }
            ltemp1 = smaxss(0,sminss(ltemp1,4095));

            if(ltemp1<3)
            {
                lltemp11 = 0;
            }
            else
            {
                ltemp1 = plogTable[ltemp1];//log2(n) * 2^26
                if (flag == 1) // 1.0 ~ 2.0
                    ltemp1 += (1<<QLOG2OUT);

                // FIXME: 1/rolloff = 3
                ltemp1 = (int64_t)3 * ltemp1;//log2(n) * 2^26 * 3

                lltemp11 = fxPOW2TaylorInt(ltemp1,QLOG2OUT,29); //power(Y, 1/Rolloff); * 2^29
            }

#define QTEMP001 58
            //Y = power(max(0,(Y-c1)./(c2-c3*Y)), 1/SoR);
            lltemp1   = (int64_t)pParam1->val;
            lltemp1 <<= (QTEMP001-pParam1->norm);
            lltemp2   = lltemp11<<(QTEMP001-29);
            lltemp1 = lltemp2 - lltemp1; // Numerator in QTEMP001

            llsum1 = pParam3->val * lltemp11;
            llsum1 >>= (pParam3->norm - uCommonNorm1); // uCommonNorm1+29
            llsum2 = (int64_t)pParam2->val;
            llsum2 <<=29;
            llsum2 >>= (pParam2->norm - uCommonNorm1);// uCommonNorm1+29

            llsum2 = llsum2 - llsum1;
            lnorm = unorm_sl(llsum2);
            llsum2 <<= lnorm;
            llsum2 >>= (32);
            ltemp1 = (int32_t)llsum2;


            ltemp1 = sdiv_sls(lltemp1,ltemp1+1);// power(max(0,(Y-c1)./(c2-c3*Y)) * 2^(62 - uCommonNorm1 - lnorm);
            ltemp1 = sshr_sss(ltemp1, (QTEMP001 - uCommonNorm1 - lnorm + 1) - 29);// power(max(0,(Y-c1)./(c2-c3*Y)) * 2^(29);
            lratio = smaxss(0,ltemp1);


            if(lratio != 0){
                lratio = fxLOG2Taylor(lratio,QLOG2OUT);
                lratio = ssub_ss(lratio,31 << QLOG2OUT);
                lltemp1 = lmpy_ss(lratio,SoR);
                lratio = sshr_slu(lltemp1,31+QLOG2OUT-QPOW2IN);
                lratio = fxPOW2Taylor(lratio,QPOW2IN,31);

                lratio = sminss(0x7fffffff,smaxss(0,lratio));
                lratio = sadd_sss(lratio , (unsigned)(0x1) << ((31-QTCLUT-1)));
                uOut = (uint16_t) ((lratio) >> (31-QTCLUT));
            }
            else
                uOut = 0;

            if (clampOut)
              uOut = sminss(pDisplayTuning->tMaxPQ >> (QB02DATAIN-12), smaxss(pDisplayTuning->tMinPQ >> (QB02DATAIN-12), uOut));

            if(uOut<uLutMaxValue)
            {   // matlab is not monotonical
                uOut = uLutMaxValue;
            }
            else
            {
                uLutMaxValue = uOut;
            }

            *toneCurveLUT++  =  uOut;
        }

    }
    else
    {
        int32_t iVal;

        for(iloop = 0 ; iloop<4096; iloop+=istep)
        {
            iVal = iloop;

            if (clampOut)
              iVal = sminss(pDisplayTuning->tMaxPQ >> (QB02DATAIN-12), smaxss(pDisplayTuning->tMinPQ >> (QB02DATAIN-12), iVal));

            *toneCurveLUT++ = (int16_t)iVal;
        }
    }



    if (!toneCurve4KLUT) { // skip 4K lut
      return;
    }

    // duplicate edge points  only used for interpolation
    toneCurve515LUT[0] = toneCurve515LUT[1];
    toneCurve515LUT[513] = toneCurve515LUT[512];
    toneCurve515LUT[514] = toneCurve515LUT[512];

    toneCurveLUT = toneCurve515LUT;
    // Interpolate from 512 to 4k
    // using  intfilt(8,1,0.1)

    for(iloop = 0 ; iloop<512; iloop++)
    {
       //X0 = P0
        *toneCurve4KLUT++ = toneCurveLUT[1] ;

       //X1 =  C9*P0 + C1*P1 + C7*P2 + C15*P3
        lratio  = INT8_FILT_C9 * toneCurveLUT[0];
        lratio += INT8_FILT_C1 * toneCurveLUT[1];
        lratio += INT8_FILT_C7 * toneCurveLUT[2];
        lratio += INT8_FILT_C15 * toneCurveLUT[3];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);

       //X2 =  C10*P0 + C2*P1 + C6*P2 + C14*P3
        lratio  = INT8_FILT_C10 * toneCurveLUT[0];
        lratio += INT8_FILT_C2 * toneCurveLUT[1];
        lratio += INT8_FILT_C6 * toneCurveLUT[2];
        lratio += INT8_FILT_C14 * toneCurveLUT[3];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);

       //X3 =  C11*P0 + C3*P1 + C5*P2 + C13*P3
        lratio  = INT8_FILT_C11 * toneCurveLUT[0];
        lratio += INT8_FILT_C3 * toneCurveLUT[1];
        lratio += INT8_FILT_C5 * toneCurveLUT[2];
        lratio += INT8_FILT_C13 * toneCurveLUT[3];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);


       //X4 =  C12 * (P0 + P3) + C4*(P1 + P2)
        lratio  = INT8_FILT_C12 * (toneCurveLUT[0] + toneCurveLUT[3]);
        lratio += INT8_FILT_C4 * (toneCurveLUT[1] + toneCurveLUT[2]);
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);

        //X5 =  C11*P3 + C3*P2 + C5*P1 + C13*P0
        lratio  = INT8_FILT_C11 * toneCurveLUT[3];
        lratio += INT8_FILT_C3 * toneCurveLUT[2];
        lratio += INT8_FILT_C5 * toneCurveLUT[1];
        lratio += INT8_FILT_C13 * toneCurveLUT[0];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);


        //X6 =  C10*P3 + C2*P2 + C6*P1 + C14*P0
        lratio  = INT8_FILT_C10 * toneCurveLUT[3];
        lratio += INT8_FILT_C2 * toneCurveLUT[2];
        lratio += INT8_FILT_C6 * toneCurveLUT[1];
        lratio += INT8_FILT_C14 * toneCurveLUT[0];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);


       //X7 =  C9*P3 + C1*P2 + C7*P1 + C15*P0
        lratio  = INT8_FILT_C9 * toneCurveLUT[3];
        lratio += INT8_FILT_C1 * toneCurveLUT[2];
        lratio += INT8_FILT_C7 * toneCurveLUT[1];
        lratio += INT8_FILT_C15 * toneCurveLUT[0];
        lratio += (unsigned)0x1 << (INT_FILT_Q-1);
        *toneCurve4KLUT++ = sminss(smaxss(lratio >> (INT_FILT_Q),toneCurveLUT[1]),uLutMaxValue);

        toneCurveLUT++;


    }


return;
}

#ifdef WIN32
#pragma optimize( "", on )
#endif
