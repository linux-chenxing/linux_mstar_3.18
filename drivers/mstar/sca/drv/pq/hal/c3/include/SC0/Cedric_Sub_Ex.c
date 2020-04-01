////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (; MStar; Confidential; Information; ) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
//****************************************************
//   Quality Map CodeGen Version 4.0
// 1920
// Cedric
// 1/7/2013 20:41
// 7/1/2008 10:31
//****************************************************

#ifndef _CEDRIC_SUB_EX_C_
#define _CEDRIC_SUB_EX_C_

#if PQ_SKIPRULE_ENABLE

code U8 MST_SkipRule_IP_Sub_Ex[PQ_IP_NUM_Sub_Ex]=
{
    /* AFEC_no_comm */ 0, /* SPF_DBK */ 0, /* DMS */ 0, /* DMS_H */ 0, 
    /* DMS_V */ 0, /* DMS_NMR */ 0, /* VIP_HNMR_Y_lowY */ 0, /* VIP_VNMR_Y_lowY */ 0, 
    /* VIP_Peaking_adptive */ 0, /* VIP_Peaking_Pcoring_ad_C */ 0, /* VIP_Peaking_gain_by_Cedge */ 0, /* VIP_ICC_Target */ 0, 
    /* VIP_ACK */ 0, 
};

#endif
//****************************************************
// AFEC_no_comm
//****************************************************
code U8 MST_AFEC_no_comm_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_AFEC_no_comm_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_AFEC_no_comm_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_BK35_2A_H), 0x04, 0x04/*ON*/, },
 { PQ_MAP_REG(REG_BK35_2A_H), 0x04, 0x00/*ON*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// SPF_DBK
//****************************************************
code U8 MST_SPF_DBK_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_SPF_DBK_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_SPF_DBK_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK0C_74_L), 0x01, 0x00/*OFF*/, 
                              0x01/*LON*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// DMS
//****************************************************
code U8 MST_DMS_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_SC_BK26_14_L), 0x02, 0x02 },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_DMS_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_DMS_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// DMS_H
//****************************************************
code U8 MST_DMS_H_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_SC_BK26_14_L), 0x10, 0x00 },
 { PQ_MAP_REG(REG_SC_BK26_1C_L), 0x0B, 0x00 },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_DMS_H_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_DMS_H_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// DMS_V
//****************************************************
code U8 MST_DMS_V_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_SC_BK26_14_L), 0x20, 0x00 },
 { PQ_MAP_REG(REG_SC_BK26_1C_L), 0x30, 0x00 },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_DMS_V_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_DMS_V_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// DMS_NMR
//****************************************************
code U8 MST_DMS_NMR_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_DMS_NMR_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_DMS_NMR_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK26_14_H), 0x04, 0x00/*OFF*/, 
                              0x04/*0x03*/, },
 { PQ_MAP_REG(REG_SC_BK26_1D_L), 0x3F, 0x00/*OFF*/, 
                              0x03/*0x03*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_HNMR_Y_lowY
//****************************************************
code U8 MST_VIP_HNMR_Y_lowY_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_HNMR_Y_lowY_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_HNMR_Y_lowY_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK18_70_L), 0x10, 0x00/*OFF*/, 
                              0x10/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK18_73_L), 0xFF, 0x00/*OFF*/, 
                              0x10/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK18_74_L), 0x3F, 0x00/*OFF*/, 
                              0x04/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK18_74_H), 0x03, 0x00/*OFF*/, 
                              0x01/*S2*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_VNMR_Y_lowY
//****************************************************
code U8 MST_VIP_VNMR_Y_lowY_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_VNMR_Y_lowY_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_VNMR_Y_lowY_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK19_7B_L), 0x10, 0x00/*OFF*/, 
                              0x10/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK19_7E_L), 0xFF, 0x00/*OFF*/, 
                              0x10/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK19_7F_L), 0x3F, 0x00/*OFF*/, 
                              0x04/*S2*/, },
 { PQ_MAP_REG(REG_SC_BK19_7F_H), 0x03, 0x00/*OFF*/, 
                              0x01/*S2*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_Peaking_adptive
//****************************************************
code U8 MST_VIP_Peaking_adptive_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_Peaking_adptive_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_Peaking_adptive_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK19_31_H), 0x9F, 0x03/*$RF_NTSC*/, 
                              0x93/*$AV_NTSC*/, 
                              0x03/*$AV_PAL*/, 
                              0x13/*$480i*/, 
                              0x11/*$720p*/, 
                              0x93/*$1080i*/, 
                              0x03/*$H_480i*/, 
                              0x11/*$H_576i*/, 
                              0x11/*$H_720p*/, 
                              0x93/*$H_1080i*/, 
                              0x03/*$DT_MPEG2_480is*/, 
                              0x11/*$DT_MPEG2_576i*/, 
                              0x03/*$DT_MPEG2_1080i*/, 
                              0x03/*$DT_H264_480is*/, 
                              0x11/*$DT_H264_576p*/, 
                              0x11/*$DT_H264_720p*/, 
                              0x03/*$DT_H264_1080i*/, 
                              0x11/*$DT_H264_1080p*/, 
                              0x11/*$PC_mode*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_Peaking_Pcoring_ad_C
//****************************************************
code U8 MST_VIP_Peaking_Pcoring_ad_C_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_SC_BK27_2F_L), 0xFF, 0x88 },//Same mark
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_Peaking_Pcoring_ad_C_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_Peaking_Pcoring_ad_C_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK19_30_H), 0x10, 0x00/*OFF*/, 
                              0x10/*T1*/, 
                              0x10/*T2*/, 
                              0x10/*T3*/, 
                              0x10/*T4*/, 
                              0x10/*T5*/, 
                              0x10/*T6*/, },
 { PQ_MAP_REG(REG_SC_BK19_37_H), 0x3F, 0x20/*OFF*/, 
                              0x20/*T1*/, 
                              0x20/*T2*/, 
                              0x20/*T3*/, 
                              0x20/*T4*/, 
                              0x00/*T5*/, 
                              0x20/*T6*/, },
 { PQ_MAP_REG(REG_SC_BK27_2E_L), 0xFF, 0x88/*$OFF*/, 
                              0xA8/*$T1*/, 
                              0xA8/*$T2*/, 
                              0x88/*$T3*/, 
                              0x88/*$T4*/, 
                              0x88/*$T5*/, 
                              0x88/*$T6*/, },
 { PQ_MAP_REG(REG_SC_BK27_2E_H), 0xFF, 0x88/*$OFF*/, 
                              0x66/*$T1*/, 
                              0x77/*$T2*/, 
                              0x86/*$T3*/, 
                              0x88/*$T4*/, 
                              0x88/*$T5*/, 
                              0x88/*$T6*/, },
 { PQ_MAP_REG(REG_SC_BK27_2F_H), 0xFF, 0x88/*$OFF*/, 
                              0x85/*$T1*/, 
                              0x86/*$T2*/, 
                              0x87/*$T3*/, 
                              0x88/*$T4*/, 
                              0x88/*$T5*/, 
                              0x88/*$T6*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_Peaking_gain_by_Cedge
//****************************************************
code U8 MST_VIP_Peaking_gain_by_Cedge_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_Peaking_gain_by_Cedge_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_Peaking_gain_by_Cedge_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK19_08_H), 0x77, 0x00/*$OFF*/, 
                              0x33/*$T2*/, },
 { PQ_MAP_REG(REG_SC_BK19_0A_L), 0xFF, 0x00/*$OFF*/, 
                              0x22/*$T2*/, },
 { PQ_MAP_REG(REG_SC_BK19_0A_H), 0xFF, 0x00/*OFF*/, 
                              0xFF/*T2*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_ICC_Target
//****************************************************
code U8 MST_VIP_ICC_Target_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_ICC_Target_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_ICC_Target_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_SC_BK18_30_H), 0x04, 0x00/*OFF*/, 
                              0x04/*ON*/, },
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

