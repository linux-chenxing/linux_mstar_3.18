#ifndef ISP_INTERRUPT_HANDLER_H
#define ISP_INTERRUPT_HANDLER_H

#include <hal_isp_private.h>

#define ISP_SHIFTBITS(a)      (0x01<<(a))
#define ISP_CHECKBITS(a, b)   ((a) & ((u32)0x01 << (b)))
#define ISP_SETBIT(a, b)      (a) |= (((u32)0x01 << (b)))
#define ISP_CLEARBIT(a, b)    (a) &= (~((u32)0x01 << (b)))

/*
interrupt group 1
# [0] :P0 VIF frame start rising edge
# [1]: P0 VIF frame start falling edge
# [2] reserved
# [3] Update double buffer register done
# [4]: AF Statistic done
# [5]: ISP internal FIFO full
# [6]: ISP busy rising edge
# [7]: ISP busy falling edge
# [8]: AWB Statistic done
# [9]: WDMA done
# [10]: RDMA done
#[11]: WDMA FIFO full
#[12]: Pip1 FIFO Full
#[13]: Pip2 FIFO Full
#[14]: isp_if vsync pulse
#[15]: AE Statistic done"
*/
typedef enum
{
    INT_P0_VIF_FS_RISING    = 0,
    INT_P0_VIF_FS_FALLING   = 1,
    INT_RESERVED_0          = 2,
    INT_DB_UPDATE_DONE      = 3,
    INT_AF_DONE             = 4,
    INT_ISP_FIFO_FULL       = 5,
    INT_ISP_BUSY_RISING_EDGE = 6,
    INT_ISP_BUSY_FALLING_EDGE            = 7,
    INT_AWB_DONE            = 8,
    INT_WDMA_DONE           = 9,
    INT_RDMA_DONE           = 10,
    INT_WDMA_FIFO_FULL      = 11,
    INT_PIPE1_FIFO_FULL     = 12,
    INT_PIPE2_FIFO_FULL     = 13,
    INT_ISPIF_VSYNC         = 14,
    INT_AE_DONE             = 15,
    ISP_INT_MAX,
} ISP_INT_STATUS;

/*
interrupt group 2
#[0] wdma0 line count hit, vdos hit line conut
#[1] wdma1 line count hit, vdos hit line conut
#[2] wdma2 line count hit, vdos hit line conut
#[3] Reserved
#[4] Reserved
#[5] Reserved
#[6] wdma6 line count hit
#[7] rdma0 done
#[8] rdma1 done
#[9] Reserved
#[10] Reserved
#[11]Reserved
#[12] Reserved
#[13] wdma0 done
#[14] wdma1 done
#[15] wdma2 done"
*/
typedef enum
{
    INT2_WDMA0_HIT_LINE_COUNT   = 0,
    INT2_WDMA1_HIT_LINE_COUNT   = 1,
    INT2_WDMA2_HIT_LINE_COUNT   = 2,
    INT2_RESERVED0              = 3,
    INT2_RESERVED1              = 4,
    INT2_RESERVED2              = 5,
    INT2_WDMA_HIT_LINE_COUNT    = 6,
    INT2_RDMA0_DONE             = 7,
    INT2_RDMA1_DONE             = 8,
    INT2_RESERVED3              = 9,
    INT2_RESERVED4              = 10,
    INT2_RESERVED5              = 11,
    INT2_RESERVED6              = 12,
    INT2_WDMA0_DONE             = 13,
    INT2_WDMA1_DONE             = 14,
    INT2_WDMA2_DONE             = 15,
    ISP_INT2_MAX,
} ISP_INT2_STATUS;

/*
interrupt group 3
#[0]: Histogram window0 statistic done
#[1]: Histogram window1 statistic done
#[2]: AE block row count requal req_ae_int_row_num
#[3]: menuload done
#[4]: P0 input line count equal reg_sw_specify_int_line
#[5]: P0 output count equal reg_sw_specify_int_line
#[6]: hit input line count1
#[7]: hit input line count2
#[8]: hit input line count3
#[9]: P1 input line count equal reg_sw_specify_int_line
#[10]: rgbir histogram done
#[11]: awb row done
#[12]: histogram row done
#[13]: histogram by pixel done
#[14]: P2 input line count equal reg_sw_specify_int_line
#[15]: Reserved"
 */
