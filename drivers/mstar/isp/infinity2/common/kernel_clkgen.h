/***************************************************************************
 *  kernel_g_ptBbTop.h
 *--------------------------------------------------------------------------
 *  Scope: BB_TOP related definitions
 *
 ****************************************************************************/
#ifndef __KERNEL_BBTOP_H__
#define __KERNEL_BBTOP_H__


/****************************************************************************/
/*        BB_TOP registers                                                  */
/****************************************************************************/
typedef struct Clkgen_s
 {
    /* [0x00] */
    u32 reg_ckg_xtali                 :4; /// xtali clock setting(only for HEMCU)
    u32 reg_ckg_xtali_sc_gp           :4; /// xtali clock setting
    u32 reg_ckg_live                  :4; /// live clock setting
    u32 :20; /// [0x00]
    /* [0x01] */
    u32 reg_ckg_mcu                   :5; /// clk_mcu clock setting
    u32 :3;
    u32 reg_ckg_riubrdg               :4; /// clk_riubrdg clock setting
    u32 :20; /// [0x01]
    /* [0x02] */
    u32 reg_ckg_bist                  :4; /// clk_bist_p clock setting
    u32 :4;
    u32 reg_ckg_bist_pm               :4; /// clk_bist_pm_p clock setting
    u32 :20; /// [0x02]
    /* [0x03] */
    u32 reg_ckg_bist_sc_gp            :4; /// clk_bist_sc_gp_p clock setting
    u32 :4 ;
    u32 reg_ckg_bist_vhe_gp           :4; /// clk_bist_vhe_gp_p clock setting
    u32 :20; /// [0x03]
    u32 :32; /// [0x04]
    u32 :32; /// [0x05]
    u32 :32; /// [0x06]
    u32 :32; /// [0x07]
    /* [0x08] */
    u32 reg_ckg_boot                  :4; /// clk_boot_p clock setting
    u32 :28; /// [0x08]
    u32 :32; /// [0x09]
    u32 :32; /// [0x0A]
    u32 :32; /// [0x0B]
    u32 :32; /// [0x0C]
    u32 :32; /// [0x0D]
    u32 :32; /// [0x0E]
    u32 :32; /// [0x0F]
    /* [0x10] */
    u32 reg_ckg_mpll_syn              :4; /// clk_mpll_syn clock setting
    u32 :4;
    u32 reg_ckg_mpll_syn2             :4; /// clk_mpll_syn2 clock setting
    u32 :20; /// [0x10]
    u32 :32; /// [0x11]
    u32 :32; /// [0x12]
    u32 :32; /// [0x13]
    u32 :32; /// [0x14]
    u32 :32; /// [0x15]
    u32 :32; /// [0x16]
    /* [0x17] */
    u32 reg_ckg_miu                   :5; /// clk_miu clock setting
    u32 :3;
    u32 reg_ckg_miu_256bus            :5; /// clk_miu_256 clock setting
    u32 :19; /// [0x17]
    /* [0x18] */
    u32 reg_ckg_miu_rec               :4; /// clk_miu_rec clock setting
    u32 :28; /// [0x18]
    /* [0x19] */
    u32 reg_ckg_ddr_syn               :4; /// clk_ddr_syn clock setting
    u32 :28; /// [0x19]
    u32 :32; /// [0x1A]
    u32 :32; /// [0x1B]
    u32 :32; /// [0x1C]
    u32 :32; /// [0x1D]
    u32 :32; /// [0x1E]
    u32 :32; /// [0x1F]
    /* [0x20] */
    u32 reg_ckg_miu_boot              :3 ; /// [0]:reserved
                                           /// [1]:reserved
                                           /// [2]: select clock source
                                           ///  0: select BOOT clock 12MHz (xtali)
                                           ///  1: select the output according to reg_ckg_miu[4:0]
    u32 :32; /// [0x21]
    u32 :32; /// [0x22]
    u32 :32; /// [0x23]
    u32 :32; /// [0x24]
    u32 :32; /// [0x25]
    u32 :32; /// [0x26]
    u32 :32; /// [0x27]
    u32 :32; /// [0x28]
    u32 :32; /// [0x29]
    u32 :32; /// [0x2A]
    u32 :32; /// [0x2B]
    u32 :32; /// [0x2C]
    u32 :32; /// [0x2D]
    u32 :32; /// [0x2E]
    u32 :32; /// [0x2F]
    /* [0x30] */
    u32 reg_ckg_tck                   :4; ///clk_tck clock setting
    u32 :28; /// [0x30]
    /* [0x31] */
    u32 reg_ckg_uart0                 :4; /// clk_uart0 clock setting
    u32 :4;
    u32 reg_ckg_uart1                 :4; /// clk_uart1 clock setting
    u32 :20; /// [0x31]
    /* [0x32] */
    u32 reg_ckg_spi                   :5; /// clk_spi clock setting
    u32 :27; /// [0x32]
    /* [0x33] */
    u32 reg_ckg_mspi0                 :4; /// clk_mspi0 clock setting
    u32 :4;
    u32 reg_ckg_mspi1                 :4; /// clk_mspi1 clock setting
    u32 :20; /// [0x33]
    /* [0x34] */
    u32 reg_ckg_fuart                 :4; /// clk_fast_uart clock setting
    u32 reg_ckg_fuart0_synth_in       :4; /// clk_fast_uart_syn clock setting (modem)
    u32 reg_uart_stnthesizer_enable   :1; /// FAST UART enable signal
    u32 reg_uart_stnthesizer_sw_rstz  :1; /// FAST UART SW resetz
    u32 :22; /// [0x34]
    /* [0x35-0x36] */ ///reg_uart_stnthesizer_fix_nf_freq
    u32 :32; /// [0x35]
    u32 :32; /// [0x36]
    /* [0x37] */
    u32 reg_ckg_miic0                 :4; /// clk_miic0 clock setting
    u32 :4;
    u32 reg_ckg_miic1                 :4; /// clk_miic1 clock setting
    u32 :20; /// [0x37]
    u32 :32; /// [0x38]
    u32 :32; /// [0x39]
    u32 :32; /// [0x3A]
    u32 :32; /// [0x3B]
    u32 :32; /// [0x3C]
    u32 :32; /// [0x3D]
    u32 :32; /// [0x3E]
    u32 :32; /// [0x3F]
    /* [0x40] */
    u32 reg_ckg_emac_rx               :4; /// clk_emac_rx clock setting
    u32 :4;
    u32 reg_ckg_emac_rx_ref           :4; /// clk_emac_rx_ref clock setting
    u32 :20; /// [0x40]
    /* [0x41] */
    u32 reg_ckg_emac_tx               :4; /// clk_emac_tx clock setting
    u32 :4;
    u32 reg_ckg_emac_tx_ref           :4; /// clk_emac_tx_ref clock setting
    u32 :20; /// [0x41]
    /* [0x42] */
    u32 reg_ckg_emac_ahb              :4 ; /// clk_emac_ahb clock setting
    u32 :28; /// [0x42]
    /* [0x43] */
    u32 reg_ckg_fcie                  :8 ; /// clk_fcie clock setting
    u32 :24; /// [0x43]
    /* [0x44] */
    u32 reg_ckg_ecc                   :4 ; /// clk_ecc clock setting
    u32 :28; /// [0x44]
    /* [0x45] */
    u32 reg_ckg_sdio                  :5 ; /// clk_sdio clock setting
    u32 :27; /// [0x45]
    u32 :32; /// [0x46]
    u32 :32; /// [0x47]
    u32 :32; /// [0x48]
    u32 :32; /// [0x49]
    u32 :32; /// [0x4A]
    u32 :32; /// [0x4B]
    u32 :32; /// [0x4C]
    u32 :32; /// [0x4D]
    u32 :32; /// [0x4E]
    u32 :32; /// [0x4F]
    u32 :32; /// [0x50]
    u32 :32; /// [0x51]
    u32 :32; /// [0x52]
    u32 :32; /// [0x53]
    u32 :32; /// [0x54]
    u32 :32; /// [0x55]
    u32 :32; /// [0x56]
    u32 :32; /// [0x57]
    u32 :32; /// [0x58]
    u32 :32; /// [0x59]
    u32 :32; /// [0x5A]
    u32 :32; /// [0x5B]
    u32 :32; /// [0x5C]
    u32 :32; /// [0x5D]
    u32 :32; /// [0x5E]
    u32 :32; /// [0x5F]
    /* [0x60] */
    u32 reg_ckg_bdma                  :5 ; /// clk_bdma clock setting
    u32 :27; /// [0x60]
    /* [0x61] */
    u32 reg_ckg_aesdma                :5 ; /// clk_aesdma clock setting
    u32 :27; /// [0x61]
    /* [0x62] */
    u32 reg_ckg_sr                    :4 ; /// clk_sr clock setting (sensor pixel clock)
    u32 :4;
    u32 reg_ckg_sr_mclk               :4 ; /// clk_sr_mclk clock setting (sensor main clock to ISP)
    u32 :20; /// [0x62]
    /* [0x63] */
    u32 reg_ckg_idclk                 :4 ; /// clk_idclk clock setting
    u32 :28; /// [0x63]
    /* [0x64] */
    u32 reg_ckg_fclk1                 :4 ; /// clk_fclk1 clock setting (only for SC1 & SC2)
    u32 :28; /// [0x64]
    /* [0x65] */
    u32 reg_ckg_fclk2                 :4 ; /// clk_fclk2 clock setting (only for SC3)
    u32 :28; /// [0x65]
    /* [0x66] */
    u32 reg_ckg_odclk                 :4; /// clk_odclk clock setting
    u32 :28; /// [0x66]
    u32 :32; /// [0x67]
    /* [0x68] */
    u32 reg_ckg_vhe                   :5; /// clk_vhe clock setting
    u32 :27; /// [0x68]
    /* [0x69] */
    u32 reg_ckg_mfe                   :5; /// clk_mfe clock setting
    u32 :27; /// [0x69]
    /* [0x6A] */
    u32 reg_ckg_jpe                   :4; /// clk_jpe clock setting
    u32 :4;
    u32 reg_ckg_ive                   :4; /// clk_ive clock setting
    u32 :20; /// [0x6A]
    /* [0x6B] */
    u32 reg_ckg_ns                    :4; /// clk_ns_p clock setting
    u32 :28; /// [0x6B]
    /* [0x6C] */
    u32 reg_ckg_csi_mac               :5; /// clk_csi_mac_p clock setting
    u32 :3;
    u32 reg_ckg_mac_lptx              :5; ///clk_mac_lptx_p clock setting
    u32 :19; /// [0x6C]
    /* [0x6D] */
    u32 reg_ckg_hemcu_216m            :1; /// clk_hemcu_216m_p clock setting
    u32 reg_ckg_216m_2digpm           :1; /// 216m clock to digpm setting, default on
    u32 reg_ckg_172m_2digpm           :1; /// 172m clock to digpm setting, default off
    u32 reg_ckg_144m_2digpm           :1; /// 144m clock to digpm setting, default off
    u32 reg_ckg_123m_2digpm           :1; /// 123m clock to digpm setting, default off
    u32 reg_ckg_86m_2digpm            :1; /// 86m clock to digpm setting, default off
    u32 :26; /// [0x6D]
    u32 :32; /// [0x6E]
    u32 :32; /// [0x6F]
    /* [0x70] */
    u32 reg_pll_gater_force_on_lock   :1; /// one way register to lock all force on registers
    u32 reg_pll_gater_force_off_lock  :1; /// one way register to lock all force off registers
    u32 :30; /// [0x70]
    /* [0x71] */
    u32 reg_upll_384_force_on         :1;
    u32 reg_upll_320_force_on         :1;
    u32 reg_utmi_160_force_on         :1;
    u32 reg_utmi_192_force_on         :1;
    u32 reg_utmi_240_force_on         :1;
    u32 reg_utmi_480_force_on         :1;
    u32 reg_mpll_432_force_on         :1;
    u32 reg_mpll_345_force_on         :1;
    u32 reg_mpll_288_force_on         :1;
    u32 reg_mpll_216_force_on         :1;
    u32 reg_mpll_172_force_on         :1;
    u32 reg_mpll_144_force_on         :1;
    u32 reg_mpll_123_force_on         :1;
    u32 reg_mpll_124_force_on         :1;
    u32 reg_mpll_86_force_on          :1;
    u32 reg_pll_rv1_force_on          :1;
    u32 :16; /// [0x71]
    /* [0x72] */
    u32 reg_upll_384_force_off        :1;
    u32 reg_upll_320_force_off        :1;
    u32 reg_utmi_160_force_off        :1;
    u32 reg_utmi_192_force_off        :1;
    u32 reg_utmi_240_force_off        :1;
    u32 reg_utmi_480_force_off        :1;
    u32 reg_mpll_432_force_off        :1;
    u32 reg_mpll_345_force_off        :1;
    u32 reg_mpll_288_force_off        :1;
    u32 reg_mpll_216_force_off        :1;
    u32 reg_mpll_172_force_off        :1;
    u32 reg_mpll_144_force_off        :1;
    u32 reg_mpll_123_force_off        :1;
    u32 reg_mpll_124_force_off        :1;
    u32 reg_mpll_86_force_off         :1;
    u32 reg_pll_rv1_force_off         :1;
    u32 :16; /// [0x72]
    /* [0x73] */
    u32 reg_upll_384_force_rd         :1;
    u32 reg_upll_320_force_rd         :1;
    u32 reg_utmi_160_force_rd         :1;
    u32 reg_utmi_192_force_rd         :1;
    u32 reg_utmi_240_force_rd         :1;
    u32 reg_utmi_480_force_rd         :1;
    u32 reg_mpll_432_force_rd         :1;
    u32 reg_mpll_345_force_rd         :1;
    u32 reg_mpll_288_force_rd         :1;
    u32 reg_mpll_216_force_rd         :1;
    u32 reg_mpll_172_force_rd         :1;
    u32 reg_mpll_144_force_rd         :1;
    u32 reg_mpll_123_force_rd         :1;
    u32 reg_mpll_124_force_rd         :1;
    u32 reg_mpll_86_force_rd          :1;
    u32 reg_pll_rv1_force_rd          :1;
    u32 :16; /// [0x73]
} Clkgen_t;

extern volatile Clkgen_t* const g_ptClkgenTop;

#endif // __KERNEL_BBTOP_H__
