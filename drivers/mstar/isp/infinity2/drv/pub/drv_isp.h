#ifndef _DRV_ISP_H
#define _DRV_ISP_H

#include <hal_isp_shadow.h>
#include <drv_isp_shadow.h>

#define MAX_ISP_CH 4 //Maximum sensor channel of ISP

typedef void* ISP_DRV_HANDLE;
ISP_DRV_HANDLE DrvIsp_Open(void);
void DrvIsp_Close(ISP_DRV_HANDLE handle);
int DrvIsp_PowerOn(ISP_DRV_HANDLE handle);
int DrvIsp_PowerOff(ISP_DRV_HANDLE handle);
int DrvIsp_InitHw(ISP_DRV_HANDLE handle);

int DrvIsp_ApplyShadow(ISP_DRV_HANDLE handle,HalIspRegShadow_t *pIspShd,IQ_SHAD_ENUM BlkInd);
int DrvIsp_ApplyShadowAll(ISP_DRV_HANDLE handle,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);

int DrvIsp_WDMACapture(ISP_DRV_HANDLE handle, ISP_WDMA_ATTR wdma_attr);

#endif
