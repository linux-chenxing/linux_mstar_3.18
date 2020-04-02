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
// 720
// Cedric
// 1/7/2013  8:43:01 PM
// 39630.43853
//****************************************************

#ifndef _CEDRIC_SC1_MAIN_H_
#define _CEDRIC_SC1_MAIN_H_

#define PQ_IP_NUM_SC1_Main 190
#define PQ_IP_SRAM_COLOR_INDEX_SIZE_SC1_Main 256
#define PQ_IP_SRAM_COLOR_GAIN_SNR_SIZE_SC1_Main 8
#define PQ_IP_SRAM_COLOR_GAIN_DNR_SIZE_SC1_Main 8
#define PQ_IP_SRAM1_SIZE_SC1_Main 320
#define PQ_IP_SRAM2_SIZE_SC1_Main 320
#define PQ_IP_SRAM3_SIZE_SC1_Main 640
#define PQ_IP_SRAM4_SIZE_SC1_Main 640
#define PQ_IP_C_SRAM1_SIZE_SC1_Main 320
#define PQ_IP_C_SRAM2_SIZE_SC1_Main 320
#define PQ_IP_C_SRAM3_SIZE_SC1_Main 320
#define PQ_IP_C_SRAM4_SIZE_SC1_Main 320
#define PQ_IP_SRAM_3x3matrix_PIC2_SIZE_SC1_Main 32
#define PQ_IP_SRAM_DLC_PIC1_SIZE_SC1_Main 65
#define PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SIZE_SC1_Main 5
#define PQ_IP_SRAM_GammaTbl_R_PIC1_SIZE_SC1_Main 386
#define PQ_IP_SRAM_GammaTbl_G_PIC1_SIZE_SC1_Main 386
#define PQ_IP_SRAM_GammaTbl_B_PIC1_SIZE_SC1_Main 386
#define PQ_IP_SRAM_Auto_Color_PIC2_SIZE_SC1_Main 6
#define PQ_IP_SRAM_Color_Temp_PIC1_SIZE_SC1_Main 9

typedef enum
{
    #if PQ_QM_CVBS
    QM_RF_NTSC_44_SC1_Main, //0
    #endif
    #if PQ_QM_CVBS
    QM_RF_NTSC_M_SC1_Main, //1
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_BGHI_SC1_Main, //2
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_60_SC1_Main, //3
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_M_SC1_Main, //4
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_N_SC1_Main, //5
    #endif
    #if PQ_QM_CVBS
    QM_RF_SECAM_SC1_Main, //6
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_44_SC1_Main, //7
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_M_SC1_Main, //8
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_BGHI_SC1_Main, //9
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_60_SC1_Main, //10
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_M_SC1_Main, //11
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_N_SC1_Main, //12
    #endif
    #if PQ_QM_CVBS
    QM_VIF_SECAM_SC1_Main, //13
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_44_SC1_Main, //14
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_M_SC1_Main, //15
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_BGHI_SC1_Main, //16
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_60_SC1_Main, //17
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_M_SC1_Main, //18
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_N_SC1_Main, //19
    #endif
    #if PQ_QM_CVBS
    QM_SV_SECAM_SC1_Main, //20
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_44_SC1_Main, //21
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_M_SC1_Main, //22
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_BGHI_SC1_Main, //23
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_60_SC1_Main, //24
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_M_SC1_Main, //25
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_N_SC1_Main, //26
    #endif
    #if PQ_QM_CVBS
    QM_AV_SECAM_SC1_Main, //27
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_44_SC1_Main, //28
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_M_SC1_Main, //29
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_BGHI_SC1_Main, //30
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_60_SC1_Main, //31
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_M_SC1_Main, //32
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_N_SC1_Main, //33
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_SECAM_SC1_Main, //34
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_44_SC1_Main, //35
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_M_SC1_Main, //36
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_BGHI_SC1_Main, //37
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_60_SC1_Main, //38
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_M_SC1_Main, //39
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_N_SC1_Main, //40
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_SECAM_SC1_Main, //41
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_NTSC_SC1_Main, //42
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_PAL_SC1_Main, //43
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480i_SC1_Main, //44
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576i_SC1_Main, //45
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480p_SC1_Main, //46
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576p_SC1_Main, //47
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_24hz_SC1_Main, //48
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_50hz_SC1_Main, //49
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_60hz_SC1_Main, //50
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_50hz_SC1_Main, //51
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_60hz_SC1_Main, //52
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_24hz_SC1_Main, //53
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_50hz_SC1_Main, //54
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_60hz_SC1_Main, //55
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480i_SC1_Main, //56
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576i_SC1_Main, //57
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480p_SC1_Main, //58
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576p_SC1_Main, //59
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_24hz_SC1_Main, //60
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_50hz_SC1_Main, //61
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_60hz_SC1_Main, //62
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_50hz_SC1_Main, //63
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_60hz_SC1_Main, //64
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_24hz_SC1_Main, //65
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_60hz_SC1_Main, //66
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_50hz_SC1_Main, //67
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vup_SC1_Main, //68
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vdown_SC1_Main, //69
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vno_SC1_Main, //70
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vup_SC1_Main, //71
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vdown_SC1_Main, //72
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vno_SC1_Main, //73
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vup_SC1_Main, //74
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vdown_SC1_Main, //75
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vno_SC1_Main, //76
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vup_SC1_Main, //77
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vdown_SC1_Main, //78
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vno_SC1_Main, //79
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vup_SC1_Main, //80
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vdown_SC1_Main, //81
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vno_SC1_Main, //82
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vup_SC1_Main, //83
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vdown_SC1_Main, //84
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vno_SC1_Main, //85
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vup_SC1_Main, //86
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown_SC1_Main, //87
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vno_SC1_Main, //88
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup_SC1_Main, //89
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown_SC1_Main, //90
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno_SC1_Main, //91
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vup_SC1_Main, //92
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown_SC1_Main, //93
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_SC1_Main, //94
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_MPEG2_SC1_Main, //95
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_MPEG2_SC1_Main, //96
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_MPEG2_SC1_Main, //97
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_MPEG2_SC1_Main, //98
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_MPEG2_SC1_Main, //99
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_MPEG2_SC1_Main, //100
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_MPEG2_SC1_Main, //101
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_MPEG2_SC1_Main, //102
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_MPEG2_SC1_Main, //103
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_MPEG2_SC1_Main, //104
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_MPEG2_SC1_Main, //105
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_MPEG2_SC1_Main, //106
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_MPEG2_SC1_Main, //107
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_H264_SC1_Main, //108
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_H264_SC1_Main, //109
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_H264_SC1_Main, //110
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_H264_SC1_Main, //111
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_H264_SC1_Main, //112
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_H264_SC1_Main, //113
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_H264_SC1_Main, //114
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_H264_SC1_Main, //115
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_H264_SC1_Main, //116
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_H264_SC1_Main, //117
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_H264_SC1_Main, //118
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_H264_SC1_Main, //119
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_H264_SC1_Main, //120
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_interlace_SC1_Main, //121
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_progressive_SC1_Main, //122
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_interlace_SC1_Main, //123
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_progressive_SC1_Main, //124
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_SD_progressive_SC1_Main, //125
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_HD_progressive_SC1_Main, //126
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_interlace_SC1_Main, //127
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_progressive_SC1_Main, //128
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_interlace_SC1_Main, //129
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_progressive_SC1_Main, //130
    #endif
    #if PQ_QM_3D
    QM_3D_Video_interlace_SC1_Main, //131
    #endif
    #if PQ_QM_3D
    QM_3D_Video_progressive_SC1_Main, //132
    #endif
    QM_INPUTTYPE_NUM_SC1_Main, // 133
} SC_QUALITY_MAP_INDEX_e_SC1_Main;

