#define _MSACE_C_

#include "MsAce_LIB.h"
#include "drvACE.h"

//--------------------------------------------------------------------------------------------------
// msACE version history:
// v100(2008-04-16): Add version infromation
//       Change internal contrast/RGB gain variable to WORD(12bits)
//       Add new function for adjust contrast/RGB gain
//          void msAdjustVideoContrast_10Bits( BOOL bScalerWin, WORD wContrast );
//          void msAdjustVideoRGB_10Bits( BOOL bScalerWin, WORD ucRCon, WORD ucGCon, WORD ucBCon);
//          void msAdjustPCContrast_10Bits( BOOL bScalerWin, WORD wContrast );
//          void msAdjustPCRGB_10Bits( BOOL bScalerWin, WORD wRed, WORD wGreen, WORD wBlue );
// v101(2008-04-16): Modify msACE_GetVersion()
// v102(2008-09-04):
//  1.Fine tune color correction control.
//  2.Fine tune sRGB control
// v103(2008-09-08):
//  1.Add new function for get more infromation from ACE library
//  2.Seperate chip relative code to another file.
// v104(2008-09-17):
//  1.Use WriteByteMask to access REG_ADDR_CM_CTL_MAIN
//  2.Fix msACE_GetInfo() uncall warning.
// v105(2009-07-14):
//  1.Fine tune the define in LIB_Group.
//  2.Add new group for external chip.
// v106(2009-10-28):
//  1.Add C51 version info.
//  2.Add debug-info-YUVtoRGB Matrix selection
//  3.Add debug function to get Matrix
//  4.Add option for using internal sin/cos function.
//  5.Change some function interface to static.
// v107(2010-01-26):
//  1.Refine Sine&Cosine function: Using lookup table.
// v108(2010-04-13):
//  1.Enable Sine&Cosine function using lookup table.
//  2.Add 2 function: msACE_SetColorMatrixControl(), msACE_SetRBChannelRange()
#define MSACE_VERSION   (108)
//----------------------------------------------------------------------------------------------------

#if defined(__C51__)
#define STR_C51_VERSION(x) "C51:"##  #x

#define C51_VERSION(x) STR_C51_VERSION(x)

char* ace_c51version = C51_VERSION(__C51__);
#endif

#define MAIN_WINDOW     0
#define SUB_WINDOW      1

#ifdef MULTI_SCALER_SUPPORTED
#define SC1_MAIN_WINDOW     2
#define SC1_SUB_WINDOW      3
#define SC2_MAIN_WINDOW     4
#define SC2_SUB_WINDOW      5
#endif

typedef short Typ3x3Array[3][3];


typedef struct
{
    WORD wContrast;
    WORD wRCon;
    WORD wGCon;
    WORD wBCon;
    BYTE ucSaturation;
    BYTE ucHue;
    short sContrastRGBMatrix[3][3];
    short sVideoSatHueMatrix[3][3];
    short* psColorCorrectionMatrix; // this variable must pointer to a array[3][3]
    short* psPCsRGBMatrix; // sRGB matrix for PC, this variable must pointer to a array[3][3]
    short* psYVUtoRGBMatrix; // this variable must pointer to a array[3][3]
    BYTE bForceYUVtoRGB : 1;
    BYTE ucYUVtoRGBMatrixSel : 2;
}StruAceInfo;

#ifdef MULTI_SCALER_SUPPORTED
#define MAX_WINDOW 6
#else
#define MAX_WINDOW 2
#endif
static StruAceInfo s_AceInfo[MAX_WINDOW];

///////////////////////////////////////////////////////////////////////////////////////////////

void msAdjustPCContrast_10Bits( BOOL bScalerWin, WORD wContrast );
void msAdjustPCRGB_10Bits( BOOL bScalerWin, WORD wRed, WORD wGreen, WORD wBlue );
void msAdjustVideoContrast_10Bits( BOOL bScalerWin, WORD wContrast );
void msAdjustVideoRGB_10Bits( BOOL bScalerWin, WORD wRCon, WORD wGCon, WORD wBCon);

///////////////////////////////////////////////////////////////////////////////////////////////
WORD msACE_GetVersion(void)
{
    return MSACE_VERSION;
}

static WORD _TransferColorCorrectionOffsetValue(short s)
{
    WORD w;

    w = s;
    if( w > 0x7F )
    {
        w = s*-1;
        w |= 0x80;
    }
    return w;
}
WORD msACE_GetInfo( BOOL bWindow, EnuACEInfoType ucInfoType )
{
    WORD wReturnValue = 0xFFFF;


    switch( ucInfoType )
    {
        case ACE_INFO_TYPE_VERSION:
            wReturnValue = MSACE_VERSION;
            break;
        case ACE_INFO_TYPE_CONTRAST:
            wReturnValue = s_AceInfo[bWindow].wContrast;
            break;
        case ACE_INFO_TYPE_R_GAIN:
            wReturnValue = s_AceInfo[bWindow].wRCon;
            break;
        case ACE_INFO_TYPE_G_GAIN:
            wReturnValue = s_AceInfo[bWindow].wGCon;
            break;
        case ACE_INFO_TYPE_B_GAIN:
            wReturnValue = s_AceInfo[bWindow].wBCon;
            break;
        case ACE_INFO_TYPE_SATURATION:
            wReturnValue = s_AceInfo[bWindow].ucSaturation;
            break;
        case ACE_INFO_TYPE_HUE:
            wReturnValue = s_AceInfo[bWindow].ucHue;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_XY_R:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = s_AceInfo[bWindow].psColorCorrectionMatrix[26]*-1;
                wReturnValue = ((wReturnValue&0xFF00)>>4)|(wReturnValue&0xFF);
            }
            else
                wReturnValue = 0;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_XY_G:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = s_AceInfo[bWindow].psColorCorrectionMatrix[27];
                wReturnValue = ((wReturnValue&0xFF00)>>4)|(wReturnValue&0xFF);
            }
            else
                wReturnValue = 0;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_XY_B:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = s_AceInfo[bWindow].psColorCorrectionMatrix[28]*-1;
                wReturnValue = ((wReturnValue&0xFF00)>>4)|(wReturnValue&0xFF);
            }
            else
                wReturnValue = 0;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_R:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = _TransferColorCorrectionOffsetValue(s_AceInfo[bWindow].psColorCorrectionMatrix[29]);
            }
            else
                wReturnValue = 0;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_G:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = _TransferColorCorrectionOffsetValue(s_AceInfo[bWindow].psColorCorrectionMatrix[30]);
            }
            else
                wReturnValue = 0;
            break;
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_B:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix != NULL )
            {
                wReturnValue = _TransferColorCorrectionOffsetValue(s_AceInfo[bWindow].psColorCorrectionMatrix[31]);
            }
            else
                wReturnValue = 0;
            break;

        case ACE_INFO_TYPE_COMPILER_VERSION:
    #if defined(__C51__)
            wReturnValue = __C51__;
    #else
            wReturnValue = 0;
    #endif
            break;

        case ACE_INFO_TYPE_YUV_TO_RGB_MATRIX_SEL:
            wReturnValue = s_AceInfo[bWindow].ucYUVtoRGBMatrixSel;
            break;

        case ACE_INFO_TYPE_DUMMY:
        default:
            wReturnValue = 0;
            break;
    }

    return wReturnValue;
}

