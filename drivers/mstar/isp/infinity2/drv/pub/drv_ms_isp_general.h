/*
 * mstar_isp_general.h
 *
 *  Created on: Apr 19, 2011
 *      Author: edwin.yang
 */

#ifndef MSTAR_ISP_GENERAL_H_
#define MSTAR_ISP_GENERAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <isp_types.h>

#define FAIL                    (-1)
#define SUCCESS                 (0)

#define CAMERA_ISR_FREQ_SEC     (0)
#define CAMERA_ISR_FREQ_NSEC    (10000000) //(1000*1000*1000) // 1sec

#define PACK_ALIGN_TAG(n)       __attribute__((packed, aligned(n)))
#define LIMIT_BITS(x, bits)     ((x) > ((1<<(bits))-1) ? (((1<<(bits))-1)) : (x))
typedef void *(*THREAD_P)(void *);

#define AE_WIN_MAX_WIDTH        (128)//(16)
#define AE_WIN_MAX_HEIGHT       (90)//(24)
#define AE_HIST_WIN_MAX_WIDTH   (128)
#define AE_HIST_WIN_MAX_HEIGHT  (90)
#define AE_HIST_BIN0            (40)
#define AE_HIST_BINX            (128)
#define AE_IR_HIST_BIN          (256)

#define AWB_WIN_MAX_WIDTH       (128)
#define AWB_WIN_MAX_HEIGHT      (90)
#define AWB_BLOCK_MIN_VALID_X   (4)
#define AWB_BLOCK_MIN_VALID_Y   (2)

#define HDR_STATS_LEN           (1024)

typedef struct
{
  short mload_rdata[3];

} ISP_MLOAD_OUTPUT;


// crop range
typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned int degree;
    char         mirror_flip;
} ISP_CROP_RANGE;

// input source/output destination
typedef enum
{
    // source
    ISP_DATA_SRC_TV,
    ISP_DATA_SRC_SENSOR,
    ISP_DATA_SRC_RDMA,

    // destination
    ISP_DATA_DST_ICP,
    ISP_DATA_DST_WDMA_SIF,
    ISP_DATA_DST_WDMA_ISP,
    ISP_DATA_DST_WDMA_ISPDS,
    ISP_DATA_DST_WDMA_ISPSWAPYC,
    ISP_DATA_DST_WDMA_VDOS
} ISP_DATA_SRCDST;
typedef enum
{
    ISP_SRC_SENSOR,
    ISP_SRC_RDMA,
} ISP_DATA_SRC;

typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} ISP_WDMA_CROP_RANGE;
// color space
typedef enum
{
    ISP_COLOR_SPACE_YUV,
    ISP_COLOR_SPACE_BAYER,
} ISP_COLOR_SPACE;

// data precision
typedef enum
{
    ISP_DATAPRECISION_8 = 0,
    ISP_DATAPRECISION_10 = 1,
    ISP_DATAPRECISION_16 = 2,
    ISP_DATAPRECISION_12 = 3,
    ISP_DATAPRECISION_1T4P_8 = 4,
    ISP_DATAPRECISION_1T4P_10 = 5,
    ISP_DATAPRECISION_1T4P_16 = 6,
    ISP_DATAPRECISION_1T4P_12 = 7
} ISP_DATAPRECISION;

typedef enum
{
    BAYER_RG = 0,
    BAYER_GR = 1,
    BAYER_BG = 2,
    BAYER_GB = 3
} SEN_BAYER;

typedef enum
{
    RGBIR_R0 = 0,
    RGBIR_G0 = 1,
    RGBIR_B0 = 2,
    RGBIR_G1 = 3,
    RGBIR_G2 = 4,
    RGBIR_I0 = 5,
    RGBIR_G3 = 6,
    RGBIR_I1 = 7


} SEN_RGBIR;
// input/output configuration
typedef struct
{
    ISP_CROP_RANGE          crop_range;
    ISP_DATA_SRCDST         srcdst;
    ISP_DATAPRECISION       data_precision;
    ISP_COLOR_SPACE         colorspace;
} ISP_INOUT_CONFIG;

