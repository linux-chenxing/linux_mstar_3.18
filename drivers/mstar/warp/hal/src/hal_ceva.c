#include "hal_ceva.h"
#include "hal_debug.h"

#include "ms_platform.h"
#include <linux/delay.h>

#define LOW_U16(value)   (((u32)(value))&0x0000FFFF)
#define HIGH_U16(value)  ((((u32)(value))&0xFFFF0000)>>16)

#define MAKE_U32(high, low) ((((u32)high)<<16) | low)

#if (HAL_MSG_LEVL < HAL_MSG_DBG)
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  ms_writew(val,((uint)base+(idx)*4))
#else
#define REGR(base,idx)      ms_readw(((uint)base+(idx)*4))
#define REGW(base,idx,val)  do{HAL_MSG(HAL_MSG_DBG, "write 0x%08X = 0x%04X\n", ((uint)base+(idx)*4), val); ms_writew(val,((uint)base+(idx)*4));} while(0)
#endif

void cava_hal_init(ceva_hal_handle *handle, phys_addr_t base_sys, phys_addr_t base_axi2miu0, phys_addr_t base_axi2miu1, phys_addr_t base_axi2miu2, phys_addr_t base_axi2miu3)
{
    memset(handle, 0, sizeof(ceva_hal_handle));
    handle->base_sys = base_sys;
    handle->base_axi2miu0 = base_axi2miu0;
    handle->base_axi2miu1 = base_axi2miu1;
    handle->base_axi2miu2 = base_axi2miu2;
    handle->base_axi2miu3 = base_axi2miu3;
}

void ceva_hal_enable_irq(ceva_hal_handle *handle, CEVA_HAL_IRQ_TARGET target, CEVA_HAL_IRQ irq)
{
    u16 irq0 = LOW_U16(irq);
    u16 irq1 = HIGH_U16(irq);

    switch(target)
    {
      case CEVA_HAL_IRQ_TARGET_ARM:
        handle->reg_sys.reg03 = REGR(handle->base_sys, 0x03);
        handle->reg_sys.reg_ceva2riu_int_en |= irq0;
        REGW(handle->base_sys, 0x03, handle->reg_sys.reg03);

        handle->reg_sys.reg04 = REGR(handle->base_sys, 0x04);
        handle->reg_sys.reg_ceva2riu_int_en2 |= irq1;
        REGW(handle->base_sys, 0x04, handle->reg_sys.reg04);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT0:
        handle->reg_sys.reg05 = REGR(handle->base_sys, 0x05);
        handle->reg_sys.reg_ceva_int0_en |= irq0;
        REGW(handle->base_sys, 0x05, handle->reg_sys.reg05);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT1:
        handle->reg_sys.reg06 = REGR(handle->base_sys, 0x06);
        handle->reg_sys.reg_ceva_int1_en |= irq0;
        REGW(handle->base_sys, 0x06, handle->reg_sys.reg06);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT2:
        handle->reg_sys.reg07 = REGR(handle->base_sys, 0x07);
        handle->reg_sys.reg_ceva_int2_en |= irq0;
        REGW(handle->base_sys, 0x07, handle->reg_sys.reg07);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_NMI:
        handle->reg_sys.reg08 = REGR(handle->base_sys, 0x08);
        handle->reg_sys.reg_ceva_nmi_en |= irq0;
        REGW(handle->base_sys, 0x08, handle->reg_sys.reg08);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_VINT:
        handle->reg_sys.reg09 = REGR(handle->base_sys, 0x09);
        handle->reg_sys.reg_ceva_vint_en |= irq0;
        REGW(handle->base_sys, 0x09, handle->reg_sys.reg09);
        break;

      default:
        break;
    }
}

