#include "hal_clk.h"
#include "hal_debug.h"

#define IVE_CLK_BASE_ADDR   (0x100A0000)
#define IVE_MCM_BASE_ADDR   (0x14310000)

#if (HAL_MSG_LEVL < HAL_MSG_DBG)
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  ms_writew(val,((uint)base+(idx)*4))
#else
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  do{HAL_MSG(HAL_MSG_DBG, "write 0x%08X = 0x%04X\n", ((uint)base+(idx)*4), val); ms_writew(val,((uint)base+(idx)*4));} while(0)
#endif

/*******************************************************************************************************************
 * clk_hal_init
 *   init device clock
 *
 * Parameters:
 *   RIU_BASE_ADDR:  clock base address
 *
 * Return:
 *   0: OK, othes: failed
 */
//<MStar Software>//CEVA PLL: 600MHz setting
int clk_hal_init(void)
{
    int err_state = 0;

    REGW(IVE_CLK_BASE_ADDR, 0x52, 0x080); //216 MHz
    REGW(IVE_MCM_BASE_ADDR, 0x2, 0x20);
        
    HAL_MSG(HAL_MSG_DBG, "IVE_CLK_BASE_ADDR = 0x%04x\n", REGR(IVE_CLK_BASE_ADDR, 0x52));

    return err_state;
}
