#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#include "mhal_dlc.h"
#include "technicolor_driver.h"
//#include "dolby_vision_driver.h"
//#include "dolby_vision_lut.h"
//#include "hwreg_xc.h"

#define ABS(x) (((x)>=0)?(x):(-(x)))
#define Min(a, b)   ( ((a) < (b)) ? (a) : (b) )
#define Max(a, b)   ( ((a) < (b)) ? (b) : (a) )
#define CLAMP(v, a, b)  ((v < a)? a: ((v > b)? b: v))

#define CFD_ColorSpace709 1
#define CFD_ColorSpace2020 9

#define BT1886 0
#define PQ     1
#define BT709  2
#define ARIB   3

STU_CFDAPI_MM_PARSER PrimeSingle_result;

static int32_t icc_x[256], icc_y[256];
int muA, muB;
uint16_t INV_C_LUT[65]={8, 13, 28, 40, 55, 67, 82, 95, 107, 121, 134, 148, 161, 174, 188, 200, 216,
228, 241, 256, 269, 282, 298, 309, 321, 334, 349, 364, 372, 390, 400, 420, 431,
443, 455, 468, 482, 496, 512, 529, 546, 546, 565, 585, 585, 607, 607, 630, 655,
655, 683, 683, 712, 712, 712, 745, 745, 780, 780, 780, 819, 819, 819, 862, 862  };

uint16_t C_LUT[65];
uint16_t P_LUT[65];
uint16_t S_LUT[65]={0, 128, 181, 222, 256, 286, 314, 339, 362, 384, 405, 425, 443, 462, 479, 496, 512, 528, 543
                             , 558, 572, 587, 600, 614, 627, 640, 653, 665, 677, 689, 701, 713, 724, 735, 746, 757, 768, 779
                             , 789, 799, 810, 820, 830, 839, 849, 859, 868, 878, 887, 896, 905, 914, 923, 932, 941, 949, 958, 966, 975, 983, 991, 1000, 1008, 1016, 1023};
uint16_t I_LUT[65] = {255, 209, 181, 162, 148, 137, 128, 121, 114, 109, 105, 100, 97, 93, 91, 88,
                    85, 83, 81, 79, 77, 75, 74, 72, 71, 70, 68, 67, 66, 65, 64, 63,
                    62, 61, 60, 60, 59, 58, 57, 57, 56, 55, 55, 54, 53, 53, 52, 52,
                    51, 51, 50, 50, 49, 49, 48, 48, 48, 47, 47, 46, 46, 46, 45, 37, 32};
uint16_t D_LUT[65];
uint16_t D_LUT_THRESHOLD[3];
uint16_t D_LUT_STEP[4];
int inv_y_ratio, inv_chroma_ratio, footroom, l_headroom, c_headroom;

#define INT8_MIN         (-127i8 - 1)
#define INT16_MIN        (-32767i16 - 1)
#define INT32_MIN        (-2147483647i32 - 1)
#define INT64_MIN        (-9223372036854775807i64 - 1)
#define INT8_MAX         127i8
#define INT16_MAX        32767i16
#define INT32_MAX        2147483647i32
#define INT64_MAX        9223372036854775807i64
#define UINT8_MAX        0xffui8
#define UINT16_MAX       0xffffui16
#define UINT32_MAX       0xffffffffui32
#define UINT64_MAX       0xffffffffffffffffui64

#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN
#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX
#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST16_MIN   INT32_MIN
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MAX   INT32_MAX
#define INT_FAST32_MAX   INT32_MAX
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT32_MAX
#define UINT_FAST32_MAX  UINT32_MAX
#define UINT_FAST64_MAX  UINT64_MAX

#ifdef _WIN64
    #define INTPTR_MIN   INT64_MIN
    #define INTPTR_MAX   INT64_MAX
    #define UINTPTR_MAX  UINT64_MAX
#else
    #define INTPTR_MIN   INT32_MIN
    #define INTPTR_MAX   INT32_MAX
    #define UINTPTR_MAX  UINT32_MAX
#endif

#define INTMAX_MIN       INT64_MIN
#define INTMAX_MAX       INT64_MAX
#define UINTMAX_MAX      UINT64_MAX

#define PTRDIFF_MIN      INTPTR_MIN
#define PTRDIFF_MAX      INTPTR_MAX

#ifndef SIZE_MAX
    #define SIZE_MAX     UINTPTR_MAX
#endif

#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX

#define WCHAR_MIN        0x0000
#define WCHAR_MAX        0xffff

#define WINT_MIN         0x0000
#define WINT_MAX         0xffff

#define INT8_C(x)    (x)
#define INT16_C(x)   (x)
#define INT32_C(x)   (x)
#define INT64_C(x)   (x ## LL)

#define UINT8_C(x)   (x)
#define UINT16_C(x)  (x)
#define UINT32_C(x)  (x ## U)
#define UINT64_C(x)  (x ## ULL)

#define INTMAX_C(x)  INT64_C(x)
#define UINTMAX_C(x) UINT64_C(x)

#define ABS(x) (((x)>=0)?(x):(-(x)))

#define Min(a, b)   ( ((a) < (b)) ? (a) : (b) )
#define Max(a, b)   ( ((a) < (b)) ? (b) : (a) )
#define CLAMP(v, a, b)  ((v < a)? a: ((v > b)? b: v))

#define BT1886 0
#define PQ     1
#define BT709  2
#define ARIB   3

//[DS]
static K_XC_DS_CMDCNT _stTCH_DS_CmdCnt;
extern void KHal_XC_StoreSWDSCommand(EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr,
                                     MS_U16 u16CmdRegValue, MS_U16 u16CmdRegMask, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);

#if 0
#define TCH_W2BYTEMSK(ADDR, VAL, MASK)  MApi_GFLIP_XC_W2BYTEMSK(ADDR,  VAL, MASK)
#define TCH_W2BYTE(ADDR, VAL)  MApi_GFLIP_XC_W2BYTE(ADDR,  VAL)
#else
#define TCH_W2BYTEMSK(ADDR, VAL, MASK)  \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR), ((VAL) & 0xFFFF), ((MASK) & 0xFFFF), K_DS_IP, K_DS_XC, &_stTCH_DS_CmdCnt);
#define TCH_W2BYTE(ADDR, VAL) \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR), ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stTCH_DS_CmdCnt);
#endif

static int32_t Gamma2OETF[65] = { 0,  2668,  5159,  7495,  9695, 11774, 13743, 15615, 17399, 19101, 20730, 22292, 23791, 25233, 26622, 27961, 29254, 30505, 31715, 32887, 34025, 35128, 36201, 37243, 38258, 39246, 40209, 41148, 42063, 42958, 43831, 44685, 45519, 46336, 47135, 47918, 48685, 49436, 50173, 50896, 51605, 52300, 52984, 53655, 54314, 54962, 55599, 56226, 56842, 57448, 58045, 58632, 59211, 59780, 60341, 60894, 61439, 61976, 62506, 63028, 63543, 64051, 64553, 65048, 65536 };

static int32_t ipbl_exp_gain[201] = {
  0,         56809,     65536,     70857,     74718,     77757,     80267,     82408,
  84274,     85929,     87417,     88769,     90007,     91150,     92211,     93201,
  94130,     95004,     95830,     96613,     97356,     98065,     98741,     99389,
  100009,    100605,    101179,    101731,    102263,    102778,    103276,    103757,
  104224,    104677,    105116,    105543,    105959,    106363,    106757,    107141,
  107515,    107881,    108237,    108586,    108926,    109260,    109586,    109905,
  110218,    110524,    110824,    111119,    111407,    111691,    111969,    112243,
  112511,    112775,    113034,    113289,    113540,    113787,    114029,    114268,
  114504,    114735,    114964,    115189,    115410,    115629,    115844,    116056,
  116266,    116472,    116676,    116877,    117076,    117272,    117466,    117657,
  117845,    118032,    118216,    118398,    118578,    118756,    118931,    119105,
  119277,    119447,    119615,    119781,    119945,    120108,    120269,    120428,
  120586,    120742,    120896,    121049,    121201,    121351,    121499,    121646,
  121792,    121936,    122079,    122221,    122361,    122500,    122638,    122774,
  122910,    123044,    123177,    123309,    123440,    123569,    123698,    123825,
  123952,    124077,    124201,    124325,    124447,    124569,    124689,    124809,
  124927,    125045,    125162,    125278,    125393,    125507,    125620,    125733,
  125845,    125956,    126066,    126175,    126284,    126391,    126498,    126605,
  126710,    126815,    126919,    127023,    127126,    127228,    127329,    127430,
  127530,    127629,    127728,    127826,    127924,    128021,    128117,    128213,
  128308,    128402,    128496,    128590,    128683,    128775,    128867,    128958,
  129048,    129139,    129228,    129317,    129406,    129494,    129581,    129668,
  129755,    129841,    129927,    130012,    130097,    130181,    130264,    130348,
  130431,    130513,    130595,    130676,    130758,    130838,    130919,    130998,
  131078,    131157,    131235,    131314,    131391,    131469,    131546,    131623,
  131699
};

static int32_t ibgl_one_over_black_gain_limiter_mix_factor[201] = {
  0,     42287, 36656, 33903, 32151, 30894, 29928, 29151,
  28506, 27956, 27481, 27062, 26690, 26355, 26052, 25775,
  25521, 25286, 25068, 24865, 24675, 24497, 24329, 24171,
  24021, 23878, 23743, 23614, 23491, 23373, 23261, 23153,
  23049, 22949, 22853, 22761, 22672, 22586, 22502, 22422,
  22344, 22268, 22195, 22123, 22054, 21987, 21921, 21858,
  21796, 21735, 21676, 21619, 21563, 21508, 21455, 21403,
  21351, 21302, 21253, 21205, 21158, 21112, 21067, 21023,
  20980, 20938, 20896, 20855, 20815, 20776, 20737, 20699,
  20662, 20625, 20589, 20554, 20519, 20485, 20451, 20418,
  20385, 20353, 20321, 20290, 20259, 20229, 20199, 20169,
  20140, 20112, 20083, 20056, 20028, 20001, 19974, 19948,
  19922, 19896, 19871, 19845, 19821, 19796, 19772, 19748,
  19724, 19701, 19678, 19655, 19633, 19610, 19588, 19567,
  19545, 19524, 19503, 19482, 19461, 19441, 19421, 19401,
  19381, 19361, 19342, 19323, 19304, 19285, 19266, 19248,
  19229, 19211, 19193, 19176, 19158, 19141, 19123, 19106,
  19089, 19072, 19056, 19039, 19023, 19007, 18991, 18975,
  18959, 18943, 18928, 18912, 18897, 18882, 18867, 18852,
  18837, 18822, 18808, 18793, 18779, 18765, 18751, 18737,
  18723, 18709, 18695, 18682, 18668, 18655, 18642, 18628,
  18615, 18602, 18589, 18577, 18564, 18551, 18539, 18526,
  18514, 18502, 18489, 18477, 18465, 18453, 18442, 18430,
  18418, 18406, 18395, 18383, 18372, 18361, 18349, 18338,
  18327, 18316, 18305, 18294, 18283, 18273, 18262, 18251,
  18241
};

static uint16_t EOTF_sqrt_alpha[199] =
{
         0,       731,      1250,      1653,      1980,      2256,      2494,      2703,
      2890,      3058,      3211,      3351,      3480,      3600,      3712,      3817,
      3915,      4008,      4096,        93,       182,       266,       347,       424,
       498,       569,       638,       703,       767,       828,       888,       945,
      1000,      1054,      1106,      1157,      1206,      1254,      1301,      1346,
      1390,      1434,      1476,      1517,      1557,      1596,      1634,      1672,
      1708,      1744,      1779,      1814,      1847,      1881,      1913,      1945,
      1976,      2007,      2037,      2066,      2096,      2124,      2152,      2180,
      2207,      2234,      2260,      2286,      2311,      2336,      2361,      2385,
      2410,      2433,      2457,      2480,      2502,      2525,      2547,      2569,
      2590,      2611,      2632,      2653,      2673,      2693,      2713,      2733,
      2752,      2771,      2790,      2809,      2828,      2846,      2864,      2882,
      2900,      2917,      2935,      2952,      2969,      2986,      3002,      3019,
      3035,      3051,      3067,      3083,      3098,      3114,      3129,      3144,
      3159,      3174,      3189,      3204,      3218,      3232,      3247,      3261,
      3275,      3289,      3302,      3316,      3329,      3343,      3356,      3369,
      3382,      3395,      3408,      3421,      3433,      3446,      3458,      3470,
      3483,      3495,      3507,      3519,      3531,      3542,      3554,      3566,
      3577,      3588,      3600,      3611,      3622,      3633,      3644,      3655,
      3666,      3677,      3687,      3698,      3709,      3719,      3729,      3740,
      3750,      3760,      3770,      3780,      3790,      3800,      3810,      3820,
      3830,      3839,      3849,      3859,      3868,      3878,      3887,      3896,
      3905,      3915,      3924,      3933,      3942,      3951,      3960,      3969,
      3978,      3986,      3995,      4004,      4012,      4021,      4029,      4038,
      4046,      4055,      4063,      4071,      4080,      4088,      4096
};

