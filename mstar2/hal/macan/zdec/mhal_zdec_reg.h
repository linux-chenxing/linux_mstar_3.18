#ifndef __MHAL_ZDEC_REG_H__
#define __MHAL_ZDEC_REG_H__

#ifndef BIT
#define BIT(x) (1UL << (x))
#endif

#if defined(MSOS_TYPE_LINUX)
#define NONPM_BASE                           u32MMIOBaseAdr
#elif defined(__KERNEL__) || defined(MSOS_TYPE_NOS)
#define NONPM_BASE                           0xfd200000
#endif

#define MIU_REG_BANK                         0x1006      // MIU2 of Monaco
#define CKG_REG_BANK                         0x100A      // CLKGEN2 of Monaco
#define ZDEC_REG_BANK                        0x123e


// The following macros handle registers in 16-bit addressing mode.
#define GET_ADDR(_bank, _offset)             (NONPM_BASE + (((_bank) - 0x1000) << 9) + ((_offset) << 2))
#define REG(_addr)                           (*(volatile unsigned short*)(_addr))
#define BANK_REG(_bank, _offset)             REG(GET_ADDR((_bank), (_offset)))
#define ZDEC_REG(_offset)                    BANK_REG(ZDEC_REG_BANK, (_offset))

#define REG_WRITE(_addr, _val)               { REG(_addr) = (unsigned short)(_val); }

#if defined(MSOS_TYPE_LINUX) || defined(__KERNEL__)
#define BANK_REG_WRITE(_bank, _offset, _val) REG_WRITE(GET_ADDR((_bank), (_offset)), (_val));
#elif defined(MSOS_TYPE_NOS) && defined(TVTOOL_SCRIPT)
#define BANK_REG_WRITE(_bank, _offset, _val) \
{ \
    REG_WRITE(GET_ADDR((_bank), (_offset)), (_val)); \
    printf("wriu -w 0x%04X%02X 0x%04X\n", (_bank), (_offset) << 1, (_val)); \
}
#elif defined(MSOS_TYPE_NOS)
#define BANK_REG_WRITE(_bank, _offset, _val) \
{ \
    REG_WRITE(GET_ADDR((_bank), (_offset)), (_val)); \
    printf("[%04X] Write 0x%04X: %04X\n", (_bank), (_offset), (_val)); \
}
#endif

#define ZDEC_REG_WRITE(_offset, _val)        BANK_REG_WRITE(ZDEC_REG_BANK, (_offset), (_val));


//
// MIU registers
//

#define REG_MIU_SEL_ZDEC                     0x7b
    #define MIU_SEL_ZDEC_0                        BIT(4)
    #define MIU_SEL_ZDEC_1                        BIT(5)


//
// CLKGEN registers for ZDEC
//

#define REG_CKG_ZDEC                         0x70
    #define CKG_ZDEC_VLD_GATE                     BIT(0)
    #define CKG_ZDEC_LZD_GATE                     BIT(4)


//
// ZDEC read-only registers
//

#define REG_ZDEC_IBUF_R_CMD                  0x0d
    #define ZDEC_IBUF_R_CMD_FULL                  BIT(0)  // reg_zdec_ibuf_r_cmd_full
#define REG_ZDEC_R_DEC_CNT_LO                0x25         // reg_zdec_r_dec_cnt[15:0]
#define REG_ZDEC_R_DEC_CNT_HI                0x26         // reg_zdec_r_dec_cnt[31:16]
#define REG_ZDEC_DIC_RIU_RD                  0x5b
    #define ZDEC_DIC_RIU_RDY                      BIT(8)  // reg_zdec_dic_riu_rdy
#define REG_ZDEC_CRC_R_RESULT                0x69


//
// ZDEC IRQ registers
//