// matrix used to convert RGB color space to YUV color space for SDTV
//code short tSDTVRgb2yuv[3][3] =
//{
//    {  0x0107,  0x0204,  0x0064 }, // 0.257,  0.504,  0.098
//    { -0x0098, -0x012A,  0x01C2 }, // -0.148, -0.291, 0.439
//    {  0x01C2, -0x0179, -0x0049 }  // 0.439,  -0.368, -0.071
//};

// matrix used to convert YUV color space to RGB color space, used for video input for SDTV
static code short tSDTVYuv2rgb[3][3] =
{
    {   0x0667, 0x04AC,  0x0000 }, // 1.600586,  1.167969, 0
    {  -0x0342, 0x04AC, -0x0191 }, // -0.815430, 1.167969, -0.392578
    {   0x0000, 0x04AC,  0x0818 }  // 0,         1.167969, 2.023438
};

// matrix used to convert RGB color space to YUV color space for HDTV
//code short tHDTVRgb2yuv[3][3] =
//{
//    {  0x00DA,  0x02DC,  0x004A }, // 0.213,  0.715,  0.072
//    { -0x0078, -0x0193,  0x020B }, // -0.117, -0.394, 0.511
//    {  0x020B, -0x01DB, -0x0030 }  // 0.511,  -0.464, -0.047
//};

// matrix used to convert YUV color space to RGB color space, used for video input for HDTV
static code short tHDTVYuv2rgb[3][3] =
{
    {   0x0731, 0x04AC,  0x0000 }, // 1.797851,  1.167969, 0
    {  -0x0222, 0x04AC, -0x00DA }, // -0.534180, 1.167969, -0.213867
    {   0x0000, 0x04AC,  0x0879 }  // 0,         1.167969, 2.118164
};

static void CopyMatrix(WORD* pwBufSrc, WORD* pwBufTgt, BYTE ucLen )
{
    BYTE i;
    for( i = 0; i < ucLen; ++ i )
    {
        pwBufTgt[i] = pwBufSrc[i];
    }
}
BOOL msACE_GetMatrix( BOOL bWindow, BYTE ucMatrixId, WORD* pwBuf, BYTE ucBufLen)
{
    BYTE ucCopyLen = 9;


    // Check parameter ...

    //if( ucBufLen < ucCopyLen )
        ucCopyLen = ucBufLen;

    switch(ucMatrixId)
    {
        case ACE_MATRIX_ID__YUV_TO_RGB:
            if( s_AceInfo[bWindow].ucYUVtoRGBMatrixSel == ACE_YUV_TO_RGB_MATRIX_SDTV )
            {
                CopyMatrix( (WORD*)tSDTVYuv2rgb, pwBuf, ucCopyLen );
            }
            else if( s_AceInfo[bWindow].ucYUVtoRGBMatrixSel == ACE_YUV_TO_RGB_MATRIX_HDTV )
            {
                CopyMatrix( (WORD*)tHDTVYuv2rgb, pwBuf, ucCopyLen );
            }
            else if( s_AceInfo[bWindow].ucYUVtoRGBMatrixSel == ACE_YUV_TO_RGB_MATRIX_USER )
            {
                if( s_AceInfo[bWindow].psYVUtoRGBMatrix == NULL )
                    return FALSE;
                CopyMatrix( (WORD*)s_AceInfo[bWindow].psYVUtoRGBMatrix, pwBuf, ucCopyLen );
            }
            break;
        case ACE_MATRIX_ID__COLOR_CORRECTION:
            if( s_AceInfo[bWindow].psColorCorrectionMatrix == NULL)
                return FALSE;
            CopyMatrix( (WORD*)s_AceInfo[bWindow].psColorCorrectionMatrix, pwBuf, ucCopyLen );
            break;

        case ACE_MATRIX_ID__SRGB:
            if( s_AceInfo[bWindow].psPCsRGBMatrix == NULL)
                return FALSE;
            CopyMatrix( (WORD*)s_AceInfo[bWindow].psPCsRGBMatrix, pwBuf, ucCopyLen );
            break;
    }
    return TRUE;
}

