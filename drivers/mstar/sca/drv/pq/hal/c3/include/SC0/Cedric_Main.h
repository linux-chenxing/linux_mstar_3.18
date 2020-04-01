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
// 1/7/2013  8:35:31 PM
// 39630.43853
//****************************************************

#ifndef _CEDRIC_MAIN_H_
#define _CEDRIC_MAIN_H_

#define PQ_IP_NUM_Main 190
#define PQ_IP_SRAM_COLOR_INDEX_SIZE_Main 256
#define PQ_IP_SRAM_COLOR_GAIN_SNR_SIZE_Main 8
#define PQ_IP_SRAM_COLOR_GAIN_DNR_SIZE_Main 8
#define PQ_IP_SRAM1_SIZE_Main 320
#define PQ_IP_SRAM2_SIZE_Main 320
#define PQ_IP_SRAM3_SIZE_Main 640
#define PQ_IP_SRAM4_SIZE_Main 640
#define PQ_IP_C_SRAM1_SIZE_Main 320
#define PQ_IP_C_SRAM2_SIZE_Main 320
#define PQ_IP_C_SRAM3_SIZE_Main 320
#define PQ_IP_C_SRAM4_SIZE_Main 320
#define PQ_IP_SRAM_3x3matrix_PIC2_SIZE_Main 32
#define PQ_IP_SRAM_DLC_PIC1_SIZE_Main 65
#define PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SIZE_Main 5
#define PQ_IP_SRAM_GammaTbl_R_PIC1_SIZE_Main 386
#define PQ_IP_SRAM_GammaTbl_G_PIC1_SIZE_Main 386
#define PQ_IP_SRAM_GammaTbl_B_PIC1_SIZE_Main 386
#define PQ_IP_SRAM_Auto_Color_PIC2_SIZE_Main 6
#define PQ_IP_SRAM_Color_Temp_PIC1_SIZE_Main 9

typedef enum
{
    #if PQ_QM_CVBS
    QM_RF_NTSC_44_Main, //0
    #endif
    #if PQ_QM_CVBS
    QM_RF_NTSC_M_Main, //1
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_BGHI_Main, //2
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_60_Main, //3
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_M_Main, //4
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_N_Main, //5
    #endif
    #if PQ_QM_CVBS
    QM_RF_SECAM_Main, //6
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_44_Main, //7
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_M_Main, //8
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_BGHI_Main, //9
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_60_Main, //10
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_M_Main, //11
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_N_Main, //12
    #endif
    #if PQ_QM_CVBS
    QM_VIF_SECAM_Main, //13
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_44_Main, //14
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_M_Main, //15
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_BGHI_Main, //16
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_60_Main, //17
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_M_Main, //18
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_N_Main, //19
    #endif
    #if PQ_QM_CVBS
    QM_SV_SECAM_Main, //20
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_44_Main, //21
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_M_Main, //22
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_BGHI_Main, //23
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_60_Main, //24
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_M_Main, //25
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_N_Main, //26
    #endif
    #if PQ_QM_CVBS
    QM_AV_SECAM_Main, //27
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_44_Main, //28
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_M_Main, //29
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_BGHI_Main, //30
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_60_Main, //31
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_M_Main, //32
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_N_Main, //33
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_SECAM_Main, //34
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_44_Main, //35
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_M_Main, //36
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_BGHI_Main, //37
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_60_Main, //38
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_M_Main, //39
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_N_Main, //40
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_SECAM_Main, //41
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_NTSC_Main, //42
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_PAL_Main, //43
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480i_Main, //44
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576i_Main, //45
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480p_Main, //46
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576p_Main, //47
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_24hz_Main, //48
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_50hz_Main, //49
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_60hz_Main, //50
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_50hz_Main, //51
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_60hz_Main, //52
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_24hz_Main, //53
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_50hz_Main, //54
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_60hz_Main, //55
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480i_Main, //56
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576i_Main, //57
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480p_Main, //58
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576p_Main, //59
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_24hz_Main, //60
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_50hz_Main, //61
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_60hz_Main, //62
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_50hz_Main, //63
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_60hz_Main, //64
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_24hz_Main, //65
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_60hz_Main, //66
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_50hz_Main, //67
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vup_Main, //68
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vdown_Main, //69
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vno_Main, //70
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vup_Main, //71
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vdown_Main, //72
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vno_Main, //73
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vup_Main, //74
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vdown_Main, //75
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vno_Main, //76
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vup_Main, //77
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vdown_Main, //78
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vno_Main, //79
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vup_Main, //80
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vdown_Main, //81
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vno_Main, //82
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vup_Main, //83
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vdown_Main, //84
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vno_Main, //85
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vup_Main, //86
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown_Main, //87
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vno_Main, //88
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup_Main, //89
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown_Main, //90
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno_Main, //91
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vup_Main, //92
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown_Main, //93
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_Main, //94
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_MPEG2_Main, //95
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_MPEG2_Main, //96
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_MPEG2_Main, //97
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_MPEG2_Main, //98
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_MPEG2_Main, //99
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_MPEG2_Main, //100
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_MPEG2_Main, //101
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_MPEG2_Main, //102
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_MPEG2_Main, //103
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_MPEG2_Main, //104
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_MPEG2_Main, //105
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_MPEG2_Main, //106
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_MPEG2_Main, //107
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_H264_Main, //108
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_H264_Main, //109
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_H264_Main, //110
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_H264_Main, //111
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_H264_Main, //112
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_H264_Main, //113
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_H264_Main, //114
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_H264_Main, //115
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_H264_Main, //116
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_H264_Main, //117
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_H264_Main, //118
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_H264_Main, //119
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_H264_Main, //120
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_interlace_Main, //121
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_progressive_Main, //122
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_interlace_Main, //123
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_progressive_Main, //124
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_SD_progressive_Main, //125
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_HD_progressive_Main, //126
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_interlace_Main, //127
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_progressive_Main, //128
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_interlace_Main, //129
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_progressive_Main, //130
    #endif
    #if PQ_QM_3D
    QM_3D_Video_interlace_Main, //131
    #endif
    #if PQ_QM_3D
    QM_3D_Video_progressive_Main, //132
    #endif
    QM_INPUTTYPE_NUM_Main, // 133
} SC_QUALITY_MAP_INDEX_e_Main;

typedef enum
{
PQ_IP_AFEC_Main,  //0
PQ_IP_Comb_Main,  //1
PQ_IP_Comb2_Main,  //2
PQ_IP_SECAM_Main,  //3
PQ_IP_VD_Sampling_no_comm_Main,  //4
PQ_IP_ADC_Sampling_Main,  //5
PQ_IP_SCinit_Main,  //6
PQ_IP_CSC_Main,  //7
PQ_IP_CSC_Dither_Main,  //8
PQ_IP_YCdelay_Main,  //9
PQ_IP_PreFilter_Main,  //10
PQ_IP_PreFilter_Dither_Main,  //11
PQ_IP_HDSDD_Main,  //12
PQ_IP_HSD_Sampling_Main,  //13
PQ_IP_HSD_Y_Main,  //14
PQ_IP_HSD_C_Main,  //15
PQ_IP_444To422_Main,  //16
PQ_IP_VSD_Main,  //17
PQ_IP_HVSD_Dither_Main,  //18
PQ_IP_10to8_Dither_Main,  //19
PQ_IP_MemFormat_Main,  //20
PQ_IP_PreSNR_Main,  //21
PQ_IP_PreSNR_Patch_Main,  //22
PQ_IP_DNR_Main,  //23
PQ_IP_DNR_Motion_Main,  //24
PQ_IP_DNR_Y_Main,  //25
PQ_IP_DNR_Y_COLOR_DEP_Main,  //26
PQ_IP_SRAM_COLOR_INDEX_Main,  //27
PQ_IP_SRAM_COLOR_GAIN_SNR_Main,  //28
PQ_IP_SRAM_COLOR_GAIN_DNR_Main,  //29
PQ_IP_DNR_Y_LUMA_ADAPTIVE_Main,  //30
PQ_IP_DNR_POSTTUNE_Main,  //31
PQ_IP_DNR_C_Main,  //32
PQ_IP_HISDNR_Main,  //33
PQ_IP_PNR_Main,  //34
PQ_IP_PNR_Y_Main,  //35
PQ_IP_PNR_C_Main,  //36
PQ_IP_PostCCS_Main,  //37
PQ_IP_PostCCS_Smooth_Main,  //38
PQ_IP_420CUP_Main,  //39
PQ_IP_MADi_Main,  //40
PQ_IP_MADi_Motion_Main,  //41
PQ_IP_MADi_ADP3x3_Main,  //42
PQ_IP_MADi_MORPHO_Main,  //43
PQ_IP_MADi_DFK_Main,  //44
PQ_IP_MADi_SST_Main,  //45
PQ_IP_MADi_EODiW_Main,  //46
PQ_IP_MADi_Force_Main,  //47
PQ_IP_EODi_Main,  //48
PQ_IP_Film_Main,  //49
PQ_IP_Film32_Main,  //50
PQ_IP_Film22_Main,  //51
PQ_IP_Film_any_Main,  //52
PQ_IP_UCNR_Main,  //53
PQ_IP_UCDi_Main,  //54
PQ_IP_UC_CTL_Main,  //55
PQ_IP_DIPF_Main,  //56
PQ_IP_VCLPF_Main,  //57
PQ_IP_Spike_NR_Main,  //58
PQ_IP_SPF_Main,  //59
PQ_IP_SPF_SNR_Main,  //60
PQ_IP_SPF_SNR_MR_Main,  //61
PQ_IP_SPF_MR_LPF_Main,  //62
PQ_IP_SPF_NMR_Y_Main,  //63
PQ_IP_SPF_NMR_Y_MR_Main,  //64
PQ_IP_VSP_Y_Main,  //65
PQ_IP_VSP_C_Main,  //66
PQ_IP_VSP_CoRing_Main,  //67
PQ_IP_VSP_DeRing_Main,  //68
PQ_IP_VSP_Dither_Main,  //69
PQ_IP_VSP_PreVBound_Main,  //70
PQ_IP_422To444_Main,  //71
PQ_IP_PreCTI_Main,  //72
PQ_IP_HSP_Y_Main,  //73
PQ_IP_HSP_C_Main,  //74
PQ_IP_HSP_CoRing_Main,  //75
PQ_IP_HSP_DeRing_Main,  //76
PQ_IP_HSP_Dither_Main,  //77
PQ_IP_HnonLinear_Main,  //78
PQ_IP_SRAM1_Main,  //79
PQ_IP_SRAM2_Main,  //80
PQ_IP_SRAM3_Main,  //81
PQ_IP_SRAM4_Main,  //82
PQ_IP_C_SRAM1_Main,  //83
PQ_IP_C_SRAM2_Main,  //84
PQ_IP_C_SRAM3_Main,  //85
PQ_IP_C_SRAM4_Main,  //86
PQ_IP_VIP_Main,  //87
PQ_IP_VIP_pseudo_Main,  //88
PQ_IP_VIP_CSC_Main,  //89
PQ_IP_VIP_CSC_dither_Main,  //90
PQ_IP_VIP_Post_YC_delay_Main,  //91
PQ_IP_VIP_HNMR_Y_Main,  //92
PQ_IP_VIP_HNMR_ad_C_Main,  //93
PQ_IP_VIP_HNMR_ad_C_gain_Main,  //94
PQ_IP_VIP_HNMR_C_win1_Main,  //95
PQ_IP_VIP_HNMR_C_win2_Main,  //96
PQ_IP_VIP_HNMR_C_win3_Main,  //97
PQ_IP_VIP_HNMR_C_win4_Main,  //98
PQ_IP_VIP_Pre_Yoffset_Main,  //99
PQ_IP_VIP_Pre_Ygain_Main,  //100
PQ_IP_VIP_Pre_Ygain_dither_Main,  //101
PQ_IP_VIP_HLPF_Main,  //102
PQ_IP_VIP_HLPF_dither_Main,  //103
PQ_IP_VIP_Peaking_Main,  //104
PQ_IP_VIP_Peaking_band_Main,  //105
PQ_IP_VIP_Peaking_Pcoring_Main,  //106
PQ_IP_VIP_Peaking_gain_Main,  //107
PQ_IP_VIP_Peaking_gain_ad_C_Main,  //108
PQ_IP_VIP_Post_CTI_Main,  //109
PQ_IP_VIP_Post_CTI_coef_Main,  //110
PQ_IP_VIP_FCC_T1_Main,  //111
PQ_IP_VIP_FCC_T2_Main,  //112
PQ_IP_VIP_FCC_T3_Main,  //113
PQ_IP_VIP_FCC_T4_Main,  //114
PQ_IP_VIP_FCC_T5_Main,  //115
PQ_IP_VIP_FCC_T6_Main,  //116
PQ_IP_VIP_FCC_T7_Main,  //117
PQ_IP_VIP_FCC_T8_Main,  //118
PQ_IP_VIP_FCC_T9_Main,  //119
PQ_IP_VIP_IHC_Main,  //120
PQ_IP_VIP_IHC_R_Main,  //121
PQ_IP_VIP_IHC_G_Main,  //122
PQ_IP_VIP_IHC_B_Main,  //123
PQ_IP_VIP_IHC_C_Main,  //124
PQ_IP_VIP_IHC_M_Main,  //125
PQ_IP_VIP_IHC_Y_Main,  //126
PQ_IP_VIP_IHC_F_Main,  //127
PQ_IP_VIP_ICC_Main,  //128
PQ_IP_VIP_ICC_R_Main,  //129
PQ_IP_VIP_ICC_G_Main,  //130
PQ_IP_VIP_ICC_B_Main,  //131
PQ_IP_VIP_ICC_C_Main,  //132
PQ_IP_VIP_ICC_M_Main,  //133
PQ_IP_VIP_ICC_Y_Main,  //134
PQ_IP_VIP_ICC_F_Main,  //135
PQ_IP_VIP_ICC_NC_Main,  //136
PQ_IP_VIP_IBC_Main,  //137
PQ_IP_VIP_IBC_R_Main,  //138
PQ_IP_VIP_IBC_G_Main,  //139
PQ_IP_VIP_IBC_B_Main,  //140
PQ_IP_VIP_IBC_C_Main,  //141
PQ_IP_VIP_IBC_M_Main,  //142
PQ_IP_VIP_IBC_Y_Main,  //143
PQ_IP_VIP_IBC_F_Main,  //144
PQ_IP_VIP_DLC_Main,  //145
PQ_IP_VIP_DLC_dither_Main,  //146
PQ_IP_VIP_DLC_Flesh_adj_Main,  //147
PQ_IP_VIP_DLC_His_range_Main,  //148
PQ_IP_VIP_DLC_His_rangeH_Main,  //149
PQ_IP_VIP_DLC_His_rangeV_Main,  //150
PQ_IP_VIP_BLE_Main,  //151
PQ_IP_VIP_WLE_Main,  //152
PQ_IP_VIP_BWLE_dither_Main,  //153
PQ_IP_VIP_UVC_Main,  //154
PQ_IP_VIP_Post_Yoffset_Main,  //155
PQ_IP_VIP_Post_Ygain_Main,  //156
PQ_IP_VIP_Post_Cgain_Main,  //157
PQ_IP_VIP_Hcoring_Y_Main,  //158
PQ_IP_VIP_Hcoring_C_Main,  //159
PQ_IP_VIP_Hcoring_dither_Main,  //160
PQ_IP_SwDriver_Main,  //161
PQ_IP_3x3_Main,  //162
PQ_IP_RGB_Offset_Main,  //163
PQ_IP_RGB_Clip_Main,  //164
PQ_IP_xvYCC_bypass_Main,  //165
PQ_IP_xvYCC_de_gamma_Main,  //166
PQ_IP_xvYCC_de_gamma_dither_Main,  //167
PQ_IP_xvYCC_3x3_Main,  //168
PQ_IP_xvYCC_Comp_Main,  //169
PQ_IP_xvYCC_Comp_dither_Main,  //170
PQ_IP_xvYCC_Clip_Main,  //171
PQ_IP_xvYCC_gamma_Main,  //172
PQ_IP_xvYCC_gamma_dither_Main,  //173
PQ_IP_rgb_3d_Main,  //174
PQ_IP_HBC_Main,  //175
PQ_IP_Pre_CON_BRI_Main,  //176
PQ_IP_Blue_Stretch_Main,  //177
PQ_IP_Blue_Stretch_dither_Main,  //178
PQ_IP_Gamma_Main,  //179
PQ_IP_Gamma_dither_Main,  //180
PQ_IP_Post_CON_BRI_Main,  //181
PQ_IP_SRAM_3x3matrix_PIC2_Main,  //182
PQ_IP_SRAM_DLC_PIC1_Main,  //183
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Main,  //184
PQ_IP_SRAM_GammaTbl_R_PIC1_Main,  //185
PQ_IP_SRAM_GammaTbl_G_PIC1_Main,  //186
PQ_IP_SRAM_GammaTbl_B_PIC1_Main,  //187
PQ_IP_SRAM_Auto_Color_PIC2_Main,  //188
PQ_IP_SRAM_Color_Temp_PIC1_Main,  //189
 }   PQ_IPTYPE_Main;

