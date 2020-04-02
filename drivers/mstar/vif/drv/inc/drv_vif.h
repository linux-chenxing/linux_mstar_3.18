#ifndef __DRV_VIF__
#define __DRV_VIF__
#include "mhal_vif_datatype.h"
#include <linux/interrupt.h>
typedef enum {
    VIF_CHANNEL_0,
    VIF_CHANNEL_1,
    VIF_CHANNEL_2,
    VIF_CHANNEL_3,
    VIF_CHANNEL_4,
    VIF_CHANNEL_5,
    VIF_CHANNEL_6,
    VIF_CHANNEL_7,
    VIF_CHANNEL_8,
    VIF_CHANNEL_9,
    VIF_CHANNEL_10,
    VIF_CHANNEL_11,
    VIF_CHANNEL_12,
    VIF_CHANNEL_13,
    VIF_CHANNEL_14,
    VIF_CHANNEL_15,
    VIF_CHANNEL_NUM,
} VIF_CHANNEL_e;

typedef enum {
    VIF_DISABLE,
    VIF_ENABLE,
} VIF_ONOFF_e;

typedef enum {
	VIF_CH_SRC_MIPI_0 = 0, 
	VIF_CH_SRC_MIPI_1 = 1, 
	VIF_CH_SRC_MIPI_2 = 2,
	VIF_CH_SRC_MIPI_3 = 3,
	VIF_CH_SRC_PARALLEL_SENSOR_0 = 4,
	VIF_CH_SRC_PARALLEL_SENSOR_1 = 5,
	VIF_CH_SRC_PARALLEL_SENSOR_2 = 6,
	VIF_CH_SRC_PARALLEL_SENSOR_3 = 7,
	VIF_CH_SRC_BT656 = 8,
	VIF_CH_SRC_MAX = 9,
} VIF_CHANNEL_SOURCE_e;

typedef enum {
	VIF_HDR_SRC_MIPI0,
	VIF_HDR_SRC_MIPI1,
	VIF_HDR_SRC_MIPI2,
	VIF_HDR_SRC_MIPI3,
	VIF_HDR_SRC_HISPI0,
	VIF_HDR_SRC_HISPI1,
	VIF_HDR_SRC_HISPI2,
	VIF_HDR_SRC_HISPI3,
	VIF_HDR_VC0 = 0,
	VIF_HDR_VC1 = 1,
	VIF_HDR_VC2 = 2,
	VIF_HDR_VC3 = 3,
} VIF_HDR_SOURCE_e;

typedef enum {
	VIF_STROBE_POLARITY_HIGH_ACTIVE,
	VIF_STROBE_POLARITY_LOW_ACTIVE,
} VIF_STROBE_POLARITY_e;

typedef enum {
	VIF_STROBE_VERTICAL_ACTIVE_START,
	VIF_STROBE_VERTICAL_BLANKING_START,
} VIF_STROBE_VERTICAL_START_e;

typedef enum {
	VIF_STROBE_SHORT,
	VIF_STROBE_LONG,
} VIF_STROBE_MODE_e;

typedef enum {
	VIF_SENSOR_POLARITY_HIGH_ACTIVE = 0,
	VIF_SENSOR_POLARITY_LOW_ACTIVE  = 1,
} VIF_SENSOR_POLARITY_e;

typedef enum {
	VIF_SENSOR_FORMAT_8BIT = 0,
	VIF_SENSOR_FORMAT_10BIT = 1,
	VIF_SENSOR_FORMAT_16BIT = 2,
	VIF_SENSOR_FORMAT_12BIT = 3,
} VIF_SENSOR_FORMAT_e;

typedef enum {
	VIF_SENSOR_INPUT_FORMAT_YUV422,
	VIF_SENSOR_INPUT_FORMAT_RGB,
} VIF_SENSOR_INPUT_FORMAT_e;

typedef enum {
	VIF_SENSOR_BIT_MODE_0,
	VIF_SENSOR_BIT_MODE_1,
	VIF_SENSOR_BIT_MODE_2,
	VIF_SENSOR_BIT_MODE_3,
} VIF_SENSOR_BIT_MODE_e;

typedef enum {
	VIF_SENSOR_YC_SEPARATE,
	VIF_SENSOR_YC_16BIT,
} VIF_SENSOR_YC_INPUT_FORMAT_e;

