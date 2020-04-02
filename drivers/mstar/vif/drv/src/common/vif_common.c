#include <vif_common.h>
#include <cam_os_wrapper.h>

void *g_ptVIF = NULL;  // BANK:0x1502 ~ 0x1507
void *g_ptVIF2 = NULL; // BANK:0x1705 ~ 0x1706
void *g_BDMA = NULL;   // BANK:0x1009 bdma ch0
void *g_MCU8051 = NULL; // BANK:0x0010 MCU
void *g_PMSLEEP = NULL; // BANK:0x000e PM_SLEEP
void *g_MAILBOX = NULL; // BANK:0x1033H MAILBOX
void *g_TOPPAD1 = NULL; // BANK:0x1026H TOPPAD1
void *g_ISP_ClkGen = NULL; // BANK:0x1432H reg_block_ispsc


#if IPC_DMA_ALLOC_COHERENT
u32 IPCRamPhys = 0;
char *IPCRamVirtAddr  = NULL;
#else
unsigned long IPCRamPhys = 0;
void *IPCRamVirtAddr = NULL;
#endif
