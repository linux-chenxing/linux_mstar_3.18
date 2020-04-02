#include <hal_isp.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "hal_isp_private.h"
#include <drv_ms_isp_general.h>
//#include <sys_MsWrapper_cus_os_mem.h>
//#include <cpu_mem_map.hc>
#include <hal_clkpad.h>
//#include "hal_drv_util.h"
//#include <isp_sys_api.h>
#include <isp_log.h>
#include <isp_sys_api.h>
#include <cam_os_wrapper.h>
#include <cpu_mem_map_infinity2.h>
#include <infinity2_reg.h>

//#define SUCCESS 0
//#define FAIL 1
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


#define ISP_DBG 1

#ifdef ISP_DBG
#if ISP_DBG == 1
#define ISP_DMSG(args...) pr_debug(args)
#define ISP_EMSG(args...) pr_err(args)
#define ISP_VMSG(args...) pr_info(args)
#elif ISP_DBG == 0
#define ISP_DMSG(args...) do{}while(0)
#define ISP_EMSG(args...) UartSendTrace(args)
#define ISP_VMSG(args...) UartSendTrace(args)
#elif ISP_DBG == 2
#define ISP_DMSG(args...) do{}while(0)
#define ISP_EMSG(args...) do{}while(0) //LOGE(args)
#define ISP_VMSG(args...) do{}while(0)
#endif
#else
#define ISP_DMSG(args...) do{}while(0)
#define ISP_EMSG(args...) do{}while(0)
#define ISP_VMSG(args...) do{}while(0)
#endif

#define ISP_LMSG(cond, args...) do { if (handle->eDbgLevel & (cond)) fprintf(stderr, args);} while (0)

#define DPRCD   ISP_VMSG("%s is deprecated!\n", __FUNCTION__)
#define TODO    ISP_VMSG("%s:%d need to be implemented/checked!\n", __FUNCTION__, __LINE__)

#ifdef DEBUG
#  define  ASSERT(cond,fmt,...)  ({ if (!(cond)) { fprintf(stderr, fmt, __VA_ARGS__); assert(cond); } })
#else
#  define  ASSERT(cond,fmt,...)  ((void)0)
#endif

#define SHOWTIME()  isp_show_time(__FUNCTION__, __LINE__)
// infinity ToDo

//#define min(a,b)    (((a) >= (b))?(b):(a))
//#define max(a,b)    (((a) >= (b))?(a):(b))
#define minmax(v,a,b)       (((v)<(a))? (a) : ((v)>(b)) ? (b) : (v))
#define sign(a)     (((a)>0)?(1):(-1))

#define _ISP_HAL(h) ((isp_handle_t*)h)

#define WriteRegs_GEV2 do{}while(0)
#define WriteRegs do{}while(0)

int ISP_AE_SetWin(ISP_HAL_HANDLE hnd, ISP_AE_WIN win);
int ISP_AE_SetIntBlockRowNum(ISP_HAL_HANDLE hnd, volatile u32 u4Num);

////////////////////////////////////////////////////
//////////// MIU ///////////////////////////////////
////////////////////////////////////////////////////
int ISP_MloadDone(ISP_HAL_HANDLE hnd)
{
  isp_hal_handle *handle = _ISP_HAL(hnd);
  //return handle->miu->reg_isp_load_done;
  return handle->mload_cfg->reg_isp_load_done;
}

int ISP_WaitMloadDone(ISP_HAL_HANDLE hnd, long us_count)
{
  isp_hal_handle *handle = _ISP_HAL(hnd);

  int i;
  u8 time_out = 1;

  for(i = 0; i < us_count; i++) {
    if(ISP_MloadDone(hnd)) {
      time_out = 0;
      break;
    }
    //usleep(1);
  }

  if (time_out) {
    //ISP_EMSG("[%s] Wait %l us, TimeOut: %u, Done:%u, Status: %u, full:%u\n", __FUNCTION__,
    //    us_count, (u32)time_out,
    //    ISP_MloadDone(hnd),
    //    handle->mload_cfg->reg_isp_load_done,
    //    handle->mload_cfg->reg_isp_load_register_non_full);
    return FAIL;
  } else {
	handle->mload_cfg->reg_isp_miu2sram_en = 0;
    return SUCCESS;
  }
}

int ISP_MLoadTableSet(ISP_HAL_HANDLE hnd, IspMloadID_e idx, const u16 *table, MLOAD_IQ_MEM *mload_mem_cfg)
{
  void *address;
  //pthread_mutex_lock(&handle->MutexMLoadDone);
  address = MLOAD_BUF_VIRT(mload_mem_cfg,idx);

  memcpy(address, (void*)table, MLOAD_TBL_SIZE_B(mload_mem_cfg,idx));
  mload_mem_cfg->bDirty[idx] = true;
  //pthread_mutex_unlock(&handle->MutexMLoadDone);
  return SUCCESS;
}

int ISP_MLoadTableGet(ISP_HAL_HANDLE hnd, IspMloadID_e idx, u16 *table, MLOAD_IQ_MEM *mload_mem_cfg)
{
  void *address;

  if (!table) return FAIL;
  address = MLOAD_BUF_VIRT(mload_mem_cfg,idx);
  memcpy((void*)table, address, MLOAD_TBL_SIZE_B(mload_mem_cfg,idx));
  return SUCCESS;
}

/////////// Enable kernel using MLoad update IQ Table to SRAM
int ISP_MLoadApply(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg)
{
  isp_hal_handle *handle = _ISP_HAL(hnd);
  int i, k;
  u32 u4OffsetIdx = 0;
  unsigned long tbl_addr;
  //ISP_DMSG("[%s] MLoad Update Start\n",__FUNCTION__);
  //print_kmsg("mload+");
  //pthread_mutex_lock(&handle->MutexMLoadDone);
  //Enable the engine by turning on the register
  handle->mload_cfg->reg_isp_miu2sram_en = 1;
  //Set SW reset as 0 and 1
  handle->mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
  //usleep(50);
  handle->mload_cfg->reg_isp_load_sw_rst = 1;

  handle->mload_cfg->reg_isp_load_wait_hardware_ready_en = 1;
  handle->mload_cfg->reg_isp_load_water_level = 0;
  //Set SW read write mode as 1 (write)
  for(i = 0; i<MLOAD_ID_NUM; ++i)
  {
    u4OffsetIdx = i;

    if (mload_mem_cfg->bDirty[u4OffsetIdx] == false) {
      continue;
    } else
      mload_mem_cfg->bDirty[u4OffsetIdx] = false;

#ifdef MLOAD_DBG // MLoad debug
    if (u4Counter % 10 >= 5) {
      if (i >= eMLOAD_ID_LN_GMA10TO12_R && i <= eMLOAD_ID_LN_GMA10TO12_B)
        u4OffsetIdx += 6;
    }
#endif

    tbl_addr = MLOAD_BUF_PHYS(mload_mem_cfg,u4OffsetIdx);
    switch(mload_mem_cfg->tblSramWidth[u4OffsetIdx])
    {
        case 2:
            handle->mload_cfg->reg_isp_sram_width  = 0;
            break;
        case 4:
            handle->mload_cfg->reg_isp_sram_width  = 1;
            break;
        case 8:
            handle->mload_cfg->reg_isp_sram_width  = 2;
            break;
        default:
            pr_info("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
            break;
    }

    handle->mload_cfg->reg_isp_sram_rw     = 1;// 1:write
    //Set SRAM ID
    handle->mload_cfg->reg_isp_load_sram_id = u4OffsetIdx;
    //Set MIU 16-byte start address
    handle->mload_cfg->reg_isp_load_st_addr = ((u32)CamOsDirectMemPhysToMiu((void*)tbl_addr)>>4) & 0xFFFF;
    handle->mload_cfg->reg_isp_load_st_addr_1 = ((u32)CamOsDirectMemPhysToMiu((void*)tbl_addr)>>20) & 0xFFFF;
    //Set data amount (2-byte)
    //The number should be set as length - 1
    handle->mload_cfg->reg_isp_load_amount    = (mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1;
    //Set destination SRAM start SRAM address (2-byte)
    handle->mload_cfg->reg_isp_sram_st_addr   = 0x0000;
    //Set SRAM loading parameter by setting write-one-clear
    handle->mload_cfg->reg_isp_load_register_w1r = 1;

    //write 0, HW is not working, same as dummy command
    for(k = 0; k < 32; k++) {
        handle->mload_cfg->reg_isp_load_register_w1r = 0;
    }
  }

  //Enable the engine by turning on the register

  //Fire Loading by setting write-one-clear
  //handle->mload_cfg->reg_isp_load_st_w1r = 1;
  handle->mload_cfg->reg_isp_miu2sram_en = 1;
  handle->mload_cfg->reg_isp_load_st_w1r = 1;

//#if ML_DOUBLE_BUFFER
//    mload_mem_cfg->uBufferIdx = (mload_mem_cfg->uBufferIdx+1)%2;
//#endif

  //pthread_mutex_unlock(&handle->MutexMLoadDone);
  //print_kmsg("mload-");

  return SUCCESS;

}

int SCL_MLoadApply(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg)
{
  isp_hal_handle *handle = _ISP_HAL(hnd);
  int i, k;
  u32 u4OffsetIdx = 0;
  unsigned long tbl_addr;
  //ISP_DMSG("[%s] MLoad Update Start\n",__FUNCTION__);
  //print_kmsg("mload+");
  //pthread_mutex_lock(&handle->MutexMLoadDone);
  //Enable the engine by turning on the register
  handle->scl_mload_cfg->reg_isp_miu2sram_en = 1;
  //Set SW reset as 0 and 1
  handle->scl_mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
  //usleep(50);
  handle->scl_mload_cfg->reg_isp_load_sw_rst = 1;

  handle->scl_mload_cfg->reg_isp_load_wait_hardware_ready_en = 0;
  handle->scl_mload_cfg->reg_isp_load_water_level = 0;
  //Set SW read write mode as 1 (write)
  for(i = SCL_MLOAD_ID_BASE; i < SCL_MLOAD_ID_NUM; ++i)
  {
    u4OffsetIdx = i;

    if (mload_mem_cfg->bDirty[u4OffsetIdx] == false) {
      continue;
    } else
      mload_mem_cfg->bDirty[u4OffsetIdx] = false;

#ifdef MLOAD_DBG // MLoad debug
    if (u4Counter % 10 >= 5) {
      if (i >= eMLOAD_ID_LN_GMA10TO12_R && i <= eMLOAD_ID_LN_GMA10TO12_B)
        u4OffsetIdx += 6;
    }
#endif

    tbl_addr = MLOAD_BUF_PHYS(mload_mem_cfg,u4OffsetIdx);

//    ISP_VMSG("[%s] BufIdx:%d, ID:%d idx:%d, table addr:0x%08x, size:%d\n", __FUNCTION__,
//        handle->mload_mem_cfg.uBufferIdx,
//        i, u4OffsetIdx, tbl_addr, handle->mload_mem_cfg.tblRealSize[u4OffsetIdx]);

    //printk(KERN_INFO "[ISP] MLoad[%d] load_done: %d\n", i, isp_miu->reg_isp_load_done);
    switch(mload_mem_cfg->tblSramWidth[u4OffsetIdx])
    {
        case 2:
            handle->scl_mload_cfg->reg_isp_sram_width  = 0;
            break;
        case 4:
            handle->scl_mload_cfg->reg_isp_sram_width  = 1;
            break;
        case 8:
            handle->scl_mload_cfg->reg_isp_sram_width  = 2;
            break;
        default:
            pr_info("ERROR : Mload invalid SRAM width,%d\n",mload_mem_cfg->tblSramWidth[u4OffsetIdx]);
            break;
    }

    handle->scl_mload_cfg->reg_isp_sram_rw     = 1;// 1:write
    //Set SRAM ID
    handle->scl_mload_cfg->reg_isp_load_sram_id = u4OffsetIdx - SCL_MLOAD_ID_BASE; //offset of SCL Mload ID
    //Set MIU 16-byte start address
    handle->scl_mload_cfg->reg_isp_load_st_addr = ((u32)CamOsDirectMemPhysToMiu((void*)tbl_addr)>>4) & 0xFFFF;
    handle->scl_mload_cfg->reg_isp_load_st_addr_1 = ((u32)CamOsDirectMemPhysToMiu((void*)tbl_addr)>>20) & 0xFFFF;
    //Set data amount (2-byte)
    //The number should be set as length - 1
    handle->scl_mload_cfg->reg_isp_load_amount    = (mload_mem_cfg->tblRealSize[u4OffsetIdx] / mload_mem_cfg->tblSramWidth[u4OffsetIdx])-1;
    //Set destination SRAM start SRAM address (2-byte)
    handle->scl_mload_cfg->reg_isp_sram_st_addr   = 0x0000;
    //Set SRAM loading parameter by setting write-one-clear
    handle->scl_mload_cfg->reg_isp_load_register_w1r = 1;

    //write 0, HW is not working, same as dummy command
    for(k = 0; k < 32; k++) {
        handle->scl_mload_cfg->reg_isp_load_register_w1r = 0;
    }
  }

  //Enable the engine by turning on the register
  handle->scl_mload_cfg->reg_isp_miu2sram_en = 1;
  //Fire Loading by setting write-one-clear
  //handle->scl_mload_cfg->reg_isp_load_st_w1r = 1; //move to CmdQ to trigger

//#if ML_DOUBLE_BUFFER
//    mload_mem_cfg->uBufferIdx = (mload_mem_cfg->uBufferIdx+1)%2;
//#endif

  //pthread_mutex_unlock(&handle->MutexMLoadDone);
  //print_kmsg("mload-");

  return SUCCESS;

}

int ISP_MLoadRead(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg,int id,int sram_offset,ISP_MLOAD_OUTPUT *output){

    isp_hal_handle *handle = _ISP_HAL(hnd);
    int k;
    //u32 u4OffsetIdx = 0;
    //unsigned long tbl_addr;

    handle->mload_cfg->reg_isp_miu2sram_en = 0;
    //Set SW reset as 0 and 1
    handle->mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
    //usleep(50);
    handle->mload_cfg->reg_isp_load_sw_rst = 1;

    handle->mload_cfg->reg_isp_sram_rw     = 0;// 0:read
    //Set SRAM ID
    handle->mload_cfg->reg_isp_sram_read_id = id;

    switch(mload_mem_cfg->tblSramWidth[id])
    {
        case 2:
            handle->mload_cfg->reg_isp_sram_width  = 0;
            break;
        case 4:
            handle->mload_cfg->reg_isp_sram_width  = 1;
            break;
        case 8:
            handle->mload_cfg->reg_isp_sram_width  = 2;
            break;
        default:
            pr_info("ERROR : Mload invalid SRAM width\n");
            break;
    }

    //Set destination read SRAM address (2-byte)
    handle->mload_cfg->reg_isp_sram_rd_addr   = sram_offset;//0x0000;

    handle->mload_cfg->reg_isp_miu2sram_en = 1;
    handle->mload_cfg->reg_isp_sram_read_w1r = 1;
    handle->mload_cfg->reg_isp_sram_read_w1r = 1;

    for(k = 0; k < 32; k++) {
        handle->mload_cfg->reg_isp_sram_read_w1r = 0;
    }


    output->mload_rdata[0] = handle->mload_cfg->reg_isp_sram_read_data;
    output->mload_rdata[1] = handle->mload_cfg->reg_isp_sram_read_data_1;
    output->mload_rdata[2] = handle->mload_cfg->reg_isp_sram_read_data_2;

    //UartSendTrace("0x%x,\n", handle->mload_cfg->reg_isp_sram_read_data);

    return SUCCESS;
}

int SCL_MLoadRead(ISP_HAL_HANDLE hnd, MLOAD_IQ_MEM *mload_mem_cfg,int id,int sram_offset,ISP_MLOAD_OUTPUT *output){

    isp_hal_handle *handle = _ISP_HAL(hnd);
    int k;
    //u32 u4OffsetIdx = 0;
    //unsigned long tbl_addr;

    handle->scl_mload_cfg->reg_isp_miu2sram_en = 0;
    //Set SW reset as 0 and 1
    handle->scl_mload_cfg->reg_isp_load_sw_rst = 0; //this register is active low, set 0 to reset
    //usleep(50);
    handle->scl_mload_cfg->reg_isp_load_sw_rst = 1;

    handle->scl_mload_cfg->reg_isp_sram_rw     = 0;// 0:read
    //Set SRAM ID
    handle->scl_mload_cfg->reg_isp_sram_read_id = id - SCL_MLOAD_ID_BASE;

    switch(mload_mem_cfg->tblSramWidth[id])
    {
        case 2:
            handle->scl_mload_cfg->reg_isp_sram_width  = 0;
            break;
        case 4:
            handle->scl_mload_cfg->reg_isp_sram_width  = 1;
            break;
        case 8:
            handle->scl_mload_cfg->reg_isp_sram_width  = 2;
            break;
        default:
            pr_info("ERROR : Mload invalid SRAM width\n");
            break;
    }

    //Set destination read SRAM address (2-byte)
    handle->scl_mload_cfg->reg_isp_sram_rd_addr   = sram_offset;//0x0000;

    handle->scl_mload_cfg->reg_isp_miu2sram_en = 1;
    handle->scl_mload_cfg->reg_isp_sram_read_w1r = 1;
    handle->scl_mload_cfg->reg_isp_sram_read_w1r = 1;

    for(k = 0; k < 32; k++) {
        handle->scl_mload_cfg->reg_isp_sram_read_w1r = 0;
    }


    output->mload_rdata[0] = handle->scl_mload_cfg->reg_isp_sram_read_data;
    output->mload_rdata[1] = handle->scl_mload_cfg->reg_isp_sram_read_data_1;
    output->mload_rdata[2] = handle->scl_mload_cfg->reg_isp_sram_read_data_2;

    //UartSendTrace("0x%x,\n", handle->mload_cfg->reg_isp_sram_read_data);

    return SUCCESS;
}

#if 0
// utility
long isp_show_time(const char *function, long int line)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    ISP_DMSG("--%lld--(us)[%s:%ld]\n", (unsigned long long)tp.tv_sec * 1000000 + (unsigned long long)tp.tv_usec, function, line);
    return ((unsigned long long) tp.tv_sec * 1000000 + (unsigned long long) tp.tv_usec);
}
#endif

//isp_hal_handle *handle = TO_ISP_HAL_HANDLE(hnd);
////////////////// DMA //////////////////////////////////////
static volatile infinity2_reg_isp_wdma* ISP_WDMABase(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    switch(id)
    {
    case ISP_DMA_PIPE0:
        return _ISP_HAL(hnd)->wdma0_cfg;
    break;
    case ISP_DMA_PIPE1:
        return _ISP_HAL(hnd)->wdma1_cfg;
    break;
    case ISP_DMA_PIPE2:
        return _ISP_HAL(hnd)->wdma2_cfg;
    break;
    case ISP_DMA_GENERAL:
        return _ISP_HAL(hnd)->wdma3_cfg;
    break;
    default:
        return 0;
    }
    return 0;
}

static volatile infinity2_reg_isp_rdma* ISP_RDMABase(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    switch(id)
    {
    case ISP_DMA_GENERAL:
    case ISP_DMA_PIPE0:
        return _ISP_HAL(hnd)->rdma0_cfg;
    break;
    case ISP_DMA_PIPE1:
        return _ISP_HAL(hnd)->rdma1_cfg;
    break;
    default:
        pr_info("ISP_DMA %d does not exist.\n",id);
        return 0;
    }
    return 0;
}
////////////////// RDMA config //////////////////////////////
int ISP_RDMAEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_en = enable;
    return SUCCESS;
}

int ISP_RDMAisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    return dma->reg_isp_rdma_en;
}

int ISP_RDMAAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_auto = enable;
    return SUCCESS;
}

//int ISP_RDMAisAuto(ISP_HAL_HANDLE hnd)
//{
//    return _ISP_HAL(hnd)->dma_cfg->rdma_cfg.reg_isp_rdma_auto;
//}

int ISP_RDMASwTrig(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_trigger = 1;
    return SUCCESS;
}

int ISP_RDMASetHighPri(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_rreq_hpri_set = enable;
    return SUCCESS;
}

int ISP_RDMAReqThd(ISP_HAL_HANDLE hnd,IspDmaId_e id,int thd)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_rreq_thrd = LIMIT_BITS(thd, 3);
    return SUCCESS;
}

int ISP_RDMAReqHighPriThd(ISP_HAL_HANDLE hnd,IspDmaId_e id,int thd)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_rreq_hpri = LIMIT_BITS(thd, 4);
    return SUCCESS;
}

int ISP_RDMAReqBustNum(ISP_HAL_HANDLE hnd,IspDmaId_e id,int num)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_rreq_max = LIMIT_BITS(num, 4);
    return SUCCESS;
}

int ISP_RDMAReqBlank(ISP_HAL_HANDLE hnd,IspDmaId_e id,int blank)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_tg_hblk = LIMIT_BITS(blank, 6);
    return SUCCESS;
}

int ISP_RDMABaseAddr(ISP_HAL_HANDLE hnd,IspDmaId_e id,unsigned int addr)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    addr >>= 4;
    if(addr != LIMIT_BITS(addr, 27))
        return FAIL;

    dma->reg_isp_rdma_base = addr & 0xffff;
    dma->reg_isp_rdma_base_1 = (addr >> 16) & 0x07ff;
    return SUCCESS;
}

MS_BOOL ISP_RDMAisBusy(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    return dma->reg_isp_rdma_eof_read;
}

int ISP_RDMAReset(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_rdma_rreq_rst = 1;
    _ISP_HAL(hnd)->isp0_cfg->reg_dbgwr_swrst = 0;
    usleep(10);
    dma->reg_isp_rdma_rreq_rst = 0;
    _ISP_HAL(hnd)->isp0_cfg->reg_dbgwr_swrst = 1;
    return SUCCESS;
}

int ISP_RDMADataPrecision(ISP_HAL_HANDLE hnd, IspDmaId_e id, ISP_DATAPRECISION prec)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    switch(prec)
    {
        case ISP_DATAPRECISION_8:
            dma->reg_isp_rdma_mode = 0;
            break;
        case ISP_DATAPRECISION_10:
            dma->reg_isp_rdma_mode = 1;
            break;
        case ISP_DATAPRECISION_16:
            dma->reg_isp_rdma_mode = 2;
            break;
        case ISP_DATAPRECISION_12:
            dma->reg_isp_rdma_mode = 3;
            break;
        default:
            break;
    }
    return SUCCESS;
}