typedef enum {
	VIF_SENSOR_VS_FALLING_EDGE,
	VIF_SENSOR_VS_FALLING_EDGE_DELAY_2_LINE,
	VIF_SENSOR_VS_FALLING_EDGE_DELAY_1_LINE,
	VIF_SENSOR_VS_RISING_EDGE,
} VIF_SENSOR_VS_DELAY_e;

typedef enum {
	VIF_SENSOR_HS_RISING_EDGE,
	VIF_SENSOR_HS_FALLING_EDGE,
} VIF_SENSOR_HS_DELAY_e;

typedef enum {
	VIF_SENSOR_BT656_YC,
	VIF_SENSOR_BT656_CY,
} VIF_SENSOR_BT656_FORMAT_e;

typedef enum {
	VIF_INTERRUPT_VREG_RISING_EDGE,
	VIF_INTERRUPT_VREG_FALLING_EDGE,
	VIF_INTERRUPT_HW_FLASH_STROBE_DONE,
	VIF_INTERRUPT_PAD2VIF_VSYNC_RISING_EDGE,
	VIF_INTERRUPT_PAD2VIF_VSYNC_FALLING_EDGE,
	VIF_INTERRUPT_VIF_TO_ISP_LINE_COUNT_HIT0,
	VIF_INTERRUPT_VIF_TO_ISP_LINE_COUNT_HIT1,
} VIF_INTERRUPT_e;

typedef enum {
    VIF_RGRG_GBGB,
	VIF_GRGR_BGBG,
	VIF_BGBG_GRGR,
	VIF_GBGB_RGRG,
} VIF_BAYER_MODE_e;

typedef enum {
    VIF_CYCY,
	VIF_YCYC,
} VIF_INPUT_PRIORITY_YUV_e;

typedef enum {
	VIF_SUCCESS,
	VIF_FAIL,
	VIF_ISR_CREATE_FAIL,
} VIF_STATUS_e;

typedef enum
{
    VIF_CH0_FRAME_START_INTS = 0x01,
    VIF_CH0_FRAME_END_INTS = 0x02,
    VIF_CH1_FRAME_START_INTS = 0x04,
    VIF_CH1_FRAME_END_INTS = 0x08,
    VIF_CH2_FRAME_START_INTS = 0x10,
    VIF_CH2_FRAME_END_INTS = 0x20,
} VIF_INTS_EVENT_TYPE_e;

typedef enum
{
    VIF_CLK_CSI2_MAC_0 = 0,//000: clk_csi2_mac_toblk0_buf0_p
    VIF_CLK_CSI2_MAC_1 = 1,//001: clk_csi2_mac_toblk0_4_buf0_p
    VIF_CLK_BT656_P0_0_P = 2,//010: clk_bt656_p0_0_p
    VIF_CLK_BT656_P0_1_P = 3,//011: clk_bt656_p0_1_p
    VIF_CLK_BT656_P1_0_P = 4,//100: clk_bt656_p1_0_p
    VIF_CLK_BT656_P1_1_P = 5,//101: clk_bt656_p1_1_p*/
    VIF_CLK_PARALLEL = 6,//110: clk_parllel*/
    VIF_CLK_FPGA_BT656 = 7, //111: TBD.
}  VIF_CLK_TYPE_e;

typedef enum
{
    VIF_BT656_EAV_DETECT = 0,
    VIF_BT656_SAV_DETECT = 1,
} VIF_BT656_CHANNEL_SELECT_e;

typedef enum
{
    VIF_BT656_VSYNC_DELAY_1LINE = 0,
    VIF_BT656_VSYNC_DELAY_2LINE = 1,
    VIF_BT656_VSYNC_DELAY_0LINE = 2,
    VIF_BT656_VSYNC_DELAY_BT656 = 3,
} VIF_BT656_VSYNC_DELAY_e;

typedef enum
{
    SENSOR_PAD_GROUP_A = 0,
    SENSOR_PAD_GROUP_B = 1,
} SENSOR_PAD_GROUP_e;

typedef struct VifCfg_s
{
    VIF_CHANNEL_SOURCE_e eSrc;
    VIF_HDR_SOURCE_e eHdrSrc;
    VIF_SENSOR_POLARITY_e HsyncPol;
    VIF_SENSOR_POLARITY_e VsyncPol;
    VIF_SENSOR_POLARITY_e ResetPinPol;
	VIF_SENSOR_POLARITY_e PdwnPinPol;
    VIF_SENSOR_FORMAT_e ePixelDepth;
    VIF_BAYER_MODE_e eBayerID;		
	u32 FrameStartLineCount; // lineCount0 for FS ints
	u32 FrameEndLineCount;   // lineCount1 for FE ints
} VifSensorCfg_t;

