#ifndef DRV_MLOAD_H_
#define DRV_MLOAD_H_

#include<mload_common.h>
#include<mdrv_mload.h>
#include <mhal_common.h>
#include<mhal_cmdq.h>

#undef false
#define false 0
#undef true
#define true  1
#define SUCCESS 0
#define FAIL -1

#define ML_DOUBLE_BUFFER 0

#undef offsetof
#define offsetof(S,F)           (u32) (((u8*)&(((S *) 0)->F))-((u8*)0))
#undef member_size
#define member_size(type, member) sizeof(((type *)0)->member)

// MIU
#define GAMMA_TBL_SIZE                 (256 * GMMA_TBL_BITWIDTH/2)  //256  x 64 bits
#define ALSC_TBL_SIZE_W                (61)
#define ALSC_TBL_SIZE_H                (69)
#define ALSC_TBL_SIZE                  (4209 * ALSC_TBL_BITWIDTH/2) //4208 x 64 bits
#define DEFECTPIX_TBL_SIZE             (1024 * DEFECTPIX_TBL_BITWIDTH/2) //1024 x 32 bits
#define FPN_TBL_SIZE                   (2816 * FPN_TBL_BITWIDTH/2)   //2816 x 16 bits

#define GMMA_TBL_BITWIDTH        (8)
#define ALSC_TBL_BITWIDTH        (8)
#define DEFECTPIX_TBL_BITWIDTH   (4)
#define FPN_TBL_BITWIDTH         (2)

#define HSP_C_SC1_SIZE                 (64 * HSP_C_SC1_WIDTH/2)   //64   x64bits
#define HSP_Y_SC1_SIZE                 (64 * HSP_Y_SC1_WIDTH/2)   //64   x64bits
#define VSP_C_SC1_SIZE                 (64 * VSP_C_SC1_WIDTH/2)   //64   x64bits
#define VSP_Y_SC1_SIZE                 (64 * VSP_Y_SC1_WIDTH/2)   //64   x64bits
#define IHC_LUT_SIZE                   (289 * IHC_LUT_WIDTH/2)  //289x64   x64bits
#define ICC_LUT_SIZE                   (289 * ICC_LUT_WIDTH/2)  //289x64   x64bits
#define YUV_GAMMA_Y_SIZE               (256 * YUV_GAMMA_Y_WIDTH/2)    //256   x 16bits
#define YUV_GAMMA_UV_SIZE              (128 * YUV_GAMMA_UV_WIDTH/2)  //289   x 32bits
#define RGB12TO12_PRE_SIZE             (256 * RGB12TO12_PRE_WIDTH/2)  //289   x 64bits
#define RGB12TO12_POST_SIZE            (256 * RGB12TO12_POST_WIDTH/2)  //289   x 64bits
#define WDR_SIZE                       (256 * WDR_WIDTH/2)   //88   x 64bits
#define HSP_C_SC2_SIZE                 (64 * HSP_C_SC2_WIDTH/2)   //64   x64bits
#define HSP_Y_SC2_SIZE                 (64 * HSP_Y_SC2_WIDTH/2)   //64   x64bits
#define VSP_C_SC2_SIZE                 (64 * VSP_C_SC2_WIDTH/2)   //64   x64bits
#define VSP_Y_SC2_SIZE                 (64 * VSP_Y_SC2_WIDTH/2)   //64   x64bits
#define HSP_C_SC3_SIZE                 (64 * HSP_C_SC3_WIDTH/2)   //64   x64bits
#define HSP_Y_SC3_SIZE                 (64 * HSP_Y_SC3_WIDTH/2)   //64   x64bits
#define VSP_C_SC3_SIZE                 (64 * VSP_C_SC3_WIDTH/2)   //64   x64bits
#define VSP_Y_SC3_SIZE                 (64 * VSP_Y_SC3_WIDTH/2)   //64   x64bits
#define HSP_C_SC4_SIZE                 (64 * HSP_C_SC4_WIDTH/2)   //64   x64bits
#define HSP_Y_SC4_SIZE                 (64 * HSP_Y_SC4_WIDTH/2)   //64   x64bits
#define VSP_C_SC4_SIZE                 (64 * VSP_C_SC4_WIDTH/2)   //64   x64bits
#define VSP_Y_SC4_SIZE                 (64 * VSP_Y_SC4_WIDTH/2)   //64   x64bits


#define HSP_C_SC1_WIDTH                 (8)   //x64bits
#define HSP_Y_SC1_WIDTH                 (8)   //64bits
#define VSP_C_SC1_WIDTH                 (8)   //64bits
#define VSP_Y_SC1_WIDTH                 (8)   //64bits
#define IHC_LUT_WIDTH                   (8)   //64bits
#define ICC_LUT_WIDTH                   (8)   //64bits
#define YUV_GAMMA_Y_WIDTH               (2)   //16bits
#define YUV_GAMMA_UV_WIDTH              (4)   //32bits
#define RGB12TO12_PRE_WIDTH             (8)   //64bits
#define RGB12TO12_POST_WIDTH            (8)   //64bits
#define WDR_WIDTH                       (8)   //64bits
#define HSP_C_SC2_WIDTH                 (8)   //64bits
#define HSP_Y_SC2_WIDTH                 (8)   //64bits
#define VSP_C_SC2_WIDTH                 (8)   //64bits
#define VSP_Y_SC2_WIDTH                 (8)   //64bits
#define HSP_C_SC3_WIDTH                 (8)   //64bits
#define HSP_Y_SC3_WIDTH                 (8)   //64bits
#define VSP_C_SC3_WIDTH                 (8)   //64bits
#define VSP_Y_SC3_WIDTH                 (8)   //64bits
#define HSP_C_SC4_WIDTH                 (8)   //64bits
#define HSP_Y_SC4_WIDTH                 (8)   //64bits
#define VSP_C_SC4_WIDTH                 (8)   //64bits
#define VSP_Y_SC4_WIDTH                 (8)   //64bits

