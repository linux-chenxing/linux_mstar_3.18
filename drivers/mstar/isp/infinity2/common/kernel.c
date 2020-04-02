#include <cam_os_wrapper.h>
#include <kernel_chiptop.h>
#include <kernel_clkgen.h>
#include <kernel_padtop.h>

volatile CHIPTOP_t * const g_ptCHIPTOP;
volatile Clkgen_t* const g_ptClkgenTop;
volatile PADTOP1_t * const g_ptPADTOP1;
volatile PADTOP2_t * const g_ptPADTOP2;

int fake_kernel_init(void)
{
    //TODO:
    /*
    g_ptCHIPTOP = ioremap();
    g_ptClkgenTop = ioremap();
    g_ptPADTOP0 = ioremap();
    g_ptPADTOP1 = ioremap();
    */
    return 0;
}