typedef enum
{
PQ_IP_AFEC_SC1_Main,  //0
PQ_IP_Comb_SC1_Main,  //1
PQ_IP_Comb2_SC1_Main,  //2
PQ_IP_SECAM_SC1_Main,  //3
PQ_IP_VD_Sampling_no_comm_SC1_Main,  //4
PQ_IP_ADC_Sampling_SC1_Main,  //5
PQ_IP_SCinit_SC1_Main,  //6
PQ_IP_CSC_SC1_Main,  //7
PQ_IP_CSC_Dither_SC1_Main,  //8
PQ_IP_YCdelay_SC1_Main,  //9
PQ_IP_PreFilter_SC1_Main,  //10
PQ_IP_PreFilter_Dither_SC1_Main,  //11
PQ_IP_HDSDD_SC1_Main,  //12
PQ_IP_HSD_Sampling_SC1_Main,  //13
PQ_IP_HSD_Y_SC1_Main,  //14
PQ_IP_HSD_C_SC1_Main,  //15
PQ_IP_444To422_SC1_Main,  //16
PQ_IP_VSD_SC1_Main,  //17
PQ_IP_HVSD_Dither_SC1_Main,  //18
PQ_IP_10to8_Dither_SC1_Main,  //19
PQ_IP_MemFormat_SC1_Main,  //20
PQ_IP_PreSNR_SC1_Main,  //21
PQ_IP_PreSNR_Patch_SC1_Main,  //22
PQ_IP_DNR_SC1_Main,  //23
PQ_IP_DNR_Motion_SC1_Main,  //24
PQ_IP_DNR_Y_SC1_Main,  //25
PQ_IP_DNR_Y_COLOR_DEP_SC1_Main,  //26
PQ_IP_SRAM_COLOR_INDEX_SC1_Main,  //27
PQ_IP_SRAM_COLOR_GAIN_SNR_SC1_Main,  //28
PQ_IP_SRAM_COLOR_GAIN_DNR_SC1_Main,  //29
PQ_IP_DNR_Y_LUMA_ADAPTIVE_SC1_Main,  //30
PQ_IP_DNR_POSTTUNE_SC1_Main,  //31
PQ_IP_DNR_C_SC1_Main,  //32
PQ_IP_HISDNR_SC1_Main,  //33
PQ_IP_PNR_SC1_Main,  //34
PQ_IP_PNR_Y_SC1_Main,  //35
PQ_IP_PNR_C_SC1_Main,  //36
PQ_IP_PostCCS_SC1_Main,  //37
PQ_IP_PostCCS_Smooth_SC1_Main,  //38
PQ_IP_420CUP_SC1_Main,  //39
PQ_IP_MADi_SC1_Main,  //40
PQ_IP_MADi_Motion_SC1_Main,  //41
PQ_IP_MADi_ADP3x3_SC1_Main,  //42
PQ_IP_MADi_MORPHO_SC1_Main,  //43
PQ_IP_MADi_DFK_SC1_Main,  //44
PQ_IP_MADi_SST_SC1_Main,  //45
PQ_IP_MADi_EODiW_SC1_Main,  //46
PQ_IP_MADi_Force_SC1_Main,  //47
PQ_IP_EODi_SC1_Main,  //48
PQ_IP_Film_SC1_Main,  //49
PQ_IP_Film32_SC1_Main,  //50
PQ_IP_Film22_SC1_Main,  //51
PQ_IP_Film_any_SC1_Main,  //52
PQ_IP_UCNR_SC1_Main,  //53
PQ_IP_UCDi_SC1_Main,  //54
PQ_IP_UC_CTL_SC1_Main,  //55
PQ_IP_DIPF_SC1_Main,  //56
PQ_IP_VCLPF_SC1_Main,  //57
PQ_IP_Spike_NR_SC1_Main,  //58
PQ_IP_SPF_SC1_Main,  //59
PQ_IP_SPF_SNR_SC1_Main,  //60
PQ_IP_SPF_SNR_MR_SC1_Main,  //61
PQ_IP_SPF_MR_LPF_SC1_Main,  //62
PQ_IP_SPF_NMR_Y_SC1_Main,  //63
PQ_IP_SPF_NMR_Y_MR_SC1_Main,  //64
PQ_IP_VSP_Y_SC1_Main,  //65
PQ_IP_VSP_C_SC1_Main,  //66
PQ_IP_VSP_CoRing_SC1_Main,  //67
PQ_IP_VSP_DeRing_SC1_Main,  //68
PQ_IP_VSP_Dither_SC1_Main,  //69
PQ_IP_VSP_PreVBound_SC1_Main,  //70
PQ_IP_422To444_SC1_Main,  //71
PQ_IP_PreCTI_SC1_Main,  //72
PQ_IP_HSP_Y_SC1_Main,  //73
PQ_IP_HSP_C_SC1_Main,  //74
PQ_IP_HSP_CoRing_SC1_Main,  //75
PQ_IP_HSP_DeRing_SC1_Main,  //76
PQ_IP_HSP_Dither_SC1_Main,  //77
PQ_IP_HnonLinear_SC1_Main,  //78
PQ_IP_SRAM1_SC1_Main,  //79
PQ_IP_SRAM2_SC1_Main,  //80
PQ_IP_SRAM3_SC1_Main,  //81
PQ_IP_SRAM4_SC1_Main,  //82
PQ_IP_C_SRAM1_SC1_Main,  //83
PQ_IP_C_SRAM2_SC1_Main,  //84
PQ_IP_C_SRAM3_SC1_Main,  //85
PQ_IP_C_SRAM4_SC1_Main,  //86
PQ_IP_VIP_SC1_Main,  //87
PQ_IP_VIP_pseudo_SC1_Main,  //88
PQ_IP_VIP_CSC_SC1_Main,  //89
PQ_IP_VIP_CSC_dither_SC1_Main,  //90
PQ_IP_VIP_Post_YC_delay_SC1_Main,  //91
PQ_IP_VIP_HNMR_Y_SC1_Main,  //92
PQ_IP_VIP_HNMR_ad_C_SC1_Main,  //93
PQ_IP_VIP_HNMR_ad_C_gain_SC1_Main,  //94
PQ_IP_VIP_HNMR_C_win1_SC1_Main,  //95
PQ_IP_VIP_HNMR_C_win2_SC1_Main,  //96
PQ_IP_VIP_HNMR_C_win3_SC1_Main,  //97
PQ_IP_VIP_HNMR_C_win4_SC1_Main,  //98
PQ_IP_VIP_Pre_Yoffset_SC1_Main,  //99
PQ_IP_VIP_Pre_Ygain_SC1_Main,  //100
PQ_IP_VIP_Pre_Ygain_dither_SC1_Main,  //101
PQ_IP_VIP_HLPF_SC1_Main,  //102
PQ_IP_VIP_HLPF_dither_SC1_Main,  //103
PQ_IP_VIP_Peaking_SC1_Main,  //104
PQ_IP_VIP_Peaking_band_SC1_Main,  //105
PQ_IP_VIP_Peaking_Pcoring_SC1_Main,  //106
PQ_IP_VIP_Peaking_gain_SC1_Main,  //107
PQ_IP_VIP_Peaking_gain_ad_C_SC1_Main,  //108
PQ_IP_VIP_Post_CTI_SC1_Main,  //109
PQ_IP_VIP_Post_CTI_coef_SC1_Main,  //110
PQ_IP_VIP_FCC_T1_SC1_Main,  //111
PQ_IP_VIP_FCC_T2_SC1_Main,  //112
PQ_IP_VIP_FCC_T3_SC1_Main,  //113
PQ_IP_VIP_FCC_T4_SC1_Main,  //114
PQ_IP_VIP_FCC_T5_SC1_Main,  //115
PQ_IP_VIP_FCC_T6_SC1_Main,  //116
PQ_IP_VIP_FCC_T7_SC1_Main,  //117
PQ_IP_VIP_FCC_T8_SC1_Main,  //118
PQ_IP_VIP_FCC_T9_SC1_Main,  //119
PQ_IP_VIP_IHC_SC1_Main,  //120
PQ_IP_VIP_IHC_R_SC1_Main,  //121
PQ_IP_VIP_IHC_G_SC1_Main,  //122
PQ_IP_VIP_IHC_B_SC1_Main,  //123
PQ_IP_VIP_IHC_C_SC1_Main,  //124
PQ_IP_VIP_IHC_M_SC1_Main,  //125
PQ_IP_VIP_IHC_Y_SC1_Main,  //126
PQ_IP_VIP_IHC_F_SC1_Main,  //127
PQ_IP_VIP_ICC_SC1_Main,  //128
PQ_IP_VIP_ICC_R_SC1_Main,  //129
PQ_IP_VIP_ICC_G_SC1_Main,  //130
PQ_IP_VIP_ICC_B_SC1_Main,  //131
PQ_IP_VIP_ICC_C_SC1_Main,  //132
PQ_IP_VIP_ICC_M_SC1_Main,  //133
PQ_IP_VIP_ICC_Y_SC1_Main,  //134
PQ_IP_VIP_ICC_F_SC1_Main,  //135
PQ_IP_VIP_ICC_NC_SC1_Main,  //136
PQ_IP_VIP_IBC_SC1_Main,  //137
PQ_IP_VIP_IBC_R_SC1_Main,  //138
PQ_IP_VIP_IBC_G_SC1_Main,  //139
PQ_IP_VIP_IBC_B_SC1_Main,  //140
PQ_IP_VIP_IBC_C_SC1_Main,  //141
PQ_IP_VIP_IBC_M_SC1_Main,  //142
PQ_IP_VIP_IBC_Y_SC1_Main,  //143
PQ_IP_VIP_IBC_F_SC1_Main,  //144
PQ_IP_VIP_DLC_SC1_Main,  //145
PQ_IP_VIP_DLC_dither_SC1_Main,  //146
PQ_IP_VIP_DLC_Flesh_adj_SC1_Main,  //147
PQ_IP_VIP_DLC_His_range_SC1_Main,  //148
PQ_IP_VIP_DLC_His_rangeH_SC1_Main,  //149
PQ_IP_VIP_DLC_His_rangeV_SC1_Main,  //150
PQ_IP_VIP_BLE_SC1_Main,  //151
PQ_IP_VIP_WLE_SC1_Main,  //152
PQ_IP_VIP_BWLE_dither_SC1_Main,  //153
PQ_IP_VIP_UVC_SC1_Main,  //154
PQ_IP_VIP_Post_Yoffset_SC1_Main,  //155
PQ_IP_VIP_Post_Ygain_SC1_Main,  //156
PQ_IP_VIP_Post_Cgain_SC1_Main,  //157
PQ_IP_VIP_Hcoring_Y_SC1_Main,  //158
PQ_IP_VIP_Hcoring_C_SC1_Main,  //159
PQ_IP_VIP_Hcoring_dither_SC1_Main,  //160
PQ_IP_SwDriver_SC1_Main,  //161
PQ_IP_3x3_SC1_Main,  //162
PQ_IP_RGB_Offset_SC1_Main,  //163
PQ_IP_RGB_Clip_SC1_Main,  //164
PQ_IP_xvYCC_bypass_SC1_Main,  //165
PQ_IP_xvYCC_de_gamma_SC1_Main,  //166
PQ_IP_xvYCC_de_gamma_dither_SC1_Main,  //167
PQ_IP_xvYCC_3x3_SC1_Main,  //168
PQ_IP_xvYCC_Comp_SC1_Main,  //169
PQ_IP_xvYCC_Comp_dither_SC1_Main,  //170
PQ_IP_xvYCC_Clip_SC1_Main,  //171
PQ_IP_xvYCC_gamma_SC1_Main,  //172
PQ_IP_xvYCC_gamma_dither_SC1_Main,  //173
PQ_IP_rgb_3d_SC1_Main,  //174
PQ_IP_HBC_SC1_Main,  //175
PQ_IP_Pre_CON_BRI_SC1_Main,  //176
PQ_IP_Blue_Stretch_SC1_Main,  //177
PQ_IP_Blue_Stretch_dither_SC1_Main,  //178
PQ_IP_Gamma_SC1_Main,  //179
PQ_IP_Gamma_dither_SC1_Main,  //180
PQ_IP_Post_CON_BRI_SC1_Main,  //181
PQ_IP_SRAM_3x3matrix_PIC2_SC1_Main,  //182
PQ_IP_SRAM_DLC_PIC1_SC1_Main,  //183
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SC1_Main,  //184
PQ_IP_SRAM_GammaTbl_R_PIC1_SC1_Main,  //185
PQ_IP_SRAM_GammaTbl_G_PIC1_SC1_Main,  //186
PQ_IP_SRAM_GammaTbl_B_PIC1_SC1_Main,  //187
PQ_IP_SRAM_Auto_Color_PIC2_SC1_Main,  //188
PQ_IP_SRAM_Color_Temp_PIC1_SC1_Main,  //189
 }   PQ_IPTYPE_SC1_Main;

typedef enum
{
PQ_IP_AFEC_COM_SC1_Main,  //0
PQ_IP_Comb_COM_SC1_Main,  //1
PQ_IP_Comb2_COM_SC1_Main,  //2
PQ_IP_SECAM_COM_SC1_Main,  //3
PQ_IP_VD_Sampling_no_comm_COM_SC1_Main,  //4
PQ_IP_ADC_Sampling_COM_SC1_Main,  //5
PQ_IP_SCinit_COM_SC1_Main,  //6
PQ_IP_CSC_COM_SC1_Main,  //7
PQ_IP_CSC_Dither_COM_SC1_Main,  //8
PQ_IP_YCdelay_COM_SC1_Main,  //9
PQ_IP_PreFilter_COM_SC1_Main,  //10
PQ_IP_PreFilter_Dither_COM_SC1_Main,  //11
PQ_IP_HDSDD_COM_SC1_Main,  //12
PQ_IP_HSD_Sampling_COM_SC1_Main,  //13
PQ_IP_HSD_Y_COM_SC1_Main,  //14
PQ_IP_HSD_C_COM_SC1_Main,  //15
PQ_IP_444To422_COM_SC1_Main,  //16
PQ_IP_VSD_COM_SC1_Main,  //17
PQ_IP_HVSD_Dither_COM_SC1_Main,  //18
PQ_IP_10to8_Dither_COM_SC1_Main,  //19
PQ_IP_MemFormat_COM_SC1_Main,  //20
PQ_IP_PreSNR_COM_SC1_Main,  //21
PQ_IP_PreSNR_Patch_COM_SC1_Main,  //22
PQ_IP_DNR_COM_SC1_Main,  //23
PQ_IP_DNR_Motion_COM_SC1_Main,  //24
PQ_IP_DNR_Y_COM_SC1_Main,  //25
PQ_IP_DNR_Y_COLOR_DEP_COM_SC1_Main,  //26
PQ_IP_SRAM_COLOR_INDEX_COM_SC1_Main,  //27
PQ_IP_SRAM_COLOR_GAIN_SNR_COM_SC1_Main,  //28
PQ_IP_SRAM_COLOR_GAIN_DNR_COM_SC1_Main,  //29
PQ_IP_DNR_Y_LUMA_ADAPTIVE_COM_SC1_Main,  //30
PQ_IP_DNR_POSTTUNE_COM_SC1_Main,  //31
PQ_IP_DNR_C_COM_SC1_Main,  //32
PQ_IP_HISDNR_COM_SC1_Main,  //33
PQ_IP_PNR_COM_SC1_Main,  //34
PQ_IP_PNR_Y_COM_SC1_Main,  //35
PQ_IP_PNR_C_COM_SC1_Main,  //36
PQ_IP_PostCCS_COM_SC1_Main,  //37
PQ_IP_PostCCS_Smooth_COM_SC1_Main,  //38
PQ_IP_420CUP_COM_SC1_Main,  //39
PQ_IP_MADi_COM_SC1_Main,  //40
PQ_IP_MADi_Motion_COM_SC1_Main,  //41
PQ_IP_MADi_ADP3x3_COM_SC1_Main,  //42
PQ_IP_MADi_MORPHO_COM_SC1_Main,  //43
PQ_IP_MADi_DFK_COM_SC1_Main,  //44
PQ_IP_MADi_SST_COM_SC1_Main,  //45
PQ_IP_MADi_EODiW_COM_SC1_Main,  //46
PQ_IP_MADi_Force_COM_SC1_Main,  //47
PQ_IP_EODi_COM_SC1_Main,  //48
PQ_IP_Film_COM_SC1_Main,  //49
PQ_IP_Film32_COM_SC1_Main,  //50
PQ_IP_Film22_COM_SC1_Main,  //51
PQ_IP_Film_any_COM_SC1_Main,  //52
PQ_IP_UCNR_COM_SC1_Main,  //53
PQ_IP_UCDi_COM_SC1_Main,  //54
PQ_IP_UC_CTL_COM_SC1_Main,  //55
PQ_IP_DIPF_COM_SC1_Main,  //56
PQ_IP_VCLPF_COM_SC1_Main,  //57
PQ_IP_Spike_NR_COM_SC1_Main,  //58
PQ_IP_SPF_COM_SC1_Main,  //59
PQ_IP_SPF_SNR_COM_SC1_Main,  //60
PQ_IP_SPF_SNR_MR_COM_SC1_Main,  //61
PQ_IP_SPF_MR_LPF_COM_SC1_Main,  //62
PQ_IP_SPF_NMR_Y_COM_SC1_Main,  //63
PQ_IP_SPF_NMR_Y_MR_COM_SC1_Main,  //64
PQ_IP_VSP_Y_COM_SC1_Main,  //65
PQ_IP_VSP_C_COM_SC1_Main,  //66
PQ_IP_VSP_CoRing_COM_SC1_Main,  //67
PQ_IP_VSP_DeRing_COM_SC1_Main,  //68
PQ_IP_VSP_Dither_COM_SC1_Main,  //69
PQ_IP_VSP_PreVBound_COM_SC1_Main,  //70
PQ_IP_422To444_COM_SC1_Main,  //71
PQ_IP_PreCTI_COM_SC1_Main,  //72
PQ_IP_HSP_Y_COM_SC1_Main,  //73
PQ_IP_HSP_C_COM_SC1_Main,  //74
PQ_IP_HSP_CoRing_COM_SC1_Main,  //75
PQ_IP_HSP_DeRing_COM_SC1_Main,  //76
PQ_IP_HSP_Dither_COM_SC1_Main,  //77
PQ_IP_HnonLinear_COM_SC1_Main,  //78
PQ_IP_SRAM1_COM_SC1_Main,  //79
PQ_IP_SRAM2_COM_SC1_Main,  //80
PQ_IP_SRAM3_COM_SC1_Main,  //81
PQ_IP_SRAM4_COM_SC1_Main,  //82
PQ_IP_C_SRAM1_COM_SC1_Main,  //83
PQ_IP_C_SRAM2_COM_SC1_Main,  //84
PQ_IP_C_SRAM3_COM_SC1_Main,  //85
PQ_IP_C_SRAM4_COM_SC1_Main,  //86
PQ_IP_VIP_COM_SC1_Main,  //87
PQ_IP_VIP_pseudo_COM_SC1_Main,  //88
PQ_IP_VIP_CSC_COM_SC1_Main,  //89
PQ_IP_VIP_CSC_dither_COM_SC1_Main,  //90
PQ_IP_VIP_Post_YC_delay_COM_SC1_Main,  //91
PQ_IP_VIP_HNMR_Y_COM_SC1_Main,  //92
PQ_IP_VIP_HNMR_ad_C_COM_SC1_Main,  //93
PQ_IP_VIP_HNMR_ad_C_gain_COM_SC1_Main,  //94
PQ_IP_VIP_HNMR_C_win1_COM_SC1_Main,  //95
PQ_IP_VIP_HNMR_C_win2_COM_SC1_Main,  //96
PQ_IP_VIP_HNMR_C_win3_COM_SC1_Main,  //97
PQ_IP_VIP_HNMR_C_win4_COM_SC1_Main,  //98
PQ_IP_VIP_Pre_Yoffset_COM_SC1_Main,  //99
PQ_IP_VIP_Pre_Ygain_COM_SC1_Main,  //100
PQ_IP_VIP_Pre_Ygain_dither_COM_SC1_Main,  //101
PQ_IP_VIP_HLPF_COM_SC1_Main,  //102
PQ_IP_VIP_HLPF_dither_COM_SC1_Main,  //103
PQ_IP_VIP_Peaking_COM_SC1_Main,  //104
PQ_IP_VIP_Peaking_band_COM_SC1_Main,  //105
PQ_IP_VIP_Peaking_Pcoring_COM_SC1_Main,  //106
PQ_IP_VIP_Peaking_gain_COM_SC1_Main,  //107
PQ_IP_VIP_Peaking_gain_ad_C_COM_SC1_Main,  //108
PQ_IP_VIP_Post_CTI_COM_SC1_Main,  //109
PQ_IP_VIP_Post_CTI_coef_COM_SC1_Main,  //110
PQ_IP_VIP_FCC_T1_COM_SC1_Main,  //111
PQ_IP_VIP_FCC_T2_COM_SC1_Main,  //112
PQ_IP_VIP_FCC_T3_COM_SC1_Main,  //113
PQ_IP_VIP_FCC_T4_COM_SC1_Main,  //114
PQ_IP_VIP_FCC_T5_COM_SC1_Main,  //115
PQ_IP_VIP_FCC_T6_COM_SC1_Main,  //116
PQ_IP_VIP_FCC_T7_COM_SC1_Main,  //117
PQ_IP_VIP_FCC_T8_COM_SC1_Main,  //118
PQ_IP_VIP_FCC_T9_COM_SC1_Main,  //119
PQ_IP_VIP_IHC_COM_SC1_Main,  //120
PQ_IP_VIP_IHC_R_COM_SC1_Main,  //121
PQ_IP_VIP_IHC_G_COM_SC1_Main,  //122
PQ_IP_VIP_IHC_B_COM_SC1_Main,  //123
PQ_IP_VIP_IHC_C_COM_SC1_Main,  //124
PQ_IP_VIP_IHC_M_COM_SC1_Main,  //125
PQ_IP_VIP_IHC_Y_COM_SC1_Main,  //126
PQ_IP_VIP_IHC_F_COM_SC1_Main,  //127
PQ_IP_VIP_ICC_COM_SC1_Main,  //128
PQ_IP_VIP_ICC_R_COM_SC1_Main,  //129
PQ_IP_VIP_ICC_G_COM_SC1_Main,  //130
PQ_IP_VIP_ICC_B_COM_SC1_Main,  //131
PQ_IP_VIP_ICC_C_COM_SC1_Main,  //132
PQ_IP_VIP_ICC_M_COM_SC1_Main,  //133
PQ_IP_VIP_ICC_Y_COM_SC1_Main,  //134
PQ_IP_VIP_ICC_F_COM_SC1_Main,  //135
PQ_IP_VIP_ICC_NC_COM_SC1_Main,  //136
PQ_IP_VIP_IBC_COM_SC1_Main,  //137
PQ_IP_VIP_IBC_R_COM_SC1_Main,  //138
PQ_IP_VIP_IBC_G_COM_SC1_Main,  //139
PQ_IP_VIP_IBC_B_COM_SC1_Main,  //140
PQ_IP_VIP_IBC_C_COM_SC1_Main,  //141
PQ_IP_VIP_IBC_M_COM_SC1_Main,  //142
PQ_IP_VIP_IBC_Y_COM_SC1_Main,  //143
PQ_IP_VIP_IBC_F_COM_SC1_Main,  //144
PQ_IP_VIP_DLC_COM_SC1_Main,  //145
PQ_IP_VIP_DLC_dither_COM_SC1_Main,  //146
PQ_IP_VIP_DLC_Flesh_adj_COM_SC1_Main,  //147
PQ_IP_VIP_DLC_His_range_COM_SC1_Main,  //148
PQ_IP_VIP_DLC_His_rangeH_COM_SC1_Main,  //149
PQ_IP_VIP_DLC_His_rangeV_COM_SC1_Main,  //150
PQ_IP_VIP_BLE_COM_SC1_Main,  //151
PQ_IP_VIP_WLE_COM_SC1_Main,  //152
PQ_IP_VIP_BWLE_dither_COM_SC1_Main,  //153
PQ_IP_VIP_UVC_COM_SC1_Main,  //154
PQ_IP_VIP_Post_Yoffset_COM_SC1_Main,  //155
PQ_IP_VIP_Post_Ygain_COM_SC1_Main,  //156
PQ_IP_VIP_Post_Cgain_COM_SC1_Main,  //157
PQ_IP_VIP_Hcoring_Y_COM_SC1_Main,  //158
PQ_IP_VIP_Hcoring_C_COM_SC1_Main,  //159
PQ_IP_VIP_Hcoring_dither_COM_SC1_Main,  //160
PQ_IP_SwDriver_COM_SC1_Main,  //161
PQ_IP_3x3_COM_SC1_Main,  //162
PQ_IP_RGB_Offset_COM_SC1_Main,  //163
PQ_IP_RGB_Clip_COM_SC1_Main,  //164
PQ_IP_xvYCC_bypass_COM_SC1_Main,  //165
PQ_IP_xvYCC_de_gamma_COM_SC1_Main,  //166
PQ_IP_xvYCC_de_gamma_dither_COM_SC1_Main,  //167
PQ_IP_xvYCC_3x3_COM_SC1_Main,  //168
PQ_IP_xvYCC_Comp_COM_SC1_Main,  //169
PQ_IP_xvYCC_Comp_dither_COM_SC1_Main,  //170
PQ_IP_xvYCC_Clip_COM_SC1_Main,  //171
PQ_IP_xvYCC_gamma_COM_SC1_Main,  //172
PQ_IP_xvYCC_gamma_dither_COM_SC1_Main,  //173
PQ_IP_rgb_3d_COM_SC1_Main,  //174
PQ_IP_HBC_COM_SC1_Main,  //175
PQ_IP_Pre_CON_BRI_COM_SC1_Main,  //176
PQ_IP_Blue_Stretch_COM_SC1_Main,  //177
PQ_IP_Blue_Stretch_dither_COM_SC1_Main,  //178
PQ_IP_Gamma_COM_SC1_Main,  //179
PQ_IP_Gamma_dither_COM_SC1_Main,  //180
PQ_IP_Post_CON_BRI_COM_SC1_Main,  //181
PQ_IP_SRAM_3x3matrix_PIC2_COM_SC1_Main,  //182
PQ_IP_SRAM_DLC_PIC1_COM_SC1_Main,  //183
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_COM_SC1_Main,  //184
PQ_IP_SRAM_GammaTbl_R_PIC1_COM_SC1_Main,  //185
PQ_IP_SRAM_GammaTbl_G_PIC1_COM_SC1_Main,  //186
PQ_IP_SRAM_GammaTbl_B_PIC1_COM_SC1_Main,  //187
PQ_IP_SRAM_Auto_Color_PIC2_COM_SC1_Main,  //188
PQ_IP_SRAM_Color_Temp_PIC1_COM_SC1_Main,  //189
PQ_IP_COM_NUMS_SC1_Main
} PQ_IP_COM_Group_SC1_Main;