typedef enum
{
PQ_IP_AFEC_COM_Main,  //0
PQ_IP_Comb_COM_Main,  //1
PQ_IP_Comb2_COM_Main,  //2
PQ_IP_SECAM_COM_Main,  //3
PQ_IP_VD_Sampling_no_comm_COM_Main,  //4
PQ_IP_ADC_Sampling_COM_Main,  //5
PQ_IP_SCinit_COM_Main,  //6
PQ_IP_CSC_COM_Main,  //7
PQ_IP_CSC_Dither_COM_Main,  //8
PQ_IP_YCdelay_COM_Main,  //9
PQ_IP_PreFilter_COM_Main,  //10
PQ_IP_PreFilter_Dither_COM_Main,  //11
PQ_IP_HDSDD_COM_Main,  //12
PQ_IP_HSD_Sampling_COM_Main,  //13
PQ_IP_HSD_Y_COM_Main,  //14
PQ_IP_HSD_C_COM_Main,  //15
PQ_IP_444To422_COM_Main,  //16
PQ_IP_VSD_COM_Main,  //17
PQ_IP_HVSD_Dither_COM_Main,  //18
PQ_IP_10to8_Dither_COM_Main,  //19
PQ_IP_MemFormat_COM_Main,  //20
PQ_IP_PreSNR_COM_Main,  //21
PQ_IP_PreSNR_Patch_COM_Main,  //22
PQ_IP_DNR_COM_Main,  //23
PQ_IP_DNR_Motion_COM_Main,  //24
PQ_IP_DNR_Y_COM_Main,  //25
PQ_IP_DNR_Y_COLOR_DEP_COM_Main,  //26
PQ_IP_SRAM_COLOR_INDEX_COM_Main,  //27
PQ_IP_SRAM_COLOR_GAIN_SNR_COM_Main,  //28
PQ_IP_SRAM_COLOR_GAIN_DNR_COM_Main,  //29
PQ_IP_DNR_Y_LUMA_ADAPTIVE_COM_Main,  //30
PQ_IP_DNR_POSTTUNE_COM_Main,  //31
PQ_IP_DNR_C_COM_Main,  //32
PQ_IP_HISDNR_COM_Main,  //33
PQ_IP_PNR_COM_Main,  //34
PQ_IP_PNR_Y_COM_Main,  //35
PQ_IP_PNR_C_COM_Main,  //36
PQ_IP_PostCCS_COM_Main,  //37
PQ_IP_PostCCS_Smooth_COM_Main,  //38
PQ_IP_420CUP_COM_Main,  //39
PQ_IP_MADi_COM_Main,  //40
PQ_IP_MADi_Motion_COM_Main,  //41
PQ_IP_MADi_ADP3x3_COM_Main,  //42
PQ_IP_MADi_MORPHO_COM_Main,  //43
PQ_IP_MADi_DFK_COM_Main,  //44
PQ_IP_MADi_SST_COM_Main,  //45
PQ_IP_MADi_EODiW_COM_Main,  //46
PQ_IP_MADi_Force_COM_Main,  //47
PQ_IP_EODi_COM_Main,  //48
PQ_IP_Film_COM_Main,  //49
PQ_IP_Film32_COM_Main,  //50
PQ_IP_Film22_COM_Main,  //51
PQ_IP_Film_any_COM_Main,  //52
PQ_IP_UCNR_COM_Main,  //53
PQ_IP_UCDi_COM_Main,  //54
PQ_IP_UC_CTL_COM_Main,  //55
PQ_IP_DIPF_COM_Main,  //56
PQ_IP_VCLPF_COM_Main,  //57
PQ_IP_Spike_NR_COM_Main,  //58
PQ_IP_SPF_COM_Main,  //59
PQ_IP_SPF_SNR_COM_Main,  //60
PQ_IP_SPF_SNR_MR_COM_Main,  //61
PQ_IP_SPF_MR_LPF_COM_Main,  //62
PQ_IP_SPF_NMR_Y_COM_Main,  //63
PQ_IP_SPF_NMR_Y_MR_COM_Main,  //64
PQ_IP_VSP_Y_COM_Main,  //65
PQ_IP_VSP_C_COM_Main,  //66
PQ_IP_VSP_CoRing_COM_Main,  //67
PQ_IP_VSP_DeRing_COM_Main,  //68
PQ_IP_VSP_Dither_COM_Main,  //69
PQ_IP_VSP_PreVBound_COM_Main,  //70
PQ_IP_422To444_COM_Main,  //71
PQ_IP_PreCTI_COM_Main,  //72
PQ_IP_HSP_Y_COM_Main,  //73
PQ_IP_HSP_C_COM_Main,  //74
PQ_IP_HSP_CoRing_COM_Main,  //75
PQ_IP_HSP_DeRing_COM_Main,  //76
PQ_IP_HSP_Dither_COM_Main,  //77
PQ_IP_HnonLinear_COM_Main,  //78
PQ_IP_SRAM1_COM_Main,  //79
PQ_IP_SRAM2_COM_Main,  //80
PQ_IP_SRAM3_COM_Main,  //81
PQ_IP_SRAM4_COM_Main,  //82
PQ_IP_C_SRAM1_COM_Main,  //83
PQ_IP_C_SRAM2_COM_Main,  //84
PQ_IP_C_SRAM3_COM_Main,  //85
PQ_IP_C_SRAM4_COM_Main,  //86
PQ_IP_VIP_COM_Main,  //87
PQ_IP_VIP_pseudo_COM_Main,  //88
PQ_IP_VIP_CSC_COM_Main,  //89
PQ_IP_VIP_CSC_dither_COM_Main,  //90
PQ_IP_VIP_Post_YC_delay_COM_Main,  //91
PQ_IP_VIP_HNMR_Y_COM_Main,  //92
PQ_IP_VIP_HNMR_ad_C_COM_Main,  //93
PQ_IP_VIP_HNMR_ad_C_gain_COM_Main,  //94
PQ_IP_VIP_HNMR_C_win1_COM_Main,  //95
PQ_IP_VIP_HNMR_C_win2_COM_Main,  //96
PQ_IP_VIP_HNMR_C_win3_COM_Main,  //97
PQ_IP_VIP_HNMR_C_win4_COM_Main,  //98
PQ_IP_VIP_Pre_Yoffset_COM_Main,  //99
PQ_IP_VIP_Pre_Ygain_COM_Main,  //100
PQ_IP_VIP_Pre_Ygain_dither_COM_Main,  //101
PQ_IP_VIP_HLPF_COM_Main,  //102
PQ_IP_VIP_HLPF_dither_COM_Main,  //103
PQ_IP_VIP_Peaking_COM_Main,  //104
PQ_IP_VIP_Peaking_band_COM_Main,  //105
PQ_IP_VIP_Peaking_Pcoring_COM_Main,  //106
PQ_IP_VIP_Peaking_gain_COM_Main,  //107
PQ_IP_VIP_Peaking_gain_ad_C_COM_Main,  //108
PQ_IP_VIP_Post_CTI_COM_Main,  //109
PQ_IP_VIP_Post_CTI_coef_COM_Main,  //110
PQ_IP_VIP_FCC_T1_COM_Main,  //111
PQ_IP_VIP_FCC_T2_COM_Main,  //112
PQ_IP_VIP_FCC_T3_COM_Main,  //113
PQ_IP_VIP_FCC_T4_COM_Main,  //114
PQ_IP_VIP_FCC_T5_COM_Main,  //115
PQ_IP_VIP_FCC_T6_COM_Main,  //116
PQ_IP_VIP_FCC_T7_COM_Main,  //117
PQ_IP_VIP_FCC_T8_COM_Main,  //118
PQ_IP_VIP_FCC_T9_COM_Main,  //119
PQ_IP_VIP_IHC_COM_Main,  //120
PQ_IP_VIP_IHC_R_COM_Main,  //121
PQ_IP_VIP_IHC_G_COM_Main,  //122
PQ_IP_VIP_IHC_B_COM_Main,  //123
PQ_IP_VIP_IHC_C_COM_Main,  //124
PQ_IP_VIP_IHC_M_COM_Main,  //125
PQ_IP_VIP_IHC_Y_COM_Main,  //126
PQ_IP_VIP_IHC_F_COM_Main,  //127
PQ_IP_VIP_ICC_COM_Main,  //128
PQ_IP_VIP_ICC_R_COM_Main,  //129
PQ_IP_VIP_ICC_G_COM_Main,  //130
PQ_IP_VIP_ICC_B_COM_Main,  //131
PQ_IP_VIP_ICC_C_COM_Main,  //132
PQ_IP_VIP_ICC_M_COM_Main,  //133
PQ_IP_VIP_ICC_Y_COM_Main,  //134
PQ_IP_VIP_ICC_F_COM_Main,  //135
PQ_IP_VIP_ICC_NC_COM_Main,  //136
PQ_IP_VIP_IBC_COM_Main,  //137
PQ_IP_VIP_IBC_R_COM_Main,  //138
PQ_IP_VIP_IBC_G_COM_Main,  //139
PQ_IP_VIP_IBC_B_COM_Main,  //140
PQ_IP_VIP_IBC_C_COM_Main,  //141
PQ_IP_VIP_IBC_M_COM_Main,  //142
PQ_IP_VIP_IBC_Y_COM_Main,  //143
PQ_IP_VIP_IBC_F_COM_Main,  //144
PQ_IP_VIP_DLC_COM_Main,  //145
PQ_IP_VIP_DLC_dither_COM_Main,  //146
PQ_IP_VIP_DLC_Flesh_adj_COM_Main,  //147
PQ_IP_VIP_DLC_His_range_COM_Main,  //148
PQ_IP_VIP_DLC_His_rangeH_COM_Main,  //149
PQ_IP_VIP_DLC_His_rangeV_COM_Main,  //150
PQ_IP_VIP_BLE_COM_Main,  //151
PQ_IP_VIP_WLE_COM_Main,  //152
PQ_IP_VIP_BWLE_dither_COM_Main,  //153
PQ_IP_VIP_UVC_COM_Main,  //154
PQ_IP_VIP_Post_Yoffset_COM_Main,  //155
PQ_IP_VIP_Post_Ygain_COM_Main,  //156
PQ_IP_VIP_Post_Cgain_COM_Main,  //157
PQ_IP_VIP_Hcoring_Y_COM_Main,  //158
PQ_IP_VIP_Hcoring_C_COM_Main,  //159
PQ_IP_VIP_Hcoring_dither_COM_Main,  //160
PQ_IP_SwDriver_COM_Main,  //161
PQ_IP_3x3_COM_Main,  //162
PQ_IP_RGB_Offset_COM_Main,  //163
PQ_IP_RGB_Clip_COM_Main,  //164
PQ_IP_xvYCC_bypass_COM_Main,  //165
PQ_IP_xvYCC_de_gamma_COM_Main,  //166
PQ_IP_xvYCC_de_gamma_dither_COM_Main,  //167
PQ_IP_xvYCC_3x3_COM_Main,  //168
PQ_IP_xvYCC_Comp_COM_Main,  //169
PQ_IP_xvYCC_Comp_dither_COM_Main,  //170
PQ_IP_xvYCC_Clip_COM_Main,  //171
PQ_IP_xvYCC_gamma_COM_Main,  //172
PQ_IP_xvYCC_gamma_dither_COM_Main,  //173
PQ_IP_rgb_3d_COM_Main,  //174
PQ_IP_HBC_COM_Main,  //175
PQ_IP_Pre_CON_BRI_COM_Main,  //176
PQ_IP_Blue_Stretch_COM_Main,  //177
PQ_IP_Blue_Stretch_dither_COM_Main,  //178
PQ_IP_Gamma_COM_Main,  //179
PQ_IP_Gamma_dither_COM_Main,  //180
PQ_IP_Post_CON_BRI_COM_Main,  //181
PQ_IP_SRAM_3x3matrix_PIC2_COM_Main,  //182
PQ_IP_SRAM_DLC_PIC1_COM_Main,  //183
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_COM_Main,  //184
PQ_IP_SRAM_GammaTbl_R_PIC1_COM_Main,  //185
PQ_IP_SRAM_GammaTbl_G_PIC1_COM_Main,  //186
PQ_IP_SRAM_GammaTbl_B_PIC1_COM_Main,  //187
PQ_IP_SRAM_Auto_Color_PIC2_COM_Main,  //188
PQ_IP_SRAM_Color_Temp_PIC1_COM_Main,  //189
PQ_IP_COM_NUMS_Main
} PQ_IP_COM_Group_Main;

