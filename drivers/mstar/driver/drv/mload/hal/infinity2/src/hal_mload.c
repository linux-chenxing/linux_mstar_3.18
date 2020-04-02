#include <hal_mload.h>
#include <mdrv_mload.h>


void ISP_MLoadSwitchClk(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable)
{
    u16 uVal;

    MloadRegR(mode,MLOAD_CLK_BANK,CLK_ISP_MLOAD,&uVal);
    uVal |= (MASK_CLK_ISP_MLOAD & SHIFT_CLK_ISP_MLOD(enable));
    MloadRegW(pCmqInterface_t,mode,MLOAD_CLK_BANK,CLK_ISP_MLOAD,uVal,MASK_CLK_ISP_MLOAD);

    return;
}


int ISP_MLoadTableSet(IspMloadID_e idx, const u16 *table, MLOAD_IQ_MEM *mload_mem_cfg)
{
  void *address;
  //pthread_mutex_lock(&handle->MutexMLoadDone);
  address = MLOAD_BUF_VIRT(mload_mem_cfg,idx);
  MLOAD_DEBUG("MloadTableSet:  address:0x%x,  table:0x%x\n",(int)address, (int)table);
  memcpy(address, (void*)table, MLOAD_TBL_SIZE_B(mload_mem_cfg,idx));
  mload_mem_cfg->bDirty[idx] = true;
  //pthread_mutex_unlock(&handle->MutexMLoadDone);
  return SUCCESS;
}

int ISP_MLoadApplyIspDomain(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode){

	  int i, k;
	  u32 u4OffsetIdx = 0;
	  u16 uVal;
	  unsigned long tbl_addr;
	  MLOAD_DEBUG("f\n");
	  //Enable the engine by turning on the register
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      MLOAD_DEBUG("g\n");
      uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
      MLOAD_DEBUG("h\n");
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
      MLOAD_DEBUG("i\n");
	  //handle->mload_cfg->reg_isp_miu2sram_en = 1;
	  //Set SW reset as 0 and 1
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal &= ~(MASK_SW_RST & SET_SW_RST(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);
	  //handle->mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
	  //usleep(50);
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_SW_RST & SET_SW_RST(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);
	  //handle->mload_cfg->reg_isp_load_sw_rst = 1;
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_WAIT_HW_RDY_EN & SET_WAIT_HW_RDY_EN(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_WAIT_HW_RDY_EN);
	  //handle->mload_cfg->reg_isp_load_wait_hardware_ready_en = 1;
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_LOAD_WATER_LEVEL & SET_LOAD_WATER_LEVEL(0));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_LOAD_WATER_LEVEL);
	  //handle->mload_cfg->reg_isp_load_water_level = 0;
	  //Set SW read write mode as 1 (write)
	  for(i = 0; i<=MLOAD_ID_PIPE1_ALSC; ++i)
	  {
	    u4OffsetIdx = i;

	    if (mload_mem_cfg->bDirty[u4OffsetIdx] == false) {
	      continue;
	    } else
	      mload_mem_cfg->bDirty[u4OffsetIdx] = false;

	    tbl_addr = MLOAD_BUF_PHYS(mload_mem_cfg,u4OffsetIdx);
	    MLOAD_DEBUG("=== tbl_addr=0x%x, wp:0x%x length:0x%x\n",(int)tbl_addr, mload_mem_cfg->u32RingBufWp,mload_mem_cfg->length);

	    switch(mload_mem_cfg->tblSramWidth[u4OffsetIdx])
	    {
	        case 2:
	            MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
	            uVal |= (MASK_SRAM_WIDTH & 0 );
	            MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
	            //handle->mload_cfg->reg_isp_sram_width  = 0;
	            break;
	        case 4:
                MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
                uVal |= (MASK_SRAM_WIDTH & 1 );
                MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
	            //handle->mload_cfg->reg_isp_sram_width  = 1;
	            break;
	        case 8:
                MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
                uVal |= (MASK_SRAM_WIDTH & 2);
                MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
	            //handle->mload_cfg->reg_isp_sram_width  = 2;
	            break;
	        default:
	            MLOAD_DEBUG("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
	            break;
	    }

        MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
        uVal |= (MASK_SRAM_RW & SET_SRAM_RW(1));
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_RW);
	    //handle->mload_cfg->reg_isp_sram_rw     = 1;// 1:write
	    //Set SRAM ID
	    MloadRegR(mode,MLOAD_BANK,SRAM_ID,&uVal);
	    uVal |= (MASK_LOAD_SRAM_ID & u4OffsetIdx);
	    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_ID,uVal,MASK_LOAD_SRAM_ID);
	    //handle->mload_cfg->reg_isp_load_sram_id = u4OffsetIdx;
	    //Set MIU 16-byte start address
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_LOAD_ADDR,(HalUtilPHY2MIUAddr(tbl_addr)>>4) & 0xFFFF,0xFFFF);
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_LOAD_ADDR_1,(HalUtilPHY2MIUAddr(tbl_addr)>>20) & 0xFFFF,0xFFFF);
	    //handle->mload_cfg->reg_isp_load_st_addr = (HalUtilPHY2MIUAddr(tbl_addr)>>4) & 0xFFFF;
	    //handle->mload_cfg->reg_isp_load_st_addr_1 = (HalUtilPHY2MIUAddr(tbl_addr)>>20) & 0xFFFF;
	    //Set data amount (2-byte)
	    //The number should be set as length - 1
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,LOAD_AMOUNT,(mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1,0xFFFF);
	    //handle->mload_cfg->reg_isp_load_amount    = (mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1;
	    //Set destination SRAM start SRAM address (2-byte)
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_ST_ADDR, 0x0000,0xFFFF);
	    //handle->mload_cfg->reg_isp_sram_st_addr   = 0x0000;
	    //Set SRAM loading parameter by setting write-one-clear
        MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
        uVal |= (MASK_LOAD_REGISTER_WLR & 1);
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);
	    //handle->mload_cfg->reg_isp_load_register_w1r = 1;

	    //write 0, HW is not working, same as dummy command
	    for(k = 0; k < 32; k++) {
	        MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
	        uVal |= (MASK_LOAD_REGISTER_WLR & 0);
	        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);
	        //handle->mload_cfg->reg_isp_load_register_w1r = 0;
	    }
	  }

	  //Enable the engine by turning on the register

	  //Fire Loading by setting write-one-clear
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
	  //handle->mload_cfg->reg_isp_miu2sram_en = 1;
      MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
      uVal |= (MASK_LOAD_ST_WLR & SET_LOAD_ST_WLR(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_ST_WLR);
	  //handle->mload_cfg->reg_isp_load_st_w1r = 1;

	#if ML_DOUBLE_BUFFER
	    mload_mem_cfg->uBufferIdx = (mload_mem_cfg->uBufferIdx+1)%2;
	#endif

	  MLOAD_DEBUG("j\n");
	  return SUCCESS;
}