int ISP_RDMASetPitch(ISP_HAL_HANDLE hnd, IspDmaId_e id, int width, int height, DATA_PITCH pitch)
{
    int denomt = 128;
    ISP_DATAPRECISION isp_data_prec;

    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    switch(pitch)
    {
        case PITCH_16BITS:
            denomt = 8;
            isp_data_prec = ISP_DATAPRECISION_16;
            dma->reg_isp_rdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 1), 12);
            break;
        case PITCH_12BITS:
            denomt = 32;
            isp_data_prec = ISP_DATAPRECISION_12;
            dma->reg_isp_rdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 3), 12);
            break;
        case PITCH_10BITS:
            denomt = 64;
            isp_data_prec = ISP_DATAPRECISION_10;
            dma->reg_isp_rdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 5), 12);
            break;
        case PITCH_8BITS:
            denomt = 16;
            isp_data_prec = ISP_DATAPRECISION_8;
            dma->reg_isp_rdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 1), 12);
            break;
        default:
            return FAIL;
    }

    ISP_RDMADataPrecision(hnd, id, isp_data_prec);
    dma->reg_isp_rdma_width_m1 = (width - 1);
    dma->reg_isp_rdma_height_m1 = height - 1;

    return SUCCESS;
}

///////////////// WDMA config ///////////////////////
int ISP_WDMAEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_en = enable;
    return SUCCESS;
}

int ISP_WDMAisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    return dma->reg_isp_wdma_en;
}

int ISP_WDMAAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_auto = enable;
    return SUCCESS;
}

int ISP_WDMAisAuto(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    return dma->reg_isp_wdma_auto;
}

int ISP_WDMASwTrig(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    /*
     nrdma triggerAwdma]trigger mode
     BK_ISP6_20[4] reg_isp_wdma_auto ]0
     BK_ISP6_30[0] reg_isp_wdma_trigger@obrdma triggere
    */
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_trigger_mode = 1;
    dma->reg_isp_wdma_trigger = 1;
    return SUCCESS;
}

int ISP_WDMASetHighPri(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_hpri_set = enable;
    return SUCCESS;
}

int ISP_WDMAReqThd(ISP_HAL_HANDLE hnd,IspDmaId_e id,int thd)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_thrd = LIMIT_BITS(thd, 3);
    return SUCCESS;
}

int ISP_WDMAReqHighPriThd(ISP_HAL_HANDLE hnd,IspDmaId_e id,int thd)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_hpri = LIMIT_BITS(thd, 4);
    return SUCCESS;
}

int ISP_WDMAReqBustNum(ISP_HAL_HANDLE hnd,IspDmaId_e id,int num)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_max = LIMIT_BITS(num, 4);
    return SUCCESS;
}

int ISP_WDMAAutoAlign(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_align_en = enable;
    return SUCCESS;
}

int ISP_WDMACheckLBFull(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL *full)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    *full = dma->reg_isp_wdma_lb_full_read;
    return SUCCESS;
}

int ISP_WDMAStatusClear(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_height_en = 0;
    dma->reg_isp_wdma_status_clr = 1;
    dma->reg_isp_wdma_height_en = 1;
    return SUCCESS;
}

int ISP_WDMABaseAddr(ISP_HAL_HANDLE hnd,IspDmaId_e id,unsigned int addr)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    //addr >>= 4;
    addr >>= 5; //32 byte aligned
    //if(addr != LIMIT_BITS(addr, 27))
    //    return FAIL;
    dma->reg_isp_wdma_base = (addr & 0xffff);
    dma->reg_isp_wdma_base_1 = (addr >> 16) & 0xffff;
    return SUCCESS;
}

MS_BOOL ISP_WDMAisBusy(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    return dma->reg_isp_wdma_eof_read;
}

int ISP_WDMADataPrecision(ISP_HAL_HANDLE hnd,IspDmaId_e id,ISP_DATAPRECISION prec)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    switch(prec)
    {
        case ISP_DATAPRECISION_8:
            dma->reg_isp_wdma_mode = 0;
            break;
        case ISP_DATAPRECISION_10:
        	dma->reg_isp_wdma_mode = 1;
            break;
        case ISP_DATAPRECISION_16:
            dma->reg_isp_wdma_mode = 2;
            break;
        case ISP_DATAPRECISION_12:
        	dma->reg_isp_wdma_mode = 3;
            break;
        case ISP_DATAPRECISION_1T4P_8:
            dma->reg_isp_wdma_mode = 4;
            break;
        case ISP_DATAPRECISION_1T4P_10:
            dma->reg_isp_wdma_mode = 5;
            break;
        case ISP_DATAPRECISION_1T4P_16:
            dma->reg_isp_wdma_mode = 6;
            break;
        case ISP_DATAPRECISION_1T4P_12:
            dma->reg_isp_wdma_mode = 7;
            break;
    }
    return SUCCESS;
}

///////////////////// DMA utiltiy function ///////////////////
int ISP_RingBuffEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_dma_ring_buf_en = enable;
    return SUCCESS;
}

int ISP_RingBuffisEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    return dma->reg_isp_dma_ring_buf_en;
}

int ISP_RingBuffSize(ISP_HAL_HANDLE hnd,IspDmaId_e id,int size)
{
    volatile  infinity2_reg_isp_rdma *dma = ISP_RDMABase(hnd,id);
    dma->reg_isp_dma_ring_buf_sz = LIMIT_BITS(size, 4);
    return SUCCESS;
}

int ISP_DMAClkForceActive(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_mreq_always_active = enable;
    return SUCCESS;
}

int ISP_DMAClkForceOff(ISP_HAL_HANDLE hnd, IspDmaId_e id,MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_mreq_force_off = enable;
    return SUCCESS;
}

int ISP_DMAClkWDMAMode(ISP_HAL_HANDLE hnd, IspDmaId_e id,MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_mreq_wdma_mode = enable;
    return SUCCESS;
}

#if 0
// should be called after setup isp/dma crop width, and mux source
int ISP_RDMASetPitch(ISP_HAL_HANDLE hnd, IspDmaId_e id, DATA_PITCH pitch)
{
    int nomt = 1, denomt = 128;
    int width = (_ISP_HAL(hnd)->isp0_cfg->reg_isp_crop_width + 1);
    infinity2_reg_isp_dmag *dma = ISP_DmaBase(hnd,id);
    SEN_DATATYPE type;

    switch(pitch)
    {
        case PITCH_16BITS:
            nomt = 16;
            dma->reg_isp_rdma_mode = 2;
            break;
        case PITCH_12BITS:
            nomt = 12;
            dma->reg_isp_rdma_mode = 3;
            break;
        case PITCH_10BITS:
            nomt = 10;
            dma->reg_isp_rdma_mode = 1;
            break;
        case PITCH_8BITS:
            nomt = 8;
            dma->reg_isp_rdma_mode = 0;
            break;
        default:
            return FAIL;
    }

    ISP_GetSenDataType(_ISP_HAL(hnd), ISP_PIPE_0/*TBD*/, &type);
    if(type == SEN_DATATYPE_YUV)
    {
        nomt *= 2;
    }
    dma->reg_isp_rdma_pitch = LIMIT_BITS((u32)ceil((double)width / denomt) * nomt, 12);
    return SUCCESS;
}
#endif

int ISP_WDMASetPitch(ISP_HAL_HANDLE hnd, IspDmaId_e id, int width, int height, DATA_PITCH pitch)
{
    int nomt = 1, denomt = 128, _1tnp = 1;
    ISP_DATAPRECISION isp_data_prec;

    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    switch(pitch)
    {
        case PITCH_16BITS:
            nomt = 16;
            _1tnp = 1;
            isp_data_prec = ISP_DATAPRECISION_16; //dma->reg_isp_wdma_mode = 2;
            //dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)/*ceil*/(width * nomt / denomt), 12);
            dma->reg_isp_wdma_pitch = (width+15)>>4; //16 data pack 1 miu data
            break;
        case PITCH_12BITS:
            nomt = 12;
            _1tnp = 1;
            isp_data_prec = ISP_DATAPRECISION_12; //dma->reg_isp_wdma_mode = 3;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)/*ceil*/(width * nomt / denomt), 12);
            break;
        case PITCH_10BITS:
            _1tnp = 1;
            nomt = 10;
            isp_data_prec = ISP_DATAPRECISION_10; //dma->reg_isp_wdma_mode = 1;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)/*ceil*/(width * nomt / denomt), 12);
            break;
        case PITCH_8BITS:
            nomt = 8;
            _1tnp = 1;
            isp_data_prec = ISP_DATAPRECISION_8; //dma->reg_isp_wdma_mode = 0;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)/*ceil*/(width * nomt / denomt), 12);
            break;
        case PITCH_1T4P_16BITS:
            denomt = 8;
            _1tnp = 4;
            isp_data_prec = ISP_DATAPRECISION_1T4P_16; //dma->reg_isp_wdma_mode = 6;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 1), 12);
            break;
        case PITCH_1T4P_12BITS:
            denomt = 32;
            _1tnp = 4;
            isp_data_prec = ISP_DATAPRECISION_1T4P_12; //dma->reg_isp_wdma_mode = 7;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 3), 12);
            break;
        case PITCH_1T4P_10BITS:
            denomt = 64;
            _1tnp = 4;
            isp_data_prec = ISP_DATAPRECISION_1T4P_10; //dma->reg_isp_wdma_mode = 5;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 5), 12);
            break;
        case PITCH_1T4P_8BITS:
            denomt = 16;
            _1tnp = 4;
            isp_data_prec = ISP_DATAPRECISION_1T4P_8; //dma->reg_isp_wdma_mode = 4;
            dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)(((width + denomt - 1) / denomt) * 1), 12);
            break;
        default:
            return FAIL;
    }

    ISP_WDMADataPrecision(hnd, id, isp_data_prec);
    //dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)ceil((double)width / denomt) * nomt, 12);
    //dma->reg_isp_wdma_pitch = LIMIT_BITS((u32)/*ceil*/(width * nomt / denomt), 12);
    dma->reg_isp_wdma_width_m1 = (width - 1) / _1tnp;
    dma->reg_isp_wdma_height_m1 = height - 1;

    return SUCCESS;
}

int ISP_WDMAReset(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_rst = 1;
    _ISP_HAL(hnd)->isp0_cfg->reg_dbgwr_swrst = 0;
    //usleep(100);
    dma->reg_isp_wdma_wreq_rst = 0;
    _ISP_HAL(hnd)->isp0_cfg->reg_dbgwr_swrst = 1;
    return SUCCESS;
}

int ISP_WDMASetLineCnt(ISP_HAL_HANDLE hnd,IspDmaId_e id, unsigned int iline_cnt)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);

    dma->reg_isp_wdma_irqlinex = LIMIT_BITS(iline_cnt, 13);
    return SUCCESS;
}

////////////// cfai parameters, isp_cfai_params//////////////////////

/////////// color matrix config, isp_ccm_cfg ////////////////

int ISP_IQ_CCM_Cfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG cfg)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    switch(cfg.eID)
    {
        case eCCM_RGB:
            pr_debug("I3 isp does not support RGB CCM.\n");
            break;
        case eCCM_YUV:
            handle->isp0_cfg->reg_isp_r2y_cmc_en            = cfg.bEnable;
            handle->isp0_cfg->reg_isp_r2y_y_sub_16_en       = cfg.by_sub_16_en;
            handle->isp0_cfg->reg_isp_r2y_r_sub_16_en       = cfg.br_sub_16_en;
            handle->isp0_cfg->reg_isp_r2y_b_sub_16_en       = cfg.bb_sub_16_en;
            handle->isp0_cfg->reg_isp_r2y_y_add_16_post_en  = cfg.by_add_16_post_en;
            handle->isp0_cfg->reg_isp_r2y_r_add_16_post_en  = cfg.br_add_16_post_en;
            handle->isp0_cfg->reg_isp_r2y_b_add_16_post_en  = cfg.bb_add_16_post_en;
            handle->isp0_cfg->reg_isp_r2y_cb_add_128_post_en = cfg.bcb_add_128_post_en;
            handle->isp0_cfg->reg_isp_r2y_cr_add_128_post_en = cfg.bcr_add_128_post_en;
            handle->isp0_cfg->reg_isp_r2y_rran              = cfg.urran;
            handle->isp0_cfg->reg_isp_r2y_gran              = cfg.ugran;
            handle->isp0_cfg->reg_isp_r2y_bran              = cfg.ubran;
            break;
    }

    return SUCCESS;
}

int ISP_SetCCMCfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG cfg)
{
    int i;

    _ISP_HAL(hnd)->isp0_cfg->reg_rgb2yuv_dith_en = cfg.bDitherEnable;
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_r2y_coeff_11 = 0x01;
#if 0
    unsigned int ctl_id;
    ctl_id = IOCTL_ISP_IQ_YUVCCM;
    if(handle->isp_dev)
    {
        isp_ioctl_ccm_coeff ctl;
        memcpy(ctl.ccm, cfg.i2CCM, sizeof(ctl.ccm));
        for(i = 0; i < 9; i++)
        {
            ctl.ccm[i] = cfg.i2CCM[i] & 0x1FFF;
        }
        int ret = ioctl(handle->isp_dev, ctl_id, &ctl);
    }
    else
#endif
    {
        //volatile isp_ccm_coeff *coeff = (isp_ccm_coeff*) (((char*)handle->isp0_cfg) + offset_of_reg_isp_r2y_coeff_11);
        volatile isp_ccm_coeff* coeff = (isp_ccm_coeff*) reg_addr(_ISP_HAL(hnd)->isp0_cfg, isp0_reg_isp_r2y_coeff_11);
        for(i = 0; i < 9; i++)
        {
            coeff[i].val = cfg.i2CCM[i] & 0x1FFF;
        }
    }
    return SUCCESS;
}

int ISP_GetCCMCfg(ISP_HAL_HANDLE hnd, ISP_CCM_CFG *cfg)
{
    int i;
    volatile isp_ccm_coeff* coeff;
    if(!cfg)
        return FAIL;

    //isp_ccm_coeff *coeff = (isp_ccm_coeff*) (((char*)handle->isp0_cfg) + offset_of_reg_isp_r2y_coeff_11);
    coeff = (volatile isp_ccm_coeff*) reg_addr(_ISP_HAL(hnd)->isp0_cfg, isp0_reg_isp_r2y_coeff_11);
    for(i = 0; i < 9; i++)
    {
        if(coeff[i].val & 0x0400)
            cfg->i2CCM[i] = coeff[i].val | 0xf800;
        else
            cfg->i2CCM[i] = coeff[i].val;
    }
    return SUCCESS;
}

/////////// gamma config, isp_gamma_cfg /////////////////////
int ISP_GammaCfg(ISP_HAL_HANDLE hnd, ISP_GAMMA_CFG cfg)
{
    switch(cfg.eID)
    {
        case eGAMMA_RAW_LC:
            pr_info("I2 isp does not support GAMMA_RGB_LC.\n");
            //_ISP_HAL(hnd)->isp1_cfg->reg_gm12to10_max_data = cfg.u4BoundMax;
            //_ISP_HAL(hnd)->isp1_cfg->reg_gm12to10_en       = cfg.bEnable;
            //_ISP_HAL(hnd)->isp1_cfg->reg_gm12to10_max_en   = cfg.bLimitMax;
            break;
        case eGAMMA_RGB_LC:
            pr_info("I3 isp does not support GAMMA_RGB_LC.\n");
            //handle->isp0_cfg->reg_isp_gamma_10to12_max_data = cfg.u4BoundMax;
            //handle->isp0_cfg->reg_isp_gamma_10to12_en       = cfg.bEnable;
            //handle->isp0_cfg->reg_isp_gamma_10to12_max_en   = cfg.bLimitMax;
            break;
        case eGAMMA_CCM:
            //handle->isp0_cfg->reg_isp_gamma_crct_max_data = cfg.u4BoundMax;
            //handle->isp0_cfg->reg_isp_gamma_crct_en       = cfg.bEnable;
            //handle->isp0_cfg->reg_isp_gamma_crct_max_en   = cfg.bLimitMax;
            _ISP_HAL(hnd)->isp0_cfg->reg_gamma_c_dith_en = cfg.bDither;
            break;
    }

    return SUCCESS;
}

/////////// edge config, isp_edge_cfg ///////////////////////
int ISP_EdgeEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_edge_enable = enable;
    return SUCCESS;
}

/////////////// crop config, isp_crop_cfg ////////////////////////
// data type -> mux -> crop -> pitch
int ISP_SetISPCrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN win)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    SEN_DATATYPE type;

    ISP_GetSenDataType(handle, isp_pipe, &type);

    if(type == SEN_DATATYPE_YUV)
    {
        win.x_start = (win.x_start) & 0xfffe; // even
        win.width = (win.width & 0x01) ? win.width : (win.width - 1); //odd
    }

    if(handle->isp0_cfg->reg_isp_if_src_sel == SIF_BUS_MIPI && win.y_start < 1)
    {
        ISP_DMSG("[%s] MIPI start y: %d\n", __FUNCTION__, win.y_start);
        win.y_start = 1;
    }
    else
    {
        //ISP_DMSG("[%s] non-MIPI case\n", __FUNCTION__);
    }

    switch(isp_pipe){
        case ISP_PIPE_0:
            handle->isp0_cfg->reg_isp_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp0_cfg->reg_isp_crop_start_y = LIMIT_BITS(win.y_start, 12);
            handle->isp0_cfg->reg_isp_crop_width = LIMIT_BITS(win.width, 12);
            handle->isp0_cfg->reg_isp_crop_high = LIMIT_BITS(win.height, 12);

            //ISP_DMSG("[%s] isp crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp0_cfg->reg_isp_crop_start_x,
            //         handle->isp0_cfg->reg_isp_crop_start_y, handle->isp0_cfg->reg_isp_crop_width,
            //         handle->isp0_cfg->reg_isp_crop_high);
            break;

        case ISP_PIPE_1:
            handle->isp11_cfg->reg_isp_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp11_cfg->reg_isp_crop_start_y = LIMIT_BITS(win.y_start, 12);
            handle->isp11_cfg->reg_isp_crop_width = LIMIT_BITS(win.width, 12);
            handle->isp11_cfg->reg_isp_crop_high = LIMIT_BITS(win.height, 12);

            //ISP_DMSG("[%s] isp crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp11_cfg->reg_isp_crop_start_x,
            //         handle->isp11_cfg->reg_isp_crop_start_y, handle->isp11_cfg->reg_isp_crop_width,
            //         handle->isp11_cfg->reg_isp_crop_high);
            break;

        case ISP_PIPE_2:
            handle->isp12_cfg->reg_isp_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp12_cfg->reg_isp_crop_start_y = LIMIT_BITS(win.y_start, 12);
            handle->isp12_cfg->reg_isp_crop_width = LIMIT_BITS(win.width, 12);
            handle->isp12_cfg->reg_isp_crop_high = LIMIT_BITS(win.height, 12);

            //ISP_DMSG("[%s] isp crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp12_cfg->reg_isp_crop_start_x,
            //         handle->isp12_cfg->reg_isp_crop_start_y, handle->isp12_cfg->reg_isp_crop_width,
            //         handle->isp12_cfg->reg_isp_crop_high);
            break;
    }

    handle->ImgRect.x_start = win.x_start;
    handle->ImgRect.y_start = win.y_start;
    handle->ImgRect.width = win.width + 1;
    handle->ImgRect.height = win.height + 1;

    win.width = win.width + 1;
    win.height = win.height + 1;

    //ISP_AE_WIN strAEAWBWin =
    //{
    //    win.width / AE_WIN_MAX_WIDTH,
    //    win.height / AE_WIN_MAX_HEIGHT,
    //    AE_WIN_MAX_WIDTH,
    //    AE_WIN_MAX_HEIGHT
    //};

    ISP_AE_SetIntBlockRowNum(handle, AE_WIN_MAX_HEIGHT - 3);

    //ISP_AWB_SetWin(handle, handle->ImgRect, handle->AwbWin);
    //ISP_SetIspOutputLineCountInt(handle, isp_pipe, handle->ImgRect.height-1); //set line output interrupt at latest line
    ISP_SetIspOutputLineCountInt(handle, isp_pipe, handle->ImgRect.height-12); //workaround , to avoid interrupt latency
    ISP_SetLineCountInt0(handle,handle->ImgRect.height-1);
    return SUCCESS;
}

int ISP_GetISPCrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN *win)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    if(!win)
        return FAIL;

    switch(isp_pipe){
        case ISP_PIPE_0:
            win->x_start = handle->isp0_cfg->reg_isp_crop_start_x;
            win->y_start = handle->isp0_cfg->reg_isp_crop_start_y;
            win->width = handle->isp0_cfg->reg_isp_crop_width;
            win->height = handle->isp0_cfg->reg_isp_crop_high;
            break;

        case ISP_PIPE_1:
            win->x_start = handle->isp11_cfg->reg_isp_crop_start_x;
            win->y_start = handle->isp11_cfg->reg_isp_crop_start_y;
            win->width = handle->isp11_cfg->reg_isp_crop_width;
            win->height = handle->isp11_cfg->reg_isp_crop_high;
            break;

        case ISP_PIPE_2:
            win->x_start = handle->isp12_cfg->reg_isp_crop_start_x;
            win->y_start = handle->isp12_cfg->reg_isp_crop_start_y;
            win->width = handle->isp12_cfg->reg_isp_crop_width;
            win->height = handle->isp12_cfg->reg_isp_crop_high;
            break;
    }
    return SUCCESS;
}

int ISP_SetWDMACrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN win)
{
    SEN_DATATYPE type;
    isp_handle_t *handle = _ISP_HAL(hnd);
    ISP_GetSenDataType(handle, isp_pipe, &type);

    if(type == SEN_DATATYPE_YUV)
    {
        win.x_start = (win.x_start) & 0xfffe; // even
        win.width = (win.width & 0x01) ? win.width : (win.width - 1); //odd
    }

    if(handle->isp0_cfg->reg_isp_if_src_sel == SIF_BUS_MIPI && win.y_start < 1)
    {
        ISP_DMSG("[%s] MIPI start y: %d\n", __FUNCTION__, win.y_start);
        win.y_start = 1;
    }
    else
    {
        ISP_DMSG("[%s] non-MIPI case\n", __FUNCTION__);
    }

    switch(isp_pipe){
        case ISP_PIPE_0:
            handle->isp0_cfg->reg_isp_wdma_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp0_cfg->reg_isp_wdma_crop_start_y = LIMIT_BITS(win.y_start, 13);
            handle->isp0_cfg->reg_isp_wdma_crop_width = LIMIT_BITS(win.width, 16);
            handle->isp0_cfg->reg_isp_wdma_crop_high = LIMIT_BITS(win.height, 13);

            pr_info("[%s] wdma crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp0_cfg->reg_isp_wdma_crop_start_x,
                     handle->isp0_cfg->reg_isp_wdma_crop_start_y, handle->isp0_cfg->reg_isp_wdma_crop_width,
                     handle->isp0_cfg->reg_isp_wdma_crop_high);
            break;
        case ISP_PIPE_1:
            handle->isp11_cfg->reg_isp_wdma_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp11_cfg->reg_isp_wdma_crop_start_y = LIMIT_BITS(win.y_start, 12);
            handle->isp11_cfg->reg_isp_wdma_crop_width = LIMIT_BITS(win.width, 16);
            handle->isp11_cfg->reg_isp_wdma_crop_high = LIMIT_BITS(win.height, 12);

            ISP_DMSG("[%s] wdma crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp11_cfg->reg_isp_wdma_crop_start_x,
                     handle->isp11_cfg->reg_isp_wdma_crop_start_y, handle->isp11_cfg->reg_isp_wdma_crop_width,
                     handle->isp11_cfg->reg_isp_wdma_crop_high);
            break;
        case ISP_PIPE_2:
            handle->isp12_cfg->reg_isp_wdma_crop_start_x = LIMIT_BITS(win.x_start, 12);
            handle->isp12_cfg->reg_isp_wdma_crop_start_y = LIMIT_BITS(win.y_start, 12);
            handle->isp12_cfg->reg_isp_wdma_crop_width = LIMIT_BITS(win.width, 16);
            handle->isp12_cfg->reg_isp_wdma_crop_high = LIMIT_BITS(win.height, 12);

            ISP_DMSG("[%s] wdma crop: %d, %d, %d, %d\n", __FUNCTION__, handle->isp12_cfg->reg_isp_wdma_crop_start_x,
                     handle->isp12_cfg->reg_isp_wdma_crop_start_y, handle->isp12_cfg->reg_isp_wdma_crop_width,
                     handle->isp12_cfg->reg_isp_wdma_crop_high);
            break;
    }

    return SUCCESS;
}