/* EOTF and Sqrt(): L_HDR = 100 nit */

static uint16_t EOETF_100_base[64] =
{
         0,       138,       321,       531,       763,      1014,      1283,      1570,
      1875,      2196,      2535,      2891,      3265,      3656,      4066,      4495,
      4942,      5409,      5896,      6403,      6932,      7482,      8054,      8650,
      9268,      9912,     10580,     11274,     11995,     12743,     13519,     14325,
     15161,     16028,      4232,      4465,      4706,      4957,      5216,      5485,
      5764,      6053,      6352,      6662,      6983,      7315,      7659,      8015,
      8384,      8766,      9161,      9570,      9994,     10432,     10886,     11355,
     11841,     12344,     12864,     13402,     13959,     14534,     15130,     15747
};

static uint16_t EOETF_100_slope[64] =
{
       138,       184,       210,       232,       251,       269,       287,       304,
       321,       339,       356,       374,       392,       410,       428,       447,
       467,       487,       507,       528,       550,       572,       595,       619,
       643,       668,       694,       721,       748,       776,       806,       836,
       867,       899,       233,       242,       250,       260,       269,       279,
       289,       299,       310,       321,       332,       344,       356,       369,
       382,       395,       409,       424,       438,       454,       469,       486,
       503,       520,       538,       557,       576,       596,       616,       637
};

static uint8_t EOETF_100_bend[64] =
{
        38,        15,        12,        10,         9,         9,         9,         9,
         9,         9,         9,         9,         9,         9,         9,        10,
        10,        10,        10,        11,        11,        11,        12,        12,
        12,        13,        13,        14,        14,        14,        15,        15,
        16,        16,         4,         4,         4,         5,         5,         5,
         5,         5,         5,         6,         6,         6,         6,         6,
         7,         7,         7,         7,         8,         8,         8,         8,
         9,         9,         9,         9,        10,        10,        10,        11
};

static uint8_t EOETF_100_shl[64] =
{
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1
};

/* EOTF and Sqrt(): L_HDR = 1000 nit */

static uint16_t EOETF_1000_base[64] =
{
         0,        71,       166,       277,       401,       537,       685,       846,
      1018,      1204,      1401,      1612,      1837,      2076,      2330,      2600,
      2885,      3187,      3507,      3846,      4203,      4581,      4980,      5401,
      5846,      6315,      6809,      7330,      7880,      8459,      9069,      9711,
     10387,     11099,     11849,     12638,     13469,     14342,     15262,     16229,
      4312,      4579,      4860,      5156,      5467,      5794,      6138,      6499,
      6879,      7279,      7698,      8139,      8603,      9090,      9602,     10140,
     10705,     11298,     11922,     12577,     13265,     13989,     14748,     15546
};

static uint16_t EOETF_1000_slope[64] =
{
        71,        96,       111,       124,       136,       148,       160,       173,
       185,       198,       211,       225,       239,       254,       269,       285,
       302,       320,       338,       358,       378,       399,       421,       444,
       469,       494,       521,       549,       579,       610,       642,       676,
       712,       750,       789,       830,       874,       919,       967,      1017,
       267,       281,       296,       311,       327,       344,       361,       380,
       399,       420,       441,       464,       487,       512,       538,       565,
       594,       624,       655,       688,       723,       760,       798,       838
};

static uint8_t EOETF_1000_bend[64] =
{
        20,         9,         7,         6,         6,         6,         6,         6,
         6,         7,         7,         7,         7,         8,         8,         8,
         9,         9,         9,        10,        10,        11,        11,        12,
        12,        13,        14,        14,        15,        16,        17,        17,
        18,        19,        20,        21,        22,        23,        24,        26,
         7,         7,         7,         8,         8,         9,         9,         9,
        10,        10,        11,        11,        12,        13,        13,        14,
        15,        15,        16,        17,        18,        19,        20,        21
};

static uint8_t EOETF_1000_shl[64] =
{
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         1,         1,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1
};

/* EOTF and Sqrt(): L_HDR = 10000 nit */

static uint16_t EOETF_10000_base[64] =
{
         0,        32,        77,       129,       189,       255,       329,       409,
       498,       594,       698,       812,       934,      1066,      1209,      1363,
      1529,      1708,      1900,      2106,      2327,      2565,      2821,      3095,
      3388,      3703,      4041,      4402,      4790,      5205,      5649,      6124,
      6633,      7178,      7761,      8385,      9052,      9766,     10529,     11346,
     12219,     13152,     14150,     15217,     16357,      4394,      4720,      5068,
      5440,      5838,      6263,      6717,      7202,      7720,      8274,      8866,
      9498,     10174,     10895,     11666,     12489,     13369,     14308,     15312
};

static uint16_t EOETF_10000_slope[64] =
{
        32,        44,        52,        60,        66,        74,        81,        88,
        96,       104,       113,       123,       132,       143,       154,       166,
       178,       192,       206,       222,       238,       255,       274,       294,
       315,       337,       362,       387,       415,       444,       476,       509,
       545,       583,       624,       667,       714,       763,       816,       873,
       933,       998,      1067,      1140,      1219,       326,       348,       372,
       398,       425,       454,       485,       518,       554,       592,       632,
       675,       722,       771,       823,       880,       940,      1004,      1072
};

static uint8_t EOETF_10000_bend[64] =
{
         9,         4,         4,         3,         3,         4,         4,         4,
         4,         4,         5,         5,         5,         5,         6,         6,
         7,         7,         7,         8,         8,         9,        10,        10,
        11,        12,        12,        13,        14,        15,        16,        17,
        18,        20,        21,        22,        24,        26,        27,        29,
        31,        33,        36,        38,        41,        11,        12,        12,
        13,        14,        15,        16,        17,        18,        20,        21,
        22,        24,        25,        27,        29,        31,        33,        35
};

static uint8_t EOETF_10000_shl[64] =
{
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         0,         0,         0,
         0,         0,         0,         0,         0,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1,
         1,         1,         1,         1,         1,         1,         1,         1
};



static uint16_t D_LUT_1886[65] = {
    0, 40, 72, 101, 128, 154, 179, 204, 228, 275, 320, 364, 406, 448, 489, 530,
        570, 648, 724, 799, 872, 944, 1015, 1085, 1154, 1222, 1290, 1357,
        1423, 1489, 1554, 1618,1682, 1809, 1933, 2056, 2178, 2298, 2418,
        2536, 2652, 2768, 2883, 2997, 3110, 3222, 3334, 3445, 3555, 3664,
        3773, 3881, 3988, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};
static uint16_t D_LUT_1886_STEP[4] = {

  5, 6, 7, 8

};
static uint16_t D_LUT_1886_THRESHOLD[3] = {

  256, 768, 2816

};


// D_LUT PQ
static uint16_t D_LUT_PQ_1000[65] = {
     0, 108, 206, 291, 367, 435, 498, 555,
 609, 707, 794, 873, 945, 1012, 1074, 1132,
1186, 1286, 1376, 1458, 1533, 1603, 1668, 1729, 1786, 1840, 1891, 1939, 1985, 2029, 2071, 2111,
2150, 2222, 2290, 2353, 2412, 2467, 2520, 2570, 2617, 2662, 2705, 2746, 2785, 2823, 2859, 2894,
2927, 2960, 2991, 3021, 3050, 3079, 3079, 3079, 3079, 3079, 3079, 3079, 3079, 3079, 3079, 3079, 3079
};
static uint16_t D_LUT_PQ_1200[65] = {
    0, 118, 223, 314, 394, 466, 531, 592,
 648, 749, 840, 922, 996, 1065, 1129, 1189,
1245, 1347, 1439, 1523, 1600, 1671, 1737, 1799, 1857, 1912, 1964, 2013, 2059, 2104, 2146, 2187,
2226, 2299, 2367, 2431, 2491, 2547, 2599, 2649, 2697, 2742, 2785, 2826, 2866, 2904, 2940, 2975,
3008, 3041, 3072, 3103, 3132, 3160, 3160, 3160, 3160, 3160, 3160, 3160, 3160, 3160, 3160, 3160, 3160,
};
static uint16_t D_LUT_PQ_1500[65] = {
     0, 132, 246, 343, 429, 505, 575, 638,
 697, 804, 899, 984, 1061, 1133, 1199, 1261,
1318, 1424, 1519, 1605, 1683, 1756, 1824, 1887, 1946, 2002, 2054, 2104, 2151, 2196, 2239, 2281,
2320, 2394, 2463, 2527, 2588, 2644, 2697, 2748, 2795, 2841, 2884, 2925, 2965, 3003, 3039, 3074,
3108, 3141, 3172, 3202, 3231, 3260, 3260, 3260, 3260, 3260, 3260, 3260, 3260, 3260, 3260, 3260, 3260
};
static uint16_t D_LUT_PQ_2000[65] = {
    0, 151, 278, 384, 477, 559, 634, 702,
 765, 878, 978, 1068, 1149, 1224, 1293, 1357,
1417, 1526, 1624, 1713, 1794, 1868, 1938, 2002, 2063, 2119, 2173, 2224, 2272, 2318, 2361, 2403,
2443, 2518, 2588, 2653, 2714, 2771, 2824, 2875, 2923, 2969, 3012, 3054, 3093, 3131, 3168, 3203,
3237, 3269, 3300, 3331, 3360, 3388, 3388, 3388, 3388, 3388, 3388, 3388, 3388, 3388, 3388, 3388, 3388
};
static uint16_t D_LUT_PQ_STEP[4] = {
  5, 6, 7, 8
};
static uint16_t D_LUT_PQ_THRESHOLD[3] = {
  256, 768, 2816
};

// D_LUT BT709
static uint16_t D_LUT_709[65] = {
     0, 1, 4, 10, 18, 28, 40, 55,
  72, 91, 112, 136, 162, 190, 220, 253,
 288, 364, 440, 516, 591, 666, 740, 813, 886, 959, 1031, 1102, 1174, 1245, 1315, 1385,
1455, 1595, 1733, 1870, 2006, 2141, 2276, 2409, 2542, 2675, 2806, 2938, 3068, 3198, 3328, 3457,
3585, 3713, 3841, 3968, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};
static uint16_t D_LUT_709_STEP[4] = {
 6, 6, 7, 8
};
static uint16_t D_LUT_709_THRESHOLD[3] = {
  512, 1024, 3072
};

// D_LUT ARIB
static uint16_t D_LUT_ARIB[65] = {
       0, 105, 209, 314, 419, 523, 628, 733,
     837, 942, 1047, 1151, 1256, 1361, 1465, 1570,
    1675, 1779, 1884, 1989, 2093, 2196, 2290, 2377, 2459, 2536, 2608, 2677, 2742, 2804, 2864, 2920,
    2975, 3078, 3174, 3263, 3347, 3427, 3502, 3573, 3642, 3707, 3769, 3829, 3886, 3941, 3994, 4045,
    4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095

};
static uint16_t D_LUT_ARIB_STEP[4] = {
  7, 7, 7, 8
};
static uint16_t D_LUT_ARIB_THRESHOLD[3] = {
 1024, 2048, 4096
};


