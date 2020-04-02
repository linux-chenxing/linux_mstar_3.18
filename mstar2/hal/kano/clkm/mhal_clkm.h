#ifndef _MHAL_CLKM_H
#define _MHAL_CLKM_H


#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define _CLKM_REG_BASE    mstar_pm_base
#else
#define _CLKM_REG_BASE    0xfd000000
#endif

#endif