int ISP_GetWDMACrop(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_CROP_WIN *win)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    if(!win)
        return FAIL;

    switch(isp_pipe){
        case ISP_PIPE_0:
            win->x_start = handle->isp0_cfg->reg_isp_wdma_crop_start_x;
            win->y_start = handle->isp0_cfg->reg_isp_wdma_crop_start_y;
            win->width = handle->isp0_cfg->reg_isp_wdma_crop_width;
            win->height = handle->isp0_cfg->reg_isp_wdma_crop_high;
            break;
        case ISP_PIPE_1:
            win->x_start = handle->isp11_cfg->reg_isp_wdma_crop_start_x;
            win->y_start = handle->isp11_cfg->reg_isp_wdma_crop_start_y;
            win->width = handle->isp11_cfg->reg_isp_wdma_crop_width;
            win->height = handle->isp11_cfg->reg_isp_wdma_crop_high;
            break;
        case ISP_PIPE_2:
            win->x_start = handle->isp12_cfg->reg_isp_wdma_crop_start_x;
            win->y_start = handle->isp12_cfg->reg_isp_wdma_crop_start_y;
            win->width = handle->isp12_cfg->reg_isp_wdma_crop_width;
            win->height = handle->isp12_cfg->reg_isp_wdma_crop_high;
            break;
    }

    return SUCCESS;
}

///////////////// sensor IF setting, isp_sensorif_cfg ////////////
int ISP_ISPMasterEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_master_mode = enable;
    return SUCCESS;
}

int ISP_SenClkPol(ISP_HAL_HANDLE hnd, CLK_POL pol)
{
    switch(pol)
    {
        case CLK_POL_POS:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_clk_sensor_polarity = 0;
            break;
        case CLK_POL_NEG:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_clk_sensor_polarity = 1;
            break;
    }

    return SUCCESS;
}

int ISP_SenPCLKDelay(ISP_HAL_HANDLE hnd, int delay)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_clk_sensor_delay = LIMIT_BITS(delay, 2);
    return SUCCESS;
}
#if 0 //move to VIF
int ISP_SenHsyncPol(ISP_HAL_HANDLE hnd, CLK_POL pol)
{
    switch(pol)
    {
        case CLK_POL_POS:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_hsync_polarity = 1;
            break;
        case CLK_POL_NEG:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_hsync_polarity = 0;
            break;
    }

    return SUCCESS;
}
#endif
#if 0 //move to VIF
int ISP_SenVsyncPol(ISP_HAL_HANDLE hnd, CLK_POL pol)
{
    switch(pol)
    {
        case CLK_POL_POS:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity = 1;
            break;
        case CLK_POL_NEG:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity = 0;
            break;
    }

    return SUCCESS;
}
#endif
#if 0 //move to VIF
int ISP_GetSenHsyncPol(ISP_HAL_HANDLE hnd, CLK_POL* pol)
{
    if(_ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity == 1)
        *pol = CLK_POL_POS;

    if(_ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity == 0)
        *pol = CLK_POL_NEG;

    return SUCCESS;
}

int ISP_GetSenVsyncPol(ISP_HAL_HANDLE hnd, CLK_POL* pol)
{
    if(_ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity == 1)
        *pol = CLK_POL_POS;

    if(_ISP_HAL(hnd)->isp0_cfg->reg_sensor_vsync_polarity == 0)
        *pol = CLK_POL_NEG;

    return SUCCESS;
}
#endif

int ISP_SenDataType(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_DATATYPE type)
{
    //ISP_DMSG("[%s] %d\n", __FUNCTION__, type);
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(type){
                case SEN_DATATYPE_YUV:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_rgb_in = 0;
                    break;
                case SEN_DATATYPE_BAYER:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_rgb_in = 1;
                    break;
                default:
                    break;
            }
            break;
        case ISP_PIPE_1:
            switch(type){
                case SEN_DATATYPE_YUV:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_rgb_in = 0;
                    break;
                case SEN_DATATYPE_BAYER:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_rgb_in = 1;
                    break;
                default:
                    break;
            }
            break;
        case ISP_PIPE_2:
            switch(type){
                case SEN_DATATYPE_YUV:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_rgb_in = 0;
                    break;
                case SEN_DATATYPE_BAYER:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_rgb_in = 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return SUCCESS;
}

int ISP_GetSenDataType(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_DATATYPE *type)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(_ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_rgb_in){
                case 0:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_YUV;
                    break;
                case 1:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_BAYER;
                    break;
                default:
                    ISP_DMSG("!!!!!Unknown sensor data type value!\n");
                    break;
            }
            break;
        case ISP_PIPE_1:
            switch(_ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_rgb_in){
                case 0:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_YUV;
                    break;
                case 1:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_BAYER;
                    break;
                default:
                    ISP_DMSG("!!!!!Unknown sensor data type value!\n");
                    break;
            }
            break;
        case ISP_PIPE_2:
            switch(_ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_rgb_in){
                case 0:
                    //ISP_VMSG("[%s] SEN_DATATYPE_YUV %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_YUV;
                    break;
                case 1:
                    //ISP_VMSG("[%s] SEN_DATATYPE_BAYER %d\n", __FUNCTION__, type);
                    *type = SEN_DATATYPE_BAYER;
                    break;
                default:
                    ISP_DMSG("!!!!!Unknown sensor data type value!\n");
                    break;
            }
            break;
    }
    return SUCCESS;
}

int ISP_SenDataPrecision(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_DATAPRECISION fmt)
{
    //ISP_VMSG("[%s] %d\n", __FUNCTION__, fmt);
    isp_handle_t *handle = _ISP_HAL(hnd);

    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(fmt){
                case ISP_DATAPRECISION_16:
                    handle->isp0_cfg->reg_isp_sensor_format = 2;
                    break;
                case ISP_DATAPRECISION_12:
                    handle->isp0_cfg->reg_isp_sensor_format = 3;
                    break;
                case ISP_DATAPRECISION_10:
                    handle->isp0_cfg->reg_isp_sensor_format = 1;
                    break;
                case ISP_DATAPRECISION_8:
                    handle->isp0_cfg->reg_isp_sensor_format = 0;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(fmt){
                case ISP_DATAPRECISION_16:
                    handle->isp11_cfg->reg_isp_sensor_format = 2;
                    break;
                case ISP_DATAPRECISION_12:
                    handle->isp11_cfg->reg_isp_sensor_format = 3;
                    break;
                case ISP_DATAPRECISION_10:
                    handle->isp11_cfg->reg_isp_sensor_format = 1;
                    break;
                case ISP_DATAPRECISION_8:
                    handle->isp11_cfg->reg_isp_sensor_format = 0;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(fmt){
                case ISP_DATAPRECISION_16:
                    handle->isp12_cfg->reg_isp_sensor_format = 2;
                    break;
                case ISP_DATAPRECISION_12:
                    handle->isp12_cfg->reg_isp_sensor_format = 3;
                    break;
                case ISP_DATAPRECISION_10:
                    handle->isp12_cfg->reg_isp_sensor_format = 1;
                    break;
                case ISP_DATAPRECISION_8:
                    handle->isp12_cfg->reg_isp_sensor_format = 0;
                    break;
                default:
                    return FAIL;
            }
            break;
        default:
            break;
    }

    return SUCCESS;
}

int ISP_GetSenDataPrecision(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_DATAPRECISION *fmt)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(_ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_format){
                case 0:
                    *fmt = ISP_DATAPRECISION_8;
                    break;
                case 1:
                    *fmt = ISP_DATAPRECISION_10;
                    break;
                case 2:
                    *fmt = ISP_DATAPRECISION_16;
                    break;
                case 3:
                    *fmt = ISP_DATAPRECISION_12;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(_ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_format){
                case 0:
                    *fmt = ISP_DATAPRECISION_8;
                    break;
                case 1:
                    *fmt = ISP_DATAPRECISION_10;
                    break;
                case 2:
                    *fmt = ISP_DATAPRECISION_16;
                    break;
                case 3:
                    *fmt = ISP_DATAPRECISION_12;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(_ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_format){
                case 0:
                    *fmt = ISP_DATAPRECISION_8;
                    break;
                case 1:
                    *fmt = ISP_DATAPRECISION_10;
                    break;
                case 2:
                    *fmt = ISP_DATAPRECISION_16;
                    break;
                case 3:
                    *fmt = ISP_DATAPRECISION_12;
                    break;
                default:
                    return FAIL;
            }
            break;
        default:
            break;
    }

    return SUCCESS;
}

#if 0 //move to VIF
int ISP_SenLeftShift(ISP_HAL_HANDLE hnd, SEN_LEFT_SHIFT left_shift)
{

    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_format_left_sht = left_shift;

    ISP_DMSG("[%s] select: %d, sensor_shift: %d\n", __FUNCTION__,
             left_shift, _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_format_left_sht);
    return SUCCESS;
}
#endif

int ISP_SenFmtConv(ISP_HAL_HANDLE hnd, SEN_FMT_CONV_MODE method)
{
    //isp_handle_t *handle = _ISP_HAL(hnd);
    u8 mode = 0;
    switch(method)
    {
        case SEN_8TO12_7074    :
            mode = 0;
            break;
        case SEN_8TO12_7000    :
            mode = 1;
            break;
        case SEN_8TO12_114118  :
            mode = 2;
            break;
        case SEN_8TO12_11400   :
            mode = 3;
            break;
        case SEN_10TO12_9098   :
            mode = 0;
            break;
        case SEN_10TO12_9000   :
            mode = 1;
            break;
        case SEN_10TO12_1121110:
            mode = 2;
            break;
        case SEN_10TO12_11200  :
            mode = 3;
            break;
        default:
            return FAIL;
    }
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_format_ext_mode = mode;
    return SUCCESS;
}

int ISP_SenBayerFmt(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, SEN_BAYER bayer)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(bayer){
                case BAYER_RG: //# 2'h0: [R G R G ...;
                    handle->isp0_cfg->reg_isp_sensor_array = 0;
                    break;
                case BAYER_GR: //# 2'h1: [G R G R ...;
                    handle->isp0_cfg->reg_isp_sensor_array = 1;
                    break;
                case BAYER_BG: //# 2'h2: [B G B G ...;
                    handle->isp0_cfg->reg_isp_sensor_array = 2;
                    break;
                case BAYER_GB: //# 2'h2: [B G B G ...;
                    handle->isp0_cfg->reg_isp_sensor_array = 3;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(bayer){
                case BAYER_RG: //# 2'h0: [R G R G ...;
                    handle->isp11_cfg->reg_sensor_array = 0;
                    break;
                case BAYER_GR: //# 2'h1: [G R G R ...;
                    handle->isp11_cfg->reg_sensor_array = 1;
                    break;
                case BAYER_BG: //# 2'h2: [B G B G ...;
                    handle->isp11_cfg->reg_sensor_array = 2;
                    break;
                case BAYER_GB: //# 2'h2: [B G B G ...;
                    handle->isp11_cfg->reg_sensor_array = 3;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(bayer){
                case BAYER_RG: //# 2'h0: [R G R G ...;
                    handle->isp12_cfg->reg_sensor_array = 0;
                    break;
                case BAYER_GR: //# 2'h1: [G R G R ...;
                    handle->isp12_cfg->reg_sensor_array = 1;
                    break;
                case BAYER_BG: //# 2'h2: [B G B G ...;
                    handle->isp12_cfg->reg_sensor_array = 2;
                    break;
                case BAYER_GB: //# 2'h2: [B G B G ...;
                    handle->isp12_cfg->reg_sensor_array = 3;
                    break;
                default:
                    return FAIL;
            }
            break;
        default:
            break;
    }

    return SUCCESS;
}
int ISP_SenRGBIRFmt(ISP_HAL_HANDLE hnd, SEN_RGBIR RGBIR_ID)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    switch(RGBIR_ID)
    {
        case RGBIR_R0:
            handle->isp0_cfg->reg_isp_rgbir_format = 0;
            break;
        case RGBIR_G0:
            handle->isp0_cfg->reg_isp_rgbir_format = 1;
            break;
        case RGBIR_B0:
            handle->isp0_cfg->reg_isp_rgbir_format = 2;
            break;
        case RGBIR_G1:
            handle->isp0_cfg->reg_isp_rgbir_format = 3;
            break;
        case RGBIR_G2:
            handle->isp0_cfg->reg_isp_rgbir_format = 4;
            break;
        case RGBIR_I0:
            handle->isp0_cfg->reg_isp_rgbir_format = 5;
            break;
        case RGBIR_G3:
            handle->isp0_cfg->reg_isp_rgbir_format = 6;
            break;
        case RGBIR_I1:
            handle->isp0_cfg->reg_isp_rgbir_format = 7;
            break;

        default:
            return FAIL;
    }
    return SUCCESS;
}

int ISP_SenYCOrder(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe,  SEN_YC_ORDER order)
{
    //isp_handle_t *handle = _ISP_HAL(hnd);
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(order){
                case SEN_YCODR_CY:
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_yuv_order = 0;
                    break;
                case SEN_YCODR_YC:
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_yuv_order = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(order){
                case SEN_YCODR_CY:
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_yuv_order = 0;
                    break;
                case SEN_YCODR_YC:
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_sensor_yuv_order = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(order){
                case SEN_YCODR_CY:
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_yuv_order = 0;
                    break;
                case SEN_YCODR_YC:
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_sensor_yuv_order = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
    }

    return SUCCESS;

}
#if 0 //move to VIF
int ISP_SenVsyncDly(ISP_HAL_HANDLE hnd, SEN_VSYNC_DLY delay)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    switch(delay)
    {
        case SEN_VSYNC_DLY_VF:
            handle->isp0_cfg->reg_isp_sensor_vs_dly = 0;
            break;
        case SEN_VSYNC_DLY_VR2:
            handle->isp0_cfg->reg_isp_sensor_vs_dly = 1;
            break;
        case SEN_VSYNC_DLY_VR1:
            handle->isp0_cfg->reg_isp_sensor_vs_dly = 2;
            break;
        case SEN_VSYNC_DLY_VR:
            handle->isp0_cfg->reg_isp_sensor_vs_dly = 3;
            break;
        default:
            return FAIL;
    }
    return SUCCESS;
}

int ISP_SenHsyncDly(ISP_HAL_HANDLE hnd, SEN_HSYNC_DLY delay)
{
    switch(delay)
    {
        case SEN_HSYNC_DLY_HR:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_hs_dly = 0;
            break;
        case SEN_HSYNC_DLY_HF:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_hs_dly = 1;
            break;
    }
    return SUCCESS;
}
#endif
///////////////// sensor interface ////////////////////
int ISP_RMUXSrc(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_RMUX src) // ISP IF (isp_dp) source, reg_isp_if_rmux_sel
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(src){
                case ISP_RMUX_SIF:
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_rmux_sel = 0;
                    break;
                case ISP_RMUX_RDMA:
                    _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_rmux_sel = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(src){
                case ISP_RMUX_SIF:
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_if_rmux_sel = 0;
                    break;
                case ISP_RMUX_RDMA:
                    _ISP_HAL(hnd)->isp11_cfg->reg_isp_if_rmux_sel = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(src){
                case ISP_RMUX_SIF:
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_if_rmux_sel = 0;
                    break;
                case ISP_RMUX_RDMA:
                    _ISP_HAL(hnd)->isp12_cfg->reg_isp_if_rmux_sel = 1;
                    break;
                default:
                    return FAIL;
            }
            break;
    }

    return SUCCESS;
}

int ISP_WMUXSrc(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, ISP_WMUX src)
{
    isp_handle_t *handle = _ISP_HAL(hnd);

    switch(isp_pipe){
        case ISP_PIPE_0:
            switch(src){
                case ISP_WMUX_SIF:
                    handle->isp0_cfg->reg_isp_if_wmux_sel = 0;
                    break;
                case ISP_WMUX_ISP:
                    handle->isp0_cfg->reg_isp_if_wmux_sel = 1;
                    break;
                case ISP_WMUX_ISPDS:
                    handle->isp0_cfg->reg_isp_if_wmux_sel = 2;
                    break;
                case ISP_WMUX_ISPSWAPYC:
                    handle->isp0_cfg->reg_isp_if_wmux_sel = 3;
                    break;
                // FIXJASON
                case ISP_WMUX_VDOS:
                    handle->isp0_cfg->reg_isp_if_wmux_sel = 4;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_1:
            switch(src){
                case ISP_WMUX_SIF:
                    handle->isp11_cfg->reg_isp_if_wmux_sel = 0;
                    break;
                case ISP_WMUX_ISP:
                    handle->isp11_cfg->reg_isp_if_wmux_sel = 1;
                    break;
                case ISP_WMUX_ISPDS:
                    handle->isp11_cfg->reg_isp_if_wmux_sel = 2;
                    break;
                case ISP_WMUX_ISPSWAPYC:
                    handle->isp11_cfg->reg_isp_if_wmux_sel = 3;
                    break;
                // FIXJASON
                case ISP_WMUX_VDOS:
                    handle->isp11_cfg->reg_isp_if_wmux_sel = 4;
                    break;
                default:
                    return FAIL;
            }
            break;
        case ISP_PIPE_2:
            switch(src){
                case ISP_WMUX_SIF:
                    handle->isp12_cfg->reg_isp_if_wmux_sel = 0;
                    break;
                case ISP_WMUX_ISP:
                    handle->isp12_cfg->reg_isp_if_wmux_sel = 1;
                    break;
                case ISP_WMUX_ISPDS:
                    handle->isp12_cfg->reg_isp_if_wmux_sel = 2;
                    break;
                case ISP_WMUX_ISPSWAPYC:
                    handle->isp12_cfg->reg_isp_if_wmux_sel = 3;
                    break;
                // FIXJASON
                case ISP_WMUX_VDOS:
                    handle->isp12_cfg->reg_isp_if_wmux_sel = 4;
                    break;
                default:
                    return FAIL;
            }
            break;
    }
    return SUCCESS;
}
#if 0 //move to VIF
int ISP_SIFReset(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_state_rst = enable;
    return SUCCESS;
}
#endif
int ISP_RVFSrc(ISP_HAL_HANDLE hnd, SIF_RVF rvf)   // Sensor IF source, reg_isp_if_src_sel
{
    switch(rvf)
    {
        case SIF_MUX_RMUX:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_rvs_sel = 0;
            break;
        case SIF_MUX_SENSOR:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_rvs_sel = 1;
            break;
        default:
            return FAIL;
    }
    return SUCCESS;
}

int ISP_SensorBus(ISP_HAL_HANDLE hnd, SIF_BUS type)
{
#if 0 //Move to drv_SensorIF.c
    // mipi/parallel
    if(type == SIF_BUS_PARL) //PARALLEL
    {
        HalClkpad_Set_SR_PCLK(ISP_PCLK_SR_PAD);
    }
    else //MIPO
    {
        HalClkpad_Set_SR_PCLK(ISP_PCLK_MIPI_TOP);
    }
#endif
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_if_src_sel = (type == SIF_BUS_PARL ? 0 : 1);
    return SUCCESS;
}

int ISP_GetSensorBus(ISP_HAL_HANDLE hnd, SIF_BUS *type)
{
    // mipi/parallel
    switch(_ISP_HAL(hnd)->isp0_cfg->reg_isp_if_src_sel)
    {
        case 0:
            *type = SIF_BUS_PARL;
            break;
        case 1:
            *type = SIF_BUS_MIPI;
            break;
    }

    return SUCCESS;
}

int ISP_HsyncPipeDelayYCmode(ISP_HAL_HANDLE hnd, int value)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_hs_pipe = value;
    return SUCCESS;
}

int ISP_HsyncPipeDelayYCmodeMask(ISP_HAL_HANDLE hnd, int value)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_s2p_hs_mask = value;
    return SUCCESS;
}

int ISP_ICPASync(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    /*
     *    0: icp2isp_ack `
          1: icp2isp_ack Tw|1, ]NO|ICPL
     */
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_icp_ack_tie1 = enable;
    if(enable)//workaround for MV5 ,
    {
        _ISP_HAL(hnd)->wdma3_cfg->reg_isp_wdma_en = 1;
    }
    return SUCCESS;
}