typedef enum
{
PQ_IP_AFEC_ADCIN_SC1_Main,
PQ_IP_AFEC_RFIN_SC1_Main,
PQ_IP_AFEC_ADCIN_SECAM_SC1_Main,
PQ_IP_AFEC_NUMS_SC1_Main
} PQ_IP_AFEC_Group_SC1_Main;

typedef enum
{
PQ_IP_Comb_NTSC_AV_SC1_Main,
PQ_IP_Comb_PAL_AV_SC1_Main,
PQ_IP_Comb_NTSC_RF_SC1_Main,
PQ_IP_Comb_PAL_RF_SC1_Main,
PQ_IP_Comb_PAL_RF_VIF_SC1_Main,
PQ_IP_Comb_NTSC_SV_SC1_Main,
PQ_IP_Comb_PAL_SV_SC1_Main,
PQ_IP_Comb_NTSC443_SC1_Main,
PQ_IP_Comb_NTSC443_SV_SC1_Main,
PQ_IP_Comb_PAL_M_SC1_Main,
PQ_IP_Comb_PAL60_SC1_Main,
PQ_IP_Comb_SECAM_SC1_Main,
PQ_IP_Comb_NTSC_AV_SONY_SC1_Main,
PQ_IP_Comb_PAL_AV_SONY_SC1_Main,
PQ_IP_Comb_NTSC_RF_SONY_SC1_Main,
PQ_IP_Comb_PAL_RF_SONY_SC1_Main,
PQ_IP_Comb_NUMS_SC1_Main
} PQ_IP_Comb_Group_SC1_Main;

typedef enum
{
PQ_IP_Comb2_NTSC_AV_SC1_Main,
PQ_IP_Comb2_PAL_AV_SC1_Main,
PQ_IP_Comb2_NTSC_RF_SC1_Main,
PQ_IP_Comb2_PAL_RF_SC1_Main,
PQ_IP_Comb2_PAL_RF_VIF_SC1_Main,
PQ_IP_Comb2_NTSC_SV_SC1_Main,
PQ_IP_Comb2_PAL_SV_SC1_Main,
PQ_IP_Comb2_NTSC443_SC1_Main,
PQ_IP_Comb2_NTSC443_SV_SC1_Main,
PQ_IP_Comb2_PAL_M_SC1_Main,
PQ_IP_Comb2_PAL60_SC1_Main,
PQ_IP_Comb2_SECAM_SC1_Main,
PQ_IP_Comb2_NTSC_AV_SONY_SC1_Main,
PQ_IP_Comb2_PAL_AV_SONY_SC1_Main,
PQ_IP_Comb2_NTSC_RF_SONY_SC1_Main,
PQ_IP_Comb2_PAL_RF_SONY_SC1_Main,
PQ_IP_Comb2_NUMS_SC1_Main
} PQ_IP_Comb2_Group_SC1_Main;

typedef enum
{
PQ_IP_SECAM_ALL_SC1_Main,
PQ_IP_SECAM_NUMS_SC1_Main
} PQ_IP_SECAM_Group_SC1_Main;

typedef enum
{
PQ_IP_VD_Sampling_no_comm_N_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P6_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_S_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_N4_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PM_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PNC_1135_SC1_Main,
PQ_IP_VD_Sampling_no_comm_N_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P6_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_S_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_N4_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PM_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PNC_1135_1o5_SC1_Main,
PQ_IP_VD_Sampling_no_comm_N_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_P6_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_S_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_N4_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PM_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_PNC_Dynamic_SC1_Main,
PQ_IP_VD_Sampling_no_comm_NUMS_SC1_Main
} PQ_IP_VD_Sampling_no_comm_Group_SC1_Main;

typedef enum
{
PQ_IP_ADC_Sampling_x1_SC1_Main,
PQ_IP_ADC_Sampling_x2_SC1_Main,
PQ_IP_ADC_Sampling_x4_SC1_Main,
PQ_IP_ADC_Sampling_NUMS_SC1_Main
} PQ_IP_ADC_Sampling_Group_SC1_Main;

typedef enum
{
PQ_IP_SCinit_Init_SC1_Main,
PQ_IP_SCinit_NUMS_SC1_Main
} PQ_IP_SCinit_Group_SC1_Main;

typedef enum
{
PQ_IP_CSC_OFF_SC1_Main,
PQ_IP_CSC_L_RGB2YCC_SD_SC1_Main,
PQ_IP_CSC_F_RGB2YCC_SD_SC1_Main,
PQ_IP_CSC_L_RGB2YCC_HD_SC1_Main,
PQ_IP_CSC_F_RGB2YCC_HD_SC1_Main,
PQ_IP_CSC_NUMS_SC1_Main
} PQ_IP_CSC_Group_SC1_Main;

typedef enum
{
PQ_IP_CSC_Dither_OFF_SC1_Main,
PQ_IP_CSC_Dither_ON_SC1_Main,
PQ_IP_CSC_Dither_NUMS_SC1_Main
} PQ_IP_CSC_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_YCdelay_OFF_SC1_Main,
PQ_IP_YCdelay_YC_91_SC1_Main,
PQ_IP_YCdelay_YC_92_SC1_Main,
PQ_IP_YCdelay_NUMS_SC1_Main
} PQ_IP_YCdelay_Group_SC1_Main;

typedef enum
{
PQ_IP_PreFilter_OFF_SC1_Main,
PQ_IP_PreFilter_FIR40_SC1_Main,
PQ_IP_PreFilter_FIR45_SC1_Main,
PQ_IP_PreFilter_FIR50_SC1_Main,
PQ_IP_PreFilter_FIR60_SC1_Main,
PQ_IP_PreFilter_FIR66_SC1_Main,
PQ_IP_PreFilter_FIR75_SC1_Main,
PQ_IP_PreFilter_FIR80_SC1_Main,
PQ_IP_PreFilter_FIR85_SC1_Main,
PQ_IP_PreFilter_FIR90_SC1_Main,
PQ_IP_PreFilter_FIR95_SC1_Main,
PQ_IP_PreFilter_NUMS_SC1_Main
} PQ_IP_PreFilter_Group_SC1_Main;

typedef enum
{
PQ_IP_PreFilter_Dither_OFF_SC1_Main,
PQ_IP_PreFilter_Dither_ON_SC1_Main,
PQ_IP_PreFilter_Dither_NUMS_SC1_Main
} PQ_IP_PreFilter_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_HDSDD_OFF_SC1_Main,
PQ_IP_HDSDD_D0_SC1_Main,
PQ_IP_HDSDD_D1_SC1_Main,
PQ_IP_HDSDD_D2_SC1_Main,
PQ_IP_HDSDD_D3_SC1_Main,
PQ_IP_HDSDD_D4_SC1_Main,
PQ_IP_HDSDD_D0_H_SC1_Main,
PQ_IP_HDSDD_D1_H_SC1_Main,
PQ_IP_HDSDD_D2_H_SC1_Main,
PQ_IP_HDSDD_D3_H_SC1_Main,
PQ_IP_HDSDD_D4_H_SC1_Main,
PQ_IP_HDSDD_NUMS_SC1_Main
} PQ_IP_HDSDD_Group_SC1_Main;

