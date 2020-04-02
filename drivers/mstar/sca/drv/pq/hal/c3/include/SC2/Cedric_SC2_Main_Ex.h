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
// 1/7/2013 20:50
// 7/1/2008 10:31
//****************************************************

#ifndef _CEDRIC_SC2_MAIN_EX_H_
#define _CEDRIC_SC2_MAIN_EX_H_

#define PQ_IP_NUM_SC2_Main_Ex 15

typedef enum
{
    #if PQ_QM_CVBS
    QM_RF_NTSC_44_SC2_Main_Ex, //0
    #endif
    #if PQ_QM_CVBS
    QM_RF_NTSC_M_SC2_Main_Ex, //1
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_BGHI_SC2_Main_Ex, //2
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_60_SC2_Main_Ex, //3
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_M_SC2_Main_Ex, //4
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_N_SC2_Main_Ex, //5
    #endif
    #if PQ_QM_CVBS
    QM_RF_SECAM_SC2_Main_Ex, //6
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_44_SC2_Main_Ex, //7
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_M_SC2_Main_Ex, //8
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_BGHI_SC2_Main_Ex, //9
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_60_SC2_Main_Ex, //10
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_M_SC2_Main_Ex, //11
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_N_SC2_Main_Ex, //12
    #endif
    #if PQ_QM_CVBS
    QM_VIF_SECAM_SC2_Main_Ex, //13
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_44_SC2_Main_Ex, //14
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_M_SC2_Main_Ex, //15
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_BGHI_SC2_Main_Ex, //16
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_60_SC2_Main_Ex, //17
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_M_SC2_Main_Ex, //18
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_N_SC2_Main_Ex, //19
    #endif
    #if PQ_QM_CVBS
    QM_SV_SECAM_SC2_Main_Ex, //20
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_44_SC2_Main_Ex, //21
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_M_SC2_Main_Ex, //22
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_BGHI_SC2_Main_Ex, //23
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_60_SC2_Main_Ex, //24
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_M_SC2_Main_Ex, //25
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_N_SC2_Main_Ex, //26
    #endif
    #if PQ_QM_CVBS
    QM_AV_SECAM_SC2_Main_Ex, //27
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_44_SC2_Main_Ex, //28
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_M_SC2_Main_Ex, //29
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_BGHI_SC2_Main_Ex, //30
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_60_SC2_Main_Ex, //31
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_M_SC2_Main_Ex, //32
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_N_SC2_Main_Ex, //33
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_SECAM_SC2_Main_Ex, //34
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_44_SC2_Main_Ex, //35
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_M_SC2_Main_Ex, //36
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_BGHI_SC2_Main_Ex, //37
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_60_SC2_Main_Ex, //38
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_M_SC2_Main_Ex, //39
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_N_SC2_Main_Ex, //40
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_SECAM_SC2_Main_Ex, //41
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_NTSC_SC2_Main_Ex, //42
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_PAL_SC2_Main_Ex, //43
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480i_SC2_Main_Ex, //44
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576i_SC2_Main_Ex, //45
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480p_SC2_Main_Ex, //46
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576p_SC2_Main_Ex, //47
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_24hz_SC2_Main_Ex, //48
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_50hz_SC2_Main_Ex, //49
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_60hz_SC2_Main_Ex, //50
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_50hz_SC2_Main_Ex, //51
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_60hz_SC2_Main_Ex, //52
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_24hz_SC2_Main_Ex, //53
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_50hz_SC2_Main_Ex, //54
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_60hz_SC2_Main_Ex, //55
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480i_SC2_Main_Ex, //56
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576i_SC2_Main_Ex, //57
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480p_SC2_Main_Ex, //58
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576p_SC2_Main_Ex, //59
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_24hz_SC2_Main_Ex, //60
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_50hz_SC2_Main_Ex, //61
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_60hz_SC2_Main_Ex, //62
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_50hz_SC2_Main_Ex, //63
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_60hz_SC2_Main_Ex, //64
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_24hz_SC2_Main_Ex, //65
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_60hz_SC2_Main_Ex, //66
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_50hz_SC2_Main_Ex, //67
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vup_SC2_Main_Ex, //68
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vdown_SC2_Main_Ex, //69
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vno_SC2_Main_Ex, //70
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vup_SC2_Main_Ex, //71
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vdown_SC2_Main_Ex, //72
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vno_SC2_Main_Ex, //73
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vup_SC2_Main_Ex, //74
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vdown_SC2_Main_Ex, //75
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vno_SC2_Main_Ex, //76
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vup_SC2_Main_Ex, //77
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vdown_SC2_Main_Ex, //78
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vno_SC2_Main_Ex, //79
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vup_SC2_Main_Ex, //80
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vdown_SC2_Main_Ex, //81
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vno_SC2_Main_Ex, //82
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vup_SC2_Main_Ex, //83
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vdown_SC2_Main_Ex, //84
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vno_SC2_Main_Ex, //85
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vup_SC2_Main_Ex, //86
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown_SC2_Main_Ex, //87
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vno_SC2_Main_Ex, //88
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup_SC2_Main_Ex, //89
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown_SC2_Main_Ex, //90
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno_SC2_Main_Ex, //91
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vup_SC2_Main_Ex, //92
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown_SC2_Main_Ex, //93
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_SC2_Main_Ex, //94
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_MPEG2_SC2_Main_Ex, //95
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_MPEG2_SC2_Main_Ex, //96
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_MPEG2_SC2_Main_Ex, //97
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_MPEG2_SC2_Main_Ex, //98
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_MPEG2_SC2_Main_Ex, //99
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_MPEG2_SC2_Main_Ex, //100
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_MPEG2_SC2_Main_Ex, //101
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_MPEG2_SC2_Main_Ex, //102
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_MPEG2_SC2_Main_Ex, //103
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_MPEG2_SC2_Main_Ex, //104
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_MPEG2_SC2_Main_Ex, //105
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_MPEG2_SC2_Main_Ex, //106
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_MPEG2_SC2_Main_Ex, //107
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_H264_SC2_Main_Ex, //108
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_H264_SC2_Main_Ex, //109
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_H264_SC2_Main_Ex, //110
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_H264_SC2_Main_Ex, //111
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_H264_SC2_Main_Ex, //112
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_H264_SC2_Main_Ex, //113
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_H264_SC2_Main_Ex, //114
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_H264_SC2_Main_Ex, //115
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_H264_SC2_Main_Ex, //116
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_H264_SC2_Main_Ex, //117
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_H264_SC2_Main_Ex, //118
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_H264_SC2_Main_Ex, //119
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_H264_SC2_Main_Ex, //120
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_interlace_SC2_Main_Ex, //121
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_progressive_SC2_Main_Ex, //122
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_interlace_SC2_Main_Ex, //123
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_progressive_SC2_Main_Ex, //124
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_SD_progressive_SC2_Main_Ex, //125
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_HD_progressive_SC2_Main_Ex, //126
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_interlace_SC2_Main_Ex, //127
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_progressive_SC2_Main_Ex, //128
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_interlace_SC2_Main_Ex, //129
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_progressive_SC2_Main_Ex, //130
    #endif
    QM_INPUTTYPE_NUM_SC2_Main_Ex, // 131
} SC_QUALITY_MAP_INDEX_e_SC2_Main_Ex;