int ISP_MLoadApplyIspImgDomain(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode){

      int i, k;
      u32 u4OffsetIdx = 0;
      u16 uVal;
      unsigned long tbl_addr;

      //Enable the engine by turning on the register
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
      //handle->mload_cfg->reg_isp_miu2sram_en = 1;
      //Set SW reset as 0 and 1
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal &= ~(MASK_SW_RST & SET_SW_RST(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);
      //handle->mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
      //usleep(50);
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_SW_RST & SET_SW_RST(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);
      //handle->mload_cfg->reg_isp_load_sw_rst = 1;
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_WAIT_HW_RDY_EN & SET_WAIT_HW_RDY_EN(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_WAIT_HW_RDY_EN);
      //handle->mload_cfg->reg_isp_load_wait_hardware_ready_en = 1;
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_LOAD_WATER_LEVEL & SET_LOAD_WATER_LEVEL(0));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_LOAD_WATER_LEVEL);
      //handle->mload_cfg->reg_isp_load_water_level = 0;
      //Set SW read write mode as 1 (write)
      for(i = MLOAD_ID_DPC; i<=MLOAD_ID_GMA10TO10; ++i)
      {
        u4OffsetIdx = i;

        if (mload_mem_cfg->bDirty[u4OffsetIdx] == false) {
          continue;
        } else
          mload_mem_cfg->bDirty[u4OffsetIdx] = false;

        tbl_addr = MLOAD_BUF_PHYS(mload_mem_cfg,u4OffsetIdx);
		MLOAD_DEBUG("=== tbl_addr2=0x%x, wp:0x%x length:0x%x\n",(int)tbl_addr, mload_mem_cfg->u32RingBufWp,mload_mem_cfg->length);

        switch(mload_mem_cfg->tblSramWidth[u4OffsetIdx])
        {
            case 2:
                MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
                uVal |= (MASK_SRAM_WIDTH & 0 );
                MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
                //handle->mload_cfg->reg_isp_sram_width  = 0;
                break;
            case 4:
                MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
                uVal |= (MASK_SRAM_WIDTH & 1 );
                MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
                //handle->mload_cfg->reg_isp_sram_width  = 1;
                break;
            case 8:
                MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
                uVal |= (MASK_SRAM_WIDTH & 2);
                MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
                //handle->mload_cfg->reg_isp_sram_width  = 2;
                break;
            default:
                MLOAD_DEBUG("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
                break;
        }

        MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
        uVal |= (MASK_SRAM_RW & SET_SRAM_RW(1));
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_RW);
        //handle->mload_cfg->reg_isp_sram_rw     = 1;// 1:write
        //Set SRAM ID
        MloadRegR(mode,MLOAD_BANK,SRAM_ID,&uVal);
        uVal |= (MASK_LOAD_SRAM_ID & u4OffsetIdx);
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_ID,uVal,MASK_LOAD_SRAM_ID);
        //handle->mload_cfg->reg_isp_load_sram_id = u4OffsetIdx;
        //Set MIU 16-byte start address
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_LOAD_ADDR,(HalUtilPHY2MIUAddr(tbl_addr)>>4) & 0xFFFF,0xFFFF);
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_LOAD_ADDR_1,(HalUtilPHY2MIUAddr(tbl_addr)>>20) & 0xFFFF,0xFFFF);
        //handle->mload_cfg->reg_isp_load_st_addr = (HalUtilPHY2MIUAddr(tbl_addr)>>4) & 0xFFFF;
        //handle->mload_cfg->reg_isp_load_st_addr_1 = (HalUtilPHY2MIUAddr(tbl_addr)>>20) & 0xFFFF;
        //Set data amount (2-byte)
        //The number should be set as length - 1
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,LOAD_AMOUNT,(mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1,0xFFFF);
        //handle->mload_cfg->reg_isp_load_amount    = (mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1;
        //Set destination SRAM start SRAM address (2-byte)
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_ST_ADDR, 0x0000,0xFFFF);
        //handle->mload_cfg->reg_isp_sram_st_addr   = 0x0000;
        //Set SRAM loading parameter by setting write-one-clear
        MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
        uVal |= (MASK_LOAD_REGISTER_WLR & 1);
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);
        //handle->mload_cfg->reg_isp_load_register_w1r = 1;

        //write 0, HW is not working, same as dummy command
        for(k = 0; k < 32; k++) {
            MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
            uVal |= (MASK_LOAD_REGISTER_WLR & 0);
            MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);
            //handle->mload_cfg->reg_isp_load_register_w1r = 0;
        }
      }

      //Enable the engine by turning on the register

      //Fire Loading by setting write-one-clear
      MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
      uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
      //handle->mload_cfg->reg_isp_miu2sram_en = 1;
      MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
      uVal |= (MASK_LOAD_ST_WLR & SET_LOAD_ST_WLR(1));
      MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_ST_WLR);
      //handle->mload_cfg->reg_isp_load_st_w1r = 1;

    #if ML_DOUBLE_BUFFER
        mload_mem_cfg->uBufferIdx = (mload_mem_cfg->uBufferIdx+1)%2;
    #endif

        ISP_MLoadSwitchClk(pCmqInterface_t,mode,0);

      return SUCCESS;
}

