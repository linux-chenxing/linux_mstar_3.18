#include <drv_isp.h>

extern IspRegShd_t *g_pstIspRegShds[16];
#if 0
int32_t DrvIspCtrlDispatch(uint32_t nID,void* pParam,uint32_t nSize)
{
    int32_t ret = 0;
    static ISP_DRV_HANDLE handle = NULL;
    switch(nID)
    {
    case eDrvIspCtl_Open:
        handle = DrvIsp_Open();
    break;
    case eDrvIspCtl_Close:
        DrvIsp_Close(handle);
    break;
    /*
    case eDrvIspCtl_PipeConfig:
    break;
    case eDrvIspCtl_NewShadow:
    break;
    */
    case eDrvIspCtl_ApplyShadowAll:
    {
    	IspRegShadowApply_t *ptCtrl = (IspRegShadowApply_t*)pParam;
        ret = DrvIsp_ApplyShadowAll(handle,g_pstIspRegShds[ptCtrl->nShdID]);
    }
    break;
    case eDrvIspCtl_ApplyShadow:
    {
    	IspRegShadowApply_t *ptCtrl = (IspRegShadowApply_t*)pParam;
        ret = DrvIsp_ApplyShadow(handle,g_pstIspRegShds[ptCtrl->nShdID],ptCtrl->nBlkID);
    }
    break;
    /*
    case eDrvIspCtl_NewStatsBuffer:
    break;
    case eDrvIspCtl_DelStatsBuffer:
    break;
    case eDrvIspCtl_ChgStatsBuffer:
    break;
    */
    }
    return ret;
}
#endif
