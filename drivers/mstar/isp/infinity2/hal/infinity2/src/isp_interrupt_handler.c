#include <hal_isp.h>
#include <isp_sys_api.h>
#include "../inc/hal_isp_private.h"
#include "../pub/hal_interrupt_handler.h"

#define _ISP_HAL(h) ((isp_handle_t*)h)

#define IQ_LEN_ALSC_GAIN            (4209)
#define IQ_LEN_GAMMA_12TO10         (256)
#define IQ_LEN_DEFECT_PIXEL         (2048)
#define IQ_LEN_GAMMA_10TO12         (256)
#define IQ_LEN_GAMMA_CORRECT        (256)
#if 0//defined(__MV5_FPGA__)
static volatile u32 u4DefDisable[4] =
{
        0xFFFF,
        0xFFFF,
        0xFFFF,
        0xFFFF
};
#else //#if defined(__MV5_FPGA__)
static volatile u32 u4DefDisable[4] =
{
    //interrupt group 1
    ISP_SHIFTBITS(INT_P0_VIF_FS_RISING)|
    ISP_SHIFTBITS(INT_P0_VIF_FS_FALLING)|
    ISP_SHIFTBITS(INT_DB_UPDATE_DONE)|
    ISP_SHIFTBITS(INT_ISP_BUSY_RISING_EDGE)|
    //ISP_SHIFTBITS(INT_ISP_FIFO_FULL)|
    ISP_SHIFTBITS(INT_ISP_BUSY_FALLING_EDGE)|
    0,

    //interrupt group 2
    ISP_SHIFTBITS(INT2_WDMA0_HIT_LINE_COUNT)|
    ISP_SHIFTBITS(INT2_WDMA1_HIT_LINE_COUNT)|
    ISP_SHIFTBITS(INT2_WDMA2_HIT_LINE_COUNT)|
    ISP_SHIFTBITS(INT2_WDMA_HIT_LINE_COUNT)|
    0,

    //interrupt group 3
    ISP_SHIFTBITS(INT3_AE_WIN1_DONE)|
    ISP_SHIFTBITS(INT3_MENULOAD_DONE)|
    //ISP_SHIFTBITS(INT3_SW_INT_OUTPUT_DONE)|
    ISP_SHIFTBITS(INT3_SW_INT_INPUT_DONE)|
    //ISP_SHIFTBITS(INT3_HIT_LINE_COUNT1)|
    ISP_SHIFTBITS(INT3_HIT_LINE_COUNT2)|
    ISP_SHIFTBITS(INT3_HIT_LINE_COUNT3)|
    ISP_SHIFTBITS(INT3_AWB_ROW_DONE)|
    ISP_SHIFTBITS(INT3_HISTO_ROW_DONE)|
    ISP_SHIFTBITS(INT3_PIPE1_INPUT_HIT_LINE_COUNT)|
    ISP_SHIFTBITS(INT3_PIPE2_INPUT_HIT_LINE_COUNT)|
    0,

    //interrupt group 4
    ISP_SHIFTBITS(INT4_PIPE1_VIF_FS_RISING)|
    ISP_SHIFTBITS(INT4_PIPE1_VIF_FS_FALLING)|
    ISP_SHIFTBITS(INT4_PIPE2_VIF_FS_RISING)|
    ISP_SHIFTBITS(INT4_PIPE2_VIF_FS_FALLING)|
    ISP_SHIFTBITS(INT4_HDR_ARRAY_HIT_COUNT)|
    ISP_SHIFTBITS(INT4_HDR_MAPPING_HIT_COUNT)|
    ISP_SHIFTBITS(INT4_HDR_LINE_BUF_CTRL_DONE)|
    ISP_SHIFTBITS(INT4_HDR_FRAME_DONE)|
    ISP_SHIFTBITS(INT4_CI_FRAME_DONE)|
    ISP_SHIFTBITS(INT4_HDR_HISTO_DONE)|
    ISP_SHIFTBITS(INT4_PIPE1_HIT_INPUT_LINE_COUNT1)|
    ISP_SHIFTBITS(INT4_PIPE1_HIT_INPUT_LINE_COUNT2)|
    ISP_SHIFTBITS(INT4_PIPE1_HIT_INPUT_LINE_COUNT3)|
    0,
};
#endif //#if defined(__MV5_FPGA__)