typedef enum
{
PQ_IP_AFEC_ADCIN_Main,
PQ_IP_AFEC_RFIN_Main,
PQ_IP_AFEC_ADCIN_SECAM_Main,
PQ_IP_AFEC_NUMS_Main
} PQ_IP_AFEC_Group_Main;

typedef enum
{
PQ_IP_Comb_NTSC_AV_Main,
PQ_IP_Comb_PAL_AV_Main,
PQ_IP_Comb_NTSC_RF_Main,
PQ_IP_Comb_PAL_RF_Main,
PQ_IP_Comb_PAL_RF_VIF_Main,
PQ_IP_Comb_NTSC_SV_Main,
PQ_IP_Comb_PAL_SV_Main,
PQ_IP_Comb_NTSC443_Main,
PQ_IP_Comb_NTSC443_SV_Main,
PQ_IP_Comb_PAL_M_Main,
PQ_IP_Comb_PAL60_Main,
PQ_IP_Comb_SECAM_Main,
PQ_IP_Comb_NTSC_AV_SONY_Main,
PQ_IP_Comb_PAL_AV_SONY_Main,
PQ_IP_Comb_NTSC_RF_SONY_Main,
PQ_IP_Comb_PAL_RF_SONY_Main,
PQ_IP_Comb_NUMS_Main
} PQ_IP_Comb_Group_Main;

typedef enum
{
PQ_IP_Comb2_NTSC_AV_Main,
PQ_IP_Comb2_PAL_AV_Main,
PQ_IP_Comb2_NTSC_RF_Main,
PQ_IP_Comb2_PAL_RF_Main,
PQ_IP_Comb2_PAL_RF_VIF_Main,
PQ_IP_Comb2_NTSC_SV_Main,
PQ_IP_Comb2_PAL_SV_Main,
PQ_IP_Comb2_NTSC443_Main,
PQ_IP_Comb2_NTSC443_SV_Main,
PQ_IP_Comb2_PAL_M_Main,
PQ_IP_Comb2_PAL60_Main,
PQ_IP_Comb2_SECAM_Main,
PQ_IP_Comb2_NTSC_AV_SONY_Main,
PQ_IP_Comb2_PAL_AV_SONY_Main,
PQ_IP_Comb2_NTSC_RF_SONY_Main,
PQ_IP_Comb2_PAL_RF_SONY_Main,
PQ_IP_Comb2_NUMS_Main
} PQ_IP_Comb2_Group_Main;

typedef enum
{
PQ_IP_SECAM_ALL_Main,
PQ_IP_SECAM_NUMS_Main
} PQ_IP_SECAM_Group_Main;

typedef enum
{
PQ_IP_VD_Sampling_no_comm_N_1135_Main,
PQ_IP_VD_Sampling_no_comm_P_1135_Main,
PQ_IP_VD_Sampling_no_comm_P6_1135_Main,
PQ_IP_VD_Sampling_no_comm_S_1135_Main,
PQ_IP_VD_Sampling_no_comm_N4_1135_Main,
PQ_IP_VD_Sampling_no_comm_PM_1135_Main,
PQ_IP_VD_Sampling_no_comm_PNC_1135_Main,
PQ_IP_VD_Sampling_no_comm_N_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_P_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_P6_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_S_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_N4_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_PM_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_PNC_1135_1o5_Main,
PQ_IP_VD_Sampling_no_comm_N_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_P_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_P6_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_S_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_N4_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_PM_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_PNC_Dynamic_Main,
PQ_IP_VD_Sampling_no_comm_NUMS_Main
} PQ_IP_VD_Sampling_no_comm_Group_Main;

typedef enum
{
PQ_IP_ADC_Sampling_x1_Main,
PQ_IP_ADC_Sampling_x2_Main,
PQ_IP_ADC_Sampling_x4_Main,
PQ_IP_ADC_Sampling_NUMS_Main
} PQ_IP_ADC_Sampling_Group_Main;

typedef enum
{
PQ_IP_SCinit_Init_Main,
PQ_IP_SCinit_NUMS_Main
} PQ_IP_SCinit_Group_Main;

typedef enum
{
PQ_IP_CSC_OFF_Main,
PQ_IP_CSC_L_RGB2YCC_SD_Main,
PQ_IP_CSC_F_RGB2YCC_SD_Main,
PQ_IP_CSC_L_RGB2YCC_HD_Main,
PQ_IP_CSC_F_RGB2YCC_HD_Main,
PQ_IP_CSC_NUMS_Main
} PQ_IP_CSC_Group_Main;

typedef enum
{
PQ_IP_CSC_Dither_OFF_Main,
PQ_IP_CSC_Dither_ON_Main,
PQ_IP_CSC_Dither_NUMS_Main
} PQ_IP_CSC_Dither_Group_Main;

typedef enum
{
PQ_IP_YCdelay_OFF_Main,
PQ_IP_YCdelay_YC_91_Main,
PQ_IP_YCdelay_YC_92_Main,
PQ_IP_YCdelay_NUMS_Main
} PQ_IP_YCdelay_Group_Main;

typedef enum
{
PQ_IP_PreFilter_OFF_Main,
PQ_IP_PreFilter_FIR40_Main,
PQ_IP_PreFilter_FIR45_Main,
PQ_IP_PreFilter_FIR50_Main,
PQ_IP_PreFilter_FIR60_Main,
PQ_IP_PreFilter_FIR66_Main,
PQ_IP_PreFilter_FIR75_Main,
PQ_IP_PreFilter_FIR80_Main,
PQ_IP_PreFilter_FIR85_Main,
PQ_IP_PreFilter_FIR90_Main,
PQ_IP_PreFilter_FIR95_Main,
PQ_IP_PreFilter_NUMS_Main
} PQ_IP_PreFilter_Group_Main;

typedef enum
{
PQ_IP_PreFilter_Dither_OFF_Main,
PQ_IP_PreFilter_Dither_ON_Main,
PQ_IP_PreFilter_Dither_NUMS_Main
} PQ_IP_PreFilter_Dither_Group_Main;

typedef enum
{
PQ_IP_HDSDD_OFF_Main,
PQ_IP_HDSDD_D0_Main,
PQ_IP_HDSDD_D1_Main,
PQ_IP_HDSDD_D2_Main,
PQ_IP_HDSDD_D3_Main,
PQ_IP_HDSDD_D4_Main,
PQ_IP_HDSDD_D0_H_Main,
PQ_IP_HDSDD_D1_H_Main,
PQ_IP_HDSDD_D2_H_Main,
PQ_IP_HDSDD_D3_H_Main,
PQ_IP_HDSDD_D4_H_Main,
PQ_IP_HDSDD_NUMS_Main
} PQ_IP_HDSDD_Group_Main;

typedef enum
{
PQ_IP_HSD_Sampling_Div_1o000_Main,
PQ_IP_HSD_Sampling_Div_1o125_Main,
PQ_IP_HSD_Sampling_Div_1o250_Main,
PQ_IP_HSD_Sampling_Div_1o375_Main,
PQ_IP_HSD_Sampling_Div_1o500_Main,
PQ_IP_HSD_Sampling_Div_1o625_Main,
PQ_IP_HSD_Sampling_Div_1o750_Main,
PQ_IP_HSD_Sampling_Div_1o875_Main,
PQ_IP_HSD_Sampling_Div_2o000_Main,
PQ_IP_HSD_Sampling_NUMS_Main
} PQ_IP_HSD_Sampling_Group_Main;

typedef enum
{
PQ_IP_HSD_Y_OFF_Main,
PQ_IP_HSD_Y_CB_Main,
PQ_IP_HSD_Y_LpfFc40Ap0As40_Main,
PQ_IP_HSD_Y_LpfFc50Ap0As40_Main,
PQ_IP_HSD_Y_LpfFc60Ap0As35_Main,
PQ_IP_HSD_Y_LpfFc70Ap0As35_Main,
PQ_IP_HSD_Y_LpfFc80Ap0As35_Main,
PQ_IP_HSD_Y_LpfFc90Ap0As35_Main,
PQ_IP_HSD_Y_ALLPASS1X_Main,
PQ_IP_HSD_Y_FIR66_Main,
PQ_IP_HSD_Y_FIR60_Main,
PQ_IP_HSD_Y_FIR55_Main,
PQ_IP_HSD_Y_FIR50_Main,
PQ_IP_HSD_Y_NUMS_Main
} PQ_IP_HSD_Y_Group_Main;

typedef enum
{
PQ_IP_HSD_C_OFF_Main,
PQ_IP_HSD_C_CB_Main,
PQ_IP_HSD_C_LpfFc15Ap0As45_Main,
PQ_IP_HSD_C_LpfFc15Ap0As55_Main,
PQ_IP_HSD_C_ALLPASS1X_Main,
PQ_IP_HSD_C_FIR50_Main,
PQ_IP_HSD_C_FIR60_Main,
PQ_IP_HSD_C_NUMS_Main
} PQ_IP_HSD_C_Group_Main;

typedef enum
{
PQ_IP_444To422_OFF_Main,
PQ_IP_444To422_ON_Main,
PQ_IP_444To422_NUMS_Main
} PQ_IP_444To422_Group_Main;

typedef enum
{
PQ_IP_VSD_OFF_Main,
PQ_IP_VSD_CB_Main,
PQ_IP_VSD_Bilinear_Main,
PQ_IP_VSD_NUMS_Main
} PQ_IP_VSD_Group_Main;

typedef enum
{
PQ_IP_HVSD_Dither_OFF_Main,
PQ_IP_HVSD_Dither_Vdith_Main,
PQ_IP_HVSD_Dither_Hdith_Main,
PQ_IP_HVSD_Dither_HVDith_Main,
PQ_IP_HVSD_Dither_NUMS_Main
} PQ_IP_HVSD_Dither_Group_Main;

typedef enum
{
PQ_IP_10to8_Dither_OFF_Main,
PQ_IP_10to8_Dither_Rand_Dith_Main,
PQ_IP_10to8_Dither_Fix_Dith_Main,
PQ_IP_10to8_Dither_NUMS_Main
} PQ_IP_10to8_Dither_Group_Main;

typedef enum
{
PQ_IP_MemFormat_422MF_Main,
PQ_IP_MemFormat_444_10BIT_Main,
PQ_IP_MemFormat_444_8BIT_Main,
PQ_IP_MemFormat_NUMS_Main
} PQ_IP_MemFormat_Group_Main;

