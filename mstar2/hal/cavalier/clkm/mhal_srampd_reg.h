#ifndef _MHAL_SRAMPD_REG_H
#define _MHAL_SRAMPD_REG_H


typedef struct _sram_ip_info
{
	char *ip_name;
	u32 bank;
	u32 reg;
	u32 offset;
}sram_ip_info;

typedef struct _sram_pd_table
{
	char *block_name;
	u32 sram_pd_ptr_num;
	sram_ip_info *sram_pd_ptr;
}sram_pd_table;

#define	SRAM_PWR_DIG_num	11


sram_ip_info SRAM_PWR_DIG_ptr[]={
	{"[0] : miu_0",0x1711,0x10,0},
	{"[1] : miu_1",0x1711,0x10,1},
	{"[2] : mailbox",0x1711,0x10,2},
	{"[3] : bdma",0x1711,0x10,3},
	{"[4] : hdmitx",0x1711,0x10,4},
	{"[5] : cmdq",0x1711,0x10,5},
	{"[6] : dvi_hdmi",0x1711,0x10,6},
	{"[7] : jpi",0x1711,0x10,7},
	{"[8] : bwp_bdma",0x1711,0x10,8},
	{"[9] : bwp_urdma",0x1711,0x10,9},
	{"[10] : bwp_mcu51",0x1711,0x10,10},
};

#define	SRAM_PWR_CODEC_num	24


sram_ip_info SRAM_PWR_CODEC_ptr[]={
	{"[0] : GE",0x1712,0x10,0},
	{"[1] : USB30",0x1712,0x10,1},
	{"[2] : MVD",0x1712,0x10,2},
	{"[3] : GPD",0x1712,0x10,3},
	{"[4] : HVD_R2",0x1712,0x10,4},
	{"[5] : HVD_R2_miu0_bwp",0x1712,0x10,5},
	{"[6] : HVD_R2_miu1_bwp",0x1712,0x10,6},
	{"[7] : GMAC",0x1712,0x10,7},
	{"[8] : pcie",0x1712,0x10,8},
	{"[9] : reserved",0x1712,0x10,9},
	{"[10] : SECGMAC",0x1712,0x10,10},
	{"[11] : reserved",0x1712,0x10,11},
	{"[12] : reserved",0x1712,0x10,12},
	{"[13] : reserved",0x1712,0x10,13},
	{"[14] : reserved",0x1712,0x10,14},
	{"[15] : FCIE5",0x1712,0x10,15},
	{"[16] : ZDEC",0x1712,0x10,16},
	{"[17] : sdio30",0x1712,0x10,17},
	{"[18] : JPD",0x1712,0x10,18},
	{"[19] : reserved",0x1712,0x10,19},
	{"[20] : MFE",0x1712,0x10,20},
	{"[21] : UHC0",0x1712,0x10,21},
	{"[22] : UHC1",0x1712,0x10,22},
	{"[23] : UHC2",0x1712,0x10,23},
};

#define	SRAM_PWR_HI_CODEC_num	2


sram_ip_info SRAM_PWR_HI_CODEC_ptr[]={
	{"[0] : hi_codec_0",0x1713,0x10,0},
	{"[1] : hi_codec_1",0x1713,0x10,1},
};

#define	SRAM_PWR_HI_CODEC_LITE_num	2


sram_ip_info SRAM_PWR_HI_CODEC_LITE_ptr[]={
	{"[0] : hi_codec_lite_0",0x1718,0x10,0},
	{"[1] : hi_codec_lite_1",0x1718,0x10,1},
};

#define	SRAM_PWR_TSP_num	16


