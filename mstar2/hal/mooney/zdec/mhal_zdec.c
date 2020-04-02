#if defined(MSOS_TYPE_LINUX)
#include <string.h>
#include "mdrv_types.h"
#elif defined(__KERNEL__)
#include <linux/string.h>
#include "mdrv_types.h"
#elif defined(MSOS_TYPE_NOS)
#include <string.h>
#include "datatype.h"
#include "zlib_hwcfg.h"
#endif

#include "mhal_zdec_reg.h"
#include "mhal_zdec.h"


#if defined(MSOS_TYPE_LINUX)
U32 u32MMIOBaseAdr = 0;
#endif

ZDEC_REGS g_regs;


//-------------------------------------------------------------------------------------------------
// Local Functions
//-------------------------------------------------------------------------------------------------

void mhal_zdec_power_on(void)
{
    BANK_REG_WRITE(CKG_REG_BANK, REG_CKG_ZDEC, 0);
}


void mhal_zdec_power_off(void)
{
    BANK_REG_WRITE(CKG_REG_BANK, REG_CKG_ZDEC, (CKG_ZDEC_VLD_GATE | CKG_ZDEC_LZD_GATE));
}


void mhal_zdec_sw_reset(void)
{
    g_regs.reg_zdec_sw_rst = 0;
    ZDEC_REG_WRITE(0x00, g_regs.reg00);
    g_regs.reg_zdec_sw_rst = 1;
    ZDEC_REG_WRITE(0x00, g_regs.reg00);
}


void mhal_zdec_set_hw_miu_selection(void)
{
#ifndef MSOS_TYPE_NOS
    BANK_REG(MIU_REG_BANK, REG_MIU_SEL_ZDEC) |= MIU_SEL_ZDEC_0;
    BANK_REG(MIU_REG_BANK, REG_MIU_SEL_ZDEC) |= MIU_SEL_ZDEC_1;
#else
    BANK_REG_WRITE(MIU_REG_BANK, REG_MIU_SEL_ZDEC, (MIU_SEL_ZDEC_0 | MIU_SEL_ZDEC_1));
#endif
}


void mhal_zdec_clear_all_interrupts(void)
{
    ZDEC_REG_WRITE(REG_ZDEC_IRQ_CLEAR, ZDEC_IRQ_ALL);
}


void mhal_zdec_set_all_regs(void)
{
    ZDEC_REG_WRITE(0x00, g_regs.reg00);
    ZDEC_REG_WRITE(0x02, g_regs.reg02);
    ZDEC_REG_WRITE(0x06, g_regs.reg06);
    ZDEC_REG_WRITE(0x07, g_regs.reg07);
    ZDEC_REG_WRITE(0x08, g_regs.reg08);
    ZDEC_REG_WRITE(0x09, g_regs.reg09);
    ZDEC_REG_WRITE(0x0a, g_regs.reg0a);
    ZDEC_REG_WRITE(0x0b, g_regs.reg0b);
    ZDEC_REG_WRITE(0x0c, g_regs.reg0c);
    ZDEC_REG_WRITE(0x0f, g_regs.reg0f);
    ZDEC_REG_WRITE(0x10, g_regs.reg10);
    ZDEC_REG_WRITE(0x11, g_regs.reg11);
    ZDEC_REG_WRITE(0x12, g_regs.reg12);
    ZDEC_REG_WRITE(0x13, g_regs.reg13);
    ZDEC_REG_WRITE(0x15, g_regs.reg15);
    ZDEC_REG_WRITE(0x16, g_regs.reg16);
    ZDEC_REG_WRITE(0x17, g_regs.reg17);
    ZDEC_REG_WRITE(0x18, g_regs.reg18);
    ZDEC_REG_WRITE(0x19, g_regs.reg19);
    ZDEC_REG_WRITE(0x1a, g_regs.reg1a);
    ZDEC_REG_WRITE(0x1b, g_regs.reg1b);
    ZDEC_REG_WRITE(0x1c, g_regs.reg1c);
    ZDEC_REG_WRITE(0x1d, g_regs.reg1d);
    ZDEC_REG_WRITE(0x1e, g_regs.reg1e);
    ZDEC_REG_WRITE(0x20, g_regs.reg20);
    ZDEC_REG_WRITE(0x21, g_regs.reg21);
    ZDEC_REG_WRITE(0x22, g_regs.reg22);
    ZDEC_REG_WRITE(0x28, g_regs.reg28);
    ZDEC_REG_WRITE(0x2a, g_regs.reg2a);
    ZDEC_REG_WRITE(0x30, g_regs.reg30);
    ZDEC_REG_WRITE(0x31, g_regs.reg31);
    ZDEC_REG_WRITE(0x32, g_regs.reg32);
    ZDEC_REG_WRITE(0x33, g_regs.reg33);
    ZDEC_REG_WRITE(0x34, g_regs.reg34);
    ZDEC_REG_WRITE(0x35, g_regs.reg35);
    ZDEC_REG_WRITE(0x36, g_regs.reg36);
    ZDEC_REG_WRITE(0x39, g_regs.reg39);
    ZDEC_REG_WRITE(0x40, g_regs.reg40);
    ZDEC_REG_WRITE(0x41, g_regs.reg41);
    ZDEC_REG_WRITE(0x42, g_regs.reg42);
    ZDEC_REG_WRITE(0x43, g_regs.reg43);
    ZDEC_REG_WRITE(0x44, g_regs.reg44);
    ZDEC_REG_WRITE(0x46, g_regs.reg46);
    ZDEC_REG_WRITE(0x50, g_regs.reg50);
    ZDEC_REG_WRITE(0x51, g_regs.reg51);
    ZDEC_REG_WRITE(0x54, g_regs.reg54);
    ZDEC_REG_WRITE(0x58, g_regs.reg58);
    ZDEC_REG_WRITE(0x59, g_regs.reg59);
    ZDEC_REG_WRITE(0x5a, g_regs.reg5a);
    ZDEC_REG_WRITE(0x68, g_regs.reg68);
    ZDEC_REG_WRITE(0x71, g_regs.reg71);
    ZDEC_REG_WRITE(0x72, g_regs.reg72);
    ZDEC_REG_WRITE(0x75, g_regs.reg75);
    ZDEC_REG_WRITE(0x78, g_regs.reg78);
    ZDEC_REG_WRITE(0x79, g_regs.reg79);
    ZDEC_REG_WRITE(0x7a, g_regs.reg7a);
    ZDEC_REG_WRITE(0x7b, g_regs.reg7b);
    ZDEC_REG_WRITE(0x7c, g_regs.reg7c);
    ZDEC_REG_WRITE(0x7d, g_regs.reg7d);
    ZDEC_REG_WRITE(0x7e, g_regs.reg7e);
    ZDEC_REG_WRITE(0x7f, g_regs.reg7f);
}