//////////////////// IQ block function /////////////////////
// global, IQ blocks, isp_block_cfg
int ISP_IQ_Init_Cfg(ISP_HAL_HANDLE hnd)
{
    //fix setting
    isp_handle_t *handle = _ISP_HAL(hnd);
    int i;
    u32 u4DefaultH[33] =
    {
        //coef0   coef1   coef2   coef3
        0x036 | (0x97 << 8) | (0x36 << 16) | (0xffd << 24),
        0x034 | (0x97 << 8) | (0x38 << 16) | (0xffd << 24),
        0x032 | (0x97 << 8) | (0x3a << 16) | (0xffd << 24),
        0x030 | (0x97 << 8) | (0x3c << 16) | (0xffd << 24),
        0x02e | (0x97 << 8) | (0x3f << 16) | (0xffc << 24),
        0x02d | (0x97 << 8) | (0x3f << 16) | (0xffd << 24),
        0x02b | (0x96 << 8) | (0x42 << 16) | (0xffd << 24),
        0x029 | (0x96 << 8) | (0x44 << 16) | (0xffd << 24),
        0x027 | (0x96 << 8) | (0x46 << 16) | (0xffd << 24),
        0x025 | (0x95 << 8) | (0x49 << 16) | (0xffd << 24),
        0x024 | (0x94 << 8) | (0x4b << 16) | (0xffd << 24),
        0x022 | (0x94 << 8) | (0x4d << 16) | (0xffd << 24),
        0x020 | (0x93 << 8) | (0x50 << 16) | (0xffd << 24),
        0x01f | (0x92 << 8) | (0x52 << 16) | (0xffd << 24),
        0x01d | (0x92 << 8) | (0x53 << 16) | (0xffe << 24),
        0x01c | (0x91 << 8) | (0x55 << 16) | (0xffe << 24),
        0x01a | (0x90 << 8) | (0x58 << 16) | (0xffe << 24),
        0x019 | (0x8f << 8) | (0x5a << 16) | (0xffe << 24),
        0x017 | (0x8e << 8) | (0x5c << 16) | (0xfff << 24),
        0x016 | (0x8c << 8) | (0x5f << 16) | (0xfff << 24),
        0x015 | (0x8b << 8) | (0x60 << 16) | (0x000 << 24),
        0x013 | (0x8a << 8) | (0x63 << 16) | (0x000 << 24),
        0x012 | (0x89 << 8) | (0x64 << 16) | (0x001 << 24),
        0x011 | (0x87 << 8) | (0x67 << 16) | (0x001 << 24),
        0x010 | (0x86 << 8) | (0x68 << 16) | (0x002 << 24),
        0x00e | (0x84 << 8) | (0x6c << 16) | (0x002 << 24),
        0x00d | (0x83 << 8) | (0x6d << 16) | (0x003 << 24),
        0x00c | (0x81 << 8) | (0x6f << 16) | (0x004 << 24),
        0x00b | (0x80 << 8) | (0x71 << 16) | (0x004 << 24),
        0x00a | (0x7e << 8) | (0x73 << 16) | (0x005 << 24),
        0x009 | (0x7c << 8) | (0x75 << 16) | (0x006 << 24),
        0x008 | (0x7a << 8) | (0x77 << 16) | (0x007 << 24),
        0x008 | (0x79 << 8) | (0x78 << 16) | (0x007 << 24),
    };

    u32 u4DefaultV[33] =
    {
        //coef0   coef1   coef2   coef3
        0x036 | (0x97 << 8) | (0x36 << 16) | (0xffd << 24),
        0x034 | (0x97 << 8) | (0x38 << 16) | (0xffd << 24),
        0x032 | (0x97 << 8) | (0x3a << 16) | (0xffd << 24),
        0x030 | (0x97 << 8) | (0x3c << 16) | (0xffd << 24),
        0x02e | (0x97 << 8) | (0x3f << 16) | (0xffc << 24),
        0x02d | (0x97 << 8) | (0x3f << 16) | (0xffd << 24),
        0x02b | (0x96 << 8) | (0x42 << 16) | (0xffd << 24),
        0x029 | (0x96 << 8) | (0x44 << 16) | (0xffd << 24),
        0x027 | (0x96 << 8) | (0x46 << 16) | (0xffd << 24),
        0x025 | (0x95 << 8) | (0x49 << 16) | (0xffd << 24),
        0x024 | (0x94 << 8) | (0x4b << 16) | (0xffd << 24),
        0x022 | (0x94 << 8) | (0x4d << 16) | (0xffd << 24),
        0x020 | (0x93 << 8) | (0x50 << 16) | (0xffd << 24),
        0x01f | (0x92 << 8) | (0x52 << 16) | (0xffd << 24),
        0x01d | (0x92 << 8) | (0x53 << 16) | (0xffe << 24),
        0x01c | (0x91 << 8) | (0x55 << 16) | (0xffe << 24),
        0x01a | (0x90 << 8) | (0x58 << 16) | (0xffe << 24),
        0x019 | (0x8f << 8) | (0x5a << 16) | (0xffe << 24),
        0x017 | (0x8e << 8) | (0x5c << 16) | (0xfff << 24),
        0x016 | (0x8c << 8) | (0x5f << 16) | (0xfff << 24),
        0x015 | (0x8b << 8) | (0x60 << 16) | (0x000 << 24),
        0x013 | (0x8a << 8) | (0x63 << 16) | (0x000 << 24),
        0x012 | (0x89 << 8) | (0x64 << 16) | (0x001 << 24),
        0x011 | (0x87 << 8) | (0x67 << 16) | (0x001 << 24),
        0x010 | (0x86 << 8) | (0x68 << 16) | (0x002 << 24),
        0x00e | (0x84 << 8) | (0x6c << 16) | (0x002 << 24),
        0x00d | (0x83 << 8) | (0x6d << 16) | (0x003 << 24),
        0x00c | (0x81 << 8) | (0x6f << 16) | (0x004 << 24),
        0x00b | (0x80 << 8) | (0x71 << 16) | (0x004 << 24),
        0x00a | (0x7e << 8) | (0x73 << 16) | (0x005 << 24),
        0x009 | (0x7c << 8) | (0x75 << 16) | (0x006 << 24),
        0x008 | (0x7a << 8) | (0x77 << 16) | (0x007 << 24),
        0x008 | (0x79 << 8) | (0x78 << 16) | (0x007 << 24),
    };

    // set ALSC interpolation coeff
    handle->isp3_cfg->reg_coef_buf_sel_v    = 0;
    for(i = 0; i < 33; i++)
    {
        handle->isp3_cfg->reg_coef_buf_wd_1 = u4DefaultH[i] >> 16;
        handle->isp3_cfg->reg_coef_buf_wd   = u4DefaultH[i] & 0x0ffff;
        handle->isp3_cfg->reg_coef_buf_adr  = i;
        handle->isp3_cfg->reg_coef_buf_we   = true;
    }

    handle->isp3_cfg->reg_coef_buf_sel_v    = 1;
    for(i = 0; i < 33; i++)
    {
        handle->isp3_cfg->reg_coef_buf_wd_1 = u4DefaultV[i] >> 16;
        handle->isp3_cfg->reg_coef_buf_wd   = u4DefaultV[i] & 0x0ffff;
        handle->isp3_cfg->reg_coef_buf_adr  = i;
        handle->isp3_cfg->reg_coef_buf_we   = true;
    }
    handle->isp3_cfg->reg_coef_buf_re       = 0;

    //
    handle->isp0_cfg->reg_isp_r2y_cmc_en = 1;
    handle->isp0_cfg->reg_isp_r2y_y_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_r_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_b_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_y_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_r_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_b_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_cb_add_128_post_en = 1;
    handle->isp0_cfg->reg_isp_r2y_cr_add_128_post_en = 1;
    handle->isp0_cfg->reg_isp_r2y_rran = 0;
    handle->isp0_cfg->reg_isp_r2y_gran = 0;
    handle->isp0_cfg->reg_isp_r2y_bran = 0;
    handle->isp0_cfg->reg_isp_r2y_coeff_11 = 512;
    handle->isp0_cfg->reg_isp_r2y_coeff_12 = 7763;
    handle->isp0_cfg->reg_isp_r2y_coeff_13 = 8109;
    handle->isp0_cfg->reg_isp_r2y_coeff_21 = 306;
    handle->isp0_cfg->reg_isp_r2y_coeff_22 = 601;
    handle->isp0_cfg->reg_isp_r2y_coeff_23 = 117;
    handle->isp0_cfg->reg_isp_r2y_coeff_31 = 8019;
    handle->isp0_cfg->reg_isp_r2y_coeff_32 = 7853;
    handle->isp0_cfg->reg_isp_r2y_coeff_33 = 512;

    handle->isp0_cfg->reg_c_filter = 0x04;  //[2]:1, color lpf

    //hdr
    handle->isp10_cfg->reg_hdr_map_dc_filter11 = 1;
    handle->isp10_cfg->reg_hdr_map_dc_filter12 = 4;
    handle->isp10_cfg->reg_hdr_map_dc_filter13 = 6;
    handle->isp10_cfg->reg_hdr_map_dc_filter21 = 4;
    handle->isp10_cfg->reg_hdr_map_dc_filter22 = 16;
    handle->isp10_cfg->reg_hdr_map_dc_filter23 = 24;
    handle->isp10_cfg->reg_hdr_map_dc_filter31 = 6;
    handle->isp10_cfg->reg_hdr_map_dc_filter32 = 24;
    handle->isp10_cfg->reg_hdr_map_dc_filter33 = 36;

    handle->isp10_cfg->reg_hdr_dc_sft_x0 = 4;
    handle->isp10_cfg->reg_hdr_dc_sft_x1 = 5;
    handle->isp10_cfg->reg_hdr_dc_sft_x2 = 5;
    handle->isp10_cfg->reg_hdr_dc_sft_x3 = 5;
    handle->isp10_cfg->reg_hdr_dc_sft_x4 = 5;
    handle->isp10_cfg->reg_hdr_dc_sft_x5 = 5;
    handle->isp10_cfg->reg_hdr_dc_sft_x6 = 10;

    handle->isp10_cfg->reg_hdr_dc_gain_y0 = 255;
    handle->isp10_cfg->reg_hdr_dc_gain_y1 = 160;
    handle->isp10_cfg->reg_hdr_dc_gain_y2 = 80;
    handle->isp10_cfg->reg_hdr_dc_gain_y3 = 40;
    handle->isp10_cfg->reg_hdr_dc_gain_y4 = 20;
    handle->isp10_cfg->reg_hdr_dc_gain_y5 = 10;
    handle->isp10_cfg->reg_hdr_dc_gain_y6 = 0;
    handle->isp10_cfg->reg_hdr_dc_gain_y7 = 0;

    handle->isp10_cfg->reg_hdr_ac_sft_x0 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x1 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x2 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x3 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x4 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x5 = 7;
    handle->isp10_cfg->reg_hdr_ac_sft_x6 = 8;

    handle->isp10_cfg->reg_hdr_ac_gain_y0 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y1 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y2 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y3 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y4 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y5 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y6 = 255;
    handle->isp10_cfg->reg_hdr_ac_gain_y7 = 255;

    return SUCCESS;
}

static int _ISP_IQ_WB_PreGain_Cfg(ISP_HAL_HANDLE hnd, IQ_WBG_CFG *wbg)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    handle->isp1_cfg->reg_wbg_dith_en = wbg->dither;
    handle->isp1_cfg->reg_isp_wbg_r_gain = LIMIT_BITS(wbg->r_gain, 13);
    handle->isp1_cfg->reg_isp_wbg_r_offset = LIMIT_BITS(wbg->r_offset, 11);
    handle->isp1_cfg->reg_isp_wbg_gr_gain = LIMIT_BITS(wbg->gr_gain, 13);
    handle->isp1_cfg->reg_isp_wbg_gr_offset = LIMIT_BITS(wbg->gr_offset, 11);
    handle->isp1_cfg->reg_isp_wbg_b_gain = LIMIT_BITS(wbg->b_gain, 13);
    handle->isp1_cfg->reg_isp_wbg_b_offset = LIMIT_BITS(wbg->b_offset, 11);
    handle->isp1_cfg->reg_isp_wbg_gb_gain = LIMIT_BITS(wbg->gb_gain, 13);
    handle->isp1_cfg->reg_isp_wbg_gb_offset = LIMIT_BITS(wbg->gb_offset, 11);
    return SUCCESS;
}

static int _ISP_IQ_WB_PostGain_Cfg(ISP_HAL_HANDLE hnd, IQ_WBG_CFG *wbg)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    handle->isp6_cfg->reg_wbg1_dith_en = wbg->dither;
    handle->isp6_cfg->reg_isp_wbg1_r_gain = LIMIT_BITS(wbg->r_gain, 13);
    handle->isp6_cfg->reg_isp_wbg1_r_offset = LIMIT_BITS(wbg->r_offset, 11);
    handle->isp6_cfg->reg_isp_wbg1_gr_gain = LIMIT_BITS(wbg->gr_gain, 13);
    handle->isp6_cfg->reg_isp_wbg1_gr_offset = LIMIT_BITS(wbg->gr_offset, 11);
    handle->isp6_cfg->reg_isp_wbg1_b_gain = LIMIT_BITS(wbg->b_gain, 13);
    handle->isp6_cfg->reg_isp_wbg1_b_offset = LIMIT_BITS(wbg->b_offset, 11);
    handle->isp6_cfg->reg_isp_wbg1_gb_gain = LIMIT_BITS(wbg->gb_gain, 13);
    handle->isp6_cfg->reg_isp_wbg1_gb_offset = LIMIT_BITS(wbg->gb_offset, 11);
    return SUCCESS;
}

int ISP_IQ_WBG_Cfg(ISP_HAL_HANDLE hnd, IQ_WBG_CFG *wbg)
{
    isp_handle_t *handle = _ISP_HAL(hnd);
    if(wbg->before_denoise == 1)
    {
        handle->isp0_cfg->reg_en_isp_wbg0 = 1;
        handle->isp0_cfg->reg_en_isp_wbg1 = 0;

        _ISP_IQ_WB_PreGain_Cfg(handle, wbg);
    }
    else
    {
        handle->isp0_cfg->reg_en_isp_wbg0 = 0;
        handle->isp0_cfg->reg_en_isp_wbg1 = 1;

        _ISP_IQ_WB_PostGain_Cfg(handle, wbg);
    }
    return SUCCESS;
}
#if 0
int ISP_IQ_DN_Spike_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise)
{
#if 0
    // spike NR
    handle->isp1_cfg->reg_spike_nr_en = denoise.spike_nr_en;
    handle->isp1_cfg->reg_spike_nr_coef = denoise.spike_nr_coef;
    handle->isp1_cfg->reg_yp_22_step = denoise.yp_22_step;
    handle->isp1_cfg->reg_d_11_21_step = denoise.d_11_21_step;
    handle->isp1_cfg->reg_d_31_step = denoise.d_31_step;
    handle->isp1_cfg->reg_p_thrd = denoise.p_thrd;
    handle->isp1_cfg->reg_p_thrd_1 = denoise.p_thrd_1;
    handle->isp1_cfg->reg_p_thrd_2 = denoise.p_thrd_2;
    handle->isp1_cfg->reg_p_thrd_3 = denoise.p_thrd_3;
#endif
    return SUCCESS;
}

int ISP_IQ_DN_BSNR_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise)
{
    int i = 0, scale = 0;
#if 0
    // mean
    handle->isp1_cfg->reg_snr_en = denoise.snr_en;

    handle->isp1_cfg->reg_snr_std_low_thrd = denoise.snr_std_low_thrd;
    handle->isp1_cfg->reg_snr_strength_gain = denoise.snr_strength_gain;
    handle->isp1_cfg->reg_snr_alpha_step = denoise.snr_alpha_step;

    handle->isp1_cfg->reg_snr_std_low_thrd_r_pix = denoise.r_snr_std_low_thrd;
    handle->isp1_cfg->reg_snr_strength_gain_r_pix = denoise.r_snr_strength_gain;
    handle->isp1_cfg->reg_snr_alpha_step_r_pix = denoise.r_snr_alpha_step;

    handle->isp1_cfg->reg_snr_std_low_thrd_b_pix = denoise.b_snr_std_low_thrd;
    handle->isp1_cfg->reg_snr_strength_gain_b_pix = denoise.b_snr_strength_gain;
    handle->isp1_cfg->reg_snr_alpha_step_b_pix = denoise.b_snr_alpha_step;

    scale = 16 / 4;
    for(i = 0; i < 16; i += scale)
    {
        //snr_table *table = &((snr_table*)&(handle->isp1_cfg->reg_snr_table_0))[i/scale];
        snr_table *table = &((snr_table*)reg_addr(handle->isp1_cfg, reg_snr_table_0))[i / scale];
        table->tbl0 = denoise.snr_table[i];
        table->tbl1 = denoise.snr_table[i + 1];
        table->tbl2 = denoise.snr_table[i + 2];
        table->tbl3 = denoise.snr_table[i + 3];

        //rb
        //table = &((snr_table*)&(handle->isp1_cfg->reg_snr_table_r_pix_0))[i/scale];
        table = &((snr_table*)reg_addr(handle->isp1_cfg, reg_snr_table_r_pix_0))[i / scale];
        table->tbl0 = denoise.r_snr_table[i];
        table->tbl1 = denoise.r_snr_table[i + 1];
        table->tbl2 = denoise.r_snr_table[i + 2];
        table->tbl3 = denoise.r_snr_table[i + 3];

        //table = &((snr_table*)&(handle->isp1_cfg->reg_snr_table_b_pix_0))[i/scale];
        table = &((snr_table*)reg_addr(handle->isp1_cfg, reg_snr_table_b_pix_0))[i / scale];
        table->tbl0 = denoise.b_snr_table[i];
        table->tbl1 = denoise.b_snr_table[i + 1];
        table->tbl2 = denoise.b_snr_table[i + 2];
        table->tbl3 = denoise.b_snr_table[i + 3];
    }
#endif
    return SUCCESS;
}

int ISP_IQ_DN_NM_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise)
{
#if 0
    // noise masking
    handle->isp1_cfg->reg_snr_nm_filter_en = denoise.snr_nm_filter_en;
    handle->isp1_cfg->reg_snr_nm_gain =      denoise.snr_nm_gain;
    handle->isp1_cfg->reg_snr_nm_max_thrd =  denoise.snr_nm_max_thrd;
    handle->isp1_cfg->reg_snr_nm_min_thrd =  denoise.snr_nm_min_thrd;

    // rb
    handle->isp1_cfg->reg_snr_nm_gain_r_pix =      denoise.r_snr_nm_gain;
    handle->isp1_cfg->reg_snr_nm_max_thrd_r_pix =  denoise.r_snr_nm_max_thrd;
    handle->isp1_cfg->reg_snr_nm_min_thrd_r_pix =  denoise.r_snr_nm_min_thrd;

    handle->isp1_cfg->reg_snr_nm_gain_b_pix =      denoise.b_snr_nm_gain;
    handle->isp1_cfg->reg_snr_nm_max_thrd_b_pix =  denoise.b_snr_nm_max_thrd;
    handle->isp1_cfg->reg_snr_nm_min_thrd_b_pix =  denoise.b_snr_nm_min_thrd;
#endif
    return SUCCESS;
}

int ISP_IQ_DN_DNR_Cfg(ISP_HAL_HANDLE hnd, IQ_DENOISE_CFG denoise)
{
    int i = 0, scale = 0;
#if 0
    // DNR
    handle->stats_alsc_dnr_cfg->reg_dnr_en                     = denoise.dnr_en                    ;
    handle->stats_alsc_dnr_cfg->reg_dnr_lum_en                 = denoise.dnr_lum_en                ;
    handle->stats_alsc_dnr_cfg->reg_dnr_lum_sh                 = denoise.dnr_lum_sh                ;
    handle->stats_alsc_dnr_cfg->reg_dnr_filter_en              = denoise.dnr_filter_en             ;
    handle->stats_alsc_dnr_cfg->reg_dnr_filter_mode            = denoise.dnr_filter_mode           ;
    handle->stats_alsc_dnr_cfg->reg_dnr_filter_div0            = denoise.dnr_filter_div0           ;
    handle->stats_alsc_dnr_cfg->reg_dnr_filter_div1            = denoise.dnr_filter_div1           ;

//  ISP_DMSG("reg_dnr_en         :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_en         );
//  ISP_DMSG("reg_dnr_lum_en     :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_lum_en     );
//  ISP_DMSG("reg_dnr_lum_sh     :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_lum_sh     );
//  ISP_DMSG("reg_dnr_filter_en  :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_filter_en  );
//  ISP_DMSG("reg_dnr_filter_mode:%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_filter_mode);
//  ISP_DMSG("reg_dnr_filter_div0:%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_filter_div0);
//  ISP_DMSG("reg_dnr_filter_div1:%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_filter_div1);

    scale = 8 / 2; // array index convertor
    for(i = 0; i < 8; i += scale)
    {
        dnr_table *table = ((dnr_table*)reg_addr(handle->stats_alsc_dnr_cfg, reg_dnr_lum_table_0)) + i / scale;
        table->o0 = denoise.dnr_lum_table[i];
        table->e0 = denoise.dnr_lum_table[i + 1];
        table->o1 = denoise.dnr_lum_table[i + 2];
        table->e1 = denoise.dnr_lum_table[i + 3];
    }

    handle->stats_alsc_dnr_cfg->reg_nr_table_sel               = denoise.dnr_table_sel             ;
    handle->stats_alsc_dnr_cfg->reg_dnr_nondither_method       = denoise.dnr_nondither_method      ;
    handle->stats_alsc_dnr_cfg->reg_dnr_nondither_method_mode  = denoise.dnr_nondither_method_mode ;
    handle->stats_alsc_dnr_cfg->reg_round_mode                 = denoise.round_mode                ;
    handle->stats_alsc_dnr_cfg->reg_sticky_solver_th           = denoise.sticky_solver_th          ;
    handle->stats_alsc_dnr_cfg->reg_sticky_solver_en           = denoise.sticky_solver_en          ;

//  ISP_DMSG("reg_dnr_table_sel            :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_table_sel            );
//  ISP_DMSG("reg_dnr_nondither_method     :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_nondither_method     );
//  ISP_DMSG("reg_dnr_nondither_method_mode:%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_nondither_method_mode);
//  ISP_DMSG("reg_round_mode               :%d\n", handle->stats_alsc_dnr_cfg->reg_round_mode               );
//  ISP_DMSG("reg_sticky_solver_th         :%d\n", handle->stats_alsc_dnr_cfg->reg_sticky_solver_th         );
//  ISP_DMSG("reg_sticky_solver_en         :%d\n", handle->stats_alsc_dnr_cfg->reg_sticky_solver_en         );

    scale = 16 / 4;
    for(i = 0; i < 16; i += scale)
    {
        dnr_table *table = ((dnr_table*)reg_addr(handle->stats_alsc_dnr_cfg, reg_dnr_tabley_0)) + i / scale;
        table->o0 = denoise.dnr_tabley[i];
        table->e0 = denoise.dnr_tabley[i + 1];
        table->o1 = denoise.dnr_tabley[i + 2];
        table->e1 = denoise.dnr_tabley[i + 3];
    }

    handle->stats_alsc_dnr_cfg->reg_dnr_fb_pitch_in_tile       =
        (handle->isp0_cfg->reg_isp_crop_width + 191) / 192;

    handle->stats_alsc_dnr_cfg->reg_dnr_ofrm_sel               = denoise.dnr_ofrm_sel              ;

//  ISP_DMSG("reg_dnr_ofrm_sel         :%d\n", handle->stats_alsc_dnr_cfg->reg_dnr_ofrm_sel         );
#endif
    return SUCCESS;
}


