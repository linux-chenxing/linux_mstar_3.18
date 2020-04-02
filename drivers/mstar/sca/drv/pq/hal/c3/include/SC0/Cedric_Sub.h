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
// 1/7/2013  8:39:41 PM
// 39630.43853
//****************************************************

#ifndef _CEDRIC_SUB_H_
#define _CEDRIC_SUB_H_

#define PQ_IP_NUM_Sub 185
#define PQ_IP_SRAM_COLOR_INDEX_SIZE_Sub 1
#define PQ_IP_SRAM_COLOR_GAIN_SNR_SIZE_Sub 1
#define PQ_IP_SRAM_COLOR_GAIN_DNR_SIZE_Sub 1
#define PQ_IP_SRAM1_SIZE_Sub 1024
#define PQ_IP_SRAM2_SIZE_Sub 1024
#define PQ_IP_SRAM3_SIZE_Sub 320
#define PQ_IP_SRAM4_SIZE_Sub 320
#define PQ_IP_C_SRAM1_SIZE_Sub 320
#define PQ_IP_C_SRAM2_SIZE_Sub 320
#define PQ_IP_C_SRAM3_SIZE_Sub 320
#define PQ_IP_C_SRAM4_SIZE_Sub 320
#define PQ_IP_SRAM_3x3matrix_PIC2_SIZE_Sub 32
#define PQ_IP_SRAM_DLC_PIC1_SIZE_Sub 65
#define PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SIZE_Sub 5
#define PQ_IP_SRAM_GammaTbl_R_PIC1_SIZE_Sub 386
#define PQ_IP_SRAM_GammaTbl_G_PIC1_SIZE_Sub 386
#define PQ_IP_SRAM_GammaTbl_B_PIC1_SIZE_Sub 386
#define PQ_IP_SRAM_Auto_Color_PIC2_SIZE_Sub 6
#define PQ_IP_SRAM_Color_Temp_PIC1_SIZE_Sub 9

typedef enum
{
    #if PQ_QM_CVBS
    QM_RF_NTSC_44_Sub, //0
    #endif
    #if PQ_QM_CVBS
    QM_RF_NTSC_M_Sub, //1
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_BGHI_Sub, //2
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_60_Sub, //3
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_M_Sub, //4
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_N_Sub, //5
    #endif
    #if PQ_QM_CVBS
    QM_RF_SECAM_Sub, //6
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_44_Sub, //7
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_M_Sub, //8
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_BGHI_Sub, //9
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_60_Sub, //10
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_M_Sub, //11
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_N_Sub, //12
    #endif
    #if PQ_QM_CVBS
    QM_VIF_SECAM_Sub, //13
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_44_Sub, //14
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_M_Sub, //15
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_BGHI_Sub, //16
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_60_Sub, //17
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_M_Sub, //18
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_N_Sub, //19
    #endif
    #if PQ_QM_CVBS
    QM_SV_SECAM_Sub, //20
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_44_Sub, //21
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_M_Sub, //22
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_BGHI_Sub, //23
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_60_Sub, //24
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_M_Sub, //25
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_N_Sub, //26
    #endif
    #if PQ_QM_CVBS
    QM_AV_SECAM_Sub, //27
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_44_Sub, //28
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_M_Sub, //29
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_BGHI_Sub, //30
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_60_Sub, //31
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_M_Sub, //32
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_N_Sub, //33
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_SECAM_Sub, //34
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_44_Sub, //35
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_M_Sub, //36
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_BGHI_Sub, //37
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_60_Sub, //38
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_M_Sub, //39
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_N_Sub, //40
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_SECAM_Sub, //41
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_NTSC_Sub, //42
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_PAL_Sub, //43
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480i_Sub, //44
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576i_Sub, //45
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480p_Sub, //46
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576p_Sub, //47
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_24hz_Sub, //48
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_50hz_Sub, //49
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_60hz_Sub, //50
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_50hz_Sub, //51
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_60hz_Sub, //52
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_24hz_Sub, //53
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_50hz_Sub, //54
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_60hz_Sub, //55
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480i_Sub, //56
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576i_Sub, //57
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480p_Sub, //58
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576p_Sub, //59
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_24hz_Sub, //60
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_50hz_Sub, //61
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_60hz_Sub, //62
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_50hz_Sub, //63
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_60hz_Sub, //64
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_24hz_Sub, //65
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_60hz_Sub, //66
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_50hz_Sub, //67
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vup_Sub, //68
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vdown_Sub, //69
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vno_Sub, //70
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vup_Sub, //71
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vdown_Sub, //72
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vno_Sub, //73
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vup_Sub, //74
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vdown_Sub, //75
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vno_Sub, //76
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vup_Sub, //77
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vdown_Sub, //78
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vno_Sub, //79
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vup_Sub, //80
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vdown_Sub, //81
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vno_Sub, //82
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vup_Sub, //83
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vdown_Sub, //84
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vno_Sub, //85
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vup_Sub, //86
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown_Sub, //87
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vno_Sub, //88
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup_Sub, //89
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown_Sub, //90
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno_Sub, //91
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vup_Sub, //92
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown_Sub, //93
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_Sub, //94
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_MPEG2_Sub, //95
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_MPEG2_Sub, //96
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_MPEG2_Sub, //97
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_MPEG2_Sub, //98
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_MPEG2_Sub, //99
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_MPEG2_Sub, //100
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_MPEG2_Sub, //101
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_MPEG2_Sub, //102
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_MPEG2_Sub, //103
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_MPEG2_Sub, //104
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_MPEG2_Sub, //105
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_MPEG2_Sub, //106
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_MPEG2_Sub, //107
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_H264_Sub, //108
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_H264_Sub, //109
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_H264_Sub, //110
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_H264_Sub, //111
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_H264_Sub, //112
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_H264_Sub, //113
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_H264_Sub, //114
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_H264_Sub, //115
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_H264_Sub, //116
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_H264_Sub, //117
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_H264_Sub, //118
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_H264_Sub, //119
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_H264_Sub, //120
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_interlace_Sub, //121
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_progressive_Sub, //122
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_interlace_Sub, //123
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_progressive_Sub, //124
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_SD_progressive_Sub, //125
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_HD_progressive_Sub, //126
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_interlace_Sub, //127
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_progressive_Sub, //128
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_interlace_Sub, //129
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_progressive_Sub, //130
    #endif
    #if PQ_QM_3D
    QM_3D_Video_interlace_Sub, //131
    #endif
    #if PQ_QM_3D
    QM_3D_Video_progressive_Sub, //132
    #endif
    QM_INPUTTYPE_NUM_Sub, // 133
} SC_QUALITY_MAP_INDEX_e_Sub;

typedef enum
{
PQ_IP_AFEC_Sub,  //0
PQ_IP_Comb_Sub,  //1
PQ_IP_Comb2_Sub,  //2
PQ_IP_SECAM_Sub,  //3
PQ_IP_VD_Sampling_no_comm_Sub,  //4
PQ_IP_SCinit_Sub,  //5
PQ_IP_CSC_Sub,  //6
PQ_IP_CSC_Dither_Sub,  //7
PQ_IP_YCdelay_Sub,  //8
PQ_IP_PreFilter_Sub,  //9
PQ_IP_PreFilter_Dither_Sub,  //10
PQ_IP_HDSDD_Sub,  //11
PQ_IP_HSD_Y_Sub,  //12
PQ_IP_HSD_C_Sub,  //13
PQ_IP_444To422_Sub,  //14
PQ_IP_VSD_Sub,  //15
PQ_IP_HVSD_Dither_Sub,  //16
PQ_IP_10to8_Dither_Sub,  //17
PQ_IP_MemFormat_Sub,  //18
PQ_IP_PreSNR_Sub,  //19
PQ_IP_PreSNR_Patch_Sub,  //20
PQ_IP_DNR_Sub,  //21
PQ_IP_DNR_Motion_Sub,  //22
PQ_IP_DNR_Y_Sub,  //23
PQ_IP_DNR_Y_COLOR_DEP_Sub,  //24
PQ_IP_SRAM_COLOR_INDEX_Sub,  //25
PQ_IP_SRAM_COLOR_GAIN_SNR_Sub,  //26
PQ_IP_SRAM_COLOR_GAIN_DNR_Sub,  //27
PQ_IP_DNR_Y_LUMA_ADAPTIVE_Sub,  //28
PQ_IP_DNR_POSTTUNE_Sub,  //29
PQ_IP_DNR_C_Sub,  //30
PQ_IP_HISDNR_Sub,  //31
PQ_IP_PNR_Sub,  //32
PQ_IP_PNR_Y_Sub,  //33
PQ_IP_PNR_C_Sub,  //34
PQ_IP_PostCCS_Sub,  //35
PQ_IP_PostCCS_Smooth_Sub,  //36
PQ_IP_420CUP_Sub,  //37
PQ_IP_MADi_Sub,  //38
PQ_IP_MADi_Motion_Sub,  //39
PQ_IP_MADi_ADP3x3_Sub,  //40
PQ_IP_MADi_MORPHO_Sub,  //41
PQ_IP_MADi_DFK_Sub,  //42
PQ_IP_MADi_SST_Sub,  //43
PQ_IP_MADi_EODiW_Sub,  //44
PQ_IP_MADi_Force_Sub,  //45
PQ_IP_EODi_Sub,  //46
PQ_IP_Film_Sub,  //47
PQ_IP_Film32_Sub,  //48
PQ_IP_Film22_Sub,  //49
PQ_IP_Film_any_Sub,  //50
PQ_IP_DIPF_Sub,  //51
PQ_IP_VCLPF_Sub,  //52
PQ_IP_Spike_NR_Sub,  //53
PQ_IP_SPF_Sub,  //54
PQ_IP_SPF_SNR_Sub,  //55
PQ_IP_SPF_SNR_MR_Sub,  //56
PQ_IP_SPF_MR_LPF_Sub,  //57
PQ_IP_SPF_NMR_Y_Sub,  //58
PQ_IP_SPF_NMR_Y_MR_Sub,  //59
PQ_IP_VSP_Y_Sub,  //60
PQ_IP_VSP_C_Sub,  //61
PQ_IP_VSP_CoRing_Sub,  //62
PQ_IP_VSP_DeRing_Sub,  //63
PQ_IP_VSP_Dither_Sub,  //64
PQ_IP_VSP_PreVBound_Sub,  //65
PQ_IP_422To444_Sub,  //66
PQ_IP_PreCTI_Sub,  //67
PQ_IP_HSP_Y_Sub,  //68
PQ_IP_HSP_C_Sub,  //69
PQ_IP_HSP_CoRing_Sub,  //70
PQ_IP_HSP_DeRing_Sub,  //71
PQ_IP_HSP_Dither_Sub,  //72
PQ_IP_HnonLinear_Sub,  //73
PQ_IP_SRAM1_Sub,  //74
PQ_IP_SRAM2_Sub,  //75
PQ_IP_SRAM3_Sub,  //76
PQ_IP_SRAM4_Sub,  //77
PQ_IP_C_SRAM1_Sub,  //78
PQ_IP_C_SRAM2_Sub,  //79
PQ_IP_C_SRAM3_Sub,  //80
PQ_IP_C_SRAM4_Sub,  //81
PQ_IP_VIP_Sub,  //82
PQ_IP_VIP_pseudo_Sub,  //83
PQ_IP_VIP_CSC_Sub,  //84
PQ_IP_VIP_CSC_dither_Sub,  //85
PQ_IP_VIP_Post_YC_delay_Sub,  //86
PQ_IP_VIP_HNMR_Y_Sub,  //87
PQ_IP_VIP_HNMR_ad_C_Sub,  //88
PQ_IP_VIP_HNMR_ad_C_gain_Sub,  //89
PQ_IP_VIP_HNMR_C_win1_Sub,  //90
PQ_IP_VIP_HNMR_C_win2_Sub,  //91
PQ_IP_VIP_HNMR_C_win3_Sub,  //92
PQ_IP_VIP_HNMR_C_win4_Sub,  //93
PQ_IP_VIP_Pre_Yoffset_Sub,  //94
PQ_IP_VIP_Pre_Ygain_Sub,  //95
PQ_IP_VIP_Pre_Ygain_dither_Sub,  //96
PQ_IP_VIP_HLPF_Sub,  //97
PQ_IP_VIP_HLPF_dither_Sub,  //98
PQ_IP_VIP_Peaking_Sub,  //99
PQ_IP_VIP_Peaking_band_Sub,  //100
PQ_IP_VIP_Peaking_Pcoring_Sub,  //101
PQ_IP_VIP_Peaking_gain_Sub,  //102
PQ_IP_VIP_Peaking_gain_ad_C_Sub,  //103
PQ_IP_VIP_Post_CTI_Sub,  //104
PQ_IP_VIP_Post_CTI_coef_Sub,  //105
PQ_IP_VIP_FCC_T1_Sub,  //106
PQ_IP_VIP_FCC_T2_Sub,  //107
PQ_IP_VIP_FCC_T3_Sub,  //108
PQ_IP_VIP_FCC_T4_Sub,  //109
PQ_IP_VIP_FCC_T5_Sub,  //110
PQ_IP_VIP_FCC_T6_Sub,  //111
PQ_IP_VIP_FCC_T7_Sub,  //112
PQ_IP_VIP_FCC_T8_Sub,  //113
PQ_IP_VIP_FCC_T9_Sub,  //114
PQ_IP_VIP_IHC_Sub,  //115
PQ_IP_VIP_IHC_R_Sub,  //116
PQ_IP_VIP_IHC_G_Sub,  //117
PQ_IP_VIP_IHC_B_Sub,  //118
PQ_IP_VIP_IHC_C_Sub,  //119
PQ_IP_VIP_IHC_M_Sub,  //120
PQ_IP_VIP_IHC_Y_Sub,  //121
PQ_IP_VIP_IHC_F_Sub,  //122
PQ_IP_VIP_ICC_Sub,  //123
PQ_IP_VIP_ICC_R_Sub,  //124
PQ_IP_VIP_ICC_G_Sub,  //125
PQ_IP_VIP_ICC_B_Sub,  //126
PQ_IP_VIP_ICC_C_Sub,  //127
PQ_IP_VIP_ICC_M_Sub,  //128
PQ_IP_VIP_ICC_Y_Sub,  //129
PQ_IP_VIP_ICC_F_Sub,  //130
PQ_IP_VIP_ICC_NC_Sub,  //131
PQ_IP_VIP_IBC_Sub,  //132
PQ_IP_VIP_IBC_R_Sub,  //133
PQ_IP_VIP_IBC_G_Sub,  //134
PQ_IP_VIP_IBC_B_Sub,  //135
PQ_IP_VIP_IBC_C_Sub,  //136
PQ_IP_VIP_IBC_M_Sub,  //137
PQ_IP_VIP_IBC_Y_Sub,  //138
PQ_IP_VIP_IBC_F_Sub,  //139
PQ_IP_VIP_DLC_Sub,  //140
PQ_IP_VIP_DLC_dither_Sub,  //141
PQ_IP_VIP_DLC_Flesh_adj_Sub,  //142
PQ_IP_VIP_DLC_His_range_Sub,  //143
PQ_IP_VIP_DLC_His_rangeH_Sub,  //144
PQ_IP_VIP_DLC_His_rangeV_Sub,  //145
PQ_IP_VIP_BLE_Sub,  //146
PQ_IP_VIP_WLE_Sub,  //147
PQ_IP_VIP_BWLE_dither_Sub,  //148
PQ_IP_VIP_UVC_Sub,  //149
PQ_IP_VIP_Post_Yoffset_Sub,  //150
PQ_IP_VIP_Post_Ygain_Sub,  //151
PQ_IP_VIP_Post_Cgain_Sub,  //152
PQ_IP_VIP_Hcoring_Y_Sub,  //153
PQ_IP_VIP_Hcoring_C_Sub,  //154
PQ_IP_VIP_Hcoring_dither_Sub,  //155
PQ_IP_SwDriver_Sub,  //156
PQ_IP_3x3_Sub,  //157
PQ_IP_RGB_Offset_Sub,  //158
PQ_IP_RGB_Clip_Sub,  //159
PQ_IP_xvYCC_bypass_Sub,  //160
PQ_IP_xvYCC_de_gamma_Sub,  //161
PQ_IP_xvYCC_de_gamma_dither_Sub,  //162
PQ_IP_xvYCC_3x3_Sub,  //163
PQ_IP_xvYCC_Comp_Sub,  //164
PQ_IP_xvYCC_Comp_dither_Sub,  //165
PQ_IP_xvYCC_Clip_Sub,  //166
PQ_IP_xvYCC_gamma_Sub,  //167
PQ_IP_xvYCC_gamma_dither_Sub,  //168
PQ_IP_rgb_3d_Sub,  //169
PQ_IP_HBC_Sub,  //170
PQ_IP_Pre_CON_BRI_Sub,  //171
PQ_IP_Blue_Stretch_Sub,  //172
PQ_IP_Blue_Stretch_dither_Sub,  //173
PQ_IP_Gamma_Sub,  //174
PQ_IP_Gamma_dither_Sub,  //175
PQ_IP_Post_CON_BRI_Sub,  //176
PQ_IP_SRAM_3x3matrix_PIC2_Sub,  //177
PQ_IP_SRAM_DLC_PIC1_Sub,  //178
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Sub,  //179
PQ_IP_SRAM_GammaTbl_R_PIC1_Sub,  //180
PQ_IP_SRAM_GammaTbl_G_PIC1_Sub,  //181
PQ_IP_SRAM_GammaTbl_B_PIC1_Sub,  //182
PQ_IP_SRAM_Auto_Color_PIC2_Sub,  //183
PQ_IP_SRAM_Color_Temp_PIC1_Sub,  //184
 }   PQ_IPTYPE_Sub;