int SCL_MLoadApply(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode)
{

    int i, k;
    u32 u4OffsetIdx = 0;
    u16 uVal;
    unsigned long tbl_addr;
  //ISP_DMSG("[%s] MLoad Update Start\n",__FUNCTION__);
  //print_kmsg("mload+");
  //pthread_mutex_lock(&handle->MutexMLoadDone);
  //Enable the engine by turning on the register
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
  //Set SW reset as 0 and 1
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal &= ~(MASK_SW_RST & SET_SW_RST(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);
  //usleep(50);
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SW_RST & SET_SW_RST(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);

    //MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    //uVal |= (MASK_WAIT_HW_RDY_EN & SET_WAIT_HW_RDY_EN(1));
    //MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_WAIT_HW_RDY_EN);

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_LOAD_WATER_LEVEL & SET_LOAD_WATER_LEVEL(0));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_LOAD_WATER_LEVEL);
  //Set SW read write mode as 1 (write)
  for(i = SCL_MLOAD_ID_BASE; i < SCL_MLOAD_ID_NUM; ++i)
  {
    u4OffsetIdx = i;

    if (mload_mem_cfg->bDirty[u4OffsetIdx] == false) {
      continue;
    } else
      mload_mem_cfg->bDirty[u4OffsetIdx] = false;


    tbl_addr = MLOAD_BUF_PHYS(mload_mem_cfg,u4OffsetIdx);
	MLOAD_DEBUG("=== tbl_addr3=0x%x, wp:0x%x length:0x%x\n",(int)tbl_addr, mload_mem_cfg->u32RingBufWp,mload_mem_cfg->length);


//    ISP_VMSG("[%s] BufIdx:%d, ID:%d idx:%d, table addr:0x%08x, size:%d\n", __FUNCTION__,
//            mload_mem_cfg->uBufferIdx,
//        i, u4OffsetIdx, tbl_addr, mload_mem_cfg->tblRealSize[u4OffsetIdx]);

    //printk(KERN_INFO "[ISP] MLoad[%d] load_done: %d\n", i, isp_miu->reg_isp_load_done);
    switch(mload_mem_cfg->tblSramWidth[u4OffsetIdx])
    {
        case 2:
            MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 0 );
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        case 4:
            MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 1 );
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        case 8:
            MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 2);
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        default:
            MLOAD_DEBUG("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
            break;
    }

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SRAM_RW & SET_SRAM_RW(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_RW);
    //Set SRAM ID
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_ID,&uVal);
    uVal |= (MASK_LOAD_SRAM_ID & (u4OffsetIdx - SCL_MLOAD_ID_BASE));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_ID,uVal,MASK_LOAD_SRAM_ID);
    //Set MIU 16-byte start address
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_LOAD_ADDR,(HalUtilPHY2MIUAddr(tbl_addr)>>4) & 0xFFFF,0xFFFF);
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_LOAD_ADDR_1,(HalUtilPHY2MIUAddr(tbl_addr)>>20) & 0xFFFF,0xFFFF);
    //Set data amount (2-byte)
    //The number should be set as length - 1
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,LOAD_AMOUNT,(mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1,0xFFFF);
    //Set destination SRAM start SRAM address (2-byte)
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_ST_ADDR, 0x0000,0xFFFF);
    //Set SRAM loading parameter by setting write-one-clear
    MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
    uVal |= (MASK_LOAD_REGISTER_WLR & 1);
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);

    //write 0, HW is not working, same as dummy command
    for(k = 0; k < 32; k++) {
        MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
        uVal |= (MASK_LOAD_REGISTER_WLR & 0);
        MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_REGISTER_WLR);
    }
  }

  //Enable the engine by turning on the register
  MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
  uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
  MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
  //Fire Loading by setting write-one-clear
  MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
  uVal |= (MASK_LOAD_ST_WLR & SET_LOAD_ST_WLR(1));
  MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_LOAD_ST_WLR);

