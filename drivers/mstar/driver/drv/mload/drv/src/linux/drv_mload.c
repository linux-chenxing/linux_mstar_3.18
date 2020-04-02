#include <mload_common.h>
#include<mdrv_mload.h>
#include<drv_mload.h>
#include<hal_mload.h>

typedef struct{

    MLOAD_IQ_MEM *mload_mem_cfg;
    FrameSyncMode mode;
    MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t;
}mload_handle;

int DrvIsp_MLoadInit(MLOAD_HANDLE *handle,MLOAD_ATTR attr)
{
    mload_handle *isp;

    *handle = MloadMalloc(sizeof(mload_handle));

    isp = (mload_handle*)*handle;

    isp->mode = attr.mode;
    isp->pCmqInterface_t = attr.pCmqInterface_t;

  //RTK OS
  isp->mload_mem_cfg = MloadMalloc(sizeof(MLOAD_IQ_MEM));//isp->mload_mem_cfg = malloc(sizeof(MLOAD_IQ_MEM));
  if(!isp->mload_mem_cfg)
    return FAIL;
  memset(isp->mload_mem_cfg, 0, sizeof(MLOAD_IQ_MEM));


  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE0_FPN] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE0_GMA16TO16] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE0_ALSC] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE1_FPN] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE1_GMA12TO12] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_PIPE1_ALSC] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_DPC] = false;
  isp->mload_mem_cfg->bDirty[MLOAD_ID_GMA10TO10] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_YUV_GAMMA_Y] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_YUV_GAMMA_UV] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_RGB12TO12_PRE] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_RGB12TO12_POST] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_WDR] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_C_SC1] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_Y_SC1] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_C_SC1] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_Y_SC1] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_C_SC2] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_Y_SC2] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_C_SC2] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_Y_SC2] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_C_SC3] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_Y_SC3] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_C_SC3] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_Y_SC3] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_C_SC4] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_HSP_Y_SC4] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_C_SC4] = false;
  isp->mload_mem_cfg->bDirty[SCL_MLOAD_ID_VSP_Y_SC4] = false;

  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE0_FPN] = member_size(MLoadLayout,pipe0_fpn);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE0_GMA16TO16] = member_size(MLoadLayout,pipe0_gamma16to16);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE0_ALSC] = member_size(MLoadLayout,pipe0_alsc);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE1_FPN] = member_size(MLoadLayout,pipe1_fpn);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE1_GMA12TO12] = member_size(MLoadLayout,pipe1_gamma12to12);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_PIPE1_ALSC] = member_size(MLoadLayout,pipe1_alsc);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_DPC] = member_size(MLoadLayout,dpc);
  isp->mload_mem_cfg->tblRealSize[MLOAD_ID_GMA10TO10] = member_size(MLoadLayout,pipe0_gamma10to10_r);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_YUV_GAMMA_Y] = member_size(MLoadLayout,yuv_gamma_y);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_YUV_GAMMA_UV] = member_size(MLoadLayout,yuv_gamma_uv);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_RGB12TO12_PRE] = member_size(MLoadLayout,rgb12to12_pre);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_RGB12TO12_POST] = member_size(MLoadLayout,rgb12to12_post);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_WDR] = member_size(MLoadLayout,wdr);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_C_SC1] = member_size(MLoadLayout,hsp_c_sc1);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_Y_SC1] = member_size(MLoadLayout,hsp_y_sc1);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_C_SC1] = member_size(MLoadLayout,vsp_c_sc1);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_Y_SC1] = member_size(MLoadLayout,vsp_y_sc1);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_C_SC2] = member_size(MLoadLayout,hsp_c_sc2);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_Y_SC2] = member_size(MLoadLayout,hsp_y_sc2);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_C_SC2] = member_size(MLoadLayout,vsp_c_sc2);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_Y_SC2] = member_size(MLoadLayout,vsp_y_sc2);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_C_SC3] = member_size(MLoadLayout,hsp_c_sc3);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_Y_SC3] = member_size(MLoadLayout,hsp_y_sc3);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_C_SC3] = member_size(MLoadLayout,vsp_c_sc3);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_Y_SC3] = member_size(MLoadLayout,vsp_y_sc3);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_C_SC4] = member_size(MLoadLayout,hsp_c_sc4);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_HSP_Y_SC4] = member_size(MLoadLayout,hsp_y_sc4);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_C_SC4] = member_size(MLoadLayout,vsp_c_sc4);
  isp->mload_mem_cfg->tblRealSize[SCL_MLOAD_ID_VSP_Y_SC4] = member_size(MLoadLayout,vsp_y_sc4);


  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE0_FPN] = offsetof(MLoadLayout,pipe0_fpn);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE0_GMA16TO16] = offsetof(MLoadLayout,pipe0_gamma16to16);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE0_ALSC] = offsetof(MLoadLayout,pipe0_alsc);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE1_FPN] = offsetof(MLoadLayout,pipe1_fpn);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE1_GMA12TO12] = offsetof(MLoadLayout,pipe1_gamma12to12);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_PIPE1_ALSC] = offsetof(MLoadLayout,pipe1_alsc);
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_DPC] = offsetof(MLoadLayout,dpc);;
  isp->mload_mem_cfg->tblOffset[MLOAD_ID_GMA10TO10] = offsetof(MLoadLayout,pipe0_gamma10to10_r);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_YUV_GAMMA_Y] = offsetof(MLoadLayout,yuv_gamma_y);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_YUV_GAMMA_UV] = offsetof(MLoadLayout,yuv_gamma_uv);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_RGB12TO12_PRE] = offsetof(MLoadLayout,rgb12to12_pre);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_RGB12TO12_POST] = offsetof(MLoadLayout,rgb12to12_post);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_WDR] = offsetof(MLoadLayout,wdr);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_C_SC1] = offsetof(MLoadLayout,hsp_c_sc1);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_Y_SC1] = offsetof(MLoadLayout,hsp_y_sc1);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_C_SC1] = offsetof(MLoadLayout,vsp_c_sc1);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_Y_SC1] = offsetof(MLoadLayout,vsp_y_sc1);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_C_SC2] = offsetof(MLoadLayout,hsp_c_sc2);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_Y_SC2] = offsetof(MLoadLayout,hsp_y_sc2);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_C_SC2] = offsetof(MLoadLayout,vsp_c_sc2);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_Y_SC2] = offsetof(MLoadLayout,vsp_y_sc2);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_C_SC3] = offsetof(MLoadLayout,hsp_c_sc3);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_Y_SC3] = offsetof(MLoadLayout,hsp_y_sc3);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_C_SC3] = offsetof(MLoadLayout,vsp_c_sc3);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_Y_SC3] = offsetof(MLoadLayout,vsp_y_sc3);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_C_SC4] = offsetof(MLoadLayout,hsp_c_sc4);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_HSP_Y_SC4] = offsetof(MLoadLayout,hsp_y_sc4);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_C_SC4] = offsetof(MLoadLayout,vsp_c_sc4);
  isp->mload_mem_cfg->tblOffset[SCL_MLOAD_ID_VSP_Y_SC4] = offsetof(MLoadLayout,vsp_y_sc4);


  isp->mload_mem_cfg->length = sizeof(MLoadLayout);

  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE0_FPN] = FPN_TBL_BITWIDTH;//2 bytes
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE0_GMA16TO16] = GMMA_TBL_BITWIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE0_ALSC] = ALSC_TBL_BITWIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE1_FPN] = FPN_TBL_BITWIDTH;//2 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE1_GMA12TO12] = GMMA_TBL_BITWIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_PIPE1_ALSC] = ALSC_TBL_BITWIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_DPC] = DEFECTPIX_TBL_BITWIDTH;//4 bytes;
  isp->mload_mem_cfg->tblSramWidth[MLOAD_ID_GMA10TO10] = GMMA_TBL_BITWIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_YUV_GAMMA_Y] = YUV_GAMMA_Y_WIDTH;//2 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_YUV_GAMMA_UV] = YUV_GAMMA_UV_WIDTH;//4 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_RGB12TO12_PRE] = RGB12TO12_PRE_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_RGB12TO12_POST] = RGB12TO12_POST_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_WDR] = WDR_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_C_SC1] = HSP_C_SC1_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_Y_SC1] = HSP_Y_SC1_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_C_SC1] = VSP_C_SC1_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_Y_SC1] = VSP_Y_SC1_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_C_SC2] = HSP_C_SC2_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_Y_SC2] = HSP_Y_SC2_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_C_SC2] = VSP_C_SC2_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_Y_SC2] = VSP_Y_SC2_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_C_SC3] = HSP_C_SC3_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_Y_SC3] = HSP_Y_SC3_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_C_SC3] = VSP_C_SC3_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_Y_SC3] = VSP_Y_SC3_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_C_SC4] = HSP_C_SC4_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_HSP_Y_SC4] = HSP_Y_SC4_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_C_SC4] = VSP_C_SC4_WIDTH;//8 bytes;
  isp->mload_mem_cfg->tblSramWidth[SCL_MLOAD_ID_VSP_Y_SC4] = VSP_Y_SC4_WIDTH;//8 bytes;