typedef enum
{
PQ_IP_AFEC_COM_Sub,  //0
PQ_IP_Comb_COM_Sub,  //1
PQ_IP_Comb2_COM_Sub,  //2
PQ_IP_SECAM_COM_Sub,  //3
PQ_IP_VD_Sampling_no_comm_COM_Sub,  //4
PQ_IP_SCinit_COM_Sub,  //5
PQ_IP_CSC_COM_Sub,  //6
PQ_IP_CSC_Dither_COM_Sub,  //7
PQ_IP_YCdelay_COM_Sub,  //8
PQ_IP_PreFilter_COM_Sub,  //9
PQ_IP_PreFilter_Dither_COM_Sub,  //10
PQ_IP_HDSDD_COM_Sub,  //11
PQ_IP_HSD_Y_COM_Sub,  //12
PQ_IP_HSD_C_COM_Sub,  //13
PQ_IP_444To422_COM_Sub,  //14
PQ_IP_VSD_COM_Sub,  //15
PQ_IP_HVSD_Dither_COM_Sub,  //16
PQ_IP_10to8_Dither_COM_Sub,  //17
PQ_IP_MemFormat_COM_Sub,  //18
PQ_IP_PreSNR_COM_Sub,  //19
PQ_IP_PreSNR_Patch_COM_Sub,  //20
PQ_IP_DNR_COM_Sub,  //21
PQ_IP_DNR_Motion_COM_Sub,  //22
PQ_IP_DNR_Y_COM_Sub,  //23
PQ_IP_DNR_Y_COLOR_DEP_COM_Sub,  //24
PQ_IP_SRAM_COLOR_INDEX_COM_Sub,  //25
PQ_IP_SRAM_COLOR_GAIN_SNR_COM_Sub,  //26
PQ_IP_SRAM_COLOR_GAIN_DNR_COM_Sub,  //27
PQ_IP_DNR_Y_LUMA_ADAPTIVE_COM_Sub,  //28
PQ_IP_DNR_POSTTUNE_COM_Sub,  //29
PQ_IP_DNR_C_COM_Sub,  //30
PQ_IP_HISDNR_COM_Sub,  //31
PQ_IP_PNR_COM_Sub,  //32
PQ_IP_PNR_Y_COM_Sub,  //33
PQ_IP_PNR_C_COM_Sub,  //34
PQ_IP_PostCCS_COM_Sub,  //35
PQ_IP_PostCCS_Smooth_COM_Sub,  //36
PQ_IP_420CUP_COM_Sub,  //37
PQ_IP_MADi_COM_Sub,  //38
PQ_IP_MADi_Motion_COM_Sub,  //39
PQ_IP_MADi_ADP3x3_COM_Sub,  //40
PQ_IP_MADi_MORPHO_COM_Sub,  //41
PQ_IP_MADi_DFK_COM_Sub,  //42
PQ_IP_MADi_SST_COM_Sub,  //43
PQ_IP_MADi_EODiW_COM_Sub,  //44
PQ_IP_MADi_Force_COM_Sub,  //45
PQ_IP_EODi_COM_Sub,  //46
PQ_IP_Film_COM_Sub,  //47
PQ_IP_Film32_COM_Sub,  //48
PQ_IP_Film22_COM_Sub,  //49
PQ_IP_Film_any_COM_Sub,  //50
PQ_IP_DIPF_COM_Sub,  //51
PQ_IP_VCLPF_COM_Sub,  //52
PQ_IP_Spike_NR_COM_Sub,  //53
PQ_IP_SPF_COM_Sub,  //54
PQ_IP_SPF_SNR_COM_Sub,  //55
PQ_IP_SPF_SNR_MR_COM_Sub,  //56
PQ_IP_SPF_MR_LPF_COM_Sub,  //57
PQ_IP_SPF_NMR_Y_COM_Sub,  //58
PQ_IP_SPF_NMR_Y_MR_COM_Sub,  //59
PQ_IP_VSP_Y_COM_Sub,  //60
PQ_IP_VSP_C_COM_Sub,  //61
PQ_IP_VSP_CoRing_COM_Sub,  //62
PQ_IP_VSP_DeRing_COM_Sub,  //63
PQ_IP_VSP_Dither_COM_Sub,  //64
PQ_IP_VSP_PreVBound_COM_Sub,  //65
PQ_IP_422To444_COM_Sub,  //66
PQ_IP_PreCTI_COM_Sub,  //67
PQ_IP_HSP_Y_COM_Sub,  //68
PQ_IP_HSP_C_COM_Sub,  //69
PQ_IP_HSP_CoRing_COM_Sub,  //70
PQ_IP_HSP_DeRing_COM_Sub,  //71
PQ_IP_HSP_Dither_COM_Sub,  //72
PQ_IP_HnonLinear_COM_Sub,  //73
PQ_IP_SRAM1_COM_Sub,  //74
PQ_IP_SRAM2_COM_Sub,  //75
PQ_IP_SRAM3_COM_Sub,  //76
PQ_IP_SRAM4_COM_Sub,  //77
PQ_IP_C_SRAM1_COM_Sub,  //78
PQ_IP_C_SRAM2_COM_Sub,  //79
PQ_IP_C_SRAM3_COM_Sub,  //80
PQ_IP_C_SRAM4_COM_Sub,  //81
PQ_IP_VIP_COM_Sub,  //82
PQ_IP_VIP_pseudo_COM_Sub,  //83
PQ_IP_VIP_CSC_COM_Sub,  //84
PQ_IP_VIP_CSC_dither_COM_Sub,  //85
PQ_IP_VIP_Post_YC_delay_COM_Sub,  //86
PQ_IP_VIP_HNMR_Y_COM_Sub,  //87
PQ_IP_VIP_HNMR_ad_C_COM_Sub,  //88
PQ_IP_VIP_HNMR_ad_C_gain_COM_Sub,  //89
PQ_IP_VIP_HNMR_C_win1_COM_Sub,  //90
PQ_IP_VIP_HNMR_C_win2_COM_Sub,  //91
PQ_IP_VIP_HNMR_C_win3_COM_Sub,  //92
PQ_IP_VIP_HNMR_C_win4_COM_Sub,  //93
PQ_IP_VIP_Pre_Yoffset_COM_Sub,  //94
PQ_IP_VIP_Pre_Ygain_COM_Sub,  //95
PQ_IP_VIP_Pre_Ygain_dither_COM_Sub,  //96
PQ_IP_VIP_HLPF_COM_Sub,  //97
PQ_IP_VIP_HLPF_dither_COM_Sub,  //98
PQ_IP_VIP_Peaking_COM_Sub,  //99
PQ_IP_VIP_Peaking_band_COM_Sub,  //100
PQ_IP_VIP_Peaking_Pcoring_COM_Sub,  //101
PQ_IP_VIP_Peaking_gain_COM_Sub,  //102
PQ_IP_VIP_Peaking_gain_ad_C_COM_Sub,  //103
PQ_IP_VIP_Post_CTI_COM_Sub,  //104
PQ_IP_VIP_Post_CTI_coef_COM_Sub,  //105
PQ_IP_VIP_FCC_T1_COM_Sub,  //106
PQ_IP_VIP_FCC_T2_COM_Sub,  //107
PQ_IP_VIP_FCC_T3_COM_Sub,  //108
PQ_IP_VIP_FCC_T4_COM_Sub,  //109
PQ_IP_VIP_FCC_T5_COM_Sub,  //110
PQ_IP_VIP_FCC_T6_COM_Sub,  //111
PQ_IP_VIP_FCC_T7_COM_Sub,  //112
PQ_IP_VIP_FCC_T8_COM_Sub,  //113
PQ_IP_VIP_FCC_T9_COM_Sub,  //114
PQ_IP_VIP_IHC_COM_Sub,  //115
PQ_IP_VIP_IHC_R_COM_Sub,  //116
PQ_IP_VIP_IHC_G_COM_Sub,  //117
PQ_IP_VIP_IHC_B_COM_Sub,  //118
PQ_IP_VIP_IHC_C_COM_Sub,  //119
PQ_IP_VIP_IHC_M_COM_Sub,  //120
PQ_IP_VIP_IHC_Y_COM_Sub,  //121
PQ_IP_VIP_IHC_F_COM_Sub,  //122
PQ_IP_VIP_ICC_COM_Sub,  //123
PQ_IP_VIP_ICC_R_COM_Sub,  //124
PQ_IP_VIP_ICC_G_COM_Sub,  //125
PQ_IP_VIP_ICC_B_COM_Sub,  //126
PQ_IP_VIP_ICC_C_COM_Sub,  //127
PQ_IP_VIP_ICC_M_COM_Sub,  //128
PQ_IP_VIP_ICC_Y_COM_Sub,  //129
PQ_IP_VIP_ICC_F_COM_Sub,  //130
PQ_IP_VIP_ICC_NC_COM_Sub,  //131
PQ_IP_VIP_IBC_COM_Sub,  //132
PQ_IP_VIP_IBC_R_COM_Sub,  //133
PQ_IP_VIP_IBC_G_COM_Sub,  //134
PQ_IP_VIP_IBC_B_COM_Sub,  //135
PQ_IP_VIP_IBC_C_COM_Sub,  //136
PQ_IP_VIP_IBC_M_COM_Sub,  //137
PQ_IP_VIP_IBC_Y_COM_Sub,  //138
PQ_IP_VIP_IBC_F_COM_Sub,  //139
PQ_IP_VIP_DLC_COM_Sub,  //140
PQ_IP_VIP_DLC_dither_COM_Sub,  //141
PQ_IP_VIP_DLC_Flesh_adj_COM_Sub,  //142
PQ_IP_VIP_DLC_His_range_COM_Sub,  //143
PQ_IP_VIP_DLC_His_rangeH_COM_Sub,  //144
PQ_IP_VIP_DLC_His_rangeV_COM_Sub,  //145
PQ_IP_VIP_BLE_COM_Sub,  //146
PQ_IP_VIP_WLE_COM_Sub,  //147
PQ_IP_VIP_BWLE_dither_COM_Sub,  //148
PQ_IP_VIP_UVC_COM_Sub,  //149
PQ_IP_VIP_Post_Yoffset_COM_Sub,  //150
PQ_IP_VIP_Post_Ygain_COM_Sub,  //151
PQ_IP_VIP_Post_Cgain_COM_Sub,  //152
PQ_IP_VIP_Hcoring_Y_COM_Sub,  //153
PQ_IP_VIP_Hcoring_C_COM_Sub,  //154
PQ_IP_VIP_Hcoring_dither_COM_Sub,  //155
PQ_IP_SwDriver_COM_Sub,  //156
PQ_IP_3x3_COM_Sub,  //157
PQ_IP_RGB_Offset_COM_Sub,  //158
PQ_IP_RGB_Clip_COM_Sub,  //159
PQ_IP_xvYCC_bypass_COM_Sub,  //160
PQ_IP_xvYCC_de_gamma_COM_Sub,  //161
PQ_IP_xvYCC_de_gamma_dither_COM_Sub,  //162
PQ_IP_xvYCC_3x3_COM_Sub,  //163
PQ_IP_xvYCC_Comp_COM_Sub,  //164
PQ_IP_xvYCC_Comp_dither_COM_Sub,  //165
PQ_IP_xvYCC_Clip_COM_Sub,  //166
PQ_IP_xvYCC_gamma_COM_Sub,  //167
PQ_IP_xvYCC_gamma_dither_COM_Sub,  //168
PQ_IP_rgb_3d_COM_Sub,  //169
PQ_IP_HBC_COM_Sub,  //170
PQ_IP_Pre_CON_BRI_COM_Sub,  //171
PQ_IP_Blue_Stretch_COM_Sub,  //172
PQ_IP_Blue_Stretch_dither_COM_Sub,  //173
PQ_IP_Gamma_COM_Sub,  //174
PQ_IP_Gamma_dither_COM_Sub,  //175
PQ_IP_Post_CON_BRI_COM_Sub,  //176
PQ_IP_SRAM_3x3matrix_PIC2_COM_Sub,  //177
PQ_IP_SRAM_DLC_PIC1_COM_Sub,  //178
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_COM_Sub,  //179
PQ_IP_SRAM_GammaTbl_R_PIC1_COM_Sub,  //180
PQ_IP_SRAM_GammaTbl_G_PIC1_COM_Sub,  //181
PQ_IP_SRAM_GammaTbl_B_PIC1_COM_Sub,  //182
PQ_IP_SRAM_Auto_Color_PIC2_COM_Sub,  //183
PQ_IP_SRAM_Color_Temp_PIC1_COM_Sub,  //184
PQ_IP_COM_NUMS_Sub
} PQ_IP_COM_Group_Sub;