//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------

void MHal_ZDEC_Conf_Reset(void)
{
    memset(&g_regs, 0, sizeof(g_regs));

    //
    // power on defaults
    //
    g_regs.reg_zdec_sw_rst = 1;
    g_regs.reg_zdec_early_done_thd = 3;
    g_regs.reg_zdec_ibuf_burst_len = 3;
    g_regs.reg_zdec_ibuf_reverse_bit = 1;
    g_regs.reg_zdec_obuf_burst_length = 3;
    g_regs.reg_zdec_obuf_timeout = 8;
    g_regs.reg_zdec_obuf_cnt_no_wait_last_done_z = 1;
    g_regs.reg_zdec_zmem_burst_len = 4;

    //
    // same value for all scenerios
    //

    // OBUF
    g_regs.reg_zdec_obuf_datacnt_en = 1;
    g_regs.reg_zdec_obuf_burst_split = 1;

    // LZD write-out byte count
    g_regs.reg_zdec_dec_cnt_sel = 1;

    // CRC
    g_regs.reg_zdec_crc_en = 1;
    g_regs.reg_zdec_crc_mode = 1;
    g_regs.reg_zdec_crc_sel = 0;

    // ECO item
    g_regs.reg_zdec_spare00 = 3;
}


void MHal_ZDEC_Conf_Zmem(U8 miu, U32 addr, U32 size)
{
    g_regs.reg_zdec_zmem_miu_sel  = miu;
    g_regs.reg_zdec_zmem_saddr_lo = GET_LOWORD(addr);
    g_regs.reg_zdec_zmem_saddr_hi = GET_HIWORD(addr);
    g_regs.reg_zdec_zmem_eaddr_lo = GET_LOWORD(addr + size - 1);
    g_regs.reg_zdec_zmem_eaddr_hi = GET_HIWORD(addr + size - 1);
}


void MHal_ZDEC_Conf_FCIE_Handshake(U8 enable)
{
    g_regs.reg_zdec_fcie_hw_talk = (0 == enable) ? 0 : 1;
}


void MHal_ZDEC_Conf_Input_Shift(U32 skip_words_cnt, U8 shift_byte, U8 shift_bit)
{
    g_regs.reg_zdec_ibuf_skip_words_cnt_lo = GET_LOWORD(skip_words_cnt);
    g_regs.reg_zdec_ibuf_skip_words_cnt_hi = GET_HIWORD(skip_words_cnt);
    g_regs.reg_zdec_ibuf_shift_byte = shift_byte;
    g_regs.reg_zdec_ibuf_shift_bit = shift_bit;
}