void msACEPCForceYUVtoRGB( BOOL bWin, BOOL bEnable )
{
    if( bEnable )
        s_AceInfo[bWin].bForceYUVtoRGB = 1;
    else
        s_AceInfo[bWin].bForceYUVtoRGB = 0;
}
void msACESelectYUVtoRGBMatrix( BOOL bWindow, BYTE ucMatrix, short* psUserYUVtoRGBMatrix )
{
    s_AceInfo[bWindow].ucYUVtoRGBMatrixSel = ucMatrix;
    if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_USER )
        s_AceInfo[bWindow].psYVUtoRGBMatrix = psUserYUVtoRGBMatrix;
    else if( ucMatrix == ACE_YUV_TO_RGB_MATRIX_HDTV )
        s_AceInfo[bWindow].psYVUtoRGBMatrix = (short*)tHDTVYuv2rgb;
    else // ACE_YUV_TO_RGB_MATRIX_SDTV
        s_AceInfo[bWindow].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb;
}
//---------------------------------------------------------------------------
// Specify the color correction table.
//---------------------------------------------------------------------------
void msACESetColorCorrectionTable( BOOL bWindow, short* psColorCorrectionTable )
{
    s_AceInfo[bWindow].psColorCorrectionMatrix = psColorCorrectionTable;
}

//---------------------------------------------------------------------------
// Specify the sRGB table.
//---------------------------------------------------------------------------
void msACESetPCsRGBTable( BOOL bWin, short* psPCsRGBTable )
{
    s_AceInfo[bWin].psPCsRGBMatrix = psPCsRGBTable;
}

void InitACEVar(void)
{
    BYTE i;

    for( i = 0; i < MAX_WINDOW; ++ i )
    {
        s_AceInfo[i].wRCon = 1024;
        s_AceInfo[i].wGCon = 1024;
        s_AceInfo[i].wBCon = 1024;
        s_AceInfo[i].wContrast = 1024;
        s_AceInfo[i].ucSaturation = 0x80;
        s_AceInfo[i].ucHue = 50;

        s_AceInfo[i].sContrastRGBMatrix[0][0] =
        s_AceInfo[i].sContrastRGBMatrix[1][1] =
        s_AceInfo[i].sContrastRGBMatrix[2][2] = 1024;
        s_AceInfo[i].sContrastRGBMatrix[0][1] =
        s_AceInfo[i].sContrastRGBMatrix[1][0] =
        s_AceInfo[i].sContrastRGBMatrix[2][0] =
        s_AceInfo[i].sContrastRGBMatrix[0][2] =
        s_AceInfo[i].sContrastRGBMatrix[1][2] =
        s_AceInfo[i].sContrastRGBMatrix[2][1] = 0;

        s_AceInfo[i].sVideoSatHueMatrix[0][0] =
        s_AceInfo[i].sVideoSatHueMatrix[1][1] =
        s_AceInfo[i].sVideoSatHueMatrix[2][2] = 1024;
        s_AceInfo[i].sVideoSatHueMatrix[0][1] =
        s_AceInfo[i].sVideoSatHueMatrix[1][0] =
        s_AceInfo[i].sVideoSatHueMatrix[2][0] =
        s_AceInfo[i].sVideoSatHueMatrix[0][2] =
        s_AceInfo[i].sVideoSatHueMatrix[1][2] =
        s_AceInfo[i].sVideoSatHueMatrix[2][1] = 0;


        //s_AceInfo[i].psColorCorrectionMatrix = NULL;
        msACESetColorCorrectionTable( i, NULL );

        //s_AceInfo[i].psPCsRGBMatrix = NULL;
        msACESetPCsRGBTable( i, NULL );

        //s_AceInfo[i].bForceYUVtoRGB = 0;
        msACEPCForceYUVtoRGB( i, FALSE );

        //s_AceInfo[i].ucYUVtoRGBMatrixSel = ACE_YUV_TO_RGB_MATRIX_SDTV;
        //s_AceInfo[i].psYVUtoRGBMatrix = (short*)tSDTVYuv2rgb;
        msACESelectYUVtoRGBMatrix( i, ACE_YUV_TO_RGB_MATRIX_SDTV, NULL );
    }

#if defined(__C51__)
    // For uncall warning...
    if( i == 0 )
    {
        msACE_GetVersion();
        msACE_GetInfo(0, 0);
        msAdjustPCContrast_10Bits(0,0);
        msAdjustPCRGB_10Bits( 0, 0, 0, 0 );
        msAdjustVideoContrast_10Bits(0,0);
        msAdjustVideoRGB_10Bits( 0, 0, 0, 0 );
        msACE_GetMatrix( 0, 0, NULL, 0);
        msACE_SetColorMatrixControl( 0, 0 );
        msACE_SetRBChannelRange( 0, 0 );
    }
#endif
}

#define ENABLE_SINE_COSINE_USING_LOOKUP_TABLE   1

#if( ENABLE_SINE_COSINE_USING_LOOKUP_TABLE ) // sine/cosine use lookup table