static void Compute_muA_muB(sl_hdr1_metadata *sl_hdr1_metadata, int *muA, int *muB)
{
    int i, valNu = 0;

    if (!sl_hdr1_metadata->payloadMode) {
        // valNu, muA and muB compute : 13 bits by construction
        if (sl_hdr1_metadata->u.variables.saturationGainNumVal > 2) {
          for (i = 1; i < sl_hdr1_metadata->u.variables.saturationGainNumVal - 1; i++){
              valNu = 2 * valNu + (sl_hdr1_metadata->u.variables.saturationGainX[i] % 2);
              sl_hdr1_metadata->u.variables.saturationGainX[i] = sl_hdr1_metadata->u.variables.saturationGainX[i] - (sl_hdr1_metadata->u.variables.saturationGainX[i] % 2);
          }
          valNu = (valNu * 8191) / ((1 << (sl_hdr1_metadata->u.variables.saturationGainNumVal - 2)) - 1);
        }
        else {
          valNu = 8191;
        }

        *muA = valNu;

        if (!sl_hdr1_metadata->sdrPicColourSpace)
            *muB = (7952 * valNu) / 8191;
        else
            *muB = (7244 * valNu) / 8191;
    }
    else {
        *muA = sl_hdr1_metadata->u.tables.chromaToLumaInjectionMuA;
        *muB = sl_hdr1_metadata->u.tables.chromaToLumaInjectionMuB;
    }
}

void Compute_C_LUT(sl_hdr1_metadata *sl_hdr1_metadata, uint16_t *LUT, uint16_t *C_LUT)
{
  int i, j, j_last, idx[6], sgf[65], betaP[65];

  if (!sl_hdr1_metadata->payloadMode) {
    for (i = 0; i < sl_hdr1_metadata->u.variables.saturationGainNumVal; i++)
    {
      sl_hdr1_metadata->u.variables.saturationGainX[i] = sl_hdr1_metadata->u.variables.saturationGainX[i] - (sl_hdr1_metadata->u.variables.saturationGainX[i] % 2);
      idx[i] = (sl_hdr1_metadata->u.variables.saturationGainX[i] + 2) >> 2; // 6b
    }

    j_last = idx[0];
    sgf[0] = sl_hdr1_metadata->u.variables.saturationGainY[0] << 12; // 8b + 12b = 20b

    for (i = 1; i < sl_hdr1_metadata->u.variables.saturationGainNumVal; i++)
    {
      for (j = j_last+1; j <= idx[i]; j++)
      {
        // 6b * 8b * 12b / 6b = 20b
        sgf[j] = sgf[j_last] + ((((j - j_last) * (sl_hdr1_metadata->u.variables.saturationGainY[i] - sl_hdr1_metadata->u.variables.saturationGainY[i-1])) << 12) + ((idx[i] - j_last) >> 1)) / (idx[i] - j_last);
      }
      j_last = idx[i];
    }

    for (i = 0; i < 65; i++)
    {
      // 2b shift instead of 8b, 4b remaining integrated in 1 << 30 during inversion and 2b for ScaleSGF = 4
      betaP[i] = (LUT[i] * sgf[i] + 2) >> 2;
    }

    betaP[0] = 2 * betaP[1] - betaP[2];
    if (betaP[0] <= 0) {
      betaP[0] = 2047;
    }
    C_LUT[0] = betaP[0];

    for (i = 1; i < 65; i++)
    {
      betaP[i] = ((1 << 30) + (betaP[i] >> 1)) / betaP[i]; // 30b = 14b (C_LUT quantization) + 4b (previous compute) + 12b
      C_LUT[i] = Min(betaP[i], (1<<11) - 1);
    }
  }
  else {
    for(i = 0; i < 32; i++){
      for(j = 0; j < 2; j++){
        C_LUT[2 * i + j] = ((2 - j) * sl_hdr1_metadata->u.tables.colourCorrectionY[i] + j * sl_hdr1_metadata->u.tables.colourCorrectionY[i+1] + 1) >> 1;
      }
    }
    C_LUT[64] = sl_hdr1_metadata->u.tables.colourCorrectionY[32];
  }
}

void Fill_D_LUT(int curve, int PeakLuminance, uint16_t *D_LUT, uint16_t *D_LUT_THRESHOLD, uint16_t *D_LUT_STEP)
{
  switch (curve) {
    case BT1886 :
      memcpy(D_LUT, D_LUT_1886, 65 * sizeof(uint16_t));
      memcpy(D_LUT_STEP, D_LUT_1886_STEP, 4 * sizeof(uint16_t));
      memcpy(D_LUT_THRESHOLD, D_LUT_1886_THRESHOLD, 3 * sizeof(uint16_t));
      break;
    case PQ :
      // PQ curve depends on the video PeakLuminance
      switch (PeakLuminance)
      {
        case 1000:
          memcpy(D_LUT, D_LUT_PQ_1000, 65 * sizeof(uint16_t));
          break;
        case 1200:
          memcpy(D_LUT, D_LUT_PQ_1200, 65 * sizeof(uint16_t));
          break;
        case 1500:
          memcpy(D_LUT, D_LUT_PQ_1500, 65 * sizeof(uint16_t));
          break;
        case 2000:
          memcpy(D_LUT, D_LUT_PQ_2000, 65 * sizeof(uint16_t));
          break;
        default:
          printf( "Can only handle peak luminance 1000, 1200, 1500 and 2000 nits\n");
          break;
      }
      memcpy(D_LUT_STEP, D_LUT_PQ_STEP, 4 * sizeof(uint16_t));
      memcpy(D_LUT_THRESHOLD, D_LUT_PQ_THRESHOLD, 3 * sizeof(uint16_t));
      break;
    case BT709 :
      memcpy(D_LUT, D_LUT_709, 65 * sizeof(uint16_t));
      memcpy(D_LUT_STEP, D_LUT_709_STEP, 4 * sizeof(uint16_t));
      memcpy(D_LUT_THRESHOLD, D_LUT_709_THRESHOLD, 3 * sizeof(uint16_t));
      break;
    case ARIB:
      memcpy(D_LUT, D_LUT_ARIB, 65 * sizeof(uint16_t));
      memcpy(D_LUT_STEP, D_LUT_ARIB_STEP, 4 * sizeof(uint16_t));
      memcpy(D_LUT_THRESHOLD, D_LUT_ARIB_THRESHOLD, 3 * sizeof(uint16_t));
      break;
    default :
      memcpy(D_LUT, D_LUT_1886, 65 * sizeof(uint16_t));
      memcpy(D_LUT_STEP, D_LUT_1886_STEP, 4 * sizeof(uint16_t));
      memcpy(D_LUT_THRESHOLD, D_LUT_1886_THRESHOLD, 3 * sizeof(uint16_t));
  }
}



static int32_t interpolate(int32_t pu, int32_t *icc_x, int32_t *icc_y, int32_t icc_size)
{
  int32_t y, delta, slope, x_tmp, y_tmp, lidx, uidx, i;

  lidx = 0;
  uidx = 0;
  for (i = 0; i < icc_size; ++i) {
    x_tmp = (icc_x[i])*257 + ((icc_x[i] & (1<<7)) ? 1 : 0);
    if (pu < x_tmp) {
      uidx = i;
      break;
    } else {
      lidx = i;
      uidx = i;
    }
  }

  if (lidx >= (icc_size - 1)) { /* Extrapolate upwards */
    delta = icc_x[icc_size - 1] - icc_x[icc_size - 2];

    /* Calculate slope */
    slope = icc_y[icc_size - 1] - icc_y[icc_size - 2]; /* to be divided by delta! */

    /* Interpolate and divide by delta */
    x_tmp = (icc_x[icc_size - 1])*257 + ((icc_x[icc_size - 1] & (1<<7)) ? 1 : 0);
    y_tmp = (icc_y[icc_size - 1])*257 + ((icc_y[icc_size - 1] & (1<<7)) ? 1 : 0);

    y = ((pu - x_tmp) * slope) / delta + y_tmp;
  }
  else if (uidx <= 0) { /* Extrapolate downwards */
    delta = icc_x[1] - icc_x[0];

    /* Calculate slope */
    slope = icc_y[1] - icc_y[0]; /* to be divided by delta! */

    x_tmp = (icc_x[0])*257 + ((icc_x[0] & (1<<7)) ? 1 : 0);
    y_tmp = (icc_y[0])*257 + ((icc_y[0] & (1<<7)) ? 1 : 0);

    y = ((pu - x_tmp) * slope) / delta + y_tmp;
  }
  else { /* Interpolate */
    delta = icc_x[uidx] - icc_x[lidx];

    /* Calculate slope */
    slope = icc_y[uidx] - icc_y[lidx]; /* to be divided by delta! */

    x_tmp = (icc_x[lidx])*257 + ((icc_x[lidx] & (1<<7)) ? 1 : 0);
    y_tmp = (icc_y[lidx])*257 + ((icc_y[lidx] & (1<<7)) ? 1 : 0);

    y = ((pu - x_tmp) * slope) / delta + y_tmp;
  }

  /* Clip to [0, ONE] */
  y = (y < 0) ? 0 : (y > (1<<16)) ? (1<<16) : y;

  return y;
}

static void interpolate_initialize(sl_hdr1_metadata *sl_hdr1_metadata, int32_t *icc_x, int32_t *icc_y, int32_t *icc_size)
{
  MS_U32  i, j;


  if (sl_hdr1_metadata->u.variables.tmOutputFineTuningNumVal > 0)
    *icc_size = sl_hdr1_metadata->u.variables.tmOutputFineTuningNumVal;
  else
    *icc_size = 0;

  if ((sl_hdr1_metadata->u.variables.tmOutputFineTuningX[0] != 0 || sl_hdr1_metadata->u.variables.tmOutputFineTuningY[0] != 0) || *icc_size == 0) {
    icc_x[0] = icc_y[0] = 0;
    *icc_size += 1;
    i = 0;
  }
  else {
    icc_x[0] = sl_hdr1_metadata->u.variables.tmOutputFineTuningX[0];
    icc_y[0] = sl_hdr1_metadata->u.variables.tmOutputFineTuningY[0];
    i = 1;
  }

  j = 1;
  while (i < sl_hdr1_metadata->u.variables.tmOutputFineTuningNumVal) {
    icc_x[j] = sl_hdr1_metadata->u.variables.tmOutputFineTuningX[i];
    icc_y[j] = sl_hdr1_metadata->u.variables.tmOutputFineTuningY[i];
    i++;
    j++;
  }

  if (icc_x[*icc_size - 1] != 255 || icc_y[*icc_size - 1] != 255) {
    *icc_size += 1;
    icc_x[*icc_size - 1] = icc_y[*icc_size - 1] = 255;
  }

}

static uint64_t div_u64_u32(uint64_t dividend, uint32_t divisor)
{
  uint32_t high;
  uint64_t remainder, r, quotient, q;

  high = dividend >> 32;
  remainder = dividend;
  r = divisor;
  quotient = 0;
  q = 1;

  if (high >= divisor) {
    high /= divisor;
    quotient = (uint64_t)high << 32;
    remainder -= (uint64_t)(high * divisor) << 32;
  }

  while ((int64_t)r > 0 && r < remainder) {
    r <<= 1;
    q <<= 1;
  }

  do {
    if (remainder >= r) {
      remainder -= r;
      quotient += q;
    }
    r >>= 1;
    q >>= 1;
  } while (q);

  return quotient;
}

static int64_t div_s64_s32(int64_t x, int32_t y)
{
  uint32_t divisor;
  uint64_t dividend, quotient;

  dividend = x < 0 ? -x : x;
  divisor = y < 0 ? -y : y;

  quotient = div_u64_u32(dividend, divisor);

  if ((x < 0 && y > 0) || (x > 0 && y < 0))
    return -(int64_t)quotient;
  else
    return quotient;
}

static uint32_t fls_u32(uint32_t x)
{
  int r = 32;

  if (!x)
    return 0;
  if (!(x & 0xffff0000u)) {
    x <<= 16;
    r -= 16;
  }
  if (!(x & 0xff000000u)) {
    x <<= 8;
    r -= 8;
  }
  if (!(x & 0xf0000000u)) {
    x <<= 4;
    r -= 4;
  }
  if (!(x & 0xc0000000u)) {
    x <<= 2;
    r -= 2;
  }
  if (!(x & 0x80000000u)) {
    x <<= 1;
    r -= 1;
  }

  return r;
}