typedef enum
{
PQ_IP_HSD_Sampling_Div_1o000_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o125_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o250_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o375_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o500_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o625_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o750_SC1_Main,
PQ_IP_HSD_Sampling_Div_1o875_SC1_Main,
PQ_IP_HSD_Sampling_Div_2o000_SC1_Main,
PQ_IP_HSD_Sampling_NUMS_SC1_Main
} PQ_IP_HSD_Sampling_Group_SC1_Main;

typedef enum
{
PQ_IP_HSD_Y_OFF_SC1_Main,
PQ_IP_HSD_Y_CB_SC1_Main,
PQ_IP_HSD_Y_LpfFc40Ap0As40_SC1_Main,
PQ_IP_HSD_Y_LpfFc50Ap0As40_SC1_Main,
PQ_IP_HSD_Y_LpfFc60Ap0As35_SC1_Main,
PQ_IP_HSD_Y_LpfFc70Ap0As35_SC1_Main,
PQ_IP_HSD_Y_LpfFc80Ap0As35_SC1_Main,
PQ_IP_HSD_Y_LpfFc90Ap0As35_SC1_Main,
PQ_IP_HSD_Y_ALLPASS1X_SC1_Main,
PQ_IP_HSD_Y_FIR66_SC1_Main,
PQ_IP_HSD_Y_FIR60_SC1_Main,
PQ_IP_HSD_Y_FIR55_SC1_Main,
PQ_IP_HSD_Y_FIR50_SC1_Main,
PQ_IP_HSD_Y_NUMS_SC1_Main
} PQ_IP_HSD_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_HSD_C_OFF_SC1_Main,
PQ_IP_HSD_C_CB_SC1_Main,
PQ_IP_HSD_C_LpfFc15Ap0As45_SC1_Main,
PQ_IP_HSD_C_LpfFc15Ap0As55_SC1_Main,
PQ_IP_HSD_C_ALLPASS1X_SC1_Main,
PQ_IP_HSD_C_FIR50_SC1_Main,
PQ_IP_HSD_C_FIR60_SC1_Main,
PQ_IP_HSD_C_NUMS_SC1_Main
} PQ_IP_HSD_C_Group_SC1_Main;

typedef enum
{
PQ_IP_444To422_OFF_SC1_Main,
PQ_IP_444To422_ON_SC1_Main,
PQ_IP_444To422_NUMS_SC1_Main
} PQ_IP_444To422_Group_SC1_Main;

typedef enum
{
PQ_IP_VSD_OFF_SC1_Main,
PQ_IP_VSD_CB_SC1_Main,
PQ_IP_VSD_Bilinear_SC1_Main,
PQ_IP_VSD_NUMS_SC1_Main
} PQ_IP_VSD_Group_SC1_Main;

typedef enum
{
PQ_IP_HVSD_Dither_OFF_SC1_Main,
PQ_IP_HVSD_Dither_Vdith_SC1_Main,
PQ_IP_HVSD_Dither_Hdith_SC1_Main,
PQ_IP_HVSD_Dither_HVDith_SC1_Main,
PQ_IP_HVSD_Dither_NUMS_SC1_Main
} PQ_IP_HVSD_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_10to8_Dither_OFF_SC1_Main,
PQ_IP_10to8_Dither_Rand_Dith_SC1_Main,
PQ_IP_10to8_Dither_Fix_Dith_SC1_Main,
PQ_IP_10to8_Dither_NUMS_SC1_Main
} PQ_IP_10to8_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_MemFormat_422MF_SC1_Main,
PQ_IP_MemFormat_444_10BIT_SC1_Main,
PQ_IP_MemFormat_444_8BIT_SC1_Main,
PQ_IP_MemFormat_NUMS_SC1_Main
} PQ_IP_MemFormat_Group_SC1_Main;

typedef enum
{
PQ_IP_PreSNR_OFF_SC1_Main,
PQ_IP_PreSNR_PS_1_SC1_Main,
PQ_IP_PreSNR_PS_2_SC1_Main,
PQ_IP_PreSNR_PS_3_SC1_Main,
PQ_IP_PreSNR_PS_4_SC1_Main,
PQ_IP_PreSNR_PS_5_SC1_Main,
PQ_IP_PreSNR_PS_6_SC1_Main,
PQ_IP_PreSNR_PS_7_SC1_Main,
PQ_IP_PreSNR_PS_8_SC1_Main,
PQ_IP_PreSNR_PS_9_SC1_Main,
PQ_IP_PreSNR_PS_10_SC1_Main,
PQ_IP_PreSNR_PS_11_SC1_Main,
PQ_IP_PreSNR_PS_12_SC1_Main,
PQ_IP_PreSNR_PS_13_SC1_Main,
PQ_IP_PreSNR_PS_14_SC1_Main,
PQ_IP_PreSNR_PS_15_SC1_Main,
PQ_IP_PreSNR_NUMS_SC1_Main
} PQ_IP_PreSNR_Group_SC1_Main;

typedef enum
{
PQ_IP_PreSNR_Patch_OFF_SC1_Main,
PQ_IP_PreSNR_Patch_PSP_1_SC1_Main,
PQ_IP_PreSNR_Patch_PSP_2_SC1_Main,
PQ_IP_PreSNR_Patch_NUMS_SC1_Main
} PQ_IP_PreSNR_Patch_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_NR_OFF_SC1_Main,
PQ_IP_DNR_DNR_OFF_SC1_Main,
PQ_IP_DNR_ON_SC1_Main,
PQ_IP_DNR_NUMS_SC1_Main
} PQ_IP_DNR_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_Motion_MR_NR_SC1_Main,
PQ_IP_DNR_Motion_MR_nonNR_SC1_Main,
PQ_IP_DNR_Motion_NUMS_SC1_Main
} PQ_IP_DNR_Motion_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_Y_OFF_SC1_Main,
PQ_IP_DNR_Y_DY_1_SC1_Main,
PQ_IP_DNR_Y_DY_1_1_SC1_Main,
PQ_IP_DNR_Y_DY_2_SC1_Main,
PQ_IP_DNR_Y_DY_2_1_SC1_Main,
PQ_IP_DNR_Y_DY_3_SC1_Main,
PQ_IP_DNR_Y_DY_3_1_SC1_Main,
PQ_IP_DNR_Y_DY_4_SC1_Main,
PQ_IP_DNR_Y_DY_4_1_SC1_Main,
PQ_IP_DNR_Y_DY_5_SC1_Main,
PQ_IP_DNR_Y_DY_5_1_SC1_Main,
PQ_IP_DNR_Y_DY_6_SC1_Main,
PQ_IP_DNR_Y_DY_6_1_SC1_Main,
PQ_IP_DNR_Y_DY_7_SC1_Main,
PQ_IP_DNR_Y_DY_7_1_SC1_Main,
PQ_IP_DNR_Y_DY_8_SC1_Main,
PQ_IP_DNR_Y_DY_8_1_SC1_Main,
PQ_IP_DNR_Y_DY_9_SC1_Main,
PQ_IP_DNR_Y_DY_9_1_SC1_Main,
PQ_IP_DNR_Y_DY_10_SC1_Main,
PQ_IP_DNR_Y_DY_10_1_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear0_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear01_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear1_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear12_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear2_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear23_SC1_Main,
PQ_IP_DNR_Y_DY_nonlinear3_SC1_Main,
PQ_IP_DNR_Y_NUMS_SC1_Main
} PQ_IP_DNR_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_Y_COLOR_DEP_OFF_SC1_Main,
PQ_IP_DNR_Y_COLOR_DEP_DYCD_1_SC1_Main,
PQ_IP_DNR_Y_COLOR_DEP_NUMS_SC1_Main
} PQ_IP_DNR_Y_COLOR_DEP_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_INDEX_Index0_SC1_Main,
PQ_IP_SRAM_COLOR_INDEX_NUMS_SC1_Main
} PQ_IP_SRAM_COLOR_INDEX_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_SNR_GainSNR0_SC1_Main,
PQ_IP_SRAM_COLOR_GAIN_SNR_NUMS_SC1_Main
} PQ_IP_SRAM_COLOR_GAIN_SNR_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR0_SC1_Main,
PQ_IP_SRAM_COLOR_GAIN_DNR_NUMS_SC1_Main
} PQ_IP_SRAM_COLOR_GAIN_DNR_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_Y_LUMA_ADAPTIVE_OFF_SC1_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_DYP_1_SC1_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_DYP_2_SC1_Main,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_NUMS_SC1_Main
} PQ_IP_DNR_Y_LUMA_ADAPTIVE_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_POSTTUNE_OFF_SC1_Main,
PQ_IP_DNR_POSTTUNE_DYP_1_SC1_Main,
PQ_IP_DNR_POSTTUNE_NUMS_SC1_Main
} PQ_IP_DNR_POSTTUNE_Group_SC1_Main;

typedef enum
{
PQ_IP_DNR_C_OFF_SC1_Main,
PQ_IP_DNR_C_DC_1_SC1_Main,
PQ_IP_DNR_C_DC_1_1_SC1_Main,
PQ_IP_DNR_C_DC_2_SC1_Main,
PQ_IP_DNR_C_DC_2_1_SC1_Main,
PQ_IP_DNR_C_DC_3_SC1_Main,
PQ_IP_DNR_C_DC_3_1_SC1_Main,
PQ_IP_DNR_C_DC_4_SC1_Main,
PQ_IP_DNR_C_DC_4_1_SC1_Main,
PQ_IP_DNR_C_DC_5_SC1_Main,
PQ_IP_DNR_C_DC_5_1_SC1_Main,
PQ_IP_DNR_C_DC_6_SC1_Main,
PQ_IP_DNR_C_DC_6_1_SC1_Main,
PQ_IP_DNR_C_DC_7_SC1_Main,
PQ_IP_DNR_C_DC_7_1_SC1_Main,
PQ_IP_DNR_C_DC_8_SC1_Main,
PQ_IP_DNR_C_DC_8_1_SC1_Main,
PQ_IP_DNR_C_DC_9_SC1_Main,
PQ_IP_DNR_C_DC_9_1_SC1_Main,
PQ_IP_DNR_C_DC_10_SC1_Main,
PQ_IP_DNR_C_DC_10_1_SC1_Main,
PQ_IP_DNR_C_DC_nonlinear1_SC1_Main,
PQ_IP_DNR_C_DC_nonlinear2_SC1_Main,
PQ_IP_DNR_C_DC_nonlinear3_SC1_Main,
PQ_IP_DNR_C_NUMS_SC1_Main
} PQ_IP_DNR_C_Group_SC1_Main;

typedef enum
{
PQ_IP_HISDNR_OFF_SC1_Main,
PQ_IP_HISDNR_ON_SC1_Main,
PQ_IP_HISDNR_NUMS_SC1_Main
} PQ_IP_HISDNR_Group_SC1_Main;

typedef enum
{
PQ_IP_PNR_OFF_SC1_Main,
PQ_IP_PNR_PNR_PCCS_AVG_OFF_SC1_Main,
PQ_IP_PNR_ON_SC1_Main,
PQ_IP_PNR_AVG_ON_SC1_Main,
PQ_IP_PNR_NUMS_SC1_Main
} PQ_IP_PNR_Group_SC1_Main;

typedef enum
{
PQ_IP_PNR_Y_OFF_SC1_Main,
PQ_IP_PNR_Y_PY1_SC1_Main,
PQ_IP_PNR_Y_NUMS_SC1_Main
} PQ_IP_PNR_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_PNR_C_OFF_SC1_Main,
PQ_IP_PNR_C_PC1_SC1_Main,
PQ_IP_PNR_C_NUMS_SC1_Main
} PQ_IP_PNR_C_Group_SC1_Main;

typedef enum
{
PQ_IP_PostCCS_OFF_SC1_Main,
PQ_IP_PostCCS_PC_0_SC1_Main,
PQ_IP_PostCCS_PC_1_SC1_Main,
PQ_IP_PostCCS_PC_2_SC1_Main,
PQ_IP_PostCCS_PC_3_SC1_Main,
PQ_IP_PostCCS_PC_4_SC1_Main,
PQ_IP_PostCCS_PC_5_SC1_Main,
PQ_IP_PostCCS_PC_6_SC1_Main,
PQ_IP_PostCCS_PC_7_SC1_Main,
PQ_IP_PostCCS_PC_8_SC1_Main,
PQ_IP_PostCCS_PC_9_SC1_Main,
PQ_IP_PostCCS_PC_10_SC1_Main,
PQ_IP_PostCCS_PC_11_SC1_Main,
PQ_IP_PostCCS_PC_12_SC1_Main,
PQ_IP_PostCCS_PC_13_SC1_Main,
PQ_IP_PostCCS_PC_14_SC1_Main,
PQ_IP_PostCCS_PC_15_SC1_Main,
PQ_IP_PostCCS_PC_16_SC1_Main,
PQ_IP_PostCCS_PC_17_SC1_Main,
PQ_IP_PostCCS_PC_18_SC1_Main,
PQ_IP_PostCCS_PC_19_SC1_Main,
PQ_IP_PostCCS_PC_20_SC1_Main,
PQ_IP_PostCCS_PC_21_SC1_Main,
PQ_IP_PostCCS_PC_22_SC1_Main,
PQ_IP_PostCCS_PC_23_SC1_Main,
PQ_IP_PostCCS_PC_24_SC1_Main,
PQ_IP_PostCCS_NUMS_SC1_Main
} PQ_IP_PostCCS_Group_SC1_Main;

typedef enum
{
PQ_IP_PostCCS_Smooth_OFF_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_0_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_1_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_2_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_3_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_4_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_5_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_6_SC1_Main,
PQ_IP_PostCCS_Smooth_PCS_7_SC1_Main,
PQ_IP_PostCCS_Smooth_NUMS_SC1_Main
} PQ_IP_PostCCS_Smooth_Group_SC1_Main;

