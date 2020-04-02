#include <isp_sys_api.h>
#include <drv_isp.h>
#include <drv_isp_private.h>
#include <hal_isp.h>
#include <hal_interrupt_handler.h>
#include <hal_isp_shadow.h>
#include <hal_clkpad.h>
#include <drv_vif_export.h>

#define ISP_DBG 1
#ifdef ISP_DBG
#define ISP_DMSG(args...) pr_debug(args)
#define ISP_EMSG(args...) pr_err(args)
#define ISP_VMSG(args...) pr_info(args)
#else
#define ISP_DMSG(args...) do{}while(0)
#define ISP_EMSG(args...) do{}while(0)
#define ISP_VMSG(args...) do{}while(0)
#endif

typedef struct
{
    uint32_t nStatus; // 0:available ,1: occupied
    HalIspRegShadow_t *ptIspRegsShd;
}RegShdBuf_t;
HalIspRegShadow_t gt_RegShdBuf[MAX_ISP_CH];

int DrvIsp_ApplyShadow(ISP_DRV_HANDLE handle,HalIspRegShadow_t *pIspShd,IQ_SHAD_ENUM BlkInd)
{
    /* TODO: Update register setting into ISP*/
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    IspShdStage_e eStage = eIspShdInit;
    switch(BlkInd)
    {
    case SHAD_DECOMP:
        HalIsp_ApplyDECOMP(isp->hal,pIspShd,eStage);
        break;
    case SHAD_FPN:
        HalIsp_ApplyFPN(isp->hal,pIspShd,eStage);
        break;
    case SHAD_FPN_P1:
        HalIsp_ApplyFPNP1(isp->hal,pIspShd,eStage);
        break;
    case SHAD_OBC:
        HalIsp_ApplyOBC(isp->hal,pIspShd,eStage);
        break;
    case SHAD_OBC_P1:
        HalIsp_ApplyOBCP1(isp->hal,pIspShd,eStage);
        break;
    case SHAD_GAMA_16to16_ISP:
        HalIsp_ApplyGAMA16to16ISP(isp->hal,pIspShd,eStage);
        break;
    case SHAD_GAMA_C2C_P1_ISP:
        HalIsp_ApplyGAMAC2C_P1(isp->hal,pIspShd,eStage);
        break;
    case SHAD_LSC:
        HalIsp_ApplyLSC(isp->hal,pIspShd,eStage);
        break;
    case SHAD_LSC_P1:
        HalIsp_ApplyLSCP1(isp->hal,pIspShd,eStage);
        break;
    case SHAD_ALSC:
        //HalIsp_ApplyALSC(isp->hal,pIspShd,eStage);
        break;
    case SHAD_ALSC_P1:
        HalIsp_ApplyALSCP1(isp->hal,pIspShd,eStage);
        break;
    case SHAD_HDR:
        HalIsp_ApplyHDR(isp->hal,pIspShd,eStage);
        break;
    case SHAD_HDR_16to10:
        HalIsp_ApplyHDR16to10(isp->hal,pIspShd,eStage);
        break;
    case SHAD_DPC:
        HalIsp_ApplyDPC(isp->hal,pIspShd,eStage);
        break;
    case SHAD_GE:
        HalIsp_ApplyGE(isp->hal,pIspShd,eStage);
        break;
    case SHAD_ANTICT:
        HalIsp_ApplyANTICT(isp->hal,pIspShd,eStage);
        break;
    case SHAD_RGBIR:
        HalIsp_ApplyRGBIR(isp->hal,pIspShd,eStage);
        break;
    case SHAD_SpikeNR:
        HalIsp_ApplySpikeNR(isp->hal,pIspShd,eStage);
        break;
    case SHAD_SDC:
        HalIsp_ApplySDC(isp->hal,pIspShd,eStage);
        break;
    case SHAD_BSNR:
        HalIsp_ApplyBSNR(isp->hal,pIspShd,eStage);
        break;
    case SHAD_NM:
        HalIsp_ApplyNM(isp->hal,pIspShd,eStage);
        break;
    case SHAD_DM:
        HalIsp_ApplyDM(isp->hal,pIspShd,eStage);
        break;
    case SHAD_PostDN:
        HalIsp_ApplyPostDN(isp->hal,pIspShd,eStage);
        break;
    case SHAD_FalseColor:
        HalIsp_ApplyFalseColor(isp->hal,pIspShd,eStage);
        break;
    case SHAD_GAMA_A2A_ISP:
        HalIsp_GammaA2A(isp->hal,pIspShd,eStage);
        break;
    default:
        break;
    }
    return 0;
}

