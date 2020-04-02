#ifndef _MHAL_CLKM_REG_H
#define _MHAL_CLKM_REG_H



#define CLKGEN0 0x100B
#define CLKGEN2 0x100A


#define CLK_GATE_REV 1
#define CLK_GATE_NORMAL 0
#define	NO_GATE_REG	2
#define	HAS_GATE_REG	1
#define NO_SEL_REG  0
#define HAS_SEL_REG 1

#define g_clk_ge_num 0x4

char *g_clk_ge_ptr[] =
{
   "CLK_FASTEST",
   "CLK_FAST",
   "CLK_MID",
   "CLK_SLOW",
};


#define g_clk_gpd_num 0x8

char *g_clk_gpd_ptr[] =
{
   "Performance_mode",
   "middle_mode",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_njpd_num 0x4

char *g_clk_njpd_ptr[] =
{
   "720p60fps_mode",
   "1080p30fps_mode",
   "720p40fps_mode",
   "720p30fps_mode",
};


#define g_clk_mfe_num 0x4

char *g_clk_mfe_ptr[] =
{
   "Debug_mode",
   "SDp30_mode",
   "720p30_mode",
   "720p60_1080p30_mode",
};


#define g_clk_mvd_num 0x8

char *g_clk_mvd_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_mvd_core_num 0x1

char *g_clk_mvd_core_ptr[] =
{
   "",
};


#define g_clk_mvd_pas_num 0x1

char *g_clk_mvd_pas_ptr[] =
{
   "",
};


#define g_clk_zdec_vld_num 0x4

char *g_clk_zdec_vld_ptr[] =
{
   "fast_mode",
   "medium_mode",
   "slow_mode",
   "very_slow_mode",
};


#define g_clk_zdec_lzd_num 0x4

char *g_clk_zdec_lzd_ptr[] =
{
   "fast_mode",
   "medium_mode",
   "slow_mode",
   "very_slow_mode",
};


#define g_clk_vd_mheg5_num 0x1

char *g_clk_vd_mheg5_ptr[] =
{
   "",
};


#define g_clk_sdio_p_num 0x10

char *g_clk_sdio_p_ptr[] =
{
   "g_sdio_clk_xtal",
   "g_sdio_clk_20",
   "g_sdio_clk_32",
   "g_sdio_clk_36",
   "g_sdio_clk_40",
   "g_sdio_clk_43",
   "g_sdio_clk_54",
   "g_sdio_clk_62",
   "g_sdio_clk_72",
   "g_sdio_clk_86",
   "g_sdio_clk_5p4",
   "g_sdio_clk_1x_p",
   "REV",
   "g_sdio_clk_300k",
   "g_sdio_clk_xtal_24",
   "g_sdio_clk_48",
};


#define g_clk_sdio_num 0x2

char *g_clk_sdio_ptr[] =
{
   "g_sdio_clk_xtal",
   "g_sdio_clk_p",
};


#define g_clk_nfie_p_num 0x10

char *g_clk_nfie_p_ptr[] =
{
   "g_nfie_clk_xtal",
   "g_nfie_clk_20",
   "g_nfie_clk_32",
   "g_nfie_clk_36",
   "g_nfie_clk_40",
   "g_nfie_clk_43",
   "g_nfie_clk_54",
   "g_nfie_clk62",
   "g_nfie_clk_72",
   "g_nfie_clk_86",
   "g_nfie_clk_5p4",
   "g_nfie_clk_1x_p",
   "g_nfie_clk_2x_p",
   "g_nfie_clk_300k",
   "g_nfie_clk_xtal_24",
   "g_nfie_clk_48",
};


#define g_clk_nfie_num 0x2

char *g_clk_nfie_ptr[] =
{
   "g_nfie_clk_xtal",
   "g_nfie_clk_p",
};


#define g_clk_ecc_num 0x8

char *g_clk_ecc_ptr[] =
{
   "g_ecc_clk_xtal",
   "g_ecc_clk_54",
   "g_ecc_clk_108",
   "g_ecc_clk_160",
   "g_ecc_clk_216",
   "REV",
   "REV",
   "REV",
};


#define g_clk_gamc_ahb_num 0x8

char *g_clk_gamc_ahb_ptr[] =
{
   "REV",
   "REV",
   "REV",
   "clk_gmac_ahb",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_secgamc_ahb_num 0x8

char *g_clk_secgamc_ahb_ptr[] =
{
   "REV",
   "REV",
   "REV",
   "clk_secgmac_ahb",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_gamc_rx_num 0x4

char *g_clk_gamc_rx_ptr[] =
{
   "clk_gmac_rx_rgmii",
   "clk_gmac_rx_rmii",
   "REV",
   "REV",
};


#define g_clk_gamc_tx_num 0x4

char *g_clk_gamc_tx_ptr[] =
{
   "clk_gmac_tx_rgmii",
   "clk_gmac_tx_rmii",
   "REV",
   "REV",
};


#define g_clk_gamc_rx_ref_num 0x2

char *g_clk_gamc_rx_ref_ptr[] =
{
   "clk_gmac_rx_ref",
   "REV",
};


#define g_clk_gamc_tx_ref_num 0x2

char *g_clk_gamc_tx_ref_ptr[] =
{
   "clk_gmac_tx_ref",
   "REV",
};


#define g_clk_secgamc_rx_num 0x4

char *g_clk_secgamc_rx_ptr[] =
{
   "clk_secgmac_rx_rgmii",
   "clk_secgmac_rx_rmii",
   "clk_secgmac_rx_ephy",
   "REV",
};


#define g_clk_secgamc_tx_num 0x4

char *g_clk_secgamc_tx_ptr[] =
{
   "clk_secgmac_tx_rgmii",
   "clk_secgmac_tx_rmii",
   "clk_secgmac_tx_ephy",
   "REV",
};


#define g_clk_secgamc_rx_ref_num 0x2

char *g_clk_secgamc_rx_ref_ptr[] =
{
   "clk_secgmac_rx_ref",
   "REV",
};


#define g_clk_secgamc_tx_ref_num 0x2

char *g_clk_secgamc_tx_ref_ptr[] =
{
   "clk_secgmac_tx_ref",
   "REV",
};


#define g_clk_usb20_p0_240m_num 0x1

char *g_clk_usb20_p0_240m_ptr[] =
{
   "",
};


#define g_clk_usb20_p1_240m_num 0x1

char *g_clk_usb20_p1_240m_ptr[] =
{
   "",
};


#define g_clk_usb20_p2_240m_num 0x1

char *g_clk_usb20_p2_240m_ptr[] =
{
   "",
};


#define g_clk_evd_num 0x8

char *g_clk_evd_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_evd_ppu_num 0x8

char *g_clk_evd_ppu_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_hvd_idb_num 0x4

char *g_clk_hvd_idb_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
};


#define g_clk_hvd_num 0x4

char *g_clk_hvd_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
};


#define g_clk_hvd_aec_num 0x4

char *g_clk_hvd_aec_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
};


