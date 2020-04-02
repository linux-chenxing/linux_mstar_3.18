#if defined(MSOS_TYPE_LINUX)
#include "mdrv_types.h"
#define EXPORT_SYMBOL(x)
#elif defined(__KERNEL__)
#include <linux/module.h>
#include <linux/slab.h>
#include "mdrv_types.h"
#include "mstar/mstar_chip.h"
#elif defined(MSOS_TYPE_NOS)
#include "datatype.h"
#include "zlib_hwcfg.h"
#define EXPORT_SYMBOL(x)
#endif

#include "mhal_zdec.h"
#include "mstar_zdec.h"


#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(MSOS_TYPE_LINUX)
#define ZMEM_BASE                       0x1000000UL // 16M
#endif

#define ZMEM_SIZE                       0x8000UL    // 32K
#define ZMEM_ALIGNMENT                  16UL
#define DICTIONARY_ALIGNMENT            16UL
#define MIU_WORD_BITS                   128UL

#define GET_REG_PAGE_SIZE(byte_size)    (byte_size >> 10)  // see 0x46, reg_zdec_page_size, reg_zdec_out_page_size

U8    zmem_miu  = 0;
void* zmem_virt = NULL;
unsigned long zmem_phys = 0;

#if defined(MSOS_TYPE_NOS)
extern zdec_hw_cfg_t g_hwcfg;
#endif


int mstar_zdec_allocate_zmem(void)
{
#if defined(MSOS_TYPE_LINUX)
    zmem_miu  = 0;
    zmem_phys = ZMEM_BASE;

#elif defined(__KERNEL__)
    if (NULL == zmem_virt)
    {
        zmem_virt = kmalloc(ZMEM_SIZE + ZMEM_ALIGNMENT, GFP_KERNEL);
        if (NULL == zmem_virt)
        {
            return ZDEC_E_OUT_OF_MEMORY;
        }
    }

    zmem_phys = virt_to_phys(zmem_virt);
    zmem_phys = (zmem_phys + ZMEM_ALIGNMENT - 1) & ~(ZMEM_ALIGNMENT - 1);

#ifdef MSTAR_MIU2_BUS_BASE
    if (zmem_phys >= MSTAR_MIU2_BUS_BASE)
    {
        zmem_miu = 2;
        zmem_phys -= MSTAR_MIU2_BUS_BASE;
    }
    else if (zmem_phys >= MSTAR_MIU1_BUS_BASE)
#else
    if (zmem_phys >= MSTAR_MIU1_BUS_BASE)
#endif
    {
        zmem_miu = 1;
        zmem_phys -= MSTAR_MIU1_BUS_BASE;
    }
    else
    {
        zmem_miu = 0;
        zmem_phys -= MSTAR_MIU0_BUS_BASE;
    }

#elif defined(MSOS_TYPE_NOS)
    zmem_miu  = g_hwcfg.lzd_reg.zmem_miu_sel;
    zmem_phys = g_hwcfg.lzd_reg.zmem_saddr;
#endif

    return 0;
}


int mstar_zdec_init(
    zdec_mode_t   zmode,
    unsigned int  preset_dict_size,
    unsigned char fcie_handshake,
    unsigned int  shift_byte,
    unsigned char shift_bit,
    unsigned char obuf_miu,
    unsigned int  obuf_addr,
    unsigned int  obuf_size,
    unsigned int  in_page_size,
    unsigned int  out_page_size)
{
    U8 dec_mode = CONTIGUOUS_MODE;
    U8 tbl_fmt = EMMC_TABLE;
    U8 op_mode = DECODING;

    switch (zmode)
    {
        case ZMODE_DEC_CONT:
            break;
        case ZMODE_DEC_SCATTER_EMMC:
            dec_mode = SCATTER_MODE;
            break;
        case ZMODE_DEC_SCATTER_NAND:
            dec_mode = SCATTER_MODE;
            tbl_fmt = NAND_TABLE;
            break;
        case ZMODE_LOAD_DICT_MIU:
            op_mode = LOADING_PRESET_DICT_MIU;
            break;
        case ZMODE_LOAD_DICT_RIU:
            op_mode = LOADING_PRESET_DICT_RIU;
            break;
        default:
            return ZDEC_E_INVALID_MODE;
    }

    if ((0 != (preset_dict_size % DICTIONARY_ALIGNMENT))
        || (DECODING != op_mode && 0 == preset_dict_size))
    {
        return ZDEC_E_INVALID_DICTIONARY_SIZE;
    }

    if (0 != mstar_zdec_allocate_zmem())
    {
        return ZDEC_E_OUT_OF_MEMORY;
    }

    MHal_ZDEC_Conf_Reset();
    MHal_ZDEC_Conf_Zmem(zmem_miu, zmem_phys, ZMEM_SIZE);
    MHal_ZDEC_Conf_FCIE_Handshake(0 == fcie_handshake ? 0: 1);
    MHal_ZDEC_Conf_Input_Shift(
        shift_byte / BITS2BYTE(MIU_WORD_BITS),
        shift_byte % BITS2BYTE(MIU_WORD_BITS),
        shift_bit);
    MHal_ZDEC_Conf_Preset_Dictionary((U32)preset_dict_size);

    if (DECODING == op_mode)
    {
        if (CONTIGUOUS_MODE == dec_mode)
        {
            MHal_ZDEC_Conf_Contiguous_Mode(obuf_miu, obuf_addr, obuf_size);
        }
        else // SCATTER_MODE
        {
            MHal_ZDEC_Conf_Scatter_Mode(
                obuf_miu,
                obuf_addr,
                tbl_fmt,
                GET_REG_PAGE_SIZE(in_page_size),
                GET_REG_PAGE_SIZE(out_page_size));
        }
    }

    return MHal_ZDEC_Start_Operation(op_mode);
}
EXPORT_SYMBOL(mstar_zdec_init);


int mstar_zdec_feed(unsigned char last, unsigned char miu, unsigned int sadr, unsigned int size)
{
    MHal_ZDEC_Feed_Data(last, miu, sadr, size);
    return ZDEC_OK;
}
EXPORT_SYMBOL(mstar_zdec_feed);


int mstar_zdec_check_internal_buffer(void)
{
    return MHal_ZDEC_Check_Internal_Buffer();
}
EXPORT_SYMBOL(mstar_zdec_check_internal_buffer);


int mstar_zdec_check_adma_table_done(void)
{
    return MHal_ZDEC_Check_ADMA_Table_Done();
}
EXPORT_SYMBOL(mstar_zdec_check_adma_table_done);


int mstar_zdec_check_dict_miu_load_done(void)
{
    return MHal_ZDEC_Check_MIU_Load_Dict_Done();
}
EXPORT_SYMBOL(mstar_zdec_check_dict_miu_load_done);


int mstar_zdec_check_decode_done(void)
{
    return MHal_ZDEC_Check_Decode_Done();
}
EXPORT_SYMBOL(mstar_zdec_check_decode_done);


int mstar_zdec_riu_load_dict(unsigned char* dict)
{
    return MHal_ZDEC_RIU_Load_Preset_Dictionary(dict);
}
EXPORT_SYMBOL(mstar_zdec_riu_load_dict);


void mstar_zdec_release()
{
#if defined(__KERNEL__)
    if (NULL != zmem_virt)
    {
        kfree(zmem_virt);
    }
#endif
    zmem_phys = 0;
    zmem_virt = NULL;
}
EXPORT_SYMBOL(mstar_zdec_release);