// data control path
typedef enum
{
    ISP_CTL_PATH_SENSOR_ISP_ICP,                        // preview
    ISP_CTL_PATH_SENSOR_WDMA_RDMA_ISP_ICP,              // preview with ring buffer
    ISP_CTL_PATH_SENSOR_WDMA,                           // capture raw
    ISP_CTL_PATH_RDMA_ISPDS_WDMA,                       // iterative processing raw
    ISP_CTL_PATH_RDMA_ISPDP_WDMA,                       // software trigger capture
    ISP_CTL_PATH_RDMA_ISP_ICP                           // software trigger capture
} ISP_CTL_PATH;

/*
typedef enum {
    CAMERA_ID_REAR1     = 0,
    CAMERA_ID_REAR2     = 1,
    CAMERA_ID_FRONT1    = 2,
    CAMERA_ID_FRONT2    = 3,
    CAMERA_ID_TV1       = 4,
    CAMERA_ID_TV2       = 5,
    CAMERA_ID_PG        = 6,
    CAMERA_ID_AUTO      = 7,
    CAMERA_ID_END       = 8
} CameraModule_id_t;
*/

typedef enum
{
    IQ_OBC,
    IQ_WBG,
    IQ_WBG0,
    IQ_WBG1,
    IQ_LSC,
    IQ_CROSSTALK,
    IQ_AEAWB_STATS,
    IQ_DPC,
    IQ_SPIKE_NR,
    IQ_MEAN_FILTER,     // mean filter
    IQ_NOISE_MASKING,   // noise masking
    IQ_FALSECOLOR,
    IQ_CFAi,            // De-Mosaic
    IQ_CFAi_NR,         // Post Denoise
    IQ_DNR,
    IQ_DNR_LUM,
    IQ_AF_STATS,
    IQ_CCM,
    IQ_UVCM,
    IQ_GAMMA_RAW_LC,
    IQ_GAMMA_RGB_LC,
    IQ_GAMMA,
    IQ_ALSC,
    IQ_MOTION,
    IQ_ROTATE,
    IQ_END
} ISP_IQ_BLK;

typedef enum
{
    ISP_WDMA_ICP,
    ISP_WDMA_SIF,
    ISP_WDMA_ISP,
    ISP_WDMA_ISPDS,
    ISP_WDMA_ISPSWAPYC,
    ISP_WDMA_VDOS,
    ISP_WDMA_DEFAULT_SIF,
    ISP_WDMA_DEFAULT_YC
} WDMA_PATH;

typedef struct
{
    MS_U32 width;
    MS_U32 height;
    MS_U32 x;
    MS_U32 y;
    WDMA_PATH wdma_path;
    MS_U32  miu_addr;
} PACK_ALIGN_TAG(1) ISP_WDMA_ATTR;

// AE functions
typedef struct
{
    MS_U16 u2BlkSize_x;
    MS_U16 u2BlkSize_y;
    MS_U16 u2BlkNum_x;
    MS_U16 u2BlkNum_y;
} ISP_AE_WIN;

typedef struct
{
    MS_U16 Rgbir_stats[AE_IR_HIST_BIN];
} ISP_RGBIR_STATS;

typedef struct
{
    MS_U16 HDR_stats[HDR_STATS_LEN];
} ISP_HDR_STATS;

typedef struct
{
    MS_U8  uAvgR;
    MS_U8  uAvgG;
    MS_U8  uAvgB;
    MS_U8  uAvgY;
} ISP_AE_AVGS;

typedef struct
{
    MS_U16 u2HistR[AE_HIST_BIN0];
    MS_U16 u2HistG[AE_HIST_BIN0];
    MS_U16 u2HistB[AE_HIST_BIN0];
} ISP_AE_HIST0;

typedef struct
{
    MS_U16 u2HistY[AE_HIST_BINX];
} ISP_AE_HISTX;

typedef struct
{
    ISP_AE_HIST0    Hist0;
    ISP_AE_HISTX    Hist1;
    ISP_AE_HISTX    Hist2;
    ISP_RGBIR_STATS HistIR;
    ISP_AE_AVGS     Avgs[AE_WIN_MAX_WIDTH * AE_WIN_MAX_HEIGHT];
} PACK_ALIGN_TAG(16) ISP_AE_STATIS;