#if ML_DOUBLE_BUFFER
    mload_mem_cfg->uBufferIdx = (mload_mem_cfg->uBufferIdx+1)%2;
#endif

  //pthread_mutex_unlock(&handle->MutexMLoadDone);
  //print_kmsg("mload-");

  return SUCCESS;

}

int ISP_MLoadRead(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode,int id,int sram_offset,ISP_MLOAD_OUTPUT *output){

    int k;
    u32 u4OffsetIdx = 0;
    u16 uVal;

    MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal &= ~(MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(0));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
    //Set SW reset as 0 and 1
    MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SW_RST & SET_SW_RST(0));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST); //this register is active low, set 0 to reset
    //usleep(50);
    MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SW_RST & SET_SW_RST(1));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);

    MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal &= ~(MASK_SRAM_RW & SET_SRAM_RW(1));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_RW);// 0:read
    //Set SRAM ID
    MloadRegR(mode,MLOAD_BANK,SRAM_ID,&uVal);
    uVal |= (MASK_SRAM_READ_ID & SET_SRAME_READ_ID(id));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_ID,uVal,MASK_SRAM_READ_ID);


    switch(mload_mem_cfg->tblSramWidth[id])
    {
        case 2:
            MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 0 );
            MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        case 4:
            MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 1 );
            MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
       case 8:
             MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 2);
            MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        default:
            MLOAD_DEBUG("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
            break;
    }

    //Set destination read SRAM address (2-byte)
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_RD_ADDR, sram_offset,0xFFFF);

    MloadRegR(mode,MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);

    MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
    uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(1));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);

    MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
    uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(1));
    MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);

    for(k = 0; k < 32; k++) {
        MloadRegR(mode,MLOAD_BANK,FIRE_WLR,&uVal);
        uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(0));
        MloadRegW(pCmqInterface_t,mode,MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);
    }

    MloadRegR(mode,MLOAD_BANK,SRAM_READ_DATA_0,&(output->mload_rdata[0]));

    //MLOAD_DEBUG("~5 output->mload_rdata[0]=0x%x\n",output->mload_rdata[0]);
    MloadRegR(mode,MLOAD_BANK,SRAM_READ_DATA_1,&(output->mload_rdata[1]));
    //MLOAD_DEBUG("~6\n output->mload_rdata[1]=0x%x",output->mload_rdata[1]);
    MloadRegR(mode,MLOAD_BANK,SRAM_READ_DATA_2,&(output->mload_rdata[2]));

    //MLOAD_DEBUG("0x%x,\n", handle->mload_cfg->reg_isp_sram_read_data);
    //MLOAD_DEBUG("~7 output->mload_rdata[2]=0x%x\n",output->mload_rdata[2]);
    return SUCCESS;
}