typedef enum
{
PQ_IP_420CUP_OFF_SC1_Main,
PQ_IP_420CUP_ON_SC1_Main,
PQ_IP_420CUP_NUMS_SC1_Main
} PQ_IP_420CUP_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_24_4R_SC1_Main,
PQ_IP_MADi_24_2R_SC1_Main,
PQ_IP_MADi_25_4R_SC1_Main,
PQ_IP_MADi_25_2R_SC1_Main,
PQ_IP_MADi_26_4R_SC1_Main,
PQ_IP_MADi_26_2R_SC1_Main,
PQ_IP_MADi_27_4R_SC1_Main,
PQ_IP_MADi_27_2R_SC1_Main,
PQ_IP_MADi_P_MODE8_SC1_Main,
PQ_IP_MADi_P_MODE10_SC1_Main,
PQ_IP_MADi_P_MODE_MOT10_8Frame_SC1_Main,
PQ_IP_MADi_P_MODE_MOT10_SC1_Main,
PQ_IP_MADi_P_MODE_MOT8_SC1_Main,
PQ_IP_MADi_24_4R_880_SC1_Main,
PQ_IP_MADi_24_2R_880_SC1_Main,
PQ_IP_MADi_25_4R_880_SC1_Main,
PQ_IP_MADi_25_4R_884_SC1_Main,
PQ_IP_MADi_25_2R_884_SC1_Main,
PQ_IP_MADi_25_2R_880_SC1_Main,
PQ_IP_MADi_25_4R_MC_SC1_Main,
PQ_IP_MADi_25_4R_MC_NW_SC1_Main,
PQ_IP_MADi_25_6R_MC_NW_SC1_Main,
PQ_IP_MADi_25_6R_MC_SC1_Main,
PQ_IP_MADi_25_12F_8R_MC_SC1_Main,
PQ_IP_MADi_25_14F_8R_MC_SC1_Main,
PQ_IP_MADi_25_16F_8R_MC_SC1_Main,
PQ_IP_MADi_P_MODE8_444_SC1_Main,
PQ_IP_MADi_P_MODE10_444_SC1_Main,
PQ_IP_MADi_P_MODE_MOT10_4Frame_SC1_Main,
PQ_IP_MADi_25_14F_6R_MC_SC1_Main,
PQ_IP_MADi_25_8F_4R_MC_SC1_Main,
PQ_IP_MADi_NUMS_SC1_Main
} PQ_IP_MADi_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_Motion_MOT_4R_5_SC1_Main,
PQ_IP_MADi_Motion_MOT_4R_6_SC1_Main,
PQ_IP_MADi_Motion_MOT_4R_7_SC1_Main,
PQ_IP_MADi_Motion_MOT_2R_SC1_Main,
PQ_IP_MADi_Motion_MOT_4R_5_MC_SC1_Main,
PQ_IP_MADi_Motion_MOT_PMODE_SC1_Main,
PQ_IP_MADi_Motion_NUMS_SC1_Main
} PQ_IP_MADi_Motion_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_ADP3x3_OFF_SC1_Main,
PQ_IP_MADi_ADP3x3_ADP1_SC1_Main,
PQ_IP_MADi_ADP3x3_NUMS_SC1_Main
} PQ_IP_MADi_ADP3x3_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_MORPHO_OFF_SC1_Main,
PQ_IP_MADi_MORPHO_M1_SC1_Main,
PQ_IP_MADi_MORPHO_NUMS_SC1_Main
} PQ_IP_MADi_MORPHO_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_DFK_OFF_SC1_Main,
PQ_IP_MADi_DFK_DFK1_SC1_Main,
PQ_IP_MADi_DFK_DFK2_SC1_Main,
PQ_IP_MADi_DFK_DFK3_SC1_Main,
PQ_IP_MADi_DFK_DFK4_SC1_Main,
PQ_IP_MADi_DFK_DFK5_SC1_Main,
PQ_IP_MADi_DFK_DFK6_SC1_Main,
PQ_IP_MADi_DFK_NUMS_SC1_Main
} PQ_IP_MADi_DFK_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_SST_OFF_SC1_Main,
PQ_IP_MADi_SST_SST1_SC1_Main,
PQ_IP_MADi_SST_SST2_SC1_Main,
PQ_IP_MADi_SST_SST3_SC1_Main,
PQ_IP_MADi_SST_SST4_SC1_Main,
PQ_IP_MADi_SST_SST5_SC1_Main,
PQ_IP_MADi_SST_SST6_SC1_Main,
PQ_IP_MADi_SST_SST_Rec1_SC1_Main,
PQ_IP_MADi_SST_NUMS_SC1_Main
} PQ_IP_MADi_SST_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_EODiW_OFF_SC1_Main,
PQ_IP_MADi_EODiW_W1_SC1_Main,
PQ_IP_MADi_EODiW_W2_SC1_Main,
PQ_IP_MADi_EODiW_W3_SC1_Main,
PQ_IP_MADi_EODiW_NUMS_SC1_Main
} PQ_IP_MADi_EODiW_Group_SC1_Main;

typedef enum
{
PQ_IP_MADi_Force_OFF_SC1_Main,
PQ_IP_MADi_Force_YC_FullMotion_SC1_Main,
PQ_IP_MADi_Force_YC_FullStill_SC1_Main,
PQ_IP_MADi_Force_Y_FullMotion_SC1_Main,
PQ_IP_MADi_Force_Y_FullStill_SC1_Main,
PQ_IP_MADi_Force_C_FullMotion_SC1_Main,
PQ_IP_MADi_Force_C_FullStill_SC1_Main,
PQ_IP_MADi_Force_NUMS_SC1_Main
} PQ_IP_MADi_Force_Group_SC1_Main;

typedef enum
{
PQ_IP_EODi_OFF_SC1_Main,
PQ_IP_EODi_SD_15_SC1_Main,
PQ_IP_EODi_SD_14_SC1_Main,
PQ_IP_EODi_SD_13_SC1_Main,
PQ_IP_EODi_SD_12_SC1_Main,
PQ_IP_EODi_SD_11_SC1_Main,
PQ_IP_EODi_SD_10_SC1_Main,
PQ_IP_EODi_SD_9_SC1_Main,
PQ_IP_EODi_SD_8_SC1_Main,
PQ_IP_EODi_SD_7_SC1_Main,
PQ_IP_EODi_SD_6_SC1_Main,
PQ_IP_EODi_SD_5_SC1_Main,
PQ_IP_EODi_SD_4_SC1_Main,
PQ_IP_EODi_SD_3_SC1_Main,
PQ_IP_EODi_SD_2_SC1_Main,
PQ_IP_EODi_SD_1_SC1_Main,
PQ_IP_EODi_HD_15_SC1_Main,
PQ_IP_EODi_HD_14_SC1_Main,
PQ_IP_EODi_HD_13_SC1_Main,
PQ_IP_EODi_HD_12_SC1_Main,
PQ_IP_EODi_HD_11_SC1_Main,
PQ_IP_EODi_HD_10_SC1_Main,
PQ_IP_EODi_HD_9_SC1_Main,
PQ_IP_EODi_HD_8_SC1_Main,
PQ_IP_EODi_HD_7_SC1_Main,
PQ_IP_EODi_HD_6_SC1_Main,
PQ_IP_EODi_HD_5_SC1_Main,
PQ_IP_EODi_HD_4_SC1_Main,
PQ_IP_EODi_HD_3_SC1_Main,
PQ_IP_EODi_HD_2_SC1_Main,
PQ_IP_EODi_HD_1_SC1_Main,
PQ_IP_EODi_NUMS_SC1_Main
} PQ_IP_EODi_Group_SC1_Main;

typedef enum
{
PQ_IP_Film_OFF_SC1_Main,
PQ_IP_Film_SD_2_SC1_Main,
PQ_IP_Film_SD_1_SC1_Main,
PQ_IP_Film_SD_3_SC1_Main,
PQ_IP_Film_HD_2_SC1_Main,
PQ_IP_Film_HD_1_SC1_Main,
PQ_IP_Film_HD_3_SC1_Main,
PQ_IP_Film_NUMS_SC1_Main
} PQ_IP_Film_Group_SC1_Main;

typedef enum
{
PQ_IP_Film32_OFF_SC1_Main,
PQ_IP_Film32_SD_1_SC1_Main,
PQ_IP_Film32_SD_2_SC1_Main,
PQ_IP_Film32_SD_3_SC1_Main,
PQ_IP_Film32_SD_4_SC1_Main,
PQ_IP_Film32_SD_5_SC1_Main,
PQ_IP_Film32_SD_6_SC1_Main,
PQ_IP_Film32_SD_7_SC1_Main,
PQ_IP_Film32_SD_8_SC1_Main,
PQ_IP_Film32_SD_9_SC1_Main,
PQ_IP_Film32_SD_10_SC1_Main,
PQ_IP_Film32_HD_1_SC1_Main,
PQ_IP_Film32_HD_2_SC1_Main,
PQ_IP_Film32_HD_3_SC1_Main,
PQ_IP_Film32_HD_4_SC1_Main,
PQ_IP_Film32_HD_5_SC1_Main,
PQ_IP_Film32_HD_6_SC1_Main,
PQ_IP_Film32_HD_1_MC_SC1_Main,
PQ_IP_Film32_HD_2_MC_SC1_Main,
PQ_IP_Film32_HD_3_MC_SC1_Main,
PQ_IP_Film32_HD_4_MC_SC1_Main,
PQ_IP_Film32_HD_5_MC_SC1_Main,
PQ_IP_Film32_HD_6_MC_SC1_Main,
PQ_IP_Film32_NUMS_SC1_Main
} PQ_IP_Film32_Group_SC1_Main;

typedef enum
{
PQ_IP_Film22_OFF_SC1_Main,
PQ_IP_Film22_SD_1_SC1_Main,
PQ_IP_Film22_SD_2_SC1_Main,
PQ_IP_Film22_SD_3_SC1_Main,
PQ_IP_Film22_SD_4_SC1_Main,
PQ_IP_Film22_SD_5_SC1_Main,
PQ_IP_Film22_SD_6_SC1_Main,
PQ_IP_Film22_SD_7_SC1_Main,
PQ_IP_Film22_SD_8_SC1_Main,
PQ_IP_Film22_SD_9_SC1_Main,
PQ_IP_Film22_SD_10_SC1_Main,
PQ_IP_Film22_HD_1_SC1_Main,
PQ_IP_Film22_HD_2_SC1_Main,
PQ_IP_Film22_HD_3_SC1_Main,
PQ_IP_Film22_HD_4_SC1_Main,
PQ_IP_Film22_HD_5_SC1_Main,
PQ_IP_Film22_HD_6_SC1_Main,
PQ_IP_Film22_NUMS_SC1_Main
} PQ_IP_Film22_Group_SC1_Main;

typedef enum
{
PQ_IP_Film_any_OFF_SC1_Main,
PQ_IP_Film_any_SD_1_SC1_Main,
PQ_IP_Film_any_SD_2_SC1_Main,
PQ_IP_Film_any_SD_3_SC1_Main,
PQ_IP_Film_any_SD_4_SC1_Main,
PQ_IP_Film_any_SD_5_SC1_Main,
PQ_IP_Film_any_SD_6_SC1_Main,
PQ_IP_Film_any_SD_7_SC1_Main,
PQ_IP_Film_any_SD_8_SC1_Main,
PQ_IP_Film_any_SD_9_SC1_Main,
PQ_IP_Film_any_SD_10_SC1_Main,
PQ_IP_Film_any_HD_1_SC1_Main,
PQ_IP_Film_any_HD_2_SC1_Main,
PQ_IP_Film_any_HD_3_SC1_Main,
PQ_IP_Film_any_HD_4_SC1_Main,
PQ_IP_Film_any_HD_5_SC1_Main,
PQ_IP_Film_any_HD_6_SC1_Main,
PQ_IP_Film_any_NUMS_SC1_Main
} PQ_IP_Film_any_Group_SC1_Main;

typedef enum
{
PQ_IP_UCNR_OFF_SC1_Main,
PQ_IP_UCNR_SD_0_SC1_Main,
PQ_IP_UCNR_SD_1_SC1_Main,
PQ_IP_UCNR_SD_2_SC1_Main,
PQ_IP_UCNR_SD_3_SC1_Main,
PQ_IP_UCNR_SD_4_SC1_Main,
PQ_IP_UCNR_HD_SC1_Main,
PQ_IP_UCNR_HD_1_SC1_Main,
PQ_IP_UCNR_SD_5_SC1_Main,
PQ_IP_UCNR_NUMS_SC1_Main
} PQ_IP_UCNR_Group_SC1_Main;

typedef enum
{
PQ_IP_UCDi_OFF_SC1_Main,
PQ_IP_UCDi_SD_1_SC1_Main,
PQ_IP_UCDi_SD_2_SC1_Main,
PQ_IP_UCDi_NUMS_SC1_Main
} PQ_IP_UCDi_Group_SC1_Main;

typedef enum
{
PQ_IP_UC_CTL_OFF_SC1_Main,
PQ_IP_UC_CTL_ON_SC1_Main,
PQ_IP_UC_CTL_ON_DHD_SC1_Main,
PQ_IP_UC_CTL_NUMS_SC1_Main
} PQ_IP_UC_CTL_Group_SC1_Main;

typedef enum
{
PQ_IP_DIPF_OFF_SC1_Main,
PQ_IP_DIPF_DIPF1_SC1_Main,
PQ_IP_DIPF_NUMS_SC1_Main
} PQ_IP_DIPF_Group_SC1_Main;

typedef enum
{
PQ_IP_VCLPF_OFF_SC1_Main,
PQ_IP_VCLPF_ON_SC1_Main,
PQ_IP_VCLPF_NUMS_SC1_Main
} PQ_IP_VCLPF_Group_SC1_Main;

