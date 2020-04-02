#ifndef HAL_ISP_H
#define HAL_ISP_H

#include <linux/types.h>
//#include <isp_types.h>
#include <infinity2_reg.h>
#include <drv_ms_isp_general.h>
//#include <cpu_mem_map.hc>

#define LIMIT_BITS(x, bits) ((x) > ((1<<(bits))-1) ? (((1<<(bits))-1)) : (x))
#define RSVD(ss,ee) u32 rsvd_##ss##_##ee[(ee) - (ss) + 1]
#define PAD16BITS  u32 :16
#define PACK_ALIGN_TAG(n)  __attribute__((packed, aligned(n)))

//AE statistics

#define AE_HIST_BIN     (64)
#define AE_STAT_WIDTH   (8)
#define AE_STAT_HEIGHT  (8)

#define AE_NUM_WIN          (2)
#define AE_NUM_HIST         (3)

typedef enum
{
    AE_HIST_FLARE,
    AE_HIST_WIN0,
    AE_HIST_WIN1,
} ISP_AE_HIST_TYPE;

typedef enum
{
    AE_STATIS_R,
    AE_STATIS_G,
    AE_STATIS_B,
    AE_STATIS_Y,
} ISP_AE_STATIS_COLOR;


#define DNR_PIXPERPACK  (12)
#define DNR_BITSPERPIX  (10)
#define DNR_MAX_WIDTH   (192)
#define DNR_MAX_HEIGHT  (16)
#define DNR_BYTEPERPACK (DNR_PIXPERPACK * DNR_BITSPERPIX / 8 + 1)
#define DNR_SIZE         DNR_BYTEPERPACK * 255

// MIU
#define GAMMA_TBL_SIZE                 (256*4)  //256  x 64 bits
#define ALSC_TBL_SIZE_W                (61)
#define ALSC_TBL_SIZE_H                (69)
#define ALSC_TBL_SIZE                  (4209*4) //4208 x 64 bits
#define DEFECTPIX_TBL_SIZE             (1024*2) //1024 x 32 bits
#define FPN_TBL_SIZE                   (2816)   //2816 x 16 bits

#define WDR_Loc1_SIZE                  (88*4)   //88   x 64bits
#define WDR_Loc2_SIZE                  (88*4)   //88   x 64bits
#define YUV_GAMMA_Y_SIZE               (256)    //289   x 16bits
#define YUV_GAMMA_UV_SIZE              (128*2)  //289   x 32bits
#define RGB10TO12_SIZE                 (256*4)  //289   x 64bits
#define RGB12TO10_SIZE                 (256*4)  //289   x 64bits

#define NLM_REGION_GAIN_ENTRY_NUM (1104)  //from apnote
#define SRAM_DATA_ENTRY_SIZE (128) // SRAM data define 128bit/1 entry

typedef struct
{
    u8  uDNRBuff[DNR_SIZE];
} ISP_DNR_STATIS;

#define MOT_MAX_WIDTH   (16)
#define MOT_MAX_HEIGHT  (16)

typedef struct
{
    u8  uMotOut[MOT_MAX_WIDTH * MOT_MAX_HEIGHT];
} PACK_ALIGN_TAG(16) ISP_MOT_STATIS;


/////////////////////////////// HAL /////////////////////////////
//typedef enum
//{
//    CLK_POL_POS = 0,
//    CLK_POL_NEG
//} CLK_POL;

// AF function
typedef enum
{
    AF_MODE_CONT,
    AF_MODE_ONCE
} ISP_AF_STATS_MODE;

typedef struct
{
    int x_start;
    int y_start;
    int x_end;
    int y_end;
} ISP_AF_WIN;


typedef struct
{
    u32 high_iir[10];
    u32 : 32;
    u32 : 32;
    u32 low_iir[10];
    u32 : 32;
    u32 : 32;
    u32 luma_iir[10];
    u32 : 32;
    u32 : 32;
    u32 sobelv[10];
    u32 : 32;
    u32 : 32;
    u32 sobelh[10];
    u32 : 32;
    u32 : 32;
} ISP_AF_STATS;


typedef struct
{
    u8 mot_block[28224];
} ISP_MOT_STATS;

typedef struct
{
    u16 burst_cnt;
    u16 thrd;
} ISP_MOT_ATTR;


typedef struct
{
    u16 roi_x_offset;
    u16 roi_x_size;
    u16 roi_y_offset;
    u16 roi_y_size;

    u16 blk_x_offset;
    u16 blk_y_offset;
    u16 blk_width;
    u16 blk_height;

    u32 roi_div;

} ISP_RGBIR_WIN;