void MHal_ZDEC_Conf_Preset_Dictionary(U32 size)
{
    g_regs.reg_zdec_preset_dic_bytes = ALIGN16(size);
}


void MHal_ZDEC_Conf_Contiguous_Mode(U8 obuf_miu, U32 obuf_addr, U32 obuf_size)
{
    g_regs.reg_zdec_iobuf_mode = CONTIGUOUS_MODE;
    g_regs.reg_zdec_obuf_miu_sel = obuf_miu;
    g_regs.reg_zdec_obuf_sadr_lo = GET_LOWORD(obuf_addr);
    g_regs.reg_zdec_obuf_sadr_hi = GET_HIWORD(obuf_addr);
    g_regs.reg_zdec_obuf_len_lo = GET_LOWORD(obuf_size);
    g_regs.reg_zdec_obuf_len_hi = GET_HIWORD(obuf_size);
}


void MHal_ZDEC_Conf_Scatter_Mode(U8 dst_tbl_miu, U32 dst_tbl_addr, U8 table_format, U8 in_nand_page_size, U8 out_nand_page_size)
{
    g_regs.reg_zdec_iobuf_mode = SCATTER_MODE;
    g_regs.reg_zdec_dst_tbl_miu_sel = dst_tbl_miu;
    g_regs.reg_zdec_dst_tbl_addr_lo = GET_LOWORD(dst_tbl_addr);
    g_regs.reg_zdec_dst_tbl_addr_hi = GET_HIWORD(dst_tbl_addr);

    if (EMMC_TABLE == table_format)
    {
        g_regs.reg_zdec_tbl_format = EMMC_TABLE;
    }
    else
    {
        g_regs.reg_zdec_tbl_format = NAND_TABLE;
        g_regs.reg_zdec_page_size = in_nand_page_size;
        g_regs.reg_zdec_out_page_size = out_nand_page_size;
    }
}


int MHal_ZDEC_Start_Operation(U8 op_mode)
{
    mhal_zdec_power_on();
    mhal_zdec_sw_reset();
    mhal_zdec_set_hw_miu_selection();
    mhal_zdec_clear_all_interrupts();
    mhal_zdec_set_all_regs();

    if (DECODING == op_mode)
    {
        ZDEC_REG_WRITE(REG_ZDEC_GLOBAL_WOC, ZDEC_WOC_FRAME_ST);
    }
    else if (LOADING_PRESET_DICT_MIU == op_mode)
    {
        ZDEC_REG_WRITE(REG_ZDEC_GLOBAL_WOC, ZDEC_WOC_LOAD_DIC);
    }

    return 0;
}


void MHal_ZDEC_Feed_Data(U8 last, U8 miu, U32 sadr, U32 size)
{
    if (CONTIGUOUS_MODE == g_regs.reg_zdec_iobuf_mode)
    {
        g_regs.reg_zdec_ibuf_cmd_miu_sel = miu;
        g_regs.reg_zdec_ibuf_cmd_last = (0 == last) ? 0 : 1;
        g_regs.reg_zdec_ibuf_cmd_sadr_lo = GET_LOWORD(sadr);
        g_regs.reg_zdec_ibuf_cmd_sadr_hi = GET_HIWORD(sadr);
        g_regs.reg_zdec_ibuf_cmd_size_lo = GET_LOWORD(size);
        g_regs.reg_zdec_ibuf_cmd_size_hi = GET_HIWORD(size);
        ZDEC_REG_WRITE(0x0c, g_regs.reg0c);
        ZDEC_REG_WRITE(0x08, g_regs.reg08);
        ZDEC_REG_WRITE(0x09, g_regs.reg09);
        ZDEC_REG_WRITE(0x0a, g_regs.reg0a);
        ZDEC_REG_WRITE(0x0b, g_regs.reg0b);
        ZDEC_REG_WRITE(REG_ZDEC_IBUF_WOC_CMD_WRITE, ZDEC_IBUF_WOC_CMD_WRITE);
    }
    else if (SCATTER_MODE == g_regs.reg_zdec_iobuf_mode)
    {
        g_regs.reg_zdec_src_tbl_miu_sel = miu;
        g_regs.reg_zdec_src_tbl_last = (0 == last) ? 0 : 1;
        g_regs.reg_zdec_src_tbl_addr_lo = GET_LOWORD(sadr);
        g_regs.reg_zdec_src_tbl_addr_hi = GET_HIWORD(sadr);
        ZDEC_REG_WRITE(0x40, g_regs.reg40);
        ZDEC_REG_WRITE(0x41, g_regs.reg41);
        ZDEC_REG_WRITE(0x42, g_regs.reg42);
        ZDEC_REG_WRITE(REG_ZDEC_WOC_NXT_SRC_TBL_VLD, ZDEC_WOC_NXT_SRC_TBL_VLD);
    }
}


