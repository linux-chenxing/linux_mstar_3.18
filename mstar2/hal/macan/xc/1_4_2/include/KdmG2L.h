/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                         All rights reserved.
****************************************************************************/
#ifndef KDM_G2L_H
#define KDM_G2L_H

#include "KdmLutLog.h"
#include "KdmLutLogRate.h"
#include "KdmLutPower.h"
#include "KdmLutPowerScale.h"

#ifndef MSTAR_DRIVER_MOD
#define MSTAR_DRIVER_MOD 1
#endif

#if MSTAR_DRIVER_MOD
uint32_t DeGamma(uint16_t sA, uint16_t sB, uint16_t sGamma, uint32_t sG, uint16_t x)
#else
uint32_t DeGamma(uint16_t sA, uint16_t sB, uint16_t sGamma, uint32_t sG, uint16_t y)
#endif
{
    int16_t overOne;
    uint32_t u32;
    uint16_t u16;
#if MSTAR_DRIVER_MOD
    uint16_t u16I;
#endif
    int toPowerOutScale2p;
    static const uint32_t logInterpBitMask = (1<<DEF_DEGAMMAR_INTERP_BITS) - 1;
    static const uint32_t logRate2logScale2p = DEF_DEGAMMAR_LOG_RATE_SCALE2P +
            DEF_DEGAMMAR_INTERP_BITS - DEF_DEGAMMAR_LOG_SCALE2P;
    static const uint32_t toPowerLutInScale2p = 16;
    static const uint32_t powerLutIdxMask =
        (1 << (DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)) - 1;

#if MSTAR_DRIVER_MOD
    // 0x0000 - 0x0FF0 : LSB 4b : 256 entries
    // 0x1000 - 0x1F80 : LSB 7b :  32 entries
    // 0x2000 - 0xFF00 : LSB 8b : 224 entries
    uint16_t y;
    if (x < 256)
        y = (MS_U16)x << 4;
    else if (x < 288)
        y = (((MS_U16)x-256) << 7) + 0x1000;
    else // if (idx < 512)
        y = (((MS_U16)x-288) << 8) + 0x2000;
#endif
    //// X+b
    u32 = y + sB;
    overOne = u32 >= (1<<DEF_DEGAMMAR_IN_SCALE2P) ? 1 : 0;

    //// down to 16 bit
    if (overOne)
    {
        u16 = (uint16_t)(u32>>1);
    }
    else
    {
        u16 = (uint16_t)u32;
    }

    //// log:
    // rate*delta, scale to log10lut scale
    u32 = (log10RateLut[u16>>DEF_DEGAMMAR_INTERP_BITS]*(u16 & logInterpBitMask)) >> logRate2logScale2p;
    // log(y+sB): liear interpolation: 'a*x + b', where the a = -Rate, b = log10Lut, x is the decimal part
    u32 = log10Lut[u16>>DEF_DEGAMMAR_INTERP_BITS] - u32;

    //// Sgamma*log(y + sB)
    u32 *= sGamma;

    //// power
    // scale to power lut input
    u32 >>= toPowerLutInScale2p;
    // power
    u16 = (uint16_t)u32;
    if ((u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)) < 10)
    {
        u32 = power10Lut[u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)][u16 & powerLutIdxMask];
        // to output scale:
        toPowerOutScale2p = powerScale2p[u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)] - DEF_DEGAMMAR_OUT_SCALE2P;
    }
    else // optimized for hardware. The last 6 LUTs are all zero. Skip.
        return 0;

    //// put /2 back and keep the original scale
    if (overOne)
    {
        u32 = (uint32_t)(((uint64_t)sG*u32) >> DEF_DEGAMMAR_OUT_SCALE2P);
    }

    u32 = ((uint64_t)u32 * sA) >> (2 + toPowerOutScale2p);
#if MSTAR_DRIVER_MOD
    // convert 32bit to mantissa(14b)+exponent(5b)
    for (u16I = 0; u16I < 18; u16I++)
    {
        if (u32 < (0x1ul << (u16I+14)))
            break;
    }
    u32 = ( (u32 >> u16I) << 5) | u16I;
#endif
    return u32;
}


#endif // KDM_G2L_H