typedef struct
{

    u8 ir_mode;
    u8 mode4x4;
    u8 rgbir_format; //range 0~7
    u8 ir_offset_en;
    u8 dir_scaler;
    u16 oft_cmp_ratio;
    u8 oft_ratio_by_y_shift_0;
    u8 oft_ratio_by_y_shift_1;
    u8 oft_ratio_by_y_shift_2;
    u8 oft_ratio_by_y_shift_3;
    u8 oft_ratio_by_y_shift_4;

    u16 oft_r_ratio_by_y_0;
    u16 oft_r_ratio_by_y_1;
    u16 oft_r_ratio_by_y_2;
    u16 oft_r_ratio_by_y_3;
    u16 oft_r_ratio_by_y_4;
    u16 oft_r_ratio_by_y_5;

    u16 oft_g_ratio_by_y_0;
    u16 oft_g_ratio_by_y_1;
    u16 oft_g_ratio_by_y_2;
    u16 oft_g_ratio_by_y_3;
    u16 oft_g_ratio_by_y_4;
    u16 oft_g_ratio_by_y_5;

    u16 oft_b_ratio_by_y_0;
    u16 oft_b_ratio_by_y_1;
    u16 oft_b_ratio_by_y_2;
    u16 oft_b_ratio_by_y_3;
    u16 oft_b_ratio_by_y_4;
    u16 oft_b_ratio_by_y_5;

} ISP_RGBIR_ATTR;

// AE/AWB functions
typedef struct
{
    u16 u2BlkSize_x;
    u16 u2BlkSize_y;
    u16 u2BlkNum_x;
    u16 u2BlkNum_y;
} ISP_AEAWB_WIN;

typedef enum
{
    AWB_SOURCE_BEFORE_DNR_WITH_LSC = 1,
    AWB_SOURCE_BEFORE_DNR_WITH_ALSC = 2,
    AWB_SOURCE_BEFORE_DNR_WITH_LSC_ALSC = 3,
    AWB_SOURCE_AFTER_DNR_WITH_LSC_ALSC = 8,
} ISP_AWB_SOURCE_SELECTION;

typedef enum
{
    AEAWB_MODE_CONT = 0,
    AEAWB_MODE_ONCE = 1
} ISP_AEAWB_STATS_MODE;

typedef struct
{
    u8 bEnable;
    u16 u2Stawin_x_offset;
    u16 u2Stawin_x_size;
    u16 u2Stawin_y_offset;
    u16 u2Stawin_y_size;
} ISP_AE_SUBWIN;

typedef enum
{
    eSUBWIN_0 = 0,
    eSUBWIN_1,
} ISP_AE_SUBWIN_IDX;

// crop config, isp_crop_cfg
typedef struct {
    int32_t x_start;
    int32_t y_start;
    int32_t width;
    int32_t height;
} ISP_CROP_WIN, IspCropWin_t;

typedef struct
{
    uint16_t    u2BlkNum_x;
    uint16_t     u2BlkNum_y;
    uint16_t     u2BlkVaild_x;
    uint16_t     u2BlkVaild_y;
    uint16_t     u2BlkValidLowLux_x;
    uint16_t     u2BlkValidLowLux_y;
} ISP_AWB_WIN, IspAwbWin_t;

//WDMA / RDMA
#if 0
// data precision
typedef enum
{
    ISP_DATAPRECISION_8,
    ISP_DATAPRECISION_10,
    ISP_DATAPRECISION_12,
    ISP_DATAPRECISION_16
} ISP_DATAPRECISION;
#endif
typedef enum
{
    PITCH_16BITS,
    PITCH_12BITS,
    PITCH_10BITS,
    PITCH_8BITS,
    PITCH_1T4P_16BITS,
    PITCH_1T4P_12BITS,
    PITCH_1T4P_10BITS,
    PITCH_1T4P_8BITS,
} DATA_PITCH;

//DATA MUX
// data source config, isp_src_cfg
typedef enum
{
    ISP_RMUX_SIF = 0,
    ISP_RMUX_RDMA
} ISP_RMUX;

typedef enum
{
    SIF_MUX_RMUX,
    SIF_MUX_SENSOR
} SIF_RVF;

//typedef enum
//{
//    SIF_BUS_PARL = 0,
//    SIF_BUS_MIPI = 1
//} SIF_BUS;

typedef enum
{
    ISP_WMUX_SIF = 0,
    ISP_WMUX_ISP,
    ISP_WMUX_ISPDS,
    ISP_WMUX_ISPSWAPYC,
    ISP_WMUX_VDOS
} ISP_WMUX;

typedef struct
{
    DATA_PITCH      pitch;
    unsigned int    addr;   // physical address
    ISP_CROP_WIN    crop_win;
    int         auto_mode;
    union
    {
        ISP_RMUX rmux;
        ISP_WMUX wmux;
    } mux;
} DMA_CFG;

// sensor IF setting, isp_sensorif_cfg
// color space
typedef enum
{
    SEN_DATATYPE_YUV = ISP_COLOR_SPACE_YUV,
    SEN_DATATYPE_BAYER = ISP_COLOR_SPACE_BAYER,
} SEN_DATATYPE;

typedef enum
{
    SEN_DATA_10B = 0,
    SEN_DATA_8B
} SEN_DATA_BITS;

typedef enum
{
    SEN_NO_SHIFT = 0,
    SEN_SHIFT_2BIT,
    SEN_SHIFT_4BIT,
    SEN_SHIFT_6BIT,
} SEN_LEFT_SHIFT;

