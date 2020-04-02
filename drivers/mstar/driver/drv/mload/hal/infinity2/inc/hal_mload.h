#ifndef HAL_MLOAD_H_
#define HAL_MLOAD_H_

#include<drv_mload.h>
#include<mload_common.h>
#include<mdrv_mload.h>


#define ISP0_BANK          0x1509
#define MLOAD_BANK         0x1519
#define SCL_MLOAD_BANK     0x1534
#define MLOAD_CLK_BANK     0x1432
#define SCL_INTR_BANK      0x1525

#define MLOAD_INTR_MASK             0x4A
#define MLOAD_INTR_CLR              0x4C
#define MLOAD_INTR_STATUS           0x4D
    #define MASK_MLOAD_DONE                 0x0008
    #define BIT_MLOAD_DONE(a)               (a<<3)

#define SCL_MLOAD_INTR_MASK         0x0B
#define SCL_MLOAD_INTR_CLR          0x13
#define SCL_MLOAD_INTR_STATUS       0x17
    #define MASK_SCL_MLOAD_DONE             0x1000
    #define BIT_SCL_MLOAD_DONE(a)           (a<<12)

#define CLK_ISP_MLOAD               0x26
#define MASK_CLK_ISP_MLOAD                  0x00C0
#define SHIFT_CLK_ISP_MLOD(a)               (a<<4)

#define SRAM_ID                     0x00
    #define MASK_LOAD_SRAM_ID               0x001F
    #define MASK_SRAM_READ_ID               0x1F00
    #define SET_SRAME_READ_ID(a)            (a<<8)
#define LOAD_AMOUNT                 0x01
#define SRAM_ST_ADDR                0x02
#define SRAM_LOAD_ADDR              0x03
#define SRAM_LOAD_ADDR_1            0x04
#define SRAM_RD_ADDR                0x05
#define SRAME_READ_DATA             0x06
#define FIRE_WLR                    0x09
    #define MASK_LOAD_REGISTER_WLR           0x0001
    #define MASK_LOAD_ST_WLR                 0x0002
    #define SET_LOAD_ST_WLR(a)               (a<<1)
    #define MASK_SRAM_READ_WLR               0x0004
    #define SET_SRAM_READ_WLR(a)             (a<<2)
    #define MASK_LOAD_START_ERROR_CLEAR_WLR  0x0008
    #define SET_LOAD_START_ERROR_WLR(a)      (a<<3)
#define LOAD_STATUS                 0x0A
    #define MASK_LOAD_START_ERROR   0x01
    #define MASK_LOAD_BUSY          0x02
#define SRAM_CTRL                   0x0B
    #define MASK_SRAM_WIDTH                 0x0003
    #define MASK_SRAM_RW                    0x0100
    #define SET_SRAM_RW(a)                  (a<<8)
    #define MASK_MIU2SRAM_EN                0x0200
    #define SET_MIU2SRAM_EN(a)              (a<<9)
    #define MASK_LOAD_REGISTER_NON_FULL     0x0400
    #define MASK_LOAD_DONE                  0x0800
    #define SET_LOAD_DONE(a)                (a<<11)
    #define MASK_WAIT_HW_RDY_EN             0x1000
    #define SET_WAIT_HW_RDY_EN(a)           (a<<12)
    #define MASK_LOAD_WATER_LEVEL           0x6000
    #define SET_LOAD_WATER_LEVEL(a)         (a<<13)
    #define MASK_SW_RST                     0x8000
    #define SET_SW_RST(a)                   (a<<15)
#define SRAM_READ_DATA_0            0x06
#define SRAM_READ_DATA_1            0x07
#define SRAM_READ_DATA_2            0x08


#define MLOAD_BUF_PHYS(mload_mem_cfg,id) ( mload_mem_cfg->u32physAddr + (mload_mem_cfg->u32RingBufWp*mload_mem_cfg->length) + mload_mem_cfg->u4Base[mload_mem_cfg->uBufferIdx] + mload_mem_cfg->tblOffset[id] )
#define MLOAD_BUF_VIRT(mload_mem_cfg,id) ( ((char*)mload_mem_cfg->pVirAddr + (mload_mem_cfg->u32RingBufWp*mload_mem_cfg->length)) + mload_mem_cfg->u4Base[mload_mem_cfg->uBufferIdx] + mload_mem_cfg->tblOffset[id] )
#define MLOAD_TBL_SIZE_W(mload_mem_cfg,id) (mload_mem_cfg->tblRealSize[id]/2)
#define MLOAD_TBL_SIZE_B(mload_mem_cfg,id) (mload_mem_cfg->tblRealSize[id])


int ISP_MLoadTableSet(IspMloadID_e idx, const u16 *table, MLOAD_IQ_MEM *mload_mem_cfg);
int ISP_MLoadApplyIspDomain(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode);
int ISP_MLoadApplyIspImgDomain(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode);
int SCL_MLoadApply(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode);
int ISP_MLoadRead(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode,int id,int sram_offset,ISP_MLOAD_OUTPUT *output);
int SCL_MLoadRead(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, MLOAD_IQ_MEM *mload_mem_cfg,FrameSyncMode mode,int id,int sram_offset,ISP_MLOAD_OUTPUT *output);
void ISP_MLoadSwitchClk(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable);
u32 ISP_WaitMloadDone(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 timeout);
u32 SCL_WaitMloadDone(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 timeout);
u32 ISP_EnableMloadIntr(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable);
u32 SCL_EnableMloadIntr(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u8 enable);

#endif //HAL_MLOAD_H_