static short sine(BYTE u8Hue)
{
    code short tHueToSine[] =
    {
        784, // 0
        772, // 1
        760, // 2
        748, // 3
        736, // 4
        724, // 5
        711, // 6
        698, // 7
        685, // 8
        671, // 9
        658, // 10
        644, // 11
        630, // 12
        616, // 13
        601, // 14
        587, // 15
        572, // 16
        557, // 17
        542, // 18
        527, // 19
        511, // 20
        496, // 21
        480, // 22
        464, // 23
        448, // 24
        432, // 25
        416, // 26
        400, // 27
        383, // 28
        366, // 29
        350, // 30
        333, // 31
        316, // 32
        299, // 33
        282, // 34
        265, // 35
        247, // 36
        230, // 37
        212, // 38
        195, // 39
        177, // 40
        160, // 41
        142, // 42
        124, // 43
        107, // 44
        89, // 45
        71, // 46
        53, // 47
        35, // 48
        17, // 49
        0, // 50
        -17, // 51
        -35, // 52
        -53, // 53
        -71, // 54
        -89, // 55
        -107, // 56
        -124, // 57
        -142, // 58
        -160, // 59
        -177, // 60
        -195, // 61
        -212, // 62
        -230, // 63
        -247, // 64
        -265, // 65
        -282, // 66
        -299, // 67
        -316, // 68
        -333, // 69
        -350, // 70
        -366, // 71
        -383, // 72
        -400, // 73
        -416, // 74
        -432, // 75
        -448, // 76
        -464, // 77
        -480, // 78
        -496, // 79
        -512, // 80
        -527, // 81
        -542, // 82
        -557, // 83
        -572, // 84
        -587, // 85
        -601, // 86
        -616, // 87
        -630, // 88
        -644, // 89
        -658, // 90
        -671, // 91
        -685, // 92
        -698, // 93
        -711, // 94
        -724, // 95
        -736, // 96
        -748, // 97
        -760, // 98
        -772, // 99
        -784, // 100
    };
    if( u8Hue > 100 )
        u8Hue = 100;
    return tHueToSine[u8Hue];
}
// cos function
static short cosine(BYTE u8Hue)
{
    code short tHueToCosine[] =
    {
        658, // 0
        671, // 1
        685, // 2
        698, // 3
        711, // 4
        724, // 5
        736, // 6
        748, // 7
        760, // 8
        772, // 9
        784, // 10
        795, // 11
        806, // 12
        817, // 13
        828, // 14
        838, // 15
        848, // 16
        858, // 17
        868, // 18
        877, // 19
        886, // 20
        895, // 21
        904, // 22
        912, // 23
        920, // 24
        928, // 25
        935, // 26
        942, // 27
        949, // 28
        955, // 29
        962, // 30
        968, // 31
        973, // 32
        979, // 33
        984, // 34
        989, // 35
        993, // 36
        997, // 37
        1001, // 38
        1005, // 39
        1008, // 40
        1011, // 41
        1014, // 42
        1016, // 43
        1018, // 44
        1020, // 45
        1021, // 46
        1022, // 47
        1023, // 48
        1023, // 49
        1024, // 50
        1023, // 51
        1023, // 52
        1022, // 53
        1021, // 54
        1020, // 55
        1018, // 56
        1016, // 57
        1014, // 58
        1011, // 59
        1008, // 60
        1005, // 61
        1001, // 62
        997, // 63
        993, // 64
        989, // 65
        984, // 66
        979, // 67
        973, // 68
        968, // 69
        962, // 70
        955, // 71
        949, // 72
        942, // 73
        935, // 74
        928, // 75
        920, // 76
        912, // 77
        904, // 78
        895, // 79
        886, // 80
        877, // 81
        868, // 82
        858, // 83
        848, // 84
        838, // 85
        828, // 86
        817, // 87
        806, // 88
        795, // 89
        784, // 90
        772, // 91
        760, // 92
        748, // 93
        736, // 94
        724, // 95
        711, // 96
        698, // 97
        685, // 98
        671, // 99
        658, // 100
    };
    if( u8Hue > 100 )
        u8Hue = 100;
    return tHueToCosine[u8Hue];
}

#else

#define PI  (3.14159265358979323846)

#if( USE_ACE_INTERNAL_SIN_COS ) // Use user define
static double _ACE_sin( double f0)
{
    double  f2, f4;
    int     r0;

    f0 *= 6.3661977236758134e-01;
    if (f0 < 0)
    {
        if (f0 < -1073741824.0)
        {

            if (f0 < -1.0e+18)
                    f0 = 0;

            f0 /= 1073741824.0;
            r0 = (int)f0;

            f2 = (double) r0;
            f0 -= f2;
            f0 *= 1073741824.0;
        }

        f2 = f0;
        f2 -= 0.5;
    }
    else
    {
        if (f0 >= 1073741824.0)
        {
            if (f0 > 1.0e+18)
                f0 = 0;
            f0 /= 1073741824.0;

            r0 = f0;
            f2 = (double) r0;
            f0 -= f2;
            f0 *= 1073741824.0;
        }

        f2 = f0;
        f2 += 0.5;
    }

    r0 = (int) f2;
    f2 = (double) r0;
    f0 -= f2;
    if (r0 & 1)
    {
        f0 *= f0;
        f2 = 6.5659631149794723e-11;
        f2 *= f0;
        f2 += -6.3866030837918522e-09;
        f2 *= f0;
        f2 += 4.7108747788181715e-07;
        f2 *= f0;
        f2 += -2.5202042373060605e-05;
        f2 *= f0;
        f2 += 9.1926027483942659e-04;
        f2 *= f0;
        f2 += -2.0863480763352961e-02;
        f2 *= f0;
        f2 += 2.5366950790104802e-01;
        f2 *= f0;
        f2 += -1.2337005501361698e+00;
        f0 *= f2;
        f0 += 1.0;
    }
    else
    {
        f4 = f0;
        f0 *= f0;
        f2 = -6.6880351098114673e-10;
        f2 *= f0;
        f2 += 5.6921729219679268e-08;
        f2 *= f0;
        f2 += -3.5988432352120853e-06;
        f2 *= f0;
        f2 += 1.6044118478735982e-04;
        f2 *= f0;
        f2 += -4.6817541353186881e-03;
        f2 *= f0;
        f2 += 7.9692626246167046e-02;
        f2 *= f0;
        f2 += -6.4596409750624625e-01;
        f0 *= f2;
        f0 += 1.5707963267948966e+00;
        f0 *= f4;
    }

    if (r0 & 2)
      f0 = -f0;

    return(f0);
}