#if 0
// crop config, isp_crop_cfg
typedef struct {
    int x_start;
    int y_start;
    int width;
    int height;
} ISP_CROP_WIN, IspCropWin_t;
#endif

#if 0
typedef struct {
    MS_U16     u2BlkNum_x;
    MS_U16     u2BlkNum_y;
    MS_U16     u2BlkVaild_x;
    MS_U16     u2BlkVaild_y;
    MS_U16     u2BlkValidLowLux_x;
    MS_U16     u2BlkValidLowLux_y;
} ISP_AWB_WIN, IspAwbWin_t;
#endif

typedef struct
{
    u16 u2Stawin_x_offset;
    u16 u2Stawin_x_size;
    u16 u2Stawin_y_offset;
    u16 u2Stawin_y_size;
    //MS_BOOL roi_mode;
} ISP_HISTO_WIN;

typedef struct
{
    u8  acc_sft;
    u16 u2BlkSize_x;
    u16 u2BlkSize_y;
    u16 u2BlkNum_x;
    u16 u2BlkNum_y;
    u16 u2Blk_h_offset;
    u16 u2Blk_v_offset;
} ISP_HISTO_ATTR;

typedef struct
{
    MS_U16  nRgain;     /** R  channel */
    MS_U16  nGrgain;    /** Gr channel */
    MS_U16  nGbgain;    /** Gb channel */
    MS_U16  nBgain;     /** B  channel */
} IspAwbGain_t;

typedef struct {
    MS_U8  uAvgR;
    MS_U8  uAvgG;
    MS_U8  uAvgB;
} PACK_ALIGN_TAG(1) ISP_AWB_AVGS, IspAwbBlkAvg_t;

typedef struct {
    IspAwbBlkAvg_t tRow[AWB_WIN_MAX_WIDTH];
} PACK_ALIGN_TAG(16) IspAwbAvgRow_t;

typedef struct {
    ISP_AWB_AVGS Avgs[AWB_WIN_MAX_WIDTH * AWB_WIN_MAX_HEIGHT];
    //IspAwbAvgRow_t Avgs[AWB_WIN_MAX_HEIGHT];
} PACK_ALIGN_TAG(16) ISP_AWB_STATIS,IspAwbStatis_t;

typedef enum
{
    SIF_BUS_PARL = 0,
    SIF_BUS_MIPI = 1
} SIF_BUS;

typedef enum
{
    CLK_POL_POS = 0,
    CLK_POL_NEG
} CLK_POL;

typedef enum
{
    MLOAD_ID_PIPE0_FPN = 0,         //pipe0 FPN
    MLOAD_ID_PIPE0_GMA16TO16 = 1,   //pipe0 RGB gamma 16 to 16
    MLOAD_ID_PIPE0_ALSC = 2,        //pipe0 RGB lens shading
    MLOAD_ID_PIPE1_FPN = 4,         //pipe1 FPN
    MLOAD_ID_PIPE1_GMA12TO12 = 5,   //pipe1 RGB gamma 12 to 12
    MLOAD_ID_PIPE1_ALSC = 6,        //pipe1 RGB lens shading
    MLOAD_ID_DPC = 16,              //defect pixel
    MLOAD_ID_GMA10TO10 = 17,         //pipe0 RGB gamma 10 to 10 table
    MLOAD_ID_NUM = 18,
    SCL_MLOAD_ID_BASE = 32,
    SCL_MLOAD_ID_YUV_GAMMA_Y = 38,
    SCL_MLOAD_ID_YUV_GAMMA_UV = 39,
    SCL_MLOAD_ID_RGB10TO12 = 40,
    SCL_MLOAD_ID_RGB12TO10 = 41,
    SCL_MLOAD_ID_WDR_LOC1 = 42,
    SCL_MLOAD_ID_WDR_LOC2 = 43,
    SCL_MLOAD_ID_NUM = 44

}IspMloadID_e;

/////////////// For AWB library /////////////



#ifdef __cplusplus
}
#endif
#endif /* MSTAR_ISP_GENERAL_H_ */