typedef struct VifBT656Cfg_s
{
    u32 bt656_ch_det_en;
    VIF_BT656_CHANNEL_SELECT_e bt656_ch_det_sel;
    u32 bt656_bit_swap;
    u32 bt656_8bit_mode;
    VIF_BT656_VSYNC_DELAY_e bt656_vsync_delay;
    u32 bt656_hsync_inv;
    u32 bt656_vsync_inv;
    u32 bt656_clamp_en;
} VifBT656Cfg_t;

typedef struct VifPatGenCfg_s
{
    u32 nWidth;
	u32 nHeight;
	u32 nFps;
} VifPatGenCfg_t;

typedef enum
{
    CLK_POL_POS = 0,
    CLK_POL_NEG
} CLK_POL;

#if 0
void DrvVif_WaitFSInts(VIF_CHANNEL_e ch);
void DrvVif_WaitFEInts(VIF_CHANNEL_e ch);
int DrvVif_CreateFSTask(void);
int DrvVif_DeleteFSTask(void);
int DrvVif_CreateFETask(void);
int DrvVif_DeleteFETask(void);
#endif
int DrvVif_PatGenCfg(VIF_CHANNEL_e ch, VifPatGenCfg_t *PatGenCfg);
int DrvVif_SensorFrontEndCfg(VIF_CHANNEL_e ch, VifSensorCfg_t *sensorcfg);
int DrvVif_SensorReset(VIF_CHANNEL_e ch,int reset);
int DrvVif_SensorPdwn(VIF_CHANNEL_e ch, int Pdwn);
int DrvVif_ChannelEnable(VIF_CHANNEL_e ch, bool ben);
int DrvVif_ConfigParallelIF(VIF_CHANNEL_e ch,
                            VIF_SENSOR_INPUT_FORMAT_e svif_sensor_in_format,
                            VIF_SENSOR_FORMAT_e PixDepth,
                            CLK_POL PclkPol,
                            CLK_POL VsyncPol,
                            CLK_POL HsyncPol,
                            u32 nImgWidth);
int DrvVif_ConfigBT656IF(SENSOR_PAD_GROUP_e pad_group, VIF_CHANNEL_e ch, VifBT656Cfg_t *pvif_bt656_cfg);
int DrvVif_SetChannelSource(VIF_CHANNEL_e ch, VIF_CHANNEL_SOURCE_e svif_ch_src);

/*@brief MHAL driver interface and 
   ARM to 8051 ring buffer control,
   queue, dequeue interface
*/

#define VIF_RING_QUEUE_SIZE 8   /**< Ring buffer size of each channel*/
#define VIF_PORT_NUM  32        /**< Number of channel*/

#define VIF_CHN_MAIN 0
#define VIF_CHN_SUB 1

typedef struct
{
    u64 count64;
    u32 prev_count;
}VifnPTs;

/*! @brief ring buffer element*/
typedef struct
{
    u32 nPhyBufAddrY_H;  /**<Highbyte physical buffer Y address 32k alignement*/
    u32 nPhyBufAddrY_L;  /**<Lowbyte physical buffer Y address 32k alignement*/
    u32 nPhyBufAddrC_H;  /**<Highbyte physical buffer C address 32k alignement*/
    u32 nPhyBufAddrC_L;  /**<Lowbyte physical buffer C address 32k alignement*/

    u16 PitchY;        /**< Width +31/32*/
    u16 PitchC;        /**< Width +31/32*/

    u16 nCropX;       /**< crop start x*/
    u16 nCropY;       /**< crop start y*/
    u16 nCropW;       /**< crop start width*/
    u16 nCropH;       /**< crop start height*/

    u16 nHeightY;      /**< Y plane height*/
    u16 nHeightC;      /**< C plane height*/

    u32 size;         /**< Frame size*/
    u32 nPTS;         /**< timestamp in 90KHz*/
    u32 nMiPriv;      /**< MI private*/
    u8 nStatus;       /**< Vif buffer status*/

    /**Extension*/
    u8 tag;
}__attribute__((packed, aligned(4))) VifRingBufElm_mcu;

