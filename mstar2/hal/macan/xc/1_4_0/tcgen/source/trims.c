
#if 0
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
#include "trims.h"
#include "DMFxPtQs.h"
#include <memory.h>

void  SwapTrims( TRIM_PARAMS *dstMDTrimsA,TRIM_PARAMS *dstMDTrimsB)
{
    TRIM_PARAMS stemp;
    memcpy(&stemp,dstMDTrimsA,sizeof(TRIM_PARAMS));
    memcpy(dstMDTrimsA,dstMDTrimsB,sizeof(TRIM_PARAMS));
    memcpy(dstMDTrimsB,&stemp,sizeof(TRIM_PARAMS));
}

void SortTrims(TRIM_PARAMS *dstTrims,int first,int last)
{
    int pivot,j,i;

    if(first<last)
    {
        pivot=first;
        i=first;
        j=last;

        while(i<j)
        {
            while(dstTrims[i].TargetMax <= dstTrims[pivot].TargetMax &&i<last)
                i++;
            while(dstTrims[j].TargetMax > dstTrims[pivot].TargetMax)
                j--;
            if(i<j)
            {
                SwapTrims(&dstTrims[i],&dstTrims[j]);
            }
        }

        SwapTrims(&dstTrims[pivot],&dstTrims[j]);

        SortTrims(dstTrims,first,j-1);
        SortTrims(dstTrims,j+1,last);

    }
}

void CalcTrimValues(
    TRIM_PARAMS *dstTrim,
    TRIM_PARAMS srcMDTrims[MAX_TRIMS+1],
    TRIM_PARAMS *defaultTrim ,
    U16         numMDTrims ,
    S32         SmaxPQ,S32 TmaxPQ)
{

    S32 TargetLum , iloop , index;
    S32 y2,y1,x2,x1,x;
    S32 ltemp2 , ldenom_n1 , ldenom,lnum , scaledown , lnorm1;
    S64 lltemp ;

    if(numMDTrims<1)
    {
        dstTrim->TargetMax         = SmaxPQ;
        dstTrim->Slope             = defaultTrim->Slope;
        dstTrim->Offset            = defaultTrim->Offset;
        dstTrim->Power             = defaultTrim->Power;
        dstTrim->ChromaWeight      = defaultTrim->ChromaWeight;
        dstTrim->SaturationGain    = sadd_sss(defaultTrim->SaturationGain , (1<<QTRIMS));

    }
    else
    {
        numMDTrims = sminss(MAX_TRIMS,numMDTrims);

        /* error check if Metadata TargetMax is same as SmaxPQ */
        for(iloop = 0 ; iloop<numMDTrims ; iloop++)
        {
            if(SmaxPQ==srcMDTrims[iloop].TargetMax)
            {
                srcMDTrims[iloop].TargetMax = 0;//Make this metadata invalid
            }

        }


        srcMDTrims[numMDTrims].TargetMax    = SmaxPQ;
        srcMDTrims[numMDTrims].Slope        = defaultTrim->Slope;
        srcMDTrims[numMDTrims].Offset       = defaultTrim->Offset;
        srcMDTrims[numMDTrims].Power        = defaultTrim->Power;
        srcMDTrims[numMDTrims].ChromaWeight = defaultTrim->ChromaWeight;
        srcMDTrims[numMDTrims].SaturationGain = defaultTrim->SaturationGain;

        /* sort trims */
        SortTrims(srcMDTrims,0,numMDTrims );

        /*TargetLum = max(Trims(1,1), min(DMParams.SmaxPQ, DMParams.TmaxPQ));*/
        TargetLum = smaxss(srcMDTrims[0].TargetMax , sminss(SmaxPQ,TmaxPQ));


        iloop = 1;
        index = numMDTrims-1;
        while(1)
        {
            if(TargetLum <= srcMDTrims[iloop].TargetMax)
            {
                index = iloop-1;
                break;
            }
            else
                iloop++;
        }

        dstTrim->TargetMax = TargetLum;

        x1 = srcMDTrims[index].TargetMax;
        x2 = srcMDTrims[index+1].TargetMax;
        x  = (TargetLum-x1);
        lnorm1   = unorm_s(x);
        x      = sshl_su(x,lnorm1);


        ltemp2      = x2-x1;
        ldenom_n1   = unorm_s(ltemp2);
        ldenom      = sshl_su(ltemp2,ldenom_n1);
        ldenom      = sdiv_sls((S64)0x1<<(30+31),ldenom);
        lnum        = smpy_ss(x , ldenom);
        scaledown   = 30-ldenom_n1+lnorm1;

        /* Slope */
        y1 = srcMDTrims[index].Slope;
        y2 = srcMDTrims[index+1].Slope;
        lltemp = lmpy_ss(lnum,y2-y1);
        ltemp2 = sshr_slu(lltemp,scaledown);
        dstTrim->Slope = sadd_sss(ltemp2,y1);

        /* Offset */
        y1 = srcMDTrims[index].Offset;
        y2 = srcMDTrims[index+1].Offset;
        lltemp = lmpy_ss(lnum,y2-y1);
        ltemp2 = sshr_slu(lltemp,scaledown);
        dstTrim->Offset = sadd_sss(ltemp2,y1);

        /* Power */
        y1 = srcMDTrims[index].Power;
        y2 = srcMDTrims[index+1].Power;
        lltemp = lmpy_ss(lnum,y2-y1);
        ltemp2 = sshr_slu(lltemp,scaledown);
        dstTrim->Power = sadd_sss(ltemp2,y1);

        /* ChromaWeight */
        y1 = srcMDTrims[index].ChromaWeight;
        y2 = srcMDTrims[index+1].ChromaWeight;
        lltemp = lmpy_ss(lnum,y2-y1);
        ltemp2 = sshr_slu(lltemp,scaledown);
        dstTrim->ChromaWeight = sadd_sss(ltemp2,y1);

        /* SaturationGain */
        y1 = srcMDTrims[index].SaturationGain;
        y2 = srcMDTrims[index+1].SaturationGain;
        lltemp = lmpy_ss(lnum,y2-y1);
        ltemp2 = sshr_slu(lltemp,scaledown);
        dstTrim->SaturationGain = sadd_sss(sadd_sss(ltemp2,y1), (1<<QTRIMS));
    }


}


#endif

