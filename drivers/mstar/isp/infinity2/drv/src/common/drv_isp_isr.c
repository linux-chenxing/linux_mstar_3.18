#include <isp_sys_api.h>
#include <drv_isp.h>
#include <hal_interrupt_handler.h>
#include <drv_isp_private.h>
//#include <isp_os_wrapper/pub/isp_os_wrapper.h>

///////////////// ISR //////////////////
typedef struct
{
    u32 nIspIntCnt[ISP_INT_MAX];
    u32 nIspInt2Cnt[ISP_INT2_MAX];
    u32 nIspInt3Cnt[ISP_INT3_MAX];
    u32 nIspInt4Cnt[ISP_INT4_MAX];
    u32 nFrameCount;
}IspIsrStatis_t;
IspIsrStatis_t gIspInts;

#define INC_COUNT(name) {gIspInts.nIspIntCnt[name]++;}
#define INC_COUNT2(name) {gIspInts.nIspInt2Cnt[name]++;}
#define INC_COUNT3(name) {gIspInts.nIspInt3Cnt[name]++;}
#define INC_COUNT4(name) {gIspInts.nIspInt4Cnt[name]++;}

//#define EXEC_ISP_CALLBACK(pISPCB) { if(pISPCB) {pISPCB();} }
int DrvIspISR(void* pParam)
{
    drv_isp_handle *isp = (drv_isp_handle*)pParam;
    ISP_HAL_HANDLE hal = (ISP_HAL_HANDLE) isp->hal;
    volatile u32 u4Clear = 0;
    volatile u32 u4Clear2 = 0;
    volatile u32 u4Clear3 = 0;
    volatile u32 u4Clear4 = 0;

    volatile u32 u4Status;
    volatile u32 u4Status2;
    volatile u32 u4Status3;
    volatile u32 u4Status4;

    static int IspInputEnable;

    u4Status = HalIspGetIntStatus1(hal);
    //HalIspMaskInt1(hal, u4Status);
    u4Status2 = HalIspGetIntStatus2(hal);
    //HalIspMaskInt2(hal, u4Status2);
    u4Status3 = HalIspGetIntStatus3(hal);
    //HalIspMaskInt3(hal, u4Status3);
    u4Status4 = HalIspGetIntStatus4(hal);
    //HalIspMaskInt4(hal, u4Status4);

    //UartSendTrace("## [%s] mask1 0x%04x, mask2 0x%04x, mask3 0x%04x\n", __func__, u4Status, u4Status2, u4Status3);
    //UartSendTrace("\nISR +");
    pr_debug("ISP irq S1=0x%X, S2=0x%X, S3=0x%X, S4=0x%X\n",u4Status,u4Status2,u4Status3,u4Status4);
    if(ISP_CHECKBITS(u4Status3, INT3_SW_INT_INPUT_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_SW_INT_INPUT_DONE);
        if(IspInputEnable == 0)
        {
            HalIspInputEnable(hal, 1);
            IspInputEnable = 1;
        }
        INC_COUNT3(INT3_SW_INT_INPUT_DONE);
        //wake_up_interruptible_all(&isp_wq_sw_int_in);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_MENULOAD_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_MENULOAD_DONE);
        if(IspInputEnable == 0)
        {
            HalIspInputEnable(hal, 1);
            IspInputEnable = 1;
        }
        INC_COUNT3(INT3_MENULOAD_DONE);
        //wake_up_interruptible_all(&isp_wq_sw_int_in);
    }



    ////////////////////////////////////////////
    //             Vsync Start                //
    ////////////////////////////////////////////
    //all sensor use ISP vsync as Frame start
    if(ISP_CHECKBITS(u4Status, INT_ISPIF_VSYNC))
    {
        HalIspClearInt(hal, &u4Clear, INT_ISPIF_VSYNC);
        //wake_up_interruptible_all(&isp_wq_VSTART);
        //MsFlagSetbits(&_ints_event_flag, FRAME_START_INTS);
        INC_COUNT(INT_ISPIF_VSYNC);
        gIspInts.nFrameCount++;
        //UartSendTrace(" FS ");
    }

    if(ISP_CHECKBITS(u4Status, INT_P0_VIF_FS_RISING))
    {
        HalIspClearInt(hal, &u4Clear, INT_P0_VIF_FS_RISING);
        //wake_up_interruptible_all(&isp_wq_hit_line_count2);
        INC_COUNT(INT_P0_VIF_FS_RISING);
    }

    if(ISP_CHECKBITS(u4Status, INT_P0_VIF_FS_FALLING))
    {
        HalIspClearInt(hal, &u4Clear, INT_P0_VIF_FS_FALLING);
        INC_COUNT(INT_P0_VIF_FS_FALLING);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE1_VIF_FS_RISING))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE1_VIF_FS_RISING);
        INC_COUNT4(INT4_PIPE1_VIF_FS_RISING);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE1_VIF_FS_FALLING))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE1_VIF_FS_FALLING);
        INC_COUNT4(INT4_PIPE1_VIF_FS_FALLING);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE2_VIF_FS_RISING))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE2_VIF_FS_RISING);
        INC_COUNT4(INT4_PIPE2_VIF_FS_RISING);
    }
    //////////////////////////////////////////////
    //           statistics                     //
    //////////////////////////////////////////////
    if(ISP_CHECKBITS(u4Status, INT_AE_DONE))
    {
        HalIspClearInt(hal, &u4Clear, INT_AE_DONE);
        //wake_up_interruptible_all(&isp_wq_ae);

        //memcpy(isp->ae_stats_output_buf,isp->ae_stats_hw_buf,128*90*4);
        //memcpy(isp->histo_stats_output_buf,isp->histo_stats_hw_buf,128*2*2);
        //MsFlagSetbits(&_ints_event_flag, AE_DONE_INTS);
        INC_COUNT(INT_AE_DONE);
    }

    // AE WIN0/1 Int Row
    if(ISP_CHECKBITS(u4Status3, INT3_AE_WIN0_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_AE_WIN0_DONE);
        //wake_up_interruptible_all(&isp_wq_ae_win0);
        INC_COUNT3(INT3_AE_WIN0_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_AE_BLK_ROW_INT_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_AE_BLK_ROW_INT_DONE);
        //MsFlagSetbits(&_ints_event_flag, AE_DONE_INTS);
        //wake_up_interruptible_all(&isp_wq_ae_row_int);
        INC_COUNT3(INT3_AE_BLK_ROW_INT_DONE);
        //UartSendTrace(" AE ");
    }

    if(ISP_CHECKBITS(u4Status, INT_AWB_DONE))
    {
        HalIspClearInt(hal, &u4Clear, INT_AWB_DONE);
        //wake_up_interruptible_all(&isp_wq_awb);
        //memcpy(isp->awb_stats_output_buf,isp->awb_stats_hw_buf,128*90*3);
        //MsFlagSetbits(&_ints_event_flag, AWB_DONE_INTS);
        INC_COUNT(INT_AWB_DONE);
        //UartSendTrace(" AWB ");
    }

    if(ISP_CHECKBITS(u4Status, INT_AF_DONE))
    {
        HalIspClearInt(hal, &u4Clear, INT_AF_DONE);
        //wake_up_interruptible_all(&isp_wq_af);
        INC_COUNT(INT_AF_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_HIT_LINE_COUNT1))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_HIT_LINE_COUNT1);
        //wake_up_interruptible_all(&isp_wq_hit_line_count1);
        INC_COUNT3(INT3_HIT_LINE_COUNT1);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_HIT_LINE_COUNT2))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_HIT_LINE_COUNT2);
        //wake_up_interruptible_all(&isp_wq_hit_line_count2);
        INC_COUNT3(INT3_HIT_LINE_COUNT2);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_HIT_LINE_COUNT3))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_HIT_LINE_COUNT3);
        //wake_up_interruptible_all(&isp_wq_ISP_IDLE);
        INC_COUNT3(INT3_HIT_LINE_COUNT3);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_HDR_HISTO_DONE))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_HDR_HISTO_DONE);
        //wake_up_interruptible_all(&isp_wq_hdr_histo_done);
        INC_COUNT4(INT4_HDR_HISTO_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_RGBIR_HISTO_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_RGBIR_HISTO_DONE);
        //wake_up_interruptible_all(&isp_wq_rgbir_histo_done);
        INC_COUNT3(INT3_RGBIR_HISTO_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_AWB_ROW_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_AWB_ROW_DONE);
        //wake_up_interruptible_all(&isp_wq_awb_row_done);
        INC_COUNT3(INT3_AWB_ROW_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_HISTO_ROW_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_HISTO_ROW_DONE);
        //wake_up_interruptible_all(&isp_wq_histo_row_done);
        INC_COUNT3(INT3_HISTO_ROW_DONE);
    }

    if(ISP_CHECKBITS(u4Status3, INT3_SW_INT_OUTPUT_DONE))
    {
        HalIspClearInt(hal, &u4Clear3, INT3_SW_INT_OUTPUT_DONE);
        //wake_up_interruptible_all(&isp_wq_sw_int_out);
        //MsFlagSetbits(&_ints_event_flag, FRAME_END_INTS);
        INC_COUNT3(INT3_SW_INT_OUTPUT_DONE);
        //UartSendTrace(" FE ");
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA0_HIT_LINE_COUNT))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA0_HIT_LINE_COUNT);
        INC_COUNT2(INT2_WDMA0_HIT_LINE_COUNT);
        //EXEC_ISP_CALLBACK(gpWDMALineCntCallBackFunc[ISP_DMA_PIPE0]);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA1_HIT_LINE_COUNT))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA1_HIT_LINE_COUNT);
        INC_COUNT2(INT2_WDMA1_HIT_LINE_COUNT);
        //EXEC_ISP_CALLBACK(gpWDMALineCntCallBackFunc[ISP_DMA_PIPE1]);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA2_HIT_LINE_COUNT))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA2_HIT_LINE_COUNT);
        INC_COUNT2(INT2_WDMA2_HIT_LINE_COUNT);
        //EXEC_ISP_CALLBACK(gpWDMALineCntCallBackFunc[ISP_DMA_PIPE2]);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA_HIT_LINE_COUNT))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA_HIT_LINE_COUNT);
        INC_COUNT2(INT2_WDMA_HIT_LINE_COUNT);
        //EXEC_ISP_CALLBACK(gpWDMALineCntCallBackFunc[ISP_DMA_PIPE2]);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE1_HIT_INPUT_LINE_COUNT1))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE1_HIT_INPUT_LINE_COUNT1);
        INC_COUNT4(INT4_PIPE1_HIT_INPUT_LINE_COUNT1);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE1_HIT_INPUT_LINE_COUNT2))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE1_HIT_INPUT_LINE_COUNT2);
        INC_COUNT4(INT4_PIPE1_HIT_INPUT_LINE_COUNT2);
    }

    if(ISP_CHECKBITS(u4Status4, INT4_PIPE1_HIT_INPUT_LINE_COUNT3))
    {
        HalIspClearInt(hal, &u4Clear4, INT4_PIPE1_HIT_INPUT_LINE_COUNT3);
        INC_COUNT4(INT4_PIPE1_HIT_INPUT_LINE_COUNT3);
    }

    if(ISP_CHECKBITS(u4Status, INT_ISP_BUSY_RISING_EDGE))
    {
        HalIspClearInt(hal, &u4Clear, INT_ISP_BUSY_RISING_EDGE);
        //wake_up_interruptible_all(&isp_wq_ISP_BUSY);
        INC_COUNT(INT_ISP_BUSY_RISING_EDGE);
    }

    if(ISP_CHECKBITS(u4Status, INT_ISP_BUSY_FALLING_EDGE))
    {
        HalIspClearInt(hal, &u4Clear, INT_ISP_BUSY_FALLING_EDGE);
        //wake_up_interruptible_all(&isp_wq_ISP_IDLE);
        INC_COUNT(INT_ISP_BUSY_FALLING_EDGE);
    }

    ////////////////////////////////////////////
    //             Vsync end                  //
    ////////////////////////////////////////////

    ////////////////////////////////////////////
    //             DMA                        //
    ////////////////////////////////////////////
    if(ISP_CHECKBITS(u4Status, INT_WDMA_DONE))
    {
        HalIspClearInt(hal, &u4Clear, INT_WDMA_DONE);
        //HalIspClearInt(hal, &u4Clear, INT_PAD_VSYNC_RISING);
        //HalIspClearInt(hal, &u4Clear, INT_PAD_VSYNC_FALLING);
        //wake_up_interruptible_all(&isp_wq_WDMA_DONE);
        INC_COUNT(INT_WDMA_DONE);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA0_DONE))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA0_DONE);
        //wake_up_interruptible_all(&isp_wq_WDMA_DONE);
        //DrvIsp_QueueJob(0,0);
        INC_COUNT(INT2_WDMA0_DONE);
        //EXEC_ISP_CALLBACK(gpWDMADoneCallBackFunc[ISP_DMA_PIPE0]);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA1_DONE))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA1_DONE);
        //wake_up_interruptible_all(&isp_wq_WDMA_DONE);
        //DrvIsp_QueueJob(1,0);
        INC_COUNT(INT2_WDMA1_DONE);
        //EXEC_ISP_CALLBACK(gpWDMADoneCallBackFunc[ISP_DMA_PIPE1]);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_WDMA2_DONE))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_WDMA2_DONE);
        //wake_up_interruptible_all(&isp_wq_WDMA_DONE);
        //DrvIsp_QueueJob(2,0);
        INC_COUNT(INT2_WDMA2_DONE);
        //EXEC_ISP_CALLBACK(gpWDMADoneCallBackFunc[ISP_DMA_PIPE2]);
    }

    if(ISP_CHECKBITS(u4Status, INT_RDMA_DONE))
    {
        HalIspClearInt(hal, &u4Clear, INT_RDMA_DONE);
        INC_COUNT(INT_RDMA_DONE);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_RDMA0_DONE))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_RDMA0_DONE);
        INC_COUNT2(INT2_RDMA0_DONE);
    }

    if(ISP_CHECKBITS(u4Status2, INT2_RDMA1_DONE))
    {
        HalIspClearInt(hal, &u4Clear2, INT2_RDMA1_DONE);
        INC_COUNT2(INT2_RDMA1_DONE);
    }

    ////////////////////////////////////////////
    //             FIFO Full                  //
    ////////////////////////////////////////////
    if(ISP_CHECKBITS(u4Status, INT_ISP_FIFO_FULL))
    {
        HalIspClearInt(hal, &u4Clear, INT_ISP_FIFO_FULL);
        pr_info("ISP FIFO FULL\n");
        HalIspReset(hal); //force ISP reset , when ISP FIFO FULL happen
        //wake_up_interruptible_all(&isp_wq_ISP_FIFO_FULL);
        INC_COUNT(INT_ISP_FIFO_FULL);
    }

    if(ISP_CHECKBITS(u4Status, INT_PIPE1_FIFO_FULL))
    {
        HalIspClearInt(hal, &u4Clear, INT_PIPE1_FIFO_FULL);
        INC_COUNT(INT_PIPE1_FIFO_FULL);
    }

    if(ISP_CHECKBITS(u4Status, INT_PIPE2_FIFO_FULL))
    {
        HalIspClearInt(hal, &u4Clear, INT_PIPE2_FIFO_FULL);
        INC_COUNT(INT_PIPE2_FIFO_FULL);
    }

    if(ISP_CHECKBITS(u4Status, INT_WDMA_FIFO_FULL))
    {
        HalIspClearInt(hal, &u4Clear, INT_WDMA_FIFO_FULL);
        //wake_up_interruptible_all(&isp_wq_WDMA_FIFO_FULL);
        INC_COUNT(INT_WDMA_FIFO_FULL);
    }

    HalIspMaskClear1(hal, u4Clear);
    HalIspMaskClear2(hal, u4Clear2);
    HalIspMaskClear3(hal, u4Clear3);
    HalIspMaskClear4(hal, u4Clear4);
    //UartSendTrace(" ISR- \n ");
    return 0;
}