sram_ip_info SRAM_PWR_TSP_ptr[]={
	{"[0] : ixsc",0x1715,0x10,0},
	{"[1] : KeyTable",0x1715,0x10,1},
	{"[2] : KeyLadders",0x1715,0x10,2},
	{"[3] : NSK2_Integrate",0x1715,0x10,3},
	{"[4] : TSCE",0x1715,0x10,4},
	{"[5] : ca_flatten",0x1715,0x10,5},
	{"[6] : cryptoDMA",0x1715,0x10,6},
	{"[7] : rasp",0x1715,0x10,7},
	{"[8] : emm",0x1715,0x10,8},
	{"[9] : CAMiuCrossBar",0x1715,0x10,9},
	{"[10] : tsp",0x1715,0x10,10},
	{"[11] : tso",0x1715,0x10,11},
	{"[12] : secure_r2",0x1715,0x10,12},
	{"[13] : secure_51",0x1715,0x10,13},
	{"[14] : secure_r2_0_bwp",0x1715,0x10,14},
	{"[15] : secure_r2_1_bwp",0x1715,0x10,15},
};

#define	SRAM_PWR_VIVALDI9_num	3


sram_ip_info SRAM_PWR_VIVALDI9_ptr[]={
	{"[0] : V9_R2_1",0x1716,0x10,0},
	{"[1] : V9_R2",0x1716,0x10,1},
	{"[2] : V9_IP",0x1716,0x10,2},
};

#define	SRAM_PWR_SC_num	84