#define REG_ZDEC_IRQ_CLEAR                   0x70         // reg_zdec_irq_clear (woc)
#define REG_ZDEC_IRQ_FINAL_S                 0x73         // reg_zdec_irq_final_s
    #define ZDEC_IRQ_DECODE_DONE                  BIT(0)
    #define ZDEC_IRQ_ADMA_TABLE_DONE              BIT(2)
    #define ZDEC_IRQ_LAST_ADMA_TABLE_DONE         BIT(3)
    #define ZDEC_IRQ_PRESET_DICTIONARY_LOAD_DONE  BIT(4)
    #define ZDEC_IRQ_ZDEC_EXCEPTION               BIT(5)
    #define ZDEC_IRQ_ALL                          0x3D


//
// ZDEC woc registers
//

#define REG_ZDEC_GLOBAL_WOC                  0x01
    #define ZDEC_WOC_STOP                         BIT(0)  // reg_zdec_woc_stop
    #define ZDEC_WOC_FRAME_ST                     BIT(1)  // reg_zdec_woc_frame_st
    #define ZDEC_WOC_PARK_MIU                     BIT(2)  // reg_zdec_woc_park_miu
    #define ZDEC_VHT_SW_CTRL_WOC_EN               BIT(3)  // reg_zdec_vht_sw_ctrl_woc_en
    #define ZDEC_VHT_SW_CTRL_WOC_DONE_CLR         BIT(4)  // reg_zdec_vht_sw_ctrl_woc_done_clr
    #define ZDEC_OBUF_DATA_CNT_GET_WOC            BIT(5)  // reg_zdec_obuf_data_cnt_get_woc
    #define ZDEC_DEBUG_WOC_TRIG                   BIT(6)  // reg_zdec_debug_woc_trig
    #define ZDEC_DIC_RIU_WOC                      BIT(7)  // reg_zdec_dic_riu_woc
    #define ZDEC_WOC_LOAD_DIC                     BIT(8)  // reg_zdec_woc_load_dic
    #define ZDEC_WOC_NXT_DST_TBL_VLD              BIT(9)  // reg_zdec_woc_nxt_dst_tbl_vld
    #define ZDEC_WOC_SET_FCIE_SWPTR               BIT(10) // reg_zdec_woc_set_fcie_swptr
#define REG_ZDEC_IBUF_WOC_CMD_WRITE          0x0e
    #define ZDEC_IBUF_WOC_CMD_WRITE               BIT(0)  // reg_zdec_ibuf_woc_cmd_write
#define REG_ZDEC_WOC_NXT_SRC_TBL_VLD         0x45
    #define ZDEC_WOC_NXT_SRC_TBL_VLD              BIT(0)  // reg_zdec_woc_nxt_src_tbl_vld


//
// ZDEC writable registers
//

