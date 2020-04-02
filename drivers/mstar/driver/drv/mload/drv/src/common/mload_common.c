#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/device.h>
#include <mload_common.h>
#include <linux/delay.h>

#if OS_SEL == LINUX

#if 0
typedef int MI_S32;
typedef int MI_U32;
typedef int MI_PHY;
typedef char MI_U8;
typedef  bool MI_BOOL;

extern MI_S32 mi_sys_MMA_Alloc(MI_U8 *u8MMAHeapName, MI_U32 u32Size ,MI_PHY *phyAddr);
extern void * mi_sys_Vmap(MI_PHY u64PhyAddr, MI_U32 u32Size , MI_BOOL bCache);
extern MI_S32 mi_sys_MMA_Free(MI_PHY phyAddr);
#endif

#define mVir  0
#define mPhy  1
#define mSize 2

static u32 _gaddr_map[MAX_CHANNEL_NUM][32];
static u32 _gaddr_cnt = 0;

/* IO REMAP */
void *gpIoBase = 0;

void MloadWarperInit(void){

    //gpIoBase = (void*) ioremap(0xFD000000, 0x400000);
	gpIoBase = (void *)0xFD000000;
	MLOAD_DEBUG("[[[[[[[[[ gpIoBase:0x%x\n ]]]]]]]]]]]]",(int)gpIoBase);
    //_gaddr_cnt = 0;

    return;
}

void *MloadMalloc(int size){

#if 1

    void *vir_addr;

    vir_addr = kmalloc(size,GFP_KERNEL);

    MLOAD_DEBUG("Malloc : vir:0x%x\n", (int)vir_addr);
#else
    char bufname[10];
    int phy_addr = 0;
    void *vir_addr;

    sprintf(bufname,"MLOAD_BUF%d",_gaddr_cnt);

    mi_sys_MMA_Alloc(bufname,size,&phy_addr);
    vir_addr = mi_sys_Vmap(phy_addr,size,0);

    MLOAD_DEBUG("Malloc [%s] : vir:0x%x , phy:0x%x\n",bufname, (int)vir_addr, (int)phy_addr);
#endif



	return vir_addr;
}

void MloadFree(void *addr){

#if 1

    kfree(addr);

#else
    if(_gaddr_cnt <= 0)
        return;

    mi_sys_MMA_Free((int)addr);

    _gaddr_cnt--;

#endif
    return;
}


void MloadFreeNonCache(void *addr){

#if 1
    int i=0;

    if(_gaddr_cnt <= 0)
        return;

    for(i=0;i<MAX_CHANNEL_NUM;i++){

        if(_gaddr_map[_gaddr_cnt][mVir] == (u32)addr){
            dma_free_coherent(NULL, _gaddr_map[_gaddr_cnt][mSize], addr, _gaddr_map[_gaddr_cnt][mPhy]);

            _gaddr_cnt--;
        }
    }
#else
    if(_gaddr_cnt <= 0)
        return;

    mi_sys_MMA_Free((int)addr);

    _gaddr_cnt--;

#endif
    return;
}

void *MloadMallocNonCache(int size,int align){

#if 1
    static dma_addr_t phy_addr;
    void *vir_addr;

    if(_gaddr_cnt >= MAX_CHANNEL_NUM)
        return NULL;

    vir_addr = dma_alloc_coherent(NULL, size, &phy_addr, GFP_KERNEL);

    _gaddr_map[_gaddr_cnt][mVir]= (u32)vir_addr;
    _gaddr_map[_gaddr_cnt][mPhy] = phy_addr;
    _gaddr_map[_gaddr_cnt][mSize] = size;

    _gaddr_cnt++;

    MLOAD_DEBUG("Malloc : vir:0x%x , phy:0x%x\n", (int)vir_addr, (int)phy_addr);
#else
    char bufname[10];
    int phy_addr = 0;
    void *vir_addr;

    sprintf(bufname,"MLOAD_BUF%d",_gaddr_cnt);

    mi_sys_MMA_Alloc(bufname,size,&phy_addr);
    vir_addr = mi_sys_Vmap(phy_addr,size,1);

    _gaddr_cnt++;
    MLOAD_DEBUG("Malloc [%s] : vir:0x%x , phy:0x%x\n",bufname, (int)vir_addr, (int)phy_addr);

#endif

    return vir_addr;
}