static uint64_t div_u64_u64(uint64_t dividend, uint64_t divisor)
{
  uint32_t high = divisor >> 32;
  uint64_t quotient;

  if (high == 0) {
    quotient = div_u64_u32(dividend, (uint32_t)divisor);
  } else {
    high = 1 + fls_u32(high);
    quotient = div_u64_u32(dividend >> high, (uint32_t)(divisor >> high));
    if (quotient != 0)
      quotient--;
    if ((dividend - quotient * divisor) >= divisor)
      quotient++;
  }

  return quotient;
}

static int64_t div_s64_s64(int64_t x, int64_t y)
{
  uint32_t divisor;
  uint64_t dividend, quotient;

  dividend = x < 0 ? -x : x;
  divisor = y < 0 ? -y : y;

  quotient = div_u64_u64(dividend, divisor);

  if ((x < 0 && y > 0) || (x > 0 && y < 0))
    return -(int64_t)quotient;
  else
    return quotient;
}


static void invparabola_initialize(sl_hdr1_metadata *sl_hdr1_metadata, int32_t *ipbl_base_gain, int32_t *ipbl_hgc, int32_t *ipbl_one_div_a, int32_t *ipbl_one_div_a_sh, int32_t *ipbl_min_b_dev_2a, int32_t *ipbl_lb_b2_min_4ac_rb_div_4a2, int32_t *ipbl_lb_b2_min_4ac_rb_div_4a2_shift, int32_t *ipbl_ylow, int32_t *ipbl_yhigh)
{
  enum { SHIFT = 16 };
  enum { TWO = (1<<(SHIFT+1)) };
  enum { ONE = (1<<SHIFT) };
  enum { HALF = (1<<(SHIFT-1)) };
  enum { QUART = (1<<(SHIFT-2)) };
  enum { PRECISION = 0 };
  int32_t Lhdr_idx = sl_hdr1_metadata->hdrMasterDisplayMaxLuminance / 50;
  Lhdr_idx = (Lhdr_idx >= 2) ? ((Lhdr_idx <= 200) ? Lhdr_idx : 200) : 2;
  //int32_t Lhdr_idx = (sl_hdr1_metadata->hdrMasterDisplayMaxLuminance >= 2) ? ((sl_hdr1_metadata->hdrMasterDisplayMaxLuminance <= 200) ? sl_hdr1_metadata->hdrMasterDisplayMaxLuminance : 200) : 2;
  int32_t exp_gain = ipbl_exp_gain[Lhdr_idx];
  int32_t base_gain = (((int64_t)exp_gain) * (HALF + ((((int32_t)sl_hdr1_metadata->u.variables.shadowGain)*257 + 1) >> 1)) + HALF) >> SHIFT;
  int32_t hgc = (((int32_t)sl_hdr1_metadata->u.variables.highlightGain)*257 + 1) >> 1;
  int32_t midadj = ((int32_t)sl_hdr1_metadata->u.variables.midToneWidthAdjFactor)*257 + ((sl_hdr1_metadata->u.variables.midToneWidthAdjFactor & 128) ? 1 : 0);
  int32_t one_minus_hgc = ONE - hgc;
  int32_t sh, pa = 0, pb = 0, pc4 = 0;
  int32_t xlow, xhigh, ylow, yhigh, den;
  int64_t tmp64, num64, den64;

  if (midadj ) {
    tmp64 = base_gain - hgc;
    tmp64 <<= (SHIFT<<1);
    tmp64 = div_s64_s32(tmp64, midadj);
    /* precision + 0 */
    tmp64 = (tmp64 + (ONE>>PRECISION)*0) >> (SHIFT+1-PRECISION);
    pa = -(int32_t)tmp64;

    /* tmp64 = ONE - hgc; */
    tmp64 = one_minus_hgc;
    tmp64 <<= (SHIFT<<1);
    tmp64 = div_s64_s32(tmp64, midadj);
    /* precision +2 */
    tmp64 = (tmp64 + HALF*0) >> (SHIFT-PRECISION);
    pb = (int32_t)tmp64;
    if (PRECISION <= 0) {
      pb += (base_gain + hgc + 1) >> (1 - PRECISION);
    }
    else {
      /* Construction to prevent compuiler warning about negative shift */
      pb += (base_gain + hgc) << (((PRECISION-1)>=0) ? (PRECISION-1) : 0);
    }

    /* Denumerator */
    den64 = (base_gain - hgc);
    den64 *= midadj;
    if (den64 == 0) {
      /* pc4 is always negative */
    pc4 = -2147483647 - 1;
       //pc4 = INT32_MIN;

    }
    else {
      /* Numerator */
      num64 = den64;
      num64 -= ((int64_t)(ONE - hgc))<<(SHIFT+1);
      /* Keep coming square within 64 bits (signed)! */
      num64 >>= 2;
      num64 *= num64;

      num64 = div_s64_s64(num64, den64);

      /* precision +2 */
      num64 >>= (SHIFT-1-2-PRECISION);

      /* pc is actually 4*pc */
      pc4 = (int32_t)(-num64);
    }
  }

  /* one_minus_hgc = (1.0 - hgc) is reused later */
  tmp64 = one_minus_hgc;
  tmp64 <<= (SHIFT<<1);

  /* 0.0 <= (base_gain - hgc) <= 1.0 */
  den = base_gain - hgc;
  den = (den == 0) ? 1 : den;
  tmp64 = div_s64_s32(tmp64, den);
  tmp64 =  (tmp64 + HALF) >> SHIFT;
  tmp64 = (tmp64 > ((1LL<<30)-1)) ? ((1<<30)-1) : tmp64;

  /* xlow = (1.0 - hgc) / (base_gain - hgc) - midadj / 2.0; */
  xlow = (int32_t)tmp64 - (midadj>>1);
  /* xhigh = (1.0 - hgc) / (base_gain - hgc) + midadj / 2.0; */
  xhigh = (int32_t)tmp64 + ((midadj + 1)>>1);

  /* ylow = base_gain * xlow; */
  tmp64 = base_gain;
  tmp64 *= xlow;
  ylow = (int32_t)((tmp64 + HALF) >> SHIFT);
  /* yhigh = hgc * xhigh + (1.0 - hgc); */
  tmp64 = hgc;
  tmp64 *= xhigh;
  yhigh = (int32_t)((tmp64 + HALF*0) >> SHIFT) ;
  yhigh += one_minus_hgc;
  /* HT 2016-05-10: limit yhigh to prevent deviations at max input */
  yhigh = (yhigh > ONE)?  ONE : yhigh;

  *ipbl_base_gain = base_gain;
  *ipbl_hgc = hgc;
  *ipbl_ylow = ylow;
  *ipbl_yhigh = yhigh;

  if (pa != 0) {
    /* ((b/2)^2/a - c)/a */
    /* pa is always less equal zero   */
    /* pb is always greater than zero */
    /* pc is always less than zero    */
    tmp64 = pb;
    tmp64 *= tmp64;
    tmp64 <<= (SHIFT-2);
    tmp64 = div_s64_s32(tmp64, pa);
    tmp64 -= (((int64_t)pc4) << (SHIFT-2));
    tmp64 <<= SHIFT;
    tmp64 = div_s64_s32(tmp64, pa);
    sh = 0;
    while ((tmp64 >> sh) >= (1LL<<31)) {
      sh += 2;
    }
    *ipbl_lb_b2_min_4ac_rb_div_4a2 = (int32_t)(tmp64 >> sh);
    *ipbl_lb_b2_min_4ac_rb_div_4a2_shift = 2 + (SHIFT-2) - sh;

    tmp64 = -pb;
    tmp64 <<= SHIFT;
    tmp64 = div_s64_s32(tmp64, pa);
    *ipbl_min_b_dev_2a = (((int32_t)tmp64) + 1) >> 1;

    tmp64 = (1LL << ((SHIFT<<2) - 2));
    tmp64 = div_s64_s32(tmp64, pa);
    sh = 0;
    while (((-tmp64) >> sh) >= (1LL<<31)) {
      sh += 1;
    }
    *ipbl_one_div_a = (int32_t)(tmp64 >> sh);
    /* precision pa is +2*/
    *ipbl_one_div_a_sh = (SHIFT<<1) - 2 - sh - PRECISION;
  }
  else {
    *ipbl_lb_b2_min_4ac_rb_div_4a2 = 0;
    *ipbl_min_b_dev_2a = 0;
    *ipbl_one_div_a = 0;
    *ipbl_one_div_a_sh = 0;
  }

  return;
}


static uint32_t sqrt16(uint32_t x)
{
  uint16_t op  = 1<<15;
  uint32_t res = 1<<15;

  for (;;) {
    if ((res*res) > x)
      res = res ^ op;

    op >>= 1;

    if (op == 0)
      return ((uint16_t)res);

    res = res | op;
  }

  return res;
}


static int32_t invparabola(int32_t icc, int32_t ipbl_base_gain, int32_t ipbl_hgc, int32_t ipbl_one_div_a, int32_t ipbl_one_div_a_sh, int32_t ipbl_min_b_dev_2a, int32_t ipbl_lb_b2_min_4ac_rb_div_4a2, int32_t ipbl_lb_b2_min_4ac_rb_div_4a2_shift, int32_t ipbl_ylow, int32_t ipbl_yhigh)
{
  enum {SHIFT = 16 };
  enum {ONE = (1<<SHIFT) };
  int32_t perceptualpbl, sh1 = 0, sh2 = 0, sh3 = 0;
  int32_t tmp1 = 0;
  int64_t tmp2 = 0, tmp641 = 0;

  if (icc >= ipbl_yhigh) { /* high */
    /* perceptualpbl = (perceptualccl + hgc - 1.0) / hgc; */
    perceptualpbl = (icc + ipbl_hgc - ONE) << (SHIFT-2);
    if (!perceptualpbl) { /* perceptualpbl == 0 */
      if (!ipbl_hgc) { /* hgc == 0           */
        perceptualpbl = ONE;
      }
    }
    else {
      /* Make sure denumerator cannot be zero */
      if (ipbl_hgc) { /* ipbl_hgc != 0 */
        perceptualpbl /= ipbl_hgc;
        perceptualpbl <<= 2;
      }
    }
  }
  else if (icc <= ipbl_ylow) { /* low */
    /* perceptualpbl =  perceptualccl / base_gain; */
    perceptualpbl =  icc << (SHIFT-2);
    /* Denumerator is always greater than zero */
    perceptualpbl /= ((ipbl_base_gain + 2)>>2);
  }
  else { /* mid */
    /* perceptualpbl = (-pb + sqrt(pb*pb - 4*pa*(pc-perceptualccl))) / (2*pa); */
    sh1 = ipbl_lb_b2_min_4ac_rb_div_4a2_shift;
    sh2 = ipbl_one_div_a_sh;
    tmp641 = ipbl_lb_b2_min_4ac_rb_div_4a2;
    tmp2 = ((int64_t)icc) * ipbl_one_div_a;
    tmp641 = tmp641 << (SHIFT-sh1+sh2);
    tmp641 += tmp2;

    /* Make sure tmp is within the set {0,1,...,(1<<16)-1} */
    if (tmp1 < 0) {
      /* Set to zero. Negative tmp1 is caused by inaccuracies in calculations. */
      tmp1 = 0;
    }
    else {
      /* Test sh2 odd */
      if (sh2 & 0x1) {
        /* sh2 odd -> make even before entering sqrt() */
        tmp641 >>= 1;
        --sh2;
      }

      /* Make sure tmp1 fits uint32_t before entering sqrt() */
      /* Increment by 2 since sqrt((x^2)>>2n) = x<<n         */
      sh3 = 0;
      while ((tmp641 >> sh3) >= (1LL<<32)) {
        /* increment by 2 */
        ++sh3;
        ++sh3;
      }
      tmp641 >>= sh3;

      /* Call to sqrt16() */
      tmp1 = (int32_t)sqrt16( (uint32_t)(((uint32_t)tmp641)) );

      /* Correct result of sqrt() */
      sh3 = sh2 - sh3;
      sh3 = ((sh3 >= 0) ? (sh3 >> 1) : -((-sh3)>>1));
      tmp1 = (sh3 >= 0) ? tmp1 >> ABS(sh3) : tmp1 << ABS(sh3);
    }

    /* Sign of tmp1 is dependent on pa, which is always negative */
    tmp1 = -tmp1;

    /* Add -pb/(2pa) to get final result */
    perceptualpbl = ipbl_min_b_dev_2a + tmp1;
  }

  /* Clip to [0, ONE] */
  perceptualpbl = (perceptualpbl > ONE) ? ONE : perceptualpbl;

  return perceptualpbl;
}