typedef struct _zdec_reg {
    union {
        struct {
            U16 reg_zdec_sw_rst:1;
            U16 reg_zdec_sw_htg_en:1;
            U16 reg_zdec_vld_bypass_en:1;
            U16 reg_zdec_sw_block_type:2;
            U16 reg_zdec_arb_wait_rw_lat:1;
            U16 reg_zdec_w_priority:1;
            U16 reg_zdec_r_priority:1;
            U16 reg_zdec_zmem_priority:1;
            U16 reg_zdec_sram_sd_en:1;
            U16 reg_zdec_00_dummy:6;
        };
        U16 reg00;
    };

    union {
        struct {
            U16 reg_zdec_vld_st_bit_cnt:3;
            U16 reg_zdec_fcie_hw_talk:1;
            U16 reg_zdec_fcie_test_mode:1;
            U16 reg_zdec_fcie_test_adma_done:1;
            U16 reg_zdec_01_dummy:10;
        };
        U16 reg02;
    };

    union {
        struct {
            U16 reg_zdec_fcie_test_job_cnt:16;
        };
        U16 reg06;
    };

    union {
        struct {
            U16 reg_zdec_fcie_test_seg_cnt:16;
        };
        U16 reg07;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_cmd_sadr_lo:16; // reg_zdec_ibuf_cmd_sadr[15:0]
        };
        U16 reg08;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_cmd_sadr_hi:16; // reg_zdec_ibuf_cmd_sadr[31:16]
        };
        U16 reg09;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_cmd_size_lo:16; // reg_zdec_ibuf_cmd_size[15:0]
        };
        U16 reg0a;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_cmd_size_hi:16; // reg_zdec_ibuf_cmd_size[31:16]
        };
        U16 reg0b;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_cmd_last:1;
            U16 reg_zdec_ibuf_cmd_miu_sel:2;
            U16 reg_zdec_0c_dummy:13;
        };
        U16 reg0c;
    };

    union {
        struct {
            U16 reg_zdec_early_done_thd:16;
        };
        U16 reg0f;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_shift_bit:3;
            U16 reg_zdec_10_dummy0:1;
            U16 reg_zdec_ibuf_shift_byte:4;
            U16 reg_zdec_10_dummy1:8;
        };
        U16 reg10;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_skip_words_cnt_lo:16; // reg_zdec_ibuf_skip_words_cnt[15:0]
        };
        U16 reg11;
    };

    union {
        struct {
            U16 reg_zdec_ibuf_skip_words_cnt_hi:16; // reg_zdec_ibuf_skip_words_cnt[31:16]
        };
        U16 reg12;
    };

    union {
        struct {
            U16 reg_zdec_13_dummy0:3;
            U16 reg_zdec_ibuf_burst_len:3;
            U16 reg_zdec_ibuf_reverse_bit:1;
            U16 reg_zdec_ibuf_reverse_bit_in_byte:1;
            U16 reg_zdec_ibuf_byte_swap_64:1;
            U16 reg_zdec_ibuf_byte_swap_32:1;
            U16 reg_zdec_ibuf_byte_swap_16:1;
            U16 reg_zdec_ibuf_byte_swap_8:1;
            U16 reg_zdec_ibuf_64to32_lsb_first:1;
            U16 reg_zdec_13_dummy1:3;
        };
        U16 reg13;
    };

    union {
        struct {
            U16 reg_zdec_obuf_write_upperbound_en:1;
            U16 reg_zdec_obuf_write_lowerbound_en:1;
            U16 reg_zdec_obuf_timeout_en:1;
            U16 reg_zdec_obuf_datacnt_en:1;
            U16 reg_zdec_obuf_burst_split:1;
            U16 reg_zdec_obuf_byte_swap_64:1;
            U16 reg_zdec_obuf_byte_swap_32:1;
            U16 reg_zdec_obuf_byte_swap_16:1;
            U16 reg_zdec_obuf_byte_swap_8:1;
            U16 reg_zdec_obuf_miu_sel:2;
            U16 reg_zdec_last_done_z_mode:1;
            U16 reg_zdec_last_done_z_force:1;
            U16 reg_zdec_obuf_burst_length:2;
            U16 reg_zdec_15_dummy:1;
        };
        U16 reg15;
    };

    union {
        struct {
            U16 reg_zdec_obuf_write_upperbound_lo:16; // reg_zdec_obuf_write_upperbound[15:0]
        };
        U16 reg16;
    };

    union {
        struct {
            U16 reg_zdec_obuf_write_upperbound_hi:16; // reg_zdec_obuf_write_upperbound[31:16]
        };
        U16 reg17;
    };

    union {
        struct {
            U16 reg_zdec_obuf_write_lowerbound_lo:16; // reg_zdec_obuf_write_lowerbound[15:0]
        };
        U16 reg18;
    };

    union {
        struct {
            U16 reg_zdec_obuf_write_lowerbound_hi:16; // reg_zdec_obuf_write_lowerbound[31:16]
        };
        U16 reg19;
    };

    union {
        struct {
            U16 reg_zdec_obuf_sadr_lo:16; // reg_zdec_obuf_sadr[15:0]
        };
        U16 reg1a;
    };

    union {
        struct {
            U16 reg_zdec_obuf_sadr_hi:16; // reg_zdec_obuf_sadr[31:16]
        };
        U16 reg1b;
    };

    union {
        struct {
            U16 reg_zdec_obuf_len_lo:16; // reg_zdec_obuf_len[15:0]
        };
        U16 reg1c;
    };

    union {
        struct {
            U16 reg_zdec_obuf_len_hi:16; // reg_zdec_obuf_len[31:16]
        };
        U16 reg1d;
    };

    union {
        struct {
            U16 reg_zdec_obuf_timeout:5;
            U16 reg_zdec_obuf_cnt_no_wait_last_done_z:1;
            U16 reg_zdec_1e_dummy:10;
        };
        U16 reg1e;
    };

    union {
        struct {
            U16 reg_zdec_vht_sw_ctrl_en:1;
            U16 reg_zdec_vht_sw_ctrl_htg_or_tbl_sel:1;
            U16 reg_zdec_vht_sw_ctrl_rw:1;
            U16 reg_zdec_vht_sw_ctrl_mode_sel:2;
            U16 reg_zdec_vht_sw_ctrl_min_code_sel:2;
            U16 reg_zdec_20_dummy0:1; // [read only] reg_zdec_vht_sw_ctrl_htg_done
            U16 reg_zdec_dec_cnt_sel:1;
            U16 reg_zdec_20_dummy1:7;
        };
        U16 reg20;
    };

    union {
        struct {
            U16 reg_zdec_vht_sw_ctrl_adr:9;
            U16 reg_zdec_21_dummy:7;
        };
        U16 reg21;
    };

    union {
        struct {
            U16 reg_zdec_vht_sw_ctrl_wd:16;
        };
        U16 reg22;
    };

    union {
        struct {
            U16 reg_zdec_block_header_error_stop:1;
            U16 reg_zdec_mini_code_error_stop:1;
            U16 reg_zdec_uncompressed_len_error_stop:1;
            U16 reg_zdec_distance_error_stop:1;
            U16 reg_zdec_literal_error_stop:1;
            U16 reg_zdec_table_error_stop:1;
            U16 reg_zdec_28_dummy:10;
        };
        U16 reg28;
    };

    union {
        struct {
            U16 reg_zdec_exception_mask:16;
        };
        U16 reg2a;
    };

    union {
        struct {
            U16 reg_zdec_lzd_bypass_en:1;
            U16 reg_zdec_frame_done_sel:1;
            U16 reg_zdec_zsram_size:2;
            U16 reg_zdec_zmem_miu_sel:2;
            U16 reg_zdec_zmem_last_done_z_mode:1;
            U16 reg_zdec_zmem_last_done_z_force:1;
            U16 reg_zdec_zsram_size_en:1;
            U16 reg_zdec_frame_done_mux:2;
            U16 reg_zdec_zmem_off:1;
            U16 reg_zdec_zmem_burst_len:3;
            U16 reg_zdec_30_dummy:1;
        };
        U16 reg30;
    };

    union {
        struct {
            U16 reg_zdec_tot_bytes_lo:16; // reg_zdec_tot_bytes[15:0]
        };
        U16 reg31;
    };

    union {
        struct {
            U16 reg_zdec_tot_bytes_hi:16; // reg_zdec_tot_bytes[31:16]
        };
        U16 reg32;
    };

    union {
        struct {
            U16 reg_zdec_zmem_saddr_lo:16; // reg_zdec_zmem_saddr[15:0]
        };
        U16 reg33;
    };

    union {
        struct {
            U16 reg_zdec_zmem_saddr_hi:16; // reg_zdec_zmem_saddr[31:16]
        };
        U16 reg34;
    };

    union {
        struct {
            U16 reg_zdec_zmem_eaddr_lo:16; // reg_zdec_zmem_eaddr[15:0]
        };
        U16 reg35;
    };

    union {
        struct {
            U16 reg_zdec_zmem_eaddr_hi:16; // reg_zdec_zmem_eaddr[31:16]
        };
        U16 reg36;
    };

    union {
        struct {
            U16 reg_zdec_preset_dic_bytes:16;
        };
        U16 reg39;
    };

    union {
        struct {
            U16 reg_zdec_iobuf_mode:3;
            U16 reg_zdec_tbl_format:1;
            U16 reg_zdec_src_tbl_miu_sel:2;
            U16 reg_zdec_dst_tbl_miu_sel:2;
            U16 reg_zdec_src_tbl_last:1;
            U16 reg_zdec_40_dummy:7;
        };
        U16 reg40;
    };

    union {
        struct {
            U16 reg_zdec_src_tbl_addr_lo:16; // reg_zdec_src_tbl_addr[15:0]
        };
        U16 reg41;
    };

    union {
        struct {
            U16 reg_zdec_src_tbl_addr_hi:16; // reg_zdec_src_tbl_addr[31:16]
        };
        U16 reg42;
    };

    union {
        struct {
            U16 reg_zdec_dst_tbl_addr_lo:16; // reg_zdec_dst_tbl_addr[15:0]
        };
        U16 reg43;
    };

    union {
        struct {
            U16 reg_zdec_dst_tbl_addr_hi:16; // reg_zdec_dst_tbl_addr[31:16]
        };
        U16 reg44;
    };

    union {
        struct {
            U16 reg_zdec_page_size:4;
            U16 reg_zdec_out_page_size:4;
            U16 reg_zdec_46_dummy:8;
        };
        U16 reg46;
    };

    union {
        struct {
            U16 reg_zdec_debug_en:1;
            U16 reg_zdec_debug_clk_sel:2;
            U16 reg_zdec_debug_trig_mode:4;
            U16 reg_zdec_50_dummy:9;
        };
        U16 reg50;
    };

    union {
        struct {
            U16 reg_zdec_debug_bus_sel:8;
            U16 reg_zdec_dbg_cnt_sel:4;
            U16 reg_zdec_51_dummy:4;
        };
        U16 reg51;
    };

    union {
        struct {
            U16 reg_zdec_debug_trig_cnt:16;
        };
        U16 reg54;
    };

    union {
        struct {
            U16 reg_zdec_dic_riu_wd:8;
            U16 reg_zdec_dic_riu_rw:1;
            U16 reg_zdec_58_dummy:7;
        };
        U16 reg58;
    };

    union {
        struct {
            U16 reg_zdec_dic_riu_addr:15;
            U16 reg_zdec_59_dummy:1;
        };
        U16 reg59;
    };

    union {
        struct {
            U16 reg_zdec_dic_riu_sel:1;
            U16 reg_zdec_5a_dummy:15;
        };
        U16 reg5a;
    };

    union {
        struct {
            U16 reg_zdec_crc_en:1;
            U16 reg_zdec_crc_mode:2;
            U16 reg_zdec_crc_sel:1;
            U16 reg_zdec_68_dummy:12;
        };
        U16 reg68;
    };

    union {
        struct {
            U16 reg_zdec_irq_force:12;
            U16 reg_zdec_71_dummy:4;
        };
        U16 reg71;
    };

    union {
        struct {
            U16 reg_zdec_irq_mask:12;
            U16 reg_zdec_72_dummy:4;
        };
        U16 reg72;
    };

    union {
        struct {
            U16 reg_zdec_irq_status_sel:1;
            U16 reg_zdec_75_dummy:15;
        };
        U16 reg75;
    };

    union {
        struct {
            U16 reg_zdec_spare00:16;
        };
        U16 reg78;
    };

    union {
        struct {
            U16 reg_zdec_spare01:16;
        };
        U16 reg79;
    };

    union {
        struct {
            U16 reg_zdec_spare02:16;
        };
        U16 reg7a;
    };

    union {
        struct {
            U16 reg_zdec_spare03:16;
        };
        U16 reg7b;
    };

    union {
        struct {
            U16 reg_zdec_spare04:16;
        };
        U16 reg7c;
    };

    union {
        struct {
            U16 reg_zdec_spare05:16;
        };
        U16 reg7d;
    };

    union {
        struct {
            U16 reg_zdec_spare06:16;
        };
        U16 reg7e;
    };

    union {
        struct {
            U16 reg_zdec_spare07:16;
        };
        U16 reg7f;
    };
} ZDEC_REGS;


#endif