typedef enum
{
PQ_IP_AFEC_ADCIN_Sub,
PQ_IP_AFEC_RFIN_Sub,
PQ_IP_AFEC_ADCIN_SECAM_Sub,
PQ_IP_AFEC_NUMS_Sub
} PQ_IP_AFEC_Group_Sub;

typedef enum
{
PQ_IP_Comb_NTSC_AV_Sub,
PQ_IP_Comb_PAL_AV_Sub,
PQ_IP_Comb_NTSC_RF_Sub,
PQ_IP_Comb_PAL_RF_Sub,
PQ_IP_Comb_PAL_RF_VIF_Sub,
PQ_IP_Comb_NTSC_SV_Sub,
PQ_IP_Comb_PAL_SV_Sub,
PQ_IP_Comb_NTSC443_Sub,
PQ_IP_Comb_NTSC443_SV_Sub,
PQ_IP_Comb_PAL_M_Sub,
PQ_IP_Comb_PAL60_Sub,
PQ_IP_Comb_SECAM_Sub,
PQ_IP_Comb_NTSC_AV_SONY_Sub,
PQ_IP_Comb_PAL_AV_SONY_Sub,
PQ_IP_Comb_NTSC_RF_SONY_Sub,
PQ_IP_Comb_PAL_RF_SONY_Sub,
PQ_IP_Comb_NUMS_Sub
} PQ_IP_Comb_Group_Sub;

typedef enum
{
PQ_IP_Comb2_NTSC_AV_Sub,
PQ_IP_Comb2_PAL_AV_Sub,
PQ_IP_Comb2_NTSC_RF_Sub,
PQ_IP_Comb2_PAL_RF_Sub,
PQ_IP_Comb2_PAL_RF_VIF_Sub,
PQ_IP_Comb2_NTSC_SV_Sub,
PQ_IP_Comb2_PAL_SV_Sub,
PQ_IP_Comb2_NTSC443_Sub,
PQ_IP_Comb2_NTSC443_SV_Sub,
PQ_IP_Comb2_PAL_M_Sub,
PQ_IP_Comb2_PAL60_Sub,
PQ_IP_Comb2_SECAM_Sub,
PQ_IP_Comb2_NTSC_AV_SONY_Sub,
PQ_IP_Comb2_PAL_AV_SONY_Sub,
PQ_IP_Comb2_NTSC_RF_SONY_Sub,
PQ_IP_Comb2_PAL_RF_SONY_Sub,
PQ_IP_Comb2_NUMS_Sub
} PQ_IP_Comb2_Group_Sub;

typedef enum
{
PQ_IP_SECAM_ALL_Sub,
PQ_IP_SECAM_NUMS_Sub
} PQ_IP_SECAM_Group_Sub;

typedef enum
{
PQ_IP_VD_Sampling_no_comm_N_1135_Sub,
PQ_IP_VD_Sampling_no_comm_P_1135_Sub,
PQ_IP_VD_Sampling_no_comm_P6_1135_Sub,
PQ_IP_VD_Sampling_no_comm_S_1135_Sub,
PQ_IP_VD_Sampling_no_comm_N4_1135_Sub,
PQ_IP_VD_Sampling_no_comm_PM_1135_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_1135_Sub,
PQ_IP_VD_Sampling_no_comm_N_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_P_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_P6_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_S_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_N4_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_PM_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_1135_1o5_Sub,
PQ_IP_VD_Sampling_no_comm_N_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_P_Dynmiac_Sub,
PQ_IP_VD_Sampling_no_comm_P6_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_S_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_N4_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_PM_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_Dynamic_Sub,
PQ_IP_VD_Sampling_no_comm_NUMS_Sub
} PQ_IP_VD_Sampling_no_comm_Group_Sub;

typedef enum
{
PQ_IP_SCinit_Init_Sub,
PQ_IP_SCinit_NUMS_Sub
} PQ_IP_SCinit_Group_Sub;

typedef enum
{
PQ_IP_CSC_OFF_Sub,
PQ_IP_CSC_L_RGB2YCC_SD_Sub,
PQ_IP_CSC_F_RGB2YCC_SD_Sub,
PQ_IP_CSC_L_RGB2YCC_HD_Sub,
PQ_IP_CSC_F_RGB2YCC_HD_Sub,
PQ_IP_CSC_NUMS_Sub
} PQ_IP_CSC_Group_Sub;

typedef enum
{
PQ_IP_CSC_Dither_OFF_Sub,
PQ_IP_CSC_Dither_ON_Sub,
PQ_IP_CSC_Dither_NUMS_Sub
} PQ_IP_CSC_Dither_Group_Sub;

typedef enum
{
PQ_IP_YCdelay_OFF_Sub,
PQ_IP_YCdelay_YC_91_Sub,
PQ_IP_YCdelay_YC_92_Sub,
PQ_IP_YCdelay_NUMS_Sub
} PQ_IP_YCdelay_Group_Sub;

typedef enum
{
PQ_IP_PreFilter_OFF_Sub,
PQ_IP_PreFilter_NUMS_Sub
} PQ_IP_PreFilter_Group_Sub;

typedef enum
{
PQ_IP_PreFilter_Dither_OFF_Sub,
PQ_IP_PreFilter_Dither_NUMS_Sub
} PQ_IP_PreFilter_Dither_Group_Sub;

typedef enum
{
PQ_IP_HDSDD_OFF_Sub,
PQ_IP_HDSDD_NUMS_Sub
} PQ_IP_HDSDD_Group_Sub;

typedef enum
{
PQ_IP_HSD_Y_OFF_Sub,
PQ_IP_HSD_Y_CB_Sub,
PQ_IP_HSD_Y_LPF40Ap0As40_Sub,
PQ_IP_HSD_Y_LPF50Ap0As40_Sub,
PQ_IP_HSD_Y_LPF60Ap0As35_Sub,
PQ_IP_HSD_Y_LPF70Ap0As35_Sub,
PQ_IP_HSD_Y_LPF80Ap0As35_Sub,
PQ_IP_HSD_Y_LPF90Ap0As35_Sub,
PQ_IP_HSD_Y_ALLPASS1X_Sub,
PQ_IP_HSD_Y_FIR66_Sub,
PQ_IP_HSD_Y_FIR60_Sub,
PQ_IP_HSD_Y_FIR55_Sub,
PQ_IP_HSD_Y_FIR50_Sub,
PQ_IP_HSD_Y_NUMS_Sub
} PQ_IP_HSD_Y_Group_Sub;

typedef enum
{
PQ_IP_HSD_C_OFF_Sub,
PQ_IP_HSD_C_CB_Sub,
PQ_IP_HSD_C_HSDC_A_Sub,
PQ_IP_HSD_C_HSDC_B_Sub,
PQ_IP_HSD_C_ALLPASS1X_Sub,
PQ_IP_HSD_C_FIR50_Sub,
PQ_IP_HSD_C_FIR60_Sub,
PQ_IP_HSD_C_NUMS_Sub
} PQ_IP_HSD_C_Group_Sub;

typedef enum
{
PQ_IP_444To422_OFF_Sub,
PQ_IP_444To422_ON_Sub,
PQ_IP_444To422_NUMS_Sub
} PQ_IP_444To422_Group_Sub;

typedef enum
{
PQ_IP_VSD_OFF_Sub,
PQ_IP_VSD_CB_Sub,
PQ_IP_VSD_Bilinear_Sub,
PQ_IP_VSD_NUMS_Sub
} PQ_IP_VSD_Group_Sub;

typedef enum
{
PQ_IP_HVSD_Dither_OFF_Sub,
PQ_IP_HVSD_Dither_Vdith_Sub,
PQ_IP_HVSD_Dither_Hdith_Sub,
PQ_IP_HVSD_Dither_HVDith_Sub,
PQ_IP_HVSD_Dither_NUMS_Sub
} PQ_IP_HVSD_Dither_Group_Sub;

typedef enum
{
PQ_IP_10to8_Dither_OFF_Sub,
PQ_IP_10to8_Dither_Rand_Dith_Sub,
PQ_IP_10to8_Dither_Fix_Dith_Sub,
PQ_IP_10to8_Dither_NUMS_Sub
} PQ_IP_10to8_Dither_Group_Sub;