static double _ACE_cos(double f0)
{
    double f2,f4;
    int r0;

    f0 *= 6.3661977236758134e-01;
    if (f0 < 0)
      f0 = -f0;

    if (f0 > 1073741824.0)
    {
        if (f0 > 1.0e+18)
            f0 = 0;

        f0 /= 1073741824.0;
        r0 = f0;
        f0 = (double) r0;
        f0 -= f2;
        f0 *= 1073741824.0;
    }

    f2 = f0;
    f0 += 0.5;
    r0 = (int)f0;
    f0 = (double) r0;
    f0 -= f2;

    if (r0 & 1)
    {
        f4 = f0;
        f0 *= f0;
        f2 = -6.6880351098114673e-10;
        f2 *= f0;
        f2 += 5.6921729219679268e-08;
        f2 *= f0;
        f2 += -3.5988432352120853e-06;
        f2 *= f0;
        f2 += 1.6044118478735982e-04;
        f2 *= f0;
        f2 += -4.6817541353186881e-03;
        f2 *= f0;
        f2 += 7.9692626246167046e-02;
        f2 *= f0;
        f2 += -6.4596409750624625e-01;
        f0 *= f2;
        f0 += 1.5707963267948966e+00;
        f0 *= f4;
    }
    else
    {
        f0 *= f0;
        f2 = 6.5659631149794723e-11;
        f2 *= f0;
        f2 += -6.3866030837918522e-09;
        f2 *= f0;
        f2 += 4.7108747788181715e-07;
        f2 *= f0;
        f2 += -2.5202042373060605e-05;
        f2 *= f0;
        f2 += 9.1926027483942659e-04;
        f2 *= f0;
        f2 += -2.0863480763352961e-02;
        f2 *= f0;
        f2 += 2.5366950790104802e-01;
        f2 *= f0;
        f2 += -1.2337005501361698e+00;
        f0 *= f2;
        f0 += 1.0;
    }

    if (r0 & 2)
        f0 = -f0;

    return(f0);
}
#define sin     _ACE_sin
#define cos     _ACE_cos

#endif

//#define PI  (3.14159265)
// sin function
static short sine(WORD u16Hue)
{
    return (short)(sin((u16Hue * PI) / 180.) * 1024.);
}
// cos function
static short cosine(WORD u16Hue)
{
    return (short)(cos((u16Hue * PI) / 180.) * 1024.);
}
#endif

static void SetVideoSatHueMatrix( BOOL bWindow )
{
    short sTmp;
    WORD u16Hue;

#if( ENABLE_SINE_COSINE_USING_LOOKUP_TABLE )
    u16Hue = s_AceInfo[bWindow].ucHue;
#else
    u16Hue = ((s_AceInfo[bWindow].ucHue <= 50) ? (50 - s_AceInfo[bWindow].ucHue) : (360-(s_AceInfo[bWindow].ucHue-50)));
#endif

    sTmp = ((short)s_AceInfo[bWindow].ucSaturation * 8);

#if (ENABLE_SINE_COSINE_USING_LOOKUP_TABLE)
    s_AceInfo[bWindow].sVideoSatHueMatrix[2][2] = (short)( ((((long)cosine((BYTE)u16Hue) * sTmp))>>10) );
    s_AceInfo[bWindow].sVideoSatHueMatrix[0][0] = (short)( ((( (long)cosine((BYTE)u16Hue)* sTmp))>>10) );
    s_AceInfo[bWindow].sVideoSatHueMatrix[2][0] = (short)( ((((long)sine((BYTE)u16Hue)   * sTmp))>>10) );
    s_AceInfo[bWindow].sVideoSatHueMatrix[0][2] = (short)( (((-(long)sine((BYTE)u16Hue)  * sTmp))>>10) );
#else
    s_AceInfo[bWindow].sVideoSatHueMatrix[2][2] = ((((long)cosine(u16Hue) * sTmp))>>10);
    s_AceInfo[bWindow].sVideoSatHueMatrix[0][0] = ((( (long)cosine(u16Hue)* sTmp))>>10);
    s_AceInfo[bWindow].sVideoSatHueMatrix[2][0] = ((((long)sine(u16Hue)   * sTmp))>>10);
    s_AceInfo[bWindow].sVideoSatHueMatrix[0][2] = (((-(long)sine(u16Hue)  * sTmp))>>10);
#endif

    s_AceInfo[bWindow].sVideoSatHueMatrix[1][1] = 1024;
    s_AceInfo[bWindow].sVideoSatHueMatrix[0][1] = s_AceInfo[bWindow].sVideoSatHueMatrix[1][0] = s_AceInfo[bWindow].sVideoSatHueMatrix[1][2] = s_AceInfo[bWindow].sVideoSatHueMatrix[2][1] = 0;
}

static void SetVideoContrastMatrix( BOOL bWindow )
{
    s_AceInfo[bWindow].sContrastRGBMatrix[0][0] = (short)(( (DWORD)s_AceInfo[bWindow].wRCon * (s_AceInfo[bWindow].wContrast)) >> 10 );
    s_AceInfo[bWindow].sContrastRGBMatrix[1][1] = (short)(( (DWORD)s_AceInfo[bWindow].wGCon * (s_AceInfo[bWindow].wContrast)) >> 10 );
    s_AceInfo[bWindow].sContrastRGBMatrix[2][2] = (short)(( (DWORD)s_AceInfo[bWindow].wBCon * (s_AceInfo[bWindow].wContrast)) >> 10 );
}

static void SetVideoContrastMatrix_G( BOOL bWindow )
{
    s_AceInfo[bWindow].sContrastRGBMatrix[0][0] = (short)(( (DWORD)s_AceInfo[bWindow].wRCon * (1024UL)) >> 10 );
    s_AceInfo[bWindow].sContrastRGBMatrix[1][1] = (short)(( (DWORD)s_AceInfo[bWindow].wGCon * (s_AceInfo[bWindow].wContrast)) >> 10 );
    s_AceInfo[bWindow].sContrastRGBMatrix[2][2] = (short)(( (DWORD)s_AceInfo[bWindow].wBCon * (1024UL)) >> 10 );
}