int MHal_ZDEC_Check_Internal_Buffer(void)
{
    U16 reg_val = ZDEC_REG(REG_ZDEC_IBUF_R_CMD);
    if ((0 == (reg_val & ZDEC_IBUF_R_CMD_FULL))
        && (((reg_val & ((unsigned short)(0x00ff) << 4)) >> 4) < 2))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


int MHal_ZDEC_Check_ADMA_Table_Done(void)
{
    U16 reg_val = ZDEC_REG(REG_ZDEC_IRQ_FINAL_S);
    if (0 == (reg_val & ZDEC_IRQ_ADMA_TABLE_DONE))
    {
        return -1;
    }
    else
    {
        ZDEC_REG_WRITE(REG_ZDEC_IRQ_CLEAR, ZDEC_IRQ_ADMA_TABLE_DONE);
        return 0;
    }
}


int MHal_ZDEC_Check_Last_ADMA_Table_Done(void)
{
    U16 reg_val = ZDEC_REG(REG_ZDEC_IRQ_FINAL_S);
    if (0 == (reg_val & ZDEC_IRQ_LAST_ADMA_TABLE_DONE))
    {
        return -1;
    }
    else
    {
        ZDEC_REG_WRITE(REG_ZDEC_IRQ_CLEAR, ZDEC_IRQ_LAST_ADMA_TABLE_DONE);
        return 0;
    }
}


int MHal_ZDEC_Check_MIU_Load_Dict_Done(void)
{
    U16 reg_val = ZDEC_REG(REG_ZDEC_IRQ_FINAL_S);
    if (0 == (reg_val & ZDEC_IRQ_PRESET_DICTIONARY_LOAD_DONE))
    {
        return -1;
    }
    else
    {
        ZDEC_REG_WRITE(REG_ZDEC_IRQ_CLEAR, ZDEC_IRQ_PRESET_DICTIONARY_LOAD_DONE);
        return 0;
    }
}


int MHal_ZDEC_Check_Decode_Done(void)
{
    U16 reg_val = ZDEC_REG(REG_ZDEC_IRQ_FINAL_S);
    if (0 == (reg_val & ZDEC_IRQ_DECODE_DONE))
    {
        return -1;
    }
    else
    {
        mhal_zdec_clear_all_interrupts();
        mhal_zdec_power_off();

        // The value stored in reg_zdec_r_dec_cnt is always short by 1 byte.
        return (((int)(ZDEC_REG(REG_ZDEC_R_DEC_CNT_HI)) << 16) | ZDEC_REG(REG_ZDEC_R_DEC_CNT_LO)) + 1;
    }
}


int MHal_ZDEC_RIU_Load_Preset_Dictionary(U8* dict)
{
    int i = 0;
    unsigned short reg_val;

    g_regs.reg_zdec_dic_riu_sel = 1;    // reg_zdec_dic_riu_sel[0] => 1; switch on preset dictionary RIU mode
    g_regs.reg_zdec_dic_riu_rw = 1;     // reg_zdec_dic_riu_rw[8]  => 1; writing mode (DRAM => SRAM)
    ZDEC_REG_WRITE(0x5a, g_regs.reg5a);
    ZDEC_REG_WRITE(0x58, g_regs.reg58);

    for (i = 0; i < g_regs.reg_zdec_preset_dic_bytes; ++i)
    {
        g_regs.reg_zdec_dic_riu_addr = i;     // i: byte address in SRAM
        g_regs.reg_zdec_dic_riu_wd = dict[i]; // src_data[i]: byte value to be written to SRAM (DRAM => register)
        ZDEC_REG_WRITE(0x59, g_regs.reg59);
        ZDEC_REG_WRITE(0x58, g_regs.reg58);
        ZDEC_REG_WRITE(REG_ZDEC_GLOBAL_WOC, ZDEC_DIC_RIU_WOC); // Signal HW to process the data (register => SRAM)

        do
        {
            reg_val = ZDEC_REG(REG_ZDEC_DIC_RIU_RD);
        } while (0 == (reg_val & ZDEC_DIC_RIU_RDY)); // reg_zdec_dic_riu_rdy == 1? ; Wait for HW to be ready for next byte
    }

    g_regs.reg_zdec_dic_riu_sel = 0;
    ZDEC_REG_WRITE(0x5a, g_regs.reg5a); // reg_zdec_dic_riu_sel[0] => 0; switch off preset dictionary RIU mode
    return 0;
}

