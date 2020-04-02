/*
 * iq_regset.h
 *
 *  Created on: 2016/3/22
 *      Author: Elsa-cf.Lin
 */

#if 0

#ifndef __iq_struct__
#define __iq_struct__

#define ISP_BYTE(x) (((u8*)(&x))[0])
#define ISP_WORD(x) (((u16*)(&x))[0])
#define ISP_DWORD(x) (((u32*)(&x))[0])

#define QMAP_Y2R_SIZE   29+1
#define QMAP_MCNR_SIZE   98+1
#define QMAP_XNR_SIZE   23+1
#define QMAP_GAMA_A2C_SC_SIZE   4+1
#define QMAP_CCM_SIZE   32+1
#define QMAP_HSV_SIZE   72+1
#define QMAP_GAMA_C2A_SC_SIZE   4+1
#define QMAP_R2Y_SIZE   29+1


#define QMAP_YEE_SIZE   205+1
#define QMAP_ACLUT_SIZE   66+1
#define QMAP_WDR_GBL_SIZE   122+1
#define QMAP_WDR_LOC_SIZE   117+1
#define QMAP_ADJUV_SIZE   67+1

#define QMAP_MXNR_SIZE   19+1
#define QMAP_YUVGAMA_SIZE   8+1
#define QMAP_YCUVM_SIZE   7+1
#define QMAP_CONOLTRANS_SIZE   27+1

#define reg_scl_y2r_cmc_en              0
#define reg_scl_y2r_y_sub_16_en         1
#define reg_scl_y2r_r_sub_16_en         2
#define reg_scl_y2r_b_sub_16_en         3
#define reg_scl_y2r_y_add_16_post_en    4
#define reg_scl_y2r_r_add_16_post_en    5
#define reg_scl_y2r_b_add_16_post_en    6
#define reg_scl_y2r_cb_add_128_post_en  7
#define reg_scl_y2r_cr_add_128_post_en  8
#define reg_scl_y2r_rran                9
#define reg_scl_y2r_gran                10
#define reg_scl_y2r_bran                11
#define reg_scl_y2r_coeff_11            12
#define reg_scl_y2r_coeff_12            14
#define reg_scl_y2r_coeff_13            16
#define reg_scl_y2r_coeff_21            18
#define reg_scl_y2r_coeff_22            20
#define reg_scl_y2r_coeff_23            22
#define reg_scl_y2r_coeff_31            24
#define reg_scl_y2r_coeff_32            26
#define reg_scl_y2r_coeff_33            28

#define reg_yuv2rgb_m11 168
#define reg_yuv2rgb_m12 170
#define reg_yuv2rgb_m13 172
#define reg_yuv2rgb_m21 174
#define reg_yuv2rgb_m22 176
#define reg_yuv2rgb_m23 178
#define reg_yuv2rgb_m31 180
#define reg_yuv2rgb_m32 182
#define reg_yuv2rgb_m33 184

#define reg_rgb2yuv_m11 186
#define reg_rgb2yuv_m12 188
#define reg_rgb2yuv_m13 190
#define reg_rgb2yuv_m21 192
#define reg_rgb2yuv_m22 194
#define reg_rgb2yuv_m23 196
#define reg_rgb2yuv_m31 198
#define reg_rgb2yuv_m32 200
#define reg_rgb2yuv_m33 202

#define reg_yee_yuv2rgb_m11 168
#define reg_yee_yuv2rgb_m12 170
#define reg_yee_yuv2rgb_m13 172
#define reg_yee_yuv2rgb_m21 174
#define reg_yee_yuv2rgb_m22 176
#define reg_yee_yuv2rgb_m23 178
#define reg_yee_yuv2rgb_m31 180
#define reg_yee_yuv2rgb_m32 182
#define reg_yee_yuv2rgb_m33 184

#define reg_yee_rgb2yuv_m11 186
#define reg_yee_rgb2yuv_m12 188
#define reg_yee_rgb2yuv_m13 190
#define reg_yee_rgb2yuv_m21 192
#define reg_yee_rgb2yuv_m22 194
#define reg_yee_rgb2yuv_m23 196
#define reg_yee_rgb2yuv_m31 198
#define reg_yee_rgb2yuv_m32 200
#define reg_yee_rgb2yuv_m33 202
#define reg_yee_merge_en    204
#define reg_yee_cbcr_en     205

//modify here from XML -------------------------------------------------

typedef enum
{

    IQ_FPN_EN,
    IQ_FPN_ROI_X,
    IQ_FPN_ROI_Y,
    IQ_FPN_WIDTH,
    IQ_FPN_HEIGHT,
    IQ_FPN_PRE_OFFSET,
    IQ_FPN_PRE_OFFSET_SIGN,
    IQ_FPN_DIFF_TH,
    IQ_FPN_CMP_RATIO,
    IQ_FPN_FRAME_NUM,
    IQ_FPN_SW_OFFSET_EN,
    IQ_FPN_SW_TABLE,
    IQ_FPN_SW_CMP_RATIO,
    IQ_FPN_END,
    IQ_FPN_END_SIZE = 5658,
} IQ_FPN_NAME;

#if 0
static char nameFPN[13][48] =
{
    "IQ_FPN_EN",
    "IQ_FPN_ROI_X",
    "IQ_FPN_ROI_Y",
    "IQ_FPN_WIDTH",
    "IQ_FPN_HEIGHT",
    "IQ_FPN_PRE_OFFSET",
    "IQ_FPN_PRE_OFFSET_SIGN",
    "IQ_FPN_DIFF_TH",
    "IQ_FPN_CMP_RATIO",
    "IQ_FPN_FRAME_NUM",
    "IQ_FPN_SW_OFFSET_EN",
    "IQ_FPN_SW_TABLE",
    "IQ_FPN_SW_CMP_RATIO",
};
#endif

//static unsigned short iq_FPN_offset[IQ_FPN_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 5654, 5656};

typedef struct
{
    u8 data[IQ_FPN_END_SIZE];
} IQ_FPN_CFG;


typedef enum
{
    IQ_SDC_EN,
    IQ_SDC_TBL,
    IQ_SDC_END,
    IQ_SDC_END_SIZE = 1028,
} IQ_SDC_NAME;

#if 0
static char nameSDC[2][48] =
{
    "IQ_SDC_EN",
    "IQ_SDC_TBL",
};
#endif

static unsigned short iq_SDC_offset[IQ_SDC_END + 1] = {0, 2, 1026};

typedef struct
{
    u8 data[IQ_SDC_END_SIZE];
} IQ_SDC_CFG;

#if 0
typedef enum
{
    IQ_DPC_EN,
    IQ_DPC_MODE,
    IQ_DPC_AWBG_EN,
    IQ_DPC_EDGE_MODE,
    IQ_DPC_LIGHT_EN,
    IQ_DPC_DARK_EN,
    IQ_DPC_TH_R,
    IQ_DPC_TH_GR,
    IQ_DPC_TH_GB,
    IQ_DPC_TH_B,
    IQ_DPC_CLUSTER_LR_EN,
    IQ_DPC_CLUSTER_LGR_EN,
    IQ_DPC_CLUSTER_LGB_EN,
    IQ_DPC_CLUSTER_LB_EN,
    IQ_DPC_CLUSTER_MODE_LOFF,
    IQ_DPC_CLUSTER_DR_EN,
    IQ_DPC_CLUSTER_DGR_EN,
    IQ_DPC_CLUSTER_DGB_EN,
    IQ_DPC_CLUSTER_DB_EN,
    IQ_DPC_CLUSTER_MODE_DOFF,
    IQ_DPC_TH_SEL,
    IQ_DPC_LIGHT_TH_ADJ,
    IQ_DPC_DARK_TH_ADJ,
    IQ_DPC_TH_LIGHT_SRC,
    IQ_DPC_TH_LIGHT_BASE,
    IQ_DPC_TH_DARK_SRC,
    IQ_DPC_TH_DARK_BASE,
    IQ_DPC_NEI_SMOOTH,
    IQ_DPC_NEI_DELTA_TH,
    IQ_DPC_NEI_DELTA_SFT_X,
    IQ_DPC_NEI_DELTA_GAIN,
    IQ_DPC_COLOR_RATIO_LR,
    IQ_DPC_COLOR_RATIO_LGR,
    IQ_DPC_COLOR_RATIO_LGB,
    IQ_DPC_COLOR_RATIO_LB,
    IQ_DPC_COLOR_RATIO_LOFF,
    IQ_DPC_COLOR_RATIO_DR,
    IQ_DPC_COLOR_RATIO_DGR,
    IQ_DPC_COLOR_RATIO_DGB,
    IQ_DPC_COLOR_RATIO_DB,
    IQ_DPC_COLOR_RATIO_DOFF,
    IQ_DPC_LIGHT_TH_ADD,
    IQ_DPC_DARK_TH_ADD,
    IQ_DPC_SORT_EN,
    IQ_DPC_SORT_R_EN,
    IQ_DPC_SORT_GR_EN,
    IQ_DPC_SORT_GB_EN,
    IQ_DPC_SORT_B_EN,
    IQ_DPC_SORT_1x3_MODE_EN,
    IQ_DPC_SORT_LUMTBL_X,
    IQ_DPC_SORT_LUMTBL_L,
    IQ_DPC_SORT_LUMTBL_D,
    IQ_DPC_SORT_LUMTBL_S,
    IQ_DPC_END,
    IQ_DPC_END_SIZE = 288,
} IQ_DPC_NAME;

static char nameDPC[53][48] =
{
    "IQ_DPC_EN",
    "IQ_DPC_MODE",
    "IQ_DPC_AWBG_EN",
    "IQ_DPC_EDGE_MODE",
    "IQ_DPC_LIGHT_EN",
    "IQ_DPC_DARK_EN",
    "IQ_DPC_TH_R",
    "IQ_DPC_TH_GR",
    "IQ_DPC_TH_GB",
    "IQ_DPC_TH_B",
    "IQ_DPC_CLUSTER_LR_EN",
    "IQ_DPC_CLUSTER_LGR_EN",
    "IQ_DPC_CLUSTER_LGB_EN",
    "IQ_DPC_CLUSTER_LB_EN",
    "IQ_DPC_CLUSTER_MODE_LOFF",
    "IQ_DPC_CLUSTER_DR_EN",
    "IQ_DPC_CLUSTER_DGR_EN",
    "IQ_DPC_CLUSTER_DGB_EN",
    "IQ_DPC_CLUSTER_DB_EN",
    "IQ_DPC_CLUSTER_MODE_DOFF",
    "IQ_DPC_TH_SEL",
    "IQ_DPC_LIGHT_TH_ADJ",
    "IQ_DPC_DARK_TH_ADJ",
    "IQ_DPC_TH_LIGHT_SRC",
    "IQ_DPC_TH_LIGHT_BASE",
    "IQ_DPC_TH_DARK_SRC",
    "IQ_DPC_TH_DARK_BASE",
    "IQ_DPC_NEI_SMOOTH",
    "IQ_DPC_NEI_DELTA_TH",
    "IQ_DPC_NEI_DELTA_SFT_X",
    "IQ_DPC_NEI_DELTA_GAIN",
    "IQ_DPC_COLOR_RATIO_LR",
    "IQ_DPC_COLOR_RATIO_LGR",
    "IQ_DPC_COLOR_RATIO_LGB",
    "IQ_DPC_COLOR_RATIO_LB",
    "IQ_DPC_COLOR_RATIO_LOFF",
    "IQ_DPC_COLOR_RATIO_DR",
    "IQ_DPC_COLOR_RATIO_DGR",
    "IQ_DPC_COLOR_RATIO_DGB",
    "IQ_DPC_COLOR_RATIO_DB",
    "IQ_DPC_COLOR_RATIO_DOFF",
    "IQ_DPC_LIGHT_TH_ADD",
    "IQ_DPC_DARK_TH_ADD",
    "IQ_DPC_SORT_EN",
    "IQ_DPC_SORT_R_EN",
    "IQ_DPC_SORT_GR_EN",
    "IQ_DPC_SORT_GB_EN",
    "IQ_DPC_SORT_B_EN",
    "IQ_DPC_SORT_1x3_MODE_EN",
    "IQ_DPC_SORT_LUMTBL_X",
    "IQ_DPC_SORT_LUMTBL_L",
    "IQ_DPC_SORT_LUMTBL_D",
    "IQ_DPC_SORT_LUMTBL_S",
};

static unsigned short iq_DPC_offset[IQ_DPC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 68, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 164, 228, 230, 232, 234, 236, 238, 240, 250, 262, 274, 286};
#endif