void *MloadVA2PA(void *addr){

    int i=0;

    for(i=0;i<MAX_CHANNEL_NUM;i++){

        if(_gaddr_map[i][mVir] == (u32)addr)
            return (void *)_gaddr_map[i][mPhy];

    }

	return NULL;
}

void MloadRegW(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t,FrameSyncMode mode,u32 uBank,u32 uOffset,u16 uVal,u16 Mask){

    int ret=0;

    if(mode == RIU_MODE){
         //MLOAD_DEBUG("--RegWrite base:0x%x , addr:0x%x , bank:0x%x, offset:0x%x, Val:0x%x\n",(int)gpIoBase,CPU_RIU_REG(uBank,uOffset),uBank,uOffset,uVal);
        *(unsigned short*)((unsigned long)gpIoBase + CPU_RIU_REG(uBank,uOffset)) = uVal;

    }else if(mode ==CMDQ_MODE){

        if(pCmqInterface_t != NULL){
			//MLOAD_DEBUG("--CMDQ RegWrite  addr:0x%x , bank:0x%x, offset:0x%x, Val:0x%x Mask:0x%x\n",(int)(CMQ_RIU_REG(uBank,uOffset)),uBank,uOffset,uVal,Mask);
			ret=pCmqInterface_t->MHAL_CMDQ_WriteRegCmdqMask(pCmqInterface_t,CMQ_RIU_REG(uBank,uOffset),uVal,Mask);
			//pCmqInterface_t->MHAL_CMDQ_KickOffCmdq(pCmqInterface_t);
            //ret=pCmqInterface_t->MHAL_CMDQ_WriteRegCmdq(pCmqInterface_t,CMQ_RIU_REG(uBank,uOffset),uVal);
			if(ret!=0)
			    MLOAD_DEBUG("cmdq fail!!!!!!!!!!!!\n");
		}

    }else{

        *(unsigned short*)((unsigned long)gpIoBase + CPU_RIU_REG(uBank,uOffset)) = uVal;
    }

    return;
}


void MloadRegR(FrameSyncMode mode,u32 uBank, u32 uOffset, u16 *uValue){

    if(mode == RIU_MODE){
        *uValue = *(unsigned short*)((unsigned long)gpIoBase + CPU_RIU_REG(uBank,uOffset));
        //MLOAD_DEBUG("##RegRead base:0x%x , addr:0x%x , bank:0x%x, offset:0x%x, Val:0x%x\n",(int)gpIoBase,CPU_RIU_REG(uBank,uOffset),uBank,uOffset,(int)*uValue);
    }else{

        *uValue=0;

    }

    return;
}


u32 HalUtilPHY2MIUAddr(u32 phyaddr)
{
    /* Transform to MIU address */
    if (phyaddr > MIU_MAX_PHYADDR || phyaddr < MIU_BASE_PHYADDR)
        return  -1;
    else
        return  phyaddr - MIU_BASE_PHYADDR;
}

void MloadSleep(u32 time){

    msleep(time);

    return;
}

u32 MloadPollRegBit(MHAL_CMDQ_CmdqInterface_t *pCmqInterface_t, u32 uBank,u32 uOffset, u16 uValue, u16 Mask, u32 bPollEq){

    if(pCmqInterface_t != NULL)
       return pCmqInterface_t->MHAL_CMDQ_CmdqPollRegBits(pCmqInterface_t,(u32) (/*gpIoBase + */CMQ_RIU_REG(uBank,uOffset)),uValue,Mask,bPollEq);
    else
        return -1;
}

#elif(OS_SEL == RTOS)
void *gpIoBase = 0;

void MloadWarperInit(void){

    gpIoBase = (void*) 0x1F000000;

    return;
}

void *MloadMalloc(int size){

    void *vir_addr;

    vir_addr = MsAllocateMem(size);

    return vir_addr;
}

void MloadFree(void *addr){

    MsReleaseMemory(addr);
}

void *MloadMallocNonCatch(int size){

    void* vir_addr;

    vir_addr = MsAllocateNonCacheMem(size);

    return vir_addr;
}

void *MloadVA2PA(void *addr){

    return MsVA2PA(addr);
}



u32 HalUtilPHY2MIUAddr(u32 phyaddr)
{
    /* Transform to MIU address */
    if (phyaddr > MIU_MAX_PHYADDR || phyaddr < MIU_BASE_PHYADDR)
        return  -1;
    else
        return  phyaddr - MIU_BASE_PHYADDR;
}

void MloadSleep(u32 time){

    MsSleep(time);
}
#endif //OS_SEL