int DrvIsp_ApplyShadowAll(ISP_DRV_HANDLE handle,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    ISP_HAL_HANDLE *hnd = isp->hal;
    switch(eStage)
    {
    case eIspShdInit:
        HalIsp_ApplyOBC(hnd,pIspShd,eStage);
        HalIsp_ApplyDPC(hnd,pIspShd,eStage);
        HalIsp_ApplySpikeNR(hnd,pIspShd,eStage);
        HalIsp_ApplyBSNR(hnd,pIspShd,eStage);
        HalIsp_ApplyDM(hnd,pIspShd,eStage);
        HalIsp_GammaA2A(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMA16to16ISP(hnd,pIspShd,eStage);
    break;
    case eIspShdRuntime:
        ISP_IQ_WBG_Cfg(isp->hal,&pIspShd->wbg);
        HalIsp_ApplyDECOMP(hnd,pIspShd,eStage);
        HalIsp_ApplyFPN(hnd,pIspShd,eStage);
        HalIsp_ApplyFPNP1(hnd,pIspShd,eStage);
        HalIsp_ApplyOBC(hnd,pIspShd,eStage);
        HalIsp_ApplyOBCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMA16to16ISP(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMAC2C_P1(hnd,pIspShd,eStage);
        HalIsp_ApplyLSC(hnd,pIspShd,eStage);
        HalIsp_ApplyLSCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyALSC(hnd,pIspShd,eStage);
        HalIsp_ApplyALSCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyHDR(hnd,pIspShd,eStage);
        HalIsp_ApplyHDR16to10(hnd,pIspShd,eStage);
        HalIsp_ApplySDC(hnd,pIspShd,eStage);
        HalIsp_ApplyDPC(hnd,pIspShd,eStage);
        HalIsp_ApplyGE(hnd,pIspShd,eStage);
        HalIsp_ApplyANTICT(hnd,pIspShd,eStage);
        HalIsp_ApplyRGBIR(hnd,pIspShd,eStage);
        HalIsp_ApplySpikeNR(hnd,pIspShd,eStage);
        HalIsp_ApplyBSNR(hnd,pIspShd,eStage);
        HalIsp_ApplyFalseColor(hnd,pIspShd,eStage);
        HalIsp_ApplyNM(hnd,pIspShd,eStage);
        HalIsp_ApplyDM(hnd,pIspShd,eStage);
        HalIsp_ApplyPostDN(hnd,pIspShd,eStage);
        HalIsp_GammaA2A(hnd,pIspShd,eStage);
    break;
    case eIspShdVsyncEnd:
        //TODO: Get WBG from shadow buffer
        //ISP_IQ_WBG_Cfg(isp->hal,&pIspShd->wbg);
        //HalIsp_ApplyALSC(isp->hal,pIspShd,eStage);
    break;
    }
    return 0;
}

//int DrvIsp_NewShadow(ISP_DRV_HANDLE handle,)
//{
//}

int DrvIsp_BufferAllocate(ISP_DRV_HANDLE handle)
{
#if 0
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    isp->ae_stats_hw_buf = MsAllocateNonCacheMem(128*90*4);
    isp->awb_stats_hw_buf = MsAllocateNonCacheMem(128*90*3);
    isp->histo_stats_hw_buf = MsAllocateNonCacheMem(128*2*2);

    isp->ae_stats_output_buf = MsAllocateNonCacheMem(128*90*4);
    isp->awb_stats_output_buf = MsAllocateNonCacheMem(128*90*3);
    isp->histo_stats_output_buf = MsAllocateNonCacheMem(128*2*2);

    isp_pr_info("=== AE stats vir buf:0x%x, phy buf:0x%x\n",isp->ae_stats_hw_buf,MsVA2PA(isp->ae_stats_hw_buf));
    isp_pr_info("=== AWB stats vir buf:0x%x, phy buf:0x%x\n",isp->awb_stats_hw_buf,MsVA2PA(isp->awb_stats_hw_buf));
    isp_pr_info("=== HISTO stats vir buf:0x%x, phy buf:0x%x\n",isp->histo_stats_hw_buf,MsVA2PA(isp->histo_stats_hw_buf));

    ISP_AE_SetStatsBUF(isp->hal,MsVA2PA(isp->ae_stats_hw_buf));
    ISP_AWB_SetStatsBUF(isp->hal,MsVA2PA(isp->awb_stats_hw_buf));
    ISP_HISTO_SetStatsBUF(isp->hal,MsVA2PA(isp->histo_stats_hw_buf));
#endif
    return 0;
}

int DrvIsp_WDMACapture(ISP_DRV_HANDLE handle, ISP_WDMA_ATTR wdma_attr)
{
    int ret = 0;
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    ISP_WdmaReset(isp->hal,ISP_DMA_GENERAL);
    ISP_SetWdmaAttr(isp->hal, wdma_attr);
    ISP_WdmaEnable(isp->hal,ISP_DMA_GENERAL, 1);
    ISP_WdmaTrigger(isp->hal,ISP_DMA_GENERAL);

    /*to do Wait WDMA done */

    return ret;
}

static drv_isp_handle g_IspHandle;
ISP_DRV_HANDLE DrvIsp_Open(void)
{
    int VifMask;
    //isp_handle_t *isp_hal = NULL;
    pr_info("Driver isp init\n");
    //HalClkpad_Set_ISP_Clk(ISP_CLK_144M);
    //HalClkpad_Set_ISP_Clk(1, ISP_CLK_192M/*ISP_CLK_86P4M*/, 0);
    HalClkgenCtrl(CLKGEN_ID_ISP_IMG,CKGGEN_ISP_IMG_320M,1); //enable isp clock
    HalIsp_Init(&g_IspHandle.hal); //init isp hal handle
    //DrvIsp_PowerOn(&g_IspHandle); //enable isp
    //DrvIsp_InitHw(&g_IspHandle);  //init isp regs
    //g_isp_drv = &handle;

    //IspIsrInit(&g_IspHandle);
    //Drv_CSIInit();
    DrvIsp_BufferAllocate(&g_IspHandle);
    //DrvIsp_MLoadInitBuffer(&g_IspHandle);

    //DrvIsp_PowerOn(&g_IspHandle);

    /* TEST MODE */
    pr_info("[ISP] Initialize real time test mode.\n");
    VifMask = DrvVif_InputMask(0,1);//mask VIF channel 0
    msleep(100);
    HalISP_InitTestMode(g_IspHandle.hal,1280,720);
    DrvVif_InputMask(0,VifMask);//restore vif mask

    HalIspEnableInt(g_IspHandle.hal);
    return (ISP_DRV_HANDLE) &g_IspHandle;
}

void DrvIsp_Close(ISP_DRV_HANDLE handle)
{
    drv_isp_handle *isp = (drv_isp_handle*)handle;

    if(isp)
    {
        //DrvIsp_PowerOff(isp);
        HalIsp_Deinit(isp->hal);
        //DrvIsp_MLoadFreeBuffer(isp);
    }

    //HalClkpad_Set_ISP_Clk(0, ISP_CLK_86P4M, 0);
    HalClkgenCtrl(CLKGEN_ID_ISP_IMG,CKGGEN_ISP_IMG_432M,0); //disable isp clock
}

//////////// power management, reset //////////////
int DrvIsp_PowerOn(ISP_DRV_HANDLE handle)
{
    u32 ulisp_pipe_loop;
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    ISP_HAL_HANDLE hal = (ISP_HAL_HANDLE) isp->hal;
    ISP_DisableISPDPClk(hal, false);
    ISP_DisableRawDNClk(hal, false);

    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_0/*TBD*/;ulisp_pipe_loop++){
        ISP_EnableISP(hal, ulisp_pipe_loop, true);
        ISP_SWReset(hal, ulisp_pipe_loop, true);
        ISP_SWReset(hal, ulisp_pipe_loop, false);
    }

    ISP_SensorEnableMCLK(hal, true);
    ISP_EnableSensor(hal, true);
    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_SensorSWReset(hal, ulisp_pipe_loop, true);
        ISP_SensorSWReset(hal, ulisp_pipe_loop, false);
    }
    return SUCCESS;
}