typedef enum
{
    INT3_AE_WIN0_DONE         = 0,
    INT3_AE_WIN1_DONE         = 1,
    INT3_AE_BLK_ROW_INT_DONE  = 2,
    INT3_MENULOAD_DONE        = 3,
    INT3_SW_INT_INPUT_DONE    = 4,
    INT3_SW_INT_OUTPUT_DONE   = 5,
    INT3_HIT_LINE_COUNT1      = 6,
    INT3_HIT_LINE_COUNT2      = 7,
    INT3_HIT_LINE_COUNT3      = 8,
    INT3_PIPE1_INPUT_HIT_LINE_COUNT = 9, //pipe1 input line count equal reg_sw_specify_int_line
    INT3_RGBIR_HISTO_DONE     = 10,
    INT3_AWB_ROW_DONE         = 11,
    INT3_HISTO_ROW_DONE       = 12,
    INT3_HISTO_BY_PIXEL_DONE = 13,
    INT3_PIPE2_INPUT_HIT_LINE_COUNT = 14, //pipe2 input line count equal reg_sw_specify_int_line
    INT3_RESERVED2            = 15,
    ISP_INT3_MAX,
} ISP_INT3_STATUS;

/*
interrupt group 4
#[0]: Reserved
#[1]: Reserved
#[2]: Reserved
#[3]: P1 VIF frame start rising edge
#[4]: P1 VIF frame start falling edge
#[5]: P2 VIF frame start rising edge
#[6]: P2 VIF frame start falling edge
#[7]: hdr array hit count
#[8]: hdr mapping hit count
#[9]: hdr line buffer countrol done
#[10]: hdr frame done
#[11]: CI frame done
#[12]: hdr histogram done
#[13]:  P1 hit input line count1
#[14]:  P1 hit input line count2
#[15]:  P1 hit input line count3"
 */
typedef enum
{
    INT4_RESERVED0              = 0,
    INT4_RESERVED1              = 1,
    INT4_RESERVED2              = 2,
    INT4_PIPE1_VIF_FS_RISING    = 3,
    INT4_PIPE1_VIF_FS_FALLING   = 4,
    INT4_PIPE2_VIF_FS_RISING    = 5,
    INT4_PIPE2_VIF_FS_FALLING   = 6,
    INT4_HDR_ARRAY_HIT_COUNT    = 7,
    INT4_HDR_MAPPING_HIT_COUNT  = 8,
    INT4_HDR_LINE_BUF_CTRL_DONE = 9,
    INT4_HDR_FRAME_DONE         = 10,
    INT4_CI_FRAME_DONE          = 11,
    INT4_HDR_HISTO_DONE         = 12,
    INT4_PIPE1_HIT_INPUT_LINE_COUNT1 = 13,
    INT4_PIPE1_HIT_INPUT_LINE_COUNT2 = 14,
    INT4_PIPE1_HIT_INPUT_LINE_COUNT3 = 15,
    ISP_INT4_MAX,
} ISP_INT4_STATUS;

void HalIspDisableInt(ISP_HAL_HANDLE *hnd);
void HalIspEnableInt(ISP_HAL_HANDLE *hnd);
void HalIspMaskIntAll(ISP_HAL_HANDLE *hnd);
u32 HalIspGetIntStatus1(ISP_HAL_HANDLE *hnd);
u32 HalIspGetIntStatus2(ISP_HAL_HANDLE *hnd);
u32 HalIspGetIntStatus3(ISP_HAL_HANDLE *hnd);
u32 HalIspGetIntStatus4(ISP_HAL_HANDLE *hnd);
void HalIspMaskClear1(ISP_HAL_HANDLE *hnd, u32 clear);
void HalIspMaskClear2(ISP_HAL_HANDLE *hnd, u32 clear);
void HalIspMaskClear3(ISP_HAL_HANDLE *hnd, u32 clear);
void HalIspMaskClear4(ISP_HAL_HANDLE *hnd, u32 clear);
void HalIspMaskInt1(ISP_HAL_HANDLE *hnd, u32 mask);
void HalIspMaskInt2(ISP_HAL_HANDLE *hnd, u32 mask);
void HalIspMaskInt3(ISP_HAL_HANDLE *hnd, u32 mask);
void HalIspMaskInt4(ISP_HAL_HANDLE *hnd, u32 mask);
void HalIspClearInt(ISP_HAL_HANDLE *hnd, volatile u32 *clear, ISP_INT_STATUS int_num);
void HalIspClearIntAll(ISP_HAL_HANDLE *hnd);
void HalIspReset(ISP_HAL_HANDLE *hnd);
void HalIspInputEnable(ISP_HAL_HANDLE *hnd, u32 enable);
void HalIspAsyncEnable(ISP_HAL_HANDLE *hnd, u32 enable);
void HalIspSetAEDgain(ISP_HAL_HANDLE *hnd, u32 enable, u32 gain);
void HalIspSetYUVCCM(ISP_HAL_HANDLE *hnd, const s16 *ccm_coeff);
u8 HalISPGetFrameDoneCount(ISP_HAL_HANDLE *hnd);
void HalIspIrqInit(ISP_HAL_HANDLE *hnd);
#endif