int SCL_MLoadRead(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode,int id,int sram_offset,ISP_MLOAD_OUTPUT *output){

    int k;
    u32 u4OffsetIdx = 0;
    u16 uVal;

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal &= ~(MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(0));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);
    //Set SW reset as 0 and 1
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SW_RST & SET_SW_RST(0));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST); //this register is active low, set 0 to reset
    //usleep(50);
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_SW_RST & SET_SW_RST(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SW_RST);

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal &= ~(MASK_SRAM_RW & SET_SRAM_RW(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_RW);// 0:read
    //Set SRAM ID
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_ID,&uVal);
    uVal |= (MASK_LOAD_SRAM_ID & (id - SCL_MLOAD_ID_BASE));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_ID,uVal,MASK_LOAD_SRAM_ID);

    switch(mload_mem_cfg->tblSramWidth[id])
    {
        case 2:
            MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 0 );
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        case 4:
            MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 1 );
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
       case 8:
             MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
            uVal |= (MASK_SRAM_WIDTH & 2);
            MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_SRAM_WIDTH);
            break;
        default:
            MLOAD_DEBUG("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
            break;
    }

    //Set destination read SRAM address (2-byte)
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_RD_ADDR, sram_offset,0xFFFF);

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_CTRL,&uVal);
    uVal |= (MASK_MIU2SRAM_EN & SET_MIU2SRAM_EN(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,SRAM_CTRL,uVal,MASK_MIU2SRAM_EN);

    MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
    uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);

    MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
    uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(1));
    MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);

    for(k = 0; k < 32; k++) {
        MloadRegR(mode,SCL_MLOAD_BANK,FIRE_WLR,&uVal);
        uVal |= (MASK_SRAM_READ_WLR & SET_SRAM_READ_WLR(0));
        MloadRegW(pCmqInterface_t,mode,SCL_MLOAD_BANK,FIRE_WLR,uVal,MASK_SRAM_READ_WLR);
    }

    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_READ_DATA_0,&(output->mload_rdata[0]));
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_READ_DATA_1,&(output->mload_rdata[1]));
    MloadRegR(mode,SCL_MLOAD_BANK,SRAM_READ_DATA_2,&(output->mload_rdata[2]));

    //MLOAD_DEBUG("0x%x,\n", handle->mload_cfg->reg_isp_sram_read_data);

    return SUCCESS;
}

u32 ISP_WaitMloadDone(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 timeout){

    u32 i=0;
    u16 uVal=0;


    if(mode == RIU_MODE){

        for(i=0; i<timeout; i++){

            MloadRegR(mode,ISP0_BANK,MLOAD_INTR_STATUS,&uVal);    //polling mload done intr

            if( ((uVal & MASK_MLOAD_DONE) & (BIT_MLOAD_DONE(1)) )!= 0){

                MloadRegR(mode,ISP0_BANK,MLOAD_INTR_CLR,&uVal);        // clear intr
                uVal |=(MASK_MLOAD_DONE & BIT_MLOAD_DONE(1));
                MloadRegW(pCmqInterface_t,mode,ISP0_BANK,MLOAD_INTR_CLR,uVal,MASK_MLOAD_DONE);


                MLOAD_DEBUG("================== mload done ================\n");
                return SUCCESS;

            }

        }

    }else{

        MloadPollRegBit(pCmqInterface_t,ISP0_BANK,MLOAD_INTR_STATUS,BIT_MLOAD_DONE(1),MASK_MLOAD_DONE,1);

		MloadRegR(mode,ISP0_BANK,MLOAD_INTR_CLR,&uVal);        // clear intr
        uVal |=(MASK_MLOAD_DONE & BIT_MLOAD_DONE(1));
		MloadRegW(pCmqInterface_t,mode,ISP0_BANK,MLOAD_INTR_CLR,uVal,MASK_MLOAD_DONE);

		MloadRegR(mode,ISP0_BANK,MLOAD_INTR_CLR,&uVal);        // clear intr
        uVal &=~(MASK_MLOAD_DONE & BIT_MLOAD_DONE(1));
		MloadRegW(pCmqInterface_t,mode,ISP0_BANK,MLOAD_INTR_CLR,uVal,MASK_MLOAD_DONE);

        MLOAD_DEBUG("================== mload done ================\n");
        return SUCCESS;
    }

    return FAIL;


}