typedef enum
{
PQ_IP_PreSNR_OFF_Main,
PQ_IP_PreSNR_PS_1_Main,
PQ_IP_PreSNR_PS_2_Main,
PQ_IP_PreSNR_PS_3_Main,
PQ_IP_PreSNR_PS_4_Main,
PQ_IP_PreSNR_PS_5_Main,
PQ_IP_PreSNR_PS_6_Main,
PQ_IP_PreSNR_PS_7_Main,
PQ_IP_PreSNR_PS_8_Main,
PQ_IP_PreSNR_PS_9_Main,
PQ_IP_PreSNR_PS_10_Main,
PQ_IP_PreSNR_PS_11_Main,
PQ_IP_PreSNR_PS_12_Main,
PQ_IP_PreSNR_PS_13_Main,
PQ_IP_PreSNR_PS_14_Main,
PQ_IP_PreSNR_PS_15_Main,
PQ_IP_PreSNR_NUMS_Main
} PQ_IP_PreSNR_Group_Main;

typedef enum
{
PQ_IP_PreSNR_Patch_OFF_Main,
PQ_IP_PreSNR_Patch_PSP_1_Main,
PQ_IP_PreSNR_Patch_PSP_2_Main,
PQ_IP_PreSNR_Patch_NUMS_Main
} PQ_IP_PreSNR_Patch_Group_Main;

typedef enum
{
PQ_IP_DNR_NR_OFF_Main,
PQ_IP_DNR_DNR_OFF_Main,
PQ_IP_DNR_ON_Main,
PQ_IP_DNR_NUMS_Main
} PQ_IP_DNR_Group_Main;

typedef enum
{
PQ_IP_DNR_Motion_MR_NR_Main,
PQ_IP_DNR_Motion_MR_nonNR_Main,
PQ_IP_DNR_Motion_NUMS_Main
} PQ_IP_DNR_Motion_Group_Main;

typedef enum
{
PQ_IP_DNR_Y_OFF_Main,
PQ_IP_DNR_Y_DY_1_Main,
PQ_IP_DNR_Y_DY_1_1_Main,
PQ_IP_DNR_Y_DY_2_Main,
PQ_IP_DNR_Y_DY_2_1_Main,
PQ_IP_DNR_Y_DY_3_Main,
PQ_IP_DNR_Y_DY_3_1_Main,
PQ_IP_DNR_Y_DY_4_Main,
PQ_IP_DNR_Y_DY_4_1_Main,
PQ_IP_DNR_Y_DY_5_Main,
PQ_IP_DNR_Y_DY_5_1_Main,
PQ_IP_DNR_Y_DY_6_Main,
PQ_IP_DNR_Y_DY_6_1_Main,
PQ_IP_DNR_Y_DY_7_Main,
PQ_IP_DNR_Y_DY_7_1_Main,
PQ_IP_DNR_Y_DY_8_Main,
PQ_IP_DNR_Y_DY_8_1_Main,
PQ_IP_DNR_Y_DY_9_Main,
PQ_IP_DNR_Y_DY_9_1_Main,
PQ_IP_DNR_Y_DY_10_Main,
PQ_IP_DNR_Y_DY_10_1_Main,
PQ_IP_DNR_Y_DY_nonlinear0_Main,
PQ_IP_DNR_Y_DY_nonlinear01_Main,
PQ_IP_DNR_Y_DY_nonlinear1_Main,
PQ_IP_DNR_Y_DY_nonlinear12_Main,
PQ_IP_DNR_Y_DY_nonlinear2_Main,
PQ_IP_DNR_Y_DY_nonlinear23_Main,
PQ_IP_DNR_Y_DY_nonlinear3_Main,
PQ_IP_DNR_Y_NUMS_Main
} PQ_IP_DNR_Y_Group_Main;

typedef enum
{
PQ_IP_DNR_Y_COLOR_DEP_OFF_Main,
PQ_IP_DNR_Y_COLOR_DEP_DYCD_1_Main,
PQ_IP_DNR_Y_COLOR_DEP_NUMS_Main
} PQ_IP_DNR_Y_COLOR_DEP_Group_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_INDEX_Index0_Main,
PQ_IP_SRAM_COLOR_INDEX_NUMS_Main
} PQ_IP_SRAM_COLOR_INDEX_Group_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_SNR_GainSNR0_Main,
PQ_IP_SRAM_COLOR_GAIN_SNR_NUMS_Main
} PQ_IP_SRAM_COLOR_GAIN_SNR_Group_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR0_Main,
PQ_IP_SRAM_COLOR_GAIN_DNR_NUMS_Main
} PQ_IP_SRAM_COLOR_GAIN_DNR_Group_Main;

typedef enum
{
PQ_IP_DNR_Y_LUMA_ADAPTIVE_OFF_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_DYP_1_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_DYP_2_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_NUMS_Main
} PQ_IP_DNR_Y_LUMA_ADAPTIVE_Group_Main;

typedef enum
{
PQ_IP_DNR_POSTTUNE_OFF_Main,
PQ_IP_DNR_POSTTUNE_DYP_1_Main,
PQ_IP_DNR_POSTTUNE_NUMS_Main
} PQ_IP_DNR_POSTTUNE_Group_Main;

typedef enum
{
PQ_IP_DNR_C_OFF_Main,
PQ_IP_DNR_C_DC_1_Main,
PQ_IP_DNR_C_DC_1_1_Main,
PQ_IP_DNR_C_DC_2_Main,
PQ_IP_DNR_C_DC_2_1_Main,
PQ_IP_DNR_C_DC_3_Main,
PQ_IP_DNR_C_DC_3_1_Main,
PQ_IP_DNR_C_DC_4_Main,
PQ_IP_DNR_C_DC_4_1_Main,
PQ_IP_DNR_C_DC_5_Main,
PQ_IP_DNR_C_DC_5_1_Main,
PQ_IP_DNR_C_DC_6_Main,
PQ_IP_DNR_C_DC_6_1_Main,
PQ_IP_DNR_C_DC_7_Main,
PQ_IP_DNR_C_DC_7_1_Main,
PQ_IP_DNR_C_DC_8_Main,
PQ_IP_DNR_C_DC_8_1_Main,
PQ_IP_DNR_C_DC_9_Main,
PQ_IP_DNR_C_DC_9_1_Main,
PQ_IP_DNR_C_DC_10_Main,
PQ_IP_DNR_C_DC_10_1_Main,
PQ_IP_DNR_C_DC_nonlinear1_Main,
PQ_IP_DNR_C_DC_nonlinear2_Main,
PQ_IP_DNR_C_DC_nonlinear3_Main,
PQ_IP_DNR_C_NUMS_Main
} PQ_IP_DNR_C_Group_Main;

typedef enum
{
PQ_IP_HISDNR_OFF_Main,
PQ_IP_HISDNR_ON_Main,
PQ_IP_HISDNR_NUMS_Main
} PQ_IP_HISDNR_Group_Main;

typedef enum
{
PQ_IP_PNR_OFF_Main,
PQ_IP_PNR_PNR_PCCS_AVG_OFF_Main,
PQ_IP_PNR_ON_Main,
PQ_IP_PNR_AVG_ON_Main,
PQ_IP_PNR_NUMS_Main
} PQ_IP_PNR_Group_Main;

typedef enum
{
PQ_IP_PNR_Y_OFF_Main,
PQ_IP_PNR_Y_PY1_Main,
PQ_IP_PNR_Y_NUMS_Main
} PQ_IP_PNR_Y_Group_Main;

typedef enum
{
PQ_IP_PNR_C_OFF_Main,
PQ_IP_PNR_C_PC1_Main,
PQ_IP_PNR_C_NUMS_Main
} PQ_IP_PNR_C_Group_Main;

typedef enum
{
PQ_IP_PostCCS_OFF_Main,
PQ_IP_PostCCS_PC_0_Main,
PQ_IP_PostCCS_PC_1_Main,
PQ_IP_PostCCS_PC_2_Main,
PQ_IP_PostCCS_PC_3_Main,
PQ_IP_PostCCS_PC_4_Main,
PQ_IP_PostCCS_PC_5_Main,
PQ_IP_PostCCS_PC_6_Main,
PQ_IP_PostCCS_PC_7_Main,
PQ_IP_PostCCS_PC_8_Main,
PQ_IP_PostCCS_PC_9_Main,
PQ_IP_PostCCS_PC_10_Main,
PQ_IP_PostCCS_PC_11_Main,
PQ_IP_PostCCS_PC_12_Main,
PQ_IP_PostCCS_PC_13_Main,
PQ_IP_PostCCS_PC_14_Main,
PQ_IP_PostCCS_PC_15_Main,
PQ_IP_PostCCS_PC_16_Main,
PQ_IP_PostCCS_PC_17_Main,
PQ_IP_PostCCS_PC_18_Main,
PQ_IP_PostCCS_PC_19_Main,
PQ_IP_PostCCS_PC_20_Main,
PQ_IP_PostCCS_PC_21_Main,
PQ_IP_PostCCS_PC_22_Main,
PQ_IP_PostCCS_PC_23_Main,
PQ_IP_PostCCS_PC_24_Main,
PQ_IP_PostCCS_NUMS_Main
} PQ_IP_PostCCS_Group_Main;

typedef enum
{
PQ_IP_PostCCS_Smooth_OFF_Main,
PQ_IP_PostCCS_Smooth_PCS_0_Main,
PQ_IP_PostCCS_Smooth_PCS_1_Main,
PQ_IP_PostCCS_Smooth_PCS_2_Main,
PQ_IP_PostCCS_Smooth_PCS_3_Main,
PQ_IP_PostCCS_Smooth_PCS_4_Main,
PQ_IP_PostCCS_Smooth_PCS_5_Main,
PQ_IP_PostCCS_Smooth_PCS_6_Main,
PQ_IP_PostCCS_Smooth_PCS_7_Main,
PQ_IP_PostCCS_Smooth_NUMS_Main
} PQ_IP_PostCCS_Smooth_Group_Main;

typedef enum
{
PQ_IP_420CUP_OFF_Main,
PQ_IP_420CUP_ON_Main,
PQ_IP_420CUP_NUMS_Main
} PQ_IP_420CUP_Group_Main;

typedef enum
{
PQ_IP_MADi_24_4R_Main,
PQ_IP_MADi_24_2R_Main,
PQ_IP_MADi_25_4R_Main,
PQ_IP_MADi_25_2R_Main,
PQ_IP_MADi_26_4R_Main,
PQ_IP_MADi_26_2R_Main,
PQ_IP_MADi_27_4R_Main,
PQ_IP_MADi_27_2R_Main,
PQ_IP_MADi_P_MODE8_Main,
PQ_IP_MADi_P_MODE10_Main,
PQ_IP_MADi_P_MODE_MOT10_8Frame_Main,
PQ_IP_MADi_P_MODE_MOT10_Main,
PQ_IP_MADi_P_MODE_MOT8_Main,
PQ_IP_MADi_24_4R_880_Main,
PQ_IP_MADi_24_2R_880_Main,
PQ_IP_MADi_25_4R_880_Main,
PQ_IP_MADi_25_4R_884_Main,
PQ_IP_MADi_25_2R_884_Main,
PQ_IP_MADi_25_2R_880_Main,
PQ_IP_MADi_25_4R_MC_Main,
PQ_IP_MADi_25_4R_MC_NW_Main,
PQ_IP_MADi_25_6R_MC_NW_Main,
PQ_IP_MADi_25_6R_MC_Main,
PQ_IP_MADi_25_12F_8R_MC_Main,
PQ_IP_MADi_25_14F_8R_MC_Main,
PQ_IP_MADi_25_16F_8R_MC_Main,
PQ_IP_MADi_P_MODE8_444_Main,
PQ_IP_MADi_P_MODE10_444_Main,
PQ_IP_MADi_P_MODE_MOT10_4Frame_Main,
PQ_IP_MADi_RFBL_2D_Main,
PQ_IP_MADi_RFBL_25D_Main,
PQ_IP_MADi_RFBL_3D_Main,
PQ_IP_MADi_25_14F_6R_MC_Main,
PQ_IP_MADi_P_MODE_MC_3Frame_Main,
PQ_IP_MADi_P_MODE_MC_6Frame_6R_Main,
PQ_IP_MADi_P_MODE_MC_12Frame_8R_Main,
PQ_IP_MADi_RFBL_MC_Main,
PQ_IP_MADi_RFBL_P_MODE_MC_Main,
PQ_IP_MADi_NUMS_Main
} PQ_IP_MADi_Group_Main;

typedef enum
{
PQ_IP_MADi_Motion_MOT_4R_5_Main,
PQ_IP_MADi_Motion_MOT_4R_6_Main,
PQ_IP_MADi_Motion_MOT_4R_7_Main,
PQ_IP_MADi_Motion_MOT_2R_Main,
PQ_IP_MADi_Motion_MOT_4R_5_MC_Main,
PQ_IP_MADi_Motion_MOT_PMODE_Main,
PQ_IP_MADi_Motion_NUMS_Main
} PQ_IP_MADi_Motion_Group_Main;

typedef enum
{
PQ_IP_MADi_ADP3x3_OFF_Main,
PQ_IP_MADi_ADP3x3_ADP1_Main,
PQ_IP_MADi_ADP3x3_NUMS_Main
} PQ_IP_MADi_ADP3x3_Group_Main;

typedef enum
{
PQ_IP_MADi_MORPHO_OFF_Main,
PQ_IP_MADi_MORPHO_M1_Main,
PQ_IP_MADi_MORPHO_NUMS_Main
} PQ_IP_MADi_MORPHO_Group_Main;

typedef enum
{
PQ_IP_MADi_DFK_OFF_Main,
PQ_IP_MADi_DFK_DFK1_Main,
PQ_IP_MADi_DFK_DFK2_Main,
PQ_IP_MADi_DFK_DFK3_Main,
PQ_IP_MADi_DFK_DFK4_Main,
PQ_IP_MADi_DFK_DFK5_Main,
PQ_IP_MADi_DFK_DFK6_Main,
PQ_IP_MADi_DFK_NUMS_Main
} PQ_IP_MADi_DFK_Group_Main;