typedef enum
{
    SEN_8TO12_7074,
    SEN_8TO12_7000,
    SEN_8TO12_114118,
    SEN_8TO12_11400,
    SEN_10TO12_9098,
    SEN_10TO12_9000,
    SEN_10TO12_1121110,
    SEN_10TO12_11200
} SEN_FMT_CONV_MODE;

typedef enum
{
    SEN_YCODR_CY = 0,
    SEN_YCODR_YC
} SEN_YC_ORDER;

typedef enum
{
    SEN_VSYNC_DLY_VF = 0,
    SEN_VSYNC_DLY_VR2,
    SEN_VSYNC_DLY_VR1,
    SEN_VSYNC_DLY_VR
} SEN_VSYNC_DLY;

typedef enum
{
    SEN_HSYNC_DLY_HR = 0,
    SEN_HSYNC_DLY_HF
} SEN_HSYNC_DLY;

//Color IP
typedef enum
{
    eCCM_RGB,  // RGB2RGB
    eCCM_YUV,  // RGB2YUV
} ISP_CCM_IP;

//CCM regs
typedef struct
{
    u32 val                             : 13; //0x32
    u32 /* reserved */                  : 3;
    PAD16BITS;
} PACK_ALIGN_TAG(1) isp_ccm_coeff;

// gamma config, isp_gamma_cfg
typedef enum
{
    eGAMMA_RAW_LC,  // linear correction
    eGAMMA_RGB_LC,  // linear correction
    eGAMMA_CCM
} ISP_GAMMA_IP;

typedef struct
{
    ISP_GAMMA_IP  eID;
    u8 bEnable;
    u8 bLimitMax;
    u32  u4BoundMax;
    u8 bDither;
} ISP_GAMMA_CFG;;

typedef enum
{
    eGAMMA_COLOR_R,
    eGAMMA_COLOR_G,
    eGAMMA_COLOR_B,
} ISP_GAMMA_COLOR;

typedef struct
{
    ISP_CCM_IP  eID;
    u8 bEnable;
    u8  bDitherEnable;
    s16   i2CCM[9];

    u8 by_sub_16_en;
    u8 br_sub_16_en;
    u8 bb_sub_16_en;
    u8 by_add_16_post_en;
    u8 br_add_16_post_en;
    u8 bb_add_16_post_en;
    u8 bcb_add_128_post_en;
    u8 bcr_add_128_post_en;
    u32 urran;
    u32 ugran;
    u32 ubran;
} ISP_CCM_CFG;

typedef struct
{
    int dither;
    int before_denoise;

    int r_gain;
    int r_offset;

    int gr_gain;
    int gr_offset;

    int b_gain;
    int b_offset;

    int gb_gain;
    int gb_offset;
} IQ_WBG_CFG;

typedef struct
{
    u32 cfai_en ;
    u32 cfai_bypass ;
    u32 dvh_slope   ;
    u32 dvh_thd ;
    u32 cpx_slope   ;
    u32 cpx_thd ;
    u32 nb_slope_m  ;
    u32 nb_slope_s  ;
} IQ_CFAI_CFG;

typedef struct
{
    u32 cfai_dnr_en ;
    u32 cfai_dnr_gavg_ref_en    ;
    u32 rb_ratio    ;
    u32 g_ratio ;
} IQ_CFAI_NR_CFG;

// Motion Vector
typedef struct
{
    u8 bEnable;
    u16 u2MotionThd;
} ISP_MOTION_CFG;

// Rotate
typedef struct
{
    u8 bEnable;
} ISP_ROTATE_CFG;

#if 0
typedef struct
{
    u16 eis_en                     ;
    u16 eis_data_fmt               ;
    u16 eis_sblock_blk             ;
    u16 eis_dummy                  ;
    u16 eis_x_st                   ;
    u16 eis_y_st                   ;
    u16 eis_sblock_width           ;
    u16 eis_sblock_height          ;
} ISP_EIS_CFG, EIS_CFG;
#endif
typedef enum  //G2E
{
    FRAME_WHOLE = 0,
    FRAME_1_4,
    FRAME_1_16,
    FRAME_1_64,
    FRAME_1_256,
    FRAMETBL_END
} ISP_AWBDSP_Mode, ISP_AWBDSP_MODE;