typedef struct
{
    u8 data[IQ_DPC_END_SIZE];
} IQ_DPC_CFG;


typedef enum
{
    IQ_GE_EN,
    IQ_GE_DIFF_STEP,
    IQ_GE_DIFF_TH,
    IQ_GE_DIFF_STEP2,
    IQ_GE_DIFF_TH2,
    IQ_GE_DIFF_MAX,
    IQ_GE_END,
    IQ_GE_END_SIZE = 14,
} IQ_GE_NAME;

static char nameGE[6][48] =
{
    "IQ_GE_EN",
    "IQ_GE_DIFF_STEP",
    "IQ_GE_DIFF_TH",
    "IQ_GE_DIFF_STEP2",
    "IQ_GE_DIFF_TH2",
    "IQ_GE_DIFF_MAX",
};

static unsigned short iq_GE_offset[IQ_GE_END + 1] = {0, 2, 4, 6, 8, 10, 12};

typedef struct
{
    u8 data[IQ_GE_END_SIZE];
} IQ_GE_CFG;


typedef enum
{
    IQ_ANTICT_EN,
    IQ_ANTICT_TH,
    IQ_ANTICT_SFT,
    IQ_ANTICT_END,
    IQ_ANTICT_END_SIZE = 8,
} IQ_ANTICT_NAME;

static char nameANTICT[3][48] =
{
    "IQ_ANTICT_EN",
    "IQ_ANTICT_TH",
    "IQ_ANTICT_SFT",
};

static unsigned short iq_ANTICT_offset[IQ_ANTICT_END + 1] = {0, 2, 4, 6};

typedef struct
{
    u8 data[IQ_ANTICT_END_SIZE];
} IQ_ANTICT_CFG;


typedef enum
{
    IQ_RGBIR_EN,
    IQ_RGBIR_MODE4x4,
    IQ_RGBIR_MODE,
    IQ_RGBIR_OFFSET_EN,
    IQ_RGBIR_HIST_IR_EN,
    IQ_RGBIR_IMONITOR_EN,
    IQ_RGBIR_OFT_RATIO_BY_Y,
    IQ_RGBIR_OFT_R_RATIO_BY_Y,
    IQ_RGBIR_OFT_G_RATIO_BY_Y,
    IQ_RGBIR_OFT_B_RATIO_BY_Y,
    IQ_RGBIR_DUMMY,
    IQ_RGBIR_OFT_CMP_RATIO,
    IQ_RGBIR_DIR_SCALER,
    IQ_RGBIR_2x2_G_LOC,
    IQ_RGBIR_END,
    IQ_RGBIR_END_SIZE = 68,
} IQ_RGBIR_NAME;

static char nameRGBIR[14][48] =
{
    "IQ_RGBIR_EN",
    "IQ_RGBIR_MODE4x4",
    "IQ_RGBIR_MODE",
    "IQ_RGBIR_OFFSET_EN",
    "IQ_RGBIR_HIST_IR_EN",
    "IQ_RGBIR_IMONITOR_EN",
    "IQ_RGBIR_OFT_RATIO_BY_Y",
    "IQ_RGBIR_OFT_R_RATIO_BY_Y",
    "IQ_RGBIR_OFT_G_RATIO_BY_Y",
    "IQ_RGBIR_OFT_B_RATIO_BY_Y",
    "IQ_RGBIR_DUMMY",
    "IQ_RGBIR_OFT_CMP_RATIO",
    "IQ_RGBIR_DIR_SCALER",
    "IQ_RGBIR_2x2_G_LOC",
};

static unsigned short iq_RGBIR_offset[IQ_RGBIR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 22, 34, 46, 58, 60, 62, 64, 66};

typedef struct
{
    u8 data[IQ_RGBIR_END_SIZE];
} IQ_RGBIR_CFG;


typedef enum
{
    IQ_OBC_EN,
    IQ_OBC_R_OFST,
    IQ_OBC_GR_OFST,
    IQ_OBC_GB_OFST,
    IQ_OBC_B_OFST,
    IQ_OBC_R_GAIN,
    IQ_OBC_GR_GAIN,
    IQ_OBC_GB_GAIN,
    IQ_OBC_B_GAIN,
    IQ_OBC_DITHER_EN,
    IQ_OBC_END,
    IQ_OBC_END_SIZE = 22,
} IQ_OBC_NAME;

static char nameOBC[10][48] =
{
    "IQ_OBC_EN",
    "IQ_OBC_R_OFST",
    "IQ_OBC_GR_OFST",
    "IQ_OBC_GB_OFST",
    "IQ_OBC_B_OFST",
    "IQ_OBC_R_GAIN",
    "IQ_OBC_GR_GAIN",
    "IQ_OBC_GB_GAIN",
    "IQ_OBC_B_GAIN",
    "IQ_OBC_DITHER_EN",
};

