#ifndef __KERNEL_CHIPTOP_H__
#define __KERNEL_CHIPTOP_H__

//#include "cpu_mem_map.hc"

typedef struct CHIPTOP_s{
    // h0000, bit: 0
    /* reserved*/
    #define offset_of_chiptop_reg_reserved0 (0)
    #define mask_of_chiptop_reg_reserved0 (0x0)
    unsigned int reg_reserved0:1;

    // h0000, bit: 14
    /* */
    unsigned int :15;

    // h0000
    unsigned int /* padding 16 bit */:16;

    // h0001, bit: 8
    /* */
    unsigned int :9;

    // h0001, bit: 9
    /* FT mode
    1: enable FT mode
    0: disable FT mode*/
    #define offset_of_chiptop_reg_ft_mode (2)
    #define mask_of_chiptop_reg_ft_mode (0x200)
    unsigned int reg_ft_mode:1;

    // h0001, bit: 10
    /* boot from Parallel Flash
    1: enable boot from Parallel Flash
    0: disable boot from Parallel Flash*/
    #define offset_of_chiptop_reg_boot_from_pf (2)
    #define mask_of_chiptop_reg_boot_from_pf (0x400)
    unsigned int reg_boot_from_pf:1;

    // h0001, bit: 11
    /* boot from SDRAM
    1: enable boot from SDRAM
    0: disable boot from SDRAM*/
    #define offset_of_chiptop_reg_boot_from_sdram (2)
    #define mask_of_chiptop_reg_boot_from_sdram (0x800)
    unsigned int reg_boot_from_sdram:1;

    // h0001, bit: 12
    /* digital pads set high*/
    #define offset_of_chiptop_reg_seth (2)
    #define mask_of_chiptop_reg_seth (0x1000)
    unsigned int reg_seth:1;

    // h0001, bit: 13
    /* digital pads set low*/
    #define offset_of_chiptop_reg_setl (2)
    #define mask_of_chiptop_reg_setl (0x2000)
    unsigned int reg_setl:1;

    // h0001, bit: 14
    /* */
    unsigned int :2;

    // h0001
    unsigned int /* padding 16 bit */:16;

    // h0002, bit: 1
    /* TS0 Mode*/
    #define offset_of_chiptop_reg_ts0_mode (4)
    #define mask_of_chiptop_reg_ts0_mode (0x3)
    unsigned int reg_ts0_mode:2;

    // h0002, bit: 3
    /* TS1 Mode*/
    #define offset_of_chiptop_reg_ts1_mode (4)
    #define mask_of_chiptop_reg_ts1_mode (0xc)
    unsigned int reg_ts1_mode:2;

    // h0002, bit: 6
    /* TS2 Mode*/
    #define offset_of_chiptop_reg_ts2_mode (4)
    #define mask_of_chiptop_reg_ts2_mode (0x70)
    unsigned int reg_ts2_mode:3;

    // h0002, bit: 14
    /* */
    unsigned int :9;

    // h0002
    unsigned int /* padding 16 bit */:16;

    // h0003, bit: 11
    /* */
    unsigned int :12;

    // h0003, bit: 12
    /* force all of the whole chip sram power-on*/
    #define offset_of_chiptop_reg_force_allsram_on (6)
    #define mask_of_chiptop_reg_force_allsram_on (0x1000)
    unsigned int reg_force_allsram_on:1;

    // h0003, bit: 13
    /* force all of the whole chip sram power-down*/
    #define offset_of_chiptop_reg_force_allsram_pd (6)
    #define mask_of_chiptop_reg_force_allsram_pd (0x2000)
    unsigned int reg_force_allsram_pd:1;

    // h0003, bit: 14
    /* BIST mode start*/
    #define offset_of_chiptop_reg_bist_start_ext (6)
    #define mask_of_chiptop_reg_bist_start_ext (0x4000)
    unsigned int reg_bist_start_ext:1;

    // h0003, bit: 15
    /* BIST mode enable*/
    #define offset_of_chiptop_reg_bist_mode_ext (6)
    #define mask_of_chiptop_reg_bist_mode_ext (0x8000)
    unsigned int reg_bist_mode_ext:1;

    // h0003
    unsigned int /* padding 16 bit */:16;

    // h0004, bit: 1
    /* MCU JTAG Mode*/
    #define offset_of_chiptop_reg_mcu_jtag_mode (8)
    #define mask_of_chiptop_reg_mcu_jtag_mode (0x3)
    unsigned int reg_mcu_jtag_mode:2;

    // h0004, bit: 3
    /* */
    unsigned int :2;

    // h0004, bit: 5
    /* EMMC mode*/
    #define offset_of_chiptop_reg_emmc_mode (8)
    #define mask_of_chiptop_reg_emmc_mode (0x30)
    unsigned int reg_emmc_mode:2;

    // h0004, bit: 7
    /* MSPI 1 mode 1*/
    #define offset_of_chiptop_reg_mspi1_mode1 (8)
    #define mask_of_chiptop_reg_mspi1_mode1 (0xc0)
    unsigned int reg_mspi1_mode1:2;

    // h0004, bit: 9
    /* MSPI 1 mode 2*/
    #define offset_of_chiptop_reg_mspi1_mode2 (8)
    #define mask_of_chiptop_reg_mspi1_mode2 (0x300)
    unsigned int reg_mspi1_mode2:2;

    // h0004, bit: 11
    /* MSPI 1 mode 3*/
    #define offset_of_chiptop_reg_mspi1_mode3 (8)
    #define mask_of_chiptop_reg_mspi1_mode3 (0xc00)
    unsigned int reg_mspi1_mode3:2;

    // h0004, bit: 13
    /* MSPI 2 mode 1*/
    #define offset_of_chiptop_reg_mspi2_mode1 (8)
    #define mask_of_chiptop_reg_mspi2_mode1 (0x3000)
    unsigned int reg_mspi2_mode1:2;

    // h0004, bit: 14
    /* MSPI 2 mode 2*/
    #define offset_of_chiptop_reg_mspi2_mode2 (8)
    #define mask_of_chiptop_reg_mspi2_mode2 (0xc000)
    unsigned int reg_mspi2_mode2:2;

    // h0004
    unsigned int /* padding 16 bit */:16;

    // h0005, bit: 1
    /* SPDIF IN Mode*/
    #define offset_of_chiptop_reg_spdif_in_mode (10)
    #define mask_of_chiptop_reg_spdif_in_mode (0x3)
    unsigned int reg_spdif_in_mode:2;

    // h0005, bit: 3
    /* */
    unsigned int :2;

    // h0005, bit: 5
    /* SPDIF OUT Mode*/
    #define offset_of_chiptop_reg_spdif_out_mode (10)
    #define mask_of_chiptop_reg_spdif_out_mode (0x30)
    unsigned int reg_spdif_out_mode:2;

    // h0005, bit: 7
    /* */
    unsigned int :2;

    // h0005, bit: 8
    /* I2S IN Mode*/
    #define offset_of_chiptop_reg_i2s_in_mode (10)
    #define mask_of_chiptop_reg_i2s_in_mode (0x100)
    unsigned int reg_i2s_in_mode:1;

    // h0005, bit: 9
    /* */
    unsigned int :1;

    // h0005, bit: 10
    /* I2S TRX Mode*/
    #define offset_of_chiptop_reg_i2s_trx_mode (10)
    #define mask_of_chiptop_reg_i2s_trx_mode (0x400)
    unsigned int reg_i2s_trx_mode:1;

    // h0005, bit: 11
    /* */
    unsigned int :1;

    // h0005, bit: 12
    /* I2S OUT Mode*/
    #define offset_of_chiptop_reg_i2s_out_mode (10)
    #define mask_of_chiptop_reg_i2s_out_mode (0x1000)
    unsigned int reg_i2s_out_mode:1;

    // h0005, bit: 14
    /* I2S OUT Mode 2*/
    #define offset_of_chiptop_reg_i2s_out_mode2 (10)
    #define mask_of_chiptop_reg_i2s_out_mode2 (0x6000)
    unsigned int reg_i2s_out_mode2:2;

    // h0005, bit: 15
    /* I2S OUT Mute Mode*/
    #define offset_of_chiptop_reg_i2s_out_mute_mode (10)
    #define mask_of_chiptop_reg_i2s_out_mute_mode (0x8000)
    unsigned int reg_i2s_out_mute_mode:1;

    // h0005
    unsigned int /* padding 16 bit */:16;

    // h0006, bit: 14
    /* */
    unsigned int :16;

    // h0006
    unsigned int /* padding 16 bit */:16;

    // h0007, bit: 1
    /* PWM0 Mode*/
    #define offset_of_chiptop_reg_pwm0_mode (14)
    #define mask_of_chiptop_reg_pwm0_mode (0x3)
    unsigned int reg_pwm0_mode:2;

    // h0007, bit: 3
    /* */
    unsigned int :2;

    // h0007, bit: 5
    /* PWM1 Mode*/
    #define offset_of_chiptop_reg_pwm1_mode (14)
    #define mask_of_chiptop_reg_pwm1_mode (0x30)
    unsigned int reg_pwm1_mode:2;

    // h0007, bit: 6
    /* CI Mode*/
    #define offset_of_chiptop_reg_ci_mode (14)
    #define mask_of_chiptop_reg_ci_mode (0x40)
    unsigned int reg_ci_mode:1;

    // h0007, bit: 7
    /* */
    unsigned int :1;

    // h0007, bit: 8
    /* SD Mode lock*/
    #define offset_of_chiptop_reg_sd_config_lock (14)
    #define mask_of_chiptop_reg_sd_config_lock (0x100)
    unsigned int reg_sd_config_lock:1;

    // h0007, bit: 9
    /* EMMC Mode lock*/
    #define offset_of_chiptop_reg_emmc_config_lock (14)
    #define mask_of_chiptop_reg_emmc_config_lock (0x200)
    unsigned int reg_emmc_config_lock:1;

    // h0007, bit: 10
    /* NAND Mode lock*/
    #define offset_of_chiptop_reg_nand_mode_lock (14)
    #define mask_of_chiptop_reg_nand_mode_lock (0x400)
    unsigned int reg_nand_mode_lock:1;

    // h0007, bit: 11
    /* SDIO Mode lock*/
    #define offset_of_chiptop_reg_sdio_mode_lock (14)
    #define mask_of_chiptop_reg_sdio_mode_lock (0x800)
    unsigned int reg_sdio_mode_lock:1;

    // h0007, bit: 13
    /* */
    unsigned int :2;

    // h0007, bit: 14
    /* TS out  Mode*/
    #define offset_of_chiptop_reg_ts_out_mode (14)
    #define mask_of_chiptop_reg_ts_out_mode (0xc000)
    unsigned int reg_ts_out_mode:2;

    // h0007
    unsigned int /* padding 16 bit */:16;

    // h0008, bit: 2
    /* EMMC Mode*/
    #define offset_of_chiptop_reg_emmc_config (16)
    #define mask_of_chiptop_reg_emmc_config (0x7)
    unsigned int reg_emmc_config:3;

    // h0008, bit: 5
    /* */
    unsigned int :3;

    // h0008, bit: 6
    /* NAND CS1 enable*/
    #define offset_of_chiptop_reg_nand_cs1_en (16)
    #define mask_of_chiptop_reg_nand_cs1_en (0x40)
    unsigned int reg_nand_cs1_en:1;

    // h0008, bit: 14
    /* */
    unsigned int :9;

    // h0008
    unsigned int /* padding 16 bit */:16;

    // h0009, bit: 7
    /* */
    unsigned int :8;

    // h0009, bit: 9
    /* MSPI Mode*/
    #define offset_of_chiptop_reg_mspi_mode (18)
    #define mask_of_chiptop_reg_mspi_mode (0x300)
    unsigned int reg_mspi_mode:2;

    // h0009, bit: 11
    /* FUART Mode*/
    #define offset_of_chiptop_reg_fuart_mode (18)
    #define mask_of_chiptop_reg_fuart_mode (0xc00)
    unsigned int reg_fuart_mode:2;

    // h0009, bit: 13
    /* I2C Master4 Mode*/
    #define offset_of_chiptop_reg_i2cm4_mode (18)
    #define mask_of_chiptop_reg_i2cm4_mode (0x3000)
    unsigned int reg_i2cm4_mode:2;

    // h0009, bit: 14
    /* */
    unsigned int :2;

    // h0009
    unsigned int /* padding 16 bit */:16;

    // h000a, bit: 0
    /* 0 : use miu_master_wcache
    1 : bypass miu_master_wcache*/
    #define offset_of_chiptop_reg_miu_wc_bypass (20)
    #define mask_of_chiptop_reg_miu_wc_bypass (0x1)
    unsigned int reg_miu_wc_bypass:1;

    // h000a, bit: 1
    /* */
    #define offset_of_chiptop_reg_miu_wc_sw_flush (20)
    #define mask_of_chiptop_reg_miu_wc_sw_flush (0x2)
    unsigned int reg_miu_wc_sw_flush:1;

    // h000a, bit: 14
    /* */
    unsigned int :14;

    // h000a
    unsigned int /* padding 16 bit */:16;

    // h000b, bit: 0
    /* ej_mode*/
    #define offset_of_chiptop_reg_ej_mode (22)
    #define mask_of_chiptop_reg_ej_mode (0x1)
    unsigned int reg_ej_mode:1;

    // h000b, bit: 3
    /* */
    unsigned int :3;

    // h000b, bit: 4
    /* hdmirx_arc_mode*/
    #define offset_of_chiptop_reg_hdmirx_arc_mode (22)
    #define mask_of_chiptop_reg_hdmirx_arc_mode (0x10)
    unsigned int reg_hdmirx_arc_mode:1;

    // h000b, bit: 7
    /* */
    unsigned int :3;

    // h000b, bit: 8
    /* hdmitx_arc_mode*/
    #define offset_of_chiptop_reg_hdmitx_arc_mode (22)
    #define mask_of_chiptop_reg_hdmitx_arc_mode (0x100)
    unsigned int reg_hdmitx_arc_mode:1;

    // h000b, bit: 11
    /* */
    unsigned int :3;

    // h000b, bit: 12
    /* hdmitx_ddc_mode*/
    #define offset_of_chiptop_reg_hdmitx_ddc_mode (22)
    #define mask_of_chiptop_reg_hdmitx_ddc_mode (0x1000)
    unsigned int reg_hdmitx_ddc_mode:1;

    // h000b, bit: 14
    /* */
    unsigned int :3;

    // h000b
    unsigned int /* padding 16 bit */:16;

    // h000c, bit: 0
    /* */
    #define offset_of_chiptop_reg_sm0_open (24)
    #define mask_of_chiptop_reg_sm0_open (0x1)
    unsigned int reg_sm0_open:1;

    // h000c, bit: 1
    /* */
    #define offset_of_chiptop_reg_sm1_open (24)
    #define mask_of_chiptop_reg_sm1_open (0x2)
    unsigned int reg_sm1_open:1;

    // h000c, bit: 3
    /* */
    unsigned int :2;

    // h000c, bit: 5
    /* */
    #define offset_of_chiptop_reg_sm0_c48 (24)
    #define mask_of_chiptop_reg_sm0_c48 (0x30)
    unsigned int reg_sm0_c48:2;

    // h000c, bit: 7
    /* */
    #define offset_of_chiptop_reg_sm1_c48 (24)
    #define mask_of_chiptop_reg_sm1_c48 (0xc0)
    unsigned int reg_sm1_c48:2;

    // h000c, bit: 8
    /* */
    #define offset_of_chiptop_reg_sm0_en (24)
    #define mask_of_chiptop_reg_sm0_en (0x100)
    unsigned int reg_sm0_en:1;

    // h000c, bit: 9
    /* */
    #define offset_of_chiptop_reg_sm1_en (24)
    #define mask_of_chiptop_reg_sm1_en (0x200)
    unsigned int reg_sm1_en:1;

    // h000c, bit: 11
    /* */
    unsigned int :2;

    // h000c, bit: 12
    /* */
    #define offset_of_chiptop_reg_sm0_io (24)
    #define mask_of_chiptop_reg_sm0_io (0x1000)
    unsigned int reg_sm0_io:1;

    // h000c, bit: 13
    /* */
    #define offset_of_chiptop_reg_sm1_io (24)
    #define mask_of_chiptop_reg_sm1_io (0x2000)
    unsigned int reg_sm1_io:1;

    // h000c, bit: 14
    /* */
    unsigned int :2;

    // h000c
    unsigned int /* padding 16 bit */:16;

    // h000d, bit: 0
    /* */
    #define offset_of_chiptop_reg_sm0x_open (26)
    #define mask_of_chiptop_reg_sm0x_open (0x1)
    unsigned int reg_sm0x_open:1;

    // h000d, bit: 1
    /* */
    #define offset_of_chiptop_reg_sm1x_open (26)
    #define mask_of_chiptop_reg_sm1x_open (0x2)
    unsigned int reg_sm1x_open:1;

    // h000d, bit: 3
    /* */
    unsigned int :2;

    // h000d, bit: 5
    /* */
    #define offset_of_chiptop_reg_sm0x_c48 (26)
    #define mask_of_chiptop_reg_sm0x_c48 (0x30)
    unsigned int reg_sm0x_c48:2;

    // h000d, bit: 7
    /* */
    unsigned int :2;

    // h000d, bit: 8
    /* */
    #define offset_of_chiptop_reg_sm0x_en (26)
    #define mask_of_chiptop_reg_sm0x_en (0x100)
    unsigned int reg_sm0x_en:1;

    // h000d, bit: 9
    /* */
    #define offset_of_chiptop_reg_sm1x_en (26)
    #define mask_of_chiptop_reg_sm1x_en (0x200)
    unsigned int reg_sm1x_en:1;

    // h000d, bit: 11
    /* */
    unsigned int :2;

    // h000d, bit: 12
    /* */
    #define offset_of_chiptop_reg_sm0x_io (26)
    #define mask_of_chiptop_reg_sm0x_io (0x1000)
    unsigned int reg_sm0x_io:1;

    // h000d, bit: 13
    /* */
    #define offset_of_chiptop_reg_sm1x_io (26)
    #define mask_of_chiptop_reg_sm1x_io (0x2000)
    unsigned int reg_sm1x_io:1;

    // h000d, bit: 14
    /* */
    unsigned int :2;

    // h000d
    unsigned int /* padding 16 bit */:16;

    // h000e, bit: 0
    /* */
    #define offset_of_chiptop_reg_ocp0_en (28)
    #define mask_of_chiptop_reg_ocp0_en (0x1)
    unsigned int reg_ocp0_en:1;

    // h000e, bit: 1
    /* */
    #define offset_of_chiptop_reg_ocp1_en (28)
    #define mask_of_chiptop_reg_ocp1_en (0x2)
    unsigned int reg_ocp1_en:1;

    // h000e, bit: 3
    /* */
    unsigned int :2;

    // h000e, bit: 4
    /* */
    #define offset_of_chiptop_reg_ocp0x_en (28)
    #define mask_of_chiptop_reg_ocp0x_en (0x10)
    unsigned int reg_ocp0x_en:1;

    // h000e, bit: 14
    /* */
    unsigned int :11;

    // h000e
    unsigned int /* padding 16 bit */:16;

    // h000f, bit: 0
    /* */
    #define offset_of_chiptop_reg_pf_cs1_en (30)
    #define mask_of_chiptop_reg_pf_cs1_en (0x1)
    unsigned int reg_pf_cs1_en:1;

    // h000f, bit: 14
    /* */
    unsigned int :15;

    // h000f
    unsigned int /* padding 16 bit */:16;

    // h0010, bit: 3
    /* 32bit MCU access MIU size
    0000:  32 Mbyte
    0010:  64 Mbyte
    0100:  128 Mbyte
    1000:  256 Mbyte*/
    #define offset_of_chiptop_reg_mcu3_dram_size (32)
    #define mask_of_chiptop_reg_mcu3_dram_size (0xf)
    unsigned int reg_mcu3_dram_size:4;

    // h0010, bit: 14
    /* */
    unsigned int :12;

    // h0010
    unsigned int /* padding 16 bit */:16;

    // h0011, bit: 0
    /* reg_ts0_3_wire_en*/
    #define offset_of_chiptop_reg_ts0_3_wire_en (34)
    #define mask_of_chiptop_reg_ts0_3_wire_en (0x1)
    unsigned int reg_ts0_3_wire_en:1;

    // h0011, bit: 1
    /* reg_ts1_3_wire_en*/
    #define offset_of_chiptop_reg_ts1_3_wire_en (34)
    #define mask_of_chiptop_reg_ts1_3_wire_en (0x2)
    unsigned int reg_ts1_3_wire_en:1;

    // h0011, bit: 2
    /* reg_ts0_clk_drv*/
    #define offset_of_chiptop_reg_ts0_clk_drv (34)
    #define mask_of_chiptop_reg_ts0_clk_drv (0x4)
    unsigned int reg_ts0_clk_drv:1;

    // h0011, bit: 3
    /* reg_ts1_clk_drv*/
    #define offset_of_chiptop_reg_ts1_clk_drv (34)
    #define mask_of_chiptop_reg_ts1_clk_drv (0x8)
    unsigned int reg_ts1_clk_drv:1;

    // h0011, bit: 4
    /* reg_ts0_d0_drv*/
    #define offset_of_chiptop_reg_ts0_d0_drv (34)
    #define mask_of_chiptop_reg_ts0_d0_drv (0x10)
    unsigned int reg_ts0_d0_drv:1;

    // h0011, bit: 5
    /* reg_ts1_d0_drv*/
    #define offset_of_chiptop_reg_ts1_d0_drv (34)
    #define mask_of_chiptop_reg_ts1_d0_drv (0x20)
    unsigned int reg_ts1_d0_drv:1;

    // h0011, bit: 6
    /* reg_ts0_sync_drv*/
    #define offset_of_chiptop_reg_ts0_sync_drv (34)
    #define mask_of_chiptop_reg_ts0_sync_drv (0x40)
    unsigned int reg_ts0_sync_drv:1;

    // h0011, bit: 7
    /* reg_ts1_sync_drv*/
    #define offset_of_chiptop_reg_ts1_sync_drv (34)
    #define mask_of_chiptop_reg_ts1_sync_drv (0x80)
    unsigned int reg_ts1_sync_drv:1;

    // h0011, bit: 8
    /* reg_ts0_vld_drv*/
    #define offset_of_chiptop_reg_ts0_vld_drv (34)
    #define mask_of_chiptop_reg_ts0_vld_drv (0x100)
    unsigned int reg_ts0_vld_drv:1;

    // h0011, bit: 9
    /* reg_ts1_vld_drv*/
    #define offset_of_chiptop_reg_ts1_vld_drv (34)
    #define mask_of_chiptop_reg_ts1_vld_drv (0x200)
    unsigned int reg_ts1_vld_drv:1;

    // h0011, bit: 10
    /* reg_ts2_3_wire_en*/
    #define offset_of_chiptop_reg_ts2_3_wire_en (34)
    #define mask_of_chiptop_reg_ts2_3_wire_en (0x400)
    unsigned int reg_ts2_3_wire_en:1;

    // h0011, bit: 11
    /* reg_ts3_3_wire_en*/
    #define offset_of_chiptop_reg_ts3_3_wire_en (34)
    #define mask_of_chiptop_reg_ts3_3_wire_en (0x800)
    unsigned int reg_ts3_3_wire_en:1;

    // h0011, bit: 12
    /* reg_ts4_3_wire_en*/
    #define offset_of_chiptop_reg_ts4_3_wire_en (34)
    #define mask_of_chiptop_reg_ts4_3_wire_en (0x1000)
    unsigned int reg_ts4_3_wire_en:1;

    // h0011, bit: 13
    /* reg_ts5_3_wire_en*/
    #define offset_of_chiptop_reg_ts5_3_wire_en (34)
    #define mask_of_chiptop_reg_ts5_3_wire_en (0x2000)
    unsigned int reg_ts5_3_wire_en:1;

    // h0011, bit: 14
    /* reg_ts6_3_wire_en*/
    #define offset_of_chiptop_reg_ts6_3_wire_en (34)
    #define mask_of_chiptop_reg_ts6_3_wire_en (0x4000)
    unsigned int reg_ts6_3_wire_en:1;

    // h0011, bit: 15
    /* reg_ts7_3_wire_en*/
    #define offset_of_chiptop_reg_ts7_3_wire_en (34)
    #define mask_of_chiptop_reg_ts7_3_wire_en (0x8000)
    unsigned int reg_ts7_3_wire_en:1;

    // h0011
    unsigned int /* padding 16 bit */:16;

    // h0012, bit: 1
    /* select TEST IN mode.
    01: TEST_IN[9:0] use TS0/TS1 pads
    10: TEST_IN[9:0] use CI pads
    11: TEST_IN[9:0] use ET pads
    00: TEST_IN functions are not enabled.*/
    #define offset_of_chiptop_reg_test_in_mode (36)
    #define mask_of_chiptop_reg_test_in_mode (0x3)
    unsigned int reg_test_in_mode:2;

    // h0012, bit: 3
    /* */
    unsigned int :2;

    // h0012, bit: 5
    /* select TEST IN mode.
    01: TEST_IN[9:0] use TS0/TS1 pads
    10: TEST_IN[9:0] use CI pads
    11: TEST_IN[9:0] use ET pads
    00: TEST_IN functions are not enabled.*/
    #define offset_of_chiptop_reg_test_out_mode (36)
    #define mask_of_chiptop_reg_test_out_mode (0x30)
    unsigned int reg_test_out_mode:2;

    // h0012, bit: 14
    /* */
    unsigned int :10;

    // h0012
    unsigned int /* padding 16 bit */:16;

    // h0013, bit: 1
    /* usb30vctl_mode*/
    #define offset_of_chiptop_reg_usb30vctl_mode (38)
    #define mask_of_chiptop_reg_usb30vctl_mode (0x3)
    unsigned int reg_usb30vctl_mode:2;

    // h0013, bit: 7
    /* */
    unsigned int :6;

    // h0013, bit: 9
    /* usb30vctl_mode1*/
    #define offset_of_chiptop_reg_usb30vctl_mode1 (38)
    #define mask_of_chiptop_reg_usb30vctl_mode1 (0x300)
    unsigned int reg_usb30vctl_mode1:2;

    // h0013, bit: 14
    /* */
    unsigned int :6;

    // h0013
    unsigned int /* padding 16 bit */:16;

    // h0014, bit: 0
    /* OCP1 Enable*/
    #define offset_of_chiptop_reg_sel_5v (40)
    #define mask_of_chiptop_reg_sel_5v (0x1)
    unsigned int reg_sel_5v:1;

    // h0014, bit: 3
    /* */
    unsigned int :3;

    // h0014, bit: 4
    /* OCP2 Enable*/
    #define offset_of_chiptop_reg_sel_5v1 (40)
    #define mask_of_chiptop_reg_sel_5v1 (0x10)
    unsigned int reg_sel_5v1:1;

    // h0014, bit: 14
    /* */
    unsigned int :11;

    // h0014
    unsigned int /* padding 16 bit */:16;

    // h0015, bit: 0
    /* */
    #define offset_of_chiptop_reg_ocp0_oc (42)
    #define mask_of_chiptop_reg_ocp0_oc (0x1)
    unsigned int reg_ocp0_oc:1;

    // h0015, bit: 3
    /* */
    unsigned int :3;

    // h0015, bit: 4
    /* */
    #define offset_of_chiptop_reg_ocp1_oc (42)
    #define mask_of_chiptop_reg_ocp1_oc (0x10)
    unsigned int reg_ocp1_oc:1;

    // h0015, bit: 14
    /* */
    unsigned int :11;

    // h0015
    unsigned int /* padding 16 bit */:16;

    // h0016, bit: 4
    /* USB REXT trim value*/
    #define offset_of_chiptop_reg_trim_usb_rext (44)
    #define mask_of_chiptop_reg_trim_usb_rext (0x1f)
    unsigned int reg_trim_usb_rext:5;

    // h0016, bit: 7
    /* */
    unsigned int :3;

    // h0016, bit: 8
    /* USB REXT trim value overwrite*/
    #define offset_of_chiptop_reg_trim_usb_rext_ov (44)
    #define mask_of_chiptop_reg_trim_usb_rext_ov (0x100)
    unsigned int reg_trim_usb_rext_ov:1;

    // h0016, bit: 14
    /* */
    unsigned int :7;

    // h0016
    unsigned int /* padding 16 bit */:16;

    // h0017, bit: 0
    /* */
    #define offset_of_chiptop_reg_shire_test_mode (46)
    #define mask_of_chiptop_reg_shire_test_mode (0x1)
    unsigned int reg_shire_test_mode:1;

    // h0017, bit: 3
    /* */
    unsigned int :3;

    // h0017, bit: 5
    /* */
    #define offset_of_chiptop_reg_abny_mode (46)
    #define mask_of_chiptop_reg_abny_mode (0x30)
    unsigned int reg_abny_mode:2;

    // h0017, bit: 14
    /* */
    unsigned int :10;

    // h0017
    unsigned int /* padding 16 bit */:16;

    // h0018, bit: 4
    /* reg_sm0_PD_54K*/
    #define offset_of_chiptop_reg_sm0_pd_54k (48)
    #define mask_of_chiptop_reg_sm0_pd_54k (0x1f)
    unsigned int reg_sm0_pd_54k:5;

    // h0018, bit: 9
    /* reg_sm0_PD_54K_HIGH*/
    #define offset_of_chiptop_reg_sm0_pd_54k_high (48)
    #define mask_of_chiptop_reg_sm0_pd_54k_high (0x3e0)
    unsigned int reg_sm0_pd_54k_high:5;

    // h0018, bit: 14
    /* reg_sm0_PD_54K_LOW*/
    #define offset_of_chiptop_reg_sm0_pd_54k_low (48)
    #define mask_of_chiptop_reg_sm0_pd_54k_low (0x7c00)
    unsigned int reg_sm0_pd_54k_low:5;

    // h0018, bit: 15
    /* */
    unsigned int :1;

    // h0018
    unsigned int /* padding 16 bit */:16;

    // h0019, bit: 4
    /* reg_sm0_PD_54K_EXT*/
    #define offset_of_chiptop_reg_sm0_pd_54k_ext (50)
    #define mask_of_chiptop_reg_sm0_pd_54k_ext (0x1f)
    unsigned int reg_sm0_pd_54k_ext:5;

    // h0019, bit: 14
    /* */
    unsigned int :11;

    // h0019
    unsigned int /* padding 16 bit */:16;

    // h001a, bit: 4
    /* reg_sm0_drv0*/
    #define offset_of_chiptop_reg_sm0_drv0 (52)
    #define mask_of_chiptop_reg_sm0_drv0 (0x1f)
    unsigned int reg_sm0_drv0:5;

    // h001a, bit: 9
    /* reg_sm0_drv1*/
    #define offset_of_chiptop_reg_sm0_drv1 (52)
    #define mask_of_chiptop_reg_sm0_drv1 (0x3e0)
    unsigned int reg_sm0_drv1:5;

    // h001a, bit: 14
    /* reg_sm0_PU_11K*/
    #define offset_of_chiptop_reg_sm0_pu_11k (52)
    #define mask_of_chiptop_reg_sm0_pu_11k (0x7c00)
    unsigned int reg_sm0_pu_11k:5;

    // h001a, bit: 15
    /* */
    unsigned int :1;

    // h001a
    unsigned int /* padding 16 bit */:16;

    // h001b, bit: 4
    /* reg_sm0_PU_11K_HIGH*/
    #define offset_of_chiptop_reg_sm0_pu_11k_high (54)
    #define mask_of_chiptop_reg_sm0_pu_11k_high (0x1f)
    unsigned int reg_sm0_pu_11k_high:5;

    // h001b, bit: 9
    /* reg_sm0_PU_11K_LOW*/
    #define offset_of_chiptop_reg_sm0_pu_11k_low (54)
    #define mask_of_chiptop_reg_sm0_pu_11k_low (0x3e0)
    unsigned int reg_sm0_pu_11k_low:5;

    // h001b, bit: 14
    /* reg_sm0_GPIO_EN*/
    #define offset_of_chiptop_reg_sm0_gpio_en (54)
    #define mask_of_chiptop_reg_sm0_gpio_en (0x7c00)
    unsigned int reg_sm0_gpio_en:5;

    // h001b, bit: 15
    /* */
    unsigned int :1;

    // h001b
    unsigned int /* padding 16 bit */:16;

    // h001c, bit: 0
    /* */
    #define offset_of_chiptop_reg_pd_demod_xtal_1 (56)
    #define mask_of_chiptop_reg_pd_demod_xtal_1 (0x1)
    unsigned int reg_pd_demod_xtal_1:1;

    // h001c, bit: 1
    /* */
    #define offset_of_chiptop_reg_pd_demod_xtal_2 (56)
    #define mask_of_chiptop_reg_pd_demod_xtal_2 (0x2)
    unsigned int reg_pd_demod_xtal_2:1;

    // h001c, bit: 2
    /* */
    #define offset_of_chiptop_reg_pd_demod_xtal_3 (56)
    #define mask_of_chiptop_reg_pd_demod_xtal_3 (0x4)
    unsigned int reg_pd_demod_xtal_3:1;

    // h001c, bit: 7
    /* */
    unsigned int :5;

    // h001c, bit: 8
    /* */
    #define offset_of_chiptop_reg_dmdtop_dmd_sel (56)
    #define mask_of_chiptop_reg_dmdtop_dmd_sel (0x100)
    unsigned int reg_dmdtop_dmd_sel:1;

    // h001c, bit: 9
    /* */
    #define offset_of_chiptop_reg_dmd_ana_regsel (56)
    #define mask_of_chiptop_reg_dmd_ana_regsel (0x200)
    unsigned int reg_dmd_ana_regsel:1;

    // h001c, bit: 11
    /* */
    unsigned int :2;

    // h001c, bit: 12
    /* riu host 5 selection
    0 : jpi
    1: SC*/
    #define offset_of_chiptop_reg_riu_host5_sel (56)
    #define mask_of_chiptop_reg_riu_host5_sel (0x1000)
    unsigned int reg_riu_host5_sel:1;

    // h001c, bit: 14
    /* */
    unsigned int :3;

    // h001c
    unsigned int /* padding 16 bit */:16;

    // h001d, bit: 14
    /* indicate sram done
          bit 0 : dig_top
          bit 1 : pm_top
          bit 2 : codec_top
          bit 3 : hi_codec_top
          bit 4 : sc_top
          bit 5 : tsp_top
          bit 6 : vivaldi9_top
          bit 7 ; mcu_top
          bit 8 : gpu_top*/
    #define offset_of_chiptop_reg_bist_done (58)
    #define mask_of_chiptop_reg_bist_done (0xffff)
    unsigned int reg_bist_done:16;

    // h001d
    unsigned int /* padding 16 bit */:16;

    // h001e, bit: 14
    /* indicate sram fail
          bit 0 : dig_top
          bit 1 : pm_top
          bit 2 : codec_top
          bit 3 : hi_codec_top
          bit 4 : sc_top
          bit 5 : tsp_top
          bit 6 : vivaldi9_top
          bit 7 ; mcu_top
          bit 8 : gpu_top*/
    #define offset_of_chiptop_reg_bist_fail (60)
    #define mask_of_chiptop_reg_bist_fail (0xffff)
    unsigned int reg_bist_fail:16;

    // h001e
    unsigned int /* padding 16 bit */:16;

    // h001f, bit: 1
    /* */
    #define offset_of_chiptop_reg_disp_misc_outsel  (62)
    #define mask_of_chiptop_reg_disp_misc_outsel  (0x3)
    unsigned int reg_disp_misc_outsel :2;

    // h001f, bit: 2
    /* */
    unsigned int :1;

    // h001f, bit: 3
    /* Set 1 to enable digital output bit order reversed.*/
    #define offset_of_chiptop_reg_disp_misc_bit_swap (62)
    #define mask_of_chiptop_reg_disp_misc_bit_swap (0x8)
    unsigned int reg_disp_misc_bit_swap:1;

    // h001f, bit: 6
    /* TTL output RGB swap
    000:  RGB
    001:  RBG
    010:  GRB
    100:  GBR
    101:  BRG
    110:  BGR
    */
    #define offset_of_chiptop_reg_disp_misc_rgb_swap (62)
    #define mask_of_chiptop_reg_disp_misc_rgb_swap (0x70)
    unsigned int reg_disp_misc_rgb_swap:3;

    // h001f, bit: 14
    /* */
    unsigned int :9;

    // h001f
    unsigned int /* padding 16 bit */:16;

    // h0020, bit: 5
    /* */
    unsigned int :6;

    // h0020, bit: 7
    /* dummy registers for CHIPTOP*/
    #define offset_of_chiptop_reg_chiptop_dummy_0_0 (64)
    #define mask_of_chiptop_reg_chiptop_dummy_0_0 (0xc0)
    unsigned int reg_chiptop_dummy_0_0:2;

    // h0020, bit: 13
    /* */
    unsigned int :6;

    // h0020, bit: 14
    /* dummy registers for CHIPTOP
    [0]: register to control uart debounce ECO
    0: default mode
    1: smart card  ECO mode*/
    #define offset_of_chiptop_reg_chiptop_dummy_0_1 (64)
    #define mask_of_chiptop_reg_chiptop_dummy_0_1 (0xc000)
    unsigned int reg_chiptop_dummy_0_1:2;

    // h0020
    unsigned int /* padding 16 bit */:16;

    // h0021, bit: 10
    /* */
    unsigned int :11;

    // h0021, bit: 11
    /* dummy registers for CHIPTOP
    clk_ge selection 0:216MHz 1:192MHz*/
    #define offset_of_chiptop_reg_chiptop_dummy_1_2 (66)
    #define mask_of_chiptop_reg_chiptop_dummy_1_2 (0x800)
    unsigned int reg_chiptop_dummy_1_2:1;

    // h0021, bit: 14
    /* */
    unsigned int :4;

    // h0021
    unsigned int /* padding 16 bit */:16;

    // h0022, bit: 5
    /* */
    unsigned int :6;

    // h0022, bit: 7
    /* dummy registers for CHIPTOP*/
    #define offset_of_chiptop_reg_chiptop_dummy_2_0 (68)
    #define mask_of_chiptop_reg_chiptop_dummy_2_0 (0xc0)
    unsigned int reg_chiptop_dummy_2_0:2;

    // h0022, bit: 8
    /* boot from Parallel Flash selection*/
    #define offset_of_chiptop_reg_boot_from_pf_sel  (68)
    #define mask_of_chiptop_reg_boot_from_pf_sel  (0x100)
    unsigned int reg_boot_from_pf_sel :1;

    // h0022, bit: 9
    /* */
    unsigned int :1;

    // h0022, bit: 14
    /* dummy registers for CHIPTOP
    reg_chiptop_dummy_2_1[0] : reg_156_mode
    reg_chiptop_dummy_2_1[5:1]: reserved*/
    #define offset_of_chiptop_reg_chiptop_dummy_2_1 (68)
    #define mask_of_chiptop_reg_chiptop_dummy_2_1 (0xfc00)
    unsigned int reg_chiptop_dummy_2_1:6;

    // h0022
    unsigned int /* padding 16 bit */:16;

    // h0023, bit: 0
    /* */
    #define offset_of_chiptop_reg_ts1_clk_rpu (70)
    #define mask_of_chiptop_reg_ts1_clk_rpu (0x1)
    unsigned int reg_ts1_clk_rpu:1;

    // h0023, bit: 1
    /* */
    #define offset_of_chiptop_reg_ts1_d3_rpu (70)
    #define mask_of_chiptop_reg_ts1_d3_rpu (0x2)
    unsigned int reg_ts1_d3_rpu:1;

    // h0023, bit: 3
    /* dummy registers for CHIPTOP
    reg_chiptop_dummy_3_0[0] : reg_kgb2_mode for KGB supporting smart card
    reg_chiptop_dummy_3_0[1] : one-way register to disable EJTAG port*/
    #define offset_of_chiptop_reg_chiptop_dummy_3_0 (70)
    #define mask_of_chiptop_reg_chiptop_dummy_3_0 (0xc)
    unsigned int reg_chiptop_dummy_3_0:2;

    // h0023, bit: 4
    /* */
    #define offset_of_chiptop_reg_spdif_out_rpu (70)
    #define mask_of_chiptop_reg_spdif_out_rpu (0x10)
    unsigned int reg_spdif_out_rpu:1;

    // h0023, bit: 5
    /* */
    #define offset_of_chiptop_reg_spdif_out2_rpu (70)
    #define mask_of_chiptop_reg_spdif_out2_rpu (0x20)
    unsigned int reg_spdif_out2_rpu:1;

    // h0023, bit: 7
    /* dummy registers for CHIPTOP
    reg_chiptop_dummy_3_1[0] : select deactive function for RMA samples
    reg_chiptop_dummy_3_1[1] : reserved*/
    #define offset_of_chiptop_reg_chiptop_dummy_3_1 (70)
    #define mask_of_chiptop_reg_chiptop_dummy_3_1 (0xc0)
    unsigned int reg_chiptop_dummy_3_1:2;

    // h0023, bit: 8
    /* */
    #define offset_of_chiptop_reg_i2s_out_sd3_rpu (70)
    #define mask_of_chiptop_reg_i2s_out_sd3_rpu (0x100)
    unsigned int reg_i2s_out_sd3_rpu:1;

    // h0023, bit: 10
    /* dummy registers for CHIPTOP*/
    #define offset_of_chiptop_reg_chiptop_dummy_3_2 (70)
    #define mask_of_chiptop_reg_chiptop_dummy_3_2 (0x600)
    unsigned int reg_chiptop_dummy_3_2:2;

    // h0023, bit: 11
    /* lock of reg_sm0_en*/
    #define offset_of_chiptop_reg_sm0_en_lock (70)
    #define mask_of_chiptop_reg_sm0_en_lock (0x800)
    unsigned int reg_sm0_en_lock:1;

    // h0023, bit: 12
    /* */
    #define offset_of_chiptop_reg_et_txd1_rpu (70)
    #define mask_of_chiptop_reg_et_txd1_rpu (0x1000)
    unsigned int reg_et_txd1_rpu:1;

    // h0023, bit: 13
    /* */
    #define offset_of_chiptop_reg_et_mdio_rpu (70)
    #define mask_of_chiptop_reg_et_mdio_rpu (0x2000)
    unsigned int reg_et_mdio_rpu:1;

    // h0023, bit: 14
    /* dummy registers for CHIPTOP*/
    #define offset_of_chiptop_reg_chiptop_dummy_3_3 (70)
    #define mask_of_chiptop_reg_chiptop_dummy_3_3 (0xc000)
    unsigned int reg_chiptop_dummy_3_3:2;

    // h0023
    unsigned int /* padding 16 bit */:16;

    // h0024, bit: 0
    /* venc_top_wp isolation off
    0: isolation on
    1: isolation off*/
    #define offset_of_chiptop_reg_ceva_top_iso_off (72)
    #define mask_of_chiptop_reg_ceva_top_iso_off (0x1)
    unsigned int reg_ceva_top_iso_off:1;

    // h0024, bit: 1
    /* ceva_top_wp power reset
    0: reset asserted
    1: reset de-asserted*/
    #define offset_of_chiptop_reg_ceva_top_pwr_rst (72)
    #define mask_of_chiptop_reg_ceva_top_pwr_rst (0x2)
    unsigned int reg_ceva_top_pwr_rst:1;

    // h0024, bit: 2
    /* ceva_top_wp power request
    0: power not request
    1: power request*/
    #define offset_of_chiptop_reg_ceva_top_power_req (72)
    #define mask_of_chiptop_reg_ceva_top_power_req (0x4)
    unsigned int reg_ceva_top_power_req:1;

    // h0024, bit: 3
    /* ceva_top_wp power grant
    0: power not granted
    1: power granted*/
    #define offset_of_chiptop_reg_ceva_top_power_gnt (72)
    #define mask_of_chiptop_reg_ceva_top_power_gnt (0x8)
    unsigned int reg_ceva_top_power_gnt:1;

    // h0024, bit: 13
    /* */
    unsigned int :10;

    // h0024, bit: 14
    /* HEMCU isolation cell enable control*/
    #define offset_of_chiptop_reg_ca9_iso_control (72)
    #define mask_of_chiptop_reg_ca9_iso_control (0xc000)
    unsigned int reg_ca9_iso_control:2;

    // h0024
    unsigned int /* padding 16 bit */:16;

    // h0025, bit: 11
    /* */
    unsigned int :12;

    // h0025, bit: 12
    /* reg_sata_led_en*/
    #define offset_of_chiptop_reg_sata_led_en (74)
    #define mask_of_chiptop_reg_sata_led_en (0x1000)
    unsigned int reg_sata_led_en:1;

    // h0025, bit: 14
    /* */
    unsigned int :3;

    // h0025
    unsigned int /* padding 16 bit */:16;

    // h0026, bit: 14
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_drv3 (76)
    #define mask_of_chiptop_reg_nand_drv3 (0xffff)
    unsigned int reg_nand_drv3:16;

    // h0026
    unsigned int /* padding 16 bit */:16;

    // h0027, bit: 3
    /* reg_gpio2_chsel*/
    #define offset_of_chiptop_reg_gpio2_chsel (78)
    #define mask_of_chiptop_reg_gpio2_chsel (0xf)
    unsigned int reg_gpio2_chsel:4;

    // h0027, bit: 4
    /* Control pull-high/low*/
    #define offset_of_chiptop_reg_gpio2_pe (78)
    #define mask_of_chiptop_reg_gpio2_pe (0x10)
    unsigned int reg_gpio2_pe:1;

    // h0027, bit: 5
    /* reg_gpio2_sen*/
    #define offset_of_chiptop_reg_gpio2_sen (78)
    #define mask_of_chiptop_reg_gpio2_sen (0x20)
    unsigned int reg_gpio2_sen:1;

    // h0027, bit: 6
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio4_drv0 (78)
    #define mask_of_chiptop_reg_gpio4_drv0 (0x40)
    unsigned int reg_gpio4_drv0:1;

    // h0027, bit: 7
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio4_drv1 (78)
    #define mask_of_chiptop_reg_gpio4_drv1 (0x80)
    unsigned int reg_gpio4_drv1:1;

    // h0027, bit: 8
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio5_drv0 (78)
    #define mask_of_chiptop_reg_gpio5_drv0 (0x100)
    unsigned int reg_gpio5_drv0:1;

    // h0027, bit: 9
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio5_drv1 (78)
    #define mask_of_chiptop_reg_gpio5_drv1 (0x200)
    unsigned int reg_gpio5_drv1:1;

    // h0027, bit: 10
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio6_drv0 (78)
    #define mask_of_chiptop_reg_gpio6_drv0 (0x400)
    unsigned int reg_gpio6_drv0:1;

    // h0027, bit: 11
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio6_drv1 (78)
    #define mask_of_chiptop_reg_gpio6_drv1 (0x800)
    unsigned int reg_gpio6_drv1:1;

    // h0027, bit: 12
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio7_drv0 (78)
    #define mask_of_chiptop_reg_gpio7_drv0 (0x1000)
    unsigned int reg_gpio7_drv0:1;

    // h0027, bit: 13
    /* Control driving strength*/
    #define offset_of_chiptop_reg_gpio7_drv1 (78)
    #define mask_of_chiptop_reg_gpio7_drv1 (0x2000)
    unsigned int reg_gpio7_drv1:1;

    // h0027, bit: 14
    /* */
    unsigned int :2;

    // h0027
    unsigned int /* padding 16 bit */:16;

    // h0028, bit: 0
    /* Control driving strength*/
    #define offset_of_chiptop_reg_hdmirx_arc_drv (80)
    #define mask_of_chiptop_reg_hdmirx_arc_drv (0x1)
    unsigned int reg_hdmirx_arc_drv:1;

    // h0028, bit: 1
    /* gpio enable*/
    #define offset_of_chiptop_reg_hdmirx_arc_gpio_en (80)
    #define mask_of_chiptop_reg_hdmirx_arc_gpio_en (0x2)
    unsigned int reg_hdmirx_arc_gpio_en:1;

    // h0028, bit: 2
    /* Control driving strength*/
    #define offset_of_chiptop_reg_hdmitx_arc_drv (80)
    #define mask_of_chiptop_reg_hdmitx_arc_drv (0x4)
    unsigned int reg_hdmitx_arc_drv:1;

    // h0028, bit: 3
    /* gpio enable*/
    #define offset_of_chiptop_reg_hdmitx_arc_gpio_en (80)
    #define mask_of_chiptop_reg_hdmitx_arc_gpio_en (0x8)
    unsigned int reg_hdmitx_arc_gpio_en:1;

    // h0028, bit: 6
    /* Control driving strength*/
    #define offset_of_chiptop_reg_i2s_in_drv0 (80)
    #define mask_of_chiptop_reg_i2s_in_drv0 (0x70)
    unsigned int reg_i2s_in_drv0:3;

    // h0028, bit: 9
    /* Control driving strength*/
    #define offset_of_chiptop_reg_i2s_in_drv1 (80)
    #define mask_of_chiptop_reg_i2s_in_drv1 (0x380)
    unsigned int reg_i2s_in_drv1:3;

    // h0028, bit: 13
    /* Control driving strength*/
    #define offset_of_chiptop_reg_i2s_out_drv (80)
    #define mask_of_chiptop_reg_i2s_out_drv (0x3c00)
    unsigned int reg_i2s_out_drv:4;

    // h0028, bit: 14
    /* */
    unsigned int :2;

    // h0028
    unsigned int /* padding 16 bit */:16;

    // h0029, bit: 0
    /* Control driving strength*/
    #define offset_of_chiptop_reg_mspi_cz_drv (82)
    #define mask_of_chiptop_reg_mspi_cz_drv (0x1)
    unsigned int reg_mspi_cz_drv:1;

    // h0029, bit: 3
    /* Control driving strength*/
    #define offset_of_chiptop_reg_mspi_drv0 (82)
    #define mask_of_chiptop_reg_mspi_drv0 (0xe)
    unsigned int reg_mspi_drv0:3;

    // h0029, bit: 6
    /* Control driving strength*/
    #define offset_of_chiptop_reg_mspi_drv1 (82)
    #define mask_of_chiptop_reg_mspi_drv1 (0x70)
    unsigned int reg_mspi_drv1:3;

    // h0029, bit: 7
    /* */
    unsigned int :1;

    // h0029, bit: 13
    /* Control driving strength*/
    #define offset_of_chiptop_reg_sd_out_drv (82)
    #define mask_of_chiptop_reg_sd_out_drv (0x3f00)
    unsigned int reg_sd_out_drv:6;

    // h0029, bit: 14
    /* */
    unsigned int :1;

    // h0029, bit: 15
    /* Control driving strength*/
    #define offset_of_chiptop_reg_spdif_out_drv (82)
    #define mask_of_chiptop_reg_spdif_out_drv (0x8000)
    unsigned int reg_spdif_out_drv:1;

    // h0029
    unsigned int /* padding 16 bit */:16;

    // h002a, bit: 14
    /* */
    unsigned int :16;

    // h002a
    unsigned int /* padding 16 bit */:16;

    // h002b, bit: 14
    /* */
    unsigned int :16;

    // h002b
    unsigned int /* padding 16 bit */:16;

    // h002c, bit: 14
    /* */
    unsigned int :16;

    // h002c
    unsigned int /* padding 16 bit */:16;

    // h002d, bit: 14
    /* */
    unsigned int :16;

    // h002d
    unsigned int /* padding 16 bit */:16;

    // h002e, bit: 14
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_pe0 (92)
    #define mask_of_chiptop_reg_nand_pe0 (0xffff)
    unsigned int reg_nand_pe0:16;

    // h002e
    unsigned int /* padding 16 bit */:16;

    // h002f, bit: 14
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_ps0 (94)
    #define mask_of_chiptop_reg_nand_ps0 (0xffff)
    unsigned int reg_nand_ps0:16;

    // h002f
    unsigned int /* padding 16 bit */:16;

    // h0030, bit: 14
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_drv0 (96)
    #define mask_of_chiptop_reg_nand_drv0 (0xffff)
    unsigned int reg_nand_drv0:16;

    // h0030
    unsigned int /* padding 16 bit */:16;

    // h0031, bit: 1
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_pe1 (98)
    #define mask_of_chiptop_reg_nand_pe1 (0x3)
    unsigned int reg_nand_pe1:2;

    // h0031, bit: 3
    /* */
    unsigned int :2;

    // h0031, bit: 5
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_ps1 (98)
    #define mask_of_chiptop_reg_nand_ps1 (0x30)
    unsigned int reg_nand_ps1:2;

    // h0031, bit: 7
    /* */
    unsigned int :2;

    // h0031, bit: 9
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_drv1 (98)
    #define mask_of_chiptop_reg_nand_drv1 (0x300)
    unsigned int reg_nand_drv1:2;

    // h0031, bit: 10
    /* */
    unsigned int :1;

    // h0031, bit: 12
    /* Nand pad control*/
    #define offset_of_chiptop_reg_nand_drv4 (98)
    #define mask_of_chiptop_reg_nand_drv4 (0x1800)
    unsigned int reg_nand_drv4:2;

    // h0031, bit: 14
    /* */
    unsigned int :3;

    // h0031
    unsigned int /* padding 16 bit */:16;

    // h0032, bit: 5
    /* Nand pad control*/
    #define offset_of_chiptop_reg_sdio_pe (100)
    #define mask_of_chiptop_reg_sdio_pe (0x3f)
    unsigned int reg_sdio_pe:6;

    // h0032, bit: 7
    /* */
    unsigned int :2;

    // h0032, bit: 13
    /* Nand pad control*/
    #define offset_of_chiptop_reg_sdio_ps (100)
    #define mask_of_chiptop_reg_sdio_ps (0x3f00)
    unsigned int reg_sdio_ps:6;

    // h0032, bit: 14
    /* */
    unsigned int :2;

    // h0032
    unsigned int /* padding 16 bit */:16;

    // h0033, bit: 5
    /* Nand pad control*/
    #define offset_of_chiptop_reg_sdio_drv (102)
    #define mask_of_chiptop_reg_sdio_drv (0x3f)
    unsigned int reg_sdio_drv:6;

    // h0033, bit: 14
    /* */
    unsigned int :10;

    // h0033
    unsigned int /* padding 16 bit */:16;

    // h0034, bit: 14
    /* riu write clock mask*/
    #define offset_of_chiptop_reg_riu_wclk_mask (104)
    #define mask_of_chiptop_reg_riu_wclk_mask (0xffff)
    unsigned int reg_riu_wclk_mask:16;

    // h0034
    unsigned int /* padding 16 bit */:16;

    // h0035, bit: 0
    /* reg_hdmitx_arc_bypass*/
    #define offset_of_chiptop_reg_hdmitx_arc_bypass (106)
    #define mask_of_chiptop_reg_hdmitx_arc_bypass (0x1)
    unsigned int reg_hdmitx_arc_bypass:1;

    // h0035, bit: 1
    /* reg_hdmirx_arc_bypass*/
    #define offset_of_chiptop_reg_hdmirx_arc_bypass (106)
    #define mask_of_chiptop_reg_hdmirx_arc_bypass (0x2)
    unsigned int reg_hdmirx_arc_bypass:1;

    // h0035, bit: 14
    /* */
    unsigned int :14;

    // h0035
    unsigned int /* padding 16 bit */:16;

    // h0036, bit: 3
    /* */
    unsigned int :4;

    // h0036, bit: 4
    /* EMMC SWRSTZ*/
    #define offset_of_chiptop_reg_emmc_rstn_sw (108)
    #define mask_of_chiptop_reg_emmc_rstn_sw (0x10)
    unsigned int reg_emmc_rstn_sw:1;

    // h0036, bit: 5
    /* EMMC SWRSTZ enable*/
    #define offset_of_chiptop_reg_emmc_rstn_en (108)
    #define mask_of_chiptop_reg_emmc_rstn_en (0x20)
    unsigned int reg_emmc_rstn_en:1;

    // h0036, bit: 14
    /* */
    unsigned int :10;

    // h0036
    unsigned int /* padding 16 bit */:16;

    // h0037, bit: 0
    /* in_sel_sbus*/
    #define offset_of_chiptop_reg_in_sel_sbus (110)
    #define mask_of_chiptop_reg_in_sel_sbus (0x1)
    unsigned int reg_in_sel_sbus:1;

    // h0037, bit: 3
    /* */
    unsigned int :3;

    // h0037, bit: 4
    /* in_sel_dbus*/
    #define offset_of_chiptop_reg_in_sel_dbus (110)
    #define mask_of_chiptop_reg_in_sel_dbus (0x10)
    unsigned int reg_in_sel_dbus:1;

    // h0037, bit: 14
    /* */
    unsigned int :11;

    // h0037
    unsigned int /* padding 16 bit */:16;

    // h0038, bit: 14
    /* */
    unsigned int :16;

    // h0038
    unsigned int /* padding 16 bit */:16;

    // h0039, bit: 14
    /* */
    unsigned int :16;

    // h0039
    unsigned int /* padding 16 bit */:16;

    // h003a, bit: 14
    /* */
    unsigned int :16;

    // h003a
    unsigned int /* padding 16 bit */:16;

    // h003b, bit: 14
    /* */
    unsigned int :16;

    // h003b
    unsigned int /* padding 16 bit */:16;

    // h003c, bit: 0
    /* one way register to lock all force on registers*/
    #define offset_of_chiptop_reg_pll_gater_force_on_lock (120)
    #define mask_of_chiptop_reg_pll_gater_force_on_lock (0x1)
    unsigned int reg_pll_gater_force_on_lock:1;

    // h003c, bit: 1
    /* one way register to lock all force off registers*/
    #define offset_of_chiptop_reg_pll_gater_force_off_lock (120)
    #define mask_of_chiptop_reg_pll_gater_force_off_lock (0x2)
    unsigned int reg_pll_gater_force_off_lock:1;

    // h003c, bit: 14
    /* */
    unsigned int :14;

    // h003c
    unsigned int /* padding 16 bit */:16;

    // h003d, bit: 0
    /* upll_384_force_on*/
    #define offset_of_chiptop_reg_upll_384_force_on (122)
    #define mask_of_chiptop_reg_upll_384_force_on (0x1)
    unsigned int reg_upll_384_force_on:1;

    // h003d, bit: 1
    /* upll_320_force_on*/
    #define offset_of_chiptop_reg_upll_320_force_on (122)
    #define mask_of_chiptop_reg_upll_320_force_on (0x2)
    unsigned int reg_upll_320_force_on:1;

    // h003d, bit: 2
    /* utmi_160_force_on*/
    #define offset_of_chiptop_reg_utmi_160_force_on (122)
    #define mask_of_chiptop_reg_utmi_160_force_on (0x4)
    unsigned int reg_utmi_160_force_on:1;

    // h003d, bit: 3
    /* utmi_192_force_on*/
    #define offset_of_chiptop_reg_utmi_192_force_on (122)
    #define mask_of_chiptop_reg_utmi_192_force_on (0x8)
    unsigned int reg_utmi_192_force_on:1;

    // h003d, bit: 4
    /* utmi_240_force_on*/
    #define offset_of_chiptop_reg_utmi_240_force_on (122)
    #define mask_of_chiptop_reg_utmi_240_force_on (0x10)
    unsigned int reg_utmi_240_force_on:1;

    // h003d, bit: 5
    /* utmi_480_force_on*/
    #define offset_of_chiptop_reg_utmi_480_force_on (122)
    #define mask_of_chiptop_reg_utmi_480_force_on (0x20)
    unsigned int reg_utmi_480_force_on:1;

    // h003d, bit: 6
    /* mpll_432_force_on*/
    #define offset_of_chiptop_reg_mpll_432_force_on (122)
    #define mask_of_chiptop_reg_mpll_432_force_on (0x40)
    unsigned int reg_mpll_432_force_on:1;

    // h003d, bit: 7
    /* mpll_345_force_on*/
    #define offset_of_chiptop_reg_mpll_345_force_on (122)
    #define mask_of_chiptop_reg_mpll_345_force_on (0x80)
    unsigned int reg_mpll_345_force_on:1;

    // h003d, bit: 8
    /* mpll_288_force_on*/
    #define offset_of_chiptop_reg_mpll_288_force_on (122)
    #define mask_of_chiptop_reg_mpll_288_force_on (0x100)
    unsigned int reg_mpll_288_force_on:1;

    // h003d, bit: 9
    /* mpll_216_force_on*/
    #define offset_of_chiptop_reg_mpll_216_force_on (122)
    #define mask_of_chiptop_reg_mpll_216_force_on (0x200)
    unsigned int reg_mpll_216_force_on:1;

    // h003d, bit: 10
    /* mpll_172_force_on*/
    #define offset_of_chiptop_reg_mpll_172_force_on (122)
    #define mask_of_chiptop_reg_mpll_172_force_on (0x400)
    unsigned int reg_mpll_172_force_on:1;

    // h003d, bit: 11
    /* mpll_123_force_on*/
    #define offset_of_chiptop_reg_mpll_123_force_on (122)
    #define mask_of_chiptop_reg_mpll_123_force_on (0x800)
    unsigned int reg_mpll_123_force_on:1;

    // h003d, bit: 12
    /* mpll_124_force_on*/
    #define offset_of_chiptop_reg_mpll_124_force_on (122)
    #define mask_of_chiptop_reg_mpll_124_force_on (0x1000)
    unsigned int reg_mpll_124_force_on:1;

    // h003d, bit: 13
    /* mpll_86_force_on*/
    #define offset_of_chiptop_reg_mpll_86_force_on (122)
    #define mask_of_chiptop_reg_mpll_86_force_on (0x2000)
    unsigned int reg_mpll_86_force_on:1;

    // h003d, bit: 14
    /* pll_rv1_force_on*/
    #define offset_of_chiptop_reg_pll_rv1_force_on (122)
    #define mask_of_chiptop_reg_pll_rv1_force_on (0x4000)
    unsigned int reg_pll_rv1_force_on:1;

    // h003d, bit: 15
    /* pll_rv2_force_on*/
    #define offset_of_chiptop_reg_pll_rv2_force_on (122)
    #define mask_of_chiptop_reg_pll_rv2_force_on (0x8000)
    unsigned int reg_pll_rv2_force_on:1;

    // h003d
    unsigned int /* padding 16 bit */:16;

    // h003e, bit: 0
    /* clk_86_req*/
    #define offset_of_chiptop_reg_clk_86_req (124)
    #define mask_of_chiptop_reg_clk_86_req (0x1)
    unsigned int reg_clk_86_req:1;

    // h003e, bit: 1
    /* clk_123_req*/
    #define offset_of_chiptop_reg_clk_123_req (124)
    #define mask_of_chiptop_reg_clk_123_req (0x2)
    unsigned int reg_clk_123_req:1;

    // h003e, bit: 2
    /* clk_172_req*/
    #define offset_of_chiptop_reg_clk_172_req (124)
    #define mask_of_chiptop_reg_clk_172_req (0x4)
    unsigned int reg_clk_172_req:1;

    // h003e, bit: 3
    /* clk_216_req*/
    #define offset_of_chiptop_reg_clk_216_req (124)
    #define mask_of_chiptop_reg_clk_216_req (0x8)
    unsigned int reg_clk_216_req:1;

    // h003e, bit: 4
    /* clk_288_req*/
    #define offset_of_chiptop_reg_clk_288_req (124)
    #define mask_of_chiptop_reg_clk_288_req (0x10)
    unsigned int reg_clk_288_req:1;

    // h003e, bit: 5
    /* clk_144_req*/
    #define offset_of_chiptop_reg_clk_144_req (124)
    #define mask_of_chiptop_reg_clk_144_req (0x20)
    unsigned int reg_clk_144_req:1;

    // h003e, bit: 6
    /* clk_432_req*/
    #define offset_of_chiptop_reg_clk_432_req (124)
    #define mask_of_chiptop_reg_clk_432_req (0x40)
    unsigned int reg_clk_432_req:1;

    // h003e, bit: 7
    /* clk_mpll_req*/
    #define offset_of_chiptop_reg_clk_mpll_req (124)
    #define mask_of_chiptop_reg_clk_mpll_req (0x80)
    unsigned int reg_clk_mpll_req:1;

    // h003e, bit: 8
    /* clk_sm_ca_syn_27_req*/
    #define offset_of_chiptop_reg_clk_sm_ca_syn_27_req (124)
    #define mask_of_chiptop_reg_clk_sm_ca_syn_27_req (0x100)
    unsigned int reg_clk_sm_ca_syn_27_req:1;

    // h003e, bit: 9
    /* clk_sm_ca_syn_432_req*/
    #define offset_of_chiptop_reg_clk_sm_ca_syn_432_req (124)
    #define mask_of_chiptop_reg_clk_sm_ca_syn_432_req (0x200)
    unsigned int reg_clk_sm_ca_syn_432_req:1;

    // h003e, bit: 10
    /* clk_rv10_req*/
    #define offset_of_chiptop_reg_clk_rv10_req (124)
    #define mask_of_chiptop_reg_clk_rv10_req (0x400)
    unsigned int reg_clk_rv10_req:1;

    // h003e, bit: 11
    /* clk_rv11_req*/
    #define offset_of_chiptop_reg_clk_rv11_req (124)
    #define mask_of_chiptop_reg_clk_rv11_req (0x800)
    unsigned int reg_clk_rv11_req:1;

    // h003e, bit: 12
    /* clk_rv12_req*/
    #define offset_of_chiptop_reg_clk_rv12_req (124)
    #define mask_of_chiptop_reg_clk_rv12_req (0x1000)
    unsigned int reg_clk_rv12_req:1;

    // h003e, bit: 13
    /* clk_rv13_req*/
    #define offset_of_chiptop_reg_clk_rv13_req (124)
    #define mask_of_chiptop_reg_clk_rv13_req (0x2000)
    unsigned int reg_clk_rv13_req:1;

    // h003e, bit: 14
    /* clk_rv14_req*/
    #define offset_of_chiptop_reg_clk_rv14_req (124)
    #define mask_of_chiptop_reg_clk_rv14_req (0x4000)
    unsigned int reg_clk_rv14_req:1;

    // h003e, bit: 15
    /* clk_rv15_req*/
    #define offset_of_chiptop_reg_clk_rv15_req (124)
    #define mask_of_chiptop_reg_clk_rv15_req (0x8000)
    unsigned int reg_clk_rv15_req:1;

    // h003e
    unsigned int /* padding 16 bit */:16;

    // h003f, bit: 0
    /* upll_384_en_rd*/
    #define offset_of_chiptop_reg_upll_384_en_rd (126)
    #define mask_of_chiptop_reg_upll_384_en_rd (0x1)
    unsigned int reg_upll_384_en_rd:1;

    // h003f, bit: 1
    /* upll_320_en_rd*/
    #define offset_of_chiptop_reg_upll_320_en_rd (126)
    #define mask_of_chiptop_reg_upll_320_en_rd (0x2)
    unsigned int reg_upll_320_en_rd:1;

    // h003f, bit: 2
    /* utmi_160_en_rd*/
    #define offset_of_chiptop_reg_utmi_160_en_rd (126)
    #define mask_of_chiptop_reg_utmi_160_en_rd (0x4)
    unsigned int reg_utmi_160_en_rd:1;

    // h003f, bit: 3
    /* utmi_192_en_rd*/
    #define offset_of_chiptop_reg_utmi_192_en_rd (126)
    #define mask_of_chiptop_reg_utmi_192_en_rd (0x8)
    unsigned int reg_utmi_192_en_rd:1;

    // h003f, bit: 4
    /* utmi_240_en_rd*/
    #define offset_of_chiptop_reg_utmi_240_en_rd (126)
    #define mask_of_chiptop_reg_utmi_240_en_rd (0x10)
    unsigned int reg_utmi_240_en_rd:1;

    // h003f, bit: 5
    /* utmi_480_en_rd*/
    #define offset_of_chiptop_reg_utmi_480_en_rd (126)
    #define mask_of_chiptop_reg_utmi_480_en_rd (0x20)
    unsigned int reg_utmi_480_en_rd:1;

    // h003f, bit: 6
    /* mpll_432_en_rd*/
    #define offset_of_chiptop_reg_mpll_432_en_rd (126)
    #define mask_of_chiptop_reg_mpll_432_en_rd (0x40)
    unsigned int reg_mpll_432_en_rd:1;

    // h003f, bit: 7
    /* mpll_345_en_rd*/
    #define offset_of_chiptop_reg_mpll_345_en_rd (126)
    #define mask_of_chiptop_reg_mpll_345_en_rd (0x80)
    unsigned int reg_mpll_345_en_rd:1;

    // h003f, bit: 8
    /* mpll_288_en_rd*/
    #define offset_of_chiptop_reg_mpll_288_en_rd (126)
    #define mask_of_chiptop_reg_mpll_288_en_rd (0x100)
    unsigned int reg_mpll_288_en_rd:1;

    // h003f, bit: 9
    /* mpll_216_en_rd*/
    #define offset_of_chiptop_reg_mpll_216_en_rd (126)
    #define mask_of_chiptop_reg_mpll_216_en_rd (0x200)
    unsigned int reg_mpll_216_en_rd:1;

    // h003f, bit: 10
    /* mpll_172_en_rd*/
    #define offset_of_chiptop_reg_mpll_172_en_rd (126)
    #define mask_of_chiptop_reg_mpll_172_en_rd (0x400)
    unsigned int reg_mpll_172_en_rd:1;

    // h003f, bit: 11
    /* mpll_123_en_rd*/
    #define offset_of_chiptop_reg_mpll_123_en_rd (126)
    #define mask_of_chiptop_reg_mpll_123_en_rd (0x800)
    unsigned int reg_mpll_123_en_rd:1;

    // h003f, bit: 12
    /* mpll_124_en_rd*/
    #define offset_of_chiptop_reg_mpll_124_en_rd (126)
    #define mask_of_chiptop_reg_mpll_124_en_rd (0x1000)
    unsigned int reg_mpll_124_en_rd:1;

    // h003f, bit: 13
    /* mpll_86_en_rd*/
    #define offset_of_chiptop_reg_mpll_86_en_rd (126)
    #define mask_of_chiptop_reg_mpll_86_en_rd (0x2000)
    unsigned int reg_mpll_86_en_rd:1;

    // h003f, bit: 14
    /* pll_rv1_en_rd*/
    #define offset_of_chiptop_reg_pll_rv1_en_rd (126)
    #define mask_of_chiptop_reg_pll_rv1_en_rd (0x4000)
    unsigned int reg_pll_rv1_en_rd:1;

    // h003f, bit: 15
    /* pll_rv2_en_rd*/
    #define offset_of_chiptop_reg_pll_rv2_en_rd (126)
    #define mask_of_chiptop_reg_pll_rv2_en_rd (0x8000)
    unsigned int reg_pll_rv2_en_rd:1;

    // h003f
    unsigned int /* padding 16 bit */:16;

    // h0040, bit: 14
    /* i64 mode select*/
    #define offset_of_chiptop_reg_miu_group0_i64 (128)
    #define mask_of_chiptop_reg_miu_group0_i64 (0xffff)
    unsigned int reg_miu_group0_i64:16;

    // h0040
    unsigned int /* padding 16 bit */:16;

    // h0041, bit: 14
    /* i64 mode select*/
    #define offset_of_chiptop_reg_miu_group1_i64 (130)
    #define mask_of_chiptop_reg_miu_group1_i64 (0xffff)
    unsigned int reg_miu_group1_i64:16;

    // h0041
    unsigned int /* padding 16 bit */:16;

    // h0042, bit: 14
    /* i64 mode select*/
    #define offset_of_chiptop_reg_miu_group2_i64 (132)
    #define mask_of_chiptop_reg_miu_group2_i64 (0xffff)
    unsigned int reg_miu_group2_i64:16;

    // h0042
    unsigned int /* padding 16 bit */:16;

    // h0043, bit: 14
    /* i64 mode select*/
    #define offset_of_chiptop_reg_miu_group3_i64 (134)
    #define mask_of_chiptop_reg_miu_group3_i64 (0xffff)
    unsigned int reg_miu_group3_i64:16;

    // h0043
    unsigned int /* padding 16 bit */:16;

    // h0044, bit: 0
    /* upll_384_force_off*/
    #define offset_of_chiptop_reg_upll_384_force_off (136)
    #define mask_of_chiptop_reg_upll_384_force_off (0x1)
    unsigned int reg_upll_384_force_off:1;

    // h0044, bit: 1
    /* upll_320_force_off*/
    #define offset_of_chiptop_reg_upll_320_force_off (136)
    #define mask_of_chiptop_reg_upll_320_force_off (0x2)
    unsigned int reg_upll_320_force_off:1;

    // h0044, bit: 2
    /* utmi_160_force_off*/
    #define offset_of_chiptop_reg_utmi_160_force_off (136)
    #define mask_of_chiptop_reg_utmi_160_force_off (0x4)
    unsigned int reg_utmi_160_force_off:1;

    // h0044, bit: 3
    /* utmi_192_force_off*/
    #define offset_of_chiptop_reg_utmi_192_force_off (136)
    #define mask_of_chiptop_reg_utmi_192_force_off (0x8)
    unsigned int reg_utmi_192_force_off:1;

    // h0044, bit: 4
    /* utmi_240_force_off*/
    #define offset_of_chiptop_reg_utmi_240_force_off (136)
    #define mask_of_chiptop_reg_utmi_240_force_off (0x10)
    unsigned int reg_utmi_240_force_off:1;

    // h0044, bit: 5
    /* utmi_480_force_off*/
    #define offset_of_chiptop_reg_utmi_480_force_off (136)
    #define mask_of_chiptop_reg_utmi_480_force_off (0x20)
    unsigned int reg_utmi_480_force_off:1;

    // h0044, bit: 6
    /* mpll_432_force_off*/
    #define offset_of_chiptop_reg_mpll_432_force_off (136)
    #define mask_of_chiptop_reg_mpll_432_force_off (0x40)
    unsigned int reg_mpll_432_force_off:1;

    // h0044, bit: 7
    /* mpll_345_force_off*/
    #define offset_of_chiptop_reg_mpll_345_force_off (136)
    #define mask_of_chiptop_reg_mpll_345_force_off (0x80)
    unsigned int reg_mpll_345_force_off:1;

    // h0044, bit: 8
    /* mpll_288_force_off*/
    #define offset_of_chiptop_reg_mpll_288_force_off (136)
    #define mask_of_chiptop_reg_mpll_288_force_off (0x100)
    unsigned int reg_mpll_288_force_off:1;

    // h0044, bit: 9
    /* mpll_216_force_off*/
    #define offset_of_chiptop_reg_mpll_216_force_off (136)
    #define mask_of_chiptop_reg_mpll_216_force_off (0x200)
    unsigned int reg_mpll_216_force_off:1;

    // h0044, bit: 10
    /* mpll_172_force_off*/
    #define offset_of_chiptop_reg_mpll_172_force_off (136)
    #define mask_of_chiptop_reg_mpll_172_force_off (0x400)
    unsigned int reg_mpll_172_force_off:1;

    // h0044, bit: 11
    /* mpll_123_force_off*/
    #define offset_of_chiptop_reg_mpll_123_force_off (136)
    #define mask_of_chiptop_reg_mpll_123_force_off (0x800)
    unsigned int reg_mpll_123_force_off:1;

    // h0044, bit: 12
    /* mpll_124_force_off*/
    #define offset_of_chiptop_reg_mpll_124_force_off (136)
    #define mask_of_chiptop_reg_mpll_124_force_off (0x1000)
    unsigned int reg_mpll_124_force_off:1;

    // h0044, bit: 13
    /* mpll_86_force_off*/
    #define offset_of_chiptop_reg_mpll_86_force_off (136)
    #define mask_of_chiptop_reg_mpll_86_force_off (0x2000)
    unsigned int reg_mpll_86_force_off:1;

    // h0044, bit: 14
    /* */
    unsigned int :2;

    // h0044
    unsigned int /* padding 16 bit */:16;

    // h0045, bit: 14
    /* [15:12] reserved for NDS
    Reserved register*/
    #define offset_of_chiptop_reg_reserved3 (138)
    #define mask_of_chiptop_reg_reserved3 (0xffff)
    unsigned int reg_reserved3:16;

    // h0045
    unsigned int /* padding 16 bit */:16;

    // h0046, bit: 3
    /* SW reset for MIU-HW div2 circuit(0 : active reset)
    [0] : reset miu0 MIU-HWd div2 circuit.
    [3:2] : reserved.*/
    #define offset_of_chiptop_reg_miu2x_div_rstz (140)
    #define mask_of_chiptop_reg_miu2x_div_rstz (0xf)
    unsigned int reg_miu2x_div_rstz:4;

    // h0046, bit: 14
    /* */
    unsigned int :12;

    // h0046
    unsigned int /* padding 16 bit */:16;

    // h0047, bit: 0
    /* reg_hsync_en*/
    #define offset_of_chiptop_reg_hsync_en (142)
    #define mask_of_chiptop_reg_hsync_en (0x1)
    unsigned int reg_hsync_en:1;

    // h0047, bit: 1
    /* reg_vsync_en*/
    #define offset_of_chiptop_reg_vsync_en (142)
    #define mask_of_chiptop_reg_vsync_en (0x2)
    unsigned int reg_vsync_en:1;

    // h0047, bit: 14
    /* */
    unsigned int :14;

    // h0047
    unsigned int /* padding 16 bit */:16;

    // h0048, bit: 7
    /* reg_u3d_miucross_rd_onflyma*/
    #define offset_of_chiptop_reg_u3d_miucross_rd_onflyma (144)
    #define mask_of_chiptop_reg_u3d_miucross_rd_onflyma (0xff)
    unsigned int reg_u3d_miucross_rd_onflyma:8;

    // h0048, bit: 8
    /* reg_u3d_miucross_eqburst_mode*/
    #define offset_of_chiptop_reg_u3d_miucross_eqburst_mode (144)
    #define mask_of_chiptop_reg_u3d_miucross_eqburst_mode (0x100)
    unsigned int reg_u3d_miucross_eqburst_mode:1;

    // h0048, bit: 11
    /* */
    unsigned int :3;

    // h0048, bit: 13
    /* reg_u3d_miucross_bus_error  */
    #define offset_of_chiptop_reg_u3d_miucross_bus_error (144)
    #define mask_of_chiptop_reg_u3d_miucross_bus_error (0x3000)
    unsigned int reg_u3d_miucross_bus_error:2;

    // h0048, bit: 14
    /* */
    unsigned int :2;

    // h0048
    unsigned int /* padding 16 bit */:16;

    // h0049, bit: 4
    /* reg_cmdq_riu_pre_arb_mask*/
    #define offset_of_chiptop_reg_cmdq_riu_pre_arb_mask (146)
    #define mask_of_chiptop_reg_cmdq_riu_pre_arb_mask (0x1f)
    unsigned int reg_cmdq_riu_pre_arb_mask:5;

    // h0049, bit: 14
    /* */
    unsigned int :11;

    // h0049
    unsigned int /* padding 16 bit */:16;

    // h004a, bit: 14
    /* */
    unsigned int :16;

    // h004a
    unsigned int /* padding 16 bit */:16;

    // h004b, bit: 14
    /* */
    unsigned int :16;

    // h004b
    unsigned int /* padding 16 bit */:16;

    // h004c, bit: 14
    /* */
    unsigned int :16;

    // h004c
    unsigned int /* padding 16 bit */:16;

    // h004d, bit: 14
    /* */
    unsigned int :16;

    // h004d
    unsigned int /* padding 16 bit */:16;

    // h004e, bit: 14
    /* */
    unsigned int :16;

    // h004e
    unsigned int /* padding 16 bit */:16;

    // h004f, bit: 14
    /* */
    unsigned int :16;

    // h004f
    unsigned int /* padding 16 bit */:16;

    // h0050, bit: 14
    /* */
    unsigned int :15;

    // h0050, bit: 15
    /* 1: Set all pads (except SPI) as input*/
    #define offset_of_chiptop_reg_allpad_in (160)
    #define mask_of_chiptop_reg_allpad_in (0x8000)
    unsigned int reg_allpad_in:1;

    // h0050
    unsigned int /* padding 16 bit */:16;

    // h0051, bit: 0
    /* jpi_sram_power_down*/
    #define offset_of_chiptop_reg_jpi_sd (162)
    #define mask_of_chiptop_reg_jpi_sd (0x1)
    unsigned int reg_jpi_sd:1;

    // h0051, bit: 14
    /* */
    unsigned int :15;

    // h0051
    unsigned int /* padding 16 bit */:16;

    // h0052, bit: 7
    /* bonding overwrite enable*/
    #define offset_of_chiptop_reg_bond_ov_en6 (164)
    #define mask_of_chiptop_reg_bond_ov_en6 (0xff)
    unsigned int reg_bond_ov_en6:8;

    // h0052, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov6 (164)
    #define mask_of_chiptop_reg_bond_ov6 (0xff00)
    unsigned int reg_bond_ov6:8;

    // h0052
    unsigned int /* padding 16 bit */:16;

    // h0053, bit: 3
    /* Select UART0 source for PAD_DDCA_DA and PAD_DDCA_CK
    0000:  SEC R2
    0001:  PIU UART1
    0010:  VD_MHEG5
    0011:  TSP
    0100:  PIU UART0
    0101:  AIDIO R2-1 UART
    0110:  AIDIO R2-0 UART
    0111:  PIU FAST UART
    1000:  VD R2 L
    1001:  N/A
    1010:  SEC 51
    1011:  N/A
    Note: For PAD_DDCA_DA and PAD_DDCA_CK, please refer to the "reg_hk51_uart0_en" and "reg_uart_rx_enable" in reg_pm_sleep.xls
    (a). "reg_hk51_uart0_en" == 0
    (b). "reg_uart_rx_enable" == 1*/
    #define offset_of_chiptop_reg_uart_sel0 (166)
    #define mask_of_chiptop_reg_uart_sel0 (0xf)
    unsigned int reg_uart_sel0:4;

    // h0053, bit: 7
    /* Select UART1 source*/
    #define offset_of_chiptop_reg_uart_sel1 (166)
    #define mask_of_chiptop_reg_uart_sel1 (0xf0)
    unsigned int reg_uart_sel1:4;

    // h0053, bit: 11
    /* Select UART2 source*/
    #define offset_of_chiptop_reg_uart_sel2 (166)
    #define mask_of_chiptop_reg_uart_sel2 (0xf00)
    unsigned int reg_uart_sel2:4;

    // h0053, bit: 14
    /* Select UART3 source*/
    #define offset_of_chiptop_reg_uart_sel3 (166)
    #define mask_of_chiptop_reg_uart_sel3 (0xf000)
    unsigned int reg_uart_sel3:4;

    // h0053
    unsigned int /* padding 16 bit */:16;

    // h0054, bit: 3
    /* Select UART4 source*/
    #define offset_of_chiptop_reg_uart_sel4 (168)
    #define mask_of_chiptop_reg_uart_sel4 (0xf)
    unsigned int reg_uart_sel4:4;

    // h0054, bit: 7
    /* */
    unsigned int :4;

    // h0054, bit: 8
    /* reg_tsp_boot_clk_sel
    0:from clk_tsp
    1:from clk_nfie*/
    #define offset_of_chiptop_reg_tsp_boot_clk_sel (168)
    #define mask_of_chiptop_reg_tsp_boot_clk_sel (0x100)
    unsigned int reg_tsp_boot_clk_sel:1;

    // h0054, bit: 14
    /* */
    unsigned int :7;

    // h0054
    unsigned int /* padding 16 bit */:16;

    // h0055, bit: 3
    /* JTAG selection
    000:  ADP_MHEG5
    001:  MHEG5
    010:  VD_MHEG5
    100:  MIPS*/
    #define offset_of_chiptop_reg_jtag_sel (170)
    #define mask_of_chiptop_reg_jtag_sel (0xf)
    unsigned int reg_jtag_sel:4;

    // h0055, bit: 7
    /* */
    unsigned int :4;

    // h0055, bit: 14
    /* invert PAD UART TX/RX*/
    #define offset_of_chiptop_reg_uart_pad_inverse (170)
    #define mask_of_chiptop_reg_uart_pad_inverse (0xff00)
    unsigned int reg_uart_pad_inverse:8;

    // h0055
    unsigned int /* padding 16 bit */:16;

    // h0056, bit: 7
    /* enable of inner loopback test for 8 sets of UART controller
    [0]: UART0 enable
    [1]: UART1 enable
    [2]: UART2 enable
    [3]: UART3 enable
    [4]: UART4 enable
    [5]: UART5 enable
    [6]: UART6 enable
    [7]: UART7 enable*/
    #define offset_of_chiptop_reg_uart_inner_loopback (172)
    #define mask_of_chiptop_reg_uart_inner_loopback (0xff)
    unsigned int reg_uart_inner_loopback:8;

    // h0056, bit: 14
    /* enable of outer loopback test for 3 sets of UART pad
    [0]: UART0 pad enable
    [1]: UART1 pad enable
    [2]: UART2 pad enable*/
    #define offset_of_chiptop_reg_uart_outer_loopback (172)
    #define mask_of_chiptop_reg_uart_outer_loopback (0xff00)
    unsigned int reg_uart_outer_loopback:8;

    // h0056
    unsigned int /* padding 16 bit */:16;

    // h0057, bit: 14
    /* reg_force_rx_disable*/
    #define offset_of_chiptop_reg_force_rx_disable (174)
    #define mask_of_chiptop_reg_force_rx_disable (0xffff)
    unsigned int reg_force_rx_disable:16;

    // h0057
    unsigned int /* padding 16 bit */:16;

    // h0058, bit: 7
    /* bonding overwrite enable
    Bit0: BOND_VP8
    Bit1: BOND_DVB_SEL[0]
    Bit2: BOND_DVB_SEL[1]
    Bit3: BOND_WATERMARK
    Reserved.*/
    #define offset_of_chiptop_reg_bond_ov_en2 (176)
    #define mask_of_chiptop_reg_bond_ov_en2 (0xff)
    unsigned int reg_bond_ov_en2:8;

    // h0058, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov2 (176)
    #define mask_of_chiptop_reg_bond_ov2 (0xff00)
    unsigned int reg_bond_ov2:8;

    // h0058
    unsigned int /* padding 16 bit */:16;

    // h0059, bit: 7
    /* bonding overwrite enable
    Bit7: BOND_MACRO_VISION[2]
    Bit6: BOND_MACRO_VISION[1]
    Bit5: BOND_MACRO_VISION[0]
    Bit4: BOND_DCS[1]
    Bit3: BOND_DCS[0]
    Reserved.*/
    #define offset_of_chiptop_reg_bond_ov_en3 (178)
    #define mask_of_chiptop_reg_bond_ov_en3 (0xff)
    unsigned int reg_bond_ov_en3:8;

    // h0059, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov3 (178)
    #define mask_of_chiptop_reg_bond_ov3 (0xff00)
    unsigned int reg_bond_ov3:8;

    // h0059
    unsigned int /* padding 16 bit */:16;

    // h005a, bit: 7
    /* bonding overwrite enable*/
    #define offset_of_chiptop_reg_bond_ov_en4 (180)
    #define mask_of_chiptop_reg_bond_ov_en4 (0xff)
    unsigned int reg_bond_ov_en4:8;

    // h005a, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov4 (180)
    #define mask_of_chiptop_reg_bond_ov4 (0xff00)
    unsigned int reg_bond_ov4:8;

    // h005a
    unsigned int /* padding 16 bit */:16;

    // h005b, bit: 14
    /* audio bonding overwrite enable
    Bit 0: BOND_AUDIO0
    Bit 1: BOND_AUDIO1
    Bit 2: BOND_AUDIO2
    Bit 3: BOND_AUDIO3
    Bit 4: BOND_AUDIO4
    Bit 5: BOND_AUDIO5
    Bit 6: BOND_AUDIO6
    Bit 7: BOND_AUDIO7
    Bit 8: BOND_AUDIO8
    Bit 9: BOND_AUDIO9
    Bit 10: BOND_AUDIO10
    Bit 11: BOND_AUDIO11
    Bit 12: BOND_AUDIO12
    Bit 13: BOND_AUDIO13
    Bit 14: BOND_AUDIO14
    Bit 15: BOND_AUDIO15*/
    #define offset_of_chiptop_reg_bond_audio_ov_en (182)
    #define mask_of_chiptop_reg_bond_audio_ov_en (0xffff)
    unsigned int reg_bond_audio_ov_en:16;

    // h005b
    unsigned int /* padding 16 bit */:16;

    // h005c, bit: 14
    /* audio bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_audio_ov (184)
    #define mask_of_chiptop_reg_bond_audio_ov (0xffff)
    unsigned int reg_bond_audio_ov:16;

    // h005c
    unsigned int /* padding 16 bit */:16;

    // h005d, bit: 7
    /* bonding overwrite enable
    Bit0: BOND_VC1
    Bit1: BOND_M2V
    Bit2: BOND_M4V
    Bit3: BOND_DIVX311
    Bit4:BOND_DIVX456
    Bit5: BOND_FLV
    Bit6: BOND_MACRO_VISION
    Bit7: BOND_DCS*/
    #define offset_of_chiptop_reg_bond_ov_en1 (186)
    #define mask_of_chiptop_reg_bond_ov_en1 (0xff)
    unsigned int reg_bond_ov_en1:8;

    // h005d, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov1 (186)
    #define mask_of_chiptop_reg_bond_ov1 (0xff00)
    unsigned int reg_bond_ov1:8;

    // h005d
    unsigned int /* padding 16 bit */:16;

    // h005e, bit: 7
    /* bonding overwrite enable
    Bit 0: BOND_MIPS
    Bit 1: BOND_AVS
    Bit 2: BOND_RVD
    Bit 3: BOND_H264
    Bit 4: BOND_OD
    Bit 5: BOND_WUXGA
    Bit 6: BOND_PIP
    Bit 7: BOND_HD*/
    #define offset_of_chiptop_reg_bond_ov_en (188)
    #define mask_of_chiptop_reg_bond_ov_en (0xff)
    unsigned int reg_bond_ov_en:8;

    // h005e, bit: 13
    /* bonding overwrite enable
    Bit 0: BOND_SW0
    Bit 1: BOND_SW1
    Bit 2: BOND_SW2
    Bit 3: BOND_SW3
    Bit 4: BOND_SW4
    Bit 5: BOND_SW5*/
    #define offset_of_chiptop_reg_bond_sw_ov_en (188)
    #define mask_of_chiptop_reg_bond_sw_ov_en (0x3f00)
    unsigned int reg_bond_sw_ov_en:6;

    // h005e, bit: 14
    /* */
    unsigned int :2;

    // h005e
    unsigned int /* padding 16 bit */:16;

    // h005f, bit: 7
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov (190)
    #define mask_of_chiptop_reg_bond_ov (0xff)
    unsigned int reg_bond_ov:8;

    // h005f, bit: 13
    /* SW bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_sw_ov (190)
    #define mask_of_chiptop_reg_bond_sw_ov (0x3f00)
    unsigned int reg_bond_sw_ov:6;

    // h005f, bit: 14
    /* */
    unsigned int :2;

    // h005f
    unsigned int /* padding 16 bit */:16;

    // h0060, bit: 7
    /* bonding status*/
    #define offset_of_chiptop_reg_stat_bond (192)
    #define mask_of_chiptop_reg_stat_bond (0xff)
    unsigned int reg_stat_bond:8;

    // h0060, bit: 13
    /* SW bonding status*/
    #define offset_of_chiptop_reg_stat_bond_sw (192)
    #define mask_of_chiptop_reg_stat_bond_sw (0x3f00)
    unsigned int reg_stat_bond_sw:6;

    // h0060, bit: 14
    /* */
    unsigned int :2;

    // h0060
    unsigned int /* padding 16 bit */:16;

    // h0061, bit: 7
    /* bonding status1*/
    #define offset_of_chiptop_reg_stat_bond1 (194)
    #define mask_of_chiptop_reg_stat_bond1 (0xff)
    unsigned int reg_stat_bond1:8;

    // h0061, bit: 14
    /* bonding status2*/
    #define offset_of_chiptop_reg_stat_bond2 (194)
    #define mask_of_chiptop_reg_stat_bond2 (0xff00)
    unsigned int reg_stat_bond2:8;

    // h0061
    unsigned int /* padding 16 bit */:16;

    // h0062, bit: 14
    /* audio bonding status*/
    #define offset_of_chiptop_reg_stat_bond_audio (196)
    #define mask_of_chiptop_reg_stat_bond_audio (0xffff)
    unsigned int reg_stat_bond_audio:16;

    // h0062
    unsigned int /* padding 16 bit */:16;

    // h0063, bit: 14
    /* set bonding overwrite key*/
    #define offset_of_chiptop_reg_bond_ov_key (198)
    #define mask_of_chiptop_reg_bond_ov_key (0xffff)
    unsigned int reg_bond_ov_key:16;

    // h0063
    unsigned int /* padding 16 bit */:16;

    // h0064, bit: 7
    /* bonding status3*/
    #define offset_of_chiptop_reg_stat_bond3 (200)
    #define mask_of_chiptop_reg_stat_bond3 (0xff)
    unsigned int reg_stat_bond3:8;

    // h0064, bit: 14
    /* */
    unsigned int :8;

    // h0064
    unsigned int /* padding 16 bit */:16;

    // h0065, bit: 4
    /* CHIP_CONFIG status*/
    #define offset_of_chiptop_reg_chip_config_stat (202)
    #define mask_of_chiptop_reg_chip_config_stat (0x1f)
    unsigned int reg_chip_config_stat:5;

    // h0065, bit: 14
    /* */
    unsigned int :11;

    // h0065
    unsigned int /* padding 16 bit */:16;

    // h0066, bit: 7
    /* bonding status4*/
    #define offset_of_chiptop_reg_stat_bond4 (204)
    #define mask_of_chiptop_reg_stat_bond4 (0xff)
    unsigned int reg_stat_bond4:8;

    // h0066, bit: 14
    /* */
    unsigned int :8;

    // h0066
    unsigned int /* padding 16 bit */:16;

    // h0067, bit: 7
    /* bonding status5*/
    #define offset_of_chiptop_reg_stat_bond5 (206)
    #define mask_of_chiptop_reg_stat_bond5 (0xff)
    unsigned int reg_stat_bond5:8;

    // h0067, bit: 14
    /* bonding status6*/
    #define offset_of_chiptop_reg_stat_bond6 (206)
    #define mask_of_chiptop_reg_stat_bond6 (0xff00)
    unsigned int reg_stat_bond6:8;

    // h0067
    unsigned int /* padding 16 bit */:16;

    // h0068, bit: 7
    /* bonding overwrite enable
    Bit0: BOND_SPI_OFFSET[0]
    Bit1: BOND_SPI_OFFSET[1]
    Bit2: BOND_SPI_OFFSET[2]
    Bit3: BOND_SPI_OFFSET[3]
    Bit4:reserved
    Bit5: reserved
    Bit6: reserved
    Bit7: reserved*/
    #define offset_of_chiptop_reg_bond_ov_en5 (208)
    #define mask_of_chiptop_reg_bond_ov_en5 (0xff)
    unsigned int reg_bond_ov_en5:8;

    // h0068, bit: 14
    /* bonding overwrite value*/
    #define offset_of_chiptop_reg_bond_ov5 (208)
    #define mask_of_chiptop_reg_bond_ov5 (0xff00)
    unsigned int reg_bond_ov5:8;

    // h0068
    unsigned int /* padding 16 bit */:16;

    // h0069, bit: 0
    /* 0: sel u2 in u3
    1: sel u2 only*/
    #define offset_of_chiptop_reg_utmi_mux_sel (210)
    #define mask_of_chiptop_reg_utmi_mux_sel (0x1)
    unsigned int reg_utmi_mux_sel:1;

    // h0069, bit: 14
    /* */
    unsigned int :15;

    // h0069
    unsigned int /* padding 16 bit */:16;

    // h006a, bit: 14
    /* The booting address of SDRAM*/
    #define offset_of_chiptop_reg_boot_from_sdram_offset (212)
    #define mask_of_chiptop_reg_boot_from_sdram_offset (0xffff)
    unsigned int reg_boot_from_sdram_offset:16;

    // h006a
    unsigned int /* padding 16 bit */:16;

    // h006b, bit: 9
    /* The booting address of SDRAM*/
    #define offset_of_chiptop_reg_boot_from_sdram_offset_1 (214)
    #define mask_of_chiptop_reg_boot_from_sdram_offset_1 (0x3ff)
    unsigned int reg_boot_from_sdram_offset_1:10;

    // h006b, bit: 14
    /* */
    unsigned int :6;

    // h006b
    unsigned int /* padding 16 bit */:16;

    // h006c, bit: 14
    /* CPU warm boot address offset*/
    #define offset_of_chiptop_reg_cpu_warmboot_adr_oneway (216)
    #define mask_of_chiptop_reg_cpu_warmboot_adr_oneway (0xffff)
    unsigned int reg_cpu_warmboot_adr_oneway:16;

    // h006c
    unsigned int /* padding 16 bit */:16;

    // h006d, bit: 7
    /* CPU warm boot address offset*/
    #define offset_of_chiptop_reg_cpu_warmboot_adr_oneway_1 (218)
    #define mask_of_chiptop_reg_cpu_warmboot_adr_oneway_1 (0xff)
    unsigned int reg_cpu_warmboot_adr_oneway_1:8;

    // h006d, bit: 14
    /* */
    unsigned int :7;

    // h006d, bit: 15
    /* oneway lock register for reg_cpu_warmboot_adr_oneway*/
    #define offset_of_chiptop_reg_cpu_warmboot_adr_oneway_lock (218)
    #define mask_of_chiptop_reg_cpu_warmboot_adr_oneway_lock (0x8000)
    unsigned int reg_cpu_warmboot_adr_oneway_lock:1;

    // h006d
    unsigned int /* padding 16 bit */:16;

    // h006e, bit: 1
    /* reg_arc_rx_bias_sel*/
    #define offset_of_chiptop_reg_arc_rx_bias_sel (220)
    #define mask_of_chiptop_reg_arc_rx_bias_sel (0x3)
    unsigned int reg_arc_rx_bias_sel:2;

    // h006e, bit: 3
    /* reg_arc_rx_res_sel*/
    #define offset_of_chiptop_reg_arc_rx_res_sel (220)
    #define mask_of_chiptop_reg_arc_rx_res_sel (0xc)
    unsigned int reg_arc_rx_res_sel:2;

    // h006e, bit: 7
    /* */
    unsigned int :4;

    // h006e, bit: 11
    /* reg_arc_rx_rt_trim*/
    #define offset_of_chiptop_reg_arc_rx_rt_trim (220)
    #define mask_of_chiptop_reg_arc_rx_rt_trim (0xf00)
    unsigned int reg_arc_rx_rt_trim:4;

    // h006e, bit: 12
    /* reg_en_arc_bypass*/
    #define offset_of_chiptop_reg_en_arc_bypass (220)
    #define mask_of_chiptop_reg_en_arc_bypass (0x1000)
    unsigned int reg_en_arc_bypass:1;

    // h006e, bit: 13
    /* reg_pd_rt*/
    #define offset_of_chiptop_reg_pd_rt (220)
    #define mask_of_chiptop_reg_pd_rt (0x2000)
    unsigned int reg_pd_rt:1;

    // h006e, bit: 14
    /* reg_en_arc_rx*/
    #define offset_of_chiptop_reg_en_arc_rx (220)
    #define mask_of_chiptop_reg_en_arc_rx (0x4000)
    unsigned int reg_en_arc_rx:1;

    // h006e, bit: 15
    /* */
    unsigned int :1;

    // h006e
    unsigned int /* padding 16 bit */:16;

    // h006f, bit: 14
    /* reg_arc_rx_test*/
    #define offset_of_chiptop_reg_arc_rx_test (222)
    #define mask_of_chiptop_reg_arc_rx_test (0xffff)
    unsigned int reg_arc_rx_test:16;

    // h006f
    unsigned int /* padding 16 bit */:16;

    // h0070, bit: 7
    /* */
    unsigned int :8;

    // h0070, bit: 10
    /* ring OSC output select
    000: select delay chain 0
    001: select delay chain 1
    010: select delay chain 2
    011: select delay chain 3
    100: select delay chain 4
    101: select delay chain 5
    110: select delay chain 6
    111: select delay chain 7*/
    #define offset_of_chiptop_reg_rosc_out_sel (224)
    #define mask_of_chiptop_reg_rosc_out_sel (0x700)
    unsigned int reg_rosc_out_sel:3;

    // h0070, bit: 13
    /* */
    unsigned int :3;

    // h0070, bit: 14
    /* CA TRNG Select the input source of ring oscillator in CHIP_CONF
    1: close-loop (default enable ring oscillator)
    0: open-loop (input from external digital input)*/
    #define offset_of_chiptop_reg_rosc_in_sel_ca (224)
    #define mask_of_chiptop_reg_rosc_in_sel_ca (0x4000)
    unsigned int reg_rosc_in_sel_ca:1;

    // h0070, bit: 15
    /* reg_clk_calc_en
    1:enable
    0:disable*/
    #define offset_of_chiptop_reg_clk_calc_en (224)
    #define mask_of_chiptop_reg_clk_calc_en (0x8000)
    unsigned int reg_clk_calc_en:1;

    // h0070
    unsigned int /* padding 16 bit */:16;

    // h0071, bit: 14
    /* reg_calc_cnt_report*/
    #define offset_of_chiptop_reg_calc_cnt_report (226)
    #define mask_of_chiptop_reg_calc_cnt_report (0xffff)
    unsigned int reg_calc_cnt_report:16;

    // h0071
    unsigned int /* padding 16 bit */:16;

    // h0072, bit: 6
    /* */
    unsigned int :7;

    // h0072, bit: 7
    /* POWERGOOD_AVDD status*/
    #define offset_of_chiptop_reg_powergood_avdd (228)
    #define mask_of_chiptop_reg_powergood_avdd (0x80)
    unsigned int reg_powergood_avdd:1;

    // h0072, bit: 8
    /* MIU clock select DFT clock
    MPLL_SYN clock select DFT clock
    MIU_REC clock select DFT clock
    GE clock select DFT clock*/
    #define offset_of_chiptop_reg_ckg_alldft (228)
    #define mask_of_chiptop_reg_ckg_alldft (0x100)
    unsigned int reg_ckg_alldft:1;

    // h0072, bit: 14
    /* */
    unsigned int :7;

    // h0072
    unsigned int /* padding 16 bit */:16;

    // h0073, bit: 7
    /* */
    unsigned int :8;

    // h0073, bit: 14
    /* Reserved register*/
    #define offset_of_chiptop_reg_reserved1 (230)
    #define mask_of_chiptop_reg_reserved1 (0xffff)
    unsigned int reg_reserved1:8;

    // h0073
    unsigned int /* padding 16 bit */:16;

    // h0074, bit: 14
    /* Reserved register*/
    #define offset_of_chiptop_reg_reserved_1 (232)
    #define mask_of_chiptop_reg_reserved_1 (0xff)
    unsigned int reg_reserved_1:16;

    // h0074
    unsigned int /* padding 16 bit */:16;

    // h0075, bit: 2
    /* select TEST_CLK_OUT source
      000: TEST_CLK_OUT= TEST_BUS_GB[0]
      001: TEST_CLK_OUT= TEST_BUS_GB[1]
      010: TEST_CLK_OUT= TEST_BUS_GB[2]
      011: TEST_CLK_OUT= TEST_BUS_GB[3]
      100: TEST_CLK_OUT= TEST_BUS_GB[4]
      101: TEST_CLK_OUT= TEST_BUS_GB[5]
      110: TEST_CLK_OUT= TEST_BUS_GB[6]
      111: TEST_CLK_OUT= TEST_BUS_GB[7]*/
    #define offset_of_chiptop_reg_clk_out_sel (234)
    #define mask_of_chiptop_reg_clk_out_sel (0x7)
    unsigned int reg_clk_out_sel:3;

    // h0075, bit: 3
    /* swap MSB 12bits with LSB 12bits of test bus*/
    #define offset_of_chiptop_reg_swaptest12bit     (234)
    #define mask_of_chiptop_reg_swaptest12bit     (0x8)
    unsigned int reg_swaptest12bit    :1;

    // h0075, bit: 4
    /* */
    unsigned int :1;

    // h0075, bit: 5
    /* setting for the data arrangement on test bus*/
    #define offset_of_chiptop_reg_test_rg            (234)
    #define mask_of_chiptop_reg_test_rg            (0x20)
    unsigned int reg_test_rg           :1;

    // h0075, bit: 6
    /* setting for the data arrangement on test bus*/
    #define offset_of_chiptop_reg_test_gb              (234)
    #define mask_of_chiptop_reg_test_gb              (0x40)
    unsigned int reg_test_gb             :1;

    // h0075, bit: 7
    /* setting for the data arrangement on test bus*/
    #define offset_of_chiptop_reg_test_rb           (234)
    #define mask_of_chiptop_reg_test_rb           (0x80)
    unsigned int reg_test_rb          :1;

    // h0075, bit: 9
    /* select CLK_TEST_OUT
    2'b00:  select CLK_TEST_OUT[47:0]
    2'b01:  select CLK_TEST_OUT[95:48]
    2'b10:  select CLK_TEST_OUT[143:96]
    2'b11:  reserved*/
    #define offset_of_chiptop_reg_sel_clk_test_out     (234)
    #define mask_of_chiptop_reg_sel_clk_test_out     (0x300)
    unsigned int reg_sel_clk_test_out    :2;

    // h0075, bit: 12
    /* */
    unsigned int :3;

    // h0075, bit: 13
    /* testCLK_mode used in TEST_CTRL*/
    #define offset_of_chiptop_reg_testclk_mode           (234)
    #define mask_of_chiptop_reg_testclk_mode           (0x2000)
    unsigned int reg_testclk_mode          :1;

    // h0075, bit: 14
    /* Enable test bus output*/
    #define offset_of_chiptop_reg_testbus_en   (234)
    #define mask_of_chiptop_reg_testbus_en   (0x4000)
    unsigned int reg_testbus_en  :1;

    // h0075, bit: 15
    /* Select the input source of ring oscillator in CHIP_CONF
    1: close-loop (enable ring oscillator)
    0: open-loop (input from external digital input)*/
    #define offset_of_chiptop_reg_rosc_in_sel (234)
    #define mask_of_chiptop_reg_rosc_in_sel (0x8000)
    unsigned int reg_rosc_in_sel:1;

    // h0075
    unsigned int /* padding 16 bit */:16;

    // h0076, bit: 2
    /* select single CLK_OUT
     001: TEST_BUS[11] = TEST_CLK_OUT_d2.
            TEST_BUS[10] = TEST_CLK_OUT.
     010: TEST_BUS[11] = TEST_CLK_OUT_d2.
            TEST_BUS[10] = TEST_CLK_OUT_d4.
     011: TEST_BUS[11] = TEST_CLK_OUT_d2.
            TEST_BUS[10] = TEST_CLK_OUT_d8.
     100: TEST_BUS[11] = TEST_CLK_OUT_d2.
            TEST_BUS[10] = TEST_CLK_OUT_d16.
     Others: no TEST_CLK_OUT*/
    #define offset_of_chiptop_reg_single_clk_out_sel     (236)
    #define mask_of_chiptop_reg_single_clk_out_sel     (0x7)
    unsigned int reg_single_clk_out_sel    :3;

    // h0076, bit: 14
    /* */
    unsigned int :13;

    // h0076
    unsigned int /* padding 16 bit */:16;

    // h0077, bit: 5
    /* select TEST_BUS[23:0] source
     6'd1: TEST_BUS = LPLL_TEST_OUT
     6'd2: TEST_BUS = CLK_TEST_OUT[23:0] after selecting CLK_TEST_OUT
     6'd3: TEST_BUS = CLK_TEST_OUT[47:24] after selecting CLK_TEST_OUT
     6'd4: TEST_BUS = {ROSC_OUT, CLK_RTC, CLK_TEST_OUT[70:48]}
     6'd5: TEST_BUS = TSP_TEST_OUT
     6'd6: TEST_BUS = MVD_TEST_OUT
     6'd7: TEST_BUS = PM_TEST_OUT
     6'd8: TEST_BUS = VD_TEST_OUT
     6'd9: TEST_BUS = SAR_TEST_OUT
     6'd10: TEST_BUS = AUDIO_TEST_OUT
     6'd11: TEST_BUS = CCFL_TEST_OUT
     6'd12: TEST_BUS = ADCDVI_TEST_OUT
     6'd13: TEST_BUS = SC_TEST_OUT
     6'd14: TEST_BUS = MIU_TEST_OUT
     6'd15: TEST_BUS = MCU_TEST_OUT
     6'd16: TEST_BUS = DC_TEST_OUT
     6'd17: TEST_BUS = GOP_TEST_OUT
     //6'd18: TEST_BUS = SDIO_TEST_OUT
     6'd19: TEST_BUS = UHC_TEST_OUT
     6'd20: TEST_BUS = UTMI_TEST_OUT
     6'd21: TEST_BUS =  M4VE_TEST_OUT
     6'd22: TEST_BUS = GE_TEST_OUT
     6'd23: TEST_BUS = MHEG5_TEST_OUT
     6'd24: TEST_BUS = PIU_TEST_OUT
     6'd25: TEST_BUS = STRLD_TEST_OUT
     6'd26: TEST_BUS = EMAC_TEST_OUT
     6'd27: TEST_BUS = JPD_TEST_OUT
     6'd28: TEST_BUS = VE_TEST_OUT
     6'd29: TEST_BUS = DHC_TEST_OUT
     6'd30: TEST_BUS = NFIE_TEST_OUT
     6'd31: TEST_BUS = MIPS_TEST_OUT
     6'd32: TEST_BUS = MLINK_TEST_OUT
     6'd33: TEST_BUS = MSTAR_LINK_TEST_OUT
     6'd34: TEST_BUS = RVD_TEST_OUT
     6'd35: TEST_BUS = DSCRMB_TEST_OUT
     6'd36: TEST_BUS = UHC1_TEST_OUT
     6'd37: TEST_BUS = HVD_TEST_OUT
     6'd38:  TEST_BUS = MPIF_TEST_OUT
     6'd39:  TEST_BUS = MSP_TEST_OUT
     6'd40:  TEST_BUS = DEMOD_TEST_OUT
     6'd41:  TEST_BUS = ADP_MHEG5_TEST_OUT
     6'd42:  TEST_BUS = VD_MCU_TEST_OUT
     6'd43:  TEST_BUS = PM_PIU_TEST_OUT
     6'd44:  TEST_BUS = DPTX_TEST_OUT
     6'd45:  TEST_BUS = TMDDRLINK_TEST_OUT
     6'd46:  TEST_BUS = VD_MHEG5_TEST_OUT
     6'd47:  TEST_BUS = NR_TEST_OUT
     6'd48:  TEST_BUS = DI_TEST_OUT
     6'd49:  TEST_BUS = AV_LNK_TEST_OUT
     6'd50:  TEST_BUS = DVI_HDMI_HDCP_TEST_OUT
     6'd51:  TEST_BUS = UTMI1_TEST_OUT
     6'd52:  TEST_BUS = G3D_TEST_OUT
     6'd53:  TEST_BUS = MCU_IF_TEST_OUT
     6'd54:  TEST_BUS = MCU_IF_PM_TEST_OUT
     6'd55:  TEST_BUS = DMD_ANA_PLL_TEST_OUT
     6'd60:  TEST_BUS = BIST_TEST_OUT[23:0]
     6'd61:  TEST_BUS = BIST_TEST_OUT[47:24]
     6'd62:  TEST_BUS = {12'h0, BIST_TEST_OUT[63:48]}
     Others: TEST_BUS ={ PIXEL_FUNC[29:22],PIXEL_FUNC[19:12],PIXEL_FUNC[9:2]}*/
    #define offset_of_chiptop_reg_test_bus24b_sel       (238)
    #define mask_of_chiptop_reg_test_bus24b_sel       (0x3f)
    unsigned int reg_test_bus24b_sel      :6;

    // h0077, bit: 14
    /* */
    unsigned int :10;

    // h0077
    unsigned int /* padding 16 bit */:16;

    // h0078, bit: 1
    /* OTP LDO voltage setting:
    00: 2.50v
    01: 2.42v
    10: 2.58v
    11: 2.96v*/
    #define offset_of_chiptop_reg_otp_ldo_vsel (240)
    #define mask_of_chiptop_reg_otp_ldo_vsel (0x3)
    unsigned int reg_otp_ldo_vsel:2;

    // h0078, bit: 5
    /* */
    #define offset_of_chiptop_reg_otp_ldo_trim_vout (240)
    #define mask_of_chiptop_reg_otp_ldo_trim_vout (0x3c)
    unsigned int reg_otp_ldo_trim_vout:4;

    // h0078, bit: 6
    /* */
    #define offset_of_chiptop_reg_otp_ldo_sel_vref (240)
    #define mask_of_chiptop_reg_otp_ldo_sel_vref (0x40)
    unsigned int reg_otp_ldo_sel_vref:1;

    // h0078, bit: 7
    /* */
    #define offset_of_chiptop_reg_otp_ldo_en_tst (240)
    #define mask_of_chiptop_reg_otp_ldo_en_tst (0x80)
    unsigned int reg_otp_ldo_en_tst:1;

    // h0078, bit: 8
    /* */
    #define offset_of_chiptop_reg_otp_ldo_pwrgd (240)
    #define mask_of_chiptop_reg_otp_ldo_pwrgd (0x100)
    unsigned int reg_otp_ldo_pwrgd:1;

    // h0078, bit: 14
    /* */
    unsigned int :6;

    // h0078, bit: 15
    /* */
    #define offset_of_chiptop_reg_otp_ldo_lock (240)
    #define mask_of_chiptop_reg_otp_ldo_lock (0x8000)
    unsigned int reg_otp_ldo_lock:1;

    // h0078
    unsigned int /* padding 16 bit */:16;

    // h0079, bit: 3
    /* Control driving strength*/
    #define offset_of_chiptop_reg_i2cm_out_drv (242)
    #define mask_of_chiptop_reg_i2cm_out_drv (0xf)
    unsigned int reg_i2cm_out_drv:4;

    // h0079, bit: 14
    /* Control driving strength*/
    #define offset_of_chiptop_reg_ts_out_drv (242)
    #define mask_of_chiptop_reg_ts_out_drv (0x7ff0)
    unsigned int reg_ts_out_drv:11;

    // h0079, bit: 15
    /* */
    unsigned int :1;

    // h0079
    unsigned int /* padding 16 bit */:16;

    // h007a, bit: 14
    /* */
    #define offset_of_chiptop_reg_tsp_reserved (244)
    #define mask_of_chiptop_reg_tsp_reserved (0xffff)
    unsigned int reg_tsp_reserved:16;

    // h007a
    unsigned int /* padding 16 bit */:16;

    // h007b, bit: 14
    /* */
    #define offset_of_chiptop_reg_chiptop_reserved (246)
    #define mask_of_chiptop_reg_chiptop_reserved (0xffff)
    unsigned int reg_chiptop_reserved:16;

    // h007b
    unsigned int /* padding 16 bit */:16;

    // h007c, bit: 14
    /* */
    #define offset_of_chiptop_reg_chk_clk_mips_freq_cmp_data (248)
    #define mask_of_chiptop_reg_chk_clk_mips_freq_cmp_data (0xffff)
    unsigned int reg_chk_clk_mips_freq_cmp_data:16;

    // h007c
    unsigned int /* padding 16 bit */:16;

    // h007d, bit: 2
    /* */
    #define offset_of_chiptop_reg_vsense_chsel (250)
    #define mask_of_chiptop_reg_vsense_chsel (0x7)
    unsigned int reg_vsense_chsel:3;

    // h007d, bit: 14
    /* */
    unsigned int :13;

    // h007d
    unsigned int /* padding 16 bit */:16;

    // h007e, bit: 1
    /* */
    #define offset_of_chiptop_reg_virage_2pw (252)
    #define mask_of_chiptop_reg_virage_2pw (0x3)
    unsigned int reg_virage_2pw:2;

    // h007e, bit: 3
    /* */
    #define offset_of_chiptop_reg_virage_2pr (252)
    #define mask_of_chiptop_reg_virage_2pr (0xc)
    unsigned int reg_virage_2pr:2;

    // h007e, bit: 5
    /* */
    #define offset_of_chiptop_reg_virage_1phs (252)
    #define mask_of_chiptop_reg_virage_1phs (0x30)
    unsigned int reg_virage_1phs:2;

    // h007e, bit: 7
    /* */
    #define offset_of_chiptop_reg_virage_1phd (252)
    #define mask_of_chiptop_reg_virage_1phd (0xc0)
    unsigned int reg_virage_1phd:2;

    // h007e, bit: 9
    /* */
    #define offset_of_chiptop_reg_virage_1prf (252)
    #define mask_of_chiptop_reg_virage_1prf (0x300)
    unsigned int reg_virage_1prf:2;

    // h007e, bit: 14
    /* */
    unsigned int :6;

    // h007e
    unsigned int /* padding 16 bit */:16;

    // h007f, bit: 1
    /* */
    #define offset_of_chiptop_reg_mstar_2pw (254)
    #define mask_of_chiptop_reg_mstar_2pw (0x3)
    unsigned int reg_mstar_2pw:2;

    // h007f, bit: 3
    /* */
    #define offset_of_chiptop_reg_mstar_2pr (254)
    #define mask_of_chiptop_reg_mstar_2pr (0xc)
    unsigned int reg_mstar_2pr:2;

    // h007f, bit: 5
    /* */
    #define offset_of_chiptop_reg_mstar_1phs (254)
    #define mask_of_chiptop_reg_mstar_1phs (0x30)
    unsigned int reg_mstar_1phs:2;

    // h007f, bit: 7
    /* */
    #define offset_of_chiptop_reg_mstar_1pamg (254)
    #define mask_of_chiptop_reg_mstar_1pamg (0xc0)
    unsigned int reg_mstar_1pamg:2;

    // h007f, bit: 9
    /* */
    #define offset_of_chiptop_reg_mstar_1phd (254)
    #define mask_of_chiptop_reg_mstar_1phd (0x300)
    unsigned int reg_mstar_1phd:2;

    // h007f, bit: 11
    /* */
    #define offset_of_chiptop_reg_mstar_1prf (254)
    #define mask_of_chiptop_reg_mstar_1prf (0xc00)
    unsigned int reg_mstar_1prf:2;

    // h007f, bit: 14
    /* */
    unsigned int :4;

    // h007f
    unsigned int /* padding 16 bit */:16;
}  __attribute__((packed, aligned(1))) CHIPTOP_t;

extern volatile CHIPTOP_t * const g_ptCHIPTOP;

#endif