typedef enum
{
PQ_IP_MemFormat_422MF_Sub,
PQ_IP_MemFormat_444_10BIT_Sub,
PQ_IP_MemFormat_444_8BIT_Sub,
PQ_IP_MemFormat_NUMS_Sub
} PQ_IP_MemFormat_Group_Sub;

typedef enum
{
PQ_IP_PreSNR_OFF_Sub,
PQ_IP_PreSNR_NUMS_Sub
} PQ_IP_PreSNR_Group_Sub;

typedef enum
{
PQ_IP_PreSNR_Patch_OFF_Sub,
PQ_IP_PreSNR_Patch_NUMS_Sub
} PQ_IP_PreSNR_Patch_Group_Sub;

typedef enum
{
PQ_IP_DNR_NR_OFF_Sub,
PQ_IP_DNR_DNR_OFF_Sub,
PQ_IP_DNR_ON_Sub,
PQ_IP_DNR_NUMS_Sub
} PQ_IP_DNR_Group_Sub;

typedef enum
{
PQ_IP_DNR_Motion_MR_NR_Sub,
PQ_IP_DNR_Motion_MR_nonNR_Sub,
PQ_IP_DNR_Motion_NUMS_Sub
} PQ_IP_DNR_Motion_Group_Sub;

typedef enum
{
PQ_IP_DNR_Y_OFF_Sub,
PQ_IP_DNR_Y_DY_1_Sub,
PQ_IP_DNR_Y_DY_1_1_Sub,
PQ_IP_DNR_Y_DY_2_Sub,
PQ_IP_DNR_Y_DY_2_1_Sub,
PQ_IP_DNR_Y_DY_3_Sub,
PQ_IP_DNR_Y_DY_3_1_Sub,
PQ_IP_DNR_Y_DY_4_Sub,
PQ_IP_DNR_Y_DY_4_1_Sub,
PQ_IP_DNR_Y_DY_5_Sub,
PQ_IP_DNR_Y_DY_5_1_Sub,
PQ_IP_DNR_Y_DY_6_Sub,
PQ_IP_DNR_Y_DY_6_1_Sub,
PQ_IP_DNR_Y_DY_7_Sub,
PQ_IP_DNR_Y_DY_7_1_Sub,
PQ_IP_DNR_Y_DY_8_Sub,
PQ_IP_DNR_Y_DY_8_1_Sub,
PQ_IP_DNR_Y_DY_9_Sub,
PQ_IP_DNR_Y_DY_9_1_Sub,
PQ_IP_DNR_Y_DY_10_Sub,
PQ_IP_DNR_Y_DY_10_1_Sub,
PQ_IP_DNR_Y_NUMS_Sub
} PQ_IP_DNR_Y_Group_Sub;

typedef enum
{
PQ_IP_DNR_Y_COLOR_DEP_OFF_Sub,
PQ_IP_DNR_Y_COLOR_DEP_NUMS_Sub
} PQ_IP_DNR_Y_COLOR_DEP_Group_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_INDEX_OFF_Sub,
PQ_IP_SRAM_COLOR_INDEX_Index0_Sub,
PQ_IP_SRAM_COLOR_INDEX_NUMS_Sub
} PQ_IP_SRAM_COLOR_INDEX_Group_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_SNR_OFF_Sub,
PQ_IP_SRAM_COLOR_GAIN_SNR_GainSNR0_Sub,
PQ_IP_SRAM_COLOR_GAIN_SNR_NUMS_Sub
} PQ_IP_SRAM_COLOR_GAIN_SNR_Group_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_DNR_OFF_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR0_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR1_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_NUMS_Sub
} PQ_IP_SRAM_COLOR_GAIN_DNR_Group_Sub;

typedef enum
{
PQ_IP_DNR_Y_LUMA_ADAPTIVE_OFF_Sub,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_NUMS_Sub
} PQ_IP_DNR_Y_LUMA_ADAPTIVE_Group_Sub;

typedef enum
{
PQ_IP_DNR_POSTTUNE_OFF_Sub,
PQ_IP_DNR_POSTTUNE_NUMS_Sub
} PQ_IP_DNR_POSTTUNE_Group_Sub;

typedef enum
{
PQ_IP_DNR_C_OFF_Sub,
PQ_IP_DNR_C_DC_1_Sub,
PQ_IP_DNR_C_DC_1_1_Sub,
PQ_IP_DNR_C_DC_2_Sub,
PQ_IP_DNR_C_DC_2_1_Sub,
PQ_IP_DNR_C_DC_3_Sub,
PQ_IP_DNR_C_DC_3_1_Sub,
PQ_IP_DNR_C_DC_4_Sub,
PQ_IP_DNR_C_DC_4_1_Sub,
PQ_IP_DNR_C_DC_5_Sub,
PQ_IP_DNR_C_DC_5_1_Sub,
PQ_IP_DNR_C_DC_6_Sub,
PQ_IP_DNR_C_DC_6_1_Sub,
PQ_IP_DNR_C_DC_7_Sub,
PQ_IP_DNR_C_DC_7_1_Sub,
PQ_IP_DNR_C_DC_8_Sub,
PQ_IP_DNR_C_DC_8_1_Sub,
PQ_IP_DNR_C_DC_9_Sub,
PQ_IP_DNR_C_DC_9_1_Sub,
PQ_IP_DNR_C_DC_10_Sub,
PQ_IP_DNR_C_DC_10_1_Sub,
PQ_IP_DNR_C_NUMS_Sub
} PQ_IP_DNR_C_Group_Sub;

typedef enum
{
PQ_IP_HISDNR_OFF_Sub,
PQ_IP_HISDNR_NUMS_Sub
} PQ_IP_HISDNR_Group_Sub;

typedef enum
{
PQ_IP_PNR_OFF_Sub,
PQ_IP_PNR_PNR_PCCS_AVG_OFF_Sub,
PQ_IP_PNR_ON_Sub,
PQ_IP_PNR_AVG_ON_Sub,
PQ_IP_PNR_NUMS_Sub
} PQ_IP_PNR_Group_Sub;

typedef enum
{
PQ_IP_PNR_Y_OFF_Sub,
PQ_IP_PNR_Y_PY1_Sub,
PQ_IP_PNR_Y_NUMS_Sub
} PQ_IP_PNR_Y_Group_Sub;

typedef enum
{
PQ_IP_PNR_C_OFF_Sub,
PQ_IP_PNR_C_PC1_Sub,
PQ_IP_PNR_C_NUMS_Sub
} PQ_IP_PNR_C_Group_Sub;

typedef enum
{
PQ_IP_PostCCS_OFF_Sub,
PQ_IP_PostCCS_PC_0_Sub,
PQ_IP_PostCCS_PC_1_Sub,
PQ_IP_PostCCS_PC_2_Sub,
PQ_IP_PostCCS_PC_3_Sub,
PQ_IP_PostCCS_PC_4_Sub,
PQ_IP_PostCCS_PC_5_Sub,
PQ_IP_PostCCS_PC_6_Sub,
PQ_IP_PostCCS_PC_7_Sub,
PQ_IP_PostCCS_PC_8_Sub,
PQ_IP_PostCCS_PC_9_Sub,
PQ_IP_PostCCS_PC_10_Sub,
PQ_IP_PostCCS_PC_11_Sub,
PQ_IP_PostCCS_PC_12_Sub,
PQ_IP_PostCCS_PC_13_Sub,
PQ_IP_PostCCS_PC_14_Sub,
PQ_IP_PostCCS_PC_15_Sub,
PQ_IP_PostCCS_PC_16_Sub,
PQ_IP_PostCCS_PC_17_Sub,
PQ_IP_PostCCS_PC_18_Sub,
PQ_IP_PostCCS_PC_19_Sub,
PQ_IP_PostCCS_PC_20_Sub,
PQ_IP_PostCCS_PC_21_Sub,
PQ_IP_PostCCS_PC_22_Sub,
PQ_IP_PostCCS_PC_23_Sub,
PQ_IP_PostCCS_PC_24_Sub,
PQ_IP_PostCCS_NUMS_Sub
} PQ_IP_PostCCS_Group_Sub;

typedef enum
{
PQ_IP_PostCCS_Smooth_OFF_Sub,
PQ_IP_PostCCS_Smooth_PCS_0_Sub,
PQ_IP_PostCCS_Smooth_PCS_1_Sub,
PQ_IP_PostCCS_Smooth_PCS_2_Sub,
PQ_IP_PostCCS_Smooth_PCS_3_Sub,
PQ_IP_PostCCS_Smooth_PCS_4_Sub,
PQ_IP_PostCCS_Smooth_PCS_5_Sub,
PQ_IP_PostCCS_Smooth_PCS_6_Sub,
PQ_IP_PostCCS_Smooth_PCS_7_Sub,
PQ_IP_PostCCS_Smooth_NUMS_Sub
} PQ_IP_PostCCS_Smooth_Group_Sub;

typedef enum
{
PQ_IP_420CUP_OFF_Sub,
PQ_IP_420CUP_ON_Sub,
PQ_IP_420CUP_NUMS_Sub
} PQ_IP_420CUP_Group_Sub;

typedef enum
{
PQ_IP_MADi_24_4R_Sub,
PQ_IP_MADi_24_2R_Sub,
PQ_IP_MADi_25_4R_Sub,
PQ_IP_MADi_25_2R_Sub,
PQ_IP_MADi_27_4R_Sub,
PQ_IP_MADi_27_2R_Sub,
PQ_IP_MADi_P_MODE8_Sub,
PQ_IP_MADi_P_MODE10_Sub,
PQ_IP_MADi_P_MODE_MOT10_Sub,
PQ_IP_MADi_P_MODE_MOT8_Sub,
PQ_IP_MADi_NUMS_Sub
} PQ_IP_MADi_Group_Sub;

typedef enum
{
PQ_IP_MADi_Motion_MOT_4R_5_Sub,
PQ_IP_MADi_Motion_MOT_4R_6_Sub,
PQ_IP_MADi_Motion_MOT_4R_7_Sub,
PQ_IP_MADi_Motion_MOT_2R_Sub,
PQ_IP_MADi_Motion_MOT_PMODE_Sub,
PQ_IP_MADi_Motion_NUMS_Sub
} PQ_IP_MADi_Motion_Group_Sub;

typedef enum
{
PQ_IP_MADi_ADP3x3_OFF_Sub,
PQ_IP_MADi_ADP3x3_ADP1_Sub,
PQ_IP_MADi_ADP3x3_NUMS_Sub
} PQ_IP_MADi_ADP3x3_Group_Sub;

typedef enum
{
PQ_IP_MADi_MORPHO_OFF_Sub,
PQ_IP_MADi_MORPHO_M1_Sub,
PQ_IP_MADi_MORPHO_NUMS_Sub
} PQ_IP_MADi_MORPHO_Group_Sub;

typedef enum
{
PQ_IP_MADi_DFK_OFF_Sub,
PQ_IP_MADi_DFK_DFK1_Sub,
PQ_IP_MADi_DFK_DFK2_Sub,
PQ_IP_MADi_DFK_DFK3_Sub,
PQ_IP_MADi_DFK_DFK4_Sub,
PQ_IP_MADi_DFK_DFK5_Sub,
PQ_IP_MADi_DFK_DFK6_Sub,
PQ_IP_MADi_DFK_NUMS_Sub
} PQ_IP_MADi_DFK_Group_Sub;

typedef enum
{
PQ_IP_MADi_SST_OFF_Sub,
PQ_IP_MADi_SST_SST1_Sub,
PQ_IP_MADi_SST_SST2_Sub,
PQ_IP_MADi_SST_SST3_Sub,
PQ_IP_MADi_SST_SST4_Sub,
PQ_IP_MADi_SST_SST5_Sub,
PQ_IP_MADi_SST_SST6_Sub,
PQ_IP_MADi_SST_SST_Rec1_Sub,
PQ_IP_MADi_SST_NUMS_Sub
} PQ_IP_MADi_SST_Group_Sub;

typedef enum
{
PQ_IP_MADi_EODiW_OFF_Sub,
PQ_IP_MADi_EODiW_W1_Sub,
PQ_IP_MADi_EODiW_W2_Sub,
PQ_IP_MADi_EODiW_W3_Sub,
PQ_IP_MADi_EODiW_NUMS_Sub
} PQ_IP_MADi_EODiW_Group_Sub;

typedef enum
{
PQ_IP_MADi_Force_OFF_Sub,
PQ_IP_MADi_Force_YC_FullMotion_Sub,
PQ_IP_MADi_Force_YC_FullStill_Sub,
PQ_IP_MADi_Force_Y_FullMotion_Sub,
PQ_IP_MADi_Force_Y_FullStill_Sub,
PQ_IP_MADi_Force_C_FullMotion_Sub,
PQ_IP_MADi_Force_C_FullStill_Sub,
PQ_IP_MADi_Force_NUMS_Sub
} PQ_IP_MADi_Force_Group_Sub;