u32 SCL_WaitMloadDone(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 timeout){

    u32 i=0;
    u16 uVal=0;


    if(mode == RIU_MODE){

        for(i=0; i<timeout; i++){

            MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_STATUS,&uVal);    //polling mload done intr

            if( ((uVal & MASK_SCL_MLOAD_DONE) & (BIT_SCL_MLOAD_DONE(1)) )!= 0){

                MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,&uVal);        // clear intr
                uVal |=(MASK_SCL_MLOAD_DONE & BIT_SCL_MLOAD_DONE(1));
                MloadRegW(pCmqInterface_t,mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,uVal,MASK_SCL_MLOAD_DONE);


                MLOAD_DEBUG("==================scl mload done ================\n");
                return SUCCESS;

            }

        }

    }else{

        MloadPollRegBit(pCmqInterface_t,SCL_INTR_BANK,SCL_MLOAD_INTR_STATUS,BIT_SCL_MLOAD_DONE(1),MASK_SCL_MLOAD_DONE,1);

	    MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,&uVal);        // set 1 to clear intr
        uVal |=(MASK_SCL_MLOAD_DONE & BIT_SCL_MLOAD_DONE(1));
		MloadRegW(pCmqInterface_t,mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,uVal,MASK_SCL_MLOAD_DONE);

		MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,&uVal);        // set 0 to recover intr
        uVal &=~(MASK_SCL_MLOAD_DONE & BIT_SCL_MLOAD_DONE(1));
		MloadRegW(pCmqInterface_t,mode,SCL_INTR_BANK,SCL_MLOAD_INTR_CLR,uVal,MASK_SCL_MLOAD_DONE);

        MLOAD_DEBUG("==================scl mload done ================\n");
        return SUCCESS;
    }

    return FAIL;


}



u32 ISP_EnableMloadIntr(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable){

    u16 uVal=0;

    if(enable){

        MloadRegR(mode,ISP0_BANK,MLOAD_INTR_MASK,&uVal);          //disable mask mload done intr
        uVal &=~(MASK_MLOAD_DONE & BIT_MLOAD_DONE(1));
        MloadRegW(pCmqInterface_t,mode,ISP0_BANK,MLOAD_INTR_MASK,uVal,MASK_MLOAD_DONE);
    }else{

        MloadRegR(mode,ISP0_BANK,MLOAD_INTR_MASK,&uVal);          //enable mask mload done intr
        uVal |= (MASK_MLOAD_DONE & BIT_MLOAD_DONE(1));
        MloadRegW(pCmqInterface_t,mode,ISP0_BANK,MLOAD_INTR_MASK,uVal,MASK_MLOAD_DONE);

    }
    return SUCCESS;
}

u32 SCL_EnableMloadIntr(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable){

    u16 uVal=0;

    if(enable){

        MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_MASK,&uVal);          //disable mask mload done intr
        uVal &=~(MASK_SCL_MLOAD_DONE & BIT_SCL_MLOAD_DONE(1));
        MloadRegW(pCmqInterface_t,mode,SCL_INTR_BANK,SCL_MLOAD_INTR_MASK,uVal,MASK_SCL_MLOAD_DONE);
    }else{

        MloadRegR(mode,SCL_INTR_BANK,SCL_MLOAD_INTR_MASK,&uVal);          //enable mask mload done intr
        uVal |= (MASK_SCL_MLOAD_DONE & BIT_SCL_MLOAD_DONE(1));
        MloadRegW(pCmqInterface_t,mode,SCL_INTR_BANK,SCL_MLOAD_INTR_MASK,uVal,MASK_SCL_MLOAD_DONE);

    }
    return SUCCESS;
}