static void invBWstretch_initialize(sl_hdr1_metadata *sl_hdr1_metadata, int32_t *ibw_black_stretch, int32_t *ibw_white_stretch)
{
  *ibw_black_stretch = (((sl_hdr1_metadata->u.variables.tmInputSignalBlackLevelOffset) * 257) >> 3) + ((sl_hdr1_metadata->u.variables.tmInputSignalBlackLevelOffset & 4) ? 1 : 0);
  *ibw_white_stretch = ((sl_hdr1_metadata->u.variables.tmInputSignalWhiteLevelOffset) * 257 + 1) >> 1;

  return;
}

static uint16_t invbwstretch(uint32_t ipbl, int32_t black_stretch, int32_t white_stretch)
{
  enum { SHIFT = 16 };
  enum { ONE = (1 << SHIFT) };
  enum { HALF = ONE >> 1 };
  uint32_t perceptualstretchedl;

  /* Make sure the product fits always in uint32_t */
  perceptualstretchedl = (ONE - white_stretch - black_stretch) >> 2;
  perceptualstretchedl *= ipbl;
  perceptualstretchedl = (perceptualstretchedl + (HALF>>2)) >> (SHIFT-2);
  perceptualstretchedl += black_stretch;

  /* Clip result to [0, ONE] */
  perceptualstretchedl = (uint16_t)((perceptualstretchedl < 0) ? 0 : ((perceptualstretchedl > (ONE-1)) ? (ONE-1) : perceptualstretchedl));

  return perceptualstretchedl;
}

static void invBGLimiter_initialize(sl_hdr1_metadata *sl_hdr1_metadata, int32_t *ibgl_alpha, int32_t *ibgl_bypass)
{
  //*ibgl_alpha = ibgl_one_over_black_gain_limiter_mix_factor[sl_hdr1_metadata->hdrMasterDisplayMaxLuminance];
  int32_t Lhdr_idx = sl_hdr1_metadata->hdrMasterDisplayMaxLuminance / 50;
  *ibgl_alpha = ibgl_one_over_black_gain_limiter_mix_factor[Lhdr_idx];
  *ibgl_bypass = (sl_hdr1_metadata->u.variables.tmInputSignalBlackLevelOffset == 0) ? 1 : 0;

  return;
}

static int32_t invBGLimiter(int32_t pu, int32_t ibw, int32_t ibgl_bypass, int32_t ibgl_alpha)
{
  int32_t res;

  if (ibgl_bypass == 0)
  {
    res = pu * ibgl_alpha;
    res += 1<<13;
    res >>= 14;
    res = (res > ibw) ? ibw : res;
  }
  else {
    res = ibw;
  }

  return res;
}

static void EOTF_sqrt_initialize(sl_hdr1_metadata *sl_hdr1_metadata, uint16_t *sqrtY_alpha, int32_t *sqrtY_is_leq_1000nit)
{

 int32_t Lhdr_idx = sl_hdr1_metadata->hdrMasterDisplayMaxLuminance / 50;

  /* Set mix factor alpha */
  *sqrtY_alpha = EOTF_sqrt_alpha[Lhdr_idx-2];
  /* Used to select either 100 and 1000 nit or 1000 nit and 10000 nit L curves */
  *sqrtY_is_leq_1000nit = (Lhdr_idx <= 20) ? 1 : 0;

  return;
}

static uint16_t quadrint(uint16_t EOETF_base[64], uint16_t EOETF_slope[64], uint8_t EOETF_bend[64], uint8_t EOETF_shl[64], uint16_t Ei)
{
    uint8_t  addr;
    uint16_t delta;
    int32_t Lr;
    uint16_t Li;

    addr  = Ei >> 10;
    delta = Ei  & 0x03ff;
    Lr = (EOETF_base[addr] << 2) + ((delta * ((EOETF_slope[addr] << 1) - (((1023 - delta) * EOETF_bend[addr]) >> 9))) >> 9);
    Li = (Lr > 0) ? ((Lr << ((uint8_t)EOETF_shl[addr] << 1)) + 2) >> 2 : 0;

    return Li;
}

static int32_t EOTF_sqrt(int32_t ibgl, uint16_t sqrtY_alpha, int32_t sqrtY_is_leq_1000nit)
{
  uint16_t x1, x2;
  int32_t res;

  /* Quadratic interpolation at L_HDR = 1000 nit as function of ibgl */
  x2 = quadrint(EOETF_1000_base, EOETF_1000_slope, EOETF_1000_bend, EOETF_1000_shl, ibgl);
  if (sqrtY_is_leq_1000nit) {
    /* Quadratic interpolation at L_HDR = 100 nit as function of ibgl */
    x1 = quadrint(EOETF_100_base, EOETF_100_slope, EOETF_100_bend, EOETF_100_shl, ibgl);
    /* Linear interpolation between 100 and 1000 nit by use of private alpha_ (determined during initialization) */
    res = (sqrtY_alpha * x2 + ((1<<12) - sqrtY_alpha) * x1) >> 12;
  }
  else {
    /* Quadratic interpolation at L_HDR = 10000 nit as function of ibgl */
    x1 = quadrint(EOETF_10000_base, EOETF_10000_slope, EOETF_10000_bend, EOETF_10000_shl, ibgl);
    /* Linear interpolation between 100 and 1000 nit by use of private alpha_ (determined during initialization) */
    res = (sqrtY_alpha * x1 + ((1<<12) - sqrtY_alpha) * x2) >> 12;
  }

  return res;
}
void Compute_P_LUT(sl_hdr1_metadata *sl_hdr1_metadata, uint16_t *P_LUT)
{
  uint32_t Lhdr =  sl_hdr1_metadata->hdrMasterDisplayMaxLuminance;
  uint32_t Lhdr_rel2abs_factor = sqrt16(Lhdr << 18);
  int32_t icc_size;
  int32_t ipbl_base_gain, ipbl_hgc, ipbl_one_div_a, ipbl_one_div_a_sh, ipbl_min_b_dev_2a, ipbl_lb_b2_min_4ac_rb_div_4a2, ipbl_lb_b2_min_4ac_rb_div_4a2_shift, ipbl_ylow, ipbl_yhigh;
  int32_t ibw_black_stretch, ibw_white_stretch;
  int32_t ibgl_bypass, ibgl_alpha;
  uint16_t sqrtY_alpha;
  int32_t sqrtY_is_leq_1000nit;
  int32_t i, j, pu, icc, ipbl, ibw, ibgl, sqrtY;

  if (!sl_hdr1_metadata->payloadMode) {
    interpolate_initialize(sl_hdr1_metadata, icc_x, icc_y, &icc_size);
    invparabola_initialize(sl_hdr1_metadata, &ipbl_base_gain, &ipbl_hgc, &ipbl_one_div_a, &ipbl_one_div_a_sh, &ipbl_min_b_dev_2a, &ipbl_lb_b2_min_4ac_rb_div_4a2, &ipbl_lb_b2_min_4ac_rb_div_4a2_shift, &ipbl_ylow, &ipbl_yhigh);
    invBWstretch_initialize(sl_hdr1_metadata, &ibw_black_stretch, &ibw_white_stretch);
    invBGLimiter_initialize(sl_hdr1_metadata, &ibgl_alpha, &ibgl_bypass);
    EOTF_sqrt_initialize(sl_hdr1_metadata, &sqrtY_alpha, &sqrtY_is_leq_1000nit);

    for (i = 0; i < 65; ++i) {
      /* Gamma and OETF combination */
      pu = Gamma2OETF[i];

      /* Inverse Custom Curve */
      icc = interpolate(pu, icc_x, icc_y, icc_size);

      /* Inverse Parabola */
      ipbl = invparabola(icc, ipbl_base_gain, ipbl_hgc, ipbl_one_div_a, ipbl_one_div_a_sh, ipbl_min_b_dev_2a, ipbl_lb_b2_min_4ac_rb_div_4a2, ipbl_lb_b2_min_4ac_rb_div_4a2_shift, ipbl_ylow, ipbl_yhigh);

      /* Inverse Black & White stretch */
      ibw = invbwstretch(ipbl, ibw_black_stretch, ibw_white_stretch);

      /* Inverse Black gain limiting */
      ibgl = invBGLimiter(pu, ibw, ibgl_bypass, ibgl_alpha);

      /* EOTF and square root combination */
      sqrtY = EOTF_sqrt(ibgl, sqrtY_alpha, sqrtY_is_leq_1000nit);

      /* Conversion from relative to absolute max brightness domain */
      P_LUT[i] = (sqrtY * Lhdr_rel2abs_factor + (1<<(16+2))) >> (16 + 3);

      /* Scaling factor : P_LUT[i] = (P_LUT[i] * 8191 /(64 * sqrt(Lhdr)) + 0.5) */
      switch (Lhdr)
      {
        case 1000:
          P_LUT[i] = (P_LUT[i] * 8191 + 1012) / 2024;
          break;
        case 1200:
          P_LUT[i] = (P_LUT[i] * 8191 + 1109) / 2217;
          break;
        case 1500:
          P_LUT[i] = (P_LUT[i] * 8191 + 1239) / 2479;
          break;
        case 2000:
          P_LUT[i] = (P_LUT[i] * 8191 + 1431) / 2862;
          break;
        default:
          printf("[TCH Prime] Can only handle peak luminance 1000, 1200, 1500 and 2000 nits\n");
          break;
      }
    }
  }
  else {
    for(i = 0; i < 32; i++){
      for(j = 0; j < 2; j++){
        P_LUT[2 * i + j] = ((2 - j) * sl_hdr1_metadata->u.tables.luminanceMappingY[i] + j * sl_hdr1_metadata->u.tables.luminanceMappingY[i+1] + 1) >> 1;
      }
    }
    P_LUT[64] = sl_hdr1_metadata->u.tables.luminanceMappingY[32];
  }
}

void Mhal_TCH_UpdateStatus(sl_hdr1_metadata *sl_hdr1_metadata_tch, STU_CFDAPI_MM_PARSER *PrimeSingle_result)
{
    PrimeSingle_result->u32Master_Panel_Max_Luminance=sl_hdr1_metadata_tch->hdrMasterDisplayMaxLuminance * 10000; //check unit
    //PrimeSingle_result->u32Master_Panel_Max_Luminance = 2000*10000;
    if (sl_hdr1_metadata_tch->hdrMasterDisplayMinLuminance == 0)
        //PrimeSingle_result->u32Master_Panel_Min_Luminance = 5000;
        PrimeSingle_result->u32Master_Panel_Min_Luminance = 1;
    else
        PrimeSingle_result->u32Master_Panel_Min_Luminance = sl_hdr1_metadata_tch->hdrMasterDisplayMinLuminance; //check unit
    PrimeSingle_result->u8Colour_primaries=(sl_hdr1_metadata_tch->hdrPicColourSpace==1 ) ? 9 : 1 ; //0:709(1); 1:2020(9)
    PrimeSingle_result->u8Matrix_Coeffs = (sl_hdr1_metadata_tch->hdrPicColourSpace == 1) ? 9 : 1 ; //0:709(1); 1:2020(9)
    PrimeSingle_result->u8Transfer_Characteristics = 16; //16   SMPTE ST2084 for 10.12.14.16-bit systetm
    PrimeSingle_result->u8Video_Full_Range_Flag =0; //limit Range

    if(sl_hdr1_metadata_tch->hdrPicColourSpace ==0)//709
    {
#if 0 //G->B->R
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0]=15000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0]=30000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1]=7500;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1]=3000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2]=32000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2]=16500;
#else //R->G->B
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0]=32000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0]=16500;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1]=15000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1]=30000;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2]=7500;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2]=3000;
#endif
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_x = 15635;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_y = 16450;
    }
    else //2020
    {
#if 0 //G->B->R
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0]=8500;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0]=39850;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1]=6550;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1]=2300;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2]=35400;
        PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2]=14600;
#else //R->G->B
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0]=35400;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0]=14600;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1]=8500;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1]=39850;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2]=6550;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2]=2300;
    #endif
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_x = 15635;
    PrimeSingle_result->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_y = 16450;

    }

    PrimeSingle_result->u8Mastering_Display_Infor_Valid =1;
    PrimeSingle_result->u8MM_HDR_ContentLightMetaData_Valid =0;
    PrimeSingle_result->u16Max_content_light_level =0;
    PrimeSingle_result->u16Max_pic_average_light_level=0;

}