typedef enum
{
PQ_IP_EODi_OFF_Sub,
PQ_IP_EODi_SD_15_Sub,
PQ_IP_EODi_SD_14_Sub,
PQ_IP_EODi_SD_13_Sub,
PQ_IP_EODi_SD_12_Sub,
PQ_IP_EODi_SD_11_Sub,
PQ_IP_EODi_SD_10_Sub,
PQ_IP_EODi_SD_9_Sub,
PQ_IP_EODi_SD_8_Sub,
PQ_IP_EODi_SD_7_Sub,
PQ_IP_EODi_SD_6_Sub,
PQ_IP_EODi_SD_5_Sub,
PQ_IP_EODi_SD_4_Sub,
PQ_IP_EODi_SD_3_Sub,
PQ_IP_EODi_SD_2_Sub,
PQ_IP_EODi_SD_1_Sub,
PQ_IP_EODi_HD_15_Sub,
PQ_IP_EODi_HD_14_Sub,
PQ_IP_EODi_HD_13_Sub,
PQ_IP_EODi_HD_12_Sub,
PQ_IP_EODi_HD_11_Sub,
PQ_IP_EODi_HD_10_Sub,
PQ_IP_EODi_HD_9_Sub,
PQ_IP_EODi_HD_8_Sub,
PQ_IP_EODi_HD_7_Sub,
PQ_IP_EODi_HD_6_Sub,
PQ_IP_EODi_HD_5_Sub,
PQ_IP_EODi_HD_4_Sub,
PQ_IP_EODi_HD_3_Sub,
PQ_IP_EODi_HD_2_Sub,
PQ_IP_EODi_HD_1_Sub,
PQ_IP_EODi_NUMS_Sub
} PQ_IP_EODi_Group_Sub;

typedef enum
{
PQ_IP_Film_OFF_Sub,
PQ_IP_Film_SD_2_Sub,
PQ_IP_Film_SD_1_Sub,
PQ_IP_Film_SD_3_Sub,
PQ_IP_Film_HD_2_Sub,
PQ_IP_Film_HD_1_Sub,
PQ_IP_Film_HD_3_Sub,
PQ_IP_Film_NUMS_Sub
} PQ_IP_Film_Group_Sub;

typedef enum
{
PQ_IP_Film32_OFF_Sub,
PQ_IP_Film32_SD_1_Sub,
PQ_IP_Film32_SD_2_Sub,
PQ_IP_Film32_SD_3_Sub,
PQ_IP_Film32_SD_4_Sub,
PQ_IP_Film32_SD_5_Sub,
PQ_IP_Film32_SD_6_Sub,
PQ_IP_Film32_SD_7_Sub,
PQ_IP_Film32_SD_8_Sub,
PQ_IP_Film32_SD_9_Sub,
PQ_IP_Film32_SD_10_Sub,
PQ_IP_Film32_HD_1_Sub,
PQ_IP_Film32_HD_2_Sub,
PQ_IP_Film32_HD_3_Sub,
PQ_IP_Film32_HD_4_Sub,
PQ_IP_Film32_HD_5_Sub,
PQ_IP_Film32_HD_6_Sub,
PQ_IP_Film32_NUMS_Sub
} PQ_IP_Film32_Group_Sub;

typedef enum
{
PQ_IP_Film22_OFF_Sub,
PQ_IP_Film22_SD_1_Sub,
PQ_IP_Film22_SD_2_Sub,
PQ_IP_Film22_SD_3_Sub,
PQ_IP_Film22_SD_4_Sub,
PQ_IP_Film22_SD_5_Sub,
PQ_IP_Film22_SD_6_Sub,
PQ_IP_Film22_SD_7_Sub,
PQ_IP_Film22_SD_8_Sub,
PQ_IP_Film22_SD_9_Sub,
PQ_IP_Film22_SD_10_Sub,
PQ_IP_Film22_HD_1_Sub,
PQ_IP_Film22_HD_2_Sub,
PQ_IP_Film22_HD_3_Sub,
PQ_IP_Film22_HD_4_Sub,
PQ_IP_Film22_HD_5_Sub,
PQ_IP_Film22_HD_6_Sub,
PQ_IP_Film22_NUMS_Sub
} PQ_IP_Film22_Group_Sub;

typedef enum
{
PQ_IP_Film_any_OFF_Sub,
PQ_IP_Film_any_SD_1_Sub,
PQ_IP_Film_any_SD_2_Sub,
PQ_IP_Film_any_SD_3_Sub,
PQ_IP_Film_any_SD_4_Sub,
PQ_IP_Film_any_SD_5_Sub,
PQ_IP_Film_any_SD_6_Sub,
PQ_IP_Film_any_SD_7_Sub,
PQ_IP_Film_any_SD_8_Sub,
PQ_IP_Film_any_SD_9_Sub,
PQ_IP_Film_any_SD_10_Sub,
PQ_IP_Film_any_HD_1_Sub,
PQ_IP_Film_any_HD_2_Sub,
PQ_IP_Film_any_HD_3_Sub,
PQ_IP_Film_any_HD_4_Sub,
PQ_IP_Film_any_HD_5_Sub,
PQ_IP_Film_any_HD_6_Sub,
PQ_IP_Film_any_NUMS_Sub
} PQ_IP_Film_any_Group_Sub;

typedef enum
{
PQ_IP_DIPF_OFF_Sub,
PQ_IP_DIPF_DIPF1_Sub,
PQ_IP_DIPF_NUMS_Sub
} PQ_IP_DIPF_Group_Sub;

typedef enum
{
PQ_IP_VCLPF_OFF_Sub,
PQ_IP_VCLPF_ON_Sub,
PQ_IP_VCLPF_NUMS_Sub
} PQ_IP_VCLPF_Group_Sub;

typedef enum
{
PQ_IP_Spike_NR_OFF_Sub,
PQ_IP_Spike_NR_S1_Sub,
PQ_IP_Spike_NR_S2_Sub,
PQ_IP_Spike_NR_S3_Sub,
PQ_IP_Spike_NR_NUMS_Sub
} PQ_IP_Spike_NR_Group_Sub;

typedef enum
{
PQ_IP_SPF_OFF_Sub,
PQ_IP_SPF_NUMS_Sub
} PQ_IP_SPF_Group_Sub;

typedef enum
{
PQ_IP_SPF_SNR_OFF_Sub,
PQ_IP_SPF_SNR_E1S1_Sub,
PQ_IP_SPF_SNR_E1S2_Sub,
PQ_IP_SPF_SNR_E2S1_Sub,
PQ_IP_SPF_SNR_E3S1_Sub,
PQ_IP_SPF_SNR_E4S1_Sub,
PQ_IP_SPF_SNR_E4S2_Sub,
PQ_IP_SPF_SNR_E4S3_Sub,
PQ_IP_SPF_SNR_NUMS_Sub
} PQ_IP_SPF_SNR_Group_Sub;

typedef enum
{
PQ_IP_SPF_SNR_MR_OFF_Sub,
PQ_IP_SPF_SNR_MR_ON_Sub,
PQ_IP_SPF_SNR_MR_NUMS_Sub
} PQ_IP_SPF_SNR_MR_Group_Sub;

typedef enum
{
PQ_IP_SPF_MR_LPF_OFF_Sub,
PQ_IP_SPF_MR_LPF_LPF3x3_Sub,
PQ_IP_SPF_MR_LPF_NUMS_Sub
} PQ_IP_SPF_MR_LPF_Group_Sub;

typedef enum
{
PQ_IP_SPF_NMR_Y_OFF_Sub,
PQ_IP_SPF_NMR_Y_S1_Sub,
PQ_IP_SPF_NMR_Y_S2_Sub,
PQ_IP_SPF_NMR_Y_S3_Sub,
PQ_IP_SPF_NMR_Y_NUMS_Sub
} PQ_IP_SPF_NMR_Y_Group_Sub;

typedef enum
{
PQ_IP_SPF_NMR_Y_MR_OFF_Sub,
PQ_IP_SPF_NMR_Y_MR_ON_Sub,
PQ_IP_SPF_NMR_Y_MR_NUMS_Sub
} PQ_IP_SPF_NMR_Y_MR_Group_Sub;

typedef enum
{
PQ_IP_VSP_Y_Bypass_Sub,
PQ_IP_VSP_Y_Bilinear_Sub,
PQ_IP_VSP_Y_SRAM_3_4Tap_Sub,
PQ_IP_VSP_Y_SRAM_4_4Tap_Sub,
PQ_IP_VSP_Y_SRAM_3_6Tap_Sub,
PQ_IP_VSP_Y_SRAM_4_6Tap_Sub,
PQ_IP_VSP_Y_NUMS_Sub
} PQ_IP_VSP_Y_Group_Sub;

typedef enum
{
PQ_IP_VSP_C_Bypass_Sub,
PQ_IP_VSP_C_Bilinear_Sub,
PQ_IP_VSP_C_C_SRAM_1_Sub,
PQ_IP_VSP_C_C_SRAM_2_Sub,
PQ_IP_VSP_C_C_SRAM_3_Sub,
PQ_IP_VSP_C_C_SRAM_4_Sub,
PQ_IP_VSP_C_SRAM_3_4Tap_Sub,
PQ_IP_VSP_C_SRAM_4_4Tap_Sub,
PQ_IP_VSP_C_NUMS_Sub
} PQ_IP_VSP_C_Group_Sub;

typedef enum
{
PQ_IP_VSP_CoRing_OFF_Sub,
PQ_IP_VSP_CoRing_Y_Coring_1_Sub,
PQ_IP_VSP_CoRing_Y_Coring_2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_3_Sub,
PQ_IP_VSP_CoRing_Y_Coring_4_Sub,
PQ_IP_VSP_CoRing_Y_Coring_5_Sub,
PQ_IP_VSP_CoRing_NUMS_Sub
} PQ_IP_VSP_CoRing_Group_Sub;

typedef enum
{
PQ_IP_VSP_DeRing_OFF_Sub,
PQ_IP_VSP_DeRing_DR1_Sub,
PQ_IP_VSP_DeRing_NUMS_Sub
} PQ_IP_VSP_DeRing_Group_Sub;

typedef enum
{
PQ_IP_VSP_Dither_OFF_Sub,
PQ_IP_VSP_Dither_ON_Sub,
PQ_IP_VSP_Dither_NUMS_Sub
} PQ_IP_VSP_Dither_Group_Sub;

typedef enum
{
PQ_IP_VSP_PreVBound_OFF_Sub,
PQ_IP_VSP_PreVBound_ON_Sub,
PQ_IP_VSP_PreVBound_NUMS_Sub
} PQ_IP_VSP_PreVBound_Group_Sub;

typedef enum
{
PQ_IP_422To444_ON_Sub,
PQ_IP_422To444_OFF_Sub,
PQ_IP_422To444_NUMS_Sub
} PQ_IP_422To444_Group_Sub;

typedef enum
{
PQ_IP_PreCTI_OFF_Sub,
PQ_IP_PreCTI_CTI_0_Sub,
PQ_IP_PreCTI_CTI_1_Sub,
PQ_IP_PreCTI_NUMS_Sub
} PQ_IP_PreCTI_Group_Sub;

typedef enum
{
PQ_IP_HSP_Y_Bypass_Sub,
PQ_IP_HSP_Y_Bilinear_Sub,
PQ_IP_HSP_Y_SRAM_3_4Tap_Sub,
PQ_IP_HSP_Y_SRAM_4_4Tap_Sub,
PQ_IP_HSP_Y_SRAM_3_6Tap_Sub,
PQ_IP_HSP_Y_SRAM_4_6Tap_Sub,
PQ_IP_HSP_Y_NUMS_Sub
} PQ_IP_HSP_Y_Group_Sub;

typedef enum
{
PQ_IP_HSP_C_Bypass_Sub,
PQ_IP_HSP_C_Bilinear_Sub,
PQ_IP_HSP_C_C_SRAM_1_Sub,
PQ_IP_HSP_C_C_SRAM_2_Sub,
PQ_IP_HSP_C_C_SRAM_3_Sub,
PQ_IP_HSP_C_C_SRAM_4_Sub,
PQ_IP_HSP_C_SRAM_3_4Tap_Sub,
PQ_IP_HSP_C_SRAM_4_4Tap_Sub,
PQ_IP_HSP_C_NUMS_Sub
} PQ_IP_HSP_C_Group_Sub;

typedef enum
{
PQ_IP_HSP_CoRing_OFF_Sub,
PQ_IP_HSP_CoRing_Y_Coring_1_Sub,
PQ_IP_HSP_CoRing_Y_Coring_2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_3_Sub,
PQ_IP_HSP_CoRing_Y_Coring_4_Sub,
PQ_IP_HSP_CoRing_Y_Coring_5_Sub,
PQ_IP_HSP_CoRing_NUMS_Sub
} PQ_IP_HSP_CoRing_Group_Sub;