#if ML_DOUBLE_BUFFER
  // Double buffer
  isp->mload_mem_cfg->length *= 2;
#endif
 MLOAD_DEBUG("[%s] MLoad Total Size = %d\n", __FUNCTION__, isp->mload_mem_cfg->length);

  //get memory from msys
  isp->mload_mem_cfg->pVirAddr = (void*)MloadMallocNonCache(sizeof(MLoadLayout)*3,16);
  isp->mload_mem_cfg->u32physAddr = (u32)MloadVA2PA(isp->mload_mem_cfg->pVirAddr);
  isp->mload_mem_cfg->u32RingBufWp = 1;
  isp->mload_mem_cfg->u32RingBufRp = 0;
  MLOAD_DEBUG("[%s] MLoad mload_mem_cfg = %p\n", __FUNCTION__, isp->mload_mem_cfg);
  MLOAD_DEBUG("[%s] MLoad pVirAddr slot0= %p\n", __FUNCTION__, isp->mload_mem_cfg->pVirAddr + (isp->mload_mem_cfg->length*0));
  MLOAD_DEBUG("[%s] MLoad pVirAddr slot1= %p\n", __FUNCTION__, isp->mload_mem_cfg->pVirAddr+ (isp->mload_mem_cfg->length*1));
  MLOAD_DEBUG("[%s] MLoad pVirAddr slot2= %p\n", __FUNCTION__, isp->mload_mem_cfg->pVirAddr+ (isp->mload_mem_cfg->length*2));
  MLOAD_DEBUG("[%s] MLoad u32physAddr = %#x\n", __FUNCTION__, isp->mload_mem_cfg->u32physAddr);
  MLOAD_DEBUG("[%s] MLoad miuAddr = %#x\n", __FUNCTION__, HalUtilPHY2MIUAddr(isp->mload_mem_cfg->u32physAddr));

  isp->mload_mem_cfg->uBufferIdx = 0;

  isp->mload_mem_cfg->u4Base[0] = 0;
  isp->mload_mem_cfg->u4Base[1] = sizeof(MLoadLayout);
  memset(isp->mload_mem_cfg->pVirAddr, 0x00, isp->mload_mem_cfg->length);
  return SUCCESS;
}

