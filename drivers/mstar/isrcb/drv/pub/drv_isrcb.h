#ifndef _DRV_ISRCB_
#define _DRV_ISRCB_

typedef enum
{
    eISRCB_ID_ISP_WDMA0_DONE=0,
    eISRCB_ID_ISP_WDMA1_DONE,
    eISRCB_ID_ISP_WDMA2_DONE,
    eISRCB_ID_ISP_WDMA3_DONE,
    eISRCB_ID_MAX,
}ISRCB_ID_e;

typedef void* ISRCB_Handle;
typedef void (*ISRCB_fp)(void* pData);

ISRCB_Handle ISRCB_RegisterCallback(ISRCB_ID_e eID,ISRCB_fp fpCB,void* pData);
void ISRCB_UnRegisterCallback(ISRCB_Handle hHnd);
void ISRCB_Proc(ISRCB_ID_e eID);

#endif
