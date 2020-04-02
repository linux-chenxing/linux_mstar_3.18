#ifndef MLOAD_COMMON_H_
#define MLOAD_COMMON_H_


#define RTOS  0
#define LINUX 1
#define OS_SEL LINUX

#if(OS_SEL == LINUX)
#include<linux/string.h>
#include <linux/time.h>
#endif
#include<mdrv_mload.h>
#include <mhal_common.h>
#include<mhal_cmdq.h>



#if(OS_SEL == LINUX)
    #define MLOAD_DEBUG(args...) printk(args)
#elif(OS_SEL == RTOS)
    #define MLOAD_DEBUG(args...) UartSendTrace(args)
#endif


typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

#define MAX_CHANNEL_NUM 16
#define MIU_BASE_PHYADDR (0x20000000)
#define MIU_MAX_PHYADDR  (0x20000000+0x28000000)

#define CMQ_RIU_REG(bank,offset) ((bank<<8)+(offset<<1)) //register address from CMQ
#define CPU_RIU_REG(bank,offset) ((bank<<9)+(offset<<2)) //register address from CPU

void MloadWarperInit(void);
void *MloadMalloc(int size);
void MloadFree(void *ptr);
void MloadFreeNonCache(void *addr);
void *MloadMallocNonCache(int size,int align);
void *MloadVA2PA(void *addr);
void MloadRegW(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 uBank,u32 uOffset,u16 uVal,u16 Mask);
void MloadRegR(FrameSyncMode mode,u32 uBank, u32 uOffset, u16 *uValue);
u32 HalUtilPHY2MIUAddr(u32 phyaddr);
void MloadSleep(u32 time);
u32 MloadPollRegBit(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, u32 uBank,u32 uOffset, u16 uValue, u16 Mask, u32 bPollEq);

#endif //MLOAD_COMMON_H_