#define g_clk_vp8_num 0x4

char *g_clk_vp8_ptr[] =
{
   "FAST_MODE",
   "REV",
   "REV",
   "REV",
};


#define g_clk_tsp_num 0x8

char *g_clk_tsp_ptr[] =
{
   "CLK_TSP_FAST",
   "CLK_TSP_MID",
   "CLK_TSP_SLOW",
   "CLK_TSP_DBG",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_parser_num 0x4

char *g_clk_parser_ptr[] =
{
   "CLK_PARSER_NORMAL",
   "CLK_PARSER_FAST",
   "REV",
   "REV",
};


#define g_clk_stamp_num 0x4

char *g_clk_stamp_ptr[] =
{
   "CLK_STAMP_NORMAL",
   "REV",
   "REV",
   "REV",
};


#define g_clk_ts_num 0x10

char *g_clk_ts_ptr[] =
{
   "CLK_TS0_PAD0",
   "CLK_TS0_PAD1",
   "CLK_TS0_PAD2",
   "CLK_TS0_PAD3",
   "CLK_TS0_PAD4",
   "CLK_TS0_PAD5",
   "CLK_TS0_PAD6",
   "CLK_TS0_PADTSO",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_ts1_num 0x10

char *g_clk_ts1_ptr[] =
{
   "CLK_TS0_PAD0",
   "CLK_TS0_PAD1",
   "CLK_TS0_PAD2",
   "CLK_TS0_PAD3",
   "CLK_TS0_PAD4",
   "CLK_TS0_PAD5",
   "CLK_TS0_PAD6",
   "CLK_TS0_PADTSO",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_ts2_num 0x10

char *g_clk_ts2_ptr[] =
{
   "CLK_TS0_PAD0",
   "CLK_TS0_PAD1",
   "CLK_TS0_PAD2",
   "CLK_TS0_PAD3",
   "CLK_TS0_PAD4",
   "CLK_TS0_PAD5",
   "CLK_TS0_PAD6",
   "CLK_TS0_PADTSO",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_ts3_num 0x10

char *g_clk_ts3_ptr[] =
{
   "CLK_TS0_PAD0",
   "CLK_TS0_PAD1",
   "CLK_TS0_PAD2",
   "CLK_TS0_PAD3",
   "CLK_TS0_PAD4",
   "CLK_TS0_PAD5",
   "CLK_TS0_PAD6",
   "CLK_TS0_PADTSO",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define g_clk_stc0_num 0x4

char *g_clk_stc0_ptr[] =
{
   "CLK_STC0_BUF",
   "CLK_STC0_1",
   "CLK_STC0_27MBUF",
   "CLK_STC0_DEBUG",
};


#define g_clk_stc1_num 0x4

char *g_clk_stc1_ptr[] =
{
   "CLK_STC0_BUF",
   "CLK_STC0_1",
   "CLK_STC0_27MBUF",
   "CLK_STC0_DEBUG",
};


#define g_clk_vedac_num 0x8

char *g_clk_vedac_ptr[] =
{
   "REV",
   "REV",
   "clk_vedac",
   "REV",
   "REV",
   "REV",
   "REV",
   "REV",
};


#define CLKM_TABLE_NUM	44




typedef struct _clock_table
{

	char *g_port_name;;
	S32	has_clk_gate;
U32 clk_gate_bank;
	U32 clk_gate_reg;
	U16 clk_gate_offset;
	S32 clk_gate_rev;
	S32 has_clk_sel;
	U32 clk_sel_bank;
	U32 clk_sel_reg;
	U16 clksel_offset_start;
	U16 clksel_offset_end;
	U32 src_num;
	S32 handle;
	char **g_src_name_ptr;

}clock_table;


typedef struct _clock_table_param
{
	clock_table* clock_table_array_ref;
	s32 num;
}clock_table_param;


clock_table clock_table_array[CLKM_TABLE_NUM] =
{

   { "g_clk_ge" , HAS_GATE_REG,	0x100B,	0x048 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x048,	2,	3,	g_clk_ge_num,	0,	g_clk_ge_ptr },

   { "g_clk_gpd" , HAS_GATE_REG,	0x100B,	0x03e ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x03e,	2,	4,	g_clk_gpd_num,	1,	g_clk_gpd_ptr },

   { "g_clk_njpd" , HAS_GATE_REG,	0x100A,	0x011 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x011,	2,	3,	g_clk_njpd_num,	2,	g_clk_njpd_ptr },

   { "g_clk_mfe" , HAS_GATE_REG,	0x100A,	0x010 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x010,	2,	3,	g_clk_mfe_num,	3,	g_clk_mfe_ptr },

   { "g_clk_mvd" , HAS_GATE_REG,	0x100B,	0x039 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x039,	2,	4,	g_clk_mvd_num,	4,	g_clk_mvd_ptr },

   { "g_clk_mvd_core" , HAS_GATE_REG,	0x100B,	0x037 ,	0,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_mvd_core_num,	5,	g_clk_mvd_core_ptr },

   { "g_clk_mvd_pas" , HAS_GATE_REG,	0x100B,	0x037 ,	1,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_mvd_pas_num,	6,	g_clk_mvd_pas_ptr },

   { "g_clk_zdec_vld" , HAS_GATE_REG,	0x100A,	0x013 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x013,	2,	3,	g_clk_zdec_vld_num,	7,	g_clk_zdec_vld_ptr },

   { "g_clk_zdec_lzd" , HAS_GATE_REG,	0x100A,	0x014 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x014,	2,	3,	g_clk_zdec_lzd_num,	8,	g_clk_zdec_lzd_ptr },

   { "g_clk_vd_mheg5" , HAS_GATE_REG,	0x100B,	0x030 ,	8,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_vd_mheg5_num,	9,	g_clk_vd_mheg5_ptr },

   { "g_clk_sdio_p" , HAS_GATE_REG,	0x100A,	0x03e ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x03e,	2,	5,	g_clk_sdio_p_num,	10,	g_clk_sdio_p_ptr },

   { "g_clk_sdio" , NO_GATE_REG,	0,	0,	0,	0,HAS_SEL_REG,	0x100A,	0x03e,	6,	6,	g_clk_sdio_num,	11,	g_clk_sdio_ptr },

   { "g_clk_nfie_p" , HAS_GATE_REG,	0x100B,	0x064 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x064,	2,	5,	g_clk_nfie_p_num,	12,	g_clk_nfie_p_ptr },

   { "g_clk_nfie" , NO_GATE_REG,	0,	0,	0,	0,HAS_SEL_REG,	0x100B,	0x064,	6,	6,	g_clk_nfie_num,	13,	g_clk_nfie_ptr },

   { "g_clk_ecc" , HAS_GATE_REG,	0x100A,	0x001 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x001,	2,	4,	g_clk_ecc_num,	14,	g_clk_ecc_ptr },

   { "g_clk_gamc_ahb" , HAS_GATE_REG,	0x100A,	0x001 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x001,	10,	12,	g_clk_gamc_ahb_num,	15,	g_clk_gamc_ahb_ptr },

   { "g_clk_secgamc_ahb" , HAS_GATE_REG,	0x1033,	0x032 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x1033,	0x032,	2,	4,	g_clk_secgamc_ahb_num,	16,	g_clk_secgamc_ahb_ptr },

   { "g_clk_gamc_rx" , HAS_GATE_REG,	0x121F,	0x012 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x121F,	0x012,	2,	3,	g_clk_gamc_rx_num,	17,	g_clk_gamc_rx_ptr },

   { "g_clk_gamc_tx" , HAS_GATE_REG,	0x121F,	0x011 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x121F,	0x011,	10,	11,	g_clk_gamc_tx_num,	18,	g_clk_gamc_tx_ptr },

   { "g_clk_gamc_rx_ref" , HAS_GATE_REG,	0x121F,	0x012 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x121F,	0x012,	10,	10,	g_clk_gamc_rx_ref_num,	19,	g_clk_gamc_rx_ref_ptr },

   { "g_clk_gamc_tx_ref" , HAS_GATE_REG,	0x121F,	0x012 ,	4,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x121F,	0x012,	6,	6,	g_clk_gamc_tx_ref_num,	20,	g_clk_gamc_tx_ref_ptr },

   { "g_clk_secgamc_rx" , HAS_GATE_REG,	0x1224,	0x012 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x1224,	0x012,	2,	3,	g_clk_secgamc_rx_num,	21,	g_clk_secgamc_rx_ptr },

   { "g_clk_secgamc_tx" , HAS_GATE_REG,	0x1224,	0x011 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x1224,	0x011,	10,	11,	g_clk_secgamc_tx_num,	22,	g_clk_secgamc_tx_ptr },

   { "g_clk_secgamc_rx_ref" , HAS_GATE_REG,	0x1224,	0x012 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x1224,	0x012,	10,	10,	g_clk_secgamc_rx_ref_num,	23,	g_clk_secgamc_rx_ref_ptr },

   { "g_clk_secgamc_tx_ref" , HAS_GATE_REG,	0x1224,	0x012 ,	4,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x1224,	0x012,	6,	6,	g_clk_secgamc_tx_ref_num,	24,	g_clk_secgamc_tx_ref_ptr },

   { "g_clk_usb20_p0_240m" , HAS_GATE_REG,	0x103A,	0x044 ,	1,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_usb20_p0_240m_num,	25,	g_clk_usb20_p0_240m_ptr },

   { "g_clk_usb20_p1_240m" , HAS_GATE_REG,	0x103A,	0x004 ,	1,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_usb20_p1_240m_num,	26,	g_clk_usb20_p1_240m_ptr },

   { "g_clk_usb20_p2_240m" , HAS_GATE_REG,	0x1039,	0x004 ,	1,	CLK_GATE_REV,	NO_SEL_REG,	0,	0,	0,	0,g_clk_usb20_p2_240m_num,	27,	g_clk_usb20_p2_240m_ptr },

   { "g_clk_evd" , HAS_GATE_REG,	0x100B,	0x03d ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x03d,	2,	4,	g_clk_evd_num,	28,	g_clk_evd_ptr },

   { "g_clk_evd_ppu" , HAS_GATE_REG,	0x100A,	0x01c ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x01c,	2,	4,	g_clk_evd_ppu_num,	29,	g_clk_evd_ppu_ptr },

   { "g_clk_hvd_idb" , HAS_GATE_REG,	0x100B,	0x034 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x01a,	0,	2,	g_clk_hvd_idb_num,	30,	g_clk_hvd_idb_ptr },

   { "g_clk_hvd" , HAS_GATE_REG,	0x100B,	0x034 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x034,	2,	4,	g_clk_hvd_num,	31,	g_clk_hvd_ptr },

   { "g_clk_hvd_aec" , HAS_GATE_REG,	0x100A,	0x01b ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x01b,	2,	3,	g_clk_hvd_aec_num,	32,	g_clk_hvd_aec_ptr },

   { "g_clk_vp8" , HAS_GATE_REG,	0x100A,	0x01d ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100A,	0x01d,	2,	3,	g_clk_vp8_num,	33,	g_clk_vp8_ptr },

   { "g_clk_tsp" , HAS_GATE_REG,	0x100B,	0x02a ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x02a,	2,	4,	g_clk_tsp_num,	34,	g_clk_tsp_ptr },

   { "g_clk_parser" , HAS_GATE_REG,	0x100B,	0x039 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x039,	10,	11,	g_clk_parser_num,	35,	g_clk_parser_ptr },

   { "g_clk_stamp" , HAS_GATE_REG,	0x100B,	0x02f ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x02f,	10,	11,	g_clk_stamp_num,	36,	g_clk_stamp_ptr },

   { "g_clk_ts" , HAS_GATE_REG,	0x100B,	0x028 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x028,	2,	5,	g_clk_ts_num,	37,	g_clk_ts_ptr },

   { "g_clk_ts1" , HAS_GATE_REG,	0x100B,	0x028 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x028,	10,	13,	g_clk_ts1_num,	38,	g_clk_ts1_ptr },

   { "g_clk_ts2" , HAS_GATE_REG,	0x100B,	0x029 ,	0,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x029,	2,	5,	g_clk_ts2_num,	39,	g_clk_ts2_ptr },

   { "g_clk_ts3" , HAS_GATE_REG,	0x100B,	0x029 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x029,	10,	13,	g_clk_ts3_num,	40,	g_clk_ts3_ptr },

   { "g_clk_stc0" , HAS_GATE_REG,	0x100B,	0x02a ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x02a,	10,	11,	g_clk_stc0_num,	41,	g_clk_stc0_ptr },

   { "g_clk_stc1" , HAS_GATE_REG,	0x100B,	0x02a ,	12,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x02a,	14,	15,	g_clk_stc1_num,	42,	g_clk_stc1_ptr },

   { "g_clk_vedac" , HAS_GATE_REG,	0x100B,	0x024 ,	8,	CLK_GATE_NORMAL,	HAS_SEL_REG,	0x100B,	0x024,	10,	12,	g_clk_vedac_num,	43,	g_clk_vedac_ptr },


};

clock_table_param clkm_param = {clock_table_array,CLKM_TABLE_NUM};



#endif