//****************************************************
// VIP_ACK
//****************************************************
code U8 MST_VIP_ACK_COM_Sub_Ex[][4] =
{      // Reg           Mask  Value
 { PQ_MAP_REG(REG_SC_BK18_38_L), 0x0C, 0x00 },//Same mark
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};

code U8 MST_VIP_ACK_Sub_Ex[][REG_ADDR_SIZE+REG_MASK_SIZE+PQ_IP_VIP_ACK_NUMS_Sub_Ex]=
{
 { PQ_MAP_REG(REG_TABLE_END), 0x00, 0x00 }
};


code EN_IPTAB_INFO PQ_IPTAB_INFO_Sub_Ex[]=
{
{*MST_AFEC_no_comm_COM_Sub_Ex, *MST_AFEC_no_comm_Sub_Ex, PQ_IP_AFEC_no_comm_NUMS_Sub_Ex, PQ_TABTYPE_GENERAL},
{*MST_SPF_DBK_COM_Sub_Ex, *MST_SPF_DBK_Sub_Ex, PQ_IP_SPF_DBK_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_DMS_COM_Sub_Ex, *MST_DMS_Sub_Ex, PQ_IP_DMS_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_DMS_H_COM_Sub_Ex, *MST_DMS_H_Sub_Ex, PQ_IP_DMS_H_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_DMS_V_COM_Sub_Ex, *MST_DMS_V_Sub_Ex, PQ_IP_DMS_V_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_DMS_NMR_COM_Sub_Ex, *MST_DMS_NMR_Sub_Ex, PQ_IP_DMS_NMR_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_HNMR_Y_lowY_COM_Sub_Ex, *MST_VIP_HNMR_Y_lowY_Sub_Ex, PQ_IP_VIP_HNMR_Y_lowY_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_VNMR_Y_lowY_COM_Sub_Ex, *MST_VIP_VNMR_Y_lowY_Sub_Ex, PQ_IP_VIP_VNMR_Y_lowY_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_Peaking_adptive_COM_Sub_Ex, *MST_VIP_Peaking_adptive_Sub_Ex, PQ_IP_VIP_Peaking_adptive_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_Peaking_Pcoring_ad_C_COM_Sub_Ex, *MST_VIP_Peaking_Pcoring_ad_C_Sub_Ex, PQ_IP_VIP_Peaking_Pcoring_ad_C_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_Peaking_gain_by_Cedge_COM_Sub_Ex, *MST_VIP_Peaking_gain_by_Cedge_Sub_Ex, PQ_IP_VIP_Peaking_gain_by_Cedge_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_ICC_Target_COM_Sub_Ex, *MST_VIP_ICC_Target_Sub_Ex, PQ_IP_VIP_ICC_Target_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
{*MST_VIP_ACK_COM_Sub_Ex, *MST_VIP_ACK_Sub_Ex, PQ_IP_VIP_ACK_NUMS_Sub_Ex, PQ_TABTYPE_SCALER},
};

#endif