typedef enum
{
PQ_IP_AFEC_no_comm_SC2_Main_Ex,  //0
PQ_IP_HISDNR_SC2_Main_Ex,  //1
PQ_IP_SPF_DBK_SC2_Main_Ex,  //2
PQ_IP_DMS_SC2_Main_Ex,  //3
PQ_IP_DMS_H_SC2_Main_Ex,  //4
PQ_IP_DMS_V_SC2_Main_Ex,  //5
PQ_IP_DMS_NMR_SC2_Main_Ex,  //6
PQ_IP_VIP_HNMR_Y_lowY_SC2_Main_Ex,  //7
PQ_IP_VIP_VNMR_Y_lowY_SC2_Main_Ex,  //8
PQ_IP_VIP_Peaking_adptive_SC2_Main_Ex,  //9
PQ_IP_VIP_Peaking_Pcoring_ad_C_SC2_Main_Ex,  //10
PQ_IP_VIP_Peaking_gain_by_Cedge_SC2_Main_Ex,  //11
PQ_IP_VIP_ICC_Target_SC2_Main_Ex,  //12
PQ_IP_VIP_IBC_weight_SC2_Main_Ex,  //13
PQ_IP_VIP_ACK_SC2_Main_Ex,  //14
 }   PQ_IPTYPE_SC2_Main_Ex;