int ISP_IQ_CFAI_Cfg(ISP_HAL_HANDLE hnd, IQ_CFAI_CFG CFAi)
{
#if 0
    handle->isp0_cfg->reg_cfai_en = CFAi.cfai_en;
    handle->isp0_cfg->reg_cfai_bypass = CFAi.cfai_bypass;

    //Demosaic
    handle->isp0_cfg->reg_demosaic_dvh_slope = CFAi.dvh_slope;
    handle->isp0_cfg->reg_demosaic_dvh_thrd = CFAi.dvh_thd;
    handle->isp0_cfg->reg_demosaic_cpxwei_slope = CFAi.cpx_slope;
    handle->isp0_cfg->reg_demosaic_cpxwei_thrd = CFAi.cpx_thd;
    handle->isp0_cfg->reg_nb_slope_m = CFAi.nb_slope_m;
    handle->isp0_cfg->reg_nb_slope_s = CFAi.nb_slope_s;
#endif
    return SUCCESS;
}

int ISP_IQ_CFAI_NR_Cfg(ISP_HAL_HANDLE hnd, IQ_CFAI_NR_CFG CFAi_NR)
{
#if 0
    //Post Denoise
    handle->isp0_cfg->reg_cfai_dnr_en = CFAi_NR.cfai_dnr_en;
    handle->isp0_cfg->reg_cfai_dnr_gavg_ref_en = CFAi_NR.cfai_dnr_gavg_ref_en;
    handle->isp0_cfg->reg_cfai_dnr_rb_ratio = CFAi_NR.rb_ratio;
    handle->isp0_cfg->reg_cfai_dnr_g_ratio = CFAi_NR.g_ratio;
#endif
    return SUCCESS;
}
#endif

int ISP_MotionCfg(ISP_HAL_HANDLE hnd, ISP_MOTION_CFG cfg)
{
    //_ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_motion_en   = cfg.bEnable;
    //_ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_motion_thrd = cfg.u2MotionThd;
    return SUCCESS;
}

int ISP_TransposeCfg(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    //_ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_rot_en      = enable;
    return SUCCESS;
}

int ISP_IQEnable(ISP_HAL_HANDLE hnd, ISP_IQ_BLK blk, u8 enable)
{
#if 0
    //ISP_DMSG("[%s] IP[%d] = %d\n",__FUNCTION__, blk, enable);

    switch(blk)
    {
        case IQ_OBC:
            handle->isp0_cfg->reg_en_isp_obc = enable;
            break;
        case IQ_WBG0:
            handle->isp0_cfg->reg_en_isp_wbg0 = enable;
            handle->isp0_cfg->reg_en_isp_wbg1 = 0;
            break;
        case IQ_WBG:
        case IQ_WBG1:
            handle->isp0_cfg->reg_en_isp_wbg1 = enable;
            handle->isp0_cfg->reg_en_isp_wbg0 = 0;
            break;
        case IQ_LSC:
            handle->isp0_cfg->reg_en_isp_lsc = enable;
            break;
        case IQ_CROSSTALK:
            handle ->isp1_cfg->reg_isp_geq_enable = enable;
            break;
        case IQ_AEAWB_STATS:
            handle->stats_alsc_dnr_cfg->reg_isp_statis_en = enable;
            break;
        case IQ_DPC:
            //if (enable == true)
            //  handle->isp0_cfg->block_cfg.reg_en_isp_denoise = enable;
            //handle->isp1_cfg->defpix_cfg.reg_isp_def_pix_en = enable;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_SPIKE_NR:
            if(enable == true)
                handle->isp0_cfg->reg_en_isp_denoise = enable;
            handle->isp1_cfg->reg_spike_nr_en = enable;
            break;
        case IQ_MEAN_FILTER:
            if(enable == true)
                handle->isp0_cfg->reg_en_isp_denoise = enable;
            handle->isp1_cfg->reg_snr_en = enable;       // mean filter
            break;
        case IQ_NOISE_MASKING:
            if(enable == true)
                handle->isp0_cfg->reg_en_isp_denoise = enable;
            handle->isp1_cfg->reg_snr_nm_filter_en = enable;
            break;
        case IQ_FALSECOLOR:
            handle->isp0_cfg->reg_rgb_falsecolor_en = enable;
            break;
        case IQ_AF_STATS:
            //handle->isp5_cfg->af_cfg.reg_af_5x5_sta_en = enable;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_CFAi:
            handle->isp0_cfg->reg_cfai_en = enable;
            break;
        case IQ_CFAi_NR:
            if(enable == true)
                handle->isp0_cfg->reg_cfai_en = enable;
            handle->isp0_cfg->reg_cfai_dnr_en = enable;
            break;
        case IQ_CCM:
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            //handle->isp0_cfg->reg_isp_cc_cmc_en = enable;
            break;
        case IQ_UVCM:
            handle->isp0_cfg->reg_isp_r2y_cmc_en = enable;
            break;
        case IQ_GAMMA_RAW_LC:
            handle->isp1_cfg->reg_gm12to10_en = enable;
            break;
        case IQ_GAMMA_RGB_LC:
            //handle->isp0_cfg->reg_isp_gamma_10to12_en = enable;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_GAMMA:
            //handle->isp0_cfg->reg_isp_gamma_crct_en = enable;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_ALSC:
            handle->stats_alsc_dnr_cfg->reg_en_alsc = enable;
            break;
        case IQ_DNR      :
            if(enable == true)
                handle->isp0_cfg->reg_en_isp_denoise = enable;
            handle->stats_alsc_dnr_cfg->reg_dnr_en = enable;
            break;
        case IQ_DNR_LUM  :
            handle->stats_alsc_dnr_cfg->reg_dnr_lum_en = enable;
            break;
        case IQ_MOTION   :
            handle->stats_alsc_dnr_cfg->reg_motion_en =  enable;
            break;
        case IQ_ROTATE   :
            handle->stats_alsc_dnr_cfg->reg_rot_en = enable;
            break;
        default:
            return FAIL;
    }
#endif
    return SUCCESS;
}
/*
int ISP_GetIQEnable(ISP_HAL_HANDLE hnd, ISP_IQ_BLK blk)
{
    int ip_en = 0;
    isp_handle_t *handle = _ISP_HAL(hnd);
    switch(blk)
    {
        case IQ_OBC:
            ip_en = handle->isp0_cfg->reg_en_isp_obc;
            break;
        case IQ_WBG0:
            ip_en = handle->isp0_cfg->reg_en_isp_wbg0;
            break;
        case IQ_WBG:
        case IQ_WBG1:
            ip_en = handle->isp0_cfg->reg_en_isp_wbg1;
            break;
        case IQ_LSC:
            ip_en = handle->isp0_cfg->reg_en_isp_lsc;
            break;
        case IQ_AEAWB_STATS:
            ip_en = handle->stats_alsc_dnr_cfg->reg_isp_statis_en;
            break;
        case IQ_DPC:
            //ip_en = handle->isp1_cfg->reg_isp_def_pix_en;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_CROSSTALK:
            ip_en = handle->isp1_cfg->reg_isp_geq_enable;
            break;
        case IQ_SPIKE_NR:
            ip_en = handle->isp1_cfg->reg_spike_nr_en;
            break;
        case IQ_MEAN_FILTER:
            ip_en = handle->isp1_cfg->reg_snr_en;       // mean filter
            break;
        case IQ_NOISE_MASKING:
            ip_en = handle->isp1_cfg->reg_snr_nm_filter_en;
            break;
        case IQ_FALSECOLOR:
            ip_en = handle->isp0_cfg->reg_rgb_falsecolor_en;
            break;
        case IQ_AF_STATS:
            //ip_en = handle->af_cfg->af_cfg.reg_af_5x5_sta_en;
            printf("%s , line%d, has no effect.\n", __FUNCTION__, __LINE__);
            break;
        case IQ_CFAi:
            ip_en = handle->isp0_cfg->reg_cfai_en;
            break;
        case IQ_CFAi_NR:
            ip_en = handle->isp0_cfg->reg_cfai_dnr_en;
            break;
        case IQ_CCM:
            //ip_en = handle->isp0_cfg->reg_isp_cc_cmc_en;
            ip_en = 0;
            break;
        case IQ_GAMMA_RAW_LC:
            //ip_en = handle->isp1_cfg->reg_gm12to10_en;
            break;
        case IQ_GAMMA_RGB_LC:
            //ip_en = handle->isp0_cfg->reg_isp_gamma_10to12_en;
            ip_en = 0;
            break;
        case IQ_GAMMA:
            //ip_en = handle->isp0_cfg->reg_isp_gamma_crct_en;
            ip_en = 0;
            break;
        case IQ_ALSC:
            ip_en = handle->stats_alsc_dnr_cfg->reg_en_alsc;
            break;
        case IQ_DNR      :
            ip_en = handle->stats_alsc_dnr_cfg->reg_dnr_en;
            break;
        case IQ_DNR_LUM  :
            ip_en = handle->stats_alsc_dnr_cfg->reg_dnr_lum_en;
            break;
        case IQ_MOTION   :
            ip_en = handle->stats_alsc_dnr_cfg->reg_motion_en;
            break;
        case IQ_ROTATE   :
            ip_en = handle->stats_alsc_dnr_cfg->reg_rot_en;
            break;
        default:
            return FAIL;
    }
    return ip_en;

}
*/

int ISP_ClearAEAWBStats(ISP_HAL_HANDLE hnd)
{
    //DPRCD;
    return 0;
}

int ISP_EnableISP(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_en_isp = enable;
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp0_cfg->reg_en_isp_p1 = enable;
            _ISP_HAL(hnd)->isp11_cfg->reg_isp_en = enable;
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp0_cfg->reg_en_isp_p2 = enable;
            _ISP_HAL(hnd)->isp12_cfg->reg_isp_en = enable;
            break;
    }
    return 0;
}

int ISP_BypassISP(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    //handle->isp0_cfg->camsensor_ctl.reg_isp_bypass_en = enable;
    DPRCD;
    return 0;
}

int ISP_EnableSensor(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_en_sensor = enable;
    return 0;
}

int ISP_SWReset(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_sw_rstz = !enable; // low active
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_sw_p1_rstz = !enable; // low active
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_sw_p2_rstz = !enable; // low active
            break;
    }

    return SUCCESS;
}

int ISP_SensorSWReset(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
#if 0 //change to VIF
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_sw_rstz = !enable;
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_sw_p1_rstz = !enable;
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp0_cfg->reg_sensor_sw_p2_rstz = !enable;
            break;
    }
#endif
    pr_info("[ISP]Sensor reset not support.\n");
    return SUCCESS;
}

int ISP_SensorPowerDown(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    //_ISP_HAL(hnd)->isp0_cfg->reg_sen_pwdn = enable;
    pr_info("[ISP]Sensor pdwn not support.\n");
    return SUCCESS;
}

int ISP_SensorEnableMCLK(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_en_sensor_clk = enable; //no used.
    return SUCCESS;
}

int ISP_EnableDblBuf(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
    isp_handle_t *handle = _ISP_HAL(hnd);

    switch(isp_pipe){
        case ISP_PIPE_0:
            if(enable){
                handle->isp0_cfg->reg_load_reg = 0; //1:Disable double buffer
                handle->isp0_cfg->reg_db_batch_mode = 0;
            }
            else{
                handle->isp0_cfg->reg_load_reg = 1; //1:Disable double buffer
                handle->isp0_cfg->reg_db_batch_mode = 0;
            }
            break;
        case ISP_PIPE_1:
            if(enable){
                handle->isp11_cfg->reg_load_reg = 0; //1:Disable double buffer
                handle->isp11_cfg->reg_db_batch_mode = 0;
            }
            else{
                handle->isp11_cfg->reg_load_reg = 1; //1:Disable double buffer
                handle->isp11_cfg->reg_db_batch_mode = 0;
            }
            break;
        case ISP_PIPE_2:
            if(enable){
                handle->isp12_cfg->reg_load_reg = 0; //1:Disable double buffer
                handle->isp12_cfg->reg_db_batch_mode = 0;
            }
            else{
                handle->isp12_cfg->reg_load_reg = 1; //1:Disable double buffer
                handle->isp12_cfg->reg_db_batch_mode = 0;
            }
            break;
    }
    return SUCCESS;
}

int ISP_TriggerDblBuf(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe)
{
    print_kmsg("DBL_BUF_TR");
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_db_batch_done = 1; //trigger double buffer
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp11_cfg->reg_db_batch_done = 1; //trigger double buffer
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp12_cfg->reg_db_batch_done = 1; //trigger double buffer
            break;
    }
    return SUCCESS;
}

#if 1
int ISP_3DSensorReset(ISP_HAL_HANDLE hnd, int idx, MS_BOOL reset)
{
    if(idx == 1)
        _ISP_HAL(hnd)->isp0_cfg->reg_sensor1_rst = reset;
    else
        _ISP_HAL(hnd)->isp0_cfg->reg_sensor2_rst = reset;
    return SUCCESS;
}

int ISP_3DSensorPowerOff(ISP_HAL_HANDLE hnd, int idx, MS_BOOL powerdn)
{
    if(idx == 1)
        _ISP_HAL(hnd)->isp0_cfg->reg_sensor1_pwrdn = powerdn;
    else
        _ISP_HAL(hnd)->isp0_cfg->reg_sensor2_pwrdn = powerdn;
    return SUCCESS;
}
#endif
int ISP_InputEnable(ISP_HAL_HANDLE hnd, MS_BOOL en)
{
#if 0 //sync with V-blanking
    isp_ioctl_fifo_mask ctl;
    ctl.enable = ~en;
    ioctl(_this->isp_dev, IOCTL_ISP_SET_FIFO_MASK, &ctl);
#else //async
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_mask = ~en; //please remove this line
#endif
    return SUCCESS;
}

#if 0
//////////////////// VS block function /////////////////////
// video stabilizer
int ISP_EIS_Enable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_vdos_en = enable;
    return 0;
}

int ISP_EIS_GetStatus(ISP_HAL_HANDLE hnd, MS_BOOL* benable)
{
    *benable = _ISP_HAL(hnd)->isp0_cfg->reg_vdos_en;
    return 0;
}

int ISP_EIS_Config(ISP_HAL_HANDLE hnd, EIS_CFG strCFG)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp0_cfg->reg_vdos_en           = strCFG.eis_en;
    switch(strCFG.eis_data_fmt)
    {
        case 0:
            handle->isp0_cfg->reg_vdos_yc_en        = 0;
            handle->isp0_cfg->reg_vdos_byer_mode    = 0;
            break;
        case 1:
            handle->isp0_cfg->reg_vdos_yc_en        = 0;
            handle->isp0_cfg->reg_vdos_byer_mode    = 1;
            break;
        case 2:
            handle->isp0_cfg->reg_vdos_yc_en        = 1;
            break;
    }
    handle->isp0_cfg->reg_vdos_sblock_blk   = MIN(3, strCFG.eis_sblock_blk);
    handle->isp0_cfg->reg_vdos_dummy        = MIN(255, strCFG.eis_dummy);
    handle->isp0_cfg->reg_vdos_x_st         = MIN(8095, strCFG.eis_x_st);
    handle->isp0_cfg->reg_vdos_y_st         = MIN(4047, strCFG.eis_y_st);
    handle->isp0_cfg->reg_vdos_sblock_width = MIN(4047, strCFG.eis_sblock_width);
    handle->isp0_cfg->reg_vdos_sblock_height = MIN(4047, strCFG.eis_sblock_height);
    return SUCCESS;
}
#endif

///////////// AE function ///////////////////////////
int ISP_AE_SetIntBlockRowNum(ISP_HAL_HANDLE hnd, volatile u32 u4Num)
{
    _ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_ae_int_row_num = LIMIT_BITS(u4Num, 7);
    return SUCCESS;
}

int ISP_AE_SetSubWinCfg(ISP_HAL_HANDLE hnd, ISP_AE_SUBWIN_IDX idx, ISP_AE_SUBWIN win)
{
#if 0
    ISP_DMSG("[%s] Enable:%d, offset:%d,%d, size:%d,%d\n", __FUNCTION__,
             win.bEnable,
             win.u2Stawin_x_offset,
             win.u2Stawin_y_offset,
             win.u2Stawin_x_size,
             win.u2Stawin_y_size);

    switch(idx)
    {
        case eSUBWIN_0:
            handle->stats_alsc_dnr_cfg->reg_ae_win0_en = win.bEnable;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_x_offset_0    = win.u2Stawin_x_offset;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_x_size_m1_0   = win.u2Stawin_x_size - 1;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_y_offset_0    = win.u2Stawin_y_offset;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_y_size_m1_0   = win.u2Stawin_y_size - 1;
            break;
        case eSUBWIN_1:
            handle->stats_alsc_dnr_cfg->reg_ae_win1_en = win.bEnable;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_x_offset_1    = win.u2Stawin_x_offset;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_x_size_m1_1   = win.u2Stawin_x_size - 1;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_y_offset_1    = win.u2Stawin_y_offset;
            handle->stats_alsc_dnr_cfg->reg_ae_stawin_y_size_m1_1   = win.u2Stawin_y_size - 1;
            break;
    }
#endif
    return SUCCESS;
}

int ISP_AE_SetDgain(ISP_HAL_HANDLE hnd, int enable, int dgain)
{
    _ISP_HAL(hnd)->isp1_cfg->reg_isp_ae_en = enable ? 1 : 0;
    _ISP_HAL(hnd)->isp1_cfg->reg_isp_ae_gain = dgain;
    return SUCCESS;
}

int ISP_AE_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable) {
  _ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_isp_statis_en = enable;
  _ISP_HAL(hnd)->isp3_cfg->reg_isp_statis_ae_en = enable;

  return SUCCESS;
}

int ISP_AE_SetWin(ISP_HAL_HANDLE hnd, ISP_AE_WIN win) {
  isp_hal_handle *handle = _ISP_HAL(hnd);
  u32 GbR = 0;
  u32 GbG = 0;
  u32 GbB = 0;
  u32 BR = 0;
  u32 BB = 0;
  u32 BG = 0;

  GbR = (((win.u2BlkSize_x + 1 ) >> 1)  *  ((win.u2BlkSize_y ) >> 1));
  GbB = (((win.u2BlkSize_x ) >> 1)  *  ((win.u2BlkSize_y + 1 ) >> 1));
  GbG = (win.u2BlkSize_x)*(win.u2BlkSize_y) - GbR - GbB;

  BR = (((win.u2BlkSize_x ) >> 1)  *  ((win.u2BlkSize_y ) >> 1));
  BB = (((win.u2BlkSize_x + 1) >> 1)  *  ((win.u2BlkSize_y + 1) >> 1));
  BG = (win.u2BlkSize_x)*(win.u2BlkSize_y) - BR - BB;

  handle->isp3_cfg->reg_ae_div_a_m1 = (65536*100/GbR +50)/100- 1;
  handle->isp3_cfg->reg_ae_div_b_m1 = (65536*100/GbG +50)/100- 1;
  handle->isp3_cfg->reg_ae_div_c_m1 = (65536*100/GbB +50)/100- 1;
  handle->isp3_cfg->reg_ae_div_d_m1 = (65536*100/BR +50)/100- 1;
  handle->isp3_cfg->reg_ae_div_e_m1 = (65536*100/BG +50)/100- 1;
  handle->isp3_cfg->reg_ae_div_f_m1 = (65536*100/BB +50)/100- 1;


  handle->isp3_cfg->reg_ae_blksize_x_m1  = win.u2BlkSize_x - 1;
  handle->isp3_cfg->reg_ae_blksize_y_m1  = win.u2BlkSize_y - 1;
  handle->isp3_cfg->reg_ae_blknum_x_m1   = win.u2BlkNum_x - 1;//max(win.u2BlkNum_x - 1, 9);
  handle->isp3_cfg->reg_ae_blknum_y_m1   = win.u2BlkNum_y - 1;//max(win.u2BlkNum_y - 1, 7);

#if 0
  UartSendTrace("----------------------------------\n");
  UartSendTrace("win.u2BlkSize_x:%d\n",win.u2BlkSize_x);
  UartSendTrace("win.u2BlkSize_y:%d\n",win.u2BlkSize_y);
  UartSendTrace("win.u2BlkNum_x:%d\n",win.u2BlkNum_x);
  UartSendTrace("win.u2BlkNum_y:%d\n",win.u2BlkNum_y);


  UartSendTrace("GbR:%d\n",GbR);
  UartSendTrace("GbG:%d\n",GbG);
  UartSendTrace("GbB:%d\n",GbB);
  UartSendTrace("BR:%d\n",BR);
  UartSendTrace("BB:%d\n",BB);
  UartSendTrace("BG:%d\n",BG);

  UartSendTrace("handle->isp3_cfg->reg_ae_div_a_m1:%d\n",handle->isp3_cfg->reg_ae_div_a_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_div_b_m1:%d\n",handle->isp3_cfg->reg_ae_div_b_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_div_c_m1:%d\n",handle->isp3_cfg->reg_ae_div_c_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_div_d_m1:%d\n",handle->isp3_cfg->reg_ae_div_d_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_div_e_m1:%d\n",handle->isp3_cfg->reg_ae_div_e_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_div_f_m1:%d\n",handle->isp3_cfg->reg_ae_div_f_m1);

  UartSendTrace("handle->isp3_cfg->reg_ae_blksize_x_m1:%d\n",handle->isp3_cfg->reg_ae_blksize_x_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_blksize_y_m1:%d\n",handle->isp3_cfg->reg_ae_blksize_y_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_blknum_x_m1:%d\n",handle->isp3_cfg->reg_ae_blknum_x_m1);
  UartSendTrace("handle->isp3_cfg->reg_ae_blknum_y_m1:%d\n",handle->isp3_cfg->reg_ae_blknum_y_m1);
  UartSendTrace("----------------------------------\n");
#endif

  handle->isp3_cfg->reg_ae_acc_sft = 0;
  return SUCCESS;
}

int ISP_AE_GetWin(ISP_HAL_HANDLE hnd, ISP_AEAWB_WIN *win)
{
  isp_hal_handle *handle = _ISP_HAL(hnd);
  win->u2BlkNum_x = handle->stats_alsc_dnr_cfg->reg_ae_blknum_x_m1 + 1;
  win->u2BlkNum_y = handle->stats_alsc_dnr_cfg->reg_ae_blknum_y_m1 + 1;
  win->u2BlkSize_x= handle->stats_alsc_dnr_cfg->reg_ae_blksize_x_m1 + 1;
  win->u2BlkSize_y= handle->stats_alsc_dnr_cfg->reg_ae_blksize_y_m1 + 1;
  return SUCCESS;
}

int ISP_AE_GetStats(ISP_HAL_HANDLE hnd, ISP_AE_STATIS *stats)
{
  //isp_hal_handle *handle = _ISP_HAL(hnd);
  return SUCCESS;
}

int ISP_AE_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);

    addr = addr >> 4;

    addr &=~(0x2000000);

    handle->isp3_cfg->reg_ae_statis_base = (addr & 0xffff);
    handle->isp3_cfg->reg_ae_statis_base_1= (addr >> 16) & 0x7ff;

    return SUCCESS;
}
///////////// AWB function //////////////////////////
int ISP_AWB_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable) {
  _ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_isp_statis_en = enable;
  _ISP_HAL(hnd)->isp3_cfg->reg_isp_statis_awb_en = enable;
    return SUCCESS;
}