void HalIspDisableInt(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask2 = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask3 = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask4 = 0xffff;
}

void HalIspEnableInt(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask  = u4DefDisable[0];
    handle->isp0_cfg->reg_c_irq_mask2  = u4DefDisable[1];
    handle->isp0_cfg->reg_c_irq_mask3  = u4DefDisable[2];
    handle->isp0_cfg->reg_c_irq_mask4  = u4DefDisable[3];

    handle->isp0_cfg->reg_c_irq_clr = 0xffff & ~(u4DefDisable[0]);
    handle->isp0_cfg->reg_c_irq_clr = 0;
    handle->isp0_cfg->reg_c_irq_clr2 = 0xffff & ~(u4DefDisable[1]);
    handle->isp0_cfg->reg_c_irq_clr2 = 0;
    handle->isp0_cfg->reg_c_irq_clr3 = 0xffff & ~(u4DefDisable[2]);
    handle->isp0_cfg->reg_c_irq_clr3 = 0;
    handle->isp0_cfg->reg_c_irq_clr4 = 0xffff & ~(u4DefDisable[3]);
    handle->isp0_cfg->reg_c_irq_clr4 = 0;
}

void HalIspMaskIntAll(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask  = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask2  = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask3  = 0xffff;
    handle->isp0_cfg->reg_c_irq_mask4  = 0xffff;

    handle->isp0_cfg->reg_c_irq_clr = 0xffff & ~(u4DefDisable[0]);
    handle->isp0_cfg->reg_c_irq_clr2 = 0xffff & ~(u4DefDisable[1]);
    handle->isp0_cfg->reg_c_irq_clr3 = 0xffff & ~(u4DefDisable[2]);
    handle->isp0_cfg->reg_c_irq_clr3 = 0xffff & ~(u4DefDisable[3]);
}

u32 HalIspGetIntStatus1(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    return (~handle->isp0_cfg->reg_c_irq_mask) & handle->isp0_cfg->reg_irq_final_status;
}

u32 HalIspGetIntStatus2(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    return (~handle->isp0_cfg->reg_c_irq_mask2) & handle->isp0_cfg->reg_irq_final_status2;
}

u32 HalIspGetIntStatus3(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    return (~handle->isp0_cfg->reg_c_irq_mask3) & handle->isp0_cfg->reg_irq_final_status3;
}

u32 HalIspGetIntStatus4(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    return (~handle->isp0_cfg->reg_c_irq_mask4) & handle->isp0_cfg->reg_irq_final_status4;
}

void HalIspMaskClear1(ISP_HAL_HANDLE *hnd, u32 clear)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_clr |= clear;
    handle->isp0_cfg->reg_c_irq_clr = 0;
    //handle->isp0_cfg->reg_c_irq_mask = u4DefDisable[0];
}

void HalIspMaskClear2(ISP_HAL_HANDLE *hnd, u32 clear)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_clr2 |= clear;
    handle->isp0_cfg->reg_c_irq_clr2 = 0;
    //handle->isp0_cfg->reg_c_irq_mask2 = u4DefDisable[1];
}

void HalIspMaskClear3(ISP_HAL_HANDLE *hnd, u32 clear)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_clr3 |= clear;
    handle->isp0_cfg->reg_c_irq_clr3 = 0;
    //handle->isp0_cfg->reg_c_irq_mask3 = u4DefDisable[2];
}

void HalIspMaskClear4(ISP_HAL_HANDLE *hnd, u32 clear)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_clr4 |= clear;
    handle->isp0_cfg->reg_c_irq_clr4 = 0;
    //handle->isp0_cfg->reg_c_irq_mask4 = u4DefDisable[3];
}

void HalIspMaskInt1(ISP_HAL_HANDLE *hnd, u32 mask)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask |= (mask | u4DefDisable[0]);
}

void HalIspMaskInt2(ISP_HAL_HANDLE *hnd, u32 mask)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask2 |= (mask | u4DefDisable[1]);
}