typedef enum
{
PQ_IP_MADi_SST_OFF_Main,
PQ_IP_MADi_SST_SST1_Main,
PQ_IP_MADi_SST_SST2_Main,
PQ_IP_MADi_SST_SST3_Main,
PQ_IP_MADi_SST_SST4_Main,
PQ_IP_MADi_SST_SST5_Main,
PQ_IP_MADi_SST_SST6_Main,
PQ_IP_MADi_SST_SST_Rec1_Main,
PQ_IP_MADi_SST_NUMS_Main
} PQ_IP_MADi_SST_Group_Main;

typedef enum
{
PQ_IP_MADi_EODiW_OFF_Main,
PQ_IP_MADi_EODiW_W1_Main,
PQ_IP_MADi_EODiW_W2_Main,
PQ_IP_MADi_EODiW_W3_Main,
PQ_IP_MADi_EODiW_NUMS_Main
} PQ_IP_MADi_EODiW_Group_Main;

typedef enum
{
PQ_IP_MADi_Force_OFF_Main,
PQ_IP_MADi_Force_YC_FullMotion_Main,
PQ_IP_MADi_Force_YC_FullStill_Main,
PQ_IP_MADi_Force_Y_FullMotion_Main,
PQ_IP_MADi_Force_Y_FullStill_Main,
PQ_IP_MADi_Force_C_FullMotion_Main,
PQ_IP_MADi_Force_C_FullStill_Main,
PQ_IP_MADi_Force_NUMS_Main
} PQ_IP_MADi_Force_Group_Main;

typedef enum
{
PQ_IP_EODi_OFF_Main,
PQ_IP_EODi_SD_15_Main,
PQ_IP_EODi_SD_14_Main,
PQ_IP_EODi_SD_13_Main,
PQ_IP_EODi_SD_12_Main,
PQ_IP_EODi_SD_11_Main,
PQ_IP_EODi_SD_10_Main,
PQ_IP_EODi_SD_9_Main,
PQ_IP_EODi_SD_8_Main,
PQ_IP_EODi_SD_7_Main,
PQ_IP_EODi_SD_6_Main,
PQ_IP_EODi_SD_5_Main,
PQ_IP_EODi_SD_4_Main,
PQ_IP_EODi_SD_3_Main,
PQ_IP_EODi_SD_2_Main,
PQ_IP_EODi_SD_1_Main,
PQ_IP_EODi_HD_15_Main,
PQ_IP_EODi_HD_14_Main,
PQ_IP_EODi_HD_13_Main,
PQ_IP_EODi_HD_12_Main,
PQ_IP_EODi_HD_11_Main,
PQ_IP_EODi_HD_10_Main,
PQ_IP_EODi_HD_9_Main,
PQ_IP_EODi_HD_8_Main,
PQ_IP_EODi_HD_7_Main,
PQ_IP_EODi_HD_6_Main,
PQ_IP_EODi_HD_5_Main,
PQ_IP_EODi_HD_4_Main,
PQ_IP_EODi_HD_3_Main,
PQ_IP_EODi_HD_2_Main,
PQ_IP_EODi_HD_1_Main,
PQ_IP_EODi_NUMS_Main
} PQ_IP_EODi_Group_Main;

typedef enum
{
PQ_IP_Film_OFF_Main,
PQ_IP_Film_SD_2_Main,
PQ_IP_Film_SD_1_Main,
PQ_IP_Film_SD_3_Main,
PQ_IP_Film_HD_2_Main,
PQ_IP_Film_HD_1_Main,
PQ_IP_Film_HD_3_Main,
PQ_IP_Film_NUMS_Main
} PQ_IP_Film_Group_Main;

typedef enum
{
PQ_IP_Film32_OFF_Main,
PQ_IP_Film32_SD_1_Main,
PQ_IP_Film32_SD_2_Main,
PQ_IP_Film32_SD_3_Main,
PQ_IP_Film32_SD_4_Main,
PQ_IP_Film32_SD_5_Main,
PQ_IP_Film32_SD_6_Main,
PQ_IP_Film32_SD_7_Main,
PQ_IP_Film32_SD_8_Main,
PQ_IP_Film32_SD_9_Main,
PQ_IP_Film32_SD_10_Main,
PQ_IP_Film32_HD_1_Main,
PQ_IP_Film32_HD_2_Main,
PQ_IP_Film32_HD_3_Main,
PQ_IP_Film32_HD_4_Main,
PQ_IP_Film32_HD_5_Main,
PQ_IP_Film32_HD_6_Main,
PQ_IP_Film32_HD_1_MC_Main,
PQ_IP_Film32_HD_2_MC_Main,
PQ_IP_Film32_HD_3_MC_Main,
PQ_IP_Film32_HD_4_MC_Main,
PQ_IP_Film32_HD_5_MC_Main,
PQ_IP_Film32_HD_6_MC_Main,
PQ_IP_Film32_NUMS_Main
} PQ_IP_Film32_Group_Main;

typedef enum
{
PQ_IP_Film22_OFF_Main,
PQ_IP_Film22_SD_1_Main,
PQ_IP_Film22_SD_2_Main,
PQ_IP_Film22_SD_3_Main,
PQ_IP_Film22_SD_4_Main,
PQ_IP_Film22_SD_5_Main,
PQ_IP_Film22_SD_6_Main,
PQ_IP_Film22_SD_7_Main,
PQ_IP_Film22_SD_8_Main,
PQ_IP_Film22_SD_9_Main,
PQ_IP_Film22_SD_10_Main,
PQ_IP_Film22_HD_1_Main,
PQ_IP_Film22_HD_2_Main,
PQ_IP_Film22_HD_3_Main,
PQ_IP_Film22_HD_4_Main,
PQ_IP_Film22_HD_5_Main,
PQ_IP_Film22_HD_6_Main,
PQ_IP_Film22_NUMS_Main
} PQ_IP_Film22_Group_Main;

typedef enum
{
PQ_IP_Film_any_OFF_Main,
PQ_IP_Film_any_SD_1_Main,
PQ_IP_Film_any_SD_2_Main,
PQ_IP_Film_any_SD_3_Main,
PQ_IP_Film_any_SD_4_Main,
PQ_IP_Film_any_SD_5_Main,
PQ_IP_Film_any_SD_6_Main,
PQ_IP_Film_any_SD_7_Main,
PQ_IP_Film_any_SD_8_Main,
PQ_IP_Film_any_SD_9_Main,
PQ_IP_Film_any_SD_10_Main,
PQ_IP_Film_any_HD_1_Main,
PQ_IP_Film_any_HD_2_Main,
PQ_IP_Film_any_HD_3_Main,
PQ_IP_Film_any_HD_4_Main,
PQ_IP_Film_any_HD_5_Main,
PQ_IP_Film_any_HD_6_Main,
PQ_IP_Film_any_NUMS_Main
} PQ_IP_Film_any_Group_Main;

typedef enum
{
PQ_IP_UCNR_OFF_Main,
PQ_IP_UCNR_SD_0_Main,
PQ_IP_UCNR_SD_1_Main,
PQ_IP_UCNR_SD_2_Main,
PQ_IP_UCNR_SD_3_Main,
PQ_IP_UCNR_SD_4_Main,
PQ_IP_UCNR_HD_Main,
PQ_IP_UCNR_HD_1_Main,
PQ_IP_UCNR_SD_5_Main,
PQ_IP_UCNR_NUMS_Main
} PQ_IP_UCNR_Group_Main;

typedef enum
{
PQ_IP_UCDi_OFF_Main,
PQ_IP_UCDi_SD_1_Main,
PQ_IP_UCDi_SD_2_Main,
PQ_IP_UCDi_NUMS_Main
} PQ_IP_UCDi_Group_Main;

typedef enum
{
PQ_IP_UC_CTL_OFF_Main,
PQ_IP_UC_CTL_ON_Main,
PQ_IP_UC_CTL_ON_DHD_Main,
PQ_IP_UC_CTL_NUMS_Main
} PQ_IP_UC_CTL_Group_Main;

typedef enum
{
PQ_IP_DIPF_OFF_Main,
PQ_IP_DIPF_DIPF1_Main,
PQ_IP_DIPF_NUMS_Main
} PQ_IP_DIPF_Group_Main;

typedef enum
{
PQ_IP_VCLPF_OFF_Main,
PQ_IP_VCLPF_ON_Main,
PQ_IP_VCLPF_NUMS_Main
} PQ_IP_VCLPF_Group_Main;

typedef enum
{
PQ_IP_Spike_NR_OFF_Main,
PQ_IP_Spike_NR_S1_Main,
PQ_IP_Spike_NR_S2_Main,
PQ_IP_Spike_NR_S3_Main,
PQ_IP_Spike_NR_NUMS_Main
} PQ_IP_Spike_NR_Group_Main;

typedef enum
{
PQ_IP_SPF_OFF_Main,
PQ_IP_SPF_ON_Main,
PQ_IP_SPF_NUMS_Main
} PQ_IP_SPF_Group_Main;

typedef enum
{
PQ_IP_SPF_SNR_OFF_Main,
PQ_IP_SPF_SNR_E1S1_Main,
PQ_IP_SPF_SNR_E1S2_Main,
PQ_IP_SPF_SNR_E2S1_Main,
PQ_IP_SPF_SNR_E3S1_Main,
PQ_IP_SPF_SNR_E4S1_Main,
PQ_IP_SPF_SNR_E4S2_Main,
PQ_IP_SPF_SNR_E4S3_Main,
PQ_IP_SPF_SNR_NUMS_Main
} PQ_IP_SPF_SNR_Group_Main;

typedef enum
{
PQ_IP_SPF_SNR_MR_OFF_Main,
PQ_IP_SPF_SNR_MR_ON_Main,
PQ_IP_SPF_SNR_MR_NUMS_Main
} PQ_IP_SPF_SNR_MR_Group_Main;

typedef enum
{
PQ_IP_SPF_MR_LPF_OFF_Main,
PQ_IP_SPF_MR_LPF_LPF3x3_Main,
PQ_IP_SPF_MR_LPF_NUMS_Main
} PQ_IP_SPF_MR_LPF_Group_Main;

typedef enum
{
PQ_IP_SPF_NMR_Y_OFF_Main,
PQ_IP_SPF_NMR_Y_S1_Main,
PQ_IP_SPF_NMR_Y_S2_Main,
PQ_IP_SPF_NMR_Y_S3_Main,
PQ_IP_SPF_NMR_Y_NUMS_Main
} PQ_IP_SPF_NMR_Y_Group_Main;

typedef enum
{
PQ_IP_SPF_NMR_Y_MR_OFF_Main,
PQ_IP_SPF_NMR_Y_MR_ON_Main,
PQ_IP_SPF_NMR_Y_MR_NUMS_Main
} PQ_IP_SPF_NMR_Y_MR_Group_Main;

typedef enum
{
PQ_IP_VSP_Y_Bypass_Main,
PQ_IP_VSP_Y_Bilinear_Main,
PQ_IP_VSP_Y_SRAM_1_4Tap_Main,
PQ_IP_VSP_Y_SRAM_2_4Tap_Main,
PQ_IP_VSP_Y_SRAM_1_6Tap_Main,
PQ_IP_VSP_Y_SRAM_2_6Tap_Main,
PQ_IP_VSP_Y_NUMS_Main
} PQ_IP_VSP_Y_Group_Main;

typedef enum
{
PQ_IP_VSP_C_Bypass_Main,
PQ_IP_VSP_C_Bilinear_Main,
PQ_IP_VSP_C_C_SRAM_1_Main,
PQ_IP_VSP_C_C_SRAM_2_Main,
PQ_IP_VSP_C_C_SRAM_3_Main,
PQ_IP_VSP_C_C_SRAM_4_Main,
PQ_IP_VSP_C_SRAM_1_4Tap_Main,
PQ_IP_VSP_C_SRAM_2_4Tap_Main,
PQ_IP_VSP_C_NUMS_Main
} PQ_IP_VSP_C_Group_Main;

typedef enum
{
PQ_IP_VSP_CoRing_OFF_Main,
PQ_IP_VSP_CoRing_Y_Coring_1_Main,
PQ_IP_VSP_CoRing_Y_Coring_2_Main,
PQ_IP_VSP_CoRing_Y_Coring_3_Main,
PQ_IP_VSP_CoRing_Y_Coring_4_Main,
PQ_IP_VSP_CoRing_Y_Coring_5_Main,
PQ_IP_VSP_CoRing_NUMS_Main
} PQ_IP_VSP_CoRing_Group_Main;

typedef enum
{
PQ_IP_VSP_DeRing_OFF_Main,
PQ_IP_VSP_DeRing_DR1_Main,
PQ_IP_VSP_DeRing_NUMS_Main
} PQ_IP_VSP_DeRing_Group_Main;

typedef enum
{
PQ_IP_VSP_Dither_OFF_Main,
PQ_IP_VSP_Dither_ON_Main,
PQ_IP_VSP_Dither_NUMS_Main
} PQ_IP_VSP_Dither_Group_Main;

typedef enum
{
PQ_IP_VSP_PreVBound_OFF_Main,
PQ_IP_VSP_PreVBound_ON_Main,
PQ_IP_VSP_PreVBound_NUMS_Main
} PQ_IP_VSP_PreVBound_Group_Main;