int ISP_AEAWB_StatsMode(ISP_HAL_HANDLE hnd, ISP_AEAWB_STATS_MODE mode)
{
    _ISP_HAL(hnd)->stats_alsc_dnr_cfg->reg_statis_once_trig = mode;
    return SUCCESS;
}

int ISP_AWB_SetWin(ISP_HAL_HANDLE hnd, ISP_CROP_WIN imagesize, ISP_AWB_WIN win)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    u32 blksizeX, blksizeY;
    u32 blkvalidX, blkvalidY;
    u32 div_1, div_2;
    u32 pixnumRB, pixnumG;
    u32 imgWidth, imgHeight;
    s32 startX, startY;

    imgWidth = imagesize.width + 1;
    imgHeight = imagesize.height + 1;

    blksizeX = (imagesize.width + 1) / win.u2BlkNum_x;
    blksizeY = (imagesize.height + 1) / win.u2BlkNum_y;

    startX = (imgWidth - blksizeX * win.u2BlkNum_x) / 2;
    startY = (imgHeight - blksizeY * win.u2BlkNum_y) / 2;

    if(startX < 0)
        startX = 0;
    if(startY < 0)
        startY = 0;

    blkvalidX = minmax(win.u2BlkVaild_x, 4, blksizeX);
    blkvalidY = minmax(win.u2BlkVaild_y, 2, blksizeY);

    blkvalidX = blkvalidX / 2 * 2;
    blkvalidY = blkvalidY / 2 * 2;

    pixnumRB = blkvalidX * blkvalidY / 4;
    pixnumG = blkvalidX * blkvalidY / 2;
    div_1 = (65536 / pixnumRB) - 1;
    div_2 = (65536 / pixnumG) - 1;

    //UartSendTrace("[%s] sx=%d,sy=%d, imgX=%d, imgY=%d, blkX=%d, blkY=%d, validX=%d, validY=%d, pixRB=%d,pixG=%d,div_1=%d, div_2=%d\n",
    //         __FUNCTION__, startX, startY, imgWidth, imgHeight, blksizeX, blksizeY, blkvalidX, blkvalidY, pixnumRB, pixnumG, div_1, div_2);

    handle->isp3_cfg->reg_awb_blk_h_offset = startX;
    handle->isp3_cfg->reg_awb_blk_v_offset = startY;

    handle->isp3_cfg->reg_awb_blknum_x_m1  = win.u2BlkNum_x - 1;//127;//max(win.u2BlkNum_x - 1, 9);
    handle->isp3_cfg->reg_awb_blknum_y_m1  = win.u2BlkNum_y - 1;//max(win.u2BlkNum_y - 1, 7);
    handle->isp3_cfg->reg_awb_blksize_x_m1 = blksizeX - 1;//win.u2BlkSize_x - 1;
    handle->isp3_cfg->reg_awb_blksize_y_m1 = blksizeY - 1;//win.u2BlkSize_y - 1;

    handle->isp3_cfg->reg_awb_div_a_m1 = div_1;
    handle->isp3_cfg->reg_awb_div_b_m1 = div_2;
    handle->isp3_cfg->reg_awb_div_c_m1 = div_1;
    handle->isp3_cfg->reg_awb_div_d_m1 = div_1;
    handle->isp3_cfg->reg_awb_div_e_m1 = div_2;
    handle->isp3_cfg->reg_awb_div_f_m1 = div_1;
    handle->isp3_cfg->reg_awb_blk_valid_x_m1 = blkvalidX - 1;
    handle->isp3_cfg->reg_awb_blk_valid_y_m1 = blkvalidY - 1;

    handle->AwbWin.u2BlkNum_x = win.u2BlkNum_x;
    handle->AwbWin.u2BlkNum_y = win.u2BlkNum_y;
    handle->AwbWin.u2BlkVaild_x = blkvalidX;
    handle->AwbWin.u2BlkVaild_y = blkvalidY;
#if 0
    ISP_DMSG("[%s] ======h=%d,v=%d,numX=%d,numY=%d,blkX=%d,blkY=%d,validX=%d,validY=%d, div_a=%d,div_b=%d,div_c=%d,div_d=%d,div_e=%d,div_f=%d\n",
             __FUNCTION__, handle->isp3_cfg->reg_awb_blk_h_offset, handle->isp3_cfg->reg_awb_blk_v_offset,
             handle->isp3_cfg->reg_awb_blknum_x_m1, handle->isp3_cfg->reg_awb_blknum_y_m1,
             handle->isp3_cfg->reg_awb_blksize_x_m1, handle->isp3_cfg->reg_awb_blksize_y_m1,
             handle->isp3_cfg->reg_awb_blk_valid_x_m1, handle->isp3_cfg->reg_awb_blk_valid_y_m1,
             handle->isp3_cfg->reg_awb_div_a_m1, handle->isp3_cfg->reg_awb_div_b_m1,
             handle->isp3_cfg->reg_awb_div_c_m1, handle->isp3_cfg->reg_awb_div_d_m1,
             handle->isp3_cfg->reg_awb_div_e_m1, handle->isp3_cfg->reg_awb_div_f_m1);
#endif
  return SUCCESS;
}


int ISP_AWB_SourceSelection(ISP_HAL_HANDLE hnd, volatile ISP_AWB_SOURCE_SELECTION eSource)
{
    //AWB statistics output location
    // 1: before DNR with LSC
    // 2: before DNR with ALSC
    // 3: before DNR with LSC and ALSC
    // 8: after  DNR with LSC and ALSC
    _ISP_HAL(hnd)->isp3_cfg->reg_isp_awb_in_mode = MIN(8, eSource);
    return SUCCESS;
}

int ISP_AWB_GetStats(ISP_HAL_HANDLE hnd, ISP_AWB_STATIS *stats)
{
  //isp_hal_handle *handle = _ISP_HAL(hnd);

  return SUCCESS;
}

int ISP_AWB_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);

    addr = addr >> 4;

    addr &=~(0x2000000);

    handle->isp3_cfg->reg_awb_statis_base = (addr & 0xffff);
    handle->isp3_cfg->reg_awb_statis_base_1= (addr >> 16) & 0x7ff;

    return SUCCESS;
}

///////////// AF function ///////////////////////////
int ISP_AF_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp5_cfg->reg_high_image_pipe_en = enable;
    handle->isp5_cfg->reg_low_image_pipe_en = enable;
    handle->isp5_cfg->reg_luma_image_pipe_en = enable;
    handle->isp5_cfg->reg_sobelv_en = enable;
    handle->isp5_cfg->reg_sobelh_en = enable;
    handle->isp5_cfg->reg_high_iir_mem_frame_clr = 1;
    handle->isp5_cfg->reg_low_iir_mem_frame_clr = 1;

    if(enable)
    {
        handle->isp2_cfg->reg_dpc_en = 1;

        handle->isp5_cfg->reg_low_1st_low_clip = 0;
        handle->isp5_cfg->reg_low_1st_high_clip = 1023;

        handle->isp5_cfg->reg_low_2nd_low_clip = 0;
        handle->isp5_cfg->reg_low_2nd_high_clip = 1023;

        handle->isp5_cfg->reg_luma_low_clip = 0;
        handle->isp5_cfg->reg_luma_high_clip = 1023;

        handle->isp5_cfg->reg_sobelv_1st_low_clip = 0;
        handle->isp5_cfg->reg_sobelv_1st_high_clip = 1023;

        handle->isp5_cfg->reg_sobelv_2nd_low_clip = 0;
        handle->isp5_cfg->reg_sobelv_2nd_high_clip = 1023;


        handle->isp5_cfg->reg_sobelh_1st_low_clip = 0;
        handle->isp5_cfg->reg_sobelh_1st_high_clip = 1023;

        handle->isp5_cfg->reg_sobelh_2nd_low_clip = 0;
        handle->isp5_cfg->reg_sobelh_2nd_high_clip = 1023;

        handle->isp5_cfg->reg_af_kr_value = 76;
        handle->isp5_cfg->reg_af_kgr_value = 75;
        handle->isp5_cfg->reg_af_kgb_value = 75;
        handle->isp5_cfg->reg_af_kb_value = 29;

        handle->isp5_cfg->reg_low_iir_b0 = 43;
        handle->isp5_cfg->reg_low_iir_b1 = 86;
        handle->isp5_cfg->reg_low_iir_b2 = 43;
        handle->isp5_cfg->reg_low_iir_a1 = 8;
        handle->isp5_cfg->reg_low_iir_a2 = 8;
    }


    return SUCCESS;
}

int ISP_AF_SetStatsMode(ISP_HAL_HANDLE hnd, ISP_AF_STATS_MODE mode)
{
#if 0
    switch(mode)
    {
        case AF_MODE_CONT:
            handle->af_cfg->af_cfg.reg_af_sta_mode = 0;
            handle->af_cfg->af_cfg.reg_af_once_trig = 0;
            break;
        case AF_MODE_ONCE:
            handle->af_cfg->af_cfg.reg_af_sta_mode = 1;
            handle->af_cfg->af_cfg.reg_af_once_trig = 1;
            break;
    }
#endif
    return SUCCESS;
}

int ISP_AF_GetNumWins(ISP_HAL_HANDLE hnd, volatile int *num)
{
    *num = 5;
    return SUCCESS;
}

int ISP_AF_SetWins(ISP_HAL_HANDLE hnd, ISP_AF_WIN win, int idx)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    u32 af_x_start = win.x_start >> 1;  // div 2 for y domain transfer
    u32 af_x_end = win.x_end >> 1;
    u32 af_y_start = win.y_start >> 1;
    u32 af_y_end = win.y_end >> 1;

    if(idx == 0)
    {
        handle->isp5_cfg->reg_high_af0_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af0_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af0_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af0_v_end = af_y_end;
    }
    else if(idx == 1)
    {
        handle->isp5_cfg->reg_high_af1_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af1_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af1_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af1_v_end = af_y_end;
    }
    else if(idx == 2)
    {
        handle->isp5_cfg->reg_high_af2_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af2_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af2_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af2_v_end = af_y_end;
    }
    else if(idx == 3)
    {
        handle->isp5_cfg->reg_high_af3_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af3_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af3_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af3_v_end = af_y_end;
    }
    else if(idx == 4)
    {
        handle->isp5_cfg->reg_high_af4_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af4_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af4_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af4_v_end = af_y_end;
    }
    else if(idx == 5)
    {
        handle->isp5_cfg->reg_high_af5_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af5_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af5_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af5_v_end = af_y_end;
    }
    else if(idx == 6)
    {
        handle->isp5_cfg->reg_high_af6_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af6_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af6_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af6_v_end = af_y_end;
    }
    else if(idx == 7)
    {
        handle->isp5_cfg->reg_high_af7_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af7_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af7_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af7_v_end = af_y_end;
    }
    else if(idx == 8)
    {
        handle->isp5_cfg->reg_high_af8_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af8_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af8_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af8_v_end = af_y_end;
    }
    else if(idx == 9)
    {
        handle->isp5_cfg->reg_high_af9_h_start = af_x_start;
        handle->isp5_cfg->reg_high_af9_v_start = af_y_start;
        handle->isp5_cfg->reg_high_af9_h_end = af_x_end;
        handle->isp5_cfg->reg_high_af9_v_end = af_y_end;
    }
    return SUCCESS;
}

int ISP_AF_GetWins(ISP_HAL_HANDLE hnd, volatile ISP_AF_WIN *win, int idx)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    if(idx == 0)
    {
        win->x_start = handle->isp5_cfg->reg_high_af0_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af0_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af0_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af0_v_end;
    }
    else if(idx == 1)
    {
        win->x_start = handle->isp5_cfg->reg_high_af1_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af1_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af1_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af1_v_end;
    }
    else if(idx == 2)
    {
        win->x_start = handle->isp5_cfg->reg_high_af2_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af2_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af2_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af2_v_end;
    }
    else if(idx == 3)
    {
        win->x_start = handle->isp5_cfg->reg_high_af3_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af3_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af3_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af3_v_end;
    }
    else if(idx == 4)
    {
        win->x_start = handle->isp5_cfg->reg_high_af4_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af4_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af4_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af4_v_end;
    }
    else if(idx == 5)
    {
        win->x_start = handle->isp5_cfg->reg_high_af5_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af5_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af5_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af5_v_end;
    }
    else if(idx == 6)
    {
        win->x_start = handle->isp5_cfg->reg_high_af6_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af6_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af6_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af6_v_end;
    }
    else if(idx == 7)
    {
        win->x_start = handle->isp5_cfg->reg_high_af7_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af7_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af7_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af7_v_end;
    }
    else if(idx == 8)
    {
        win->x_start = handle->isp5_cfg->reg_high_af8_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af8_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af8_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af8_v_end;
    }
    else if(idx == 9)
    {
        win->x_start = handle->isp5_cfg->reg_high_af9_h_start;
        win->y_start = handle->isp5_cfg->reg_high_af9_v_start;
        win->x_end = handle->isp5_cfg->reg_high_af9_h_end;
        win->y_end = handle->isp5_cfg->reg_high_af9_v_end;
    }
    return SUCCESS;
}

int ISP_AF_GetStats(ISP_HAL_HANDLE hnd, volatile ISP_AF_STATS *stats, int idx)
{
    u32 af_stats_dma_addr = 0;
    af_stats_dma_addr = (_ISP_HAL(hnd)->isp5_cfg->reg_af_dma_addr << 11) || _ISP_HAL(hnd)->isp5_cfg->reg_af_dma_addr_1;
    memcpy((void *)stats, (void *)af_stats_dma_addr, sizeof(ISP_AF_STATS));
    return SUCCESS;
}
///////////////// Motion ///////////////////////////
int ISP_MOT_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
#if 0
    _ISP_HAL(hnd)->isp3_cfg->reg_motion_en = enable;
    if(enable)
        _ISP_HAL(hnd)->isp3_cfg->reg_dnr_en = 1;
#endif
    return SUCCESS;
}

int ISP_MOT_SetAttr(ISP_HAL_HANDLE hnd, ISP_MOT_ATTR attr)
{
 /*
    _ISP_HAL(hnd)->isp3_cfg->reg_mot_burst_cnt = attr.burst_cnt;
    _ISP_HAL(hnd)->isp3_cfg->reg_motion_thrd = attr.thrd;
*/
    return SUCCESS;
}

/////////////// Histogram  /////////////////////////
int ISP_HISTO_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable, int idx)
{
    if(idx == 0)
    {
        _ISP_HAL(hnd)->isp3_cfg->reg_histo_win0_en = enable;
    }
    else if(idx == 1)
    {
        _ISP_HAL(hnd)->isp3_cfg->reg_histo_win1_en = enable;
    }
    else
    {
        return FAIL;
    }
    if(enable)
    {
        //_ISP_HAL(hnd)->isp3_cfg->reg_dnr_en = 1;
        _ISP_HAL(hnd)->isp9_cfg->reg_histo_p_en = 1;
    }
    return SUCCESS;
}

int ISP_HISTO_SetAttr(ISP_HAL_HANDLE hnd, const ISP_HISTO_ATTR *attr)
{
    u32 temp1 = 0;
    u32 temp2 = 0;
    u32 temp3 = 0;
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp3_cfg->reg_histo_acc_sft = attr->acc_sft;
    handle->isp3_cfg->reg_histo_blksize_x_m1 = attr->u2BlkSize_x - 1;
    handle->isp3_cfg->reg_histo_blksize_y_m1 = attr->u2BlkSize_y - 1;
    handle->isp3_cfg->reg_histo_blknum_x_m1 = attr->u2BlkNum_x - 1;
    handle->isp3_cfg->reg_histo_blknum_y_m1 = attr->u2BlkNum_y - 1;
    handle->isp3_cfg->reg_histo_blk_h_offset = attr->u2Blk_h_offset;
    handle->isp3_cfg->reg_histo_blk_v_offset = attr->u2Blk_v_offset;

    temp1 = ((attr->u2BlkSize_x + 1) / 2) * (attr->u2BlkSize_y / 2) ;
    temp3 = (attr->u2BlkSize_x / 2) * ((attr->u2BlkSize_y + 1) / 2) ;
    temp2 = attr->u2BlkSize_x * attr->u2BlkSize_y - temp1 - temp3;
    handle->isp3_cfg->reg_histo_div_a_m1 = (65536  + temp1 / 2) / temp1 - 1;
    handle->isp3_cfg->reg_histo_div_b_m1 = (65536  + temp2 / 2) / temp2 - 1;
    handle->isp3_cfg->reg_histo_div_c_m1 = (65536  + temp3 / 2) / temp3 - 1;
    temp1 = (attr->u2BlkSize_x / 2) * (attr->u2BlkSize_y / 2) ;
    temp3 = ((attr->u2BlkSize_x + 1) / 2) * ((attr->u2BlkSize_y + 1) / 2) ;
    temp2 = attr->u2BlkSize_x * attr->u2BlkSize_y - temp1 - temp3;
    handle->isp3_cfg->reg_histo_div_d_m1 = (65536  + temp1 / 2) / temp1 - 1;
    handle->isp3_cfg->reg_histo_div_e_m1 = (65536  + temp2 / 2) / temp2 - 1;
    handle->isp3_cfg->reg_histo_div_f_m1 = (65536  + temp3 / 2) / temp3 - 1;
#if 0
    ISP_DMSG("[%s] acc_sft=%d,BlkSizeX=%d, BlkSizeY=%d, BlkNumX=%d, BlkNumY=%d, hOffset=%d, vOffset=%d, div_a_m1=%d, div_b_m1=%d,div_c_m1=%d, div_d_m1=%d, div_e_m1=%d, div_f_m1=%d\n",
//printf("[%s] acc_sft=%d,BlkSizeX=%d, BlkSizeY=%d, BlkNumX=%d, BlkNumY=%d, hOffset=%d, vOffset=%d, div_a_m1=%d, div_b_m1=%d,div_c_m1=%d, div_d_m1=%d, div_e_m1=%d, div_f_m1=%d\n",
             __FUNCTION__,
             handle->isp3_cfg->reg_histo_acc_sft,
             attr->u2BlkSize_x, attr->u2BlkSize_y,
             attr->u2BlkNum_x, attr->u2BlkNum_y,
             attr->u2Blk_h_offset, attr->u2Blk_v_offset,
             handle->isp3_cfg->reg_histo_div_a_m1,
             handle->isp3_cfg->reg_histo_div_b_m1,
             handle->isp3_cfg->reg_histo_div_c_m1,
             handle->isp3_cfg->reg_histo_div_d_m1,
             handle->isp3_cfg->reg_histo_div_e_m1,
             handle->isp3_cfg->reg_histo_div_f_m1);
#endif
    return SUCCESS;
}

int ISP_HISTO_SetWins(ISP_HAL_HANDLE hnd, const ISP_HISTO_WIN *win, int idx)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    if(idx == 0)
    {
        handle->isp3_cfg->reg_histo_win0_en = 1;
        handle->isp3_cfg->reg_histo_stawin_x_offset_0 = win->u2Stawin_x_offset;
        handle->isp3_cfg->reg_histo_stawin_x_size_m1_0 = win->u2Stawin_x_size - 1;
        handle->isp3_cfg->reg_histo_stawin_y_offset_0 = win->u2Stawin_y_offset;
        handle->isp3_cfg->reg_histo_stawin_y_size_m1_0 = win->u2Stawin_y_size - 1;
        handle->isp3_cfg->reg_histo_roi_0_mode = 1;//win->roi_mode;
    }
    else if(idx == 1)
    {
        handle->isp3_cfg->reg_histo_win1_en = 1;
        handle->isp3_cfg->reg_histo_stawin_x_offset_1 = win->u2Stawin_x_offset;
        handle->isp3_cfg->reg_histo_stawin_x_size_m1_1 = win->u2Stawin_x_size - 1;
        handle->isp3_cfg->reg_histo_stawin_y_offset_1 = win->u2Stawin_y_offset;
        handle->isp3_cfg->reg_histo_stawin_y_size_m1_1 = win->u2Stawin_y_size - 1;
        handle->isp3_cfg->reg_histo_roi_1_mode = 1;//win->roi_mode;
    }
    else
    {
        return FAIL;
    }
    return SUCCESS;
}

int ISP_HISTO_GetWins(ISP_HAL_HANDLE hnd, volatile ISP_HISTO_WIN *win, int idx)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    if(idx == 0)
    {
        win->u2Stawin_x_offset = handle->isp3_cfg->reg_histo_stawin_x_offset_0;
        win->u2Stawin_x_size = handle->isp3_cfg->reg_histo_stawin_x_size_m1_0;
        win->u2Stawin_y_offset = handle->isp3_cfg->reg_histo_stawin_y_offset_0;
        win->u2Stawin_y_size = handle->isp3_cfg->reg_histo_stawin_y_size_m1_0;
        //win->roi_mode = handle->isp3_cfg->reg_histo_roi_0_mode;
    }
    else if(idx == 1)
    {
        win->u2Stawin_x_offset = handle->isp3_cfg->reg_histo_stawin_x_offset_1;
        win->u2Stawin_x_size = handle->isp3_cfg->reg_histo_stawin_x_size_m1_1;
        win->u2Stawin_y_offset = handle->isp3_cfg->reg_histo_stawin_y_offset_1;
        win->u2Stawin_y_size = handle->isp3_cfg->reg_histo_stawin_y_size_m1_1;
        //win->roi_mode = handle->isp3_cfg->reg_histo_roi_1_mode;
    }
    else
    {
        return FAIL;
    }
    return SUCCESS;
}

int ISP_HISTO_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);

    addr = addr >> 4;

    addr &=~(0x2000000);

    handle->isp3_cfg->reg_histo_statis_base = (addr & 0xffff);
    handle->isp3_cfg->reg_histo_statis_base_1= (addr >> 16) & 0x7ff;

    return SUCCESS;
}

////////////// RGBIR histogram ////////////////////////
int ISP_RGBIR_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp2_cfg->reg_dpc_en = enable;
    _ISP_HAL(hnd)->isp8_cfg->reg_rgbir_en = enable;
    _ISP_HAL(hnd)->isp8_cfg->reg_hist_ir_en = enable;
    return SUCCESS;
}