MS_U8 Mhal_TCH_Caculation(sl_hdr1_metadata *psl_hdr1_metadata_tch)
{
    MS_U8 u8Success = 0;
    sl_hdr1_metadata sl_hdr1_metadata_tch;
    memset(&sl_hdr1_metadata_tch,0,sizeof(sl_hdr1_metadata));
    memcpy(&sl_hdr1_metadata_tch,psl_hdr1_metadata_tch,sizeof(sl_hdr1_metadata));

     //STU_CFDAPI_MM_PARSER PrimeSingle_result;
     //memset(&PrimeSingle_result,0,sizeof(PrimeSingle_result));
     //memcpy(&PrimeSingle_result,pPrimeSingle_result,sizeof(PrimeSingle_result));

    //if (msReadByte(REG_SC_YAHAN_BK71_08_L) == 0)
    if(1)
    {
        //intk("\n [Yahan][ %s  , %d ]  TCH Prime on  !!!\n",__FUNCTION__,__LINE__);
        //TCH_W2BYTEMSK(REG_SC_YAHAN_BK71_01_L, 0x0001, 0x0001 );
        TCH_W2BYTE(REG_SC_YAHAN_BK71_01_L, 0x0001);

        int display_EOTF = 1; //display_EOTF : 0 = BT1886, 1 = PQ, 2 = BT709, 3 = ARIB
        MS_U32 tmo_reg;
        MS_U8 idx =0;
        int *OCT;
        int OCT_709[7]  = {32, 70, 175, 403, -48, -120, 475};
        int OCT_2020[7] = {33, 58, 203, 377, -42, -146, 482};

        Compute_muA_muB(&sl_hdr1_metadata_tch, &muA, &muB);
        Compute_P_LUT(&sl_hdr1_metadata_tch, P_LUT);
        Compute_C_LUT(&sl_hdr1_metadata_tch, INV_C_LUT, C_LUT);

        Fill_D_LUT(display_EOTF, sl_hdr1_metadata_tch.hdrMasterDisplayMaxLuminance, D_LUT, D_LUT_THRESHOLD, D_LUT_STEP);

        // OCT matrix pointer
        if (sl_hdr1_metadata_tch.hdrPicColourSpace == 0) // rec709
            OCT = OCT_709;
    else // rec2020
    {
        OCT = OCT_2020;
    }

    TCH_W2BYTE(REG_SC_YAHAN_BK71_02_L,0x0000);

    TCH_W2BYTE(REG_SC_YAHAN_BK71_10_L,0xE030);//R2Y (fullR to limitY), dither rounding enable

    if( sl_hdr1_metadata_tch.hdrPicColourSpace == 0) // (fullR to limitY)709
    {
        TCH_W2BYTE(REG_SC_YAHAN_BK71_11_L, 0x01C0);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_12_L, 0x1E69);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_13_L, 0x1FD7);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_14_L, 0x00BA);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_15_L, 0x0273);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_16_L, 0x003F);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_17_L, 0x1F99);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_18_L, 0x1EA6);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_19_L, 0x01C0);
    }
    else if ( sl_hdr1_metadata_tch.hdrPicColourSpace == 1) //(fullR to limitY)2020
    {
        TCH_W2BYTE(REG_SC_YAHAN_BK71_11_L, 0x01C0);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_12_L, 0x1E64);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_13_L, 0x1FDC);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_14_L, 0x00E6);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_15_L, 0x0253);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_16_L, 0x0034);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_17_L, 0x1F83);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_18_L, 0x1EBD);
        TCH_W2BYTE(REG_SC_YAHAN_BK71_19_L, 0x01C0);
    }

   //YUV input range
    if (sl_hdr1_metadata_tch.limit_range_en == 0) //limit range
    {
        TCH_W2BYTE(REG_SC_YAHAN_BK71_25_L, 0x0040);  //footroom_c
        TCH_W2BYTE(REG_SC_YAHAN_BK71_26_L, 0x0040);  //footroom_y
        TCH_W2BYTE(REG_SC_YAHAN_BK71_28_L, 0x0491); //inv_ratio_c
        TCH_W2BYTE(REG_SC_YAHAN_BK71_29_L, 0x04AC); //inv_ratio_y
    }
    else //full range
    {
        TCH_W2BYTE(REG_SC_YAHAN_BK71_25_L, 0x0000);  //footroom_c
        TCH_W2BYTE(REG_SC_YAHAN_BK71_26_L, 0x0000);  //footroom_y
        TCH_W2BYTE(REG_SC_YAHAN_BK71_28_L, 0x0400); //inv_ratio_c
        TCH_W2BYTE(REG_SC_YAHAN_BK71_29_L, 0x0400); //inv_ratio_y
    }

    for( idx =0; idx < 7 ; idx++)
    {
        tmo_reg = (0x130000 | ((MS_U16)(0x71) << 8) | (MS_U16)((0x30+idx)*2));
        TCH_W2BYTE(tmo_reg,OCT[idx]);
    }

    TCH_W2BYTE(REG_SC_YAHAN_BK71_20_L,muA);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_21_L,muB);

 //fill dlut setting
    TCH_W2BYTE(REG_SC_YAHAN_BK71_2a_L,D_LUT_THRESHOLD[0]);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_2b_L,D_LUT_THRESHOLD[1]);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_2c_L,D_LUT_THRESHOLD[2]);

    TCH_W2BYTE(REG_SC_YAHAN_BK71_40_L,D_LUT_STEP[0]);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_41_L,D_LUT_STEP[1]);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_42_L,D_LUT_STEP[2]);
    TCH_W2BYTE(REG_SC_YAHAN_BK71_43_L,D_LUT_STEP[3]);

    TCH_W2BYTE(REG_SC_YAHAN_BK71_02_L,0x0001);

    //fill clut
       TCH_W2BYTE(REG_SC_YAHAN_BK72_02_L,0x0000);
       for( idx =0; idx < 65 ; idx++)
       {
         tmo_reg = (0x130000 | ((MS_U16)(0x72) << 8) | (MS_U16)((0x10+idx)*2));
         TCH_W2BYTE(tmo_reg,C_LUT[idx]);
       }
       TCH_W2BYTE(REG_SC_YAHAN_BK72_02_L,0x0001);

        //fill plut
        TCH_W2BYTE(REG_SC_YAHAN_BK73_02_L,0x0000);
        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x73) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,P_LUT[idx]);
        }
        TCH_W2BYTE(REG_SC_YAHAN_BK73_02_L,0x0001);

        //fill dlut0
        TCH_W2BYTE(REG_SC_YAHAN_BK76_02_L,0x0000);

        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x76) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,D_LUT[idx]);
        }
        TCH_W2BYTE(REG_SC_YAHAN_BK76_02_L,0x0001);

        //fill dlut1
        TCH_W2BYTE(REG_SC_YAHAN_BK77_02_L,0x0000);

        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x77) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,D_LUT[idx]);
        }
        TCH_W2BYTE(REG_SC_YAHAN_BK77_02_L,0x0001);

        //fill dlut1
        TCH_W2BYTE(REG_SC_YAHAN_BK78_02_L,0x0000);

        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x78) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,D_LUT[idx]);
        }
        TCH_W2BYTE(REG_SC_YAHAN_BK78_02_L,0x0001);

         //FIX
        //fill slut
        TCH_W2BYTE(REG_SC_YAHAN_BK74_02_L,0x0000);

        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x74) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,S_LUT[idx]);
        }
        TCH_W2BYTE(REG_SC_YAHAN_BK74_02_L,0x0001);

        //fill ilut
        TCH_W2BYTE(REG_SC_YAHAN_BK75_02_L,0x0000);

        for( idx =0; idx < 65 ; idx++)
        {
            tmo_reg = (0x130000 | ((MS_U16)(0x75) << 8) | (MS_U16)((0x10+idx)*2));
            TCH_W2BYTE(tmo_reg,I_LUT[idx]);
        }

#if 0 //Test write OK
        static MS_U8 tchct = 0;
        if(tchct == 0)
        {
            TCH_W2BYTE(0x130680,0xBEEF);
        }
        else if(tchct == 1)
        {
            TCH_W2BYTE(0x130682,0xBEEF);
        }
        else if(tchct == 2)
        {
            TCH_W2BYTE(0x130684,0xBEEF);
        }
        else if(tchct == 3)
        {
            TCH_W2BYTE(0x130686,0xBEEF);
        }
        tchct++;
#endif


        u8Success = 1;
        return u8Success;
    }
    else
    {
        printk("\n  [Yahan][ %s  , %d ]  TCH Prime off  !!!\n",__FUNCTION__,__LINE__);
        u8Success = 0;
    }
}

#define DBGMETADATA0(x) //x
#define DBGMETADATA1(x) //x