typedef enum
{
PQ_IP_422To444_ON_Main,
PQ_IP_422To444_OFF_Main,
PQ_IP_422To444_NUMS_Main
} PQ_IP_422To444_Group_Main;

typedef enum
{
PQ_IP_PreCTI_OFF_Main,
PQ_IP_PreCTI_CTI_0_Main,
PQ_IP_PreCTI_CTI_1_Main,
PQ_IP_PreCTI_NUMS_Main
} PQ_IP_PreCTI_Group_Main;

typedef enum
{
PQ_IP_HSP_Y_Bypass_Main,
PQ_IP_HSP_Y_Bilinear_Main,
PQ_IP_HSP_Y_SRAM_1_4Tap_Main,
PQ_IP_HSP_Y_SRAM_2_4Tap_Main,
PQ_IP_HSP_Y_SRAM_1_6Tap_Main,
PQ_IP_HSP_Y_SRAM_2_6Tap_Main,
PQ_IP_HSP_Y_NUMS_Main
} PQ_IP_HSP_Y_Group_Main;

typedef enum
{
PQ_IP_HSP_C_Bypass_Main,
PQ_IP_HSP_C_Bilinear_Main,
PQ_IP_HSP_C_C_SRAM_1_Main,
PQ_IP_HSP_C_C_SRAM_2_Main,
PQ_IP_HSP_C_C_SRAM_3_Main,
PQ_IP_HSP_C_C_SRAM_4_Main,
PQ_IP_HSP_C_SRAM_1_4Tap_Main,
PQ_IP_HSP_C_SRAM_2_4Tap_Main,
PQ_IP_HSP_C_NUMS_Main
} PQ_IP_HSP_C_Group_Main;

typedef enum
{
PQ_IP_HSP_CoRing_OFF_Main,
PQ_IP_HSP_CoRing_Y_Coring_1_Main,
PQ_IP_HSP_CoRing_Y_Coring_2_Main,
PQ_IP_HSP_CoRing_Y_Coring_3_Main,
PQ_IP_HSP_CoRing_Y_Coring_4_Main,
PQ_IP_HSP_CoRing_Y_Coring_5_Main,
PQ_IP_HSP_CoRing_NUMS_Main
} PQ_IP_HSP_CoRing_Group_Main;

typedef enum
{
PQ_IP_HSP_DeRing_OFF_Main,
PQ_IP_HSP_DeRing_DR1_Main,
PQ_IP_HSP_DeRing_NUMS_Main
} PQ_IP_HSP_DeRing_Group_Main;

typedef enum
{
PQ_IP_HSP_Dither_OFF_Main,
PQ_IP_HSP_Dither_ON_Main,
PQ_IP_HSP_Dither_NUMS_Main
} PQ_IP_HSP_Dither_Group_Main;

typedef enum
{
PQ_IP_HnonLinear_OFF_Main,
PQ_IP_HnonLinear_H_1366_0_Main,
PQ_IP_HnonLinear_H_1366_1_Main,
PQ_IP_HnonLinear_H_1366_2_Main,
PQ_IP_HnonLinear_H_1440_Main,
PQ_IP_HnonLinear_H_1680_Main,
PQ_IP_HnonLinear_H_1920_0_Main,
PQ_IP_HnonLinear_H_1920_1_Main,
PQ_IP_HnonLinear_H_1920_2_Main,
PQ_IP_HnonLinear_NUMS_Main
} PQ_IP_HnonLinear_Group_Main;

typedef enum
{
PQ_IP_SRAM1_InvSinc4Tc4p4Fc45Apass01Astop40_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc50Apass01Astop55_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc65Apass3Astop60G14_Main,
PQ_IP_SRAM1_InvSinc3Tc0p0Fc75Ap001As60G13_Main,
PQ_IP_SRAM1_InvSinc3Tc0p0Fc75Ap001As60_Main,
PQ_IP_SRAM1_NUMS_Main
} PQ_IP_SRAM1_Group_Main;

typedef enum
{
PQ_IP_SRAM2_InvSinc4Tc4p4Fc45Apass01Astop40_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc50Apass01Astop55_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc65Apass3Astop60G14_Main,
PQ_IP_SRAM2_InvSinc3Tc0p0Fc75Ap001As60G13_Main,
PQ_IP_SRAM2_InvSinc3Tc0p0Fc75Ap001As60_Main,
PQ_IP_SRAM2_NUMS_Main
} PQ_IP_SRAM2_Group_Main;

typedef enum
{
PQ_IP_SRAM3_OFF_Main,
PQ_IP_SRAM3_NUMS_Main
} PQ_IP_SRAM3_Group_Main;

typedef enum
{
PQ_IP_SRAM4_OFF_Main,
PQ_IP_SRAM4_NUMS_Main
} PQ_IP_SRAM4_Group_Main;

typedef enum
{
PQ_IP_C_SRAM1_C2121_Main,
PQ_IP_C_SRAM1_C121_Main,
PQ_IP_C_SRAM1_NUMS_Main
} PQ_IP_C_SRAM1_Group_Main;

typedef enum
{
PQ_IP_C_SRAM2_C2121_Main,
PQ_IP_C_SRAM2_C121_Main,
PQ_IP_C_SRAM2_NUMS_Main
} PQ_IP_C_SRAM2_Group_Main;

typedef enum
{
PQ_IP_C_SRAM3_C2121_Main,
PQ_IP_C_SRAM3_C121_Main,
PQ_IP_C_SRAM3_NUMS_Main
} PQ_IP_C_SRAM3_Group_Main;

typedef enum
{
PQ_IP_C_SRAM4_C2121_Main,
PQ_IP_C_SRAM4_C121_Main,
PQ_IP_C_SRAM4_NUMS_Main
} PQ_IP_C_SRAM4_Group_Main;

typedef enum
{
PQ_IP_VIP_OFF_Main,
PQ_IP_VIP_ON_Main,
PQ_IP_VIP_NUMS_Main
} PQ_IP_VIP_Group_Main;

typedef enum
{
PQ_IP_VIP_pseudo_OFF_Main,
PQ_IP_VIP_pseudo_ON_Main,
PQ_IP_VIP_pseudo_NUMS_Main
} PQ_IP_VIP_pseudo_Group_Main;

typedef enum
{
PQ_IP_VIP_CSC_OFF_Main,
PQ_IP_VIP_CSC_L_RGB2YCC_SD_Main,
PQ_IP_VIP_CSC_F_RGB2YCC_SD_Main,
PQ_IP_VIP_CSC_L_RGB2YCC_HD_Main,
PQ_IP_VIP_CSC_F_RGB2YCC_HD_Main,
PQ_IP_VIP_CSC_NUMS_Main
} PQ_IP_VIP_CSC_Group_Main;

typedef enum
{
PQ_IP_VIP_CSC_dither_OFF_Main,
PQ_IP_VIP_CSC_dither_ON_Main,
PQ_IP_VIP_CSC_dither_NUMS_Main
} PQ_IP_VIP_CSC_dither_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_YC_delay_OFF_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb0Cr0_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb1Cr0_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb2Cr0_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb3Cr0_Main,
PQ_IP_VIP_Post_YC_delay_Y1Cb0Cr0_Main,
PQ_IP_VIP_Post_YC_delay_NUMS_Main
} PQ_IP_VIP_Post_YC_delay_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_Y_OFF_Main,
PQ_IP_VIP_HNMR_Y_S1_Main,
PQ_IP_VIP_HNMR_Y_S2_Main,
PQ_IP_VIP_HNMR_Y_S3_Main,
PQ_IP_VIP_HNMR_Y_NUMS_Main
} PQ_IP_VIP_HNMR_Y_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_OFF_Main,
PQ_IP_VIP_HNMR_ad_C_Yon_Con_Main,
PQ_IP_VIP_HNMR_ad_C_Yon_Coff_Main,
PQ_IP_VIP_HNMR_ad_C_Yoff_Con_Main,
PQ_IP_VIP_HNMR_ad_C_NUMS_Main
} PQ_IP_VIP_HNMR_ad_C_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_gain_S0_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S1_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S2_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S3_Main,
PQ_IP_VIP_HNMR_ad_C_gain_NUMS_Main
} PQ_IP_VIP_HNMR_ad_C_gain_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win1_OFF_Main,
PQ_IP_VIP_HNMR_C_win1_Flesh_1_Main,
PQ_IP_VIP_HNMR_C_win1_Flesh_2_Main,
PQ_IP_VIP_HNMR_C_win1_NUMS_Main
} PQ_IP_VIP_HNMR_C_win1_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win2_OFF_Main,
PQ_IP_VIP_HNMR_C_win2_Blue_1_Main,
PQ_IP_VIP_HNMR_C_win2_Blue_2_Main,
PQ_IP_VIP_HNMR_C_win2_NUMS_Main
} PQ_IP_VIP_HNMR_C_win2_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win3_OFF_Main,
PQ_IP_VIP_HNMR_C_win3_Green_1_Main,
PQ_IP_VIP_HNMR_C_win3_NUMS_Main
} PQ_IP_VIP_HNMR_C_win3_Group_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win4_OFF_Main,
PQ_IP_VIP_HNMR_C_win4_NUMS_Main
} PQ_IP_VIP_HNMR_C_win4_Group_Main;

typedef enum
{
PQ_IP_VIP_Pre_Yoffset_OFF_Main,
PQ_IP_VIP_Pre_Yoffset_0x05_Main,
PQ_IP_VIP_Pre_Yoffset_0xF0_Main,
PQ_IP_VIP_Pre_Yoffset_0xF4_Main,
PQ_IP_VIP_Pre_Yoffset_0xF6_Main,
PQ_IP_VIP_Pre_Yoffset_0xF7_Main,
PQ_IP_VIP_Pre_Yoffset_0xFA_Main,
PQ_IP_VIP_Pre_Yoffset_NUMS_Main
} PQ_IP_VIP_Pre_Yoffset_Group_Main;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_OFF_Main,
PQ_IP_VIP_Pre_Ygain_0x42_Main,
PQ_IP_VIP_Pre_Ygain_0x4A_Main,
PQ_IP_VIP_Pre_Ygain_NUMS_Main
} PQ_IP_VIP_Pre_Ygain_Group_Main;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_dither_OFF_Main,
PQ_IP_VIP_Pre_Ygain_dither_ON_Main,
PQ_IP_VIP_Pre_Ygain_dither_NUMS_Main
} PQ_IP_VIP_Pre_Ygain_dither_Group_Main;

typedef enum
{
PQ_IP_VIP_HLPF_OFF_Main,
PQ_IP_VIP_HLPF_0x1_Main,
PQ_IP_VIP_HLPF_0x2_Main,
PQ_IP_VIP_HLPF_0x3_Main,
PQ_IP_VIP_HLPF_0x4_Main,
PQ_IP_VIP_HLPF_0x5_Main,
PQ_IP_VIP_HLPF_0x6_Main,
PQ_IP_VIP_HLPF_0x7_Main,
PQ_IP_VIP_HLPF_NUMS_Main
} PQ_IP_VIP_HLPF_Group_Main;

typedef enum
{
PQ_IP_VIP_HLPF_dither_OFF_Main,
PQ_IP_VIP_HLPF_dither_ON_Main,
PQ_IP_VIP_HLPF_dither_NUMS_Main
} PQ_IP_VIP_HLPF_dither_Group_Main;

typedef enum
{
PQ_IP_VIP_Peaking_OFF_Main,
PQ_IP_VIP_Peaking_ON_Main,
PQ_IP_VIP_Peaking_NUMS_Main
} PQ_IP_VIP_Peaking_Group_Main;

typedef enum
{
PQ_IP_VIP_Peaking_band_RF_NTSC_Main,
PQ_IP_VIP_Peaking_band_RF_SECAM_Main,
PQ_IP_VIP_Peaking_band_AV_NTSC_Main,
PQ_IP_VIP_Peaking_band_AV_PAL_Main,
PQ_IP_VIP_Peaking_band_AV_SECAM_Main,
PQ_IP_VIP_Peaking_band_SV_NTSC_Main,
PQ_IP_VIP_Peaking_band_480i_Main,
PQ_IP_VIP_Peaking_band_720p_Main,
PQ_IP_VIP_Peaking_band_1080i_Main,
PQ_IP_VIP_Peaking_band_H_480i_Main,
PQ_IP_VIP_Peaking_band_H_576i_Main,
PQ_IP_VIP_Peaking_band_H_720p_Main,
PQ_IP_VIP_Peaking_band_H_1080i_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_480is_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_720p_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_1080i_Main,
PQ_IP_VIP_Peaking_band_DT_H264_480is_Main,
PQ_IP_VIP_Peaking_band_DT_H264_720p_Main,
PQ_IP_VIP_Peaking_band_PC_mode_Main,
PQ_IP_VIP_Peaking_band_NUMS_Main
} PQ_IP_VIP_Peaking_band_Group_Main;

typedef enum
{
PQ_IP_VIP_Peaking_Pcoring_S00_0_Main,
PQ_IP_VIP_Peaking_Pcoring_S21_1_Main,
PQ_IP_VIP_Peaking_Pcoring_S30_0_Main,
PQ_IP_VIP_Peaking_Pcoring_S30_1_Main,
PQ_IP_VIP_Peaking_Pcoring_S31_0_Main,
PQ_IP_VIP_Peaking_Pcoring_S31_1_Main,
PQ_IP_VIP_Peaking_Pcoring_S20_1_Main,
PQ_IP_VIP_Peaking_Pcoring_S32_0_Main,
PQ_IP_VIP_Peaking_Pcoring_S32_1_Main,
PQ_IP_VIP_Peaking_Pcoring_S42_1_Main,
PQ_IP_VIP_Peaking_Pcoring_NUMS_Main
} PQ_IP_VIP_Peaking_Pcoring_Group_Main;