int DrvIsp_MLoadFreeBuffer(MLOAD_HANDLE handle)
{
    mload_handle *isp = (mload_handle*)handle;

    MloadFreeNonCache(isp->mload_mem_cfg->pVirAddr);
    MloadFree(isp->mload_mem_cfg);
    MloadFree(handle);

    return SUCCESS;
}


int DrvIsp_MLoadTableSet(MLOAD_HANDLE handle, IspMloadID_e idx, const u16 *table)
{
	mload_handle *isp = (mload_handle*)handle;

    ISP_MLoadTableSet(idx, table, isp->mload_mem_cfg);
    return SUCCESS;
}

int DrvIsp_MLoadApply(MLOAD_HANDLE handle)
{
	mload_handle *isp = (mload_handle*)handle;
	MLOAD_DEBUG("a\n");

    DrvIsp_MloadUpdateWp(handle);

	ISP_EnableMloadIntr(isp->pCmqInterface_t,isp->mode,1); //enable mload done intr

    ISP_MLoadApplyIspDomain(isp->pCmqInterface_t, isp->mload_mem_cfg,isp->mode);
    MLOAD_DEBUG("b\n");

    if(ISP_WaitMloadDone(isp->pCmqInterface_t,isp->mode,6) == SUCCESS){
        ISP_MLoadSwitchClk(isp->pCmqInterface_t,isp->mode,1); //DPC & GAMMA10to10 need to switch to clk_isp_img domain

    }
    MLOAD_DEBUG("c\n");

    ISP_MLoadApplyIspImgDomain(isp->pCmqInterface_t, isp->mload_mem_cfg,isp->mode);
    MLOAD_DEBUG("d\n");


    if(ISP_WaitMloadDone(isp->pCmqInterface_t,isp->mode,6) == SUCCESS){
        ISP_MLoadSwitchClk(isp->pCmqInterface_t,isp->mode,0);
        DrvIsp_MloadUpdateRp(handle);
    }
    MLOAD_DEBUG("e\n");

    //ISP_EnableMloadIntr(isp->pCmqInterface_t,isp->mode,0); //disable mload done intr

    MLOAD_DEBUG("z\n");
    return SUCCESS;
}

int DrvScl_MLoadApply(MLOAD_HANDLE handle)
{
    mload_handle *isp = (mload_handle*)handle;

    //SCL_EnableMloadIntr(isp->pCmqInterface_t,isp->mode,1);
    SCL_MLoadApply(isp->pCmqInterface_t, isp->mload_mem_cfg,isp->mode);
    DrvIsp_MloadUpdateWp(handle);

    if(SCL_WaitMloadDone(isp->pCmqInterface_t,isp->mode,6) == SUCCESS){
            DrvIsp_MloadUpdateRp(handle);
    }


    return SUCCESS;
}