CEVA_HAL_IRQ ceva_hal_get_irq(ceva_hal_handle *handle, CEVA_HAL_IRQ_TARGET target)
{
    u32 enable = 0;
    u32 irq;

    switch(target)
    {
      case CEVA_HAL_IRQ_TARGET_ARM:
        handle->reg_sys.reg03 = REGR(handle->base_sys, 0x03);
        handle->reg_sys.reg04 = REGR(handle->base_sys, 0x04);
        enable = MAKE_U32(handle->reg_sys.reg_ceva2riu_int_en2, handle->reg_sys.reg_ceva2riu_int_en);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT0:
        handle->reg_sys.reg05 = REGR(handle->base_sys, 0x05);
        enable = MAKE_U32(0, handle->reg_sys.reg_ceva_int0_en);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT1:
        handle->reg_sys.reg06 = REGR(handle->base_sys, 0x06);
        enable = MAKE_U32(0, handle->reg_sys.reg_ceva_int1_en);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_INT2:
        handle->reg_sys.reg07 = REGR(handle->base_sys, 0x07);
        enable = MAKE_U32(0, handle->reg_sys.reg_ceva_int2_en);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_NMI:
        handle->reg_sys.reg08 = REGR(handle->base_sys, 0x08);
        enable = MAKE_U32(0, handle->reg_sys.reg_ceva_nmi_en);
        break;

      case CEVA_HAL_IRQ_TARGET_XM6_VINT:
        handle->reg_sys.reg09 = REGR(handle->base_sys, 0x09);
        enable = MAKE_U32(0, handle->reg_sys.reg_ceva_vint_en);
        break;

      default:
        break;
    }

    handle->reg_sys.reg18 = REGR(handle->base_sys, 0x18);
    handle->reg_sys.reg19 = REGR(handle->base_sys, 0x19);
    irq = MAKE_U32(handle->reg_sys.reg_ceva_is2, handle->reg_sys.reg_ceva_is);

    return enable & irq;
}

void ceva_hal_reset_xm6(ceva_hal_handle *handle)
{
    // ensure only bit 0 is set
    // enable = enable ? 1:0;

    handle->reg_sys.reg_ceva_en = 0;
    REGW(handle->base_sys, 0x00, handle->reg_sys.reg00);

    handle->reg_sys.reg_rstz = ~CEVA_HAL_RESET_XM6_ALL;
    REGW(handle->base_sys, 0x02, handle->reg_sys.reg02);

    // delay 1us
    udelay(1);
}

void ceva_hal_enable_xm6(ceva_hal_handle *handle, void *boot_addr)
{
    u32 addr  = Chip_Phys_to_MIU((uintptr_t)boot_addr);

    handle->reg_sys.reg_ceva_en = 1;
    REGW(handle->base_sys, 0x00, handle->reg_sys.reg00);

    handle->reg_sys.reg_ceva_boot = 1;
    REGW(handle->base_sys, 0x15, handle->reg_sys.reg15);

    handle->reg_sys.reg_ceva_vector_low = LOW_U16(addr);
    REGW(handle->base_sys, 0x16, handle->reg_sys.reg16);

    handle->reg_sys.reg_ceva_vector_high = HIGH_U16(addr);
    REGW(handle->base_sys, 0x17, handle->reg_sys.reg17);

    handle->reg_sys.reg_rstz = CEVA_HAL_RESET_XM6_ALL;
    REGW(handle->base_sys, 0x02, handle->reg_sys.reg02);
}

void ceva_hal_reset_warp(ceva_hal_handle *handle)
{
    handle->reg_sys.reg_warp_en = 0;
    REGW(handle->base_sys, 0x40, handle->reg_sys.reg40);

    handle->reg_sys.reg_rstz_warp = 0; // ~CEVA_HAL_RESET_WARP_ALL
    REGW(handle->base_sys, 0x42, handle->reg_sys.reg42);

    // delay 1us
    udelay(1);
}

void ceva_hal_enable_warp(ceva_hal_handle *handle)
{
    handle->reg_sys.reg_warp_en = 0;
    REGW(handle->base_sys, 0x40, handle->reg_sys.reg40);

    handle->reg_sys.reg_rstz_warp = 0;
    REGW(handle->base_sys, 0x42, handle->reg_sys.reg42);

    // delay 1us
    udelay(1);

    handle->reg_sys.reg_warp_en = 1;
    REGW(handle->base_sys, 0x40, handle->reg_sys.reg40);

    // handle->reg_sys.reg_rstz_warp = 0xFFFF;
    handle->reg_sys.reg42 = CEVA_HAL_RESET_WARP_ALL;
    REGW(handle->base_sys, 0x42, handle->reg_sys.reg42);

    // delay 1us
    udelay(1);
}

void ceva_hal_set_axi2miu(ceva_hal_handle *handle)
{
    unsigned short axi2miu2_data = 0;
    unsigned short axi2miu3_data = 0;

    axi2miu2_data = REGR(handle->base_axi2miu2, 0x02);
    axi2miu3_data = REGR(handle->base_axi2miu3, 0x02);

    axi2miu2_data = axi2miu2_data | 0x0001;
    axi2miu3_data = axi2miu3_data | 0x0001;

    REGW(handle->base_axi2miu2, 0x02, axi2miu2_data);
    REGW(handle->base_axi2miu3, 0x02, axi2miu2_data);

    REGW(handle->base_axi2miu2, 0x01, 0x1010);
    REGW(handle->base_axi2miu3, 0x01, 0x1010);
}