/////////////// MIU MenuLoad ///////////
typedef struct
{
    u16 pipe0_fpn[FPN_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_gamma16to16[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_alsc[ALSC_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_fpn[FPN_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_gamma12to12[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe1_alsc[ALSC_TBL_SIZE] __attribute__((aligned(16)));
    u16 dpc[DEFECTPIX_TBL_SIZE] __attribute__((aligned(16)));
    u16 pipe0_gamma10to10_r[GAMMA_TBL_SIZE] __attribute__((aligned(16)));
    u16 wdr[WDR_SIZE] __attribute__((aligned(16)));
    u16 yuv_gamma_y[YUV_GAMMA_Y_SIZE] __attribute__((aligned(16)));
    u16 yuv_gamma_uv[YUV_GAMMA_UV_SIZE] __attribute__((aligned(16)));
    u16 rgb12to12_pre[RGB12TO12_PRE_SIZE] __attribute__((aligned(16)));
    u16 rgb12to12_post[RGB12TO12_POST_SIZE] __attribute__((aligned(16)));
    u16 hsp_c_sc1[HSP_C_SC1_SIZE] __attribute__((aligned(16)));
    u16 hsp_y_sc1[HSP_Y_SC1_SIZE] __attribute__((aligned(16)));
    u16 vsp_c_sc1[VSP_C_SC1_SIZE] __attribute__((aligned(16)));
    u16 vsp_y_sc1[VSP_Y_SC1_SIZE] __attribute__((aligned(16)));
    u16 hsp_c_sc2[HSP_C_SC2_SIZE] __attribute__((aligned(16)));
    u16 hsp_y_sc2[HSP_Y_SC2_SIZE] __attribute__((aligned(16)));
    u16 vsp_c_sc2[VSP_C_SC2_SIZE] __attribute__((aligned(16)));
    u16 vsp_y_sc2[VSP_Y_SC2_SIZE] __attribute__((aligned(16)));
    u16 hsp_c_sc3[HSP_C_SC3_SIZE] __attribute__((aligned(16)));
    u16 hsp_y_sc3[HSP_Y_SC3_SIZE] __attribute__((aligned(16)));
    u16 vsp_c_sc3[VSP_C_SC3_SIZE] __attribute__((aligned(16)));
    u16 vsp_y_sc3[VSP_Y_SC3_SIZE] __attribute__((aligned(16)));
    u16 hsp_c_sc4[HSP_C_SC4_SIZE] __attribute__((aligned(16)));
    u16 hsp_y_sc4[HSP_Y_SC4_SIZE] __attribute__((aligned(16)));
    u16 vsp_c_sc4[VSP_C_SC4_SIZE] __attribute__((aligned(16)));
    u16 vsp_y_sc4[VSP_Y_SC4_SIZE] __attribute__((aligned(16)));

}__attribute__((aligned(16))) MLoadLayout;

typedef struct
{
  short mload_rdata[3];

} ISP_MLOAD_OUTPUT;

typedef struct {
    //DMA_MEM_INFO base_dmem;
	u32         length;
    void*       pVirAddr;
    u32         u32physAddr;
    u32         u32RingBufWp;
    u32         u32RingBufRp;
    u8          uBufferIdx;//current buffer
    u32         u4Base[2];//dram double buffer
    u32         bDirty[SCL_MLOAD_ID_NUM];
    //u32         tblSize[eMLOAD_ID_NUM]; // 16bytes alignment size
    u32         tblOffset[SCL_MLOAD_ID_NUM];// memory offset in dma memory address
    u32         tblRealSize[SCL_MLOAD_ID_NUM];// real iq table size
    u32         tblSramWidth[SCL_MLOAD_ID_NUM];// Logical size in ip side
} MLOAD_IQ_MEM;

int DrvIsp_MLoadInit(MLOAD_HANDLE *handle,MLOAD_ATTR attr);
int DrvIsp_MLoadTableSet(MLOAD_HANDLE handle, IspMloadID_e idx, const u16 *table);
int DrvIsp_MLoadApply(MLOAD_HANDLE handle);
int DrvScl_MLoadApply(MLOAD_HANDLE handle);
int DrvIsp_MLoadFreeBuffer(MLOAD_HANDLE handle);
void DrvIsp_MLoadDump(MLOAD_HANDLE handle,IspMloadID_e ID, short *buf);
void DrvScl_MLoadDump(MLOAD_HANDLE handle,IspMloadID_e ID, short *buf);
void DrvIsp_MloadUpdateWp(MLOAD_HANDLE handle);
void DrvIsp_MloadUpdateRp(MLOAD_HANDLE handle);

#endif //DRV_MLOAD_H_