MS_U8 Mhal_TCH_MetaData_Check(sl_hdr1_metadata *sl_hdr1_metadata_tch)
{
    MS_U8 u8check = 0;
    MS_U8 idx = 0;

    if (sl_hdr1_metadata_tch->specVersion !=0 )
    {
        u8check = 1;
        printk("Technicolor Meta data : specVersion = %d  is wrong %s %d\n",sl_hdr1_metadata_tch->specVersion,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->payloadMode !=0 && sl_hdr1_metadata_tch->payloadMode !=1  )
    {
        u8check = 1;
        printk("Technicolor Meta data : payloadMode = %d  is wrong %s %d\n",sl_hdr1_metadata_tch->payloadMode ,__FUNCTION__,__LINE__);
    }


    if (sl_hdr1_metadata_tch->hdrPicColourSpace !=0 && sl_hdr1_metadata_tch->hdrPicColourSpace !=1 )
    {
        u8check = 1;
        printk("Technicolor Meta data : hdrPicColourSpace = %d  is wrong [%s %d]\n", sl_hdr1_metadata_tch->hdrPicColourSpace,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->hdrMasterDisplayColourSpace !=0 && sl_hdr1_metadata_tch->hdrMasterDisplayColourSpace !=1)
    {
        u8check = 1;
        printk("Technicolor Meta data : hdrMasterDisplayColourSpace = %d  is wrong [%s %d]\n",sl_hdr1_metadata_tch->hdrMasterDisplayColourSpace,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->hdrMasterDisplayMaxLuminance < 0 || sl_hdr1_metadata_tch->hdrMasterDisplayMaxLuminance > 10000)
    {
        u8check = 1;
        printk("Technicolor Meta data : hdrMasterDisplayMaxLuminance = %d  is out of range [%s %d]\n",sl_hdr1_metadata_tch->hdrMasterDisplayMaxLuminance,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->hdrMasterDisplayMinLuminance < 0 ||sl_hdr1_metadata_tch->hdrMasterDisplayMinLuminance > 10000)
    {
        u8check = 1;
        printk("Technicolor Meta data : hdrMasterDisplayMinLuminance = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->hdrMasterDisplayMinLuminance,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->sdrPicColourSpace !=0 && sl_hdr1_metadata_tch->sdrPicColourSpace !=1 )
    {
         u8check = 1;
         printk("Technicolor Meta data : sdrPicColourSpace = %d  is wrong %s %d\n",sl_hdr1_metadata_tch->sdrPicColourSpace ,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->sdrMasterDisplayColourSpace !=0 &&sl_hdr1_metadata_tch->sdrMasterDisplayColourSpace !=1)
    {
         u8check = 1;
         printk("Technicolor Meta data : sdrMasterDisplayColourSpace = %d  is wrong %s %d\n",sl_hdr1_metadata_tch->sdrMasterDisplayColourSpace,__FUNCTION__,__LINE__);
    }

    //payload type =0
#if 0
    if (sl_hdr1_metadata_tch->u.variables.tmInputSignalBlackLevelOffset < 0|| sl_hdr1_metadata_tch->u.variables.tmInputSignalBlackLevelOffset >255)
    {
        u8check = 1;
        printk("Technicolor Meta data : tmInputSignalBlackLevelOffset  = %d is out of range  %s %d\n",sl_hdr1_metadata_tch->u.variables.tmInputSignalBlackLevelOffset ,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.tmInputSignalWhiteLevelOffset< 0 || sl_hdr1_metadata_tch->u.variables.tmInputSignalWhiteLevelOffset>255)
    {
         u8check = 1;
         printk("Technicolor Meta data : tmInputSignalWhiteLevelOffset  = %d  is out of range%s %d\n",sl_hdr1_metadata_tch->u.variables.tmInputSignalWhiteLevelOffset,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.shadowGain< 0 || sl_hdr1_metadata_tch->u.variables.shadowGain>255)
    {
         u8check = 1;
         printk("Technicolor Meta data : shadowGain  = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->u.variables.shadowGain,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.highlightGain< 0 || sl_hdr1_metadata_tch ->u.variables.highlightGain>255)
    {
         u8check = 1;
         printk("Technicolor Meta data : highlightGain  = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->u.variables.highlightGain,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.midToneWidthAdjFactor< 0 || sl_hdr1_metadata_tch->u.variables.midToneWidthAdjFactor>255)
    {
         u8check = 1;
         printk("Technicolor Meta data : midToneWidthAdjFactor  = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->u.variables.midToneWidthAdjFactor,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.tmOutputFineTuningNumVal != 0) // range 0~10, but current value = 0
    {
         u8check = 1;
         printk("Technicolor Meta data : tmOutputFineTuningNumVal  = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->u.variables.tmOutputFineTuningNumVal,__FUNCTION__,__LINE__);
    }

    if (sl_hdr1_metadata_tch->u.variables.saturationGainNumVal< 0 || sl_hdr1_metadata_tch->u.variables.saturationGainNumVal>6)
    {
         u8check = 1;
         printk("Technicolor Meta data : saturationGainNumVal  = %d  is out of range %s %d\n",sl_hdr1_metadata_tch->u.variables.saturationGainNumVal,__FUNCTION__,__LINE__);
    }

    for ( idx=0; idx <sl_hdr1_metadata_tch->u.variables.saturationGainNumVal; idx++ )
    {
        if (sl_hdr1_metadata_tch->u.variables.saturationGainX[idx]< 0 || sl_hdr1_metadata_tch->u.variables.saturationGainX[idx]>255)
        {
            u8check = 1;
            printk("Technicolor Meta data : saturationGainX[%d]  = %d   is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.variables.saturationGainX[idx],__FUNCTION__,__LINE__);
        }
        if (sl_hdr1_metadata_tch->u.variables.saturationGainY[idx]< 0 || sl_hdr1_metadata_tch->u.variables.saturationGainY[idx]>255)
        {
            u8check = 1;
            printk("Technicolor Meta data : saturationGainY[%d]  = %d   is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.variables.saturationGainY[idx],__FUNCTION__,__LINE__);
        }
    }
#endif

    //payload type =1
#if 0
    if (sl_hdr1_metadata_tch->u.tables.luminanceMappingNumVal < 0 || sl_hdr1_metadata_tch->u.tables.luminanceMappingNumVal >33)
    {
         u8check = 1;
          printk("Technicolor Meta data : luminanceMappingNumVal  = %d  is out of range  %s %d\n",sl_hdr1_metadata_tch->u.tables.luminanceMappingNumVal,__FUNCTION__,__LINE__);
    }

    for ( idx=0; idx <sl_hdr1_metadata_tch->u.tables.luminanceMappingNumVal; idx++ )
    {
        if (sl_hdr1_metadata_tch->u.tables.luminanceMappingX[idx]< 0 || sl_hdr1_metadata_tch->u.tables.luminanceMappingX[idx]>16383)
        {
            u8check = 1;
            printk("Technicolor Meta data : luminanceMappingX[%d]  = %d  is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.tables.luminanceMappingX[idx],__FUNCTION__,__LINE__);
        }
        if (sl_hdr1_metadata_tch->u.tables.luminanceMappingY[idx]< 0 || sl_hdr1_metadata_tch->u.tables.luminanceMappingY[idx]>8192)
        {
            u8check = 1;
            printk("Technicolor Meta data : luminanceMappingY[%d]   = %d  is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.tables.luminanceMappingY[idx],__FUNCTION__,__LINE__);
        }
    }

     if (sl_hdr1_metadata_tch->u.tables.colourCorrectionNumVal < 0 || sl_hdr1_metadata_tch->u.tables.colourCorrectionNumVal >33)
    {
         u8check = 1;
         printk("Technicolor Meta data : colourCorrectionNumVal  = %d  is out of range  %s %d\n",sl_hdr1_metadata_tch->u.tables.colourCorrectionNumVal,__FUNCTION__,__LINE__);
    }

    for ( idx=0; idx <sl_hdr1_metadata_tch->u.tables.luminanceMappingNumVal; idx++ )
    {
        if (sl_hdr1_metadata_tch->u.tables.colourCorrectionX[idx]< 0 || sl_hdr1_metadata_tch->u.tables.colourCorrectionX[idx]>2047)
        {
            u8check = 1;
            printk("Technicolor Meta data : colourCorrectionX[%d]  = %d  is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.tables.colourCorrectionX[idx],__FUNCTION__,__LINE__);
        }
        if (sl_hdr1_metadata_tch->u.tables.colourCorrectionY[idx]< 0 || sl_hdr1_metadata_tch->u.tables.colourCorrectionY[idx]>2047)
        {
            u8check = 1;
            printk("Technicolor Meta data : colourCorrectionY[%d]  = %d  is out of range %s %d\n",idx,sl_hdr1_metadata_tch->u.tables.colourCorrectionY[idx],__FUNCTION__,__LINE__);
        }
    }

    if (sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuA < 0 || sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuA >8191)
    {
         u8check = 1;
         printk("Technicolor Meta data : chromaToLumaInjectionMuA  = %d  is out of range  %s %d\n",sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuA,__FUNCTION__,__LINE__);
    }
    if (sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuB < 0 || sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuB >8191)
    {
         u8check = 1;
         printk("Technicolor Meta data : chromaToLumaInjectionMuB  = %d  is out of range  %s %d\n",sl_hdr1_metadata_tch->u.tables.chromaToLumaInjectionMuB,__FUNCTION__,__LINE__);
    }
#endif
    return u8check;

}
void Mdrv_TCH_handler(sl_hdr1_metadata *pst_hdr1_metadata_tch)
{

   // STU_CFDAPI_MM_PARSER PrimeSingle_result;

    MS_U8 u8MetaDataError = 0;
    if(pst_hdr1_metadata_tch == NULL)
    {
        sl_hdr1_metadata sl_hdr1_metadata_tch;
        sl_hdr1_metadata_tch.specVersion =0;
        sl_hdr1_metadata_tch.payloadMode =0;
        sl_hdr1_metadata_tch.limit_range_en = 1;
        sl_hdr1_metadata_tch.hdrPicColourSpace =0;
        sl_hdr1_metadata_tch.hdrMasterDisplayColourSpace =0;
        sl_hdr1_metadata_tch.hdrMasterDisplayMaxLuminance = 1000;
        sl_hdr1_metadata_tch.hdrMasterDisplayMinLuminance =0;
        sl_hdr1_metadata_tch.sdrPicColourSpace =0;
        sl_hdr1_metadata_tch.sdrMasterDisplayColourSpace =0;
        sl_hdr1_metadata_tch.u.variables.tmInputSignalBlackLevelOffset = 101;
        sl_hdr1_metadata_tch.u.variables.tmInputSignalWhiteLevelOffset = 3;
        sl_hdr1_metadata_tch.u.variables.shadowGain = 88;
        sl_hdr1_metadata_tch.u.variables.highlightGain = 157;
        sl_hdr1_metadata_tch.u.variables.midToneWidthAdjFactor = 0;
        sl_hdr1_metadata_tch.u.variables.tmOutputFineTuningNumVal =0;
        sl_hdr1_metadata_tch.u.variables.saturationGainNumVal =6;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[0] = 0;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[0] = 64;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[1] = 5;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[1] = 64;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[2] = 245;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[2] = 64;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[3] = 249;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[3] = 64;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[4] = 253;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[4] = 64;
        sl_hdr1_metadata_tch.u.variables.saturationGainX[5] = 255;
        sl_hdr1_metadata_tch.u.variables.saturationGainY[5] = 64;

        u8MetaDataError = Mhal_TCH_MetaData_Check(&sl_hdr1_metadata_tch);
        if(u8MetaDataError == 1)
        {
            //Meta data is wrong!!
            printk("Technicolor Meta data is wrong!!%s %d\n",__FUNCTION__,__LINE__);
            return;
        }
        else
        {
            Mhal_TCH_Caculation(&sl_hdr1_metadata_tch);
            Mhal_TCH_UpdateStatus(&sl_hdr1_metadata_tch,&PrimeSingle_result);
        }
    }
    else
    {
        u8MetaDataError = Mhal_TCH_MetaData_Check(pst_hdr1_metadata_tch);
        if(u8MetaDataError == 1)
        {
            //Meta data is wrong!!
            printk("Technicolor Meta data is wrong!!%s %d\n",__FUNCTION__,__LINE__);
            return;
        }
        else
        {
            DBGMETADATA0(printk("TCH calc %d %d %d %d %d %d %d %d %d\n",pst_hdr1_metadata_tch->specVersion,pst_hdr1_metadata_tch->payloadMode,
                pst_hdr1_metadata_tch->hdrPicColourSpace,pst_hdr1_metadata_tch->hdrMasterDisplayColourSpace,
                pst_hdr1_metadata_tch->sdrPicColourSpace,pst_hdr1_metadata_tch->sdrMasterDisplayColourSpace,
                pst_hdr1_metadata_tch->hdrMasterDisplayMaxLuminance,pst_hdr1_metadata_tch->hdrMasterDisplayMinLuminance,
                pst_hdr1_metadata_tch->limit_range_en));

            DBGMETADATA0(printk("TCH CALC %d %d %d %d %d %d %d %d %d\n",pst_hdr1_metadata_tch->u.variables.tmInputSignalBlackLevelOffset,
                pst_hdr1_metadata_tch->u.variables.tmInputSignalWhiteLevelOffset,
                pst_hdr1_metadata_tch->u.variables.shadowGain,
                pst_hdr1_metadata_tch->u.variables.highlightGain,
                pst_hdr1_metadata_tch->u.variables.midToneWidthAdjFactor,
                pst_hdr1_metadata_tch->u.variables.tmOutputFineTuningNumVal,
                pst_hdr1_metadata_tch->u.variables.saturationGainNumVal,
                pst_hdr1_metadata_tch->u.variables.saturationGainX[0],
                pst_hdr1_metadata_tch->u.variables.saturationGainY[0]));

            Mhal_TCH_Caculation(pst_hdr1_metadata_tch);
            Mhal_TCH_UpdateStatus(pst_hdr1_metadata_tch,&PrimeSingle_result);

        }
    }

}


//[DS]
void TCH_Prepare_DS(MS_U8 u8Version)
{
    //_u8Version = u8Version;
    memset(&_stTCH_DS_CmdCnt, 0, sizeof(K_XC_DS_CMDCNT));
}

MS_U16 TCH_GetCmdCnt(void)
{
    return _stTCH_DS_CmdCnt.u16CMDCNT_IPM;
}

MS_BOOL SetTCHParameter(MS_BOOL enT35, MS_U8 *pu8T35Data, MS_BOOL enCR, MS_U8 *pu8CRData, sl_hdr1_metadata *pstTCHMetadata)
{
    if(pu8T35Data == NULL)
    {
        printk("SetTCHParameter pu8T35Data NULL\n");
        return FALSE;
    }
    if(enT35 == FALSE)
    {
        printk("enT35 FALSE!!!!!!!\n");
        return FALSE;
    }
    MS_U8 ct = 0;
    TCH_VDEC_Metadata* pstTCH_VDEC_MD = (TCH_VDEC_Metadata*)pu8T35Data;
    TCH_VDEC_Metadata_CR* pstTCH_VDEC_MD_CR = (TCH_VDEC_Metadata_CR*)pu8CRData;

    pstTCHMetadata->specVersion = pstTCH_VDEC_MD->u8Spec_version;
    pstTCHMetadata->payloadMode = pstTCH_VDEC_MD->u8payload_mode;
    DBGMETADATA1(printk("\nSetTCHParameter %d %d:",sizeof(sl_hdr1_metadata),sizeof(TCH_VDEC_Metadata)));
    DBGMETADATA1(printk("PLM %d %d ,",pstTCHMetadata->payloadMode,pstTCH_VDEC_MD->u8payload_mode));
    if(pstTCHMetadata->payloadMode == 1)
    {
        if(enCR == FALSE)
        {
            printk("payloadMode is 1 but CR not enable!!!!!!!!!!\n");
            return FALSE;
        }
        if(pu8CRData == NULL)
        {
            printk("SetTCHParameter pu8CRData NULL\n");
            return FALSE;
        }
    }

    if(pstTCHMetadata->payloadMode == 1)
    {
        if((pstTCH_VDEC_MD_CR->m_colourRemapPrimaries == 9) && (pstTCH_VDEC_MD_CR->m_colourRemapMatrixCoefficients == 9))
        {
            pstTCHMetadata->hdrPicColourSpace = 1;
        }
        else if((pstTCH_VDEC_MD_CR->m_colourRemapPrimaries == 1) && (pstTCH_VDEC_MD_CR->m_colourRemapMatrixCoefficients == 1))
        {
            pstTCHMetadata->hdrPicColourSpace = 0;
        }
        else
        {
            pstTCHMetadata->hdrPicColourSpace = 0;  //=>default?
        }
        DBGMETADATA1(printk("hPCS %d (%d %d),",pstTCHMetadata->hdrPicColourSpace,pstTCH_VDEC_MD_CR->m_colourRemapPrimaries,pstTCH_VDEC_MD_CR->m_colourRemapMatrixCoefficients));
    }
    else
    {
        if(pstTCH_VDEC_MD->u8cv_rec_target_info_flag == 1)
        {
            if((pstTCH_VDEC_MD->u8cv_rec_primaries == 9) && (pstTCH_VDEC_MD->u8cv_rec_matrix_coefficients == 9))
            {
                pstTCHMetadata->hdrPicColourSpace = 1;
            }
            else if((pstTCH_VDEC_MD->u8cv_rec_primaries == 1) && (pstTCH_VDEC_MD->u8cv_rec_matrix_coefficients == 1))
            {
                pstTCHMetadata->hdrPicColourSpace = 0;
            }
            else
            {
                pstTCHMetadata->hdrPicColourSpace = 0;  //=>default?
            }
        }
        else
        {
            pstTCHMetadata->hdrPicColourSpace = 0;  //=>default?
        }
        DBGMETADATA1(printk("hPCS %d %d (%d %d),",pstTCHMetadata->hdrPicColourSpace,pstTCH_VDEC_MD->u8cv_rec_target_info_flag,
            pstTCH_VDEC_MD->u8cv_rec_primaries,pstTCH_VDEC_MD->u8cv_rec_matrix_coefficients));
    }
    pstTCHMetadata->hdrMasterDisplayColourSpace = 0; //=>nearest?
    pstTCHMetadata->hdrMasterDisplayMaxLuminance = 50*((pstTCH_VDEC_MD->u32max_display_mastering_luminance+250000)/500000);
    DBGMETADATA1(printk("hMDML (%ld) ",pstTCHMetadata->hdrMasterDisplayMaxLuminance));
    if(pstTCHMetadata->hdrMasterDisplayMaxLuminance >= 10000)
    {
        pstTCHMetadata->hdrMasterDisplayMaxLuminance = 10000;
    }
    DBGMETADATA1(printk("%ld %ld ,",pstTCHMetadata->hdrMasterDisplayMaxLuminance,pstTCH_VDEC_MD->u32max_display_mastering_luminance));
    pstTCHMetadata->hdrMasterDisplayMinLuminance = (pstTCH_VDEC_MD->u32min_display_mastering_luminance)/10000;
    DBGMETADATA1(printk("hMDmL (%ld) ",pstTCHMetadata->hdrMasterDisplayMinLuminance));
    if(pstTCHMetadata->hdrMasterDisplayMinLuminance >= 10000)
    {
        pstTCHMetadata->hdrMasterDisplayMinLuminance = 10000;
    }
    DBGMETADATA1(printk("%ld %ld ,",pstTCHMetadata->hdrMasterDisplayMinLuminance,pstTCH_VDEC_MD->u32min_display_mastering_luminance));
    if(pstTCH_VDEC_MD->u8VUI_Enable == 1)
    {
        if(pstTCH_VDEC_MD->u32colourPrimaries == 9 && pstTCH_VDEC_MD->u32matrixCoefficients == 9)
        {
            pstTCHMetadata->sdrPicColourSpace = 1;
        }
        else if(pstTCH_VDEC_MD->u32colourPrimaries == 1 && pstTCH_VDEC_MD->u32matrixCoefficients == 1)
        {
            pstTCHMetadata->sdrPicColourSpace = 0;
        }
        else
        {
            pstTCHMetadata->sdrPicColourSpace = 0;  //=>default?
        }
    }
    else
    {
        pstTCHMetadata->sdrPicColourSpace = 0;  //=>default?
    }
    DBGMETADATA1(printk("sPCS %d %d ,",pstTCHMetadata->sdrPicColourSpace,pstTCH_VDEC_MD->u8VUI_Enable));
    //pstTCHMetadata->sdrPicColourSpace =0; //=>no syntax in metadata
    pstTCHMetadata->sdrMasterDisplayColourSpace = pstTCHMetadata->sdrPicColourSpace;
    pstTCHMetadata->limit_range_en = pstTCH_VDEC_MD->u8Full_range_enable;
    if(pstTCHMetadata->payloadMode == 1)
    {
        pstTCHMetadata->u.tables.luminanceMappingNumVal = (pstTCH_VDEC_MD_CR->m_postLutNumValMinus1_0 + 1) & 0xFF;
        DBGMETADATA1(printk("lMNu %d %d ,",pstTCHMetadata->u.tables.luminanceMappingNumVal,pstTCH_VDEC_MD_CR->m_postLutNumValMinus1_0));
        if(pstTCHMetadata->u.tables.luminanceMappingNumVal >= 33)
            pstTCHMetadata->u.tables.luminanceMappingNumVal = 33;
        for(ct = 0; ct < pstTCHMetadata->u.tables.luminanceMappingNumVal ; ct++)
        {
            pstTCHMetadata->u.tables.luminanceMappingX[ct] = pstTCH_VDEC_MD_CR->m_postLut_0[ct][0];
            pstTCHMetadata->u.tables.luminanceMappingY[ct] = pstTCH_VDEC_MD_CR->m_postLut_0[ct][1];
        }
        pstTCHMetadata->u.tables.colourCorrectionNumVal = (pstTCH_VDEC_MD_CR->m_postLutNumValMinus1_1 + 1) & 0xFF;
        DBGMETADATA1(printk("cCNu %d %d ,",pstTCHMetadata->u.tables.colourCorrectionNumVal,pstTCH_VDEC_MD_CR->m_postLutNumValMinus1_1));
        if(pstTCHMetadata->u.tables.colourCorrectionNumVal >= 33)
            pstTCHMetadata->u.tables.colourCorrectionNumVal = 33;
        for(ct = 0; ct < pstTCHMetadata->u.tables.colourCorrectionNumVal ; ct++)
        {
            if(pstTCH_VDEC_MD_CR->m_postLut_1[ct][0] >= 2047)
                pstTCH_VDEC_MD_CR->m_postLut_1[ct][0] = 2047;
            if(pstTCH_VDEC_MD_CR->m_postLut_1[ct][1] >= 2047)
                pstTCH_VDEC_MD_CR->m_postLut_1[ct][1] = 2047;
            pstTCHMetadata->u.tables.colourCorrectionX[ct] = pstTCH_VDEC_MD_CR->m_postLut_1[ct][0];
            pstTCHMetadata->u.tables.colourCorrectionY[ct] = pstTCH_VDEC_MD_CR->m_postLut_1[ct][1];
        }

        pstTCHMetadata->u.tables.chromaToLumaInjectionMuA = pstTCH_VDEC_MD_CR->m_colourRemapCoeffs[0][1];
        pstTCHMetadata->u.tables.chromaToLumaInjectionMuB = pstTCH_VDEC_MD_CR->m_colourRemapCoeffs[0][2];

        DBGMETADATA1(printk("\n"));
        if(pstTCH_VDEC_MD_CR->m_colourRemapPersistenceFlag > 0)
        {
            printk("m_colourRemapPersistenceFlag !!!!!!!!!!!!!!!\n");
        }
        return TRUE;
    }
    pstTCHMetadata->u.variables.tmInputSignalBlackLevelOffset = pstTCH_VDEC_MD->u8tone_mapping_input_signal_black_level_offset;
    pstTCHMetadata->u.variables.tmInputSignalWhiteLevelOffset = pstTCH_VDEC_MD->u8tone_mapping_input_signal_white_level_offset;
    DBGMETADATA1(printk("ISBLO %d %d ,",pstTCHMetadata->u.variables.tmInputSignalBlackLevelOffset,pstTCH_VDEC_MD->u8tone_mapping_input_signal_black_level_offset));
    DBGMETADATA1(printk("ISWLO %d %d ,",pstTCHMetadata->u.variables.tmInputSignalWhiteLevelOffset,pstTCH_VDEC_MD->u8tone_mapping_input_signal_white_level_offset));
    pstTCHMetadata->u.variables.shadowGain = pstTCH_VDEC_MD->u8shadow_gain_control;
    pstTCHMetadata->u.variables.highlightGain = pstTCH_VDEC_MD->u8highlight_gain_control;
    pstTCHMetadata->u.variables.midToneWidthAdjFactor = pstTCH_VDEC_MD->u8mid_tone_width_adjustment_factor;
    DBGMETADATA1(printk("sG %d %d ,",pstTCHMetadata->u.variables.shadowGain,pstTCH_VDEC_MD->u8shadow_gain_control));
    DBGMETADATA1(printk("hlG %d %d ,",pstTCHMetadata->u.variables.highlightGain,pstTCH_VDEC_MD->u8highlight_gain_control));
    DBGMETADATA1(printk("mTWAF %d %d ,",pstTCHMetadata->u.variables.midToneWidthAdjFactor,pstTCH_VDEC_MD->u8mid_tone_width_adjustment_factor));
    pstTCHMetadata->u.variables.tmOutputFineTuningNumVal = pstTCH_VDEC_MD->u8tone_mapping_output_fine_tuning_num_val;
    DBGMETADATA1(printk("OFTNu %d %d ,",pstTCHMetadata->u.variables.tmOutputFineTuningNumVal,pstTCH_VDEC_MD->u8tone_mapping_output_fine_tuning_num_val));
    if(pstTCHMetadata->u.variables.tmOutputFineTuningNumVal >= 16)
        pstTCHMetadata->u.variables.tmOutputFineTuningNumVal = 16;
    for(ct = 0; ct < pstTCHMetadata->u.variables.tmOutputFineTuningNumVal ; ct++)
    {
        pstTCHMetadata->u.variables.tmOutputFineTuningX[ct] = pstTCH_VDEC_MD->u8tone_mapping_output_fine_tuning_x_y[ct][0];
        pstTCHMetadata->u.variables.tmOutputFineTuningY[ct] = pstTCH_VDEC_MD->u8tone_mapping_output_fine_tuning_x_y[ct][1];
    }
    pstTCHMetadata->u.variables.saturationGainNumVal = pstTCH_VDEC_MD->u8saturation_gain_num_val;
    DBGMETADATA1(printk("SGNu %d %d ,",pstTCHMetadata->u.variables.saturationGainNumVal,pstTCH_VDEC_MD->u8tone_mapping_output_fine_tuning_num_val));
    if(pstTCHMetadata->u.variables.saturationGainNumVal >= 16)
        pstTCHMetadata->u.variables.saturationGainNumVal = 16;
    for(ct = 0; ct < pstTCHMetadata->u.variables.saturationGainNumVal ; ct++)
    {
        pstTCHMetadata->u.variables.saturationGainX[ct] = pstTCH_VDEC_MD->u8saturation_gain_x_y[ct][0];
        pstTCHMetadata->u.variables.saturationGainY[ct] = pstTCH_VDEC_MD->u8saturation_gain_x_y[ct][1];
    }
    DBGMETADATA1(printk("\n"));
    if(pstTCH_VDEC_MD->u8Transfercv_rec_persistence_flagCharacteristics > 0)
    {
        printk("u8Transfercv_rec_persistence_flagCharacteristics !!!!!!!!!!!!!!!\n");
    }
    return TRUE;
}