/*! @brief 32 channels ring buffer*/
typedef struct
{
    u8 nEnable;
    u8 nReadIdx;
    u8 nWriteIdx;
    u8 pre_nReadIdx;
    u32 nFPS_bitMap;
    VifRingBufElm_mcu data[VIF_RING_QUEUE_SIZE];
}__attribute__((packed, aligned(4))) VifRingBufShm_t;

/** @brief ARM to 8051 ring buffer binding.
@param[in] pstRingBuf physical address of ring buffer
@retval 0 if successed or E_HAL_VIF_ERROR if error occurs.
@remark MI call this function to bind 8051 ring buffer
*/

s32 DrvVif_Init(void);
s32 DrvVif_Deinit(void);
s32 DrvVif_Reset(void);

s32 DrvVif_DevSetConfig(MHal_VIF_DEV u32VifDev, MHal_VIF_DevCfg_t *pstDevAttr);
s32 DrvVif_DevEnable(MHal_VIF_DEV u32VifDev);
s32 DrvVif_DevDisable(MHal_VIF_DEV u32VifDev);

s32 DrvVif_ChnSetConfig(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnCfg_t *pstAttr);
s32 DrvVif_ChnEnable(MHal_VIF_CHN u32VifChn);
s32 DrvVif_ChnDisable(MHal_VIF_CHN u32VifChn);
s32 DrvVif_ChnQuery(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnStat_t *pstStat);

s32 DrvVif_SubChnSetConfig(MHal_VIF_CHN u32VifChn, MHal_VIF_SubChnCfg_t *pstAttr);
s32 DrvVif_SubChnEnable(MHal_VIF_CHN u32VifChn);
s32 DrvVif_SubChnDisable(MHal_VIF_CHN u32VifChn);
s32 DrvVif_SubChnQuery(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnStat_t *pstStat);

/** @brief Push new frame buffer into VIF wait queue
@param[in] u32VifChn VIF channel
@retval 0 if successed or E_HAL_VIF_ERROR if error occurs.
*/
s32 DrvVif_QueueFrameBuffer(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, const MHal_VIF_RingBufElm_t *ptFbInfo);

/** @brief Query VIF for ready frame
@param[in] u32VifChn VIF channel
@param[out] pNumBuf number of frame ready
@retval 0 if successed or E_HAL_VIF_ERROR if error occurs.
*/
s32 DrvVif_QueryFrames(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, u32 *pNumBuf);

/** @brief Receive ready frame from VIF quque
@param[in] u32VifChn VIF channel
@param[out] ptFbInfo output frame buffer
@retval 0 if successed or E_HAL_VIF_ERROR if error occurs.
*/
s32 DrvVif_DequeueFrameBuffer(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, MHal_VIF_RingBufElm_t *ptFbInfo);

/** @brief Set framerate
@param[in] u32VifChn VIF channel
@param[in] port number of u32VifChn
@param[in] frame rate value
@param[in] manual fps mask
@retval 0 if successed or E_HAL_VIF_ERROR if error occurs.
*/
s32 DrvVif_setChnFPSBitMask(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, MHal_VIF_FrameRate_e u32Chnfps, u32 *manualMask);

u8 DrvVif_FakeIrq(irqreturn_t func);

#define MCU_WINDOWS0_OFFSET 0x4400UL
//8051 state machine
#define MCU_STATE_STOP    0
#define MCU_STATE_READY   1
#define MCU_STATE_POLLING 2
#define MCU_STATE_NUM MCU_STATE_POLLING+1

#define MCU_REG_TIMEOUT 10


int initialSHMRing(void);
int unInitialSHMRing(void);

/* MCU control function */
int DrvVif_setARMControlStatus(u16 status);
int DrvVif_getMCUStatus(u16 *status);
u32 DrvVif_changeMCUStatus(u16 status);
u32 convertMhalBuffElmtoMCU(volatile VifRingBufElm_mcu *mcu, const MHal_VIF_RingBufElm_t *ptFbInfo);
u32 convertMCUBuffElmtoMhal(volatile VifRingBufElm_mcu *mcu, MHal_VIF_RingBufElm_t *element, u8 portidx);
u32 convertFPSMaskToMCU(u32 mask);

u32 DrvVif_stopMCU(void);
u32 DrvVif_startMCU(void);
u32 DrvVif_pollingMCU(void);
u32 DrvVif_pauseMCU(void);
void DrvVif_dumpInfo(void);
int DrvVif_MCULoadBin(void);
int DrvVif_SetVifChanelBaseAddr(void);
int DrvVif_CLK(void);
#endif