int ISP_RGBIR_SetAttr(ISP_HAL_HANDLE hnd, ISP_RGBIR_ATTR *attr)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp8_cfg->reg_mode4x4 = attr->mode4x4;
    handle->isp0_cfg->reg_isp_rgbir_format = attr->rgbir_format;
    handle->isp8_cfg->reg_ir_dir_scaler = attr->dir_scaler;
    handle->isp8_cfg->reg_ir_oft_cmp_ratio = attr->oft_cmp_ratio;

    handle->isp8_cfg->reg_ir_oft_ratio_by_y_shift_0 = attr->oft_ratio_by_y_shift_0;
    handle->isp8_cfg->reg_ir_oft_ratio_by_y_shift_1 = attr->oft_ratio_by_y_shift_1;
    handle->isp8_cfg->reg_ir_oft_ratio_by_y_shift_2 = attr->oft_ratio_by_y_shift_2;
    handle->isp8_cfg->reg_ir_oft_ratio_by_y_shift_3 = attr->oft_ratio_by_y_shift_3;
    handle->isp8_cfg->reg_ir_oft_ratio_by_y_shift_4 = attr->oft_ratio_by_y_shift_4;

    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_0 = attr->oft_r_ratio_by_y_0 ;
    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_1 = attr->oft_r_ratio_by_y_1 ;
    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_2 = attr->oft_r_ratio_by_y_2 ;
    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_3 = attr->oft_r_ratio_by_y_3 ;
    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_4 = attr->oft_r_ratio_by_y_4 ;
    handle->isp8_cfg->reg_ir_oft_r_ratio_by_y_5 = attr->oft_r_ratio_by_y_5 ;

    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_0 = attr->oft_g_ratio_by_y_0 ;
    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_1 = attr->oft_g_ratio_by_y_1 ;
    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_2 = attr->oft_g_ratio_by_y_2 ;
    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_3 = attr->oft_g_ratio_by_y_3 ;
    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_4 = attr->oft_g_ratio_by_y_4 ;
    handle->isp8_cfg->reg_ir_oft_g_ratio_by_y_5 = attr->oft_g_ratio_by_y_5 ;

    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_0 = attr->oft_b_ratio_by_y_0 ;
    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_1 = attr->oft_b_ratio_by_y_1 ;
    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_2 = attr->oft_b_ratio_by_y_2 ;
    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_3 = attr->oft_b_ratio_by_y_3 ;
    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_4 = attr->oft_b_ratio_by_y_4 ;
    handle->isp8_cfg->reg_ir_oft_b_ratio_by_y_5 = attr->oft_b_ratio_by_y_5 ;

    return SUCCESS;
}

int ISP_RGBIR_SetWins(ISP_HAL_HANDLE hnd, ISP_RGBIR_WIN *win)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    handle->isp8_cfg->reg_hist_offsetx = win->blk_x_offset;
    handle->isp8_cfg->reg_hist_offsety = win->blk_y_offset;
    handle->isp8_cfg->reg_hist_blk_width = win->blk_width;
    handle->isp8_cfg->reg_hist_blk_height = win->blk_height;

    handle->isp8_cfg->reg_roi_offsetx = win->roi_x_offset;
    handle->isp8_cfg->reg_roi_offsety = win->roi_y_offset;
    handle->isp8_cfg->reg_roi_sizex = win->roi_x_size;
    handle->isp8_cfg->reg_roi_sizey = win->roi_y_size;

    handle->isp8_cfg->reg_roi_div = win->roi_div;

    return SUCCESS;
}

int ISP_RGBIR_GetWins(ISP_HAL_HANDLE hnd, ISP_RGBIR_WIN *win)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    win->blk_x_offset = handle->isp8_cfg->reg_hist_offsetx ;
    win->blk_y_offset = handle->isp8_cfg->reg_hist_offsety ;
    win->blk_width = handle->isp8_cfg->reg_hist_blk_width ;
    win->blk_height = handle->isp8_cfg->reg_hist_blk_height ;

    win->roi_x_offset = handle->isp8_cfg->reg_roi_offsetx;
    win->roi_y_offset = handle->isp8_cfg->reg_roi_offsety;
    win->roi_x_size = handle->isp8_cfg->reg_roi_sizex;
    win->roi_y_size = handle->isp8_cfg->reg_roi_sizey;

    win->roi_div = handle->isp8_cfg->reg_roi_div;

    return SUCCESS;
}

/////////////////////// HDR statistic ///////////////////////////////
int ISP_HDR_StatsEnable(ISP_HAL_HANDLE hnd, MS_BOOL enable) {

  _ISP_HAL(hnd)->isp9_cfg->reg_hdr_histo_en = enable;

  return SUCCESS;
}


int ISP_HDR_SetStatsBUF(ISP_HAL_HANDLE hnd,int addr)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);

    addr = addr >> 4;

    addr &=~(0x2000000);

    handle->isp9_cfg->reg_hdr_histo_miu_base_addr = (addr & 0xffff);
    handle->isp9_cfg->reg_hdr_histo_miu_base_addr_1= (addr >> 16) & 0x7ff;

    return SUCCESS;
}

////////////// gated clock ////////////////////////
// gate clk config, isp_gatelevel_cfg
int ISP_ISPDP_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_dp_gated_clk_en = enable;
    return SUCCESS;
}

int ISP_Raw_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_m3isp_rawdn_gated_clk_en = enable;
    return SUCCESS;
}

int ISP_Kernel_GatedClk(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_m3isp_kernel_gated_clk_en = enable;
    return SUCCESS;
}

int ISP_LatchMode(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_latch_mode = enable;    //clock gating
    return SUCCESS;
}

/////////////// path clk control //////////////////
int ISP_DPRstEveryVsync(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_dp_rstz_mode = !enable;
    return SUCCESS;
}

int ISP_DisableRawDNClk(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_m3isp_rawdn_clk_force_dis = enable;
    return SUCCESS;
}

int ISP_DisableISPDPClk(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_dp_clk_force_dis = enable;
    return SUCCESS;
}

int ISP_EnableRawDNWDMAClk(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_dspl2wdma_en = enable;
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp11_cfg->reg_dspl2wdma_en = enable;
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp12_cfg->reg_dspl2wdma_en = enable;
            break;
    }

    return SUCCESS;
}

int ISP_SetAWBDSPMode(ISP_HAL_HANDLE hnd, ISP_AWBDSP_Mode mode)
{
    //G2E
    _ISP_HAL(hnd)->isp0_cfg->reg_dsp_mode = mode;
    return SUCCESS;
}

////////////// irq control ////////////////////////
int ISP_GetSenLineCnt(ISP_HAL_HANDLE hnd, int *cnt)
{
    *cnt = _ISP_HAL(hnd)->isp0_cfg->reg_isp_sensor_line_cnt;
    return SUCCESS;
}

int ISP_isISPBusy(ISP_HAL_HANDLE hnd)
{
    return _ISP_HAL(hnd)->isp0_cfg->reg_isp_busy;
}

int ISP_isFIFOFull(ISP_HAL_HANDLE hnd)
{
    return _ISP_HAL(hnd)->isp0_cfg->reg_isp_fifo_ful;
}

int ISP_SetIspOutputLineCountInt(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, volatile u32 Line)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_sw_specify_int_line = LIMIT_BITS(Line, 12);
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp11_cfg->reg_sw_specify_int_line = LIMIT_BITS(Line, 12);
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp12_cfg->reg_sw_specify_int_line = LIMIT_BITS(Line, 12);
            break;
    }

    return SUCCESS;
}

int ISP_SetLineCountInt0(ISP_HAL_HANDLE hnd, u32 Line)
{
    _ISP_HAL(hnd)->isp1_cfg->reg_isp_obc_line_cnt1 = LIMIT_BITS(Line, 12);
    return SUCCESS;
}

int ISP_SetLineCountInt1(ISP_HAL_HANDLE hnd, u32 Line)
{
    _ISP_HAL(hnd)->isp1_cfg->reg_isp_obc_line_cnt2 = LIMIT_BITS(Line, 12);
    return SUCCESS;
}

int ISP_SetLineCountInt2(ISP_HAL_HANDLE hnd, u32 Line)
{
    _ISP_HAL(hnd)->isp1_cfg->reg_isp_obc_line_cnt3 = LIMIT_BITS(Line, 12);
    return SUCCESS;
}

inline void ISP_SelDbgOut(ISP_HAL_HANDLE hnd, ISP_DBG_SEL mode)
{
    _ISP_HAL(hnd)->isp0_cfg->reg_isp_debug_sel = mode;
}

inline u32 ISP_GetDgbOut(ISP_HAL_HANDLE hnd)
{
    return _ISP_HAL(hnd)->isp0_cfg->reg_isp_debug | (_ISP_HAL(hnd)->isp0_cfg->reg_isp_debug_1 << 16);
}

////////////// data path control //////////////////
int HalIsp_InitReg(ISP_HAL_HANDLE hnd)
{
    // enable RGB to YUV422
    isp_hal_handle *handle = _ISP_HAL(hnd);
    int i;
    u32 u4DefaultH[33] =
    {
        //coef0   coef1   coef2   coef3
        0x036 | (0x97 << 8) | (0x36 << 16) | (0xffd << 24),
        0x034 | (0x97 << 8) | (0x38 << 16) | (0xffd << 24),
        0x032 | (0x97 << 8) | (0x3a << 16) | (0xffd << 24),
        0x030 | (0x97 << 8) | (0x3c << 16) | (0xffd << 24),
        0x02e | (0x97 << 8) | (0x3f << 16) | (0xffc << 24),
        0x02d | (0x97 << 8) | (0x3f << 16) | (0xffd << 24),
        0x02b | (0x96 << 8) | (0x42 << 16) | (0xffd << 24),
        0x029 | (0x96 << 8) | (0x44 << 16) | (0xffd << 24),
        0x027 | (0x96 << 8) | (0x46 << 16) | (0xffd << 24),
        0x025 | (0x95 << 8) | (0x49 << 16) | (0xffd << 24),
        0x024 | (0x94 << 8) | (0x4b << 16) | (0xffd << 24),
        0x022 | (0x94 << 8) | (0x4d << 16) | (0xffd << 24),
        0x020 | (0x93 << 8) | (0x50 << 16) | (0xffd << 24),
        0x01f | (0x92 << 8) | (0x52 << 16) | (0xffd << 24),
        0x01d | (0x92 << 8) | (0x53 << 16) | (0xffe << 24),
        0x01c | (0x91 << 8) | (0x55 << 16) | (0xffe << 24),
        0x01a | (0x90 << 8) | (0x58 << 16) | (0xffe << 24),
        0x019 | (0x8f << 8) | (0x5a << 16) | (0xffe << 24),
        0x017 | (0x8e << 8) | (0x5c << 16) | (0xfff << 24),
        0x016 | (0x8c << 8) | (0x5f << 16) | (0xfff << 24),
        0x015 | (0x8b << 8) | (0x60 << 16) | (0x000 << 24),
        0x013 | (0x8a << 8) | (0x63 << 16) | (0x000 << 24),
        0x012 | (0x89 << 8) | (0x64 << 16) | (0x001 << 24),
        0x011 | (0x87 << 8) | (0x67 << 16) | (0x001 << 24),
        0x010 | (0x86 << 8) | (0x68 << 16) | (0x002 << 24),
        0x00e | (0x84 << 8) | (0x6c << 16) | (0x002 << 24),
        0x00d | (0x83 << 8) | (0x6d << 16) | (0x003 << 24),
        0x00c | (0x81 << 8) | (0x6f << 16) | (0x004 << 24),
        0x00b | (0x80 << 8) | (0x71 << 16) | (0x004 << 24),
        0x00a | (0x7e << 8) | (0x73 << 16) | (0x005 << 24),
        0x009 | (0x7c << 8) | (0x75 << 16) | (0x006 << 24),
        0x008 | (0x7a << 8) | (0x77 << 16) | (0x007 << 24),
        0x008 | (0x79 << 8) | (0x78 << 16) | (0x007 << 24),
    };

    u32 u4DefaultV[33] =
    {
        //coef0   coef1   coef2   coef3
        0x036 | (0x97 << 8) | (0x36 << 16) | (0xffd << 24),
        0x034 | (0x97 << 8) | (0x38 << 16) | (0xffd << 24),
        0x032 | (0x97 << 8) | (0x3a << 16) | (0xffd << 24),
        0x030 | (0x97 << 8) | (0x3c << 16) | (0xffd << 24),
        0x02e | (0x97 << 8) | (0x3f << 16) | (0xffc << 24),
        0x02d | (0x97 << 8) | (0x3f << 16) | (0xffd << 24),
        0x02b | (0x96 << 8) | (0x42 << 16) | (0xffd << 24),
        0x029 | (0x96 << 8) | (0x44 << 16) | (0xffd << 24),
        0x027 | (0x96 << 8) | (0x46 << 16) | (0xffd << 24),
        0x025 | (0x95 << 8) | (0x49 << 16) | (0xffd << 24),
        0x024 | (0x94 << 8) | (0x4b << 16) | (0xffd << 24),
        0x022 | (0x94 << 8) | (0x4d << 16) | (0xffd << 24),
        0x020 | (0x93 << 8) | (0x50 << 16) | (0xffd << 24),
        0x01f | (0x92 << 8) | (0x52 << 16) | (0xffd << 24),
        0x01d | (0x92 << 8) | (0x53 << 16) | (0xffe << 24),
        0x01c | (0x91 << 8) | (0x55 << 16) | (0xffe << 24),
        0x01a | (0x90 << 8) | (0x58 << 16) | (0xffe << 24),
        0x019 | (0x8f << 8) | (0x5a << 16) | (0xffe << 24),
        0x017 | (0x8e << 8) | (0x5c << 16) | (0xfff << 24),
        0x016 | (0x8c << 8) | (0x5f << 16) | (0xfff << 24),
        0x015 | (0x8b << 8) | (0x60 << 16) | (0x000 << 24),
        0x013 | (0x8a << 8) | (0x63 << 16) | (0x000 << 24),
        0x012 | (0x89 << 8) | (0x64 << 16) | (0x001 << 24),
        0x011 | (0x87 << 8) | (0x67 << 16) | (0x001 << 24),
        0x010 | (0x86 << 8) | (0x68 << 16) | (0x002 << 24),
        0x00e | (0x84 << 8) | (0x6c << 16) | (0x002 << 24),
        0x00d | (0x83 << 8) | (0x6d << 16) | (0x003 << 24),
        0x00c | (0x81 << 8) | (0x6f << 16) | (0x004 << 24),
        0x00b | (0x80 << 8) | (0x71 << 16) | (0x004 << 24),
        0x00a | (0x7e << 8) | (0x73 << 16) | (0x005 << 24),
        0x009 | (0x7c << 8) | (0x75 << 16) | (0x006 << 24),
        0x008 | (0x7a << 8) | (0x77 << 16) | (0x007 << 24),
        0x008 | (0x79 << 8) | (0x78 << 16) | (0x007 << 24),
    };

    // set ALSC interpolation coeff
    handle->isp3_cfg->reg_coef_buf_sel_v    = 0;
    for(i = 0; i < 33; i++)
    {
        handle->isp3_cfg->reg_coef_buf_wd_1 = u4DefaultH[i] >> 16;
        handle->isp3_cfg->reg_coef_buf_wd   = u4DefaultH[i] & 0x0ffff;
        handle->isp3_cfg->reg_coef_buf_adr  = i;
        handle->isp3_cfg->reg_coef_buf_we   = true;
    }

    handle->isp3_cfg->reg_coef_buf_sel_v    = 1;
    for(i = 0; i < 33; i++)
    {
        handle->isp3_cfg->reg_coef_buf_wd_1 = u4DefaultV[i] >> 16;
        handle->isp3_cfg->reg_coef_buf_wd   = u4DefaultV[i] & 0x0ffff;
        handle->isp3_cfg->reg_coef_buf_adr  = i;
        handle->isp3_cfg->reg_coef_buf_we   = true;
    }
    handle->isp3_cfg->reg_coef_buf_re       = 0;

    handle->isp0_cfg->reg_c_filter = 4;
    handle->isp0_cfg->reg_isp_r2y_cmc_en = 1;
    handle->isp0_cfg->reg_isp_r2y_y_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_r_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_b_sub_16_en = 0;
    handle->isp0_cfg->reg_isp_r2y_y_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_r_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_b_add_16_post_en = 0;
    handle->isp0_cfg->reg_isp_r2y_cb_add_128_post_en = 1;
    handle->isp0_cfg->reg_isp_r2y_cr_add_128_post_en = 1;
    handle->isp0_cfg->reg_isp_r2y_rran = 0;
    handle->isp0_cfg->reg_isp_r2y_gran = 0;
    handle->isp0_cfg->reg_isp_r2y_bran = 0;
    handle->isp0_cfg->reg_isp_r2y_coeff_11 = 512;
    handle->isp0_cfg->reg_isp_r2y_coeff_12 = 7763;
    handle->isp0_cfg->reg_isp_r2y_coeff_13 = 8109;
    handle->isp0_cfg->reg_isp_r2y_coeff_21 = 306;
    handle->isp0_cfg->reg_isp_r2y_coeff_22 = 601;
    handle->isp0_cfg->reg_isp_r2y_coeff_23 = 117;
    handle->isp0_cfg->reg_isp_r2y_coeff_31 = 8019;
    handle->isp0_cfg->reg_isp_r2y_coeff_32 = 7853;
    handle->isp0_cfg->reg_isp_r2y_coeff_33 = 512;

    //CFAI
    handle->isp0_cfg->reg_demosaic_dvh_slope   = 0x7;
    handle->isp0_cfg->reg_demosaic_dvh_thrd    = 0xd;
    handle->isp0_cfg->reg_demosaic_cpxwei_slope = 0x1;
    handle->isp0_cfg->reg_demosaic_cpxwei_thrd = 0x27;
    handle->isp0_cfg->reg_nb_slope_m           = 0xd;
    handle->isp0_cfg->reg_nb_slope_s           = 0x4;
    handle->isp0_cfg->reg_cfai_en              = 1;
    handle->isp0_cfg->reg_cfai_bypass          = 0;
    handle->isp0_cfg->reg_cfai_dnr_gavg_ref_en = 0;
    handle->isp0_cfg->reg_cfai_dnr_en          = 0;
    handle->isp0_cfg->reg_cfai_dnr_rb_ratio    = 0x6;
    handle->isp0_cfg->reg_cfai_dnr_g_ratio     = 0x5;

    handle->isp0_cfg->reg_isp_icp_ack_tie1 = 1;

    //AE statistics source selection
    // 1: before DNR with LSC (HW default)
    // 2: before DNR with ALSC
    // 3: before DNR with LSC and ALSC
    // 4: before DNR with WB
    // 5: before DNR with WB,LSC
    // 6: before DNR with WB,ALSC
    // 7: before DNR with WB,LSC,ALSC
    // 8: after DNR with LSC and ALSC
    // 12: after DNR with WB,LSC and ALSC
    handle->isp3_cfg->reg_isp_ae_in_mode = 0;

    //AWB statistics source selection
    // 1: before DNR with LSC
    // 2: before DNR with ALSC
    // 3: before DNR with LSC and ALSC
    // 8: after DNR with LSC and ALSC
    handle->isp3_cfg->reg_isp_awb_in_mode = 2;
    return SUCCESS;
}

int ISP_WdmaTrigger(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_wreq_hpri_set = 1;
    dma->reg_isp_wdma_wreq_thrd = LIMIT_BITS(0x0, 3);
    dma->reg_isp_wdma_wreq_hpri = LIMIT_BITS(0x1, 4);
    dma->reg_isp_wdma_wreq_max = LIMIT_BITS(0xf, 4);

    dma->reg_isp_wdma_trigger_mode = 1;
    dma->reg_isp_wdma_trigger = 1;   //Trigger WDMA

    return 0;
}

int ISP_WdmaEnable(ISP_HAL_HANDLE hnd,IspDmaId_e id, MS_BOOL enable)
{
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_en = enable;
    return 0;
}

int ISP_WdmaReset(ISP_HAL_HANDLE hnd,IspDmaId_e id)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,id);
    dma->reg_isp_wdma_en = 0;
    dma->reg_isp_wdma_wreq_rst = 1;
    handle->isp0_cfg->reg_dbgwr_swrst = 0;
    dma->reg_isp_wdma_wreq_rst = 0;
    handle->isp0_cfg->reg_dbgwr_swrst = 1;
    return 0;
}


u32 ISP_VDOSInit(ISP_HAL_HANDLE hnd)
{

    isp_hal_handle *handle = _ISP_HAL(hnd);
    volatile infinity2_reg_isp_wdma *dma = ISP_WDMABase(hnd,ISP_DMA_GENERAL);

    handle->isp0_cfg->reg_vdos_sblock_blk = 3;
    handle->isp0_cfg->reg_vdos_x_st = 800;
    handle->isp0_cfg->reg_vdos_y_st = 100;
    handle->isp0_cfg->reg_vdos_sblock_width = 170;
    handle->isp0_cfg->reg_vdos_sblock_height = 170;
    handle->isp0_cfg->reg_isp_icp_ack_tie1 = 0;

    handle->isp0_cfg->reg_isp_wdma_crop_start_x = 0;
    handle->isp0_cfg->reg_isp_wdma_crop_start_y = 0;

    handle->isp0_cfg->reg_isp_wdma_crop_width = 1021; // 170 * 3 + 512 -1 = 1021
    handle->isp0_cfg->reg_isp_wdma_crop_high = 2;


    handle->isp0_cfg->reg_load_reg = 0;
    handle->isp0_cfg->reg_vdos_yc_en = 0;
    dma->reg_isp_wdma_align_en = 1;

#if 0
    handle->isp6_cfg->reg_isp_rdma_pitch = ((1021 + 7) >> 3) * 1;
    handle->isp6_cfg->reg_isp_wdma_pitch = ((1021 + 7) >> 3) * 1;
#elif 0
    handle->isp6_cfg->reg_isp_rdma_pitch = (((1021 + 32) >> 5) << 5) >> 3;
    handle->isp6_cfg->reg_isp_wdma_pitch = (((1021 + 32) >> 5) << 5) >> 3;
#elif 1
    //ANDY:Need to fixed
    //dma->reg_isp_rdma_pitch = (((1021 + 8) >> 3) << 3) >> 3;
    dma->reg_isp_wdma_pitch = (((1021 + 8) >> 3) << 3) >> 3;
#endif

    handle->isp0_cfg->reg_vdos_en = 1;
    return 0;
}

