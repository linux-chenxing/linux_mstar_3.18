#include "ms_platform.h"
#include "mdrv_cwa.h"
#include "hal_debug.h"

#if (!DUMP_REGISTER)
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  ms_writew(val,((uint)base+(idx)*4))
#else
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  do{CWA_MSG(CWA_MSG_DBG, "write 0x%08X = 0x%04X\n", ((uint)base+(idx)*4), val); } while(0)
#endif


//=================================================
void debug_hal_isr_check(void)
{
   //interrupt status
    CWA_MSG(CWA_MSG_DBG, "cevasys interrupt : bank 0x%08X, index 0x%02X, value 0x%04X\n", BANK_CAL(0x1128) , (0x18), REGR(BANK_CAL(0x1128), 0x18));
}
//=================================================
void debug_hal_buffer_check(unsigned char* puaddr)
{
    int i = 0;

    CWA_MSG(CWA_MSG_DBG, "addr phy addr 0x%8X\n",(unsigned int)puaddr);

    for (i=0; i< 1000; i++)
    {
        CWA_MSG(CWA_MSG_DBG, "addr 0x%8X] : 0x%02X\n", i, puaddr[i]);
    }
}
//=================================================
void debug_warp_status_check()
{
   //interrupt status
   //interrupt status
    CWA_MSG(CWA_MSG_DBG, "warp status : bank 0x%08X, index 0x%02X, value 0x%04X\n", BANK_CAL(0x1A24) , (0x04/4), REGR(BANK_CAL(0x1A24), 0x04/4));
}