typedef enum
{
PQ_IP_VIP_Peaking_gain_0x18_Main,
PQ_IP_VIP_Peaking_gain_0x1C_Main,
PQ_IP_VIP_Peaking_gain_0x20_Main,
PQ_IP_VIP_Peaking_gain_0x24_Main,
PQ_IP_VIP_Peaking_gain_0x28_Main,
PQ_IP_VIP_Peaking_gain_0x2C_Main,
PQ_IP_VIP_Peaking_gain_0x30_Main,
PQ_IP_VIP_Peaking_gain_0x34_Main,
PQ_IP_VIP_Peaking_gain_0x38_Main,
PQ_IP_VIP_Peaking_gain_NUMS_Main
} PQ_IP_VIP_Peaking_gain_Group_Main;

typedef enum
{
PQ_IP_VIP_Peaking_gain_ad_C_OFF_Main,
PQ_IP_VIP_Peaking_gain_ad_C_ON_Main,
PQ_IP_VIP_Peaking_gain_ad_C_NUMS_Main
} PQ_IP_VIP_Peaking_gain_ad_C_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_CTI_OFF_Main,
PQ_IP_VIP_Post_CTI_S0L0_Main,
PQ_IP_VIP_Post_CTI_S3L3_Main,
PQ_IP_VIP_Post_CTI_S1L6_Main,
PQ_IP_VIP_Post_CTI_S1L0_Main,
PQ_IP_VIP_Post_CTI_S2L3_Main,
PQ_IP_VIP_Post_CTI_S2L4_Main,
PQ_IP_VIP_Post_CTI_S2L5_Main,
PQ_IP_VIP_Post_CTI_S2L6_Main,
PQ_IP_VIP_Post_CTI_S2L7_Main,
PQ_IP_VIP_Post_CTI_NUMS_Main
} PQ_IP_VIP_Post_CTI_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_CTI_coef_0x18_Main,
PQ_IP_VIP_Post_CTI_coef_0x1C_Main,
PQ_IP_VIP_Post_CTI_coef_0x20_Main,
PQ_IP_VIP_Post_CTI_coef_0x24_Main,
PQ_IP_VIP_Post_CTI_coef_0x28_Main,
PQ_IP_VIP_Post_CTI_coef_0x04_Main,
PQ_IP_VIP_Post_CTI_coef_NUMS_Main
} PQ_IP_VIP_Post_CTI_coef_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T1_OFF_Main,
PQ_IP_VIP_FCC_T1_S4_Main,
PQ_IP_VIP_FCC_T1_mts_Main,
PQ_IP_VIP_FCC_T1_mts_1080_Main,
PQ_IP_VIP_FCC_T1_NUMS_Main
} PQ_IP_VIP_FCC_T1_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T2_OFF_Main,
PQ_IP_VIP_FCC_T2_S8_Main,
PQ_IP_VIP_FCC_T2_mts_Main,
PQ_IP_VIP_FCC_T2_mts_1080_Main,
PQ_IP_VIP_FCC_T2_NUMS_Main
} PQ_IP_VIP_FCC_T2_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T3_OFF_Main,
PQ_IP_VIP_FCC_T3_S4_Main,
PQ_IP_VIP_FCC_T3_mts_Main,
PQ_IP_VIP_FCC_T3_mts_1080_Main,
PQ_IP_VIP_FCC_T3_NUMS_Main
} PQ_IP_VIP_FCC_T3_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T4_OFF_Main,
PQ_IP_VIP_FCC_T4_S4_Main,
PQ_IP_VIP_FCC_T4_mts_Main,
PQ_IP_VIP_FCC_T4_mts_1080_Main,
PQ_IP_VIP_FCC_T4_NUMS_Main
} PQ_IP_VIP_FCC_T4_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T5_OFF_Main,
PQ_IP_VIP_FCC_T5_S6_Main,
PQ_IP_VIP_FCC_T5_S8_Main,
PQ_IP_VIP_FCC_T5_mts_Main,
PQ_IP_VIP_FCC_T5_mts_1080_Main,
PQ_IP_VIP_FCC_T5_NUMS_Main
} PQ_IP_VIP_FCC_T5_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T6_OFF_Main,
PQ_IP_VIP_FCC_T6_S6_Main,
PQ_IP_VIP_FCC_T6_S8_Main,
PQ_IP_VIP_FCC_T6_mts_Main,
PQ_IP_VIP_FCC_T6_mts_1080_Main,
PQ_IP_VIP_FCC_T6_NUMS_Main
} PQ_IP_VIP_FCC_T6_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T7_OFF_Main,
PQ_IP_VIP_FCC_T7_S8_Main,
PQ_IP_VIP_FCC_T7_mts_Main,
PQ_IP_VIP_FCC_T7_mts_1080_Main,
PQ_IP_VIP_FCC_T7_NUMS_Main
} PQ_IP_VIP_FCC_T7_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T8_OFF_Main,
PQ_IP_VIP_FCC_T8_S5_Main,
PQ_IP_VIP_FCC_T8_mts_Main,
PQ_IP_VIP_FCC_T8_mts_1080_Main,
PQ_IP_VIP_FCC_T8_NUMS_Main
} PQ_IP_VIP_FCC_T8_Group_Main;

typedef enum
{
PQ_IP_VIP_FCC_T9_OFF_Main,
PQ_IP_VIP_FCC_T9_S5_Main,
PQ_IP_VIP_FCC_T9_mts_Main,
PQ_IP_VIP_FCC_T9_mts_1080_Main,
PQ_IP_VIP_FCC_T9_NUMS_Main
} PQ_IP_VIP_FCC_T9_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_OFF_Main,
PQ_IP_VIP_IHC_ON_Main,
PQ_IP_VIP_IHC_NUMS_Main
} PQ_IP_VIP_IHC_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_R_OFF_Main,
PQ_IP_VIP_IHC_R_N_20_Main,
PQ_IP_VIP_IHC_R_P_20_Main,
PQ_IP_VIP_IHC_R_mts_AV_Main,
PQ_IP_VIP_IHC_R_NUMS_Main
} PQ_IP_VIP_IHC_R_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_G_OFF_Main,
PQ_IP_VIP_IHC_G_N_30_Main,
PQ_IP_VIP_IHC_G_N_10_Main,
PQ_IP_VIP_IHC_G_mts_AV_Main,
PQ_IP_VIP_IHC_G_NUMS_Main
} PQ_IP_VIP_IHC_G_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_B_OFF_Main,
PQ_IP_VIP_IHC_B_mts_AV_Main,
PQ_IP_VIP_IHC_B_NUMS_Main
} PQ_IP_VIP_IHC_B_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_C_OFF_Main,
PQ_IP_VIP_IHC_C_N_3F_Main,
PQ_IP_VIP_IHC_C_N_30_Main,
PQ_IP_VIP_IHC_C_N_10_Main,
PQ_IP_VIP_IHC_C_mts_AV_Main,
PQ_IP_VIP_IHC_C_mts_1080_Main,
PQ_IP_VIP_IHC_C_NUMS_Main
} PQ_IP_VIP_IHC_C_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_M_OFF_Main,
PQ_IP_VIP_IHC_M_P_10_Main,
PQ_IP_VIP_IHC_M_NUMS_Main
} PQ_IP_VIP_IHC_M_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_Y_OFF_Main,
PQ_IP_VIP_IHC_Y_P_8_Main,
PQ_IP_VIP_IHC_Y_P_10_Main,
PQ_IP_VIP_IHC_Y_P_15_Main,
PQ_IP_VIP_IHC_Y_mts_AV_Main,
PQ_IP_VIP_IHC_Y_mts_1080_Main,
PQ_IP_VIP_IHC_Y_NUMS_Main
} PQ_IP_VIP_IHC_Y_Group_Main;

typedef enum
{
PQ_IP_VIP_IHC_F_OFF_Main,
PQ_IP_VIP_IHC_F_N_10_Main,
PQ_IP_VIP_IHC_F_P_10_Main,
PQ_IP_VIP_IHC_F_mts_AV_Main,
PQ_IP_VIP_IHC_F_NUMS_Main
} PQ_IP_VIP_IHC_F_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_OFF_Main,
PQ_IP_VIP_ICC_ON_Main,
PQ_IP_VIP_ICC_NUMS_Main
} PQ_IP_VIP_ICC_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_R_OFF_Main,
PQ_IP_VIP_ICC_R_INC_8_Main,
PQ_IP_VIP_ICC_R_INC_3_Main,
PQ_IP_VIP_ICC_R_INC_B_Main,
PQ_IP_VIP_ICC_R_DEC_3_Main,
PQ_IP_VIP_ICC_R_NUMS_Main
} PQ_IP_VIP_ICC_R_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_G_OFF_Main,
PQ_IP_VIP_ICC_G_INC_8_Main,
PQ_IP_VIP_ICC_G_INC_6_Main,
PQ_IP_VIP_ICC_G_INC_B_Main,
PQ_IP_VIP_ICC_G_NUMS_Main
} PQ_IP_VIP_ICC_G_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_B_OFF_Main,
PQ_IP_VIP_ICC_B_INC_A_Main,
PQ_IP_VIP_ICC_B_INC_B_Main,
PQ_IP_VIP_ICC_B_INC_9_Main,
PQ_IP_VIP_ICC_B_INC_6_Main,
PQ_IP_VIP_ICC_B_INC_5_Main,
PQ_IP_VIP_ICC_B_NUMS_Main
} PQ_IP_VIP_ICC_B_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_C_OFF_Main,
PQ_IP_VIP_ICC_C_INC_6_Main,
PQ_IP_VIP_ICC_C_INC_9_Main,
PQ_IP_VIP_ICC_C_INC_A_Main,
PQ_IP_VIP_ICC_C_INC_B_Main,
PQ_IP_VIP_ICC_C_INC_4_Main,
PQ_IP_VIP_ICC_C_NUMS_Main
} PQ_IP_VIP_ICC_C_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_M_OFF_Main,
PQ_IP_VIP_ICC_M_INC_6_Main,
PQ_IP_VIP_ICC_M_INC_9_Main,
PQ_IP_VIP_ICC_M_INC_A_Main,
PQ_IP_VIP_ICC_M_NUMS_Main
} PQ_IP_VIP_ICC_M_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_Y_OFF_Main,
PQ_IP_VIP_ICC_Y_INC_7_Main,
PQ_IP_VIP_ICC_Y_INC_8_Main,
PQ_IP_VIP_ICC_Y_INC_A_Main,
PQ_IP_VIP_ICC_Y_NUMS_Main
} PQ_IP_VIP_ICC_Y_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_F_OFF_Main,
PQ_IP_VIP_ICC_F_INC_2_Main,
PQ_IP_VIP_ICC_F_INC_3_Main,
PQ_IP_VIP_ICC_F_INC_A_Main,
PQ_IP_VIP_ICC_F_NUMS_Main
} PQ_IP_VIP_ICC_F_Group_Main;

typedef enum
{
PQ_IP_VIP_ICC_NC_OFF_Main,
PQ_IP_VIP_ICC_NC_INC_2_Main,
PQ_IP_VIP_ICC_NC_INC_3_Main,
PQ_IP_VIP_ICC_NC_INC_8_Main,
PQ_IP_VIP_ICC_NC_INC_A_Main,
PQ_IP_VIP_ICC_NC_NUMS_Main
} PQ_IP_VIP_ICC_NC_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_OFF_Main,
PQ_IP_VIP_IBC_ON_Main,
PQ_IP_VIP_IBC_NUMS_Main
} PQ_IP_VIP_IBC_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_R_OFF_Main,
PQ_IP_VIP_IBC_R_DEC_10_Main,
PQ_IP_VIP_IBC_R_DEC_18_Main,
PQ_IP_VIP_IBC_R_NUMS_Main
} PQ_IP_VIP_IBC_R_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_G_OFF_Main,
PQ_IP_VIP_IBC_G_DEC_10_Main,
PQ_IP_VIP_IBC_G_DEC_1C_Main,
PQ_IP_VIP_IBC_G_DEC_18_Main,
PQ_IP_VIP_IBC_G_NUMS_Main
} PQ_IP_VIP_IBC_G_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_B_OFF_Main,
PQ_IP_VIP_IBC_B_DEC_10_Main,
PQ_IP_VIP_IBC_B_DEC_18_Main,
PQ_IP_VIP_IBC_B_NUMS_Main
} PQ_IP_VIP_IBC_B_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_C_OFF_Main,
PQ_IP_VIP_IBC_C_DEC_10_Main,
PQ_IP_VIP_IBC_C_DEC_18_Main,
PQ_IP_VIP_IBC_C_INC_22_Main,
PQ_IP_VIP_IBC_C_NUMS_Main
} PQ_IP_VIP_IBC_C_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_M_OFF_Main,
PQ_IP_VIP_IBC_M_DEC_18_Main,
PQ_IP_VIP_IBC_M_NUMS_Main
} PQ_IP_VIP_IBC_M_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_Y_OFF_Main,
PQ_IP_VIP_IBC_Y_DEC_18_Main,
PQ_IP_VIP_IBC_Y_NUMS_Main
} PQ_IP_VIP_IBC_Y_Group_Main;