sram_ip_info SRAM_PWR_SC_ptr[]={
	{"[5:0] : sc_di",0x1714,0x10,0},
	{"[5:0] : sc_di",0x1714,0x10,1},
	{"[5:0] : sc_di",0x1714,0x10,2},
	{"[5:0] : sc_di",0x1714,0x10,3},
	{"[5:0] : sc_di",0x1714,0x10,4},
	{"[5:0] : sc_di",0x1714,0x10,5},
	{"[9:6] : sc_dip",0x1714,0x10,6},
	{"[9:6] : sc_dip",0x1714,0x10,7},
	{"[9:6] : sc_dip",0x1714,0x10,8},
	{"[9:6] : sc_dip",0x1714,0x10,9},
	{"[17:10] : sc_gop",0x1714,0x10,10},
	{"[17:10] : sc_gop",0x1714,0x10,11},
	{"[17:10] : sc_gop",0x1714,0x10,12},
	{"[17:10] : sc_gop",0x1714,0x10,13},
	{"[17:10] : sc_gop",0x1714,0x10,14},
	{"[17:10] : sc_gop",0x1714,0x10,15},
	{"[17:10] : sc_gop",0x1714,0x10,16},
	{"[17:10] : sc_gop",0x1714,0x10,17},
	{"[19:18] : sc_hdmi",0x1714,0x10,18},
	{"[19:18] : sc_hdmi",0x1714,0x10,19},
	{"[22:20] : sc_hvsd",0x1714,0x10,20},
	{"[22:20] : sc_hvsd",0x1714,0x10,21},
	{"[22:20] : sc_hvsd",0x1714,0x10,22},
	{"[25:23] : sc_hvsp ",0x1714,0x10,23},
	{"[25:23] : sc_hvsp ",0x1714,0x10,24},
	{"[25:23] : sc_hvsp ",0x1714,0x10,25},
	{"[33:26] : sc_ip",0x1714,0x10,26},
	{"[33:26] : sc_ip",0x1714,0x10,27},
	{"[33:26] : sc_ip",0x1714,0x10,28},
	{"[33:26] : sc_ip",0x1714,0x10,29},
	{"[33:26] : sc_ip",0x1714,0x10,30},
	{"[33:26] : sc_ip",0x1714,0x10,31},
	{"[33:26] : sc_ip",0x1714,0x10,32},
	{"[33:26] : sc_ip",0x1714,0x10,33},
	{"[36:34] : sc_mvop ",0x1714,0x10,34},
	{"[36:34] : sc_mvop ",0x1714,0x10,35},
	{"[36:34] : sc_mvop ",0x1714,0x10,36},
	{"[39:37] : sc_op1_0",0x1714,0x10,37},
	{"[39:37] : sc_op1_0",0x1714,0x10,38},
	{"[39:37] : sc_op1_0",0x1714,0x10,39},
	{"[41:40] : sc_op1_1",0x1714,0x10,40},
	{"[41:40] : sc_op1_1",0x1714,0x10,41},
	{"[44:42] : sc_op2 ",0x1714,0x10,42},
	{"[44:42] : sc_op2 ",0x1714,0x10,43},
	{"[44:42] : sc_op2 ",0x1714,0x10,44},
	{"[48:45] : sc_pdw0",0x1714,0x10,45},
	{"[48:45] : sc_pdw0",0x1714,0x10,46},
	{"[48:45] : sc_pdw0",0x1714,0x10,47},
	{"[48:45] : sc_pdw0",0x1714,0x10,48},
	{"[55:49] : sc_scmi",0x1714,0x10,49},
	{"[55:49] : sc_scmi",0x1714,0x10,50},
	{"[55:49] : sc_scmi",0x1714,0x10,51},
	{"[55:49] : sc_scmi",0x1714,0x10,52},
	{"[55:49] : sc_scmi",0x1714,0x10,53},
	{"[55:49] : sc_scmi",0x1714,0x10,54},
	{"[55:49] : sc_scmi",0x1714,0x10,55},
	{"[56] : sc_ve",0x1714,0x10,56},
	{"[57] : sc_disp",0x1714,0x10,57},
	{"[58] : sc1_di",0x1714,0x10,58},
	{"[61:59] : sc1_hvsp",0x1714,0x10,59},
	{"[61:59] : sc1_hvsp",0x1714,0x10,60},
	{"[61:59] : sc1_hvsp",0x1714,0x10,61},
	{"[69:62] : sc1_op ",0x1714,0x10,62},
	{"[69:62] : sc1_op ",0x1714,0x10,63},
	{"[69:62] : sc1_op ",0x1714,0x10,64},
	{"[69:62] : sc1_op ",0x1714,0x10,65},
	{"[69:62] : sc1_op ",0x1714,0x10,66},
	{"[69:62] : sc1_op ",0x1714,0x10,67},
	{"[69:62] : sc1_op ",0x1714,0x10,68},
	{"[69:62] : sc1_op ",0x1714,0x10,69},
	{"[72:70] : sc1_op1_0",0x1714,0x10,70},
	{"[72:70] : sc1_op1_0",0x1714,0x10,71},
	{"[72:70] : sc1_op1_0",0x1714,0x10,72},
	{"[73] : sc1_op2",0x1714,0x10,73},
	{"[79:74] : sc1_scmi",0x1714,0x10,74},
	{"[79:74] : sc1_scmi",0x1714,0x10,75},
	{"[79:74] : sc1_scmi",0x1714,0x10,76},
	{"[79:74] : sc1_scmi",0x1714,0x10,77},
	{"[79:74] : sc1_scmi",0x1714,0x10,78},
	{"[79:74] : sc1_scmi",0x1714,0x10,79},
	{"[83:80] : sc_hdr",0x1714,0x10,80},
	{"[83:80] : sc_hdr",0x1714,0x10,81},
	{"[83:80] : sc_hdr",0x1714,0x10,82},
	{"[83:80] : sc_hdr",0x1714,0x10,83},
};

#define SRAM_PD_TABLE_NUM 7
sram_pd_table sram_pd_table_array[SRAM_PD_TABLE_NUM] =
{
	{"SRAM_PWR_DIG",SRAM_PWR_DIG_num,SRAM_PWR_DIG_ptr },
	{"SRAM_PWR_CODEC",SRAM_PWR_CODEC_num,SRAM_PWR_CODEC_ptr },
	{"SRAM_PWR_HI_CODEC",SRAM_PWR_HI_CODEC_num,SRAM_PWR_HI_CODEC_ptr },
	{"SRAM_PWR_HI_CODEC_LITE",SRAM_PWR_HI_CODEC_LITE_num,SRAM_PWR_HI_CODEC_LITE_ptr },
	{"SRAM_PWR_TSP",SRAM_PWR_TSP_num,SRAM_PWR_TSP_ptr },
	{"SRAM_PWR_VIVALDI9",SRAM_PWR_VIVALDI9_num,SRAM_PWR_VIVALDI9_ptr },
	{"SRAM_PWR_SC",SRAM_PWR_SC_num,SRAM_PWR_SC_ptr },
};

#endif