int DrvIsp_PowerOff(ISP_DRV_HANDLE handle)
{
    u32 ulisp_pipe_loop;
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    ISP_HAL_HANDLE hal = (ISP_HAL_HANDLE) isp->hal;

    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_SWReset(hal, ulisp_pipe_loop, true);
        ISP_SWReset(hal, ulisp_pipe_loop, false);
    }
    ISP_DisableISPDPClk(hal, true);
    ISP_DisableRawDNClk(hal, true);
    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_EnableISP(hal, ulisp_pipe_loop, false);
    }
    ISP_SensorEnableMCLK(hal, false);
    ISP_EnableSensor(hal, false);
    return SUCCESS;
}

int DrvIsp_InitHw(ISP_DRV_HANDLE handle)
{
    u32 ulisp_pipe_loop;
    ISP_CROP_WIN crop_win;
    drv_isp_handle *isp = (drv_isp_handle*)handle;
    ISP_HAL_HANDLE hal = (ISP_HAL_HANDLE) isp->hal;
    SEN_DATATYPE type = SEN_DATATYPE_BAYER;
    ISP_DATAPRECISION fmt = ISP_DATAPRECISION_10;
    //CLK_POL vsync_pol = CLK_POL_POS;
    //CLK_POL hsync_pol = CLK_POL_POS;

    // gate
    ISP_LatchMode(hal, false);
    ISP_Kernel_GatedClk(hal, false);
    ISP_Raw_GatedClk(hal, true);
    ISP_ISPDP_GatedClk(hal, false); //WORKAROUND , disable auto clock gating

    // enable sensor
    ISP_DMSG("Sensor Interface!!\n");
    ISP_EnableSensor(hal, true);
    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_SWReset(hal, ulisp_pipe_loop, true);
        ISP_SWReset(hal, ulisp_pipe_loop, false);
        ISP_SensorSWReset(hal, ulisp_pipe_loop, true);
        ISP_SensorSWReset(hal, ulisp_pipe_loop, false);
    }

    ISP_SensorPowerDown(hal, false);
    //double buffer disable, kernel already set
    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_EnableDblBuf(hal, ulisp_pipe_loop, false); //disable double buffer first for stability.
    }
    ISP_SenFmtConv(hal, SEN_10TO12_11200);

    HalIsp_InitReg(hal);

    // IQ Setting
    ISP_DMSG("IQ Setting!!(def:0)\n");
    //ISP_IQEnable(handle->isp_drv, IQ_WBG, true); //Disable for FPGA

    //IQ block disable except CI, UVCM
    ISP_IQEnable(hal, IQ_OBC,           false);
    ISP_IQEnable(hal, IQ_LSC,           false);
    ISP_IQEnable(hal, IQ_CROSSTALK,     false);
    ISP_IQEnable(hal, IQ_DPC,           false);  //off
    ISP_IQEnable(hal, IQ_SPIKE_NR,      false);
    ISP_IQEnable(hal, IQ_MEAN_FILTER,   false);  //BSNR
    ISP_IQEnable(hal, IQ_NOISE_MASKING, false);
    ISP_IQEnable(hal, IQ_FALSECOLOR,    false);
    //ISP_IQEnable(handle->isp_drv, IQ_CFAi,          true); //DM //Disable for FPGA
    ISP_IQEnable(hal, IQ_CFAi_NR,       false);  //PostDenoise
    ISP_IQEnable(hal, IQ_DNR,           false);  //BDNR
    ISP_IQEnable(hal, IQ_CCM,           false);
    //ISP_IQEnable(handle->isp_drv, IQ_UVCM,          true); //Disable for FPGA

    ISP_IQEnable(hal, IQ_GAMMA_RAW_LC,  false);  //GAMAC2A
    ISP_IQEnable(hal, IQ_GAMMA_RGB_LC,  false);  //GAMAA2C
    ISP_IQEnable(hal, IQ_GAMMA,         false);
    ISP_IQEnable(hal, IQ_ALSC,          false);

    // ISP 0
    ISP_DMSG("ISP0\n");
    crop_win.x_start  = 0;
    crop_win.y_start  = 0;
    crop_win.width    = 1280;//640;
    crop_win.height   = 800;//480;
    //PG will overwrite

    for(ulisp_pipe_loop = ISP_PIPE_0; ulisp_pipe_loop <= ISP_PIPE_2;ulisp_pipe_loop++){
        ISP_SenDataPrecision(hal, ulisp_pipe_loop, fmt);
        ISP_SenDataType(hal, ulisp_pipe_loop, type);
    }
    //ISP_SenHsyncPol(hal, hsync_pol);
    //ISP_SenVsyncPol(hal, vsync_pol);
    ISP_SetISPCrop(hal, ISP_PIPE_0, crop_win);
    ISP_SetISPCrop(hal, ISP_PIPE_1, crop_win);
    ISP_SetISPCrop(hal, ISP_PIPE_2, crop_win);
    // switch back to sensor input
    ISP_RDMAEnable(hal,ISP_DMA_PIPE0,false);
    ISP_WDMAEnable(hal,ISP_DMA_PIPE0,false);
    ISP_RDMAEnable(hal,ISP_DMA_PIPE1,false);
    ISP_WDMAEnable(hal,ISP_DMA_PIPE1,false);
    //ISP_RDMAEnable(hal,ISP_DMA_PIPE2,false);
    ISP_WDMAEnable(hal,ISP_DMA_PIPE2,false);
    ISP_RDMAEnable(hal,ISP_DMA_GENERAL,false);
    ISP_WDMAEnable(hal,ISP_DMA_GENERAL,false);
    ISP_RMUXSrc(hal, ISP_PIPE_0, ISP_RMUX_SIF);
    ISP_RMUXSrc(hal, ISP_PIPE_1, ISP_RMUX_SIF);
    ISP_RMUXSrc(hal, ISP_PIPE_2, ISP_RMUX_SIF);
    ISP_WMUXSrc(hal, ISP_PIPE_0, ISP_WMUX_ISP);
    ISP_WMUXSrc(hal, ISP_PIPE_1, ISP_WMUX_ISP);
    ISP_WMUXSrc(hal, ISP_PIPE_2, ISP_WMUX_ISP);
    //pHal_Reset(handle);

    return SUCCESS;
}