typedef enum
{
PQ_IP_Spike_NR_OFF_SC1_Main,
PQ_IP_Spike_NR_S1_SC1_Main,
PQ_IP_Spike_NR_S2_SC1_Main,
PQ_IP_Spike_NR_S3_SC1_Main,
PQ_IP_Spike_NR_NUMS_SC1_Main
} PQ_IP_Spike_NR_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_OFF_SC1_Main,
PQ_IP_SPF_ON_SC1_Main,
PQ_IP_SPF_NUMS_SC1_Main
} PQ_IP_SPF_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_SNR_OFF_SC1_Main,
PQ_IP_SPF_SNR_E1S1_SC1_Main,
PQ_IP_SPF_SNR_E1S2_SC1_Main,
PQ_IP_SPF_SNR_E2S1_SC1_Main,
PQ_IP_SPF_SNR_E3S1_SC1_Main,
PQ_IP_SPF_SNR_E4S1_SC1_Main,
PQ_IP_SPF_SNR_E4S2_SC1_Main,
PQ_IP_SPF_SNR_E4S3_SC1_Main,
PQ_IP_SPF_SNR_NUMS_SC1_Main
} PQ_IP_SPF_SNR_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_SNR_MR_OFF_SC1_Main,
PQ_IP_SPF_SNR_MR_ON_SC1_Main,
PQ_IP_SPF_SNR_MR_NUMS_SC1_Main
} PQ_IP_SPF_SNR_MR_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_MR_LPF_OFF_SC1_Main,
PQ_IP_SPF_MR_LPF_LPF3x3_SC1_Main,
PQ_IP_SPF_MR_LPF_NUMS_SC1_Main
} PQ_IP_SPF_MR_LPF_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_NMR_Y_OFF_SC1_Main,
PQ_IP_SPF_NMR_Y_S1_SC1_Main,
PQ_IP_SPF_NMR_Y_S2_SC1_Main,
PQ_IP_SPF_NMR_Y_S3_SC1_Main,
PQ_IP_SPF_NMR_Y_NUMS_SC1_Main
} PQ_IP_SPF_NMR_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_SPF_NMR_Y_MR_OFF_SC1_Main,
PQ_IP_SPF_NMR_Y_MR_ON_SC1_Main,
PQ_IP_SPF_NMR_Y_MR_NUMS_SC1_Main
} PQ_IP_SPF_NMR_Y_MR_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_Y_Bypass_SC1_Main,
PQ_IP_VSP_Y_Bilinear_SC1_Main,
PQ_IP_VSP_Y_SRAM_1_4Tap_SC1_Main,
PQ_IP_VSP_Y_SRAM_2_4Tap_SC1_Main,
PQ_IP_VSP_Y_SRAM_1_6Tap_SC1_Main,
PQ_IP_VSP_Y_SRAM_2_6Tap_SC1_Main,
PQ_IP_VSP_Y_NUMS_SC1_Main
} PQ_IP_VSP_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_C_Bypass_SC1_Main,
PQ_IP_VSP_C_Bilinear_SC1_Main,
PQ_IP_VSP_C_C_SRAM_1_SC1_Main,
PQ_IP_VSP_C_C_SRAM_2_SC1_Main,
PQ_IP_VSP_C_C_SRAM_3_SC1_Main,
PQ_IP_VSP_C_C_SRAM_4_SC1_Main,
PQ_IP_VSP_C_SRAM_1_4Tap_SC1_Main,
PQ_IP_VSP_C_SRAM_2_4Tap_SC1_Main,
PQ_IP_VSP_C_NUMS_SC1_Main
} PQ_IP_VSP_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_CoRing_OFF_SC1_Main,
PQ_IP_VSP_CoRing_Y_Coring_1_SC1_Main,
PQ_IP_VSP_CoRing_Y_Coring_2_SC1_Main,
PQ_IP_VSP_CoRing_Y_Coring_3_SC1_Main,
PQ_IP_VSP_CoRing_Y_Coring_4_SC1_Main,
PQ_IP_VSP_CoRing_Y_Coring_5_SC1_Main,
PQ_IP_VSP_CoRing_NUMS_SC1_Main
} PQ_IP_VSP_CoRing_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_DeRing_OFF_SC1_Main,
PQ_IP_VSP_DeRing_DR1_SC1_Main,
PQ_IP_VSP_DeRing_NUMS_SC1_Main
} PQ_IP_VSP_DeRing_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_Dither_OFF_SC1_Main,
PQ_IP_VSP_Dither_ON_SC1_Main,
PQ_IP_VSP_Dither_NUMS_SC1_Main
} PQ_IP_VSP_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VSP_PreVBound_OFF_SC1_Main,
PQ_IP_VSP_PreVBound_ON_SC1_Main,
PQ_IP_VSP_PreVBound_NUMS_SC1_Main
} PQ_IP_VSP_PreVBound_Group_SC1_Main;

typedef enum
{
PQ_IP_422To444_ON_SC1_Main,
PQ_IP_422To444_OFF_SC1_Main,
PQ_IP_422To444_NUMS_SC1_Main
} PQ_IP_422To444_Group_SC1_Main;

typedef enum
{
PQ_IP_PreCTI_OFF_SC1_Main,
PQ_IP_PreCTI_CTI_0_SC1_Main,
PQ_IP_PreCTI_CTI_1_SC1_Main,
PQ_IP_PreCTI_NUMS_SC1_Main
} PQ_IP_PreCTI_Group_SC1_Main;

typedef enum
{
PQ_IP_HSP_Y_Bypass_SC1_Main,
PQ_IP_HSP_Y_Bilinear_SC1_Main,
PQ_IP_HSP_Y_SRAM_1_4Tap_SC1_Main,
PQ_IP_HSP_Y_SRAM_2_4Tap_SC1_Main,
PQ_IP_HSP_Y_SRAM_1_6Tap_SC1_Main,
PQ_IP_HSP_Y_SRAM_2_6Tap_SC1_Main,
PQ_IP_HSP_Y_NUMS_SC1_Main
} PQ_IP_HSP_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_HSP_C_Bypass_SC1_Main,
PQ_IP_HSP_C_Bilinear_SC1_Main,
PQ_IP_HSP_C_C_SRAM_1_SC1_Main,
PQ_IP_HSP_C_C_SRAM_2_SC1_Main,
PQ_IP_HSP_C_C_SRAM_3_SC1_Main,
PQ_IP_HSP_C_C_SRAM_4_SC1_Main,
PQ_IP_HSP_C_SRAM_1_4Tap_SC1_Main,
PQ_IP_HSP_C_SRAM_2_4Tap_SC1_Main,
PQ_IP_HSP_C_NUMS_SC1_Main
} PQ_IP_HSP_C_Group_SC1_Main;

typedef enum
{
PQ_IP_HSP_CoRing_OFF_SC1_Main,
PQ_IP_HSP_CoRing_Y_Coring_1_SC1_Main,
PQ_IP_HSP_CoRing_Y_Coring_2_SC1_Main,
PQ_IP_HSP_CoRing_Y_Coring_3_SC1_Main,
PQ_IP_HSP_CoRing_Y_Coring_4_SC1_Main,
PQ_IP_HSP_CoRing_Y_Coring_5_SC1_Main,
PQ_IP_HSP_CoRing_NUMS_SC1_Main
} PQ_IP_HSP_CoRing_Group_SC1_Main;

typedef enum
{
PQ_IP_HSP_DeRing_OFF_SC1_Main,
PQ_IP_HSP_DeRing_DR1_SC1_Main,
PQ_IP_HSP_DeRing_NUMS_SC1_Main
} PQ_IP_HSP_DeRing_Group_SC1_Main;

typedef enum
{
PQ_IP_HSP_Dither_OFF_SC1_Main,
PQ_IP_HSP_Dither_ON_SC1_Main,
PQ_IP_HSP_Dither_NUMS_SC1_Main
} PQ_IP_HSP_Dither_Group_SC1_Main;

typedef enum
{
PQ_IP_HnonLinear_OFF_SC1_Main,
PQ_IP_HnonLinear_H_1366_0_SC1_Main,
PQ_IP_HnonLinear_H_1366_1_SC1_Main,
PQ_IP_HnonLinear_H_1366_2_SC1_Main,
PQ_IP_HnonLinear_H_1440_SC1_Main,
PQ_IP_HnonLinear_H_1680_SC1_Main,
PQ_IP_HnonLinear_H_1920_0_SC1_Main,
PQ_IP_HnonLinear_H_1920_1_SC1_Main,
PQ_IP_HnonLinear_H_1920_2_SC1_Main,
PQ_IP_HnonLinear_NUMS_SC1_Main
} PQ_IP_HnonLinear_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM1_InvSinc4Tc4p4Fc45Apass01Astop40_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc50Apass01Astop55_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_SC1_Main,
PQ_IP_SRAM1_InvSinc4Tc4p4Fc65Apass3Astop60G14_SC1_Main,
PQ_IP_SRAM1_InvSinc3Tc0p0Fc75Ap001As60G13_SC1_Main,
PQ_IP_SRAM1_InvSinc3Tc0p0Fc75Ap001As60_SC1_Main,
PQ_IP_SRAM1_NUMS_SC1_Main
} PQ_IP_SRAM1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM2_InvSinc4Tc4p4Fc45Apass01Astop40_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc50Apass01Astop55_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_SC1_Main,
PQ_IP_SRAM2_InvSinc4Tc4p4Fc65Apass3Astop60G14_SC1_Main,
PQ_IP_SRAM2_InvSinc3Tc0p0Fc75Ap001As60G13_SC1_Main,
PQ_IP_SRAM2_InvSinc3Tc0p0Fc75Ap001As60_SC1_Main,
PQ_IP_SRAM2_NUMS_SC1_Main
} PQ_IP_SRAM2_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM3_OFF_SC1_Main,
PQ_IP_SRAM3_NUMS_SC1_Main
} PQ_IP_SRAM3_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM4_OFF_SC1_Main,
PQ_IP_SRAM4_NUMS_SC1_Main
} PQ_IP_SRAM4_Group_SC1_Main;

typedef enum
{
PQ_IP_C_SRAM1_C2121_SC1_Main,
PQ_IP_C_SRAM1_C121_SC1_Main,
PQ_IP_C_SRAM1_NUMS_SC1_Main
} PQ_IP_C_SRAM1_Group_SC1_Main;

typedef enum
{
PQ_IP_C_SRAM2_C2121_SC1_Main,
PQ_IP_C_SRAM2_C121_SC1_Main,
PQ_IP_C_SRAM2_NUMS_SC1_Main
} PQ_IP_C_SRAM2_Group_SC1_Main;

typedef enum
{
PQ_IP_C_SRAM3_C2121_SC1_Main,
PQ_IP_C_SRAM3_C121_SC1_Main,
PQ_IP_C_SRAM3_NUMS_SC1_Main
} PQ_IP_C_SRAM3_Group_SC1_Main;