typedef enum
{
PQ_IP_VIP_IBC_F_OFF_Main,
PQ_IP_VIP_IBC_F_INC_28_Main,
PQ_IP_VIP_IBC_F_NUMS_Main
} PQ_IP_VIP_IBC_F_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_OFF_Main,
PQ_IP_VIP_DLC_ON_Main,
PQ_IP_VIP_DLC_NUMS_Main
} PQ_IP_VIP_DLC_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_dither_OFF_Main,
PQ_IP_VIP_DLC_dither_ON_Main,
PQ_IP_VIP_DLC_dither_NUMS_Main
} PQ_IP_VIP_DLC_dither_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_Flesh_adj_OFF_Main,
PQ_IP_VIP_DLC_Flesh_adj_S1_Main,
PQ_IP_VIP_DLC_Flesh_adj_NUMS_Main
} PQ_IP_VIP_DLC_Flesh_adj_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_range_OFF_Main,
PQ_IP_VIP_DLC_His_range_ON_Main,
PQ_IP_VIP_DLC_His_range_NUMS_Main
} PQ_IP_VIP_DLC_His_range_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeH_90pa_1366_Main,
PQ_IP_VIP_DLC_His_rangeH_90pa_1920_Main,
PQ_IP_VIP_DLC_His_rangeH_NUMS_Main
} PQ_IP_VIP_DLC_His_rangeH_Group_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeV_90pa_1366_Main,
PQ_IP_VIP_DLC_His_rangeV_90pa_1920_Main,
PQ_IP_VIP_DLC_His_rangeV_NUMS_Main
} PQ_IP_VIP_DLC_His_rangeV_Group_Main;

typedef enum
{
PQ_IP_VIP_BLE_OFF_Main,
PQ_IP_VIP_BLE_0x82_0x40_Main,
PQ_IP_VIP_BLE_0x82_0x50_Main,
PQ_IP_VIP_BLE_0x82_0x60_Main,
PQ_IP_VIP_BLE_0x84_0x40_Main,
PQ_IP_VIP_BLE_0x86_0x40_Main,
PQ_IP_VIP_BLE_0x88_0x40_Main,
PQ_IP_VIP_BLE_0x88_0x60_Main,
PQ_IP_VIP_BLE_0x8A_0x40_Main,
PQ_IP_VIP_BLE_NUMS_Main
} PQ_IP_VIP_BLE_Group_Main;

typedef enum
{
PQ_IP_VIP_WLE_OFF_Main,
PQ_IP_VIP_WLE_0x78_0x20_Main,
PQ_IP_VIP_WLE_NUMS_Main
} PQ_IP_VIP_WLE_Group_Main;

typedef enum
{
PQ_IP_VIP_BWLE_dither_OFF_Main,
PQ_IP_VIP_BWLE_dither_ON_Main,
PQ_IP_VIP_BWLE_dither_NUMS_Main
} PQ_IP_VIP_BWLE_dither_Group_Main;

typedef enum
{
PQ_IP_VIP_UVC_OFF_Main,
PQ_IP_VIP_UVC_ON_Main,
PQ_IP_VIP_UVC_NUMS_Main
} PQ_IP_VIP_UVC_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_Yoffset_OFF_Main,
PQ_IP_VIP_Post_Yoffset_0x05_Main,
PQ_IP_VIP_Post_Yoffset_NUMS_Main
} PQ_IP_VIP_Post_Yoffset_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_Ygain_OFF_Main,
PQ_IP_VIP_Post_Ygain_0x3C_Main,
PQ_IP_VIP_Post_Ygain_0x36_Main,
PQ_IP_VIP_Post_Ygain_NUMS_Main
} PQ_IP_VIP_Post_Ygain_Group_Main;

typedef enum
{
PQ_IP_VIP_Post_Cgain_OFF_Main,
PQ_IP_VIP_Post_Cgain_0x44_Main,
PQ_IP_VIP_Post_Cgain_NUMS_Main
} PQ_IP_VIP_Post_Cgain_Group_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_Y_OFF_Main,
PQ_IP_VIP_Hcoring_Y_C4_Main,
PQ_IP_VIP_Hcoring_Y_C3_Main,
PQ_IP_VIP_Hcoring_Y_C2_Main,
PQ_IP_VIP_Hcoring_Y_C1_Main,
PQ_IP_VIP_Hcoring_Y_P6_Main,
PQ_IP_VIP_Hcoring_Y_P5_Main,
PQ_IP_VIP_Hcoring_Y_P4_Main,
PQ_IP_VIP_Hcoring_Y_P3_Main,
PQ_IP_VIP_Hcoring_Y_P2_Main,
PQ_IP_VIP_Hcoring_Y_P1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P6_Main,
PQ_IP_VIP_Hcoring_Y_PC_P5_Main,
PQ_IP_VIP_Hcoring_Y_PC_P4_Main,
PQ_IP_VIP_Hcoring_Y_PC_P3_Main,
PQ_IP_VIP_Hcoring_Y_PC_P2_Main,
PQ_IP_VIP_Hcoring_Y_PC_P1_Main,
PQ_IP_VIP_Hcoring_Y_NUMS_Main
} PQ_IP_VIP_Hcoring_Y_Group_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_C_OFF_Main,
PQ_IP_VIP_Hcoring_C_C4_Main,
PQ_IP_VIP_Hcoring_C_C3_Main,
PQ_IP_VIP_Hcoring_C_C2_Main,
PQ_IP_VIP_Hcoring_C_C1_Main,
PQ_IP_VIP_Hcoring_C_P5_Main,
PQ_IP_VIP_Hcoring_C_P4_Main,
PQ_IP_VIP_Hcoring_C_P3_Main,
PQ_IP_VIP_Hcoring_C_P2_Main,
PQ_IP_VIP_Hcoring_C_P1_Main,
PQ_IP_VIP_Hcoring_C_NUMS_Main
} PQ_IP_VIP_Hcoring_C_Group_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_dither_OFF_Main,
PQ_IP_VIP_Hcoring_dither_ON_Main,
PQ_IP_VIP_Hcoring_dither_NUMS_Main
} PQ_IP_VIP_Hcoring_dither_Group_Main;

typedef enum
{
PQ_IP_SwDriver_OFF_Main,
PQ_IP_SwDriver_ALL_Main,
PQ_IP_SwDriver_SD_HD_Main,
PQ_IP_SwDriver_HD_ALL_Main,
PQ_IP_SwDriver_NUMS_Main
} PQ_IP_SwDriver_Group_Main;

typedef enum
{
PQ_IP_3x3_OFF_Main,
PQ_IP_3x3_SD_Main,
PQ_IP_3x3_HD_Main,
PQ_IP_3x3_NUMS_Main
} PQ_IP_3x3_Group_Main;

typedef enum
{
PQ_IP_RGB_Offset_OFF_Main,
PQ_IP_RGB_Offset_NUMS_Main
} PQ_IP_RGB_Offset_Group_Main;

typedef enum
{
PQ_IP_RGB_Clip_OFF_Main,
PQ_IP_RGB_Clip_NUMS_Main
} PQ_IP_RGB_Clip_Group_Main;

typedef enum
{
PQ_IP_xvYCC_bypass_OFF_Main,
PQ_IP_xvYCC_bypass_ON_Main,
PQ_IP_xvYCC_bypass_NUMS_Main
} PQ_IP_xvYCC_bypass_Group_Main;

typedef enum
{
PQ_IP_xvYCC_de_gamma_OFF_Main,
PQ_IP_xvYCC_de_gamma_ON_Main,
PQ_IP_xvYCC_de_gamma_NUMS_Main
} PQ_IP_xvYCC_de_gamma_Group_Main;

typedef enum
{
PQ_IP_xvYCC_de_gamma_dither_OFF_Main,
PQ_IP_xvYCC_de_gamma_dither_ON_Main,
PQ_IP_xvYCC_de_gamma_dither_NUMS_Main
} PQ_IP_xvYCC_de_gamma_dither_Group_Main;

typedef enum
{
PQ_IP_xvYCC_3x3_OFF_Main,
PQ_IP_xvYCC_3x3_NUMS_Main
} PQ_IP_xvYCC_3x3_Group_Main;

typedef enum
{
PQ_IP_xvYCC_Comp_OFF_Main,
PQ_IP_xvYCC_Comp_NUMS_Main
} PQ_IP_xvYCC_Comp_Group_Main;

typedef enum
{
PQ_IP_xvYCC_Comp_dither_OFF_Main,
PQ_IP_xvYCC_Comp_dither_ON_Main,
PQ_IP_xvYCC_Comp_dither_NUMS_Main
} PQ_IP_xvYCC_Comp_dither_Group_Main;

typedef enum
{
PQ_IP_xvYCC_Clip_OFF_Main,
PQ_IP_xvYCC_Clip_NUMS_Main
} PQ_IP_xvYCC_Clip_Group_Main;

typedef enum
{
PQ_IP_xvYCC_gamma_OFF_Main,
PQ_IP_xvYCC_gamma_ON_Main,
PQ_IP_xvYCC_gamma_NUMS_Main
} PQ_IP_xvYCC_gamma_Group_Main;

typedef enum
{
PQ_IP_xvYCC_gamma_dither_OFF_Main,
PQ_IP_xvYCC_gamma_dither_ON_Main,
PQ_IP_xvYCC_gamma_dither_NUMS_Main
} PQ_IP_xvYCC_gamma_dither_Group_Main;

typedef enum
{
PQ_IP_rgb_3d_OFF_Main,
PQ_IP_rgb_3d_ON_Main,
PQ_IP_rgb_3d_NUMS_Main
} PQ_IP_rgb_3d_Group_Main;

typedef enum
{
PQ_IP_HBC_OFF_Main,
PQ_IP_HBC_HBC1_Main,
PQ_IP_HBC_NUMS_Main
} PQ_IP_HBC_Group_Main;

typedef enum
{
PQ_IP_Pre_CON_BRI_OFF_Main,
PQ_IP_Pre_CON_BRI_NUMS_Main
} PQ_IP_Pre_CON_BRI_Group_Main;

typedef enum
{
PQ_IP_Blue_Stretch_OFF_Main,
PQ_IP_Blue_Stretch_BS1_Main,
PQ_IP_Blue_Stretch_NUMS_Main
} PQ_IP_Blue_Stretch_Group_Main;

typedef enum
{
PQ_IP_Blue_Stretch_dither_OFF_Main,
PQ_IP_Blue_Stretch_dither_ON_Main,
PQ_IP_Blue_Stretch_dither_NUMS_Main
} PQ_IP_Blue_Stretch_dither_Group_Main;

typedef enum
{
PQ_IP_Gamma_OFF_Main,
PQ_IP_Gamma_ON_Main,
PQ_IP_Gamma_NUMS_Main
} PQ_IP_Gamma_Group_Main;

typedef enum
{
PQ_IP_Gamma_dither_OFF_Main,
PQ_IP_Gamma_dither_ON_Main,
PQ_IP_Gamma_dither_NUMS_Main
} PQ_IP_Gamma_dither_Group_Main;

typedef enum
{
PQ_IP_Post_CON_BRI_OFF_Main,
PQ_IP_Post_CON_BRI_NUMS_Main
} PQ_IP_Post_CON_BRI_Group_Main;

typedef enum
{
PQ_IP_SRAM_3x3matrix_PIC2_Build_IN_Main,
PQ_IP_SRAM_3x3matrix_PIC2_NUMS_Main
} PQ_IP_SRAM_3x3matrix_PIC2_Group_Main;

typedef enum
{
PQ_IP_SRAM_DLC_PIC1_Build_IN_Main,
PQ_IP_SRAM_DLC_PIC1_NUMS_Main
} PQ_IP_SRAM_DLC_PIC1_Group_Main;

typedef enum
{
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Build_IN_Main,
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_NUMS_Main
} PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Group_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_R_PIC1_Build_IN_Main,
PQ_IP_SRAM_GammaTbl_R_PIC1_NUMS_Main
} PQ_IP_SRAM_GammaTbl_R_PIC1_Group_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_G_PIC1_Build_IN_Main,
PQ_IP_SRAM_GammaTbl_G_PIC1_NUMS_Main
} PQ_IP_SRAM_GammaTbl_G_PIC1_Group_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_B_PIC1_Build_IN_Main,
PQ_IP_SRAM_GammaTbl_B_PIC1_NUMS_Main
} PQ_IP_SRAM_GammaTbl_B_PIC1_Group_Main;

typedef enum
{
PQ_IP_SRAM_Auto_Color_PIC2_Build_IN_Main,
PQ_IP_SRAM_Auto_Color_PIC2_NUMS_Main
} PQ_IP_SRAM_Auto_Color_PIC2_Group_Main;

typedef enum
{
PQ_IP_SRAM_Color_Temp_PIC1_Build_IN_Main,
PQ_IP_SRAM_Color_Temp_PIC1_NUMS_Main
} PQ_IP_SRAM_Color_Temp_PIC1_Group_Main;

extern code U8 MST_SkipRule_IP_Main[PQ_IP_NUM_Main];
extern code EN_IPTAB_INFO PQ_IPTAB_INFO_Main[];
extern code U8 QMAP_1920_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];
extern code U8 QMAP_1920_PIP_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];
extern code U8 QMAP_1920_POP_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];
extern code U8 QMAP_1366_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];
extern code U8 QMAP_1366_PIP_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];
extern code U8 QMAP_1366_POP_Main[QM_INPUTTYPE_NUM_Main][PQ_IP_NUM_Main];

#endif