typedef enum
{
PQ_IP_HSP_DeRing_OFF_Sub,
PQ_IP_HSP_DeRing_DR1_Sub,
PQ_IP_HSP_DeRing_NUMS_Sub
} PQ_IP_HSP_DeRing_Group_Sub;

typedef enum
{
PQ_IP_HSP_Dither_OFF_Sub,
PQ_IP_HSP_Dither_ON_Sub,
PQ_IP_HSP_Dither_NUMS_Sub
} PQ_IP_HSP_Dither_Group_Sub;

typedef enum
{
PQ_IP_HnonLinear_OFF_Sub,
PQ_IP_HnonLinear_H_1366_0_Sub,
PQ_IP_HnonLinear_H_1366_1_Sub,
PQ_IP_HnonLinear_H_1366_2_Sub,
PQ_IP_HnonLinear_H_1440_Sub,
PQ_IP_HnonLinear_H_1680_Sub,
PQ_IP_HnonLinear_H_1920_0_Sub,
PQ_IP_HnonLinear_H_1920_1_Sub,
PQ_IP_HnonLinear_H_1920_2_Sub,
PQ_IP_HnonLinear_NUMS_Sub
} PQ_IP_HnonLinear_Group_Sub;

typedef enum
{
PQ_IP_SRAM1_InvSinc6Tc4p4Fc57Apass01Astop60_Sub,
PQ_IP_SRAM1_InvSinc6Tc4p4Fc67Apass01Astop60_Sub,
PQ_IP_SRAM1_InvSinc6Tc4p4Fc77Apass0001Astop40_Sub,
PQ_IP_SRAM1_InvSinc6Tc4p4Fc81Apass01Astop40_Sub,
PQ_IP_SRAM1_InvSinc6Tc4p4Fc87Apass01Astop50_Sub,
PQ_IP_SRAM1_InvSinc6Tc4p4Fc94Apass01Astop60_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc50Apass01Astop55_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc60Apass0001Astop40_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc65Apass0001Astop40_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc75Apass0001Astop40_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Apass01Astop50_Sub,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc90Apass0001Astop40_Sub,
PQ_IP_SRAM1_NUMS_Sub
} PQ_IP_SRAM1_Group_Sub;

typedef enum
{
PQ_IP_SRAM2_InvSinc6Tc4p4Fc57Apass01Astop60_Sub,
PQ_IP_SRAM2_InvSinc6Tc4p4Fc67Apass01Astop60_Sub,
PQ_IP_SRAM2_InvSinc6Tc4p4Fc77Apass0001Astop40_Sub,
PQ_IP_SRAM2_InvSinc6Tc4p4Fc81Apass01Astop40_Sub,
PQ_IP_SRAM2_InvSinc6Tc4p4Fc87Apass01Astop50_Sub,
PQ_IP_SRAM2_InvSinc6Tc4p4Fc94Apass01Astop60_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc50Apass01Astop55_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc60Apass0001Astop40_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc65Apass0001Astop40_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc75Apass0001Astop40_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Apass01Astop50_Sub,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc90Apass0001Astop40_Sub,
PQ_IP_SRAM2_NUMS_Sub
} PQ_IP_SRAM2_Group_Sub;

typedef enum
{
PQ_IP_SRAM3_InvSinc4Tc4p4Fc45Apass01Astop40_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc50Apass01Astop55_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc65Apass3Astop60G14_Sub,
PQ_IP_SRAM3_InvSinc3Tc0p0Fc75Ap001As60G13_Sub,
PQ_IP_SRAM3_InvSinc3Tc0p0Fc75Ap001As60_Sub,
PQ_IP_SRAM3_NUMS_Sub
} PQ_IP_SRAM3_Group_Sub;

typedef enum
{
PQ_IP_SRAM4_InvSinc4Tc4p4Fc45Apass01Astop40_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc50Apass01Astop55_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc65Apass3Astop60G14_Sub,
PQ_IP_SRAM4_InvSinc3Tc0p0Fc75Ap001As60G13_Sub,
PQ_IP_SRAM4_InvSinc3Tc0p0Fc75Ap001As60_Sub,
PQ_IP_SRAM4_NUMS_Sub
} PQ_IP_SRAM4_Group_Sub;

typedef enum
{
PQ_IP_C_SRAM1_C2121_Sub,
PQ_IP_C_SRAM1_C121_Sub,
PQ_IP_C_SRAM1_NUMS_Sub
} PQ_IP_C_SRAM1_Group_Sub;

typedef enum
{
PQ_IP_C_SRAM2_C2121_Sub,
PQ_IP_C_SRAM2_C121_Sub,
PQ_IP_C_SRAM2_NUMS_Sub
} PQ_IP_C_SRAM2_Group_Sub;

typedef enum
{
PQ_IP_C_SRAM3_C2121_Sub,
PQ_IP_C_SRAM3_C121_Sub,
PQ_IP_C_SRAM3_NUMS_Sub
} PQ_IP_C_SRAM3_Group_Sub;

typedef enum
{
PQ_IP_C_SRAM4_C2121_Sub,
PQ_IP_C_SRAM4_C121_Sub,
PQ_IP_C_SRAM4_NUMS_Sub
} PQ_IP_C_SRAM4_Group_Sub;

typedef enum
{
PQ_IP_VIP_OFF_Sub,
PQ_IP_VIP_NUMS_Sub
} PQ_IP_VIP_Group_Sub;

typedef enum
{
PQ_IP_VIP_pseudo_OFF_Sub,
PQ_IP_VIP_pseudo_NUMS_Sub
} PQ_IP_VIP_pseudo_Group_Sub;

typedef enum
{
PQ_IP_VIP_CSC_OFF_Sub,
PQ_IP_VIP_CSC_L_RGB2YCC_SD_Sub,
PQ_IP_VIP_CSC_F_RGB2YCC_SD_Sub,
PQ_IP_VIP_CSC_L_RGB2YCC_HD_Sub,
PQ_IP_VIP_CSC_F_RGB2YCC_HD_Sub,
PQ_IP_VIP_CSC_NUMS_Sub
} PQ_IP_VIP_CSC_Group_Sub;

typedef enum
{
PQ_IP_VIP_CSC_dither_OFF_Sub,
PQ_IP_VIP_CSC_dither_NUMS_Sub
} PQ_IP_VIP_CSC_dither_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_YC_delay_OFF_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb0Cr0_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb1Cr0_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb2Cr0_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb3Cr0_Sub,
PQ_IP_VIP_Post_YC_delay_Y1Cb0Cr0_Sub,
PQ_IP_VIP_Post_YC_delay_NUMS_Sub
} PQ_IP_VIP_Post_YC_delay_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_Y_OFF_Sub,
PQ_IP_VIP_HNMR_Y_S1_Sub,
PQ_IP_VIP_HNMR_Y_S2_Sub,
PQ_IP_VIP_HNMR_Y_S3_Sub,
PQ_IP_VIP_HNMR_Y_NUMS_Sub
} PQ_IP_VIP_HNMR_Y_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_OFF_Sub,
PQ_IP_VIP_HNMR_ad_C_Yon_Con_Sub,
PQ_IP_VIP_HNMR_ad_C_Yon_Coff_Sub,
PQ_IP_VIP_HNMR_ad_C_Yoff_Con_Sub,
PQ_IP_VIP_HNMR_ad_C_NUMS_Sub
} PQ_IP_VIP_HNMR_ad_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_gain_S0_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S1_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S2_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S3_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_NUMS_Sub
} PQ_IP_VIP_HNMR_ad_C_gain_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win1_OFF_Sub,
PQ_IP_VIP_HNMR_C_win1_Flesh_1_Sub,
PQ_IP_VIP_HNMR_C_win1_Flesh_2_Sub,
PQ_IP_VIP_HNMR_C_win1_NUMS_Sub
} PQ_IP_VIP_HNMR_C_win1_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win2_OFF_Sub,
PQ_IP_VIP_HNMR_C_win2_Blue_1_Sub,
PQ_IP_VIP_HNMR_C_win2_Blue_2_Sub,
PQ_IP_VIP_HNMR_C_win2_NUMS_Sub
} PQ_IP_VIP_HNMR_C_win2_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win3_OFF_Sub,
PQ_IP_VIP_HNMR_C_win3_Green_1_Sub,
PQ_IP_VIP_HNMR_C_win3_NUMS_Sub
} PQ_IP_VIP_HNMR_C_win3_Group_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win4_OFF_Sub,
PQ_IP_VIP_HNMR_C_win4_NUMS_Sub
} PQ_IP_VIP_HNMR_C_win4_Group_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Yoffset_OFF_Sub,
PQ_IP_VIP_Pre_Yoffset_0x05_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF0_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF4_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF6_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF7_Sub,
PQ_IP_VIP_Pre_Yoffset_0xFA_Sub,
PQ_IP_VIP_Pre_Yoffset_NUMS_Sub
} PQ_IP_VIP_Pre_Yoffset_Group_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_OFF_Sub,
PQ_IP_VIP_Pre_Ygain_0x42_Sub,
PQ_IP_VIP_Pre_Ygain_0x4A_Sub,
PQ_IP_VIP_Pre_Ygain_NUMS_Sub
} PQ_IP_VIP_Pre_Ygain_Group_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_dither_OFF_Sub,
PQ_IP_VIP_Pre_Ygain_dither_NUMS_Sub
} PQ_IP_VIP_Pre_Ygain_dither_Group_Sub;

typedef enum
{
PQ_IP_VIP_HLPF_OFF_Sub,
PQ_IP_VIP_HLPF_0x1_Sub,
PQ_IP_VIP_HLPF_0x2_Sub,
PQ_IP_VIP_HLPF_0x3_Sub,
PQ_IP_VIP_HLPF_0x4_Sub,
PQ_IP_VIP_HLPF_0x5_Sub,
PQ_IP_VIP_HLPF_0x6_Sub,
PQ_IP_VIP_HLPF_0x7_Sub,
PQ_IP_VIP_HLPF_NUMS_Sub
} PQ_IP_VIP_HLPF_Group_Sub;

