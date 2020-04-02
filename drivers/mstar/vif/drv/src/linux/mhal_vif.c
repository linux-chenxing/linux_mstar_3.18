#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "drv_vif.h"

#include <MsTypes.h>
#include "mhal_vif.h"

/* new API */
MS_S32 MHal_VIF_Init(void)
{
    return DrvVif_Init();
}
EXPORT_SYMBOL(MHal_VIF_Init);

MS_S32 MHal_VIF_Deinit(void)
{
    return DrvVif_Deinit();
}
EXPORT_SYMBOL(MHal_VIF_Deinit);

MS_S32 MHal_VIF_Reset(void)
{
    return DrvVif_Reset();
}
EXPORT_SYMBOL(MHal_VIF_Reset);

MS_S32 MHal_VIF_DevSetConfig(MHal_VIF_DEV u32VifDev, MHal_VIF_DevCfg_t *pstDevAttr)
{
    return DrvVif_DevSetConfig(u32VifDev, pstDevAttr);
}
EXPORT_SYMBOL(MHal_VIF_DevSetConfig);

MS_S32 MHal_VIF_DevEnable(MHal_VIF_DEV u32VifDev)
{
    return DrvVif_DevEnable(u32VifDev);
}
EXPORT_SYMBOL(MHal_VIF_DevEnable);

MS_S32 MHal_VIF_DevDisable(MHal_VIF_DEV u32VifDev)
{
    return DrvVif_DevDisable(u32VifDev);
}
EXPORT_SYMBOL(MHal_VIF_DevDisable);

MS_S32 MHal_VIF_ChnSetConfig(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnCfg_t *pstAttr)
{
    return DrvVif_ChnSetConfig(u32VifChn, pstAttr);
}
EXPORT_SYMBOL(MHal_VIF_ChnSetConfig);

MS_S32 MHal_VIF_ChnEnable(MHal_VIF_CHN u32VifChn)
{
    return DrvVif_ChnEnable(u32VifChn);
}
EXPORT_SYMBOL(MHal_VIF_ChnEnable);

MS_S32 MHal_VIF_ChnDisable(MHal_VIF_CHN u32VifChn)
{
    return DrvVif_ChnDisable(u32VifChn);
}
EXPORT_SYMBOL(MHal_VIF_ChnDisable);

MS_S32 MHal_VIF_ChnQuery(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnStat_t *pstStat)
{
    return DrvVif_ChnQuery(u32VifChn, pstStat);
}
EXPORT_SYMBOL(MHal_VIF_ChnQuery);

MS_S32 MHal_VIF_SubChnSetConfig(MHal_VIF_CHN u32VifChn, MHal_VIF_SubChnCfg_t *pstAttr)
{
    return DrvVif_SubChnSetConfig(u32VifChn, pstAttr);
}
EXPORT_SYMBOL(MHal_VIF_SubChnSetConfig);

MS_S32 MHal_VIF_SubChnEnable(MHal_VIF_CHN u32VifChn)
{
    return DrvVif_SubChnEnable(u32VifChn);
}
EXPORT_SYMBOL(MHal_VIF_SubChnEnable);

MS_S32 MHal_VIF_SubChnDisable(MHal_VIF_CHN u32VifChn)
{
    return DrvVif_SubChnDisable(u32VifChn);
}
EXPORT_SYMBOL(MHal_VIF_SubChnDisable);

MS_S32 MHal_VIF_SubChnQuery(MHal_VIF_CHN u32VifChn, MHal_VIF_ChnStat_t *pstStat)
{
    return DrvVif_SubChnQuery(u32VifChn, pstStat);
}
EXPORT_SYMBOL(MHal_VIF_SubChnQuery);

MS_S32 MHal_VIF_QueueFrameBuffer(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, const MHal_VIF_RingBufElm_t *ptFbInfo)
{
    return DrvVif_QueueFrameBuffer(u32VifChn, u32ChnPort, ptFbInfo);
}
EXPORT_SYMBOL(MHal_VIF_QueueFrameBuffer);

MS_S32 MHal_VIF_QueryFrames(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, MS_U32 *pNumBuf)
{
    return DrvVif_QueryFrames(u32VifChn, u32ChnPort, pNumBuf);
}
EXPORT_SYMBOL(MHal_VIF_QueryFrames);

MS_S32 MHal_VIF_DequeueFrameBuffer(MHal_VIF_CHN u32VifChn, MHal_VIF_PORT u32ChnPort, MHal_VIF_RingBufElm_t *ptFbInfo)
{
    return DrvVif_DequeueFrameBuffer(u32VifChn, u32ChnPort, ptFbInfo);
}
EXPORT_SYMBOL(MHal_VIF_DequeueFrameBuffer);