static void ArrayMultiply(short sFirst[3][3], short sSecond[3][3], short sResult[3][3])
{
    BYTE ucRow, ucCol;

    //go line by line
    for(ucRow=0; ucRow!=3; ucRow++)
    {
        // go column by column
        for(ucCol=0; ucCol!=3; ucCol++)
        {
            sResult[ucRow][ucCol] = (short)( ( ((long)sFirst[ucRow][0] * sSecond[0][ucCol]) +
                                               ((long)sFirst[ucRow][1] * sSecond[1][ucCol]) +
                                               ((long)sFirst[ucRow][2] * sSecond[2][ucCol]  )) >> 10 );
        } // for
    } // for
}

static void msWriteColorMatrix( BOOL bWindow, short* psMatrix )
{
    BYTE i,j;
    BYTE ucAddr;
    short sTmp;
    DECLARA_BANK_VARIABLE


    BACKUP_SC_BANK
    if( bWindow == MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_MAIN);
        ucAddr = REG_ADDR_CM_MAIN&0xFF;
    }
    else if(bWindow == SUB_WINDOW)
    {
        SET_SC_BANK(BANK_CM_SUB);
        ucAddr = REG_ADDR_CM_SUB&0xFF;
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if(bWindow == SC1_MAIN_WINDOW)
    {
        SET_SC_BANK(BANK_CM_SC1_MAIN);
        ucAddr = REG_ADDR_CM_SC1_MAIN&0xFF;
    }
    else if(bWindow == SC1_SUB_WINDOW)
    {
        SET_SC_BANK(BANK_CM_SC1_SUB);
        ucAddr = REG_ADDR_CM_SC1_SUB&0xFF;
    }
    else if(bWindow == SC2_MAIN_WINDOW)
    {
        SET_SC_BANK(BANK_CM_SC2_MAIN);
        ucAddr = REG_ADDR_CM_SC2_MAIN&0xFF;
    }
    else// SC2_SUB_WINDOW
    {
        SET_SC_BANK(BANK_CM_SC2_SUB);
        ucAddr = REG_ADDR_CM_SC2_SUB&0xFF;
    }
#endif

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
            sTmp = psMatrix[i*3+j];
            if( sTmp >= 0 )
            {
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
                //SC_WRITE_2BYTE( ucAddr, sTmp );
            }
            else
            {
                sTmp = sTmp * -1;
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
                #ifdef USE_NEW_ACE_MATRIX_RULE
                    sTmp = ~sTmp +1;
                #else
                sTmp |= 0x1000;
                #endif

                //SC_WRITE_2BYTE( ucAddr, sTmp );
            }
            ACE_WRITE_CM( ucAddr, sTmp );

            ucAddr += 2;
        } // for
    } // for
    RECOVER_SC_BANK

}