typedef enum
{
PQ_IP_C_SRAM4_C2121_SC1_Main,
PQ_IP_C_SRAM4_C121_SC1_Main,
PQ_IP_C_SRAM4_NUMS_SC1_Main
} PQ_IP_C_SRAM4_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_OFF_SC1_Main,
PQ_IP_VIP_ON_SC1_Main,
PQ_IP_VIP_NUMS_SC1_Main
} PQ_IP_VIP_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_pseudo_OFF_SC1_Main,
PQ_IP_VIP_pseudo_ON_SC1_Main,
PQ_IP_VIP_pseudo_NUMS_SC1_Main
} PQ_IP_VIP_pseudo_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_CSC_OFF_SC1_Main,
PQ_IP_VIP_CSC_L_RGB2YCC_SD_SC1_Main,
PQ_IP_VIP_CSC_F_RGB2YCC_SD_SC1_Main,
PQ_IP_VIP_CSC_L_RGB2YCC_HD_SC1_Main,
PQ_IP_VIP_CSC_F_RGB2YCC_HD_SC1_Main,
PQ_IP_VIP_CSC_NUMS_SC1_Main
} PQ_IP_VIP_CSC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_CSC_dither_OFF_SC1_Main,
PQ_IP_VIP_CSC_dither_ON_SC1_Main,
PQ_IP_VIP_CSC_dither_NUMS_SC1_Main
} PQ_IP_VIP_CSC_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_YC_delay_OFF_SC1_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb0Cr0_SC1_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb1Cr0_SC1_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb2Cr0_SC1_Main,
PQ_IP_VIP_Post_YC_delay_Y0Cb3Cr0_SC1_Main,
PQ_IP_VIP_Post_YC_delay_Y1Cb0Cr0_SC1_Main,
PQ_IP_VIP_Post_YC_delay_NUMS_SC1_Main
} PQ_IP_VIP_Post_YC_delay_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_Y_OFF_SC1_Main,
PQ_IP_VIP_HNMR_Y_S1_SC1_Main,
PQ_IP_VIP_HNMR_Y_S2_SC1_Main,
PQ_IP_VIP_HNMR_Y_S3_SC1_Main,
PQ_IP_VIP_HNMR_Y_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_OFF_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_Yon_Con_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_Yon_Coff_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_Yoff_Con_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_ad_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_gain_S0_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S1_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S2_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_gain_S3_SC1_Main,
PQ_IP_VIP_HNMR_ad_C_gain_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_ad_C_gain_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win1_OFF_SC1_Main,
PQ_IP_VIP_HNMR_C_win1_Flesh_1_SC1_Main,
PQ_IP_VIP_HNMR_C_win1_Flesh_2_SC1_Main,
PQ_IP_VIP_HNMR_C_win1_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_C_win1_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win2_OFF_SC1_Main,
PQ_IP_VIP_HNMR_C_win2_Blue_1_SC1_Main,
PQ_IP_VIP_HNMR_C_win2_Blue_2_SC1_Main,
PQ_IP_VIP_HNMR_C_win2_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_C_win2_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win3_OFF_SC1_Main,
PQ_IP_VIP_HNMR_C_win3_Green_1_SC1_Main,
PQ_IP_VIP_HNMR_C_win3_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_C_win3_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HNMR_C_win4_OFF_SC1_Main,
PQ_IP_VIP_HNMR_C_win4_NUMS_SC1_Main
} PQ_IP_VIP_HNMR_C_win4_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Pre_Yoffset_OFF_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0x05_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0xF0_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0xF4_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0xF6_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0xF7_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_0xFA_SC1_Main,
PQ_IP_VIP_Pre_Yoffset_NUMS_SC1_Main
} PQ_IP_VIP_Pre_Yoffset_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_OFF_SC1_Main,
PQ_IP_VIP_Pre_Ygain_0x42_SC1_Main,
PQ_IP_VIP_Pre_Ygain_0x4A_SC1_Main,
PQ_IP_VIP_Pre_Ygain_NUMS_SC1_Main
} PQ_IP_VIP_Pre_Ygain_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_dither_OFF_SC1_Main,
PQ_IP_VIP_Pre_Ygain_dither_ON_SC1_Main,
PQ_IP_VIP_Pre_Ygain_dither_NUMS_SC1_Main
} PQ_IP_VIP_Pre_Ygain_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HLPF_OFF_SC1_Main,
PQ_IP_VIP_HLPF_0x1_SC1_Main,
PQ_IP_VIP_HLPF_0x2_SC1_Main,
PQ_IP_VIP_HLPF_0x3_SC1_Main,
PQ_IP_VIP_HLPF_0x4_SC1_Main,
PQ_IP_VIP_HLPF_0x5_SC1_Main,
PQ_IP_VIP_HLPF_0x6_SC1_Main,
PQ_IP_VIP_HLPF_0x7_SC1_Main,
PQ_IP_VIP_HLPF_NUMS_SC1_Main
} PQ_IP_VIP_HLPF_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_HLPF_dither_OFF_SC1_Main,
PQ_IP_VIP_HLPF_dither_ON_SC1_Main,
PQ_IP_VIP_HLPF_dither_NUMS_SC1_Main
} PQ_IP_VIP_HLPF_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Peaking_OFF_SC1_Main,
PQ_IP_VIP_Peaking_ON_SC1_Main,
PQ_IP_VIP_Peaking_NUMS_SC1_Main
} PQ_IP_VIP_Peaking_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Peaking_band_RF_NTSC_SC1_Main,
PQ_IP_VIP_Peaking_band_RF_SECAM_SC1_Main,
PQ_IP_VIP_Peaking_band_AV_NTSC_SC1_Main,
PQ_IP_VIP_Peaking_band_AV_PAL_SC1_Main,
PQ_IP_VIP_Peaking_band_AV_SECAM_SC1_Main,
PQ_IP_VIP_Peaking_band_SV_NTSC_SC1_Main,
PQ_IP_VIP_Peaking_band_480i_SC1_Main,
PQ_IP_VIP_Peaking_band_720p_SC1_Main,
PQ_IP_VIP_Peaking_band_1080i_SC1_Main,
PQ_IP_VIP_Peaking_band_H_480i_SC1_Main,
PQ_IP_VIP_Peaking_band_H_576i_SC1_Main,
PQ_IP_VIP_Peaking_band_H_720p_SC1_Main,
PQ_IP_VIP_Peaking_band_H_1080i_SC1_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_480is_SC1_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_720p_SC1_Main,
PQ_IP_VIP_Peaking_band_DT_MPEG2_1080i_SC1_Main,
PQ_IP_VIP_Peaking_band_DT_H264_480is_SC1_Main,
PQ_IP_VIP_Peaking_band_DT_H264_720p_SC1_Main,
PQ_IP_VIP_Peaking_band_PC_mode_SC1_Main,
PQ_IP_VIP_Peaking_band_NUMS_SC1_Main
} PQ_IP_VIP_Peaking_band_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Peaking_Pcoring_S00_0_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S21_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S30_0_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S30_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S31_0_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S31_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S20_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S32_0_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S32_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_S42_1_SC1_Main,
PQ_IP_VIP_Peaking_Pcoring_NUMS_SC1_Main
} PQ_IP_VIP_Peaking_Pcoring_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Peaking_gain_0x18_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x1C_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x20_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x24_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x28_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x2C_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x30_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x34_SC1_Main,
PQ_IP_VIP_Peaking_gain_0x38_SC1_Main,
PQ_IP_VIP_Peaking_gain_NUMS_SC1_Main
} PQ_IP_VIP_Peaking_gain_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Peaking_gain_ad_C_OFF_SC1_Main,
PQ_IP_VIP_Peaking_gain_ad_C_ON_SC1_Main,
PQ_IP_VIP_Peaking_gain_ad_C_NUMS_SC1_Main
} PQ_IP_VIP_Peaking_gain_ad_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_CTI_OFF_SC1_Main,
PQ_IP_VIP_Post_CTI_S0L0_SC1_Main,
PQ_IP_VIP_Post_CTI_S3L3_SC1_Main,
PQ_IP_VIP_Post_CTI_S1L6_SC1_Main,
PQ_IP_VIP_Post_CTI_S1L0_SC1_Main,
PQ_IP_VIP_Post_CTI_S2L3_SC1_Main,
PQ_IP_VIP_Post_CTI_S2L4_SC1_Main,
PQ_IP_VIP_Post_CTI_S2L5_SC1_Main,
PQ_IP_VIP_Post_CTI_S2L6_SC1_Main,
PQ_IP_VIP_Post_CTI_S2L7_SC1_Main,
PQ_IP_VIP_Post_CTI_NUMS_SC1_Main
} PQ_IP_VIP_Post_CTI_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_CTI_coef_0x18_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_0x1C_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_0x20_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_0x24_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_0x28_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_0x04_SC1_Main,
PQ_IP_VIP_Post_CTI_coef_NUMS_SC1_Main
} PQ_IP_VIP_Post_CTI_coef_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T1_OFF_SC1_Main,
PQ_IP_VIP_FCC_T1_S4_SC1_Main,
PQ_IP_VIP_FCC_T1_mts_SC1_Main,
PQ_IP_VIP_FCC_T1_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T1_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T1_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T2_OFF_SC1_Main,
PQ_IP_VIP_FCC_T2_S8_SC1_Main,
PQ_IP_VIP_FCC_T2_mts_SC1_Main,
PQ_IP_VIP_FCC_T2_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T2_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T2_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T3_OFF_SC1_Main,
PQ_IP_VIP_FCC_T3_S4_SC1_Main,
PQ_IP_VIP_FCC_T3_mts_SC1_Main,
PQ_IP_VIP_FCC_T3_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T3_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T3_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T4_OFF_SC1_Main,
PQ_IP_VIP_FCC_T4_S4_SC1_Main,
PQ_IP_VIP_FCC_T4_mts_SC1_Main,
PQ_IP_VIP_FCC_T4_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T4_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T4_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T5_OFF_SC1_Main,
PQ_IP_VIP_FCC_T5_S6_SC1_Main,
PQ_IP_VIP_FCC_T5_S8_SC1_Main,
PQ_IP_VIP_FCC_T5_mts_SC1_Main,
PQ_IP_VIP_FCC_T5_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T5_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T5_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T6_OFF_SC1_Main,
PQ_IP_VIP_FCC_T6_S6_SC1_Main,
PQ_IP_VIP_FCC_T6_S8_SC1_Main,
PQ_IP_VIP_FCC_T6_mts_SC1_Main,
PQ_IP_VIP_FCC_T6_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T6_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T6_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T7_OFF_SC1_Main,
PQ_IP_VIP_FCC_T7_S8_SC1_Main,
PQ_IP_VIP_FCC_T7_mts_SC1_Main,
PQ_IP_VIP_FCC_T7_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T7_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T7_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T8_OFF_SC1_Main,
PQ_IP_VIP_FCC_T8_S5_SC1_Main,
PQ_IP_VIP_FCC_T8_mts_SC1_Main,
PQ_IP_VIP_FCC_T8_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T8_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T8_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_FCC_T9_OFF_SC1_Main,
PQ_IP_VIP_FCC_T9_S5_SC1_Main,
PQ_IP_VIP_FCC_T9_mts_SC1_Main,
PQ_IP_VIP_FCC_T9_mts_1080_SC1_Main,
PQ_IP_VIP_FCC_T9_NUMS_SC1_Main
} PQ_IP_VIP_FCC_T9_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_OFF_SC1_Main,
PQ_IP_VIP_IHC_ON_SC1_Main,
PQ_IP_VIP_IHC_NUMS_SC1_Main
} PQ_IP_VIP_IHC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_R_OFF_SC1_Main,
PQ_IP_VIP_IHC_R_N_20_SC1_Main,
PQ_IP_VIP_IHC_R_P_20_SC1_Main,
PQ_IP_VIP_IHC_R_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_R_NUMS_SC1_Main
} PQ_IP_VIP_IHC_R_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_G_OFF_SC1_Main,
PQ_IP_VIP_IHC_G_N_30_SC1_Main,
PQ_IP_VIP_IHC_G_N_10_SC1_Main,
PQ_IP_VIP_IHC_G_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_G_NUMS_SC1_Main
} PQ_IP_VIP_IHC_G_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_B_OFF_SC1_Main,
PQ_IP_VIP_IHC_B_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_B_NUMS_SC1_Main
} PQ_IP_VIP_IHC_B_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_C_OFF_SC1_Main,
PQ_IP_VIP_IHC_C_N_3F_SC1_Main,
PQ_IP_VIP_IHC_C_N_30_SC1_Main,
PQ_IP_VIP_IHC_C_N_10_SC1_Main,
PQ_IP_VIP_IHC_C_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_C_mts_1080_SC1_Main,
PQ_IP_VIP_IHC_C_NUMS_SC1_Main
} PQ_IP_VIP_IHC_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_M_OFF_SC1_Main,
PQ_IP_VIP_IHC_M_P_10_SC1_Main,
PQ_IP_VIP_IHC_M_NUMS_SC1_Main
} PQ_IP_VIP_IHC_M_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_Y_OFF_SC1_Main,
PQ_IP_VIP_IHC_Y_P_8_SC1_Main,
PQ_IP_VIP_IHC_Y_P_10_SC1_Main,
PQ_IP_VIP_IHC_Y_P_15_SC1_Main,
PQ_IP_VIP_IHC_Y_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_Y_mts_1080_SC1_Main,
PQ_IP_VIP_IHC_Y_NUMS_SC1_Main
} PQ_IP_VIP_IHC_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IHC_F_OFF_SC1_Main,
PQ_IP_VIP_IHC_F_N_10_SC1_Main,
PQ_IP_VIP_IHC_F_P_10_SC1_Main,
PQ_IP_VIP_IHC_F_mts_AV_SC1_Main,
PQ_IP_VIP_IHC_F_NUMS_SC1_Main
} PQ_IP_VIP_IHC_F_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_OFF_SC1_Main,
PQ_IP_VIP_ICC_ON_SC1_Main,
PQ_IP_VIP_ICC_NUMS_SC1_Main
} PQ_IP_VIP_ICC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_R_OFF_SC1_Main,
PQ_IP_VIP_ICC_R_INC_8_SC1_Main,
PQ_IP_VIP_ICC_R_INC_3_SC1_Main,
PQ_IP_VIP_ICC_R_INC_B_SC1_Main,
PQ_IP_VIP_ICC_R_DEC_3_SC1_Main,
PQ_IP_VIP_ICC_R_NUMS_SC1_Main
} PQ_IP_VIP_ICC_R_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_G_OFF_SC1_Main,
PQ_IP_VIP_ICC_G_INC_8_SC1_Main,
PQ_IP_VIP_ICC_G_INC_6_SC1_Main,
PQ_IP_VIP_ICC_G_INC_B_SC1_Main,
PQ_IP_VIP_ICC_G_NUMS_SC1_Main
} PQ_IP_VIP_ICC_G_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_B_OFF_SC1_Main,
PQ_IP_VIP_ICC_B_INC_A_SC1_Main,
PQ_IP_VIP_ICC_B_INC_B_SC1_Main,
PQ_IP_VIP_ICC_B_INC_9_SC1_Main,
PQ_IP_VIP_ICC_B_INC_6_SC1_Main,
PQ_IP_VIP_ICC_B_INC_5_SC1_Main,
PQ_IP_VIP_ICC_B_NUMS_SC1_Main
} PQ_IP_VIP_ICC_B_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_C_OFF_SC1_Main,
PQ_IP_VIP_ICC_C_INC_6_SC1_Main,
PQ_IP_VIP_ICC_C_INC_9_SC1_Main,
PQ_IP_VIP_ICC_C_INC_A_SC1_Main,
PQ_IP_VIP_ICC_C_INC_B_SC1_Main,
PQ_IP_VIP_ICC_C_INC_4_SC1_Main,
PQ_IP_VIP_ICC_C_NUMS_SC1_Main
} PQ_IP_VIP_ICC_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_M_OFF_SC1_Main,
PQ_IP_VIP_ICC_M_INC_6_SC1_Main,
PQ_IP_VIP_ICC_M_INC_9_SC1_Main,
PQ_IP_VIP_ICC_M_INC_A_SC1_Main,
PQ_IP_VIP_ICC_M_NUMS_SC1_Main
} PQ_IP_VIP_ICC_M_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_Y_OFF_SC1_Main,
PQ_IP_VIP_ICC_Y_INC_7_SC1_Main,
PQ_IP_VIP_ICC_Y_INC_8_SC1_Main,
PQ_IP_VIP_ICC_Y_INC_A_SC1_Main,
PQ_IP_VIP_ICC_Y_NUMS_SC1_Main
} PQ_IP_VIP_ICC_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_F_OFF_SC1_Main,
PQ_IP_VIP_ICC_F_INC_2_SC1_Main,
PQ_IP_VIP_ICC_F_INC_3_SC1_Main,
PQ_IP_VIP_ICC_F_INC_A_SC1_Main,
PQ_IP_VIP_ICC_F_NUMS_SC1_Main
} PQ_IP_VIP_ICC_F_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_ICC_NC_OFF_SC1_Main,
PQ_IP_VIP_ICC_NC_INC_2_SC1_Main,
PQ_IP_VIP_ICC_NC_INC_3_SC1_Main,
PQ_IP_VIP_ICC_NC_INC_8_SC1_Main,
PQ_IP_VIP_ICC_NC_INC_A_SC1_Main,
PQ_IP_VIP_ICC_NC_NUMS_SC1_Main
} PQ_IP_VIP_ICC_NC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_OFF_SC1_Main,
PQ_IP_VIP_IBC_ON_SC1_Main,
PQ_IP_VIP_IBC_NUMS_SC1_Main
} PQ_IP_VIP_IBC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_R_OFF_SC1_Main,
PQ_IP_VIP_IBC_R_DEC_10_SC1_Main,
PQ_IP_VIP_IBC_R_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_R_NUMS_SC1_Main
} PQ_IP_VIP_IBC_R_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_G_OFF_SC1_Main,
PQ_IP_VIP_IBC_G_DEC_10_SC1_Main,
PQ_IP_VIP_IBC_G_DEC_1C_SC1_Main,
PQ_IP_VIP_IBC_G_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_G_NUMS_SC1_Main
} PQ_IP_VIP_IBC_G_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_B_OFF_SC1_Main,
PQ_IP_VIP_IBC_B_DEC_10_SC1_Main,
PQ_IP_VIP_IBC_B_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_B_NUMS_SC1_Main
} PQ_IP_VIP_IBC_B_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_C_OFF_SC1_Main,
PQ_IP_VIP_IBC_C_DEC_10_SC1_Main,
PQ_IP_VIP_IBC_C_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_C_INC_22_SC1_Main,
PQ_IP_VIP_IBC_C_NUMS_SC1_Main
} PQ_IP_VIP_IBC_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_M_OFF_SC1_Main,
PQ_IP_VIP_IBC_M_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_M_NUMS_SC1_Main
} PQ_IP_VIP_IBC_M_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_Y_OFF_SC1_Main,
PQ_IP_VIP_IBC_Y_DEC_18_SC1_Main,
PQ_IP_VIP_IBC_Y_NUMS_SC1_Main
} PQ_IP_VIP_IBC_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_IBC_F_OFF_SC1_Main,
PQ_IP_VIP_IBC_F_INC_28_SC1_Main,
PQ_IP_VIP_IBC_F_NUMS_SC1_Main
} PQ_IP_VIP_IBC_F_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_OFF_SC1_Main,
PQ_IP_VIP_DLC_ON_SC1_Main,
PQ_IP_VIP_DLC_NUMS_SC1_Main
} PQ_IP_VIP_DLC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_dither_OFF_SC1_Main,
PQ_IP_VIP_DLC_dither_ON_SC1_Main,
PQ_IP_VIP_DLC_dither_NUMS_SC1_Main
} PQ_IP_VIP_DLC_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_Flesh_adj_OFF_SC1_Main,
PQ_IP_VIP_DLC_Flesh_adj_S1_SC1_Main,
PQ_IP_VIP_DLC_Flesh_adj_NUMS_SC1_Main
} PQ_IP_VIP_DLC_Flesh_adj_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_range_OFF_SC1_Main,
PQ_IP_VIP_DLC_His_range_ON_SC1_Main,
PQ_IP_VIP_DLC_His_range_NUMS_SC1_Main
} PQ_IP_VIP_DLC_His_range_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeH_90pa_1366_SC1_Main,
PQ_IP_VIP_DLC_His_rangeH_90pa_1920_SC1_Main,
PQ_IP_VIP_DLC_His_rangeH_NUMS_SC1_Main
} PQ_IP_VIP_DLC_His_rangeH_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeV_90pa_1366_SC1_Main,
PQ_IP_VIP_DLC_His_rangeV_90pa_1920_SC1_Main,
PQ_IP_VIP_DLC_His_rangeV_NUMS_SC1_Main
} PQ_IP_VIP_DLC_His_rangeV_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_BLE_OFF_SC1_Main,
PQ_IP_VIP_BLE_0x82_0x40_SC1_Main,
PQ_IP_VIP_BLE_0x82_0x50_SC1_Main,
PQ_IP_VIP_BLE_0x82_0x60_SC1_Main,
PQ_IP_VIP_BLE_0x84_0x40_SC1_Main,
PQ_IP_VIP_BLE_0x86_0x40_SC1_Main,
PQ_IP_VIP_BLE_0x88_0x40_SC1_Main,
PQ_IP_VIP_BLE_0x88_0x60_SC1_Main,
PQ_IP_VIP_BLE_0x8A_0x40_SC1_Main,
PQ_IP_VIP_BLE_NUMS_SC1_Main
} PQ_IP_VIP_BLE_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_WLE_OFF_SC1_Main,
PQ_IP_VIP_WLE_0x78_0x20_SC1_Main,
PQ_IP_VIP_WLE_NUMS_SC1_Main
} PQ_IP_VIP_WLE_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_BWLE_dither_OFF_SC1_Main,
PQ_IP_VIP_BWLE_dither_ON_SC1_Main,
PQ_IP_VIP_BWLE_dither_NUMS_SC1_Main
} PQ_IP_VIP_BWLE_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_UVC_OFF_SC1_Main,
PQ_IP_VIP_UVC_ON_SC1_Main,
PQ_IP_VIP_UVC_NUMS_SC1_Main
} PQ_IP_VIP_UVC_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_Yoffset_OFF_SC1_Main,
PQ_IP_VIP_Post_Yoffset_0x05_SC1_Main,
PQ_IP_VIP_Post_Yoffset_NUMS_SC1_Main
} PQ_IP_VIP_Post_Yoffset_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_Ygain_OFF_SC1_Main,
PQ_IP_VIP_Post_Ygain_0x3C_SC1_Main,
PQ_IP_VIP_Post_Ygain_0x36_SC1_Main,
PQ_IP_VIP_Post_Ygain_NUMS_SC1_Main
} PQ_IP_VIP_Post_Ygain_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Post_Cgain_OFF_SC1_Main,
PQ_IP_VIP_Post_Cgain_0x44_SC1_Main,
PQ_IP_VIP_Post_Cgain_NUMS_SC1_Main
} PQ_IP_VIP_Post_Cgain_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_Y_OFF_SC1_Main,
PQ_IP_VIP_Hcoring_Y_C4_SC1_Main,
PQ_IP_VIP_Hcoring_Y_C3_SC1_Main,
PQ_IP_VIP_Hcoring_Y_C2_SC1_Main,
PQ_IP_VIP_Hcoring_Y_C1_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P6_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P5_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P4_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P3_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P2_SC1_Main,
PQ_IP_VIP_Hcoring_Y_P1_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P6_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P5_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P4_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P3_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P2_SC1_Main,
PQ_IP_VIP_Hcoring_Y_PC_P1_SC1_Main,
PQ_IP_VIP_Hcoring_Y_NUMS_SC1_Main
} PQ_IP_VIP_Hcoring_Y_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_C_OFF_SC1_Main,
PQ_IP_VIP_Hcoring_C_C4_SC1_Main,
PQ_IP_VIP_Hcoring_C_C3_SC1_Main,
PQ_IP_VIP_Hcoring_C_C2_SC1_Main,
PQ_IP_VIP_Hcoring_C_C1_SC1_Main,
PQ_IP_VIP_Hcoring_C_P5_SC1_Main,
PQ_IP_VIP_Hcoring_C_P4_SC1_Main,
PQ_IP_VIP_Hcoring_C_P3_SC1_Main,
PQ_IP_VIP_Hcoring_C_P2_SC1_Main,
PQ_IP_VIP_Hcoring_C_P1_SC1_Main,
PQ_IP_VIP_Hcoring_C_NUMS_SC1_Main
} PQ_IP_VIP_Hcoring_C_Group_SC1_Main;