typedef enum
{
PQ_IP_VIP_HLPF_dither_OFF_Sub,
PQ_IP_VIP_HLPF_dither_NUMS_Sub
} PQ_IP_VIP_HLPF_dither_Group_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_OFF_Sub,
PQ_IP_VIP_Peaking_ON_Sub,
PQ_IP_VIP_Peaking_NUMS_Sub
} PQ_IP_VIP_Peaking_Group_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_band_RF_NTSC_Sub,
PQ_IP_VIP_Peaking_band_RF_SECAM_Sub,
PQ_IP_VIP_Peaking_band_AV_NTSC_Sub,
PQ_IP_VIP_Peaking_band_AV_PAL_Sub,
PQ_IP_VIP_Peaking_band_AV_SECAM_Sub,
PQ_IP_VIP_Peaking_band_SV_NTSC_Sub,
PQ_IP_VIP_Peaking_band_480i_Sub,
PQ_IP_VIP_Peaking_band_720p_Sub,
PQ_IP_VIP_Peaking_band_1080i_Sub,
PQ_IP_VIP_Peaking_band_H_480i_Sub,
PQ_IP_VIP_Peaking_band_H_576i_Sub,
PQ_IP_VIP_Peaking_band_H_720p_Sub,
PQ_IP_VIP_Peaking_band_H_1080i_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_480is_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_720p_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_1080i_Sub,
PQ_IP_VIP_Peaking_band_DT_H264_480is_Sub,
PQ_IP_VIP_Peaking_band_DT_H264_720p_Sub,
PQ_IP_VIP_Peaking_band_PC_mode_Sub,
PQ_IP_VIP_Peaking_band_NUMS_Sub
} PQ_IP_VIP_Peaking_band_Group_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_Pcoring_S00_0_Sub,
PQ_IP_VIP_Peaking_Pcoring_S21_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_S30_0_Sub,
PQ_IP_VIP_Peaking_Pcoring_S30_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_S31_0_Sub,
PQ_IP_VIP_Peaking_Pcoring_S31_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_S20_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_S32_0_Sub,
PQ_IP_VIP_Peaking_Pcoring_S32_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_S42_1_Sub,
PQ_IP_VIP_Peaking_Pcoring_NUMS_Sub
} PQ_IP_VIP_Peaking_Pcoring_Group_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_gain_0x18_Sub,
PQ_IP_VIP_Peaking_gain_0x1C_Sub,
PQ_IP_VIP_Peaking_gain_0x20_Sub,
PQ_IP_VIP_Peaking_gain_0x24_Sub,
PQ_IP_VIP_Peaking_gain_0x28_Sub,
PQ_IP_VIP_Peaking_gain_0x2C_Sub,
PQ_IP_VIP_Peaking_gain_0x30_Sub,
PQ_IP_VIP_Peaking_gain_0x34_Sub,
PQ_IP_VIP_Peaking_gain_0x38_Sub,
PQ_IP_VIP_Peaking_gain_NUMS_Sub
} PQ_IP_VIP_Peaking_gain_Group_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_gain_ad_C_OFF_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_ON_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_NUMS_Sub
} PQ_IP_VIP_Peaking_gain_ad_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_CTI_OFF_Sub,
PQ_IP_VIP_Post_CTI_S0L0_Sub,
PQ_IP_VIP_Post_CTI_S3L3_Sub,
PQ_IP_VIP_Post_CTI_S1L6_Sub,
PQ_IP_VIP_Post_CTI_S1L0_Sub,
PQ_IP_VIP_Post_CTI_S2L3_Sub,
PQ_IP_VIP_Post_CTI_S2L4_Sub,
PQ_IP_VIP_Post_CTI_S2L5_Sub,
PQ_IP_VIP_Post_CTI_S2L6_Sub,
PQ_IP_VIP_Post_CTI_S2L7_Sub,
PQ_IP_VIP_Post_CTI_NUMS_Sub
} PQ_IP_VIP_Post_CTI_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_CTI_coef_0x18_Sub,
PQ_IP_VIP_Post_CTI_coef_0x1C_Sub,
PQ_IP_VIP_Post_CTI_coef_0x20_Sub,
PQ_IP_VIP_Post_CTI_coef_0x24_Sub,
PQ_IP_VIP_Post_CTI_coef_0x28_Sub,
PQ_IP_VIP_Post_CTI_coef_0x04_Sub,
PQ_IP_VIP_Post_CTI_coef_NUMS_Sub
} PQ_IP_VIP_Post_CTI_coef_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T1_OFF_Sub,
PQ_IP_VIP_FCC_T1_S4_Sub,
PQ_IP_VIP_FCC_T1_mts_Sub,
PQ_IP_VIP_FCC_T1_mts_1080_Sub,
PQ_IP_VIP_FCC_T1_NUMS_Sub
} PQ_IP_VIP_FCC_T1_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T2_OFF_Sub,
PQ_IP_VIP_FCC_T2_S8_Sub,
PQ_IP_VIP_FCC_T2_mts_Sub,
PQ_IP_VIP_FCC_T2_mts_1080_Sub,
PQ_IP_VIP_FCC_T2_NUMS_Sub
} PQ_IP_VIP_FCC_T2_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T3_OFF_Sub,
PQ_IP_VIP_FCC_T3_S4_Sub,
PQ_IP_VIP_FCC_T3_mts_Sub,
PQ_IP_VIP_FCC_T3_mts_1080_Sub,
PQ_IP_VIP_FCC_T3_NUMS_Sub
} PQ_IP_VIP_FCC_T3_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T4_OFF_Sub,
PQ_IP_VIP_FCC_T4_S4_Sub,
PQ_IP_VIP_FCC_T4_mts_Sub,
PQ_IP_VIP_FCC_T4_mts_1080_Sub,
PQ_IP_VIP_FCC_T4_NUMS_Sub
} PQ_IP_VIP_FCC_T4_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T5_OFF_Sub,
PQ_IP_VIP_FCC_T5_S6_Sub,
PQ_IP_VIP_FCC_T5_S8_Sub,
PQ_IP_VIP_FCC_T5_mts_Sub,
PQ_IP_VIP_FCC_T5_mts_1080_Sub,
PQ_IP_VIP_FCC_T5_NUMS_Sub
} PQ_IP_VIP_FCC_T5_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T6_OFF_Sub,
PQ_IP_VIP_FCC_T6_S6_Sub,
PQ_IP_VIP_FCC_T6_S8_Sub,
PQ_IP_VIP_FCC_T6_mts_Sub,
PQ_IP_VIP_FCC_T6_mts_1080_Sub,
PQ_IP_VIP_FCC_T6_NUMS_Sub
} PQ_IP_VIP_FCC_T6_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T7_OFF_Sub,
PQ_IP_VIP_FCC_T7_S8_Sub,
PQ_IP_VIP_FCC_T7_mts_Sub,
PQ_IP_VIP_FCC_T7_mts_1080_Sub,
PQ_IP_VIP_FCC_T7_NUMS_Sub
} PQ_IP_VIP_FCC_T7_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T8_OFF_Sub,
PQ_IP_VIP_FCC_T8_S5_Sub,
PQ_IP_VIP_FCC_T8_mts_Sub,
PQ_IP_VIP_FCC_T8_mts_1080_Sub,
PQ_IP_VIP_FCC_T8_NUMS_Sub
} PQ_IP_VIP_FCC_T8_Group_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T9_OFF_Sub,
PQ_IP_VIP_FCC_T9_S5_Sub,
PQ_IP_VIP_FCC_T9_mts_Sub,
PQ_IP_VIP_FCC_T9_mts_1080_Sub,
PQ_IP_VIP_FCC_T9_NUMS_Sub
} PQ_IP_VIP_FCC_T9_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_OFF_Sub,
PQ_IP_VIP_IHC_ON_Sub,
PQ_IP_VIP_IHC_NUMS_Sub
} PQ_IP_VIP_IHC_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_R_OFF_Sub,
PQ_IP_VIP_IHC_R_N_20_Sub,
PQ_IP_VIP_IHC_R_P_20_Sub,
PQ_IP_VIP_IHC_R_mts_AV_Sub,
PQ_IP_VIP_IHC_R_NUMS_Sub
} PQ_IP_VIP_IHC_R_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_G_OFF_Sub,
PQ_IP_VIP_IHC_G_N_30_Sub,
PQ_IP_VIP_IHC_G_N_10_Sub,
PQ_IP_VIP_IHC_G_mts_AV_Sub,
PQ_IP_VIP_IHC_G_NUMS_Sub
} PQ_IP_VIP_IHC_G_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_B_OFF_Sub,
PQ_IP_VIP_IHC_B_mts_AV_Sub,
PQ_IP_VIP_IHC_B_NUMS_Sub
} PQ_IP_VIP_IHC_B_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_C_OFF_Sub,
PQ_IP_VIP_IHC_C_N_3F_Sub,
PQ_IP_VIP_IHC_C_N_30_Sub,
PQ_IP_VIP_IHC_C_N_10_Sub,
PQ_IP_VIP_IHC_C_mts_AV_Sub,
PQ_IP_VIP_IHC_C_mts_1080_Sub,
PQ_IP_VIP_IHC_C_NUMS_Sub
} PQ_IP_VIP_IHC_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_M_OFF_Sub,
PQ_IP_VIP_IHC_M_P_10_Sub,
PQ_IP_VIP_IHC_M_NUMS_Sub
} PQ_IP_VIP_IHC_M_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_Y_OFF_Sub,
PQ_IP_VIP_IHC_Y_P_8_Sub,
PQ_IP_VIP_IHC_Y_P_10_Sub,
PQ_IP_VIP_IHC_Y_P_15_Sub,
PQ_IP_VIP_IHC_Y_mts_AV_Sub,
PQ_IP_VIP_IHC_Y_mts_1080_Sub,
PQ_IP_VIP_IHC_Y_NUMS_Sub
} PQ_IP_VIP_IHC_Y_Group_Sub;

typedef enum
{
PQ_IP_VIP_IHC_F_OFF_Sub,
PQ_IP_VIP_IHC_F_N_10_Sub,
PQ_IP_VIP_IHC_F_P_10_Sub,
PQ_IP_VIP_IHC_F_mts_AV_Sub,
PQ_IP_VIP_IHC_F_NUMS_Sub
} PQ_IP_VIP_IHC_F_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_OFF_Sub,
PQ_IP_VIP_ICC_ON_Sub,
PQ_IP_VIP_ICC_NUMS_Sub
} PQ_IP_VIP_ICC_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_R_OFF_Sub,
PQ_IP_VIP_ICC_R_INC_8_Sub,
PQ_IP_VIP_ICC_R_INC_3_Sub,
PQ_IP_VIP_ICC_R_INC_B_Sub,
PQ_IP_VIP_ICC_R_DEC_3_Sub,
PQ_IP_VIP_ICC_R_NUMS_Sub
} PQ_IP_VIP_ICC_R_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_G_OFF_Sub,
PQ_IP_VIP_ICC_G_INC_8_Sub,
PQ_IP_VIP_ICC_G_INC_6_Sub,
PQ_IP_VIP_ICC_G_INC_B_Sub,
PQ_IP_VIP_ICC_G_NUMS_Sub
} PQ_IP_VIP_ICC_G_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_B_OFF_Sub,
PQ_IP_VIP_ICC_B_INC_A_Sub,
PQ_IP_VIP_ICC_B_INC_B_Sub,
PQ_IP_VIP_ICC_B_INC_9_Sub,
PQ_IP_VIP_ICC_B_INC_6_Sub,
PQ_IP_VIP_ICC_B_INC_5_Sub,
PQ_IP_VIP_ICC_B_NUMS_Sub
} PQ_IP_VIP_ICC_B_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_C_OFF_Sub,
PQ_IP_VIP_ICC_C_INC_6_Sub,
PQ_IP_VIP_ICC_C_INC_9_Sub,
PQ_IP_VIP_ICC_C_INC_A_Sub,
PQ_IP_VIP_ICC_C_INC_B_Sub,
PQ_IP_VIP_ICC_C_INC_4_Sub,
PQ_IP_VIP_ICC_C_NUMS_Sub
} PQ_IP_VIP_ICC_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_M_OFF_Sub,
PQ_IP_VIP_ICC_M_INC_6_Sub,
PQ_IP_VIP_ICC_M_INC_9_Sub,
PQ_IP_VIP_ICC_M_INC_A_Sub,
PQ_IP_VIP_ICC_M_NUMS_Sub
} PQ_IP_VIP_ICC_M_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_Y_OFF_Sub,
PQ_IP_VIP_ICC_Y_INC_7_Sub,
PQ_IP_VIP_ICC_Y_INC_8_Sub,
PQ_IP_VIP_ICC_Y_INC_A_Sub,
PQ_IP_VIP_ICC_Y_NUMS_Sub
} PQ_IP_VIP_ICC_Y_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_F_OFF_Sub,
PQ_IP_VIP_ICC_F_INC_2_Sub,
PQ_IP_VIP_ICC_F_INC_3_Sub,
PQ_IP_VIP_ICC_F_INC_A_Sub,
PQ_IP_VIP_ICC_F_NUMS_Sub
} PQ_IP_VIP_ICC_F_Group_Sub;

typedef enum
{
PQ_IP_VIP_ICC_NC_OFF_Sub,
PQ_IP_VIP_ICC_NC_INC_2_Sub,
PQ_IP_VIP_ICC_NC_INC_3_Sub,
PQ_IP_VIP_ICC_NC_INC_8_Sub,
PQ_IP_VIP_ICC_NC_INC_A_Sub,
PQ_IP_VIP_ICC_NC_NUMS_Sub
} PQ_IP_VIP_ICC_NC_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_OFF_Sub,
PQ_IP_VIP_IBC_ON_Sub,
PQ_IP_VIP_IBC_NUMS_Sub
} PQ_IP_VIP_IBC_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_R_OFF_Sub,
PQ_IP_VIP_IBC_R_DEC_10_Sub,
PQ_IP_VIP_IBC_R_DEC_18_Sub,
PQ_IP_VIP_IBC_R_NUMS_Sub
} PQ_IP_VIP_IBC_R_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_G_OFF_Sub,
PQ_IP_VIP_IBC_G_DEC_10_Sub,
PQ_IP_VIP_IBC_G_DEC_1C_Sub,
PQ_IP_VIP_IBC_G_DEC_18_Sub,
PQ_IP_VIP_IBC_G_NUMS_Sub
} PQ_IP_VIP_IBC_G_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_B_OFF_Sub,
PQ_IP_VIP_IBC_B_DEC_10_Sub,
PQ_IP_VIP_IBC_B_DEC_18_Sub,
PQ_IP_VIP_IBC_B_NUMS_Sub
} PQ_IP_VIP_IBC_B_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_C_OFF_Sub,
PQ_IP_VIP_IBC_C_DEC_10_Sub,
PQ_IP_VIP_IBC_C_DEC_18_Sub,
PQ_IP_VIP_IBC_C_INC_22_Sub,
PQ_IP_VIP_IBC_C_NUMS_Sub
} PQ_IP_VIP_IBC_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_M_OFF_Sub,
PQ_IP_VIP_IBC_M_DEC_18_Sub,
PQ_IP_VIP_IBC_M_NUMS_Sub
} PQ_IP_VIP_IBC_M_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_Y_OFF_Sub,
PQ_IP_VIP_IBC_Y_DEC_18_Sub,
PQ_IP_VIP_IBC_Y_NUMS_Sub
} PQ_IP_VIP_IBC_Y_Group_Sub;