#if 0
typedef struct
{
    // dnr
    MS_BOOL      dnr_en                    ;
    MS_BOOL      dnr_lum_en                ;
    MS_BOOL      dnr_lum_sh                ;
    MS_BOOL      dnr_filter_en             ;
    u8        dnr_filter_mode           ;
    u8        dnr_filter_div0           ;
    u8        dnr_filter_div1           ;
    u8        dnr_lum_table[8];
    MS_BOOL      dnr_table_sel             ;
    MS_BOOL      dnr_nondither_method      ;
    MS_BOOL      dnr_nondither_method_mode ;
    u8        round_mode                ;
    u8        sticky_solver_th          ;
    MS_BOOL      sticky_solver_en          ;
    u8        dnr_tabley[16]      ;
    u8        dnr_ofrm_sel              ;

    //spike NR
    MS_BOOL     spike_nr_en;
    u8       spike_nr_coef;
    u8       p_thrd;
    u8       p_thrd_1;
    u8       p_thrd_2;
    u8       p_thrd_3;
    u8       d_11_21_step;
    u8       d_31_step;
    u8       yp_22_step;

    // mean filter
    MS_BOOL     snr_en;
    u8       snr_std_low_thrd;
    u8       snr_strength_gain;
    u8       snr_alpha_step;
    u8       snr_table[16];

    u8       r_snr_std_low_thrd;
    u8       r_snr_strength_gain;
    u8       r_snr_alpha_step;
    u8       r_snr_table[16];

    u8       b_snr_std_low_thrd;
    u8       b_snr_strength_gain;
    u8       b_snr_alpha_step;
    u8       b_snr_table[16];

    // noise masking
    MS_BOOL     snr_nm_filter_en;
    u8       snr_nm_gain;
    u8       snr_nm_max_thrd;
    u8       snr_nm_min_thrd;

    u8       r_snr_nm_gain;
    u8       r_snr_nm_max_thrd;
    u8       r_snr_nm_min_thrd;

    u8       b_snr_nm_gain;
    u8       b_snr_nm_max_thrd;
    u8       b_snr_nm_min_thrd;
} IQ_DENOISE_CFG;
#endif

typedef enum
{
    INPUT_PIXEL_COUNT = 0xC,
    INPUT_LIINE_COUNT = 0xD,
    OUTPUT_LINE_COUNT = 0xF,
} ISP_DBG_SEL;