void msSetVideoColorMatrix( BOOL bScalerWin )
{
    short sResultTmp1[3][3];
    short sResultTmp2[3][3];
    short* psLastResult;
    DECLARA_BANK_VARIABLE

    U8 i,j;
    short ColorMatrix_1[3][3];
    short ColorMatrix_2[3][3];
    U8 u8CTLVal;

    BACKUP_SC_BANK


    // Adjust hue&saturation, and then YUV to RGB
    //ArrayMultiply( (Typ3x3Array)(s_AceInfo[bScalerWin].psYVUtoRGBMatrix), s_AceInfo[bScalerWin].sVideoSatHueMatrix, sResultTmp1 );

        for ( i=0;i<3;i++ )
        {
            for ( j=0;j<3;j++ )
            {
               ColorMatrix_1[i][j]=*(s_AceInfo[bScalerWin].psYVUtoRGBMatrix +(i*3)+j);;
               ColorMatrix_2[i][j]= s_AceInfo[bScalerWin].sVideoSatHueMatrix[i][j];
            }
        }
       ArrayMultiply(ColorMatrix_1,ColorMatrix_2, sResultTmp1 );




    // Do color correction
    if( NULL != s_AceInfo[bScalerWin].psColorCorrectionMatrix )
    {
      // ArrayMultiply( (Typ3x3Array)(s_AceInfo[bScalerWin].psColorCorrectionMatrix), sResultTmp1, sResultTmp2 );
        for ( i=0;i<3;i++ )
        {
            for ( j=0;j<3;j++ )
            {
               ColorMatrix_1[i][j]=*(s_AceInfo[bScalerWin].psColorCorrectionMatrix +(i*3)+j);
            }
         }
         ArrayMultiply( ColorMatrix_1, sResultTmp1, sResultTmp2 );


        // Adjust contrast-RGB
        ArrayMultiply( s_AceInfo[bScalerWin].sContrastRGBMatrix, sResultTmp2, sResultTmp1 );

        psLastResult = (short*)sResultTmp1;
    }
    else // No color correction
    {
        // Adjust contrast-RGB
        ArrayMultiply( s_AceInfo[bScalerWin].sContrastRGBMatrix, sResultTmp1, sResultTmp2 );
        psLastResult = (short*)sResultTmp2;
    }
    if (MDrv_ACE_IsSupportMLoad(bScalerWin) && (!MDrv_ACE_GetSkipWaitVsync(bScalerWin)))
        {
            MDrv_ACE_WriteColorMatrixBurst( bScalerWin, (MS_U16*)psLastResult );
        }
        else
        {
            msWriteColorMatrix( bScalerWin, psLastResult );
        }


    if(s_AceInfo[bScalerWin].bForceYUVtoRGB == FALSE)
    {
        u8CTLVal = ENABLE_CM_CTL2;
    }
    else
    {
        u8CTLVal = ENABLE_CM_CTL1;
    }

    if( bScalerWin == MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_MAIN, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
        //ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_MAIN, 0x35, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
    else if( bScalerWin == SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SUB, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
        //ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SUB, 0x35, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if( bScalerWin == SC1_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_MAIN, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
    else if( bScalerWin == SC1_SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_SUB, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
    else if( bScalerWin == SC2_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC2_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_MAIN, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
    else // SC2_SUB_WINDOW
    {
        SET_SC_BANK(BANK_CM_SC2_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_SUB, u8CTLVal, MASK_CM_CTL ); // Enable color matrix&Change R/B range
    }
#endif
    RECOVER_SC_BANK
}

void msAdjustHSC(BOOL bScalerWin, BYTE ucHue, BYTE ucSaturation, BYTE ucContrast)
{
    s_AceInfo[bScalerWin].ucSaturation = ucSaturation;
    s_AceInfo[bScalerWin].wContrast = (WORD)ucContrast<<3;
    s_AceInfo[bScalerWin].ucHue = ucHue;
    SetVideoSatHueMatrix( bScalerWin );
    SetVideoContrastMatrix( bScalerWin );

    msSetVideoColorMatrix( bScalerWin );
}

void msAdjustVideoContrast( BOOL bScalerWin, BYTE ucContrast )
{
    s_AceInfo[bScalerWin].wContrast = (WORD)ucContrast<<3;
    SetVideoContrastMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}

void msAdjustVideoContrast_G( BOOL bScalerWin, BYTE ucContrast )
{
    s_AceInfo[bScalerWin].wContrast = (WORD)ucContrast<<3;
    SetVideoContrastMatrix_G( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}

void msAdjustVideoContrast_10Bits( BOOL bScalerWin, WORD wContrast )
{
    s_AceInfo[bScalerWin].wContrast = wContrast<<1;
    SetVideoContrastMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}

void msAdjustVideoSaturation( BOOL bScalerWin, BYTE ucSaturation )
{
    s_AceInfo[bScalerWin].ucSaturation = ucSaturation;
    SetVideoSatHueMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}

void msAdjustVideoHue( BOOL bScalerWin, BYTE ucHue )
{
    s_AceInfo[bScalerWin].ucHue = ucHue;
    SetVideoSatHueMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}
void msAdjustVideoRGB( BOOL bScalerWin, BYTE ucRCon, BYTE ucGCon, BYTE ucBCon)
{
    s_AceInfo[bScalerWin].wRCon = (WORD)ucRCon<<3;
    s_AceInfo[bScalerWin].wGCon = (WORD)ucGCon<<3;
    s_AceInfo[bScalerWin].wBCon = (WORD)ucBCon<<3;
    SetVideoContrastMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}
void msAdjustVideoRGB_10Bits( BOOL bScalerWin, WORD wRCon, WORD wGCon, WORD wBCon)
{
    s_AceInfo[bScalerWin].wRCon = wRCon<<1;
    s_AceInfo[bScalerWin].wGCon = wGCon<<1;
    s_AceInfo[bScalerWin].wBCon = wBCon<<1;
    SetVideoContrastMatrix( bScalerWin );
    msSetVideoColorMatrix( bScalerWin );
}

static void SetPCConRGBMatrix( BOOL bWindow )
{
    s_AceInfo[bWindow].sContrastRGBMatrix[0][0] = (short)(( (DWORD)s_AceInfo[bWindow].wRCon * (s_AceInfo[bWindow].wContrast)) >> 10);
    s_AceInfo[bWindow].sContrastRGBMatrix[1][1] = (short)(( (DWORD)s_AceInfo[bWindow].wGCon * (s_AceInfo[bWindow].wContrast)) >> 10);
    s_AceInfo[bWindow].sContrastRGBMatrix[2][2] = (short)(( (DWORD)s_AceInfo[bWindow].wBCon * (s_AceInfo[bWindow].wContrast)) >> 10);
}

static void SetPCConRGBMatrix_G( BOOL bWindow)
{
    s_AceInfo[bWindow].sContrastRGBMatrix[0][0] = (short)(( (DWORD)s_AceInfo[bWindow].wRCon * (1024UL)) >> 10);
    s_AceInfo[bWindow].sContrastRGBMatrix[1][1] = (short)(( (DWORD)s_AceInfo[bWindow].wGCon * (s_AceInfo[bWindow].wContrast)) >> 10);
    s_AceInfo[bWindow].sContrastRGBMatrix[2][2] = (short)(( (DWORD)s_AceInfo[bWindow].wBCon * (1024UL)) >> 10);
}


void msSetPCColorMatrix( BOOL bScalerWin )
{
    short sResultTmp1[3][3];
    short sResultTmp2[3][3];
    short* psOutTab;
    //BYTE ucCMCtlVal;
    DECLARA_CMCTL_VARIABLE
    DECLARA_BANK_VARIABLE

    U8 i,j;
    short ColorMatrix_1[3][3];
    short ColorMatrix_2[3][3];


    BACKUP_SC_BANK


    psOutTab = (short*)s_AceInfo[bScalerWin].sContrastRGBMatrix;

    // YUV => RGB
    if( (s_AceInfo[bScalerWin].bForceYUVtoRGB) || (bScalerWin == SUB_WINDOW) )
    {
        //ArrayMultiply( s_AceInfo[bScalerWin].sContrastRGBMatrix, (Typ3x3Array)(s_AceInfo[bScalerWin].psYVUtoRGBMatrix), sResultTmp1 );
        for ( i=0;i<3;i++ )
        {
            for ( j=0;j<3;j++ )
            {
               ColorMatrix_1[i][j]=*(s_AceInfo[bScalerWin].psYVUtoRGBMatrix +(i*3)+j);;
            }
        }
        ArrayMultiply( s_AceInfo[bScalerWin].sContrastRGBMatrix, ColorMatrix_1, sResultTmp1 );


        psOutTab = (short*)sResultTmp1;
        //ucCMCtlVal = 0x35;
        CTLVal = ENABLE_CM_CTL1;
    }
    else
    {
        CTLVal = ENABLE_CM_CTL2;
        //ucCMCtlVal = 0x30;
    }

    // Convert to sRGB
    if( s_AceInfo[bScalerWin].psPCsRGBMatrix != NULL )
    {
        //ArrayMultiply( (Typ3x3Array)s_AceInfo[bScalerWin].psPCsRGBMatrix, (Typ3x3Array)psOutTab, sResultTmp2 );
        for ( i=0;i<3;i++ )
        {
            for ( j=0;j<3;j++ )
            {
               ColorMatrix_1[i][j]=*(s_AceInfo[bScalerWin].psPCsRGBMatrix +(i*3)+j);;
               ColorMatrix_2[i][j]=*(psOutTab +(i*3)+j);;
            }
        }
        ArrayMultiply( ColorMatrix_1, ColorMatrix_2, sResultTmp2 );


        psOutTab = (short*)sResultTmp2;
    }

    if (MDrv_ACE_IsSupportMLoad(bScalerWin))
    {
        MDrv_ACE_WriteColorMatrixBurst( bScalerWin, (MS_U16*)psOutTab );
    }
    else
    {
        msWriteColorMatrix( bScalerWin, psOutTab );
    }

    if( bScalerWin == MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_MAIN, CTLVal, MASK_CM_CTL );
    }
    else if( bScalerWin == SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SUB, CTLVal, MASK_CM_CTL );
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if( bScalerWin == SC1_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_MAIN, CTLVal, MASK_CM_CTL );
    }
    else if( bScalerWin == SC1_SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_SUB, CTLVal, MASK_CM_CTL );
    }
    else if( bScalerWin == SC2_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC2_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_MAIN, CTLVal, MASK_CM_CTL );
    }
    else // SC2_SUB_WINDOW
    {
        SET_SC_BANK(BANK_CM_SC2_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_SUB, CTLVal, MASK_CM_CTL );
    }
#endif
    RECOVER_SC_BANK
}

void msAdjustPCContrast( BOOL bScalerWin, BYTE ucContrast )
{
    s_AceInfo[bScalerWin].wContrast = (WORD)ucContrast<<3;
    SetPCConRGBMatrix( bScalerWin );
    msSetPCColorMatrix( bScalerWin );
}


void msAdjustPCContrast_G( BOOL bScalerWin, BYTE ucContrast )
{
    s_AceInfo[bScalerWin].wContrast = (WORD)ucContrast<<3;
    SetPCConRGBMatrix_G( bScalerWin );
    msSetPCColorMatrix( bScalerWin );
}

void msAdjustPCContrast_10Bits( BOOL bScalerWin, WORD wContrast )
{
    s_AceInfo[bScalerWin].wContrast = wContrast<<1;
    SetPCConRGBMatrix( bScalerWin );
    msSetPCColorMatrix( bScalerWin );
}

void msAdjustPCRGB( BOOL bScalerWin, BYTE ucRed, BYTE ucGreen, BYTE ucBlue )
{
    s_AceInfo[bScalerWin].wRCon = (WORD)ucRed<<3;
    s_AceInfo[bScalerWin].wGCon = (WORD)ucGreen<<3;
    s_AceInfo[bScalerWin].wBCon = (WORD)ucBlue<<3;
    SetPCConRGBMatrix( bScalerWin );
    msSetPCColorMatrix( bScalerWin );
}
void msAdjustPCRGB_10Bits( BOOL bScalerWin, WORD wRed, WORD wGreen, WORD wBlue )
{
    s_AceInfo[bScalerWin].wRCon = wRed<<1;
    s_AceInfo[bScalerWin].wGCon = wGreen<<1;
    s_AceInfo[bScalerWin].wBCon = wBlue<<1;
    SetPCConRGBMatrix( bScalerWin );
    msSetPCColorMatrix( bScalerWin );
}

//-------------------------------------------------------------------------------
// Directly control color matrix on/off
//-------------------------------------------------------------------------------
void msACE_SetColorMatrixControl( BOOL bScalerWin, BOOL bEnable )
{
    DECLARA_CMCTL_VARIABLE
    DECLARA_BANK_VARIABLE

    BACKUP_SC_BANK

    if( bEnable )
        CTLVal = MASK_CM_CTL_EN;
    else
        CTLVal = 0;

    if( bScalerWin == MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_MAIN, CTLVal, MASK_CM_CTL_EN );
    }
    else if( bScalerWin == SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SUB, CTLVal, MASK_CM_CTL_EN );
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if( bScalerWin == SC1_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_MAIN, CTLVal, MASK_CM_CTL_EN );
    }
    else if( bScalerWin == SC1_SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_SUB, CTLVal, MASK_CM_CTL_EN );
    }
    else if( bScalerWin == SC2_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC2_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_MAIN, CTLVal, MASK_CM_CTL_EN );
    }
    else // SC2_SUB_WINDOW
    {
        SET_SC_BANK(BANK_CM_SC2_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_SUB, CTLVal, MASK_CM_CTL_EN );
    }