static unsigned short iq_OBC_offset[IQ_OBC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

typedef struct
{
    u8 data[IQ_OBC_END_SIZE];
} IQ_OBC_CFG;


typedef enum
{
    IQ_GAMA_C2A_ISP_EN,
    IQ_GAMA_C2A_ISP_LUT_R,
    IQ_GAMA_C2A_ISP_LUT_G,
    IQ_GAMA_C2A_ISP_LUT_B,
    IQ_GAMA_C2A_ISP_MAX_EN,
    IQ_GAMA_C2A_ISP_MAX_DATA,
    IQ_GAMA_C2A_ISP_END,
    IQ_GAMA_C2A_ISP_END_SIZE = 1544,
} IQ_GAMA_C2A_ISP_NAME;

static char nameGAMA_C2A_ISP[6][48] =
{
    "IQ_GAMA_C2A_ISP_EN",
    "IQ_GAMA_C2A_ISP_LUT_R",
    "IQ_GAMA_C2A_ISP_LUT_G",
    "IQ_GAMA_C2A_ISP_LUT_B",
    "IQ_GAMA_C2A_ISP_MAX_EN",
    "IQ_GAMA_C2A_ISP_MAX_DATA",
};

static unsigned short iq_GAMA_C2A_ISP_offset[IQ_GAMA_C2A_ISP_END + 1] = {0, 2, 514, 1026, 1538, 1540, 1542};

typedef struct
{
    u8 data[IQ_GAMA_C2A_ISP_END_SIZE];
} IQ_GAMA_C2A_ISP_CFG;


typedef enum
{
    IQ_LSC_EN,
    IQ_LSC_GLB_GAIN,
    IQ_LSC_CENTER_X,
    IQ_LSC_CENTER_Y,
    IQ_LSC_SHIFT,
    IQ_LSC_DITH_EN,
    IQ_LSC_R_GAIN_TABLE,
    IQ_LSC_G_GAIN_TABLE,
    IQ_LSC_B_GAIN_TABLE,
    IQ_LSC_END,
    IQ_LSC_END_SIZE = 206,
} IQ_LSC_NAME;

static char nameLSC[9][48] =
{
    "IQ_LSC_EN",
    "IQ_LSC_GLB_GAIN",
    "IQ_LSC_CENTER_X",
    "IQ_LSC_CENTER_Y",
    "IQ_LSC_SHIFT",
    "IQ_LSC_DITH_EN",
    "IQ_LSC_R_GAIN_TABLE",
    "IQ_LSC_G_GAIN_TABLE",
    "IQ_LSC_B_GAIN_TABLE",
};

static unsigned short iq_LSC_offset[IQ_LSC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 76, 140, 204};

typedef struct
{
    u8 data[IQ_LSC_END_SIZE];
} IQ_LSC_CFG;


typedef enum
{
    IQ_ALSC_EN,
    IQ_ALSC_DITHER_EN,
    IQ_ALSC_X_BLK_MODE,
    IQ_ALSC_Y_BLK_MODE,
    IQ_ALSC_DBG_EN,
    IQ_ALSC_DBG_SHIFT,
    IQ_ALSC_OFFSETX,
    IQ_ALSC_OFFSETY,
    IQ_ALSC_SCALEX,
    IQ_ALSC_SCALEY,
    IQ_ALSC_R_GAIN_TABLE,
    IQ_ALSC_G_GAIN_TABLE,
    IQ_ALSC_B_GAIN_TABLE,
    IQ_ALSC_END,
    IQ_ALSC_END_SIZE = 25276,
} IQ_ALSC_NAME;

static char nameALSC[13][48] =
{
    "IQ_ALSC_EN",
    "IQ_ALSC_DITHER_EN",
    "IQ_ALSC_X_BLK_MODE",
    "IQ_ALSC_Y_BLK_MODE",
    "IQ_ALSC_DBG_EN",
    "IQ_ALSC_DBG_SHIFT",
    "IQ_ALSC_OFFSETX",
    "IQ_ALSC_OFFSETY",
    "IQ_ALSC_SCALEX",
    "IQ_ALSC_SCALEY",
    "IQ_ALSC_R_GAIN_TABLE",
    "IQ_ALSC_G_GAIN_TABLE",
    "IQ_ALSC_B_GAIN_TABLE",
};

static unsigned short iq_ALSC_offset[IQ_ALSC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 8438, 16856, 25274};

typedef struct
{
    u8 data[IQ_ALSC_END_SIZE];
} IQ_ALSC_CFG;


typedef enum
{
    IQ_BDNR_EN,
    IQ_BDNR_TABLE_SEL,
    IQ_BDNR_FILTER_EN,
    IQ_BDNR_FILTER_MODE,
    IQ_BDNR_FILTER_DIV0,
    IQ_BDNR_FILTER_DIV1,
    IQ_BDNR_ROUND_MODE,
    IQ_BDNR_TABLEY,
    IQ_BDNR_TABLEY_MOTION,
    IQ_BDNR_LUM_TABLE,
    IQ_BDNR_LUM_TABLE_MOTION,
    IQ_BDNR_LUM_SH,
    IQ_BDNR_LUM_EN,
    IQ_BDNR_NONDITHER_METHOD,
    IQ_BDNR_NONDITHER_METHOD_MODE,
    IQ_BDNR_STICKY_SOLVER_EN,
    IQ_BDNR_STICKY_SOLVER_TH,
    IQ_BDNR_END,
    IQ_BDNR_END_SIZE = 124,
} IQ_BDNR_NAME;

static char nameBDNR[17][48] =
{
    "IQ_BDNR_EN",
    "IQ_BDNR_TABLE_SEL",
    "IQ_BDNR_FILTER_EN",
    "IQ_BDNR_FILTER_MODE",
    "IQ_BDNR_FILTER_DIV0",
    "IQ_BDNR_FILTER_DIV1",
    "IQ_BDNR_ROUND_MODE",
    "IQ_BDNR_TABLEY",
    "IQ_BDNR_TABLEY_MOTION",
    "IQ_BDNR_LUM_TABLE",
    "IQ_BDNR_LUM_TABLE_MOTION",
    "IQ_BDNR_LUM_SH",
    "IQ_BDNR_LUM_EN",
    "IQ_BDNR_NONDITHER_METHOD",
    "IQ_BDNR_NONDITHER_METHOD_MODE",
    "IQ_BDNR_STICKY_SOLVER_EN",
    "IQ_BDNR_STICKY_SOLVER_TH",
};

static unsigned short iq_BDNR_offset[IQ_BDNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 46, 78, 94, 110, 112, 114, 116, 118, 120, 122};

typedef struct
{
    u8 data[IQ_BDNR_END_SIZE];
} IQ_BDNR_CFG;


typedef enum
{
    IQ_SpikeNR_EN,
    IQ_SpikeNR_NR_COEF,
    IQ_SpikeNR_YP_22_STEP,
    IQ_SpikeNR_D_11_21_STEP,
    IQ_SpikeNR_D_31_STEP,
    IQ_SpikeNR_P_THRD,
    IQ_SpikeNR_P_THRD_1,
    IQ_SpikeNR_P_THRD_2,
    IQ_SpikeNR_P_THRD_3,
    IQ_SpikeNR_END,
    IQ_SpikeNR_END_SIZE = 20,
} IQ_SpikeNR_NAME;

static char nameSpikeNR[9][48] =
{
    "IQ_SpikeNR_EN",
    "IQ_SpikeNR_NR_COEF",
    "IQ_SpikeNR_YP_22_STEP",
    "IQ_SpikeNR_D_11_21_STEP",
    "IQ_SpikeNR_D_31_STEP",
    "IQ_SpikeNR_P_THRD",
    "IQ_SpikeNR_P_THRD_1",
    "IQ_SpikeNR_P_THRD_2",
    "IQ_SpikeNR_P_THRD_3",
};

static unsigned short iq_SpikeNR_offset[IQ_SpikeNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};

typedef struct
{
    u8 data[IQ_SpikeNR_END_SIZE];
} IQ_SpikeNR_CFG;


typedef enum
{
    IQ_BSNR_EN,
    IQ_BSNR_ALPHA_STEP_R,
    IQ_BSNR_ALPHA_STEP_G,
    IQ_BSNR_ALPHA_STEP_B,
    IQ_BSNR_STD_LOW_THRD_R,
    IQ_BSNR_STD_LOW_THRD_G,
    IQ_BSNR_STD_LOW_THRD_B,
    IQ_BSNR_STRENGTH_GAIN_R,
    IQ_BSNR_STRENGTH_GAIN_G,
    IQ_BSNR_STRENGTH_GAIN_B,
    IQ_BSNR_TABLE_R,
    IQ_BSNR_TABLE_G,
    IQ_BSNR_TABLE_B,
    IQ_BSNR_END,
    IQ_BSNR_END_SIZE = 118,
} IQ_BSNR_NAME;

static char nameBSNR[13][48] =
{
    "IQ_BSNR_EN",
    "IQ_BSNR_ALPHA_STEP_R",
    "IQ_BSNR_ALPHA_STEP_G",
    "IQ_BSNR_ALPHA_STEP_B",
    "IQ_BSNR_STD_LOW_THRD_R",
    "IQ_BSNR_STD_LOW_THRD_G",
    "IQ_BSNR_STD_LOW_THRD_B",
    "IQ_BSNR_STRENGTH_GAIN_R",
    "IQ_BSNR_STRENGTH_GAIN_G",
    "IQ_BSNR_STRENGTH_GAIN_B",
    "IQ_BSNR_TABLE_R",
    "IQ_BSNR_TABLE_G",
    "IQ_BSNR_TABLE_B",
};

static unsigned short iq_BSNR_offset[IQ_BSNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 52, 84, 116};

typedef struct
{
    u8 data[IQ_BSNR_END_SIZE];
} IQ_BSNR_CFG;


typedef enum
{
    IQ_NM_EN,
    IQ_NM_GAIN_R,
    IQ_NM_GAIN_G,
    IQ_NM_GAIN_B,
    IQ_NM_MAX_THRD_R,
    IQ_NM_MAX_THRD_G,
    IQ_NM_MAX_THRD_B,
    IQ_NM_MIN_THRD_R,
    IQ_NM_MIN_THRD_G,
    IQ_NM_MIN_THRD_B,
    IQ_NM_END,
    IQ_NM_END_SIZE = 22,
} IQ_NM_NAME;

static char nameNM[10][48] =
{
    "IQ_NM_EN",
    "IQ_NM_GAIN_R",
    "IQ_NM_GAIN_G",
    "IQ_NM_GAIN_B",
    "IQ_NM_MAX_THRD_R",
    "IQ_NM_MAX_THRD_G",
    "IQ_NM_MAX_THRD_B",
    "IQ_NM_MIN_THRD_R",
    "IQ_NM_MIN_THRD_G",
    "IQ_NM_MIN_THRD_B",
};

static unsigned short iq_NM_offset[IQ_NM_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

typedef struct
{
    u8 data[IQ_NM_END_SIZE];
} IQ_NM_CFG;


typedef enum
{
    IQ_DM_EN,
    IQ_DM_CFAI_BYPASS,
    IQ_DM_DVH_SLOPE,
    IQ_DM_DVH_THRD,
    IQ_DM_CPXWEI_SLOPE,
    IQ_DM_CPXWEI_THRD,
    IQ_DM_NB_SLOPE_M,
    IQ_DM_NB_SLOPE_S,
    IQ_DM_END,
    IQ_DM_END_SIZE = 18,
} IQ_DM_NAME;

static char nameDM[8][48] =
{
    "IQ_DM_EN",
    "IQ_DM_CFAI_BYPASS",
    "IQ_DM_DVH_SLOPE",
    "IQ_DM_DVH_THRD",
    "IQ_DM_CPXWEI_SLOPE",
    "IQ_DM_CPXWEI_THRD",
    "IQ_DM_NB_SLOPE_M",
    "IQ_DM_NB_SLOPE_S",
};

static unsigned short iq_DM_offset[IQ_DM_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16};

typedef struct
{
    u8 data[IQ_DM_END_SIZE];
} IQ_DM_CFG;


typedef enum
{
    IQ_PostDN_EN,
    IQ_PostDN_G_RATIO,
    IQ_PostDN_RB_RATIO,
    IQ_PostDN_GAVG_REF_EN,
    IQ_PostDN_END,
    IQ_PostDN_END_SIZE = 10,
} IQ_PostDN_NAME;

static char namePostDN[4][48] =
{
    "IQ_PostDN_EN",
    "IQ_PostDN_G_RATIO",
    "IQ_PostDN_RB_RATIO",
    "IQ_PostDN_GAVG_REF_EN",
};

static unsigned short iq_PostDN_offset[IQ_PostDN_END + 1] = {0, 2, 4, 6, 8};

typedef struct
{
    u8 data[IQ_PostDN_END_SIZE];
} IQ_PostDN_CFG;


typedef enum
{
    IQ_FalseColor_EN,
    IQ_FalseColor_R_RATIO,
    IQ_FalseColor_B_RATIO,
    IQ_FalseColor_COLOR_RATIO,
    IQ_FalseColor_END,
    IQ_FalseColor_END_SIZE = 10,
} IQ_FalseColor_NAME;

static char nameFalseColor[4][48] =
{
    "IQ_FalseColor_EN",
    "IQ_FalseColor_R_RATIO",
    "IQ_FalseColor_B_RATIO",
    "IQ_FalseColor_COLOR_RATIO",
};

static unsigned short iq_FalseColor_offset[IQ_FalseColor_END + 1] = {0, 2, 4, 6, 8};

typedef struct
{
    u8 data[IQ_FalseColor_END_SIZE];
} IQ_FalseColor_CFG;


typedef enum
{
    IQ_GAMA_A2A_ISP_EN,
    IQ_GAMA_A2A_ISP_LUT_R,
    IQ_GAMA_A2A_ISP_LUT_G,
    IQ_GAMA_A2A_ISP_LUT_B,
    IQ_GAMA_A2A_ISP_MAX_EN,
    IQ_GAMA_A2A_ISP_MAX_DATA,
    IQ_GAMA_A2A_ISP_END,
    IQ_GAMA_A2A_ISP_END_SIZE = 1544,
} IQ_GAMA_A2A_ISP_NAME;

static char nameGAMA_A2A_ISP[6][48] =
{
    "IQ_GAMA_A2A_ISP_EN",
    "IQ_GAMA_A2A_ISP_LUT_R",
    "IQ_GAMA_A2A_ISP_LUT_G",
    "IQ_GAMA_A2A_ISP_LUT_B",
    "IQ_GAMA_A2A_ISP_MAX_EN",
    "IQ_GAMA_A2A_ISP_MAX_DATA",
};

static unsigned short iq_GAMA_A2A_ISP_offset[IQ_GAMA_A2A_ISP_END + 1] = {0, 2, 514, 1026, 1538, 1540, 1542};

typedef struct
{
    u8 data[IQ_GAMA_A2A_ISP_END_SIZE];
} IQ_GAMA_A2A_ISP_CFG;


typedef enum
{
    IQ_MCNR_PATH_EN,
    IQ_MCNR_EN,
    IQ_MCNR_NORM_MVSAD,
    IQ_MCNR_MOVINGW_GAIN,
    IQ_MCNR_CONFIRMW_GAIN,
    IQ_MCNR_EDGE_GAIN,
    IQ_MCNR_EDGE_MAXSAD_GAIN,
    IQ_MCNR_EDGE_MAXSAD_SR_SEL,
    IQ_MCNR_NR_VSAD_SEL,
    IQ_MCNR_NR_HSAD_SEL,
    IQ_MCNR_NR_BLKSIZE_MV0DIFF_C,
    IQ_MCNR_NR_CSAD_MAX_EN,
    IQ_MCNR_NR_NORM_MV0DIFF_Y,
    IQ_MCNR_NR_NORM_MV0DIFF_C,
    IQ_MCNR_NR_DNRY_PN_EN,
    IQ_MCNR_NR_NORM_MV0DIFF_PN,
    IQ_MCNR_NR_DNRY_PN_OFFSET,
    IQ_MCNR_NR_DNRY_PN_GAIN,
    IQ_MCNR_NR_DNRY_PN_REDUCE_OFFSET,
    IQ_MCNR_NR_DNRY_PN_REDUCE_GAIN,
    IQ_MCNR_STICKY_SOLVER_EN_Y,
    IQ_MCNR_STICKY_SOLVER_EN_C,
    IQ_MCNR_STICKY_SOLVER_EN_HISTIIR,
    IQ_MCNR_STICKY_SOLVER_DITH_EN_Y,
    IQ_MCNR_STICKY_SOLVER_DITH_EN_C,
    IQ_MCNR_STICKY_SOLVER_DITH_EN_HISTIIR,
    IQ_MCNR_STICKY_SOLVER_TH,
    IQ_MCNR_STICKY_SOLVER_TH_HISTIIR,
    IQ_MCNR_HISTALPHA_FAST,
    IQ_MCNR_HSITALPHA,
    IQ_MCNR_HISTMOV_EN,
    IQ_MCNR_HSITALPHA_LOW_THRD,
    IQ_MCNR_LUMA_ADPT_EN,
    IQ_MCNR_MAXMOT_Y_EN,
    IQ_MCNR_MAXMOT_C_EN,
    IQ_MCNR_MV0CONF_GAIN,
    IQ_MCNR_LUMA_ADPT_GAIN,
    IQ_MCNR_DNRY_GAIN,
    IQ_MCNR_DNRC_GAIN,
    IQ_MCNR_ENHW_SEL,
    IQ_MCNR_MV0CONF_GAIN_ENH,
    IQ_MCNR_DNRY_GAIN_ENH,
    IQ_MCNR_MV0W_OFFSET,
    IQ_MCNR_MV0W_GAIN,
    IQ_MCNR_DNRLUT_Y,
    IQ_MCNR_DNRLUT_C,
    IQ_MCNR_LUMALUT_Y,
    IQ_MCNR_LUMALUT_C,
    IQ_MCNR_END,
    IQ_MCNR_END_SIZE = 186,
} IQ_MCNR_NAME;

static char nameMCNR[48][48] =
{
    "IQ_MCNR_PATH_EN",
    "IQ_MCNR_EN",
    "IQ_MCNR_NORM_MVSAD",
    "IQ_MCNR_MOVINGW_GAIN",
    "IQ_MCNR_CONFIRMW_GAIN",
    "IQ_MCNR_EDGE_GAIN",
    "IQ_MCNR_EDGE_MAXSAD_GAIN",
    "IQ_MCNR_EDGE_MAXSAD_SR_SEL",
    "IQ_MCNR_NR_VSAD_SEL",
    "IQ_MCNR_NR_HSAD_SEL",
    "IQ_MCNR_NR_BLKSIZE_MV0DIFF_C",
    "IQ_MCNR_NR_CSAD_MAX_EN",
    "IQ_MCNR_NR_NORM_MV0DIFF_Y",
    "IQ_MCNR_NR_NORM_MV0DIFF_C",
    "IQ_MCNR_NR_DNRY_PN_EN",
    "IQ_MCNR_NR_NORM_MV0DIFF_PN",
    "IQ_MCNR_NR_DNRY_PN_OFFSET",
    "IQ_MCNR_NR_DNRY_PN_GAIN",
    "IQ_MCNR_NR_DNRY_PN_REDUCE_OFFSET",
    "IQ_MCNR_NR_DNRY_PN_REDUCE_GAIN",
    "IQ_MCNR_STICKY_SOLVER_EN_Y",
    "IQ_MCNR_STICKY_SOLVER_EN_C",
    "IQ_MCNR_STICKY_SOLVER_EN_HISTIIR",
    "IQ_MCNR_STICKY_SOLVER_DITH_EN_Y",
    "IQ_MCNR_STICKY_SOLVER_DITH_EN_C",
    "IQ_MCNR_STICKY_SOLVER_DITH_EN_HISTIIR",
    "IQ_MCNR_STICKY_SOLVER_TH",
    "IQ_MCNR_STICKY_SOLVER_TH_HISTIIR",
    "IQ_MCNR_HISTALPHA_FAST",
    "IQ_MCNR_HSITALPHA",
    "IQ_MCNR_HISTMOV_EN",
    "IQ_MCNR_HSITALPHA_LOW_THRD",
    "IQ_MCNR_LUMA_ADPT_EN",
    "IQ_MCNR_MAXMOT_Y_EN",
    "IQ_MCNR_MAXMOT_C_EN",
    "IQ_MCNR_MV0CONF_GAIN",
    "IQ_MCNR_LUMA_ADPT_GAIN",
    "IQ_MCNR_DNRY_GAIN",
    "IQ_MCNR_DNRC_GAIN",
    "IQ_MCNR_ENHW_SEL",
    "IQ_MCNR_MV0CONF_GAIN_ENH",
    "IQ_MCNR_DNRY_GAIN_ENH",
    "IQ_MCNR_MV0W_OFFSET",
    "IQ_MCNR_MV0W_GAIN",
    "IQ_MCNR_DNRLUT_Y",
    "IQ_MCNR_DNRLUT_C",
    "IQ_MCNR_LUMALUT_Y",
    "IQ_MCNR_LUMALUT_C",
};

static unsigned short iq_MCNR_offset[IQ_MCNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 120, 152, 168, 184};

typedef struct
{
    u8 data[IQ_MCNR_END_SIZE];
} IQ_MCNR_CFG;


typedef enum
{
    IQ_NLM_EN,
    IQ_NLM_HISTIIR_ADAP_EN,
    IQ_NLM_HISTIIR_ADAP_RATIO,
    IQ_NLM_DSW_OFFSET,
    IQ_NLM_DSW_SHIFT,
    IQ_NLM_DSW_RATIO,
    IQ_NLM_DSW_ADAP_EN,
    IQ_NLM_DSW_LPF_EN,
    IQ_NLM_AVG_MODE,
    IQ_NLM_LUMA_ADAP_EN,
    IQ_NLM_LUMA_ADAP_GAIN_LUT,
    IQ_NLM_REGION_ADAP_EN,
    IQ_NLM_REGION_ADAP_SIZE_CONFIG,
    IQ_NLM_SAD_GAIN,
    IQ_NLM_SAD_SHIFT,
    IQ_NLM_DIST_WEIGHT_7X7,
    IQ_NLM_WEIGHT_LUT,
    IQ_NLM_POST_LUMA_ADAP_EN,
    IQ_NLM_POST_LUMA_ADAP_GAIN_LUT0,
    IQ_NLM_POST_LUMA_ADAP_GAIN_LUT0_MOTION,
    IQ_NLM_FIN_GAIN,
    IQ_NLM_FIN_GAIN_MOTION,
    IQ_NLM_MAIN_SNR_LUT,
    IQ_NLM_WB_SNR_LUT,
    IQ_NLM_END,
    IQ_NLM_END_SIZE = 374,
} IQ_NLM_NAME;

static char nameNLM[24][48] =
{
    "IQ_NLM_EN",
    "IQ_NLM_HISTIIR_ADAP_EN",
    "IQ_NLM_HISTIIR_ADAP_RATIO",
    "IQ_NLM_DSW_OFFSET",
    "IQ_NLM_DSW_SHIFT",
    "IQ_NLM_DSW_RATIO",
    "IQ_NLM_DSW_ADAP_EN",
    "IQ_NLM_DSW_LPF_EN",
    "IQ_NLM_AVG_MODE",
    "IQ_NLM_LUMA_ADAP_EN",
    "IQ_NLM_LUMA_ADAP_GAIN_LUT",
    "IQ_NLM_REGION_ADAP_EN",
    "IQ_NLM_REGION_ADAP_SIZE_CONFIG",
    "IQ_NLM_SAD_GAIN",
    "IQ_NLM_SAD_SHIFT",
    "IQ_NLM_DIST_WEIGHT_7X7",
    "IQ_NLM_WEIGHT_LUT",
    "IQ_NLM_POST_LUMA_ADAP_EN",
    "IQ_NLM_POST_LUMA_ADAP_GAIN_LUT0",
    "IQ_NLM_POST_LUMA_ADAP_GAIN_LUT0_MOTION",
    "IQ_NLM_FIN_GAIN",
    "IQ_NLM_FIN_GAIN_MOTION",
    "IQ_NLM_MAIN_SNR_LUT",
    "IQ_NLM_WB_SNR_LUT",
};

static unsigned short iq_NLM_offset[IQ_NLM_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 148, 150, 152, 154, 156, 174, 238, 240, 272, 304, 306, 308, 340, 372};

typedef struct
{
    u8 data[IQ_NLM_END_SIZE];
} IQ_NLM_CFG;


typedef enum
{
    IQ_XNR_EN,
    IQ_XNR_LPF_EN,
    IQ_XNR_H_DN_SEL,
    IQ_XNR_DW_WINDOW,
    IQ_XNR_DW_TH_U,
    IQ_XNR_DW_TH_V,
    IQ_XNR_DW_TH_BND_U,
    IQ_XNR_DW_TH_BND_V,
    IQ_XNR_MODE,
    IQ_XNR_MEAN_YTH,
    IQ_XNR_MEAN_UTH,
    IQ_XNR_MEAN_VTH,
    IQ_XNR_DUV_LW,
    IQ_XNR_DUV_GAP,
    IQ_XNR_MED_YTH,
    IQ_XNR_MED_UTH,
    IQ_XNR_MED_VTH,
    IQ_XNR_CBCR_MODE,
    IQ_XNR_END,
    IQ_XNR_END_SIZE = 38,
} IQ_XNR_NAME;

static char nameXNR[18][48] =
{
    "IQ_XNR_EN",
    "IQ_XNR_LPF_EN",
    "IQ_XNR_H_DN_SEL",
    "IQ_XNR_DW_WINDOW",
    "IQ_XNR_DW_TH_U",
    "IQ_XNR_DW_TH_V",
    "IQ_XNR_DW_TH_BND_U",
    "IQ_XNR_DW_TH_BND_V",
    "IQ_XNR_MODE",
    "IQ_XNR_MEAN_YTH",
    "IQ_XNR_MEAN_UTH",
    "IQ_XNR_MEAN_VTH",
    "IQ_XNR_DUV_LW",
    "IQ_XNR_DUV_GAP",
    "IQ_XNR_MED_YTH",
    "IQ_XNR_MED_UTH",
    "IQ_XNR_MED_VTH",
    "IQ_XNR_CBCR_MODE",
};

static unsigned short iq_XNR_offset[IQ_XNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36};

typedef struct
{
    u8 data[IQ_XNR_END_SIZE];
} IQ_XNR_CFG;


typedef enum
{
    IQ_LDC_EN,
    IQ_LDC_422TO444MODE,
    IQ_LDC_444TO422MODE,
    IQ_LDC_END,
    IQ_LDC_END_SIZE = 8,
} IQ_LDC_NAME;

static char nameLDC[3][48] =
{
    "IQ_LDC_EN",
    "IQ_LDC_422TO444MODE",
    "IQ_LDC_444TO422MODE",
};

static unsigned short iq_LDC_offset[IQ_LDC_END + 1] = {0, 2, 4, 6};

typedef struct
{
    u8 data[IQ_LDC_END_SIZE];
} IQ_LDC_CFG;


typedef enum
{
    IQ_GAMA_A2C_SC_EN,
    IQ_GAMA_A2C_SC_LUT_R,
    IQ_GAMA_A2C_SC_LUT_G,
    IQ_GAMA_A2C_SC_LUT_B,
    IQ_GAMA_A2C_SC_MAX_EN,
    IQ_GAMA_A2C_SC_MAX_DATA,
    IQ_GAMA_A2C_SC_END,
    IQ_GAMA_A2C_SC_END_SIZE = 1544,
} IQ_GAMA_A2C_SC_NAME;

static char nameGAMA_A2C_SC[6][48] =
{
    "IQ_GAMA_A2C_SC_EN",
    "IQ_GAMA_A2C_SC_LUT_R",
    "IQ_GAMA_A2C_SC_LUT_G",
    "IQ_GAMA_A2C_SC_LUT_B",
    "IQ_GAMA_A2C_SC_MAX_EN",
    "IQ_GAMA_A2C_SC_MAX_DATA",
};

static unsigned short iq_GAMA_A2C_SC_offset[IQ_GAMA_A2C_SC_END + 1] = {0, 2, 514, 1026, 1538, 1540, 1542};

typedef struct
{
    u8 data[IQ_GAMA_A2C_SC_END_SIZE];
} IQ_GAMA_A2C_SC_CFG;


typedef enum
{
    IQ_CCM_EN,
    IQ_CCM_COEFF,
    IQ_CCM_SAT_GAIN,
    IQ_CCM_CCT_THR,
    IQ_CCM_KEEP_HUE_EN,
    IQ_CCM_HUE_TH,
    IQ_CCM_END,
    IQ_CCM_END_SIZE = 60,
} IQ_CCM_NAME;

static char nameCCM[6][48] =
{
    "IQ_CCM_EN",
    "IQ_CCM_COEFF",
    "IQ_CCM_SAT_GAIN",
    "IQ_CCM_CCT_THR",
    "IQ_CCM_KEEP_HUE_EN",
    "IQ_CCM_HUE_TH",
};

static unsigned short iq_CCM_offset[IQ_CCM_END + 1] = {0, 2, 20, 22, 54, 56, 58};

typedef struct
{
    u8 data[IQ_CCM_END_SIZE];
} IQ_CCM_CFG;


typedef enum
{
    IQ_HSV_EN,
    IQ_HSV_HUE,
    IQ_HSV_SAT,
    IQ_HSV_ADJ,
    IQ_HSV_END,
    IQ_HSV_END_SIZE = 148,
} IQ_HSV_NAME;

static char nameHSV[4][48] =
{
    "IQ_HSV_EN",
    "IQ_HSV_HUE",
    "IQ_HSV_SAT",
    "IQ_HSV_ADJ",
};

static unsigned short iq_HSV_offset[IQ_HSV_END + 1] = {0, 2, 50, 98, 146};

typedef struct
{
    u8 data[IQ_HSV_END_SIZE];
} IQ_HSV_CFG;


typedef enum
{
    IQ_GAMA_C2A_SC_EN,
    IQ_GAMA_C2A_SC_LUT_R,
    IQ_GAMA_C2A_SC_LUT_G,
    IQ_GAMA_C2A_SC_LUT_B,
    IQ_GAMA_C2A_SC_MAX_EN,
    IQ_GAMA_C2A_SC_MAX_DATA,
    IQ_GAMA_C2A_SC_END,
    IQ_GAMA_C2A_SC_END_SIZE = 1544,
} IQ_GAMA_C2A_SC_NAME;

static char nameGAMA_C2A_SC[6][48] =
{
    "IQ_GAMA_C2A_SC_EN",
    "IQ_GAMA_C2A_SC_LUT_R",
    "IQ_GAMA_C2A_SC_LUT_G",
    "IQ_GAMA_C2A_SC_LUT_B",
    "IQ_GAMA_C2A_SC_MAX_EN",
    "IQ_GAMA_C2A_SC_MAX_DATA",
};

static unsigned short iq_GAMA_C2A_SC_offset[IQ_GAMA_C2A_SC_END + 1] = {0, 2, 514, 1026, 1538, 1540, 1542};

typedef struct
{
    u8 data[IQ_GAMA_C2A_SC_END_SIZE];
} IQ_GAMA_C2A_SC_CFG;


typedef enum
{
    IQ_R2Y_EN,
    IQ_R2Y_COEFF,
    IQ_R2Y_END,
    IQ_R2Y_END_SIZE = 22,
} IQ_R2Y_NAME;

static char nameR2Y[2][48] =
{
    "IQ_R2Y_EN",
    "IQ_R2Y_COEFF",
};

static unsigned short iq_R2Y_offset[IQ_R2Y_END + 1] = {0, 2, 20};

typedef struct
{
    u8 data[IQ_R2Y_END_SIZE];
} IQ_R2Y_CFG;


typedef enum
{
    IQ_TwoDPK_EN,
    IQ_TwoDPK_AC_YEE_MODE,
    IQ_TwoDPK_H_LPF_COEF,
    IQ_TwoDPK_V_LPF_COEF_1,
    IQ_TwoDPK_V_LPF_COEF_2,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL,
    IQ_TwoDPK_OSD_SHARPNESS_SEP_HV_EN,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL_H,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL_V,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL_OFFSET,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL_H_OFFSET,
    IQ_TwoDPK_OSD_SHARPNESS_CTRL_V_OFFSET,
    IQ_TwoDPK_ALPHA_THRD,
    IQ_TwoDPK_PEAKING_TERM_SELECT,
    IQ_TwoDPK_BAND_OVER_LIMIT,
    IQ_TwoDPK_BAND_UNDER_LIMIT,
    IQ_TwoDPK_BAND_COEF_STEP,
    IQ_TwoDPK_BAND_CORING_THRD,
    IQ_TwoDPK_BAND_COEF,
    IQ_TwoDPK_BAND_ADAPTIVE_EN,
    IQ_TwoDPK_BAND_PEAKING_EN,
    IQ_TwoDPK_BAND_LOW_DIFF_THRD,
    IQ_TwoDPK_BAND_ADAPTIVE_GAIN_STEP,
    IQ_TwoDPK_VER_LUT,
    IQ_TwoDPK_HOR_LUT,
    IQ_TwoDPK_DIA_LUT,
    IQ_TwoDPK_COLOR_PEAKING_EN,
    IQ_TwoDPK_COLOR_CORING_EN,
    IQ_TwoDPK_CORING_THRD_2,
    IQ_TwoDPK_CORING_THRD_1,
    IQ_TwoDPK_CORING_THRD_STEP,
    IQ_TwoDPK_CORING_ADP_Y_EN,
    IQ_TwoDPK_CORING_ADP_Y_ALPHA_LPF_EN,
    IQ_TwoDPK_CORING_Y_LOW_THRD,
    IQ_TwoDPK_CORING_ADP_Y_STEP,
    IQ_TwoDPK_CORING_ADP_Y_ALPHA_LUT,
    IQ_TwoDPK_ADP_Y_EN,
    IQ_TwoDPK_ADP_Y_ALPHA_LPF_EN,
    IQ_TwoDPK_Y_LOW_THRD,
    IQ_TwoDPK_ADP_Y_STEP,
    IQ_TwoDPK_ADP_Y_ALPHA_LUT,
    IQ_TwoDPK_END,
    IQ_TwoDPK_END_SIZE = 414,
} IQ_TwoDPK_NAME;

static char nameTwoDPK[41][48] =
{
    "IQ_TwoDPK_EN",
    "IQ_TwoDPK_AC_YEE_MODE",
    "IQ_TwoDPK_H_LPF_COEF",
    "IQ_TwoDPK_V_LPF_COEF_1",
    "IQ_TwoDPK_V_LPF_COEF_2",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL",
    "IQ_TwoDPK_OSD_SHARPNESS_SEP_HV_EN",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL_H",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL_V",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL_OFFSET",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL_H_OFFSET",
    "IQ_TwoDPK_OSD_SHARPNESS_CTRL_V_OFFSET",
    "IQ_TwoDPK_ALPHA_THRD",
    "IQ_TwoDPK_PEAKING_TERM_SELECT",
    "IQ_TwoDPK_BAND_OVER_LIMIT",
    "IQ_TwoDPK_BAND_UNDER_LIMIT",
    "IQ_TwoDPK_BAND_COEF_STEP",
    "IQ_TwoDPK_BAND_CORING_THRD",
    "IQ_TwoDPK_BAND_COEF",
    "IQ_TwoDPK_BAND_ADAPTIVE_EN",
    "IQ_TwoDPK_BAND_PEAKING_EN",
    "IQ_TwoDPK_BAND_LOW_DIFF_THRD",
    "IQ_TwoDPK_BAND_ADAPTIVE_GAIN_STEP",
    "IQ_TwoDPK_VER_LUT",
    "IQ_TwoDPK_HOR_LUT",
    "IQ_TwoDPK_DIA_LUT",
    "IQ_TwoDPK_COLOR_PEAKING_EN",
    "IQ_TwoDPK_COLOR_CORING_EN",
    "IQ_TwoDPK_CORING_THRD_2",
    "IQ_TwoDPK_CORING_THRD_1",
    "IQ_TwoDPK_CORING_THRD_STEP",
    "IQ_TwoDPK_CORING_ADP_Y_EN",
    "IQ_TwoDPK_CORING_ADP_Y_ALPHA_LPF_EN",
    "IQ_TwoDPK_CORING_Y_LOW_THRD",
    "IQ_TwoDPK_CORING_ADP_Y_STEP",
    "IQ_TwoDPK_CORING_ADP_Y_ALPHA_LUT",
    "IQ_TwoDPK_ADP_Y_EN",
    "IQ_TwoDPK_ADP_Y_ALPHA_LPF_EN",
    "IQ_TwoDPK_Y_LOW_THRD",
    "IQ_TwoDPK_ADP_Y_STEP",
    "IQ_TwoDPK_ADP_Y_ALPHA_LUT",
};

static unsigned short iq_TwoDPK_offset[IQ_TwoDPK_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 58, 74, 90, 114, 138, 162, 186, 210, 234, 258, 290, 322, 354, 356, 358, 360, 362, 364, 366, 368, 370, 372, 388, 390, 392, 394, 396, 412};

typedef struct
{
    u8 data[IQ_TwoDPK_END_SIZE];
} IQ_TwoDPK_CFG;


typedef enum
{
    IQ_LCE_EN,
    IQ_LCE_Y_AVE_SEL,
    IQ_LCE_STD_TH1,
    IQ_LCE_STD_SLOP1,
    IQ_LCE_STD_TH2,
    IQ_LCE_STD_SLOP2,
    IQ_LCE_GAIN_MIN,
    IQ_LCE_GAIN_MAX,
    IQ_LCE_GAIN_COMPLEX,
    IQ_LCE_DSW_MINSADGAIN,
    IQ_LCE_DSW_THRD,
    IQ_LCE_DSW_GAIN,
    IQ_LCE_SODC_ALPHA_EN,
    IQ_LCE_SODC_SLOP,
    IQ_LCE_SODC_LOW_TH,
    IQ_LCE_SODC_LOW_ALPHA,
    IQ_LCE_DITHER_EN,
    IQ_LCE_COLOR_ALPHA_EN,
    IQ_LCE_3CURVE_EN,
    IQ_LCE_CURVE,
    IQ_LCE_CURVE_LUT1,
    IQ_LCE_CURVE_LUT2,
    IQ_LCE_CURVE_LUT3,
    IQ_LCE_DIFF_GAIN,
    IQ_LCE_COLOR_G_STRENGTH_LCE,
    IQ_LCE_COLOR_G_SELECT_LCE,
    IQ_LCE_END,
    IQ_LCE_END_SIZE = 162,
} IQ_LCE_NAME;

static char nameLCE[26][48] =
{
    "IQ_LCE_EN",
    "IQ_LCE_Y_AVE_SEL",
    "IQ_LCE_STD_TH1",
    "IQ_LCE_STD_SLOP1",
    "IQ_LCE_STD_TH2",
    "IQ_LCE_STD_SLOP2",
    "IQ_LCE_GAIN_MIN",
    "IQ_LCE_GAIN_MAX",
    "IQ_LCE_GAIN_COMPLEX",
    "IQ_LCE_DSW_MINSADGAIN",
    "IQ_LCE_DSW_THRD",
    "IQ_LCE_DSW_GAIN",
    "IQ_LCE_SODC_ALPHA_EN",
    "IQ_LCE_SODC_SLOP",
    "IQ_LCE_SODC_LOW_TH",
    "IQ_LCE_SODC_LOW_ALPHA",
    "IQ_LCE_DITHER_EN",
    "IQ_LCE_COLOR_ALPHA_EN",
    "IQ_LCE_3CURVE_EN",
    "IQ_LCE_CURVE",
    "IQ_LCE_CURVE_LUT1",
    "IQ_LCE_CURVE_LUT2",
    "IQ_LCE_CURVE_LUT3",
    "IQ_LCE_DIFF_GAIN",
    "IQ_LCE_COLOR_G_STRENGTH_LCE",
    "IQ_LCE_COLOR_G_SELECT_LCE",
};

static unsigned short iq_LCE_offset[IQ_LCE_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 46, 78, 110, 142, 144, 152, 160};

typedef struct
{
    u8 data[IQ_LCE_END_SIZE];
} IQ_LCE_CFG;


typedef enum
{
    IQ_DLC_EN,
    IQ_DLC_LEVEL_CTRL_BY_AE,
    IQ_DLC_LEVEL_MIN_MAX,
    IQ_DLC_CURVE_FIT_VAR_PW_EN,
    IQ_DLC_CURVE_FIT_VAR_CP,
    IQ_DLC_CURVE_FIT_TABLE,
    IQ_DLC_IN_Y_GAIN,
    IQ_DLC_IN_Y_OFFSET,
    IQ_DLC_IN_C_GAIN,
    IQ_DLC_IN_C_OFFSET,
    IQ_DLC_OUT_Y_GAIN,
    IQ_DLC_OUT_Y_OFFSET,
    IQ_DLC_OUT_C_GAIN,
    IQ_DLC_OUT_C_OFFSET,
    IQ_DLC_END,
    IQ_DLC_END_SIZE = 288,
} IQ_DLC_NAME;

static char nameDLC[14][48] =
{
    "IQ_DLC_EN",
    "IQ_DLC_LEVEL_CTRL_BY_AE",
    "IQ_DLC_LEVEL_MIN_MAX",
    "IQ_DLC_CURVE_FIT_VAR_PW_EN",
    "IQ_DLC_CURVE_FIT_VAR_CP",
    "IQ_DLC_CURVE_FIT_TABLE",
    "IQ_DLC_IN_Y_GAIN",
    "IQ_DLC_IN_Y_OFFSET",
    "IQ_DLC_IN_C_GAIN",
    "IQ_DLC_IN_C_OFFSET",
    "IQ_DLC_OUT_Y_GAIN",
    "IQ_DLC_OUT_Y_OFFSET",
    "IQ_DLC_OUT_C_GAIN",
    "IQ_DLC_OUT_C_OFFSET",
};

static unsigned short iq_DLC_offset[IQ_DLC_END + 1] = {0, 2, 4, 8, 10, 138, 270, 272, 274, 276, 278, 280, 282, 284, 286};

typedef struct
{
    u8 data[IQ_DLC_END_SIZE];
} IQ_DLC_CFG;


typedef enum
{
    IQ_UVC_EN,
    IQ_UVC_LOCATE,
    IQ_UVC_RGB_EN,
    IQ_UVC_DLC_FULLRANGE_EN,
    IQ_UVC_ADAPTIVE_LUMA_EN,
    IQ_UVC_ADAPTIVE_LUMA_WHITE_THRD,
    IQ_UVC_ADAPTIVE_LUMA_WHITE_STEP,
    IQ_UVC_ADAPTIVE_LUMA_BLACK_THRD,
    IQ_UVC_ADAPTIVE_LUMA_BLACK_STEP,
    IQ_UVC_ADAPTIVE_LUMA_GAIN_LOW,
    IQ_UVC_ADAPTIVE_LUMA_GAIN_MED,
    IQ_UVC_ADAPTIVE_LUMA_GAIN_HIGH,
    IQ_UVC_ADAPTIVE_LUMA_Y_INPUT,
    IQ_UVC_LOW_Y_SAT_PROT_EN,
    IQ_UVC_LOW_SAT_PROT_EN,
    IQ_UVC_LOW_SAT_PROT_THRD,
    IQ_UVC_LOW_SAT_MIN_STRENGTH,
    IQ_UVC_LOW_SAT_PROT_SLOPE,
    IQ_UVC_LOW_Y_PROT_EN,
    IQ_UVC_LOW_Y_SEL,
    IQ_UVC_LOW_Y_PROT_THRD,
    IQ_UVC_LOW_Y_MIN_STRENGTH,
    IQ_UVC_LOW_Y_PROT_SLOPE,
    IQ_UVC_LPF_EN,
    IQ_UVC_GAIN_HIGH_LIMIT,
    IQ_UVC_GAIN_LOW_LIMIT,
    IQ_UVC_END,
    IQ_UVC_END_SIZE = 54,
} IQ_UVC_NAME;

static char nameUVC[26][48] =
{
    "IQ_UVC_EN",
    "IQ_UVC_LOCATE",
    "IQ_UVC_RGB_EN",
    "IQ_UVC_DLC_FULLRANGE_EN",
    "IQ_UVC_ADAPTIVE_LUMA_EN",
    "IQ_UVC_ADAPTIVE_LUMA_WHITE_THRD",
    "IQ_UVC_ADAPTIVE_LUMA_WHITE_STEP",
    "IQ_UVC_ADAPTIVE_LUMA_BLACK_THRD",
    "IQ_UVC_ADAPTIVE_LUMA_BLACK_STEP",
    "IQ_UVC_ADAPTIVE_LUMA_GAIN_LOW",
    "IQ_UVC_ADAPTIVE_LUMA_GAIN_MED",
    "IQ_UVC_ADAPTIVE_LUMA_GAIN_HIGH",
    "IQ_UVC_ADAPTIVE_LUMA_Y_INPUT",
    "IQ_UVC_LOW_Y_SAT_PROT_EN",
    "IQ_UVC_LOW_SAT_PROT_EN",
    "IQ_UVC_LOW_SAT_PROT_THRD",
    "IQ_UVC_LOW_SAT_MIN_STRENGTH",
    "IQ_UVC_LOW_SAT_PROT_SLOPE",
    "IQ_UVC_LOW_Y_PROT_EN",
    "IQ_UVC_LOW_Y_SEL",
    "IQ_UVC_LOW_Y_PROT_THRD",
    "IQ_UVC_LOW_Y_MIN_STRENGTH",
    "IQ_UVC_LOW_Y_PROT_SLOPE",
    "IQ_UVC_LPF_EN",
    "IQ_UVC_GAIN_HIGH_LIMIT",
    "IQ_UVC_GAIN_LOW_LIMIT",
};

static unsigned short iq_UVC_offset[IQ_UVC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};

typedef struct
{
    u8 data[IQ_UVC_END_SIZE];
} IQ_UVC_CFG;


typedef enum
{
    IQ_IHC_EN,
    IQ_IHC_CBCR_TO_UV,
    IQ_IHC_HUE_USER_COLOR,
    IQ_IHC_Y_MODE_EN,
    IQ_IHC_Y_MODE_DIFF_COLOR_EN,
    IQ_IHC_HUE_USER_COLOR_0,
    IQ_IHC_HUE_USER_COLOR_1,
    IQ_IHC_HUE_USER_COLOR_2,
    IQ_IHC_END,
    IQ_IHC_END_SIZE = 138,
} IQ_IHC_NAME;

static char nameIHC[8][48] =
{
    "IQ_IHC_EN",
    "IQ_IHC_CBCR_TO_UV",
    "IQ_IHC_HUE_USER_COLOR",
    "IQ_IHC_Y_MODE_EN",
    "IQ_IHC_Y_MODE_DIFF_COLOR_EN",
    "IQ_IHC_HUE_USER_COLOR_0",
    "IQ_IHC_HUE_USER_COLOR_1",
    "IQ_IHC_HUE_USER_COLOR_2",
};

static unsigned short iq_IHC_offset[IQ_IHC_END + 1] = {0, 2, 4, 36, 38, 40, 72, 104, 136};

typedef struct
{
    u8 data[IQ_IHC_END_SIZE];
} IQ_IHC_CFG;


typedef enum
{
    IQ_ICC_EN,
    IQ_ICC_Y_MODE_EN,
    IQ_ICC_Y_MODE_DIFF_COLOR_EN,
    IQ_ICC_SIGN_SA_USER,
    IQ_ICC_STEP_SA_USER,
    IQ_ICC_COMMON_MINUS_GAIN,
    IQ_ICC_SA_USER_COLOR,
    IQ_ICC_SA_USER_COLOR_0,
    IQ_ICC_SA_USER_COLOR_1,
    IQ_ICC_SA_USER_COLOR_2,
    IQ_ICC_SIGN_SA_USER_0,
    IQ_ICC_SIGN_SA_USER_1,
    IQ_ICC_SIGN_SA_USER_2,
    IQ_ICC_SA_MIN,
    IQ_ICC_END,
    IQ_ICC_END_SIZE = 150,
} IQ_ICC_NAME;

static char nameICC[14][48] =
{
    "IQ_ICC_EN",
    "IQ_ICC_Y_MODE_EN",
    "IQ_ICC_Y_MODE_DIFF_COLOR_EN",
    "IQ_ICC_SIGN_SA_USER",
    "IQ_ICC_STEP_SA_USER",
    "IQ_ICC_COMMON_MINUS_GAIN",
    "IQ_ICC_SA_USER_COLOR",
    "IQ_ICC_SA_USER_COLOR_0",
    "IQ_ICC_SA_USER_COLOR_1",
    "IQ_ICC_SA_USER_COLOR_2",
    "IQ_ICC_SIGN_SA_USER_0",
    "IQ_ICC_SIGN_SA_USER_1",
    "IQ_ICC_SIGN_SA_USER_2",
    "IQ_ICC_SA_MIN",
};

static unsigned short iq_ICC_offset[IQ_ICC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 44, 76, 108, 140, 142, 144, 146, 148};

typedef struct
{
    u8 data[IQ_ICC_END_SIZE];
} IQ_ICC_CFG;


typedef enum
{
    IQ_IBC_EN,
    IQ_IBC_WEIGHT_Y_MIN_LIMIT,
    IQ_IBC_WEIGHT_C_MIN_LIMIT,
    IQ_IBC_YCOLOR_ADJ,
    IQ_IBC_CORING_THRD,
    IQ_IBC_Y_ADJUST_LPF_EN,
    IQ_IBC_END,
    IQ_IBC_END_SIZE = 44,
} IQ_IBC_NAME;

static char nameIBC[6][48] =
{
    "IQ_IBC_EN",
    "IQ_IBC_WEIGHT_Y_MIN_LIMIT",
    "IQ_IBC_WEIGHT_C_MIN_LIMIT",
    "IQ_IBC_YCOLOR_ADJ",
    "IQ_IBC_CORING_THRD",
    "IQ_IBC_Y_ADJUST_LPF_EN",
};

static unsigned short iq_IBC_offset[IQ_IBC_END + 1] = {0, 2, 4, 6, 38, 40, 42};

typedef struct
{
    u8 data[IQ_IBC_END_SIZE];
} IQ_IBC_CFG;


typedef enum
{
    IQ_IHCICC_Y_0,
    IQ_IHCICC_Y_1,
    IQ_IHCICC_Y_2,
    IQ_IHCICC_Y_3,
    IQ_IHCICC_COLOR_Y_0,
    IQ_IHCICC_COLOR_Y_1,
    IQ_IHCICC_COLOR_Y_2,
    IQ_IHCICC_COLOR_Y_3,
    IQ_IHCICC_END,
    IQ_IHCICC_END_SIZE = 138,
} IQ_IHCICC_NAME;

static char nameIHCICC[8][48] =
{
    "IQ_IHCICC_Y_0",
    "IQ_IHCICC_Y_1",
    "IQ_IHCICC_Y_2",
    "IQ_IHCICC_Y_3",
    "IQ_IHCICC_COLOR_Y_0",
    "IQ_IHCICC_COLOR_Y_1",
    "IQ_IHCICC_COLOR_Y_2",
    "IQ_IHCICC_COLOR_Y_3",
};

static unsigned short iq_IHCICC_offset[IQ_IHCICC_END + 1] = {0, 2, 4, 6, 8, 40, 72, 104, 136};

typedef struct
{
    u8 data[IQ_IHCICC_END_SIZE];
} IQ_IHCICC_CFG;


typedef enum
{
    IQ_FCC_EN,
    IQ_FCC_FR_EN,
    IQ_FCC_WIN1_DARK_MODE_EN,
    IQ_FCC_9T_FIRST_EN,
    IQ_FCC_BDRY_DIST,
    IQ_FCC_WINEN,
    IQ_FCC_WINCRDOWN,
    IQ_FCC_WINCRUP,
    IQ_FCC_WINCBDOWN,
    IQ_FCC_WINCBUP,
    IQ_FCC_WINGAIN,
    IQ_FCC_CBTAR,
    IQ_FCC_CRTAR,
    IQ_FCC_ADP_Y_WIN_EN,
    IQ_FCC_ADP_Y_WIN_NUM,
    IQ_FCC_ADP_Y_LUT_WIN0,
    IQ_FCC_ADP_Y_LUT_WIN1,
    IQ_FCC_ADP_Y_LUT_WIN2,
    IQ_FCC_ADP_Y_LUT_WIN3,
    IQ_FCC_CB_T1_Y,
    IQ_FCC_CR_T1_Y,
    IQ_FCC_END,
    IQ_FCC_END_SIZE = 328,
} IQ_FCC_NAME;

static char nameFCC[21][48] =
{
    "IQ_FCC_EN",
    "IQ_FCC_FR_EN",
    "IQ_FCC_WIN1_DARK_MODE_EN",
    "IQ_FCC_9T_FIRST_EN",
    "IQ_FCC_BDRY_DIST",
    "IQ_FCC_WINEN",
    "IQ_FCC_WINCRDOWN",
    "IQ_FCC_WINCRUP",
    "IQ_FCC_WINCBDOWN",
    "IQ_FCC_WINCBUP",
    "IQ_FCC_WINGAIN",
    "IQ_FCC_CBTAR",
    "IQ_FCC_CRTAR",
    "IQ_FCC_ADP_Y_WIN_EN",
    "IQ_FCC_ADP_Y_WIN_NUM",
    "IQ_FCC_ADP_Y_LUT_WIN0",
    "IQ_FCC_ADP_Y_LUT_WIN1",
    "IQ_FCC_ADP_Y_LUT_WIN2",
    "IQ_FCC_ADP_Y_LUT_WIN3",
    "IQ_FCC_CB_T1_Y",
    "IQ_FCC_CR_T1_Y",
};

static unsigned short iq_FCC_offset[IQ_FCC_END + 1] = {0, 2, 4, 6, 8, 10, 28, 46, 64, 82, 100, 118, 136, 154, 162, 170, 204, 238, 272, 306, 316, 326};

typedef struct
{
    u8 data[IQ_FCC_END_SIZE];
} IQ_FCC_CFG;


typedef enum
{
    IQ_ACK_EN,
    IQ_ACK_C_COMP_EN,
    IQ_ACK_C_RANGE,
    IQ_ACK_C_THRD,
    IQ_ACK_LIMIT,
    IQ_ACK_Y_SLOP,
    IQ_ACK_Y_THRD,
    IQ_ACK_OFFSET,
    IQ_ACK_MAIN_Y_SWITCH_EN,
    IQ_ACK_U_SWITCH_COEF,
    IQ_ACK_V_SWITCH_COEF,
    IQ_ACK_CLAMP_EN,
    IQ_ACK_CB_MAX_CLAMP,
    IQ_ACK_CB_MIN_CLAMP,
    IQ_ACK_CR_MAX_CLAMP,
    IQ_ACK_CR_MIN_CLAMP,
    IQ_ACK_Y_MAX_CLAMP,
    IQ_ACK_Y_MIN_CLAMP,
    IQ_ACK_END,
    IQ_ACK_END_SIZE = 38,
} IQ_ACK_NAME;

static char nameACK[18][48] =
{
    "IQ_ACK_EN",
    "IQ_ACK_C_COMP_EN",
    "IQ_ACK_C_RANGE",
    "IQ_ACK_C_THRD",
    "IQ_ACK_LIMIT",
    "IQ_ACK_Y_SLOP",
    "IQ_ACK_Y_THRD",
    "IQ_ACK_OFFSET",
    "IQ_ACK_MAIN_Y_SWITCH_EN",
    "IQ_ACK_U_SWITCH_COEF",
    "IQ_ACK_V_SWITCH_COEF",
    "IQ_ACK_CLAMP_EN",
    "IQ_ACK_CB_MAX_CLAMP",
    "IQ_ACK_CB_MIN_CLAMP",
    "IQ_ACK_CR_MAX_CLAMP",
    "IQ_ACK_CR_MIN_CLAMP",
    "IQ_ACK_Y_MAX_CLAMP",
    "IQ_ACK_Y_MIN_CLAMP",
};

static unsigned short iq_ACK_offset[IQ_ACK_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36};

typedef struct
{
    u8 data[IQ_ACK_END_SIZE];
} IQ_ACK_CFG;


typedef enum
{
    IQ_YEE_YNR_EE_EN,
    IQ_YEE_EYEE_EN,
    IQ_YEE_Y_CLIP,
    IQ_YEE_DIR_THL,
    IQ_YEE_DIR_THW,
    IQ_YEE_YNR_DIR_THW,
    IQ_YEE_YNR_COF_DIR,
    IQ_YEE_YNR_COF_ALL,
    IQ_YEE_COR_PGAIN,
    IQ_YEE_COR_MGAIN,
    IQ_YEE_SCL_PGAIN,
    IQ_YEE_SCL_MGAIN,
    IQ_YEE_COF_ALL,
    IQ_YEE_COF_DIR,
    IQ_YEE_DTL_EN,
    IQ_YEE_DTL_THL,
    IQ_YEE_DTL_THW,
    IQ_YEE_EDGE_LV_BASE,
    IQ_YEE_COR_SFT_X,
    IQ_YEE_COR_P,
    IQ_YEE_COR_M,
    IQ_YEE_SCALE_SFT_X,
    IQ_YEE_SCALE_P,
    IQ_YEE_SCALE_M,
    IQ_YEE_ETC_SFT_X,
    IQ_YEE_ETC_P,
    IQ_YEE_ETC_M,
    IQ_YEE_YNR_DG_EN,
    IQ_YEE_YNR_DIST_SFT_X,
    IQ_YEE_YNR_DIST_LUT_Y,
    IQ_YEE_YNR_EE_VGAIN,
    IQ_YEE_YNR_EE_HGAIN,
    IQ_YEE_YNR_EE_NGAIN,
    IQ_YEE_YNR_EE_ZGAIN,
    IQ_YEE_YNR_EE_OPT,
    IQ_YEE_EYEE_HPF_ENG_GAIN,
    IQ_YEE_EYEE_BPF_ENG_GAIN,
    IQ_YEE_EYEE_SFT,
    IQ_YEE_EYEE_SBL_SFT_X,
    IQ_YEE_EYEE_SBL_LUT_Y,
    IQ_YEE_EYEE_COR_SFT_X,
    IQ_YEE_EYEE_COR_LUT_P,
    IQ_YEE_EYEE_COR_LUT_N,
    IQ_YEE_EYEE_SCA_SFT_X,
    IQ_YEE_EYEE_SCA_LUT_P,
    IQ_YEE_EYEE_SCA_LUT_N,
    IQ_YEE_EYEE_GBL_PGAIN,
    IQ_YEE_EYEE_GBL_NGAIN,
    IQ_YEE_EYEE_WA_GAIN,
    IQ_YEE_EYEE_WB_GAIN,
    IQ_YEE_EYEE_WA_CLIP_P,
    IQ_YEE_EYEE_WA_CLIP_M,
    IQ_YEE_EYEE_WB_CLIP_P,
    IQ_YEE_EYEE_WB_CLIP_M,
    IQ_YEE_Y2ES_EDGE_LMPT,
    IQ_YEE_Y2ES_EDGE_LMPM,
    IQ_YEE_END,
    IQ_YEE_END_SIZE = 290,
} IQ_YEE_NAME;

static char nameYEE[56][48] =
{
    "IQ_YEE_YNR_EE_EN",
    "IQ_YEE_EYEE_EN",
    "IQ_YEE_Y_CLIP",
    "IQ_YEE_DIR_THL",
    "IQ_YEE_DIR_THW",
    "IQ_YEE_YNR_DIR_THW",
    "IQ_YEE_YNR_COF_DIR",
    "IQ_YEE_YNR_COF_ALL",
    "IQ_YEE_COR_PGAIN",
    "IQ_YEE_COR_MGAIN",
    "IQ_YEE_SCL_PGAIN",
    "IQ_YEE_SCL_MGAIN",
    "IQ_YEE_COF_ALL",
    "IQ_YEE_COF_DIR",
    "IQ_YEE_DTL_EN",
    "IQ_YEE_DTL_THL",
    "IQ_YEE_DTL_THW",
    "IQ_YEE_EDGE_LV_BASE",
    "IQ_YEE_COR_SFT_X",
    "IQ_YEE_COR_P",
    "IQ_YEE_COR_M",
    "IQ_YEE_SCALE_SFT_X",
    "IQ_YEE_SCALE_P",
    "IQ_YEE_SCALE_M",
    "IQ_YEE_ETC_SFT_X",
    "IQ_YEE_ETC_P",
    "IQ_YEE_ETC_M",
    "IQ_YEE_YNR_DG_EN",
    "IQ_YEE_YNR_DIST_SFT_X",
    "IQ_YEE_YNR_DIST_LUT_Y",
    "IQ_YEE_YNR_EE_VGAIN",
    "IQ_YEE_YNR_EE_HGAIN",
    "IQ_YEE_YNR_EE_NGAIN",
    "IQ_YEE_YNR_EE_ZGAIN",
    "IQ_YEE_YNR_EE_OPT",
    "IQ_YEE_EYEE_HPF_ENG_GAIN",
    "IQ_YEE_EYEE_BPF_ENG_GAIN",
    "IQ_YEE_EYEE_SFT",
    "IQ_YEE_EYEE_SBL_SFT_X",
    "IQ_YEE_EYEE_SBL_LUT_Y",
    "IQ_YEE_EYEE_COR_SFT_X",
    "IQ_YEE_EYEE_COR_LUT_P",
    "IQ_YEE_EYEE_COR_LUT_N",
    "IQ_YEE_EYEE_SCA_SFT_X",
    "IQ_YEE_EYEE_SCA_LUT_P",
    "IQ_YEE_EYEE_SCA_LUT_N",
    "IQ_YEE_EYEE_GBL_PGAIN",
    "IQ_YEE_EYEE_GBL_NGAIN",
    "IQ_YEE_EYEE_WA_GAIN",
    "IQ_YEE_EYEE_WB_GAIN",
    "IQ_YEE_EYEE_WA_CLIP_P",
    "IQ_YEE_EYEE_WA_CLIP_M",
    "IQ_YEE_EYEE_WB_CLIP_P",
    "IQ_YEE_EYEE_WB_CLIP_M",
    "IQ_YEE_Y2ES_EDGE_LMPT",
    "IQ_YEE_Y2ES_EDGE_LMPM",
};

static unsigned short iq_YEE_offset[IQ_YEE_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 46, 58, 70, 80, 92, 104, 114, 126, 138, 140, 150, 162, 164, 166, 168, 170, 172, 174, 176, 178, 188, 200, 210, 222, 234, 244, 256, 268, 270, 272, 274, 276, 278, 280, 282, 284, 286, 288};

typedef struct
{
    u8 data[IQ_YEE_END_SIZE];
} IQ_YEE_CFG;


typedef enum
{
    IQ_ACLUT_SRC_SEL,
    IQ_ACLUT_2DPK_LUT,
    IQ_ACLUT_2DPK_BASE,
    IQ_ACLUT_YEE_LUT,
    IQ_ACLUT_YEE_BASE,
    IQ_ACLUT_END,
    IQ_ACLUT_END_SIZE = 72,
} IQ_ACLUT_NAME;

static char nameACLUT[5][48] =
{
    "IQ_ACLUT_SRC_SEL",
    "IQ_ACLUT_2DPK_LUT",
    "IQ_ACLUT_2DPK_BASE",
    "IQ_ACLUT_YEE_LUT",
    "IQ_ACLUT_YEE_BASE",
};

static unsigned short iq_ACLUT_offset[IQ_ACLUT_END + 1] = {0, 2, 34, 36, 68, 70};

typedef struct
{
    u8 data[IQ_ACLUT_END_SIZE];
} IQ_ACLUT_CFG;


typedef enum
{
    IQ_WDR_GAIN_EN,
    IQ_WDR_GAIN_SMOOTH_EN,
    IQ_WDR_GAIN_BASE,
    IQ_WDR_GAIN_LW,
    IQ_WDR_GAIN_HI,
    IQ_WDR_GAIN_WEIGHT,
    IQ_WDR_GAIN_CURVE_X,
    IQ_WDR_GAIN_CURVE,
    IQ_WDR_GBL_UVTBL0_X,
    IQ_WDR_GBL_UVTBL0_Y,
    IQ_WDR_GBL_UVTBL1_X,
    IQ_WDR_GBL_UVTBL1_Y,
    IQ_WDR_GBL_CBCR_MODE,
    IQ_WDR_GBL_END,
    IQ_WDR_GBL_END_SIZE = 166,
} IQ_WDR_GBL_NAME;

static char nameWDR_GBL[13][48] =
{
    "IQ_WDR_GAIN_EN",
    "IQ_WDR_GAIN_SMOOTH_EN",
    "IQ_WDR_GAIN_BASE",
    "IQ_WDR_GAIN_LW",
    "IQ_WDR_GAIN_HI",
    "IQ_WDR_GAIN_WEIGHT",
    "IQ_WDR_GAIN_CURVE_X",
    "IQ_WDR_GAIN_CURVE",
    "IQ_WDR_GBL_UVTBL0_X",
    "IQ_WDR_GBL_UVTBL0_Y",
    "IQ_WDR_GBL_UVTBL1_X",
    "IQ_WDR_GBL_UVTBL1_Y",
    "IQ_WDR_GBL_CBCR_MODE",
};

static unsigned short iq_WDR_GBL_offset[IQ_WDR_GBL_END + 1] = {0, 2, 4, 6, 8, 10, 12, 52, 94, 110, 128, 144, 162, 164};

typedef struct
{
    u8 data[IQ_WDR_GBL_END_SIZE];
} IQ_WDR_GBL_CFG;


typedef enum
{
    IQ_WDR_LOC_EN,
    IQ_WDR_LOC_WIN_WIDTH,
    IQ_WDR_LOC_WIN_HEIGHT,
    IQ_WDR_LOC_VALID_X,
    IQ_WDR_LOC_VALID_Y,
    IQ_WDR_LOC_HE_CNT_CUR0,
    IQ_WDR_LOC_HE_CNT_CUR1,
    IQ_WDR_LOC_HE_CNT_CUR2,
    IQ_WDR_LOC_HE_CNT_CUR3,
    IQ_WDR_LOC_HE_BIN_CUR_0,
    IQ_WDR_LOC_HE_BIN_CUR_1,
    IQ_WDR_LOC_HE_BIN_CUR_2,
    IQ_WDR_LOC_HE_BIN_CUR_3,
    IQ_WDR_LOC_TMP_MAX_SHADE_GAIN,
    IQ_WDR_LOC_HE_GAIN_HI,
    IQ_WDR_LOC_HE_GAIN_L,
    IQ_WDR_LOC_HE_GAIN_WEIGHT,
    IQ_WDR_LOC_UVTBL0_X,
    IQ_WDR_LOC_UVTBL0_Y,
    IQ_WDR_LOC_UVTBL1_X,
    IQ_WDR_LOC_UVTBL1_Y,
    IQ_WDR_LOC_CBCR_MODE,
    IQ_WDR_LOC_GAIN_TBL,
    IQ_WDR_LOC_TAR_TBL,
    IQ_WDR_LOC_END,
    IQ_WDR_LOC_END_SIZE = 2734,
} IQ_WDR_LOC_NAME;

static char nameWDR_LOC[24][48] =
{
    "IQ_WDR_LOC_EN",
    "IQ_WDR_LOC_WIN_WIDTH",
    "IQ_WDR_LOC_WIN_HEIGHT",
    "IQ_WDR_LOC_VALID_X",
    "IQ_WDR_LOC_VALID_Y",
    "IQ_WDR_LOC_HE_CNT_CUR0",
    "IQ_WDR_LOC_HE_CNT_CUR1",
    "IQ_WDR_LOC_HE_CNT_CUR2",
    "IQ_WDR_LOC_HE_CNT_CUR3",
    "IQ_WDR_LOC_HE_BIN_CUR_0",
    "IQ_WDR_LOC_HE_BIN_CUR_1",
    "IQ_WDR_LOC_HE_BIN_CUR_2",
    "IQ_WDR_LOC_HE_BIN_CUR_3",
    "IQ_WDR_LOC_TMP_MAX_SHADE_GAIN",
    "IQ_WDR_LOC_HE_GAIN_HI",
    "IQ_WDR_LOC_HE_GAIN_L",
    "IQ_WDR_LOC_HE_GAIN_WEIGHT",
    "IQ_WDR_LOC_UVTBL0_X",
    "IQ_WDR_LOC_UVTBL0_Y",
    "IQ_WDR_LOC_UVTBL1_X",
    "IQ_WDR_LOC_UVTBL1_Y",
    "IQ_WDR_LOC_CBCR_MODE",
    "IQ_WDR_LOC_GAIN_TBL",
    "IQ_WDR_LOC_TAR_TBL",
};

static unsigned short iq_WDR_LOC_offset[IQ_WDR_LOC_END + 1] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 28, 38, 48, 58, 60, 64, 68, 70, 86, 104, 120, 138, 140, 1436, 2732};

typedef struct
{
    u8 data[IQ_WDR_LOC_END_SIZE];
} IQ_WDR_LOC_CFG;


typedef enum
{
    IQ_ADJUV_YUV_EN,
    IQ_ADJUV_UV_BY_Y_EN,
    IQ_ADJUV_UV_BY_S_OPT,
    IQ_ADJUV_Y_OPT,
    IQ_ADJUV_YUV_OPT,
    IQ_ADJUV_YUV_IN_SIGH,
    IQ_ADJUV_UV_BY_Y_SFT_X,
    IQ_ADJUV_UV_BY_UV_SHT_X,
    IQ_ADJUV_YUV_Y_SFT_X,
    IQ_ADJUV_YUV_UV_SFT_X,
    IQ_ADJUV_U_BY_Y_TBL_Y,
    IQ_ADJUV_V_BY_Y_TBL_Y,
    IQ_ADJUV_U_BY_UV_TBL_Y,
    IQ_ADJUV_V_BY_UV_TBL_Y,
    IQ_ADJUV_YUV_Y_TBL_Y,
    IQ_ADJUV_YUV_UV_TBL_Y,
    IQ_ADJUV_CORING_UV_BY_UV,
    IQ_ADJUV_END,
    IQ_ADJUV_END_SIZE = 138,
} IQ_ADJUV_NAME;

static char nameADJUV[17][48] =
{
    "IQ_ADJUV_YUV_EN",
    "IQ_ADJUV_UV_BY_Y_EN",
    "IQ_ADJUV_UV_BY_S_OPT",
    "IQ_ADJUV_Y_OPT",
    "IQ_ADJUV_YUV_OPT",
    "IQ_ADJUV_YUV_IN_SIGH",
    "IQ_ADJUV_UV_BY_Y_SFT_X",
    "IQ_ADJUV_UV_BY_UV_SHT_X",
    "IQ_ADJUV_YUV_Y_SFT_X",
    "IQ_ADJUV_YUV_UV_SFT_X",
    "IQ_ADJUV_U_BY_Y_TBL_Y",
    "IQ_ADJUV_V_BY_Y_TBL_Y",
    "IQ_ADJUV_U_BY_UV_TBL_Y",
    "IQ_ADJUV_V_BY_UV_TBL_Y",
    "IQ_ADJUV_YUV_Y_TBL_Y",
    "IQ_ADJUV_YUV_UV_TBL_Y",
    "IQ_ADJUV_CORING_UV_BY_UV",
};

static unsigned short iq_ADJUV_offset[IQ_ADJUV_END + 1] = {0, 2, 4, 6, 8, 10, 12, 22, 32, 42, 52, 64, 76, 88, 100, 112, 124, 136};

typedef struct
{
    u8 data[IQ_ADJUV_END_SIZE];
} IQ_ADJUV_CFG;


typedef enum
{
    IQ_MXNR_EN,
    IQ_MXNR_IN_SIGH,
    IQ_MXNR_MATCH_CNT,
    IQ_MXNR_UTH,
    IQ_MXNR_VTH,
    IQ_MXNR_CNT_TH,
    IQ_MXNR_WEI_Y,
    IQ_MXNR_END,
    IQ_MXNR_END_SIZE = 26,
} IQ_MXNR_NAME;

static char nameMXNR[7][48] =
{
    "IQ_MXNR_EN",
    "IQ_MXNR_IN_SIGH",
    "IQ_MXNR_MATCH_CNT",
    "IQ_MXNR_UTH",
    "IQ_MXNR_VTH",
    "IQ_MXNR_CNT_TH",
    "IQ_MXNR_WEI_Y",
};

static unsigned short iq_MXNR_offset[IQ_MXNR_END + 1] = {0, 2, 4, 6, 8, 10, 12, 24};

typedef struct
{
    u8 data[IQ_MXNR_END_SIZE];
} IQ_MXNR_CFG;


typedef enum
{
    IQ_YUVGAMA_EN,
    IQ_YUVGAMA_CBCR_MODE,
    IQ_YUVGAMA_LUT_Y,
    IQ_YUVGAMA_LUT_U,
    IQ_YUVGAMA_LUT_V,
    IQ_YUVGAMA_MAX_EN,
    IQ_YUVGAMA_MAX_Y,
    IQ_YUVGAMA_MAX_U,
    IQ_YUVGAMA_MAX_V,
    IQ_YUVGAMA_END,
    IQ_YUVGAMA_END_SIZE = 1038,
} IQ_YUVGAMA_NAME;

static char nameYUVGAMA[9][48] =
{
    "IQ_YUVGAMA_EN",
    "IQ_YUVGAMA_CBCR_MODE",
    "IQ_YUVGAMA_LUT_Y",
    "IQ_YUVGAMA_LUT_U",
    "IQ_YUVGAMA_LUT_V",
    "IQ_YUVGAMA_MAX_EN",
    "IQ_YUVGAMA_MAX_Y",
    "IQ_YUVGAMA_MAX_U",
    "IQ_YUVGAMA_MAX_V",
};

static unsigned short iq_YUVGAMA_offset[IQ_YUVGAMA_END + 1] = {0, 2, 4, 516, 772, 1028, 1030, 1032, 1034, 1036};

typedef struct
{
    u8 data[IQ_YUVGAMA_END_SIZE];
} IQ_YUVGAMA_CFG;


typedef enum
{
    IQ_YC10_BYPASS_EN,
    IQ_YC10_CBCR_MODE,
    IQ_YC10_GAIN,
    IQ_YC10_OFFSET,
    IQ_UVM10_M,
    IQ_YCUVM10_END,
    IQ_YCUVM10_END_SIZE = 18,
} IQ_YCUVM10_NAME;

static char nameYCUVM10[5][48] =
{
    "IQ_YC10_BYPASS_EN",
    "IQ_YC10_CBCR_MODE",
    "IQ_YC10_GAIN",
    "IQ_YC10_OFFSET",
    "IQ_UVM10_M",
};

static unsigned short iq_YCUVM10_offset[IQ_YCUVM10_END + 1] = {0, 2, 4, 6, 8, 16};

typedef struct
{
    u8 data[IQ_YCUVM10_END_SIZE];
} IQ_YCUVM10_CFG;


typedef enum
{
    IQ_COLORTRANS_MIX_EN,
    IQ_COLORTRANS_PREOFFSET,
    IQ_COLORTRANS_M,
    IQ_COLORTRANS_Y_OFFSET,
    IQ_COLORTRANS_U_OFFSET,
    IQ_COLORTRANS_V_OFFSET,
    IQ_COLORTRANS_END,
    IQ_COLORTRANS_END_SIZE = 30,
} IQ_COLORTRANS_NAME;

static char nameCOLORTRANS[6][48] =
{
    "IQ_COLORTRANS_MIX_EN",
    "IQ_COLORTRANS_PREOFFSET",
    "IQ_COLORTRANS_M",
    "IQ_COLORTRANS_Y_OFFSET",
    "IQ_COLORTRANS_U_OFFSET",
    "IQ_COLORTRANS_V_OFFSET",
};

static unsigned short iq_COLORTRANS_offset[IQ_COLORTRANS_END + 1] = {0, 2, 4, 22, 24, 26, 28};

typedef struct
{
    u8 data[IQ_COLORTRANS_END_SIZE];
} IQ_COLORTRANS_CFG;


typedef enum
{
    IQ_TC_EN,
    IQ_TC_GAIN,
    IQ_TC_TEMPTURE,
    IQ_TC_SATURATION,
    IQ_TC_OB,
    IQ_TC_CROSSTALK,
    IQ_TC_DYNAMICDP,
    IQ_TC_NR3D,
    IQ_TC_NR2D,
    IQ_TC_SHARPNESS,
    IQ_TC_COLORTONE,
    IQ_TC_INFO_END,
    IQ_TC_INFO_END_SIZE = 306,
} IQ_TC_INFO_NAME;

static char nameTC_INFO[11][48] =
{
    "IQ_TC_EN",
    "IQ_TC_GAIN",
    "IQ_TC_TEMPTURE",
    "IQ_TC_SATURATION",
    "IQ_TC_OB",
    "IQ_TC_CROSSTALK",
    "IQ_TC_DYNAMICDP",
    "IQ_TC_NR3D",
    "IQ_TC_NR2D",
    "IQ_TC_SHARPNESS",
    "IQ_TC_COLORTONE",
};

static unsigned short iq_TC_INFO_offset[IQ_TC_INFO_END + 1] = {0, 32, 96, 128, 160, 192, 224, 256, 288, 292, 298, 304};

typedef struct
{
    u8 data[IQ_TC_INFO_END_SIZE];
} IQ_TC_INFO_CFG;


typedef enum
{
    IQ_ITP_FPN,
    IQ_ITP_SDC,
    IQ_ITP_DPC,
    IQ_ITP_GE,
    IQ_ITP_ANTICT,
    IQ_ITP_RGBIR,
    IQ_ITP_OBC,
    IQ_ITP_GAMA_C2A_ISP,
    IQ_ITP_LSC,
    IQ_ITP_ALSC,
    IQ_ITP_BDNR,
    IQ_ITP_SpikeNR,
    IQ_ITP_BSNR,
    IQ_ITP_NM,
    IQ_ITP_DM,
    IQ_ITP_PostDN,
    IQ_ITP_FalseColor,
    IQ_ITP_GAMA_A2A_ISP,
    IQ_ITP_MCNR,
    IQ_ITP_NLM,
    IQ_ITP_XNR,
    IQ_ITP_LDC,
    IQ_ITP_GAMA_A2C_SC,
    IQ_ITP_CCM,
    IQ_ITP_HSV,
    IQ_ITP_GAMA_C2A_SC,
    IQ_ITP_R2Y,
    IQ_ITP_TwoDPK,
    IQ_ITP_LCE,
    IQ_ITP_DLC,
    IQ_ITP_UVC,
    IQ_ITP_IHC,
    IQ_ITP_ICC,
    IQ_ITP_IBC,
    IQ_ITP_FCC,
    IQ_ITP_ACK,
    IQ_ITP_YEE,
    IQ_ITP_ACLUT,
    IQ_ITP_WDR_GBL,
    IQ_ITP_WDR_LOC,
    IQ_ITP_ADJUV,
    IQ_ITP_MXNR,
    IQ_ITP_YUVGAMA,
    IQ_ITP_YCUVM10,
    IQ_ITP_COLORTRANS,
    IQ_ITP_TEMPTURE,
    IQ_ITP_END,
    IQ_ITP_END_SIZE = 1372,
} IQ_ITP_NAME;

static char nameITP[46][48] =
{
    "IQ_ITP_FPN",
    "IQ_ITP_SDC",
    "IQ_ITP_DPC",
    "IQ_ITP_GE",
    "IQ_ITP_ANTICT",
    "IQ_ITP_RGBIR",
    "IQ_ITP_OBC",
    "IQ_ITP_GAMA_C2A_ISP",
    "IQ_ITP_LSC",
    "IQ_ITP_ALSC",
    "IQ_ITP_BDNR",
    "IQ_ITP_SpikeNR",
    "IQ_ITP_BSNR",
    "IQ_ITP_NM",
    "IQ_ITP_DM",
    "IQ_ITP_PostDN",
    "IQ_ITP_FalseColor",
    "IQ_ITP_GAMA_A2A_ISP",
    "IQ_ITP_MCNR",
    "IQ_ITP_NLM",
    "IQ_ITP_XNR",
    "IQ_ITP_LDC",
    "IQ_ITP_GAMA_A2C_SC",
    "IQ_ITP_CCM",
    "IQ_ITP_HSV",
    "IQ_ITP_GAMA_C2A_SC",
    "IQ_ITP_R2Y",
    "IQ_ITP_TwoDPK",
    "IQ_ITP_LCE",
    "IQ_ITP_DLC",
    "IQ_ITP_UVC",
    "IQ_ITP_IHC",
    "IQ_ITP_ICC",
    "IQ_ITP_IBC",
    "IQ_ITP_FCC",
    "IQ_ITP_ACK",
    "IQ_ITP_YEE",
    "IQ_ITP_ACLUT",
    "IQ_ITP_WDR_GBL",
    "IQ_ITP_WDR_LOC",
    "IQ_ITP_ADJUV",
    "IQ_ITP_MXNR",
    "IQ_ITP_YUVGAMA",
    "IQ_ITP_YCUVM10",
    "IQ_ITP_COLORTRANS",
    "IQ_ITP_TEMPTURE",
};

//static unsigned short iq_ITP_offset[IQ_ITP_END+1] = {0,8,14,122,136,144,174,196,210,230,258,294,314,342,364,382,392,402,416,514,564,602,610,624,638,648,662,668,752,806,832,886,920,950,964,1008,1046,1160,1174,1202,1252,1288,1304,1324,1334,1348,1370};
static unsigned short iq_ITP_offset[IQ_ITP_END + 1] = {0, 4, 7, 61, 68, 72, 87, 98, 105, 115, 129, 147, 157, 171, 182, 191, 196, 201, 208, 257, 282, 301, 305, 312, 319, 324, 331, 334, 376, 403, 416, 443, 460, 475, 482, 504, 523, 580, 587, 601, 626, 644, 652, 662, 667, 674, 685};

typedef struct
{
    u8 data[IQ_ITP_END_SIZE];
} IQ_ITP_CFG;


#endif /* __iq_regset__ */


#endif