#if 1
/////////////// MIU MenuLoad ///////////
typedef struct
{
    u16 pipe0_fpn[FPN_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_gamma16to16[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_alsc[ALSC_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_fpn[FPN_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_gamma12to12[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_alsc[ALSC_TBL_SIZE] __attribute__((aligned(16)));
    u16 dpc[DEFECTPIX_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_gamma10to10_r[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 wdr_loc1[WDR_Loc1_SIZE] __attribute__((aligned(16)));
    u16 wdr_loc2[WDR_Loc2_SIZE] __attribute__((aligned(16)));
    u16 yuv_gamma_y[YUV_GAMMA_Y_SIZE] __attribute__((aligned(16)));
    u16 yuv_gamma_uv[YUV_GAMMA_UV_SIZE] __attribute__((aligned(16)));
    u16 rgb10to12[RGB10TO12_SIZE] __attribute__((aligned(16)));
    u16 rgb12to10[RGB12TO10_SIZE] __attribute__((aligned(16)));

}__attribute__((aligned(16))) MLoadLayout;

typedef struct {
    IspMloadID_e  uID;
    u32 u4SrcPhyAddr;
    u32 u4LengthIn2Bytes;
    u32 u4DstAddrOffset;
} ISP_MLOAD;

typedef enum
{
    ISP_PIPE_0 = 0,
    ISP_PIPE_1 = 1,
    ISP_PIPE_2 = 2,
}ISP_PIPE_e;

typedef enum
{
    ISP_DMA_PIPE0=0,
    ISP_DMA_PIPE1=1,
    ISP_DMA_PIPE2=2,
    ISP_DMA_GENERAL=3,
}IspDmaId_e;

typedef enum
{
    ISP_DMA_POSITION_HEAD = 0,
    ISP_DMA_POSITION_TAIL = 1,
}ISP_DMA_POSITION_e;

#ifndef member_size
#define member_size(type, member) sizeof(((type *)0)->member)
#endif
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

typedef struct {
    //DMA_MEM_INFO base_dmem;
    u32         length;
    void*       pVirAddr;
    u32         u32physAddr;
    u8          uBufferIdx;//current buffer
    u32         u4Base[2];//dram double buffer
    MS_BOOL        bDirty[SCL_MLOAD_ID_NUM];
    //u32         tblSize[eMLOAD_ID_NUM]; // 16bytes alignment size
    u32         tblOffset[SCL_MLOAD_ID_NUM];// memory offset in dma memory address
    u32         tblRealSize[SCL_MLOAD_ID_NUM];// real iq table size
    u32         tblSramWidth[SCL_MLOAD_ID_NUM];// Logical size in ip side
} MLOAD_IQ_MEM;

#define MLOAD_BUF_PHYS(mload_mem_cfg,id) ( mload_mem_cfg->u32physAddr + mload_mem_cfg->u4Base[mload_mem_cfg->uBufferIdx] + mload_mem_cfg->tblOffset[id] )
#define MLOAD_BUF_VIRT(mload_mem_cfg,id) ( ((char*)mload_mem_cfg->pVirAddr) + mload_mem_cfg->u4Base[mload_mem_cfg->uBufferIdx] + mload_mem_cfg->tblOffset[id] )
#define MLOAD_TBL_SIZE_W(mload_mem_cfg,id) (mload_mem_cfg->tblRealSize[id]/2)
#define MLOAD_TBL_SIZE_B(mload_mem_cfg,id) (mload_mem_cfg->tblRealSize[id])
typedef void* ISP_HAL_HANDLE;
//////////   ISP HAL APIs    ///////////
int HalIsp_Init(ISP_HAL_HANDLE *p_hnd);
int HalIsp_InitReg(ISP_HAL_HANDLE hnd);
int HalIsp_InitReg(ISP_HAL_HANDLE hnd);
int HalIsp_Deinit(ISP_HAL_HANDLE hnd);
int HalISP_InitTestMode(ISP_HAL_HANDLE hnd,u32 nWidth,u32 nHeight);

//int isp_probe(void *handle, volatile void *isp_mem);
//int isp_release(ISP_HAL_HANDLE hnd);
//int isp_suspend(ISP_HAL_HANDLE hnd);
//int isp_resume(ISP_HAL_HANDLE hnd);

//ISP APIs
int ISP_AE_SetWin(ISP_HAL_HANDLE hnd, ISP_AE_WIN win);
int ISP_AE_SetIntBlockRowNum(ISP_HAL_HANDLE hnd, volatile u32 u4Num);
int ISP_AWB_SetWin(ISP_HAL_HANDLE hnd, ISP_CROP_WIN imagesize, ISP_AWB_WIN win);
int ISP_AWB_SourceSelection(ISP_HAL_HANDLE hnd, volatile ISP_AWB_SOURCE_SELECTION eSource);

////////////////////////////////////////////////////
//////////// MIU ///////////////////////////////////
////////////////////////////////////////////////////
int ISP_MloadDone(ISP_HAL_HANDLE hnd);
int ISP_WaitMloadDone(ISP_HAL_HANDLE hnd, long us_count);
int ISP_MLoadTableSet(ISP_HAL_HANDLE hnd, IspMloadID_e idx, const u16 *table, MLOAD_IQ_MEM *mload_mem_cfg);
int ISP_MLoadTableGet(ISP_HAL_HANDLE hnd, IspMloadID_e idx, u16 *table, MLOAD_IQ_MEM *mload_mem_cfg);
int ISP_MLoadApply(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg);
int SCL_MLoadApply(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg);
int ISP_MLoadRead(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg,int id,int sram_offset,ISP_MLOAD_OUTPUT *output);
int SCL_MLoadRead(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg,int id,int sram_offset,ISP_MLOAD_OUTPUT *output);

////////////////// RDMA config //////////////////////////////
int ISP_RDMAEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id,MS_BOOL enable);
int ISP_RDMAisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RDMAAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
//int ISP_RDMAisAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RDMASwTrig(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RDMASetHighPri(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_RDMAReqThd(ISP_HAL_HANDLE hnd,IspDmaId_e id, int thd);
int ISP_RDMAReqHighPriThd(ISP_HAL_HANDLE hnd,IspDmaId_e id, int thd);
int ISP_RDMAReqBustNum(ISP_HAL_HANDLE hnd,IspDmaId_e id, int num);
int ISP_RDMAReqBlank(ISP_HAL_HANDLE hnd,IspDmaId_e id, int blank);
int ISP_RDMABaseAddr(ISP_HAL_HANDLE hnd,IspDmaId_e id, unsigned int addr);
MS_BOOL ISP_RDMAisBusy(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RDMAReset(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RDMADataPrecision(ISP_HAL_HANDLE hnd, IspDmaId_e id, ISP_DATAPRECISION prec);
int ISP_RDMASetPitch(ISP_HAL_HANDLE hnd, IspDmaId_e id, int width, int height, DATA_PITCH pitch);

///////////////// WDMA config ///////////////////////
int ISP_WDMAEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_WDMAisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMAAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_WDMAisAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMASwTrig(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMASetHighPri(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_WDMAReqThd(ISP_HAL_HANDLE hnd,IspDmaId_e id, int thd);
int ISP_WDMAReqHighPriThd(ISP_HAL_HANDLE hnd,IspDmaId_e id, int thd);
int ISP_WDMAReqBustNum(ISP_HAL_HANDLE hnd,IspDmaId_e id, int num);
int ISP_WDMAAutoAlign(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_WDMACheckLBFull(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL *full);
int ISP_WDMAStatusClear(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMABaseAddr(ISP_HAL_HANDLE hnd,IspDmaId_e id, unsigned int addr);
MS_BOOL ISP_WDMAisBusy(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMADataPrecision(ISP_HAL_HANDLE hnd,IspDmaId_e id, ISP_DATAPRECISION prec);

///////////////////// DMA utiltiy function ///////////////////
int ISP_RingBuffEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_RingBuffisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_RingBuffSize(ISP_HAL_HANDLE hnd,IspDmaId_e id, int size);
int ISP_DMAClkForceActive(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_DMAClkForceOff(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_DMAClkWDMAMode(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
// should be called after setup isp/dma crop width, and mux source
//int ISP_RDMASetPitch(ISP_HAL_HANDLE hnd,IspDmaId_e id, DATA_PITCH pitch);
int ISP_WDMASetPitch(ISP_HAL_HANDLE hnd, IspDmaId_e id, int width, int height, DATA_PITCH pitch);
int ISP_WDMAReset(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WDMASetLineCnt(ISP_HAL_HANDLE hnd,IspDmaId_e id, unsigned int iline_cnt);

#if 0
////////////// cfai parameters, isp_cfai_params//////////////////////
int ISP_CFAIEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_CFAIParams(ISP_HAL_HANDLE hnd, ISP_CFAI_PARAM params);
int ISP_CFAI_NR_Enable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
#endif

/////////// color matrix config, isp_ccm_cfg ////////////////
int ISP_IQ_CCM_Cfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG cfg);
int ISP_SetCCMCfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG cfg);
int ISP_GetCCMCfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG *cfg);
/////////// gamma config, isp_gamma_cfg /////////////////////
int ISP_GammaCfg(ISP_HAL_HANDLE hnd, ISP_GAMMA_CFG cfg);
/////////// edge config, isp_edge_cfg ///////////////////////
int ISP_EdgeEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);

/////////////// crop config, isp_crop_cfg ////////////////////////
// data type -> mux -> crop -> pitch
int ISP_SetISPCrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN win);
int ISP_GetISPCrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN *win);
int ISP_SetWDMACrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN win);
int ISP_GetWDMACrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN *win);

///////////////// sensor IF setting, isp_sensorif_cfg ////////////
int ISP_ISPMasterEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_SenClkPol(ISP_HAL_HANDLE hnd, CLK_POL pol);
int ISP_SenPCLKDelay(ISP_HAL_HANDLE hnd, int delay);
//int ISP_SenHsyncPol(ISP_HAL_HANDLE hnd, CLK_POL pol);
//int ISP_SenVsyncPol(ISP_HAL_HANDLE hnd, CLK_POL pol);
//int ISP_GetSenHsyncPol(ISP_HAL_HANDLE hnd, CLK_POL* pol);
//int ISP_GetSenVsyncPol(ISP_HAL_HANDLE hnd, CLK_POL* pol);
int ISP_SenDataType(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_DATATYPE type);
int ISP_GetSenDataType(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_DATATYPE *type);
int ISP_SenDataPrecision(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_DATAPRECISION fmt);
int ISP_GetSenDataPrecision(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_DATAPRECISION *fmt);
int ISP_SenLeftShift(ISP_HAL_HANDLE hnd, SEN_LEFT_SHIFT left_shift);
int ISP_SenFmtConv(ISP_HAL_HANDLE hnd, SEN_FMT_CONV_MODE method);
int ISP_SenBayerFmt(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_BAYER bayer);
int ISP_SenRGBIRFmt(ISP_HAL_HANDLE hnd, SEN_RGBIR RGBIR_ID);
int ISP_SenYCOrder(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe,  SEN_YC_ORDER order);
//int ISP_SenVsyncDly(ISP_HAL_HANDLE hnd, SEN_VSYNC_DLY delay);
//int ISP_SenHsyncDly(ISP_HAL_HANDLE hnd, SEN_HSYNC_DLY delay);

///////////////// sensor interface ////////////////////
int ISP_RMUXSrc(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_RMUX src); // ISP IF (isp_dp) source, reg_isp_if_rmux_sel
int ISP_WMUXSrc(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_WMUX src);
int ISP_SIFReset(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_RVFSrc(ISP_HAL_HANDLE hnd, SIF_RVF rvf);// Sensor IF source, reg_isp_if_src_sel
int ISP_SensorBus(ISP_HAL_HANDLE hnd, SIF_BUS type);// mipi/parallel
int ISP_GetSensorBus(ISP_HAL_HANDLE hnd, SIF_BUS *type);// mipi/parallel

int ISP_HsyncPipeDelayYCmode(ISP_HAL_HANDLE hnd, int value);
int ISP_HsyncPipeDelayYCmodeMask(ISP_HAL_HANDLE hnd, int value);
int ISP_ICPASync(ISP_HAL_HANDLE hnd, MS_BOOL enable);

//////////////////// IQ block function /////////////////////
// global, IQ blocks, isp_block_cfg
int ISP_IQ_Init_Cfg(ISP_HAL_HANDLE hnd);
#if 0
int ISP_IQ_FPN_Cfg(ISP_HAL_HANDLE hnd, IQ_FPN_CFG cfg);
int ISP_IQ_SDC_Cfg(ISP_HAL_HANDLE hnd, IQ_SDC_CFG cfg);
int ISP_IQ_DPC_Cfg(ISP_HAL_HANDLE hnd, IQ_DPC_CFG cfg);
int ISP_IQ_GE_Cfg(ISP_HAL_HANDLE hnd, IQ_GE_CFG cfg);
int ISP_IQ_ANTICT_Cfg(ISP_HAL_HANDLE hnd, IQ_ANTICT_CFG cfg);
int ISP_IQ_RGBIR_Cfg(ISP_HAL_HANDLE hnd, IQ_RGBIR_CFG cfg);
int ISP_IQ_OBC_Cfg(ISP_HAL_HANDLE hnd, IQ_OBC_CFG cfg);

int ISP_IQ_GAMA_C2A_ISP_Cfg(ISP_HAL_HANDLE hnd, IQ_GAMA_C2A_ISP_CFG cfg);

int ISP_IQ_LSC_Cfg(ISP_HAL_HANDLE hnd, IQ_LSC_CFG cfg);
int ISP_IQ_ALSC_Cfg(ISP_HAL_HANDLE hnd, IQ_ALSC_CFG cfg);

int ISP_IQ_BDNR_Cfg(ISP_HAL_HANDLE hnd, IQ_BDNR_CFG cfg);
int ISP_IQ_SpikeNR_Cfg(ISP_HAL_HANDLE hnd, IQ_SpikeNR_CFG cfg);
int ISP_IQ_BSNR_Cfg(ISP_HAL_HANDLE hnd, IQ_BSNR_CFG cfg);
int ISP_IQ_NM_Cfg(ISP_HAL_HANDLE hnd, IQ_NM_CFG cfg);
int ISP_IQ_DM_Cfg(ISP_HAL_HANDLE hnd, IQ_DM_CFG cfg);
int ISP_IQ_PostDN_Cfg(ISP_HAL_HANDLE hnd, IQ_PostDN_CFG cfg);

int ISP_IQ_FalseColor_Cfg(ISP_HAL_HANDLE hnd, IQ_FalseColor_CFG cfg);
int ISP_IQ_GAMA_A2A_ISP_Cfg(ISP_HAL_HANDLE hnd, IQ_GAMA_A2A_ISP_CFG cfg);


int ISP_IQ_CsTk_Cfg(ISP_HAL_HANDLE hnd, IQ_CSTK_CFG cstk);
int ISP_IQ_DefPix_Cfg(ISP_HAL_HANDLE hnd, IQ_DPC_CFG dpc);

int ISP_IQ_WBG_Cfg(ISP_HAL_HANDLE hnd, IQ_WBG_CFG wbg);
int ISP_IQ_Denoise_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise);
int ISP_IQ_DN_Spike_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise);
int ISP_IQ_DN_BSNR_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise);
int ISP_IQ_DN_NM_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise);
int ISP_IQ_DN_DNR_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise);
int ISP_IQ_CFAI_Cfg(ISP_HAL_HANDLE hnd, IQ_CFAI_CFG CFAi);
int ISP_IQ_CFAI_NR_Cfg(ISP_HAL_HANDLE hnd, IQ_CFAI_NR_CFG CFAi_NR);
#endif
int ISP_MotionCfg(ISP_HAL_HANDLE hnd, ISP_MOTION_CFG cfg);

//int ISP_TransposeCfg(ISP_HAL_HANDLE hnd, MS_BOOL enable);

int ISP_IQEnable(ISP_HAL_HANDLE hnd, ISP_IQ_BLK blk, u8 enable);
//int ISP_GetIQEnable(ISP_HAL_HANDLE hnd, ISP_IQ_BLK blk);

int ISP_ClearAEAWBStats(ISP_HAL_HANDLE hnd);

int ISP_EnableISP(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);
int ISP_BypassISP(ISP_HAL_HANDLE hnd, MS_BOOL enable);

int ISP_EnableSensor(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_SWReset(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);

int ISP_SensorSWReset(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);
int ISP_SensorPowerDown(ISP_HAL_HANDLE hnd, MS_BOOL enable);

int ISP_SensorEnableMCLK(ISP_HAL_HANDLE hnd, MS_BOOL enable);
MS_BOOL ISP_isJPEGDone(ISP_HAL_HANDLE hnd);
int ISP_EnableDblBuf(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);
int ISP_TriggerDblBuf(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe);

int ISP_3DSensorReset(ISP_HAL_HANDLE hnd, int idx, MS_BOOL reset);
int ISP_3DSensorPowerOff(ISP_HAL_HANDLE hnd, int idx, MS_BOOL powerdn);
int ISP_InputEnable(ISP_HAL_HANDLE hnd, MS_BOOL en);
//////////////////// VS block function /////////////////////
#if 0
// video stabilizer
int ISP_EIS_Enable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_EIS_GetStatus(ISP_HAL_HANDLE hnd, MS_BOOL* benable);
int ISP_EIS_Config(ISP_HAL_HANDLE hnd, EIS_CFG strCFG);
#endif
///////////// AE function ///////////////////////////
int ISP_AE_SetIntBlockRowNum(ISP_HAL_HANDLE hnd, volatile u32 u4Num);
int ISP_AE_SetSubWinCfg(ISP_HAL_HANDLE hnd, ISP_AE_SUBWIN_IDX idx, ISP_AE_SUBWIN win);
int ISP_AE_SetDgain(ISP_HAL_HANDLE hnd, int enable, int dgain);
///////////// AWB function //////////////////////////
int ISP_AWB_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_AEAWB_StatsMode(ISP_HAL_HANDLE hnd, ISP_AEAWB_STATS_MODE mode);
int ISP_AWB_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr);
///////////// AE function //////////////////////////
int ISP_AE_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_AE_SetWin(ISP_HAL_HANDLE hnd, ISP_AE_WIN win);
int ISP_AE_GetWin(ISP_HAL_HANDLE hnd, ISP_AEAWB_WIN *win);
///////////// AF function ///////////////////////////
int ISP_AF_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_AF_SetStatsMode(ISP_HAL_HANDLE hnd, ISP_AF_STATS_MODE mode);
int ISP_AF_GetNumWins(ISP_HAL_HANDLE hnd, volatile int *num);
int ISP_AF_SetWins(ISP_HAL_HANDLE hnd, ISP_AF_WIN win, int idx);
int ISP_AF_GetWins(ISP_HAL_HANDLE hnd, volatile ISP_AF_WIN *win, int idx);
int ISP_AF_GetStats(ISP_HAL_HANDLE hnd, volatile ISP_AF_STATS *stats, int idx);
////////////// motion function ////////////////////
int ISP_MOT_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_MOT_SetAttr(ISP_HAL_HANDLE hnd, ISP_MOT_ATTR attr);
///////////// histogram function //////////////////
int ISP_HISTO_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable, int idx);
int ISP_HISTO_SetWins(ISP_HAL_HANDLE hnd, const ISP_HISTO_WIN *win, int idx);
int ISP_HISTO_GetWins(ISP_HAL_HANDLE hnd, volatile ISP_HISTO_WIN *win, int idx);
int ISP_HISTO_SetAttr(ISP_HAL_HANDLE hnd, const ISP_HISTO_ATTR *attr);
///////////// rgbir function //////////////////
int ISP_RGBIR_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_RGBIR_SetWins(ISP_HAL_HANDLE hnd, ISP_RGBIR_WIN *win);
int ISP_RGBIR_GetWins(ISP_HAL_HANDLE hnd, ISP_RGBIR_WIN *win);
int ISP_RGBIR_SetAttr(ISP_HAL_HANDLE hnd, ISP_RGBIR_ATTR *attr);
///////////// hdr function ///////////////////
int ISP_HDR_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_HDR_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr);
////////////// gated clock ////////////////////////
// gate clk config, isp_gatelevel_cfg
int ISP_ISPDP_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_Raw_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_Kernel_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_LatchMode(ISP_HAL_HANDLE hnd, MS_BOOL enable);
/////////////// path clk control //////////////////
int ISP_DPRstEveryVsync(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_DisableRawDNClk(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_DisableISPDPClk(ISP_HAL_HANDLE hnd, MS_BOOL enable);
int ISP_EnableRawDNWDMAClk(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);
int ISP_SetAWBDSPMode(ISP_HAL_HANDLE hnd, ISP_AWBDSP_Mode mode); //G2E
////////////// debug //////////////
//int ISP_SetDebugLevel(ISP_HAL_HANDLE hnd, ISP_DBG_LEVEL eDbg);
//int ISP_TESTPIN(ISP_HAL_HANDLE hnd);
///////////////////////////////////
int ISP_GetSenLineCnt(ISP_HAL_HANDLE hnd, int *cnt);
int ISP_isISPBusy(ISP_HAL_HANDLE hnd);
int ISP_isFIFOFull(ISP_HAL_HANDLE hnd);
int ISP_SetIspOutputLineCountInt(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, volatile u32 Line);
int ISP_SetLineCountInt0(ISP_HAL_HANDLE hnd, u32 Line);
int ISP_SetLineCountInt1(ISP_HAL_HANDLE hnd, u32 Line);
int ISP_SetLineCountInt2(ISP_HAL_HANDLE hnd, u32 Line);
void ISP_SelDbgOut(ISP_HAL_HANDLE hnd, ISP_DBG_SEL mode);
u32 ISP_GetDgbOut(ISP_HAL_HANDLE hnd);

int ISP_InitReg(ISP_HAL_HANDLE hnd);
int ISP_Suspend(ISP_HAL_HANDLE hnd);
int ISP_Resume(ISP_HAL_HANDLE hnd);
int ISP_MIPI_Initial(ISP_HAL_HANDLE hnd);
int ISP_InitHandle(ISP_HAL_HANDLE hnd);

/* AWB Gain */
int ISP_IQ_WBG_Cfg(ISP_HAL_HANDLE hnd, IQ_WBG_CFG *wbg);

//WDMA function
int ISP_WdmaEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable);
int ISP_WdmaTrigger(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_WdmaReset(ISP_HAL_HANDLE hnd,IspDmaId_e id);
int ISP_SetWdmaAttr(ISP_HAL_HANDLE hnd, ISP_WDMA_ATTR wdma_attr);

int ISP_RAW_Store_Mode(ISP_HAL_HANDLE hnd, IspDmaId_e isp_dma_id, ISP_DMA_POSITION_e isp_dma_pos, u32 ulmode);
int ISP_RAW_Fetch_Mode(ISP_HAL_HANDLE hnd, IspDmaId_e isp_dma_id, ISP_DMA_POSITION_e isp_dma_pos, u32 ulmode);
int ISP_4Pixel_Data_Pak_AtMsb_To_Wdma(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable);

int ISP_EnableISP_HDR(ISP_HAL_HANDLE hnd, MS_BOOL enable);
#endif

#endif