#endif
    RECOVER_SC_BANK
}

//-------------------------------------------------------------------------------
// Directly control color matrix R/B range
// bRange: 0 -> 0~255, 1 -> -128~127
//-------------------------------------------------------------------------------
void msACE_SetRBChannelRange( BOOL bScalerWin, BOOL bRange )
{
    DECLARA_CMCTL_VARIABLE

    DECLARA_BANK_VARIABLE

    BACKUP_SC_BANK

    if( bRange )
        CTLVal = MASK_CM_CTL_RB_RANGE;
    else
        CTLVal = 0;

    if( bScalerWin == MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_MAIN, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
    else if( bScalerWin == SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SUB, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if( bScalerWin == SC1_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_MAIN, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
    else if( bScalerWin == SC1_SUB_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC1_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC1_SUB, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
    else if( bScalerWin == SC2_MAIN_WINDOW )
    {
        SET_SC_BANK(BANK_CM_SC2_MAIN);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_MAIN, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
    else // SC2_SBU_WINDOW
    {
        SET_SC_BANK(BANK_CM_SC2_SUB);
        ACE_WRITE_CM_CTL( REG_ADDR_CM_CTL_SC2_SUB, CTLVal, MASK_CM_CTL_RB_RANGE );
    }
#endif
    RECOVER_SC_BANK
}