void HalIspMaskInt3(ISP_HAL_HANDLE *hnd, u32 mask)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask3 |= (mask | u4DefDisable[2]);
}

void HalIspMaskInt4(ISP_HAL_HANDLE *hnd, u32 mask)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask4 |= (mask | u4DefDisable[3]);
}

void HalIspClearInt(ISP_HAL_HANDLE *hnd, volatile u32 *clear, ISP_INT_STATUS int_num)
{
    *clear |= (0x01 << int_num);
}

void HalIspClearIntAll(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_c_irq_mask = handle->isp0_cfg->reg_c_irq_clr;
    handle->isp0_cfg->reg_c_irq_clr = 0;
    handle->isp0_cfg->reg_c_irq_mask2 = handle->isp0_cfg->reg_c_irq_clr2;
    handle->isp0_cfg->reg_c_irq_clr2 = 0;
    handle->isp0_cfg->reg_c_irq_mask3 = handle->isp0_cfg->reg_c_irq_clr3;
    handle->isp0_cfg->reg_c_irq_clr3 = 0;
    handle->isp0_cfg->reg_c_irq_mask4 = handle->isp0_cfg->reg_c_irq_clr4;
    handle->isp0_cfg->reg_c_irq_clr4 = 0;
}


void HalIspReset(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    u32 fifo_mask = handle->isp0_cfg->reg_isp_sensor_mask;
    handle->isp0_cfg->reg_isp_sensor_mask = 1;//fifo gating
    handle->isp0_cfg->reg_isp_sw_rstz = 0;
    //handle->isp0_cfg->reg_sensor_sw_rstz = 0;
    //handle->isp0_cfg->reg_isp_sw_p1_rstz = 0;
    //handle->isp0_cfg->reg_isp_sw_p2_rstz = 0;
    //udelay(1);
    udelay(1);
    //handle->isp0_cfg->reg_sensor_sw_rstz = 1;
    handle->isp0_cfg->reg_isp_sw_rstz = 1;
    handle->isp0_cfg->reg_isp_sensor_mask = fifo_mask;//fifo gating
    //handle->isp0_cfg->reg_isp_sw_p1_rstz = 1;
    //handle->isp0_cfg->reg_isp_sw_p2_rstz = 1;
}

void HalIspInputEnable(ISP_HAL_HANDLE *hnd, u32 enable)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    if(enable)
    {
        //handle->isp0_cfg->reg_isp_sensor_mask = 0;
        //*(volatile unsigned short *)0x1F262404 &= ~1;
        handle->isp0_cfg->reg_isp_icp_ack_tie1 = 0;
    }
    else
    {
        //mask VIF
        //handle->isp0_cfg->reg_isp_sensor_mask = 1;
        //*(volatile unsigned short *)0x1F262404 |= 1;
        handle->isp0_cfg->reg_isp_icp_ack_tie1 = 1;
    }
}

void HalIspAsyncEnable(ISP_HAL_HANDLE *hnd, u32 enable)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_isp_icp_ack_tie1 = enable ? 1 : 0;
}

void HalIspSetAEDgain(ISP_HAL_HANDLE *hnd, u32 enable, u32 gain)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp1_cfg->reg_isp_ae_en = enable ? 1 : 0;
    handle->isp1_cfg->reg_isp_ae_gain = gain;
}

void HalIspSetYUVCCM(ISP_HAL_HANDLE *hnd, const s16 *ccm_coeff)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    int n = 0;
    //volatile isp_ccm_coeff* coeff = (isp_ccm_coeff*) (((char*)handle->isp0_cfg) + offset_of_reg_isp_r2y_coeff_11);
    volatile isp_ccm_coeff* coeff = (isp_ccm_coeff*) reg_addr(handle->isp0_cfg, isp0_reg_isp_r2y_coeff_11);
    for(n = 0; n < 9; ++n)
    {
        coeff[n].val = ccm_coeff[n] & 0x1FFF;
    }
}

u8 HalISPGetFrameDoneCount(ISP_HAL_HANDLE *hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    return handle->isp0_cfg->reg_isp_frm_done_cnt;
}

void HalIspIrqInit(ISP_HAL_HANDLE *hnd)
{
}