typedef enum
{
PQ_IP_AFEC_no_comm_COM_SC2_Main_Ex,  //0
PQ_IP_HISDNR_COM_SC2_Main_Ex,  //1
PQ_IP_SPF_DBK_COM_SC2_Main_Ex,  //2
PQ_IP_DMS_COM_SC2_Main_Ex,  //3
PQ_IP_DMS_H_COM_SC2_Main_Ex,  //4
PQ_IP_DMS_V_COM_SC2_Main_Ex,  //5
PQ_IP_DMS_NMR_COM_SC2_Main_Ex,  //6
PQ_IP_VIP_HNMR_Y_lowY_COM_SC2_Main_Ex,  //7
PQ_IP_VIP_VNMR_Y_lowY_COM_SC2_Main_Ex,  //8
PQ_IP_VIP_Peaking_adptive_COM_SC2_Main_Ex,  //9
PQ_IP_VIP_Peaking_Pcoring_ad_C_COM_SC2_Main_Ex,  //10
PQ_IP_VIP_Peaking_gain_by_Cedge_COM_SC2_Main_Ex,  //11
PQ_IP_VIP_ICC_Target_COM_SC2_Main_Ex,  //12
PQ_IP_VIP_IBC_weight_COM_SC2_Main_Ex,  //13
PQ_IP_VIP_ACK_COM_SC2_Main_Ex,  //14
PQ_IP_COM_NUMS_SC2_Main_Ex
} PQ_IP_COM_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_AFEC_no_comm_ON_SC2_Main_Ex,
PQ_IP_AFEC_no_comm_NUMS_SC2_Main_Ex
} PQ_IP_AFEC_no_comm_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_HISDNR_OFF_SC2_Main_Ex,
PQ_IP_HISDNR_ON_SC2_Main_Ex,
PQ_IP_HISDNR_NUMS_SC2_Main_Ex
} PQ_IP_HISDNR_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_SPF_DBK_OFF_SC2_Main_Ex,
PQ_IP_SPF_DBK_LON_SC2_Main_Ex,
PQ_IP_SPF_DBK_NUMS_SC2_Main_Ex
} PQ_IP_SPF_DBK_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_DMS_OFF_SC2_Main_Ex,
PQ_IP_DMS_ON_SC2_Main_Ex,
PQ_IP_DMS_NUMS_SC2_Main_Ex
} PQ_IP_DMS_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_DMS_H_J0_SC2_Main_Ex,
PQ_IP_DMS_H_J1_SC2_Main_Ex,
PQ_IP_DMS_H_NUMS_SC2_Main_Ex
} PQ_IP_DMS_H_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_DMS_V_J0_SC2_Main_Ex,
PQ_IP_DMS_V_J1_SC2_Main_Ex,
PQ_IP_DMS_V_NUMS_SC2_Main_Ex
} PQ_IP_DMS_V_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_DMS_NMR_OFF_SC2_Main_Ex,
PQ_IP_DMS_NMR_0x03_SC2_Main_Ex,
PQ_IP_DMS_NMR_NUMS_SC2_Main_Ex
} PQ_IP_DMS_NMR_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_HNMR_Y_lowY_OFF_SC2_Main_Ex,
PQ_IP_VIP_HNMR_Y_lowY_S2_SC2_Main_Ex,
PQ_IP_VIP_HNMR_Y_lowY_NUMS_SC2_Main_Ex
} PQ_IP_VIP_HNMR_Y_lowY_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_VNMR_Y_lowY_OFF_SC2_Main_Ex,
PQ_IP_VIP_VNMR_Y_lowY_S2_SC2_Main_Ex,
PQ_IP_VIP_VNMR_Y_lowY_NUMS_SC2_Main_Ex
} PQ_IP_VIP_VNMR_Y_lowY_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_Peaking_adptive_RF_NTSC_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_AV_NTSC_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_AV_PAL_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_480i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_720p_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_1080i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_H_480i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_H_576i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_H_720p_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_H_1080i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_MPEG2_480is_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_MPEG2_576i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_MPEG2_1080i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_H264_480is_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_H264_576p_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_H264_720p_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_H264_1080i_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_DT_H264_1080p_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_PC_mode_SC2_Main_Ex,
PQ_IP_VIP_Peaking_adptive_NUMS_SC2_Main_Ex
} PQ_IP_VIP_Peaking_adptive_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_Peaking_Pcoring_ad_C_OFF_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T1_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T2_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T3_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T4_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T5_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_T6_SC2_Main_Ex,
PQ_IP_VIP_Peaking_Pcoring_ad_C_NUMS_SC2_Main_Ex
} PQ_IP_VIP_Peaking_Pcoring_ad_C_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_Peaking_gain_by_Cedge_OFF_SC2_Main_Ex,
PQ_IP_VIP_Peaking_gain_by_Cedge_T2_SC2_Main_Ex,
PQ_IP_VIP_Peaking_gain_by_Cedge_NUMS_SC2_Main_Ex
} PQ_IP_VIP_Peaking_gain_by_Cedge_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_ICC_Target_OFF_SC2_Main_Ex,
PQ_IP_VIP_ICC_Target_ON_SC2_Main_Ex,
PQ_IP_VIP_ICC_Target_NUMS_SC2_Main_Ex
} PQ_IP_VIP_ICC_Target_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_IBC_weight_W6_SC2_Main_Ex,
PQ_IP_VIP_IBC_weight_W8_SC2_Main_Ex,
PQ_IP_VIP_IBC_weight_NUMS_SC2_Main_Ex
} PQ_IP_VIP_IBC_weight_Group_SC2_Main_Ex;

typedef enum
{
PQ_IP_VIP_ACK_OFF_SC2_Main_Ex,
PQ_IP_VIP_ACK_NUMS_SC2_Main_Ex
} PQ_IP_VIP_ACK_Group_SC2_Main_Ex;

extern code U8 MST_SkipRule_IP_SC2_Main_Ex[PQ_IP_NUM_SC2_Main_Ex];
extern code EN_IPTAB_INFO PQ_IPTAB_INFO_SC2_Main_Ex[];
extern code U8 QMAP_1920_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];
extern code U8 QMAP_1920_PIP_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];
extern code U8 QMAP_1920_POP_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];
extern code U8 QMAP_1366_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];
extern code U8 QMAP_1366_PIP_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];
extern code U8 QMAP_1366_POP_SC2_Main_Ex[QM_INPUTTYPE_NUM_SC2_Main_Ex][PQ_IP_NUM_SC2_Main_Ex];

#endif