typedef enum
{
PQ_IP_VIP_Hcoring_dither_OFF_SC1_Main,
PQ_IP_VIP_Hcoring_dither_ON_SC1_Main,
PQ_IP_VIP_Hcoring_dither_NUMS_SC1_Main
} PQ_IP_VIP_Hcoring_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_SwDriver_OFF_SC1_Main,
PQ_IP_SwDriver_ALL_SC1_Main,
PQ_IP_SwDriver_SD_HD_SC1_Main,
PQ_IP_SwDriver_HD_ALL_SC1_Main,
PQ_IP_SwDriver_NUMS_SC1_Main
} PQ_IP_SwDriver_Group_SC1_Main;

typedef enum
{
PQ_IP_3x3_OFF_SC1_Main,
PQ_IP_3x3_SD_SC1_Main,
PQ_IP_3x3_HD_SC1_Main,
PQ_IP_3x3_NUMS_SC1_Main
} PQ_IP_3x3_Group_SC1_Main;

typedef enum
{
PQ_IP_RGB_Offset_OFF_SC1_Main,
PQ_IP_RGB_Offset_NUMS_SC1_Main
} PQ_IP_RGB_Offset_Group_SC1_Main;

typedef enum
{
PQ_IP_RGB_Clip_OFF_SC1_Main,
PQ_IP_RGB_Clip_NUMS_SC1_Main
} PQ_IP_RGB_Clip_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_bypass_OFF_SC1_Main,
PQ_IP_xvYCC_bypass_ON_SC1_Main,
PQ_IP_xvYCC_bypass_NUMS_SC1_Main
} PQ_IP_xvYCC_bypass_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_de_gamma_OFF_SC1_Main,
PQ_IP_xvYCC_de_gamma_ON_SC1_Main,
PQ_IP_xvYCC_de_gamma_NUMS_SC1_Main
} PQ_IP_xvYCC_de_gamma_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_de_gamma_dither_OFF_SC1_Main,
PQ_IP_xvYCC_de_gamma_dither_ON_SC1_Main,
PQ_IP_xvYCC_de_gamma_dither_NUMS_SC1_Main
} PQ_IP_xvYCC_de_gamma_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_3x3_OFF_SC1_Main,
PQ_IP_xvYCC_3x3_NUMS_SC1_Main
} PQ_IP_xvYCC_3x3_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_Comp_OFF_SC1_Main,
PQ_IP_xvYCC_Comp_NUMS_SC1_Main
} PQ_IP_xvYCC_Comp_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_Comp_dither_OFF_SC1_Main,
PQ_IP_xvYCC_Comp_dither_ON_SC1_Main,
PQ_IP_xvYCC_Comp_dither_NUMS_SC1_Main
} PQ_IP_xvYCC_Comp_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_Clip_OFF_SC1_Main,
PQ_IP_xvYCC_Clip_NUMS_SC1_Main
} PQ_IP_xvYCC_Clip_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_gamma_OFF_SC1_Main,
PQ_IP_xvYCC_gamma_ON_SC1_Main,
PQ_IP_xvYCC_gamma_NUMS_SC1_Main
} PQ_IP_xvYCC_gamma_Group_SC1_Main;

typedef enum
{
PQ_IP_xvYCC_gamma_dither_OFF_SC1_Main,
PQ_IP_xvYCC_gamma_dither_ON_SC1_Main,
PQ_IP_xvYCC_gamma_dither_NUMS_SC1_Main
} PQ_IP_xvYCC_gamma_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_rgb_3d_OFF_SC1_Main,
PQ_IP_rgb_3d_ON_SC1_Main,
PQ_IP_rgb_3d_NUMS_SC1_Main
} PQ_IP_rgb_3d_Group_SC1_Main;

typedef enum
{
PQ_IP_HBC_OFF_SC1_Main,
PQ_IP_HBC_HBC1_SC1_Main,
PQ_IP_HBC_NUMS_SC1_Main
} PQ_IP_HBC_Group_SC1_Main;

typedef enum
{
PQ_IP_Pre_CON_BRI_OFF_SC1_Main,
PQ_IP_Pre_CON_BRI_NUMS_SC1_Main
} PQ_IP_Pre_CON_BRI_Group_SC1_Main;

typedef enum
{
PQ_IP_Blue_Stretch_OFF_SC1_Main,
PQ_IP_Blue_Stretch_BS1_SC1_Main,
PQ_IP_Blue_Stretch_NUMS_SC1_Main
} PQ_IP_Blue_Stretch_Group_SC1_Main;

typedef enum
{
PQ_IP_Blue_Stretch_dither_OFF_SC1_Main,
PQ_IP_Blue_Stretch_dither_ON_SC1_Main,
PQ_IP_Blue_Stretch_dither_NUMS_SC1_Main
} PQ_IP_Blue_Stretch_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_Gamma_OFF_SC1_Main,
PQ_IP_Gamma_ON_SC1_Main,
PQ_IP_Gamma_NUMS_SC1_Main
} PQ_IP_Gamma_Group_SC1_Main;

typedef enum
{
PQ_IP_Gamma_dither_OFF_SC1_Main,
PQ_IP_Gamma_dither_ON_SC1_Main,
PQ_IP_Gamma_dither_NUMS_SC1_Main
} PQ_IP_Gamma_dither_Group_SC1_Main;

typedef enum
{
PQ_IP_Post_CON_BRI_OFF_SC1_Main,
PQ_IP_Post_CON_BRI_NUMS_SC1_Main
} PQ_IP_Post_CON_BRI_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_3x3matrix_PIC2_Build_IN_SC1_Main,
PQ_IP_SRAM_3x3matrix_PIC2_NUMS_SC1_Main
} PQ_IP_SRAM_3x3matrix_PIC2_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_DLC_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_DLC_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_DLC_PIC1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_R_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_GammaTbl_R_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_GammaTbl_R_PIC1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_G_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_GammaTbl_G_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_GammaTbl_G_PIC1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_GammaTbl_B_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_GammaTbl_B_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_GammaTbl_B_PIC1_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_Auto_Color_PIC2_Build_IN_SC1_Main,
PQ_IP_SRAM_Auto_Color_PIC2_NUMS_SC1_Main
} PQ_IP_SRAM_Auto_Color_PIC2_Group_SC1_Main;

typedef enum
{
PQ_IP_SRAM_Color_Temp_PIC1_Build_IN_SC1_Main,
PQ_IP_SRAM_Color_Temp_PIC1_NUMS_SC1_Main
} PQ_IP_SRAM_Color_Temp_PIC1_Group_SC1_Main;

extern code U8 MST_SkipRule_IP_SC1_Main[PQ_IP_NUM_SC1_Main];
extern code EN_IPTAB_INFO PQ_IPTAB_INFO_SC1_Main[];
extern code U8 QMAP_720_SC1_Main[QM_INPUTTYPE_NUM_SC1_Main][PQ_IP_NUM_SC1_Main];

#endif