typedef enum
{
PQ_IP_VIP_IBC_F_OFF_Sub,
PQ_IP_VIP_IBC_F_INC_28_Sub,
PQ_IP_VIP_IBC_F_NUMS_Sub
} PQ_IP_VIP_IBC_F_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_OFF_Sub,
PQ_IP_VIP_DLC_ON_Sub,
PQ_IP_VIP_DLC_NUMS_Sub
} PQ_IP_VIP_DLC_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_dither_OFF_Sub,
PQ_IP_VIP_DLC_dither_NUMS_Sub
} PQ_IP_VIP_DLC_dither_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_Flesh_adj_OFF_Sub,
PQ_IP_VIP_DLC_Flesh_adj_S1_Sub,
PQ_IP_VIP_DLC_Flesh_adj_NUMS_Sub
} PQ_IP_VIP_DLC_Flesh_adj_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_range_OFF_Sub,
PQ_IP_VIP_DLC_His_range_ON_Sub,
PQ_IP_VIP_DLC_His_range_NUMS_Sub
} PQ_IP_VIP_DLC_His_range_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeH_90pa_1366_Sub,
PQ_IP_VIP_DLC_His_rangeH_90pa_1920_Sub,
PQ_IP_VIP_DLC_His_rangeH_NUMS_Sub
} PQ_IP_VIP_DLC_His_rangeH_Group_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeV_90pa_1366_Sub,
PQ_IP_VIP_DLC_His_rangeV_90pa_1920_Sub,
PQ_IP_VIP_DLC_His_rangeV_NUMS_Sub
} PQ_IP_VIP_DLC_His_rangeV_Group_Sub;

typedef enum
{
PQ_IP_VIP_BLE_OFF_Sub,
PQ_IP_VIP_BLE_0x82_0x40_Sub,
PQ_IP_VIP_BLE_0x82_0x50_Sub,
PQ_IP_VIP_BLE_0x82_0x60_Sub,
PQ_IP_VIP_BLE_0x84_0x40_Sub,
PQ_IP_VIP_BLE_0x86_0x40_Sub,
PQ_IP_VIP_BLE_0x88_0x40_Sub,
PQ_IP_VIP_BLE_0x88_0x60_Sub,
PQ_IP_VIP_BLE_0x8A_0x40_Sub,
PQ_IP_VIP_BLE_NUMS_Sub
} PQ_IP_VIP_BLE_Group_Sub;

typedef enum
{
PQ_IP_VIP_WLE_OFF_Sub,
PQ_IP_VIP_WLE_0x78_0x20_Sub,
PQ_IP_VIP_WLE_NUMS_Sub
} PQ_IP_VIP_WLE_Group_Sub;

typedef enum
{
PQ_IP_VIP_BWLE_dither_OFF_Sub,
PQ_IP_VIP_BWLE_dither_NUMS_Sub
} PQ_IP_VIP_BWLE_dither_Group_Sub;

typedef enum
{
PQ_IP_VIP_UVC_OFF_Sub,
PQ_IP_VIP_UVC_ON_Sub,
PQ_IP_VIP_UVC_NUMS_Sub
} PQ_IP_VIP_UVC_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_Yoffset_OFF_Sub,
PQ_IP_VIP_Post_Yoffset_0x05_Sub,
PQ_IP_VIP_Post_Yoffset_NUMS_Sub
} PQ_IP_VIP_Post_Yoffset_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_Ygain_OFF_Sub,
PQ_IP_VIP_Post_Ygain_0x3C_Sub,
PQ_IP_VIP_Post_Ygain_0x36_Sub,
PQ_IP_VIP_Post_Ygain_NUMS_Sub
} PQ_IP_VIP_Post_Ygain_Group_Sub;

typedef enum
{
PQ_IP_VIP_Post_Cgain_OFF_Sub,
PQ_IP_VIP_Post_Cgain_0x44_Sub,
PQ_IP_VIP_Post_Cgain_NUMS_Sub
} PQ_IP_VIP_Post_Cgain_Group_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_Y_OFF_Sub,
PQ_IP_VIP_Hcoring_Y_C4_Sub,
PQ_IP_VIP_Hcoring_Y_C3_Sub,
PQ_IP_VIP_Hcoring_Y_C2_Sub,
PQ_IP_VIP_Hcoring_Y_C1_Sub,
PQ_IP_VIP_Hcoring_Y_P6_Sub,
PQ_IP_VIP_Hcoring_Y_P5_Sub,
PQ_IP_VIP_Hcoring_Y_P4_Sub,
PQ_IP_VIP_Hcoring_Y_P3_Sub,
PQ_IP_VIP_Hcoring_Y_P2_Sub,
PQ_IP_VIP_Hcoring_Y_P1_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P6_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P5_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P4_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P3_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P1_Sub,
PQ_IP_VIP_Hcoring_Y_NUMS_Sub
} PQ_IP_VIP_Hcoring_Y_Group_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_C_OFF_Sub,
PQ_IP_VIP_Hcoring_C_C4_Sub,
PQ_IP_VIP_Hcoring_C_C3_Sub,
PQ_IP_VIP_Hcoring_C_C2_Sub,
PQ_IP_VIP_Hcoring_C_C1_Sub,
PQ_IP_VIP_Hcoring_C_P5_Sub,
PQ_IP_VIP_Hcoring_C_P4_Sub,
PQ_IP_VIP_Hcoring_C_P3_Sub,
PQ_IP_VIP_Hcoring_C_P2_Sub,
PQ_IP_VIP_Hcoring_C_P1_Sub,
PQ_IP_VIP_Hcoring_C_NUMS_Sub
} PQ_IP_VIP_Hcoring_C_Group_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_dither_OFF_Sub,
PQ_IP_VIP_Hcoring_dither_NUMS_Sub
} PQ_IP_VIP_Hcoring_dither_Group_Sub;

typedef enum
{
PQ_IP_SwDriver_OFF_Sub,
PQ_IP_SwDriver_ALL_Sub,
PQ_IP_SwDriver_DynamicDNR_ON_Sub,
PQ_IP_SwDriver_NUMS_Sub
} PQ_IP_SwDriver_Group_Sub;

typedef enum
{
PQ_IP_3x3_OFF_Sub,
PQ_IP_3x3_SD_Sub,
PQ_IP_3x3_HD_Sub,
PQ_IP_3x3_NUMS_Sub
} PQ_IP_3x3_Group_Sub;

typedef enum
{
PQ_IP_RGB_Offset_OFF_Sub,
PQ_IP_RGB_Offset_NUMS_Sub
} PQ_IP_RGB_Offset_Group_Sub;

typedef enum
{
PQ_IP_RGB_Clip_OFF_Sub,
PQ_IP_RGB_Clip_NUMS_Sub
} PQ_IP_RGB_Clip_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_bypass_OFF_Sub,
PQ_IP_xvYCC_bypass_ON_Sub,
PQ_IP_xvYCC_bypass_NUMS_Sub
} PQ_IP_xvYCC_bypass_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_de_gamma_OFF_Sub,
PQ_IP_xvYCC_de_gamma_ON_Sub,
PQ_IP_xvYCC_de_gamma_NUMS_Sub
} PQ_IP_xvYCC_de_gamma_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_de_gamma_dither_OFF_Sub,
PQ_IP_xvYCC_de_gamma_dither_ON_Sub,
PQ_IP_xvYCC_de_gamma_dither_NUMS_Sub
} PQ_IP_xvYCC_de_gamma_dither_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_3x3_OFF_Sub,
PQ_IP_xvYCC_3x3_NUMS_Sub
} PQ_IP_xvYCC_3x3_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_Comp_OFF_Sub,
PQ_IP_xvYCC_Comp_NUMS_Sub
} PQ_IP_xvYCC_Comp_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_Comp_dither_OFF_Sub,
PQ_IP_xvYCC_Comp_dither_ON_Sub,
PQ_IP_xvYCC_Comp_dither_NUMS_Sub
} PQ_IP_xvYCC_Comp_dither_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_Clip_OFF_Sub,
PQ_IP_xvYCC_Clip_NUMS_Sub
} PQ_IP_xvYCC_Clip_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_gamma_OFF_Sub,
PQ_IP_xvYCC_gamma_ON_Sub,
PQ_IP_xvYCC_gamma_NUMS_Sub
} PQ_IP_xvYCC_gamma_Group_Sub;

typedef enum
{
PQ_IP_xvYCC_gamma_dither_OFF_Sub,
PQ_IP_xvYCC_gamma_dither_ON_Sub,
PQ_IP_xvYCC_gamma_dither_NUMS_Sub
} PQ_IP_xvYCC_gamma_dither_Group_Sub;

typedef enum
{
PQ_IP_rgb_3d_OFF_Sub,
PQ_IP_rgb_3d_ON_Sub,
PQ_IP_rgb_3d_NUMS_Sub
} PQ_IP_rgb_3d_Group_Sub;

typedef enum
{
PQ_IP_HBC_OFF_Sub,
PQ_IP_HBC_HBC1_Sub,
PQ_IP_HBC_NUMS_Sub
} PQ_IP_HBC_Group_Sub;

typedef enum
{
PQ_IP_Pre_CON_BRI_OFF_Sub,
PQ_IP_Pre_CON_BRI_NUMS_Sub
} PQ_IP_Pre_CON_BRI_Group_Sub;

typedef enum
{
PQ_IP_Blue_Stretch_OFF_Sub,
PQ_IP_Blue_Stretch_BS1_Sub,
PQ_IP_Blue_Stretch_NUMS_Sub
} PQ_IP_Blue_Stretch_Group_Sub;

typedef enum
{
PQ_IP_Blue_Stretch_dither_OFF_Sub,
PQ_IP_Blue_Stretch_dither_NUMS_Sub
} PQ_IP_Blue_Stretch_dither_Group_Sub;

typedef enum
{
PQ_IP_Gamma_OFF_Sub,
PQ_IP_Gamma_ON_Sub,
PQ_IP_Gamma_NUMS_Sub
} PQ_IP_Gamma_Group_Sub;

typedef enum
{
PQ_IP_Gamma_dither_OFF_Sub,
PQ_IP_Gamma_dither_NUMS_Sub
} PQ_IP_Gamma_dither_Group_Sub;

typedef enum
{
PQ_IP_Post_CON_BRI_OFF_Sub,
PQ_IP_Post_CON_BRI_NUMS_Sub
} PQ_IP_Post_CON_BRI_Group_Sub;

typedef enum
{
PQ_IP_SRAM_3x3matrix_PIC2_Build_IN_Sub,
PQ_IP_SRAM_3x3matrix_PIC2_NUMS_Sub
} PQ_IP_SRAM_3x3matrix_PIC2_Group_Sub;

typedef enum
{
PQ_IP_SRAM_DLC_PIC1_Build_IN_Sub,
PQ_IP_SRAM_DLC_PIC1_NUMS_Sub
} PQ_IP_SRAM_DLC_PIC1_Group_Sub;

typedef enum
{
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Build_IN_Sub,
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_NUMS_Sub
} PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Group_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_R_PIC1_Build_IN_Sub,
PQ_IP_SRAM_GammaTbl_R_PIC1_NUMS_Sub
} PQ_IP_SRAM_GammaTbl_R_PIC1_Group_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_G_PIC1_Build_IN_Sub,
PQ_IP_SRAM_GammaTbl_G_PIC1_NUMS_Sub
} PQ_IP_SRAM_GammaTbl_G_PIC1_Group_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_B_PIC1_Build_IN_Sub,
PQ_IP_SRAM_GammaTbl_B_PIC1_NUMS_Sub
} PQ_IP_SRAM_GammaTbl_B_PIC1_Group_Sub;

typedef enum
{
PQ_IP_SRAM_Auto_Color_PIC2_Build_IN_Sub,
PQ_IP_SRAM_Auto_Color_PIC2_NUMS_Sub
} PQ_IP_SRAM_Auto_Color_PIC2_Group_Sub;

typedef enum
{
PQ_IP_SRAM_Color_Temp_PIC1_Build_IN_Sub,
PQ_IP_SRAM_Color_Temp_PIC1_NUMS_Sub
} PQ_IP_SRAM_Color_Temp_PIC1_Group_Sub;

extern code U8 MST_SkipRule_IP_Sub[PQ_IP_NUM_Sub];
extern code EN_IPTAB_INFO PQ_IPTAB_INFO_Sub[];
extern code U8 QMAP_1920_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];
extern code U8 QMAP_1920_PIP_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];
extern code U8 QMAP_1920_POP_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];
extern code U8 QMAP_1366_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];
extern code U8 QMAP_1366_PIP_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];
extern code U8 QMAP_1366_POP_Sub[QM_INPUTTYPE_NUM_Sub][PQ_IP_NUM_Sub];

#endif
