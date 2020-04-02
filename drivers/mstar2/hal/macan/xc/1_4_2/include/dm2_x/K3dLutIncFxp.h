/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
* 			              All rights reserved.
****************************************************************************/
#ifndef _K_3DLUT_INC_FXP_H_
#define _K_3DLUT_INC_FXP_H_

typedef uint16_t UShortV3_t[3];

#define D3INTERP_FixPnt(lut3d_, c_,  b_, px_, py_, pz_, dx_, dy_, dz_)                 \
  D3INTERP0_FixPnt(lut3d_[b_            ][c_],   lut3d_[b_             + pz_][c_],   \
              lut3d_[b_       + py_][c_],   lut3d_[b_       + py_ + pz_][c_],   \
              lut3d_[b_ + px_      ][c_],   lut3d_[b_ + px_       + pz_][c_],   \
              lut3d_[b_ + px_ + py_][c_],   lut3d_[b_ + px_ + py_ + pz_][c_],   \
              dx_, dy_, dz_)

// Input/output data are all 16 bits unsigned. Intermediate variables maintain 20 bits
uint16_t D3INTERP0_FixPnt(uint16_t v000_, uint16_t v001_, uint16_t v010_, uint16_t v011_, uint16_t v100_, uint16_t v101_, uint16_t v110_, uint16_t v111_, uint16_t dx_, uint16_t dy_, uint16_t dz_)
{
    uint32_t v, v00, v01, v10, v11;
    uint16_t one;
    
    // make 2^16-1 as 1.0
    one = DLB_UINT_MAX(16);

#   if USE_12BITS_IN_3D_LUT
    // 12x16 unsigned multiplier, result maitains 16 bits MSB
    v00 = ((uint32_t)v000_ * (one-dz_) + (uint32_t)v001_ * dz_) >> 12;
    v01 = ((uint32_t)v010_ * (one-dz_) + (uint32_t)v011_ * dz_) >> 12;
    v10 = ((uint32_t)v100_ * (one-dz_) + (uint32_t)v101_ * dz_) >> 12;
    v11 = ((uint32_t)v110_ * (one-dz_) + (uint32_t)v111_ * dz_) >> 12;

    // 16x16 unsigned multiplier, result maitains 16 bits MSB
    v00 = ((uint32_t)v00 * (one-dy_) + (uint32_t)v01 * dy_) >> 16;
    v11 = ((uint32_t)v10 * (one-dy_) + (uint32_t)v11 * dy_) >> 16;

    // 16x16 unsigned multiplier, result maitains 16 bits MSB
    v = ((uint32_t)v00 * (one-dx_) + (uint32_t)v11 * dx_) >> 16;
#   else
    // 16x16 unsigned multiplier, result maitains 20 bits MSB
    v00 = ((uint32_t)v000_ * (one-dz_) + (uint32_t)v001_ * dz_) >> 12;
    v01 = ((uint32_t)v010_ * (one-dz_) + (uint32_t)v011_ * dz_) >> 12;
    v10 = ((uint32_t)v100_ * (one-dz_) + (uint32_t)v101_ * dz_) >> 12;
    v11 = ((uint32_t)v110_ * (one-dz_) + (uint32_t)v111_ * dz_) >> 12;
    
    // 20x16 unsigned multiplier, result maitains 20 bits MSB
    v00 = ((uint64_t)v00 * (one-dy_) + (uint64_t)v01 * dy_) >> 16;
    v11 = ((uint64_t)v10 * (one-dy_) + (uint64_t)v11 * dy_) >> 16;
    
    // 20x16 unsigned multiplier, result maitains 20 bits MSB
    v = ((uint64_t)v00 * (one-dx_) + (uint64_t)v11 * dx_) >> 16;
    
    // result in 16 bits unsigned
    v = v >> 4;
#   endif
    
    return (uint16_t)v;
}

// Input format 0.15 unsigned; output format 0.12 unsigned
static void C3DLutFixPnt(const DmLutFxp_t *pDmLut, UShortV3_t *lut, int32_t *iy0, int32_t *iy1, int32_t *iy2)
{
    int32_t iI,iP,iT;   // index of grid coordinates
    int32_t iB;         // base linear addr
    uint16_t dI,dP,dT;   // delta linear addr of the next position
    uint16_t ix0, ix1, ix2;
    uint16_t data_scale;
    
	// we use 15 to achieve better precision for re-map
    // re-map to x.y=4.16 format (4 bits integer and 16 bits fraction)
    iI = ((int64_t)((int32_t)*iy0 - pDmLut->iMinC1) * pDmLut->iDistC1Inv) >> (14 - 4); // 2^4 = dmLut.dimC1 - 1
    iP = ((int64_t)((int32_t)*iy1 - pDmLut->iMinC2) * pDmLut->iDistC2Inv) >> (14 - 4);
    iT = ((int64_t)((int32_t)*iy2 - pDmLut->iMinC3) * pDmLut->iDistC3Inv) >> (14 - 4);

    data_scale = 16;

    ix0 = iI & DLB_UINT_MAX(data_scale);
    ix1 = iP & DLB_UINT_MAX(data_scale);
    ix2 = iT & DLB_UINT_MAX(data_scale);
    iI = iI >> data_scale; 
    iP = iP >> data_scale;
    iT = iT >> data_scale;
    
//    assert(iI >= 0 && iP >= 0 && iT >= 0);
//    assert(iI <= (pDmLut->dimC1 - 1) && iP <= (pDmLut->dimC2 - 1) && iT <= (pDmLut->dimC3 - 1));

    // the lutMap is indexed in [B][G][R](or T, P, I)
    iB = iI + iP*pDmLut->pitch + iT*pDmLut->slice;
    dI = (iI < (pDmLut->dimC1 - 1)) ? 1           : 0;
    dP = (iP < (pDmLut->dimC2 - 1)) ? pDmLut->pitch : 0;
    dT = (iT < (pDmLut->dimC3 - 1)) ? pDmLut->slice : 0;
    *iy0 =  D3INTERP_FixPnt(lut, 0, iB, dI, dP, dT, ix0, ix1, ix2);
    *iy1 =  D3INTERP_FixPnt(lut, 1, iB, dI, dP, dT, ix0, ix1, ix2);
    *iy2 =  D3INTERP_FixPnt(lut, 2, iB, dI, dP, dT, ix0, ix1, ix2);
}

#define LUT3DFixPnt(gmLut_, y0_, y1_, y2_)																	\
	C3DLutFixPnt(gmLut_, (UShortV3_t *)(gmLut_->lutMap), y0_, y1_, y2_)

#endif // _K_3DLUT_INC_H_