int DrvIsp_MLoadRead(MLOAD_HANDLE handle,int id,int sram_offset,ISP_MLOAD_OUTPUT *output)
{
    mload_handle *isp = (mload_handle*)handle;

    ISP_MLoadRead(isp->pCmqInterface_t,isp->mload_mem_cfg,isp->mode,id,sram_offset, output);
    return SUCCESS;
}

int DrvScl_MLoadRead(MLOAD_HANDLE handle,int id,int sram_offset,ISP_MLOAD_OUTPUT *output)
{
    mload_handle *isp = (mload_handle*)handle;

    SCL_MLoadRead(isp->pCmqInterface_t,isp->mload_mem_cfg,isp->mode, id, sram_offset, output);
    return SUCCESS;
}

void DrvIsp_MLoadDump(MLOAD_HANDLE handle,IspMloadID_e ID, short *buf){

    int i=0,j=0;
    int item_num;
    int item_width;
    ISP_MLOAD_OUTPUT mload_output;
    mload_handle *isp = (mload_handle*)handle;

    item_num = isp->mload_mem_cfg->tblRealSize[ID] / isp->mload_mem_cfg->tblSramWidth[ID] ;
    item_width = isp->mload_mem_cfg->tblSramWidth[ID];

    MLOAD_DEBUG("===== item num=%d, width=%d",item_num,item_width);

    for(i=0,j=0; i< item_num; i++){

        DrvIsp_MLoadRead(isp,ID,i,&mload_output);

        buf[j]=mload_output.mload_rdata[0];

        if(item_width >=4)
            buf[j+1] = mload_output.mload_rdata[1];
        if(item_width >=6)
            buf[j+2] = mload_output.mload_rdata[2];
        if(item_width >=8)
            buf[j+3] = 0;

        if(item_width == 2)
            j+=1;
        else if(item_width == 4 )
            j+=2;
        else if(item_width == 6)
            j+=3;
        else if(item_width == 8)
            j+=4;
    }

}

void DrvScl_MLoadDump(MLOAD_HANDLE handle,IspMloadID_e ID, short *buf){

    int i=0,j=0;
    int item_num;
    int item_width;
    ISP_MLOAD_OUTPUT mload_output;
    mload_handle *isp = (mload_handle*)handle;

    item_num = isp->mload_mem_cfg->tblRealSize[ID] / isp->mload_mem_cfg->tblSramWidth[ID] ;
    item_width = isp->mload_mem_cfg->tblSramWidth[ID];

    MLOAD_DEBUG("===== item num=%d, width=%d\n",item_num,item_width);

    for(i=0,j=0; i< item_num; i++){

        DrvScl_MLoadRead(isp,ID,i,&mload_output);

        buf[j]=mload_output.mload_rdata[0];

        if(item_width >=4)
            buf[j+1] = mload_output.mload_rdata[1];
        if(item_width >=6)
            buf[j+2] = mload_output.mload_rdata[2];
        if(item_width >=8)
            buf[j+3] = 0;

        if(item_width == 2)
            j+=1;
        else if(item_width == 4 )
            j+=2;
        else if(item_width == 6)
            j+=3;
        else if(item_width == 8)
            j+=4;
    }

}


void DrvIsp_MloadUpdateRp(MLOAD_HANDLE handle){
#if 1
    mload_handle *isp = (mload_handle*)handle;

    //mutex

    if((isp->mload_mem_cfg->u32RingBufRp +1 ) % 3 != isp->mload_mem_cfg->u32RingBufWp){
        isp->mload_mem_cfg->u32RingBufRp = (isp->mload_mem_cfg->u32RingBufRp +1 ) % 3;
    }
    else
        MLOAD_DEBUG("Mload ringbuf rp:0x%x over wp:0x%x\n",isp->mload_mem_cfg->u32RingBufRp,isp->mload_mem_cfg->u32RingBufWp);

    //mutex
#endif
    return;
}


void DrvIsp_MloadUpdateWp(MLOAD_HANDLE handle){

    mload_handle *isp = (mload_handle*)handle;

    //mutex

    if((isp->mload_mem_cfg->u32RingBufWp +1 ) % 3 != isp->mload_mem_cfg->u32RingBufRp)
        isp->mload_mem_cfg->u32RingBufWp = (isp->mload_mem_cfg->u32RingBufWp +1 ) % 3;
    else
        MLOAD_DEBUG("Mload ringbuf wp:0x%x over rp:0x%x\n",isp->mload_mem_cfg->u32RingBufWp,isp->mload_mem_cfg->u32RingBufRp);

    //mutex

    return;
}