int ISP_SetWdmaAttr(ISP_HAL_HANDLE hnd, ISP_WDMA_ATTR wdma_attr)
{
    IspDmaId_e isp_dma_id = ISP_DMA_GENERAL; //TBD
    ISP_PIPE_e isp_pipe = ISP_PIPE_0; //TBD
    int wdma_buf = 0;
    ISP_CROP_WIN isp_crop_win;

    ISP_RDMAEnable(hnd, isp_dma_id, false);
    ISP_WDMAEnable(hnd, isp_dma_id, false);

    wdma_buf = wdma_attr.miu_addr;
    //wdma_buf &= ~(0x20000000);

#if 1
    ISP_WDMABaseAddr(hnd, isp_dma_id, wdma_buf);

    isp_crop_win.x_start = wdma_attr.x;
    isp_crop_win.y_start = wdma_attr.y;
    isp_crop_win.width = wdma_attr.width;
    isp_crop_win.height = wdma_attr.height;
    ISP_SetWDMACrop(hnd, isp_pipe, isp_crop_win);
#else
    dma->reg_isp_wdma_base = (wdma_buf & 0xffff);
    dma->reg_isp_wdma_base_1 = (wdma_buf >> 16) & 0x7ff;

    //UartSendTrace("base:0x%x\n",handle->isp6_cfg->reg_isp_wdma_base );
    //UartSendTrace("base1:0x%x\n",handle->isp6_cfg->reg_isp_wdma_base_1 );

    handle->isp0_cfg->reg_isp_wdma_crop_start_x = wdma_attr.x;
    handle->isp0_cfg->reg_isp_wdma_crop_start_y = wdma_attr.y;
    handle->isp0_cfg->reg_isp_wdma_crop_width = wdma_attr.width;
    handle->isp0_cfg->reg_isp_wdma_crop_high = wdma_attr.height;
#endif

    switch(wdma_attr.wdma_path)
    {
        case ISP_WDMA_SIF:
            //UartSendTrace("--------ISP_WDMA_SIF--------\n");
#if 1
            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_SIF);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_if_wmux_sel = 0;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        case ISP_WDMA_ISP:
            //UartSendTrace("--------ISP_WDMA_ISP--------\n");
#if 1
            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_ISP);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_if_wmux_sel = 1;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        case ISP_WDMA_ISPDS:
            //UartSendTrace("--------ISP_WDMA_ISPDS--------\n");
#if 1
            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_ISPDS);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, true);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_if_wmux_sel = 2;
            handle->isp0_cfg->reg_dspl2wdma_en = 1;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        case ISP_WDMA_ISPSWAPYC:
            //UartSendTrace("--------ISP_WDMA_ISPSWAPYC--------\n");

#if 1
            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_ISPSWAPYC);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_if_wmux_sel = 3;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        // FIXJASON
        case ISP_WDMA_VDOS:
            //UartSendTrace("--------ISP_WDMA_VDOS--------\n");
            ISP_VDOSInit(hnd);
#if 1
            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_VDOS);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_if_wmux_sel = 4;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            //isp6->reg_isp_wdma_mode = 0; //PITCH_8BITS
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
#endif
            break;
        case ISP_WDMA_DEFAULT_SIF: //Use ISP crop range
            //UartSendTrace("--------ISP_WDMA_DEFAULT_SIF--------\n");
#if 1
            ISP_GetISPCrop(hnd, isp_pipe, &isp_crop_win);
            ISP_SetWDMACrop(hnd, isp_pipe, isp_crop_win);

            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_SIF);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_wdma_crop_start_x = handle->isp0_cfg->reg_isp_crop_start_x;
            handle->isp0_cfg->reg_isp_wdma_crop_start_y = handle->isp0_cfg->reg_isp_crop_start_y;
            handle->isp0_cfg->reg_isp_wdma_crop_width = handle->isp0_cfg->reg_isp_crop_width;
            handle->isp0_cfg->reg_isp_wdma_crop_high = handle->isp0_cfg->reg_isp_crop_high;

            handle->isp0_cfg->reg_isp_if_wmux_sel = 0;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        case ISP_WDMA_DEFAULT_YC:  //Use ISP crop range
            //UartSendTrace("--------ISP_WDMA_DEFAULT_YC--------\n");
#if 1
            ISP_GetISPCrop(hnd, isp_pipe, &isp_crop_win);
            isp_crop_win.x_start = 0;
            isp_crop_win.y_start = 0;
            ISP_SetWDMACrop(hnd, isp_pipe, isp_crop_win);

            ISP_WMUXSrc(hnd, isp_pipe, ISP_WMUX_ISPSWAPYC);
            ISP_EnableRawDNWDMAClk(hnd, isp_pipe, false);
            ISP_WDMASetPitch(hnd, isp_dma_id, isp_crop_win.width, isp_crop_win.height, PITCH_16BITS);
#else
            handle->isp0_cfg->reg_isp_wdma_crop_start_x = 0;
            handle->isp0_cfg->reg_isp_wdma_crop_start_y = 0;
            handle->isp0_cfg->reg_isp_wdma_crop_width = handle->isp0_cfg->reg_isp_crop_width;
            handle->isp0_cfg->reg_isp_wdma_crop_high = handle->isp0_cfg->reg_isp_crop_high;

            handle->isp0_cfg->reg_isp_if_wmux_sel = 3;
            handle->isp0_cfg->reg_dspl2wdma_en = 0;
            ISP_WDMADataPrecision(hnd, ISP_DMA_GENERAL/*TBD*/, ISP_DATAPRECISION_16); //dma->reg_isp_wdma_mode = 2; //PITCH_16BITS
            dma->reg_isp_wdma_pitch = ((wdma_attr.width + 7) >> 3) * 1;
#endif
            break;
        default:
            return -1;
    }

#if 1
    ISP_WDMAAuto(hnd, isp_dma_id, true);
#else
    dma->reg_isp_wdma_auto = 1;
#endif

    return 0;
}

int ISP_RAW_Store_Mode(ISP_HAL_HANDLE hnd, IspDmaId_e isp_dma_id, ISP_DMA_POSITION_e isp_dma_pos, u32 ulmode)
{
    switch(isp_dma_pos){
        case ISP_DMA_POSITION_HEAD:
            switch(isp_dma_id)
            {
                case ISP_DMA_PIPE0:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p0h_rawstore_mode = ulmode;
                    break;
                case ISP_DMA_PIPE1:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p1h_rawstore_mode = ulmode;
                    break;
                case ISP_DMA_PIPE2:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p2h_rawstore_mode = ulmode;
                    break;
                default:
                    //Error
                    break;
            }
            break;
        case ISP_DMA_POSITION_TAIL:
            switch(isp_dma_id)
            {
                case ISP_DMA_PIPE0:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p0t_rawstore_mode = ulmode;
                    break;
                case ISP_DMA_PIPE1:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p1t_rawstore_mode = ulmode;
                    break;
                case ISP_DMA_PIPE2:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p2t_rawstore_mode = ulmode;
                    break;
                default:
                    //Error
                    break;
            }
            break;
        default:
            //Error
            break;
    }

    return SUCCESS;
}

int ISP_RAW_Fetch_Mode(ISP_HAL_HANDLE hnd, IspDmaId_e isp_dma_id, ISP_DMA_POSITION_e isp_dma_pos, u32 ulmode)
{
    switch(isp_dma_pos){
        case ISP_DMA_POSITION_HEAD:
            switch(isp_dma_id)
            {
                case ISP_DMA_PIPE0:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p0h_rawfetch_mode = ulmode;
                    break;
                case ISP_DMA_PIPE1:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p1h_rawfetch_mode = ulmode;
                    break;
                case ISP_DMA_PIPE2:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p1h_rawfetch_mode = ulmode;
                    break;
                default:
                    //Error
                    break;
            }
            break;
        case ISP_DMA_POSITION_TAIL:
            switch(isp_dma_id)
            {
                case ISP_DMA_PIPE0:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p0t_rawfetch_mode = ulmode;
                    break;
                case ISP_DMA_PIPE1:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p1t_rawfetch_mode = ulmode;
                    break;
                case ISP_DMA_PIPE2:
                    _ISP_HAL(hnd)->isp0_cfg->reg_p2t_rawfetch_mode = ulmode;
                    break;
                default:
                    //Error
                    break;
            }
            break;
        default:
            //Error
            break;
    }

    return SUCCESS;
}

int ISP_4Pixel_Data_Pak_AtMsb_To_Wdma(ISP_HAL_HANDLE hnd, ISP_PIPE_e isp_pipe, MS_BOOL enable)
{
    switch(isp_pipe){
        case ISP_PIPE_0:
            _ISP_HAL(hnd)->isp0_cfg->reg_isp_dma_pk_msb = enable;
            break;
        case ISP_PIPE_1:
            _ISP_HAL(hnd)->isp11_cfg->reg_isp_dma_pk_msb = enable;
            break;
        case ISP_PIPE_2:
            _ISP_HAL(hnd)->isp12_cfg->reg_isp_dma_pk_msb = enable;
            break;
    }

    return 0;
}

int ISP_EnableISP_HDR(ISP_HAL_HANDLE hnd, MS_BOOL enable)
{
    if(enable){
        _ISP_HAL(hnd)->isp0_cfg->reg_en_isp_hdr = 1;
        _ISP_HAL(hnd)->isp0_cfg->reg_isp_hdr_gated_clk_en = 0;

        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_en = 1;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_stitch_en = 1;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_mapping_en = 1;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_pipe0_dma_src = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_pipe1_dma_src = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_merge_option = 1;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_vs_sel = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_s_sel = 1;
    }
    else{
        _ISP_HAL(hnd)->isp0_cfg->reg_en_isp_hdr = 0;
        _ISP_HAL(hnd)->isp0_cfg->reg_isp_hdr_gated_clk_en = 1;

        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_en = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_stitch_en = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_mapping_en = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_pipe0_dma_src = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_pipe1_dma_src = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_merge_option = 1;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_vs_sel = 0;
        _ISP_HAL(hnd)->isp10_cfg->reg_hdr_s_sel = 1;
    }

    return 0;
}

int HalISP_InitTestMode(ISP_HAL_HANDLE hnd,u32 nWidth,u32 nHeight)
{
    //Set ISP register to real time mode. and all IQ are disabled.
    isp_hal_handle *handle = (isp_hal_handle *)hnd;
    u16 *reg = 0;

    //wriu -w 0x150978 0x0191   //0x3C: CMC enable
    reg = reg_addr(handle->isp0_cfg,isp0_reg_isp_r2y_cmc_en);
    *reg = 0x0191; //CMC ENABLE

    /*
    wriu -w 0x15097a 0x01c2
    wriu -w 0x15097c 0x1e87
    wriu -w 0x15097e 0x1fb7
    wriu -w 0x150980 0x0107
    wriu -w 0x150982 0x0204
    wriu -w 0x150984 0x0064
    wriu -w 0x150986 0x1f68
    wriu -w 0x150988 0x1ed6
    wriu -w 0x15098a 0x01c2
    wriu -w 0x150960 0x0000
     */
    handle->isp0_cfg->reg_isp_r2y_coeff_11 = 0x01C2;
    handle->isp0_cfg->reg_isp_r2y_coeff_12 = 0x1E87;
    handle->isp0_cfg->reg_isp_r2y_coeff_13 = 0x1FB7;
    handle->isp0_cfg->reg_isp_r2y_coeff_21 = 0x0107;
    handle->isp0_cfg->reg_isp_r2y_coeff_22 = 0x0204;
    handle->isp0_cfg->reg_isp_r2y_coeff_23 = 0x0064;
    handle->isp0_cfg->reg_isp_r2y_coeff_31 = 0x1F68;
    handle->isp0_cfg->reg_isp_r2y_coeff_32 = 0x1ED6;
    handle->isp0_cfg->reg_isp_r2y_coeff_33 = 0x01C2;
    handle->isp0_cfg->reg_c_filter         = 0x0;
    handle->isp0_cfg->reg_ipm_h_mirror     = 0x0;
    handle->isp0_cfg->reg_44to42_dith_en   = 0x0;
    handle->isp0_cfg->reg_rgb2yuv_dith_en  = 0x0;

    //wriu -w 0x1509e2 0x0d07
    handle->isp0_cfg->reg_demosaic_dvh_slope = 0x07;
    handle->isp0_cfg->reg_demosaic_dvh_thrd  = 0x0D;
    //wriu -w 0x1509e4 0x2701
    handle->isp0_cfg->reg_demosaic_cpxwei_slope = 0x01;
    handle->isp0_cfg->reg_demosaic_cpxwei_thrd  = 0x27;
    //wriu -w 0x1509e6 0x040d
    handle->isp0_cfg->reg_nb_slope_m        = 0x0d;
    handle->isp0_cfg->reg_nb_slope_s        = 0x04;

    //wriu -w 0x1509e0 0x5601   //CFAI enable , DEMOSAIC
    reg = reg_addr(handle->isp0_cfg,isp0_reg_cfai_en);
    *reg = 0x5601;

    //wriu -w 0x1509F4 0x0080 // Frame end not check ACC done
    handle->isp0_cfg->reg_mask_isp_misc_frame_done = 1;

    //wriu -w 0x151160 0x0100 // BS bypass
    handle->isp8_cfg->reg_bs_bypass         = 0;
    handle->isp8_cfg->reg_bs_store0_en      = 0;
    handle->isp8_cfg->reg_bs_store1_en      = 0;
    handle->isp8_cfg->reg_src2bs_path_sel   = 0;
    handle->isp8_cfg->reg_bs2post_path_sel  = 0;
    handle->isp8_cfg->reg_bs_bypass_all     = 1;

    //wriu -w 0x151176 0x0001 // BS output crop x  st
    handle->isp8_cfg->reg_scale_out_start_h = 0x0001;
    //wriu -w 0x151178 0x0500 // BS output crop x  end
    handle->isp8_cfg->reg_scale_out_end_h   = nWidth;//0x0500;
    //wriu -w 0x15117a 0x0001 // BS output crop y  st
    handle->isp8_cfg->reg_scale_out_start_v = 0x0001;
    //wriu -w 0x15117c 0x02D0 // BS output crop y  end
    handle->isp8_cfg->reg_scale_out_end_v   = nHeight;//0x02D0;

    //wriu -w 0x150922 0x0001 //bayer ID
    handle->isp0_cfg->reg_isp_sensor_array  = 1;
    //handle->isp0_cfg->reg_isp_sensor_yuv_order = 1;
    //wriu -w 0x150902 0x0000 //0x00, reset ISP
    reg = reg_addr(handle->isp0_cfg,isp0_reg_isp_sw_rstz);
    *reg = 0x0000;
    udelay(500);
    //wriu -w 0x150902 0x8003 //0x01, disable double buffer
    handle->isp0_cfg->reg_load_reg          = 1;
    handle->isp0_cfg->reg_isp_sw_rstz       = 1;
    handle->isp0_cfg->reg_isp_sw_p1_rstz    = 1;
    //wriu -w 0x150908 0x0240 //0x04, isp output async mode = 1,isp_if_rmux = RDMA, isp_wdma_mux=isp_dp
    reg = reg_addr(handle->isp0_cfg,isp0_reg_isp_if_rmux_sel);
    *reg = 0x0240;
    //wriu -w 0x15090A 0x0000 //0x0A, reg_isp_if_src_sel from sensor
    reg = reg_addr(handle->isp0_cfg,isp0_reg_isp_if_src_sel);
    *reg = 0x0000;
    //wriu -w 0x150924 0x0001 //0x12, isp crop X START 1
    handle->isp0_cfg->reg_isp_crop_start_x      = 0x0001;
    //wriu -w 0x150926 0x0001 //0x13, isp crop Y START 1
    handle->isp0_cfg->reg_isp_crop_start_y      = 0x0001;
    //wriu -w 0x150928 0x04FF //0x14, isp crop width 1280
    handle->isp0_cfg->reg_isp_crop_width        = nWidth-1;//0x04FF;
    //wriu -w 0x15092A 0x02CF //0x15, isp crop height 720
    handle->isp0_cfg->reg_isp_crop_high         = nHeight-1;//0x02CF;
    //wriu -w 0x150920 0x1500 //0x10, sensor formate 10 bits , RGB , reg_sensor_hsync_polarity=low active
    reg = reg_addr(handle->isp0_cfg,isp0_reg_isp_master_mode);
    *reg = 0x1500;
    //wriu -w 0x150950 0x0000 //0x28, clear interrupt mask1
    handle->isp0_cfg->reg_c_irq_mask            = 0;
    //wriu -w 0x150954 0xFFFF //0x2A, clear interrupt status1
    handle->isp0_cfg->reg_c_irq_clr             = 0xFFFF;
    //wriu -w 0x150954 0x0000 //0x2A, clear interrupt status1
    handle->isp0_cfg->reg_c_irq_clr             = 0x0000;
    //wriu -w 0x150994 0x0000 //0x4A, clear interrupt mask2
    handle->isp0_cfg->reg_c_irq_mask3           = 0x0;
    //wriu -w 0x150998 0xFFFF //0x4B, clear interrupt status2
    handle->isp0_cfg->reg_c_irq_clr3            = 0xFFFF;
    //wriu -w 0x150998 0x0000 //0x4C, clear interrupt status2
    handle->isp0_cfg->reg_c_irq_clr3            = 0x0000;

    handle->isp0_cfg->reg_p0h_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p0h_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p0t_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p0t_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p1h_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p1h_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p1t_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p1t_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p2h_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p2h_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p2t_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p2t_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p3h_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p3h_rawfetch_mode     = 0;
    handle->isp0_cfg->reg_p3t_rawstore_mode     = 0;
    handle->isp0_cfg->reg_p3t_rawfetch_mode     = 0;

    //wriu -w 0x150AC8 0x0001 //0x64, set line count interrupt0
    //wriu -w 0x150ACA 0x0002 //0x65, set line count interrupt1
    //wriu -w 0x150ACC 0x0010 //0x66, set line count interrupt2
    handle->isp1_cfg->reg_isp_obc_line_cnt1     = 1;
    handle->isp1_cfg->reg_isp_obc_line_cnt2     = 2;
    handle->isp1_cfg->reg_isp_obc_line_cnt3     = nHeight-1;

    //wriu -w 0x150900 0x0003 //0x00, enable ISP
    handle->isp0_cfg->reg_en_sensor             = 1;
    handle->isp0_cfg->reg_en_isp                = 1;

    //stop vif
    //((unsigned short*)BANK_BASE_ADDR(0x1502))[1*2] = 0x1;
    //msleep(100);

    //reset
    //wriu -w 0x150902 0x8003 //0x01, disable double buffer
    handle->isp0_cfg->reg_load_reg          = 0;
    handle->isp0_cfg->reg_isp_sw_rstz       = 0;
    handle->isp0_cfg->reg_isp_sw_p1_rstz    = 0;

    udelay(500);
    //((unsigned short*)BANK_BASE_ADDR(0x1502))[1*2] = 0x0;

    //wriu -w 0x150902 0x8003 //0x01, disable double buffer
    handle->isp0_cfg->reg_load_reg          = 1;
    handle->isp0_cfg->reg_isp_sw_rstz       = 1;
    handle->isp0_cfg->reg_isp_sw_p1_rstz    = 1;
    return 0;
}

int HalIsp_Init(ISP_HAL_HANDLE *p_handle)
{
    isp_handle_t* handle = IspSysMalloc(sizeof(isp_handle_t));
    handle->isp0_cfg          = (volatile infinity2_reg_isp0       *)BANK_BASE_ADDR(RIUBASE_ISP_0);
    handle->isp1_cfg          = (volatile infinity2_reg_isp1       *)BANK_BASE_ADDR(RIUBASE_ISP_1);
    handle->isp2_cfg          = (volatile infinity2_reg_isp2       *)BANK_BASE_ADDR(RIUBASE_ISP_2);
    handle->isp3_cfg          = (volatile infinity2_reg_isp3       *)BANK_BASE_ADDR(RIUBASE_ISP_3);
    handle->stats_alsc_dnr_cfg = handle->isp3_cfg;
    handle->isp4_cfg          = (volatile infinity2_reg_isp4       *)BANK_BASE_ADDR(RIUBASE_ISP_4);
    handle->isp5_cfg          = (volatile infinity2_reg_isp5       *)BANK_BASE_ADDR(RIUBASE_ISP_5);
    handle->isp6_cfg          = (volatile infinity2_reg_isp6       *)BANK_BASE_ADDR(RIUBASE_ISP_6);
    handle->isp7_cfg          = (volatile infinity2_reg_isp7       *)BANK_BASE_ADDR(RIUBASE_ISP_7);
    handle->isp8_cfg          = (volatile infinity2_reg_isp8       *)BANK_BASE_ADDR(RIUBASE_ISP_8);
    handle->isp9_cfg          = (volatile infinity2_reg_isp9       *)BANK_BASE_ADDR(RIUBASE_ISP_9);
    handle->isp10_cfg         = (volatile infinity2_reg_isp10      *)BANK_BASE_ADDR(RIUBASE_ISP_10);
    handle->isp11_cfg         = (volatile infinity2_reg_isp11      *)BANK_BASE_ADDR(RIUBASE_ISP_11);
    handle->isp12_cfg         = (volatile infinity2_reg_isp12      *)BANK_BASE_ADDR(RIUBASE_ISP_12);
    handle->mload_cfg         = (volatile infinity2_reg_isp_miu2sram     *)BANK_BASE_ADDR(RIUBASE_ISP_MLOAD);
    handle->scl_mload_cfg     = (volatile infinity2_reg_isp_miu2sram     *)BANK_BASE_ADDR(RIUBASE_SCL_MLOAD);
    handle->wdma0_cfg          = 0;//(volatile infinity2_reg_isp_wdma   *)(BANK_BASE_ADDR(RIUBASE_ISP_DMA0)+0x20*4);
    handle->wdma1_cfg          = 0;//(volatile infinity2_reg_isp_wdma   *)(BANK_BASE_ADDR(RIUBASE_ISP_DMA0)+0x60*4);
    handle->wdma2_cfg          = 0;//(volatile infinity2_reg_isp_wdma   *)BANK_BASE_ADDR(RIUBASE_ISP_12);
    handle->wdma3_cfg          = (volatile infinity2_reg_isp_wdma   *)(BANK_BASE_ADDR(RIUBASE_ISP_DMA6)+0x60*4);;//(volatile infinity2_reg_isp_wdma   *)(BANK_BASE_ADDR(RIUBASE_ISP_12)+0x20*4);
    handle->rdma0_cfg          = (volatile infinity2_reg_isp_rdma   *)BANK_BASE_ADDR(RIUBASE_ISP_DMA0);
    handle->rdma1_cfg          = (volatile infinity2_reg_isp_rdma   *)(BANK_BASE_ADDR(RIUBASE_ISP_DMA0)+0x40*4);

    pr_info("isp0 base = 0x%X\n", (u32)handle->isp0_cfg);
    pr_info("isp1 base = 0x%X\n", (u32)handle->isp1_cfg);
    pr_info("isp2 base = 0x%X\n", (u32)handle->isp2_cfg);

    handle->AwbWin.u2BlkNum_x = AWB_WIN_MAX_WIDTH;
    handle->AwbWin.u2BlkNum_y = AWB_WIN_MAX_HEIGHT;
    handle->AwbWin.u2BlkVaild_x = AWB_BLOCK_MIN_VALID_X;
    handle->AwbWin.u2BlkVaild_y = AWB_BLOCK_MIN_VALID_Y;

    //pr_info("Iq init cfg\n");
    //ISP_IQ_Init_Cfg(handle);
    //HalIspInitReg(handle);
    *p_handle = handle;

    return SUCCESS;
}

int HalIsp_Deinit(ISP_HAL_HANDLE hnd)
{
    isp_hal_handle *handle = _ISP_HAL(hnd);
    IspSysFree(handle);
    return SUCCESS;
}
