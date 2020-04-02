#ifndef MDRV_MLOAD_H_
#define MDRV_MLOAD_H_

#include "mhal_common.h"
#include "mhal_cmdq.h"


#define LINUX_UNIT_TEST 1

typedef void*  MLOAD_HANDLE;

typedef enum
{
    MLOAD_ID_PIPE0_FPN = 0,         //pipe0 FPN
    MLOAD_ID_PIPE0_GMA16TO16 = 1,   //pipe0 RGB gamma 16 to 16
    MLOAD_ID_PIPE0_ALSC = 2,        //pipe0 RGB lens shading
    MLOAD_ID_PIPE1_FPN = 4,         //pipe1 FPN
    MLOAD_ID_PIPE1_GMA12TO12 = 5,   //pipe1 RGB gamma 12 to 12
    MLOAD_ID_PIPE1_ALSC = 6,        //pipe1 RGB lens shading
    MLOAD_ID_DPC = 16,              //defect pixel
    MLOAD_ID_GMA10TO10 = 17,         //pipe0 RGB gamma 10 to 10 table
    MLOAD_ID_NUM = 18,
    SCL_MLOAD_ID_BASE = 33,
    SCL_MLOAD_ID_HSP_C_SC1 = 33,
    SCL_MLOAD_ID_HSP_Y_SC1 = 34,
    SCL_MLOAD_ID_VSP_C_SC1 = 35,
    SCL_MLOAD_ID_VSP_Y_SC1 = 36,
    SCL_MLOAD_ID_IHC_LUT = 37,
    SCL_MLOAD_ID_ICC_LUT = 38,
    SCL_MLOAD_ID_YUV_GAMMA_Y = 39,
    SCL_MLOAD_ID_YUV_GAMMA_UV = 40,
    SCL_MLOAD_ID_RGB12TO12_PRE = 41,
    SCL_MLOAD_ID_RGB12TO12_POST = 42,
    SCL_MLOAD_ID_WDR = 43,
    SCL_MLOAD_ID_HSP_C_SC2 = 49,
    SCL_MLOAD_ID_HSP_Y_SC2 = 50,
    SCL_MLOAD_ID_VSP_C_SC2 = 51,
    SCL_MLOAD_ID_VSP_Y_SC2 = 52,
    SCL_MLOAD_ID_HSP_C_SC3 = 53,
    SCL_MLOAD_ID_HSP_Y_SC3 = 54,
    SCL_MLOAD_ID_VSP_C_SC3 = 55,
    SCL_MLOAD_ID_VSP_Y_SC3 = 56,
    SCL_MLOAD_ID_HSP_C_SC4 = 57,
    SCL_MLOAD_ID_HSP_Y_SC4 = 58,
    SCL_MLOAD_ID_VSP_C_SC4 = 59,
    SCL_MLOAD_ID_VSP_Y_SC4 = 60,
    SCL_MLOAD_ID_NUM = 61

}IspMloadID_e;


typedef enum{
    CMDQ_MODE,
    RIU_MODE
}FrameSyncMode;


typedef struct{
    FrameSyncMode mode;
    MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t;
}MLOAD_ATTR;

MLOAD_HANDLE IspMLoadInit(MLOAD_ATTR attr);
int IspMLoadDeInit(MLOAD_HANDLE handle);
int IspMLoadTableSet(MLOAD_HANDLE handle, IspMloadID_e id,void *table);
int IspMLoadTableGet(MLOAD_HANDLE handle, IspMloadID_e id,void *table);
int IspMLoadApply(MLOAD_HANDLE handle);
int SclMLoadApply(MLOAD_HANDLE handle);
void IspMloadUT(MHAL_CMDQ_CmdqInterface_t *cmdq,FrameSyncMode mode);



#endif //MDRV_MLOAD_H_
