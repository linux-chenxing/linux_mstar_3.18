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
// 1/7/2013  8:51:35 PM
// 39630.43853
//****************************************************

#ifndef _CEDRIC_SC2_SUB_H_
#define _CEDRIC_SC2_SUB_H_

#define PQ_IP_NUM_SC2_Sub 196
#define PQ_IP_SRAM_COLOR_INDEX_SIZE_SC2_Sub 1
#define PQ_IP_SRAM_COLOR_GAIN_SNR_SIZE_SC2_Sub 1
#define PQ_IP_SRAM_COLOR_GAIN_DNR_SIZE_SC2_Sub 1
#define PQ_IP_SRAM1_SIZE_SC2_Sub 640
#define PQ_IP_SRAM2_SIZE_SC2_Sub 640
#define PQ_IP_SRAM3_SIZE_SC2_Sub 320
#define PQ_IP_SRAM4_SIZE_SC2_Sub 320
#define PQ_IP_C_SRAM1_SIZE_SC2_Sub 320
#define PQ_IP_C_SRAM2_SIZE_SC2_Sub 320
#define PQ_IP_C_SRAM3_SIZE_SC2_Sub 320
#define PQ_IP_C_SRAM4_SIZE_SC2_Sub 320
#define PQ_IP_VIP_IHC_CRD_SRAM_SIZE_SC2_Sub 1
#define PQ_IP_VIP_ICC_CRD_SRAM_SIZE_SC2_Sub 1
#define PQ_IP_SRAM_3x3matrix_PIC2_SIZE_SC2_Sub 32
#define PQ_IP_SRAM_DLC_PIC1_SIZE_SC2_Sub 65
#define PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SIZE_SC2_Sub 5
#define PQ_IP_SRAM_GammaTbl_R_PIC1_SIZE_SC2_Sub 386
#define PQ_IP_SRAM_GammaTbl_G_PIC1_SIZE_SC2_Sub 386
#define PQ_IP_SRAM_GammaTbl_B_PIC1_SIZE_SC2_Sub 386
#define PQ_IP_SRAM_Auto_Color_PIC2_SIZE_SC2_Sub 6
#define PQ_IP_SRAM_Color_Temp_PIC1_SIZE_SC2_Sub 9

typedef enum
{
    #if PQ_QM_CVBS
    QM_RF_NTSC_44_SC2_Sub, //0
    #endif
    #if PQ_QM_CVBS
    QM_RF_NTSC_M_SC2_Sub, //1
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_BGHI_SC2_Sub, //2
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_60_SC2_Sub, //3
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_M_SC2_Sub, //4
    #endif
    #if PQ_QM_CVBS
    QM_RF_PAL_N_SC2_Sub, //5
    #endif
    #if PQ_QM_CVBS
    QM_RF_SECAM_SC2_Sub, //6
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_44_SC2_Sub, //7
    #endif
    #if PQ_QM_CVBS
    QM_VIF_NTSC_M_SC2_Sub, //8
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_BGHI_SC2_Sub, //9
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_60_SC2_Sub, //10
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_M_SC2_Sub, //11
    #endif
    #if PQ_QM_CVBS
    QM_VIF_PAL_N_SC2_Sub, //12
    #endif
    #if PQ_QM_CVBS
    QM_VIF_SECAM_SC2_Sub, //13
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_44_SC2_Sub, //14
    #endif
    #if PQ_QM_CVBS
    QM_SV_NTSC_M_SC2_Sub, //15
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_BGHI_SC2_Sub, //16
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_60_SC2_Sub, //17
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_M_SC2_Sub, //18
    #endif
    #if PQ_QM_CVBS
    QM_SV_PAL_N_SC2_Sub, //19
    #endif
    #if PQ_QM_CVBS
    QM_SV_SECAM_SC2_Sub, //20
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_44_SC2_Sub, //21
    #endif
    #if PQ_QM_CVBS
    QM_AV_NTSC_M_SC2_Sub, //22
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_BGHI_SC2_Sub, //23
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_60_SC2_Sub, //24
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_M_SC2_Sub, //25
    #endif
    #if PQ_QM_CVBS
    QM_AV_PAL_N_SC2_Sub, //26
    #endif
    #if PQ_QM_CVBS
    QM_AV_SECAM_SC2_Sub, //27
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_44_SC2_Sub, //28
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_NTSC_M_SC2_Sub, //29
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_BGHI_SC2_Sub, //30
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_60_SC2_Sub, //31
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_M_SC2_Sub, //32
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_PAL_N_SC2_Sub, //33
    #endif
    #if PQ_QM_CVBS
    QM_SCART_AV_SECAM_SC2_Sub, //34
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_44_SC2_Sub, //35
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_NTSC_M_SC2_Sub, //36
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_BGHI_SC2_Sub, //37
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_60_SC2_Sub, //38
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_M_SC2_Sub, //39
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_PAL_N_SC2_Sub, //40
    #endif
    #if PQ_QM_CVBS
    QM_SCART_SV_SECAM_SC2_Sub, //41
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_NTSC_SC2_Sub, //42
    #endif
    #if PQ_QM_CVBS
    QM_SCART_RGB_PAL_SC2_Sub, //43
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480i_SC2_Sub, //44
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576i_SC2_Sub, //45
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_480p_SC2_Sub, //46
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_576p_SC2_Sub, //47
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_24hz_SC2_Sub, //48
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_50hz_SC2_Sub, //49
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_720p_60hz_SC2_Sub, //50
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_50hz_SC2_Sub, //51
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080i_60hz_SC2_Sub, //52
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_24hz_SC2_Sub, //53
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_50hz_SC2_Sub, //54
    #endif
    #if PQ_QM_YPBPR
    QM_YPbPr_1080p_60hz_SC2_Sub, //55
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480i_SC2_Sub, //56
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576i_SC2_Sub, //57
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_480p_SC2_Sub, //58
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_576p_SC2_Sub, //59
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_24hz_SC2_Sub, //60
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_50hz_SC2_Sub, //61
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_720p_60hz_SC2_Sub, //62
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_50hz_SC2_Sub, //63
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080i_60hz_SC2_Sub, //64
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_24hz_SC2_Sub, //65
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_60hz_SC2_Sub, //66
    #endif
    #if PQ_QM_HMDI
    QM_HDMI_1080p_50hz_SC2_Sub, //67
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vup_SC2_Sub, //68
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vdown_SC2_Sub, //69
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hup_Vno_SC2_Sub, //70
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vup_SC2_Sub, //71
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vdown_SC2_Sub, //72
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hdown_Vno_SC2_Sub, //73
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vup_SC2_Sub, //74
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vdown_SC2_Sub, //75
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_444_PC_Hno_Vno_SC2_Sub, //76
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vup_SC2_Sub, //77
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vdown_SC2_Sub, //78
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hup_Vno_SC2_Sub, //79
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vup_SC2_Sub, //80
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vdown_SC2_Sub, //81
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hdown_Vno_SC2_Sub, //82
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vup_SC2_Sub, //83
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vdown_SC2_Sub, //84
    #endif
    #if PQ_QM_HDMI_PC
    QM_HDMI_422_PC_Hno_Vno_SC2_Sub, //85
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vup_SC2_Sub, //86
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vdown_SC2_Sub, //87
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hup_Vno_SC2_Sub, //88
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vup_SC2_Sub, //89
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vdown_SC2_Sub, //90
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hdown_Vno_SC2_Sub, //91
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vup_SC2_Sub, //92
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vdown_SC2_Sub, //93
    #endif
    #if PQ_QM_PC
    QM_DVI_Dsub_HDMI_RGB_PC_Hno_Vno_SC2_Sub, //94
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_MPEG2_SC2_Sub, //95
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_MPEG2_SC2_Sub, //96
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_MPEG2_SC2_Sub, //97
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_MPEG2_SC2_Sub, //98
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_MPEG2_SC2_Sub, //99
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_MPEG2_SC2_Sub, //100
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_MPEG2_SC2_Sub, //101
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_MPEG2_SC2_Sub, //102
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_MPEG2_SC2_Sub, //103
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_MPEG2_SC2_Sub, //104
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_MPEG2_SC2_Sub, //105
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_MPEG2_SC2_Sub, //106
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_MPEG2_SC2_Sub, //107
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_352x480_H264_SC2_Sub, //108
    #endif
    #if PQ_QM_DTV
    QM_DTV_480i_H264_SC2_Sub, //109
    #endif
    #if PQ_QM_DTV
    QM_DTV_576i_H264_SC2_Sub, //110
    #endif
    #if PQ_QM_DTV
    QM_DTV_480p_H264_SC2_Sub, //111
    #endif
    #if PQ_QM_DTV
    QM_DTV_576p_H264_SC2_Sub, //112
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_24hz_H264_SC2_Sub, //113
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_50hz_H264_SC2_Sub, //114
    #endif
    #if PQ_QM_DTV
    QM_DTV_720p_60hz_H264_SC2_Sub, //115
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_50hz_H264_SC2_Sub, //116
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080i_60hz_H264_SC2_Sub, //117
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_24hz_H264_SC2_Sub, //118
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_50hz_H264_SC2_Sub, //119
    #endif
    #if PQ_QM_DTV
    QM_DTV_1080p_60hz_H264_SC2_Sub, //120
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_interlace_SC2_Sub, //121
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_SD_progressive_SC2_Sub, //122
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_interlace_SC2_Sub, //123
    #endif
    #if PQ_QM_MM_VIDEO
    QM_Multimedia_video_HD_progressive_SC2_Sub, //124
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_SD_progressive_SC2_Sub, //125
    #endif
    #if PQ_QM_MM_PHOTO
    QM_Multimedia_photo_HD_progressive_SC2_Sub, //126
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_interlace_SC2_Sub, //127
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_SD_progressive_SC2_Sub, //128
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_interlace_SC2_Sub, //129
    #endif
    #if PQ_QM_DTV
    QM_DTV_iFrame_HD_progressive_SC2_Sub, //130
    #endif
    #if PQ_QM_3D
    QM_3D_Video_interlace_SC2_Sub, //131
    #endif
    #if PQ_QM_3D
    QM_3D_Video_progressive_SC2_Sub, //132
    #endif
    QM_INPUTTYPE_NUM_SC2_Sub, // 133
} SC_QUALITY_MAP_INDEX_e_SC2_Sub;

typedef enum
{
PQ_IP_AFEC_SC2_Sub,  //0
PQ_IP_Comb_SC2_Sub,  //1
PQ_IP_Comb2_SC2_Sub,  //2
PQ_IP_SECAM_SC2_Sub,  //3
PQ_IP_VD_Sampling_no_comm_SC2_Sub,  //4
PQ_IP_SCinit_SC2_Sub,  //5
PQ_IP_CSC_SC2_Sub,  //6
PQ_IP_CSC_Dither_SC2_Sub,  //7
PQ_IP_YCdelay_SC2_Sub,  //8
PQ_IP_PreFilter_SC2_Sub,  //9
PQ_IP_PreFilter_Dither_SC2_Sub,  //10
PQ_IP_HDSDD_SC2_Sub,  //11
PQ_IP_HSD_Y_SC2_Sub,  //12
PQ_IP_HSD_C_SC2_Sub,  //13
PQ_IP_444To422_SC2_Sub,  //14
PQ_IP_VSD_SC2_Sub,  //15
PQ_IP_HVSD_Dither_SC2_Sub,  //16
PQ_IP_10to8_Dither_SC2_Sub,  //17
PQ_IP_MemFormat_SC2_Sub,  //18
PQ_IP_PreSNR_SC2_Sub,  //19
PQ_IP_PreSNR_Patch_SC2_Sub,  //20
PQ_IP_DNR_SC2_Sub,  //21
PQ_IP_DNR_Motion_SC2_Sub,  //22
PQ_IP_DNR_Y_SC2_Sub,  //23
PQ_IP_DNR_Y_COLOR_DEP_SC2_Sub,  //24
PQ_IP_SRAM_COLOR_INDEX_SC2_Sub,  //25
PQ_IP_SRAM_COLOR_GAIN_SNR_SC2_Sub,  //26
PQ_IP_SRAM_COLOR_GAIN_DNR_SC2_Sub,  //27
PQ_IP_DNR_Y_LUMA_ADAPTIVE_SC2_Sub,  //28
PQ_IP_DNR_POSTTUNE_SC2_Sub,  //29
PQ_IP_DNR_C_SC2_Sub,  //30
PQ_IP_HISDNR_SC2_Sub,  //31
PQ_IP_PNR_SC2_Sub,  //32
PQ_IP_PNR_Y_SC2_Sub,  //33
PQ_IP_PNR_C_SC2_Sub,  //34
PQ_IP_PostCCS_SC2_Sub,  //35
PQ_IP_PostCCS_Smooth_SC2_Sub,  //36
PQ_IP_420CUP_SC2_Sub,  //37
PQ_IP_MADi_SC2_Sub,  //38
PQ_IP_MADi_Motion_SC2_Sub,  //39
PQ_IP_MADi_ADP3x3_SC2_Sub,  //40
PQ_IP_MADi_MORPHO_SC2_Sub,  //41
PQ_IP_MADi_DFK_SC2_Sub,  //42
PQ_IP_MADi_SST_SC2_Sub,  //43
PQ_IP_MADi_EODiW_SC2_Sub,  //44
PQ_IP_MADi_Force_SC2_Sub,  //45
PQ_IP_EODi_SC2_Sub,  //46
PQ_IP_Film_SC2_Sub,  //47
PQ_IP_Film32_SC2_Sub,  //48
PQ_IP_Film22_SC2_Sub,  //49
PQ_IP_Film_any_SC2_Sub,  //50
PQ_IP_DIPF_SC2_Sub,  //51
PQ_IP_VCLPF_SC2_Sub,  //52
PQ_IP_Spike_NR_SC2_Sub,  //53
PQ_IP_SPF_SC2_Sub,  //54
PQ_IP_SPF_DBK_SC2_Sub,  //55
PQ_IP_SPF_DBK_BKN_SC2_Sub,  //56
PQ_IP_SPF_DBK_MR_SC2_Sub,  //57
PQ_IP_SPF_SNR_SC2_Sub,  //58
PQ_IP_SPF_SNR_MR_SC2_Sub,  //59
PQ_IP_SPF_MR_LPF_SC2_Sub,  //60
PQ_IP_SPF_NMR_Y_SC2_Sub,  //61
PQ_IP_SPF_NMR_Y_MR_SC2_Sub,  //62
PQ_IP_SPF_NMR_C_SC2_Sub,  //63
PQ_IP_DMS_SC2_Sub,  //64
PQ_IP_DMS_H_SC2_Sub,  //65
PQ_IP_DMS_V_SC2_Sub,  //66
PQ_IP_DMS_V_12L_SC2_Sub,  //67
PQ_IP_VSP_Y_SC2_Sub,  //68
PQ_IP_VSP_C_SC2_Sub,  //69
PQ_IP_VSP_CoRing_SC2_Sub,  //70
PQ_IP_VSP_DeRing_SC2_Sub,  //71
PQ_IP_VSP_Dither_SC2_Sub,  //72
PQ_IP_VSP_PreVBound_SC2_Sub,  //73
PQ_IP_422To444_SC2_Sub,  //74
PQ_IP_PreCTI_SC2_Sub,  //75
PQ_IP_HSP_Y_SC2_Sub,  //76
PQ_IP_HSP_C_SC2_Sub,  //77
PQ_IP_HSP_CoRing_SC2_Sub,  //78
PQ_IP_HSP_DeRing_SC2_Sub,  //79
PQ_IP_HSP_Dither_SC2_Sub,  //80
PQ_IP_HnonLinear_SC2_Sub,  //81
PQ_IP_SRAM1_SC2_Sub,  //82
PQ_IP_SRAM2_SC2_Sub,  //83
PQ_IP_SRAM3_SC2_Sub,  //84
PQ_IP_SRAM4_SC2_Sub,  //85
PQ_IP_C_SRAM1_SC2_Sub,  //86
PQ_IP_C_SRAM2_SC2_Sub,  //87
PQ_IP_C_SRAM3_SC2_Sub,  //88
PQ_IP_C_SRAM4_SC2_Sub,  //89
PQ_IP_VIP_SC2_Sub,  //90
PQ_IP_VIP_pseudo_SC2_Sub,  //91
PQ_IP_VIP_CSC_SC2_Sub,  //92
PQ_IP_VIP_CSC_dither_SC2_Sub,  //93
PQ_IP_VIP_Post_YC_delay_SC2_Sub,  //94
PQ_IP_VIP_HNMR_Y_SC2_Sub,  //95
PQ_IP_VIP_HNMR_C_SC2_Sub,  //96
PQ_IP_VIP_HNMR_ad_C_SC2_Sub,  //97
PQ_IP_VIP_HNMR_ad_C_gain_SC2_Sub,  //98
PQ_IP_VIP_HNMR_C_win1_SC2_Sub,  //99
PQ_IP_VIP_HNMR_C_win2_SC2_Sub,  //100
PQ_IP_VIP_Pre_Yoffset_SC2_Sub,  //101
PQ_IP_VIP_Pre_Ygain_SC2_Sub,  //102
PQ_IP_VIP_Pre_Ygain_dither_SC2_Sub,  //103
PQ_IP_VIP_HLPF_SC2_Sub,  //104
PQ_IP_VIP_HLPF_dither_SC2_Sub,  //105
PQ_IP_VIP_VNMR_SC2_Sub,  //106
PQ_IP_VIP_VNMR_dither_SC2_Sub,  //107
PQ_IP_VIP_VLPF_coef1_SC2_Sub,  //108
PQ_IP_VIP_VLPF_coef2_SC2_Sub,  //109
PQ_IP_VIP_VLPF_dither_SC2_Sub,  //110
PQ_IP_VIP_EE_SC2_Sub,  //111
PQ_IP_VIP_Peaking_SC2_Sub,  //112
PQ_IP_VIP_Peaking_band_SC2_Sub,  //113
PQ_IP_VIP_Peaking_dering_SC2_Sub,  //114
PQ_IP_VIP_Peaking_Pcoring_SC2_Sub,  //115
PQ_IP_VIP_Peaking_gain_SC2_Sub,  //116
PQ_IP_VIP_Peaking_gain_ad_C_SC2_Sub,  //117
PQ_IP_VIP_Peaking_gain_ad_Y_SC2_Sub,  //118
PQ_IP_VIP_Post_SNR_SC2_Sub,  //119
PQ_IP_VIP_Post_CTI_SC2_Sub,  //120
PQ_IP_VIP_Post_CTI_coef_SC2_Sub,  //121
PQ_IP_VIP_Post_CTI_gray_SC2_Sub,  //122
PQ_IP_VIP_FCC_full_range_SC2_Sub,  //123
PQ_IP_VIP_FCC_T1_SC2_Sub,  //124
PQ_IP_VIP_FCC_T2_SC2_Sub,  //125
PQ_IP_VIP_FCC_T3_SC2_Sub,  //126
PQ_IP_VIP_FCC_T4_SC2_Sub,  //127
PQ_IP_VIP_FCC_T5_SC2_Sub,  //128
PQ_IP_VIP_FCC_T6_SC2_Sub,  //129
PQ_IP_VIP_FCC_T7_SC2_Sub,  //130
PQ_IP_VIP_FCC_T8_SC2_Sub,  //131
PQ_IP_VIP_FCC_T9_SC2_Sub,  //132
PQ_IP_VIP_IHC_SC2_Sub,  //133
PQ_IP_VIP_IHC_Ymode_SC2_Sub,  //134
PQ_IP_VIP_IHC_dither_SC2_Sub,  //135
PQ_IP_VIP_IHC_CRD_SRAM_SC2_Sub,  //136
PQ_IP_VIP_IHC_SETTING_SC2_Sub,  //137
PQ_IP_VIP_ICC_SC2_Sub,  //138
PQ_IP_VIP_ICC_Ymode_SC2_Sub,  //139
PQ_IP_VIP_ICC_dither_SC2_Sub,  //140
PQ_IP_VIP_ICC_CRD_SRAM_SC2_Sub,  //141
PQ_IP_VIP_ICC_SETTING_SC2_Sub,  //142
PQ_IP_VIP_Ymode_Yvalue_ALL_SC2_Sub,  //143
PQ_IP_VIP_Ymode_Yvalue_SETTING_SC2_Sub,  //144
PQ_IP_VIP_IBC_SC2_Sub,  //145
PQ_IP_VIP_IBC_dither_SC2_Sub,  //146
PQ_IP_VIP_IBC_SETTING_SC2_Sub,  //147
PQ_IP_VIP_DLC_SC2_Sub,  //148
PQ_IP_VIP_DLC_dither_SC2_Sub,  //149
PQ_IP_VIP_DLC_His_range_SC2_Sub,  //150
PQ_IP_VIP_DLC_His_rangeH_SC2_Sub,  //151
PQ_IP_VIP_DLC_His_rangeV_SC2_Sub,  //152
PQ_IP_VIP_DLC_PC_SC2_Sub,  //153
PQ_IP_VIP_BLE_SC2_Sub,  //154
PQ_IP_VIP_WLE_SC2_Sub,  //155
PQ_IP_VIP_BWLE_dither_SC2_Sub,  //156
PQ_IP_VIP_UVC_SC2_Sub,  //157
PQ_IP_VIP_Post_Yoffset_SC2_Sub,  //158
PQ_IP_VIP_Post_Ygain_SC2_Sub,  //159
PQ_IP_VIP_Post_Yoffset_2_SC2_Sub,  //160
PQ_IP_VIP_Post_Cgain_SC2_Sub,  //161
PQ_IP_VIP_Post_Cgain_by_Y_SC2_Sub,  //162
PQ_IP_VIP_Hcoring_Y_SC2_Sub,  //163
PQ_IP_VIP_Hcoring_C_SC2_Sub,  //164
PQ_IP_VIP_Hcoring_dither_SC2_Sub,  //165
PQ_IP_VIP_YCbCr_Clip_SC2_Sub,  //166
PQ_IP_SwDriver_SC2_Sub,  //167
PQ_IP_3x3_SC2_Sub,  //168
PQ_IP_RGB_Offset_SC2_Sub,  //169
PQ_IP_RGB_Clip_SC2_Sub,  //170
PQ_IP_xvYCC_bypass_SC2_Sub,  //171
PQ_IP_xvYCC_de_gamma_SC2_Sub,  //172
PQ_IP_xvYCC_de_gamma_dither_SC2_Sub,  //173
PQ_IP_xvYCC_3x3_SC2_Sub,  //174
PQ_IP_xvYCC_Comp_SC2_Sub,  //175
PQ_IP_xvYCC_Comp_dither_SC2_Sub,  //176
PQ_IP_xvYCC_Clip_SC2_Sub,  //177
PQ_IP_xvYCC_gamma_SC2_Sub,  //178
PQ_IP_xvYCC_gamma_dither_SC2_Sub,  //179
PQ_IP_rgb_3d_SC2_Sub,  //180
PQ_IP_HBC_SC2_Sub,  //181
PQ_IP_Pre_CON_BRI_SC2_Sub,  //182
PQ_IP_Blue_Stretch_SC2_Sub,  //183
PQ_IP_Blue_Stretch_dither_SC2_Sub,  //184
PQ_IP_Gamma_SC2_Sub,  //185
PQ_IP_Gamma_dither_SC2_Sub,  //186
PQ_IP_Post_CON_BRI_SC2_Sub,  //187
PQ_IP_SRAM_3x3matrix_PIC2_SC2_Sub,  //188
PQ_IP_SRAM_DLC_PIC1_SC2_Sub,  //189
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_SC2_Sub,  //190
PQ_IP_SRAM_GammaTbl_R_PIC1_SC2_Sub,  //191
PQ_IP_SRAM_GammaTbl_G_PIC1_SC2_Sub,  //192
PQ_IP_SRAM_GammaTbl_B_PIC1_SC2_Sub,  //193
PQ_IP_SRAM_Auto_Color_PIC2_SC2_Sub,  //194
PQ_IP_SRAM_Color_Temp_PIC1_SC2_Sub,  //195
 }   PQ_IPTYPE_SC2_Sub;

typedef enum
{
PQ_IP_AFEC_COM_SC2_Sub,  //0
PQ_IP_Comb_COM_SC2_Sub,  //1
PQ_IP_Comb2_COM_SC2_Sub,  //2
PQ_IP_SECAM_COM_SC2_Sub,  //3
PQ_IP_VD_Sampling_no_comm_COM_SC2_Sub,  //4
PQ_IP_SCinit_COM_SC2_Sub,  //5
PQ_IP_CSC_COM_SC2_Sub,  //6
PQ_IP_CSC_Dither_COM_SC2_Sub,  //7
PQ_IP_YCdelay_COM_SC2_Sub,  //8
PQ_IP_PreFilter_COM_SC2_Sub,  //9
PQ_IP_PreFilter_Dither_COM_SC2_Sub,  //10
PQ_IP_HDSDD_COM_SC2_Sub,  //11
PQ_IP_HSD_Y_COM_SC2_Sub,  //12
PQ_IP_HSD_C_COM_SC2_Sub,  //13
PQ_IP_444To422_COM_SC2_Sub,  //14
PQ_IP_VSD_COM_SC2_Sub,  //15
PQ_IP_HVSD_Dither_COM_SC2_Sub,  //16
PQ_IP_10to8_Dither_COM_SC2_Sub,  //17
PQ_IP_MemFormat_COM_SC2_Sub,  //18
PQ_IP_PreSNR_COM_SC2_Sub,  //19
PQ_IP_PreSNR_Patch_COM_SC2_Sub,  //20
PQ_IP_DNR_COM_SC2_Sub,  //21
PQ_IP_DNR_Motion_COM_SC2_Sub,  //22
PQ_IP_DNR_Y_COM_SC2_Sub,  //23
PQ_IP_DNR_Y_COLOR_DEP_COM_SC2_Sub,  //24
PQ_IP_SRAM_COLOR_INDEX_COM_SC2_Sub,  //25
PQ_IP_SRAM_COLOR_GAIN_SNR_COM_SC2_Sub,  //26
PQ_IP_SRAM_COLOR_GAIN_DNR_COM_SC2_Sub,  //27
PQ_IP_DNR_Y_LUMA_ADAPTIVE_COM_SC2_Sub,  //28
PQ_IP_DNR_POSTTUNE_COM_SC2_Sub,  //29
PQ_IP_DNR_C_COM_SC2_Sub,  //30
PQ_IP_HISDNR_COM_SC2_Sub,  //31
PQ_IP_PNR_COM_SC2_Sub,  //32
PQ_IP_PNR_Y_COM_SC2_Sub,  //33
PQ_IP_PNR_C_COM_SC2_Sub,  //34
PQ_IP_PostCCS_COM_SC2_Sub,  //35
PQ_IP_PostCCS_Smooth_COM_SC2_Sub,  //36
PQ_IP_420CUP_COM_SC2_Sub,  //37
PQ_IP_MADi_COM_SC2_Sub,  //38
PQ_IP_MADi_Motion_COM_SC2_Sub,  //39
PQ_IP_MADi_ADP3x3_COM_SC2_Sub,  //40
PQ_IP_MADi_MORPHO_COM_SC2_Sub,  //41
PQ_IP_MADi_DFK_COM_SC2_Sub,  //42
PQ_IP_MADi_SST_COM_SC2_Sub,  //43
PQ_IP_MADi_EODiW_COM_SC2_Sub,  //44
PQ_IP_MADi_Force_COM_SC2_Sub,  //45
PQ_IP_EODi_COM_SC2_Sub,  //46
PQ_IP_Film_COM_SC2_Sub,  //47
PQ_IP_Film32_COM_SC2_Sub,  //48
PQ_IP_Film22_COM_SC2_Sub,  //49
PQ_IP_Film_any_COM_SC2_Sub,  //50
PQ_IP_DIPF_COM_SC2_Sub,  //51
PQ_IP_VCLPF_COM_SC2_Sub,  //52
PQ_IP_Spike_NR_COM_SC2_Sub,  //53
PQ_IP_SPF_COM_SC2_Sub,  //54
PQ_IP_SPF_DBK_COM_SC2_Sub,  //55
PQ_IP_SPF_DBK_BKN_COM_SC2_Sub,  //56
PQ_IP_SPF_DBK_MR_COM_SC2_Sub,  //57
PQ_IP_SPF_SNR_COM_SC2_Sub,  //58
PQ_IP_SPF_SNR_MR_COM_SC2_Sub,  //59
PQ_IP_SPF_MR_LPF_COM_SC2_Sub,  //60
PQ_IP_SPF_NMR_Y_COM_SC2_Sub,  //61
PQ_IP_SPF_NMR_Y_MR_COM_SC2_Sub,  //62
PQ_IP_SPF_NMR_C_COM_SC2_Sub,  //63
PQ_IP_DMS_COM_SC2_Sub,  //64
PQ_IP_DMS_H_COM_SC2_Sub,  //65
PQ_IP_DMS_V_COM_SC2_Sub,  //66
PQ_IP_DMS_V_12L_COM_SC2_Sub,  //67
PQ_IP_VSP_Y_COM_SC2_Sub,  //68
PQ_IP_VSP_C_COM_SC2_Sub,  //69
PQ_IP_VSP_CoRing_COM_SC2_Sub,  //70
PQ_IP_VSP_DeRing_COM_SC2_Sub,  //71
PQ_IP_VSP_Dither_COM_SC2_Sub,  //72
PQ_IP_VSP_PreVBound_COM_SC2_Sub,  //73
PQ_IP_422To444_COM_SC2_Sub,  //74
PQ_IP_PreCTI_COM_SC2_Sub,  //75
PQ_IP_HSP_Y_COM_SC2_Sub,  //76
PQ_IP_HSP_C_COM_SC2_Sub,  //77
PQ_IP_HSP_CoRing_COM_SC2_Sub,  //78
PQ_IP_HSP_DeRing_COM_SC2_Sub,  //79
PQ_IP_HSP_Dither_COM_SC2_Sub,  //80
PQ_IP_HnonLinear_COM_SC2_Sub,  //81
PQ_IP_SRAM1_COM_SC2_Sub,  //82
PQ_IP_SRAM2_COM_SC2_Sub,  //83
PQ_IP_SRAM3_COM_SC2_Sub,  //84
PQ_IP_SRAM4_COM_SC2_Sub,  //85
PQ_IP_C_SRAM1_COM_SC2_Sub,  //86
PQ_IP_C_SRAM2_COM_SC2_Sub,  //87
PQ_IP_C_SRAM3_COM_SC2_Sub,  //88
PQ_IP_C_SRAM4_COM_SC2_Sub,  //89
PQ_IP_VIP_COM_SC2_Sub,  //90
PQ_IP_VIP_pseudo_COM_SC2_Sub,  //91
PQ_IP_VIP_CSC_COM_SC2_Sub,  //92
PQ_IP_VIP_CSC_dither_COM_SC2_Sub,  //93
PQ_IP_VIP_Post_YC_delay_COM_SC2_Sub,  //94
PQ_IP_VIP_HNMR_Y_COM_SC2_Sub,  //95
PQ_IP_VIP_HNMR_C_COM_SC2_Sub,  //96
PQ_IP_VIP_HNMR_ad_C_COM_SC2_Sub,  //97
PQ_IP_VIP_HNMR_ad_C_gain_COM_SC2_Sub,  //98
PQ_IP_VIP_HNMR_C_win1_COM_SC2_Sub,  //99
PQ_IP_VIP_HNMR_C_win2_COM_SC2_Sub,  //100
PQ_IP_VIP_Pre_Yoffset_COM_SC2_Sub,  //101
PQ_IP_VIP_Pre_Ygain_COM_SC2_Sub,  //102
PQ_IP_VIP_Pre_Ygain_dither_COM_SC2_Sub,  //103
PQ_IP_VIP_HLPF_COM_SC2_Sub,  //104
PQ_IP_VIP_HLPF_dither_COM_SC2_Sub,  //105
PQ_IP_VIP_VNMR_COM_SC2_Sub,  //106
PQ_IP_VIP_VNMR_dither_COM_SC2_Sub,  //107
PQ_IP_VIP_VLPF_coef1_COM_SC2_Sub,  //108
PQ_IP_VIP_VLPF_coef2_COM_SC2_Sub,  //109
PQ_IP_VIP_VLPF_dither_COM_SC2_Sub,  //110
PQ_IP_VIP_EE_COM_SC2_Sub,  //111
PQ_IP_VIP_Peaking_COM_SC2_Sub,  //112
PQ_IP_VIP_Peaking_band_COM_SC2_Sub,  //113
PQ_IP_VIP_Peaking_dering_COM_SC2_Sub,  //114
PQ_IP_VIP_Peaking_Pcoring_COM_SC2_Sub,  //115
PQ_IP_VIP_Peaking_gain_COM_SC2_Sub,  //116
PQ_IP_VIP_Peaking_gain_ad_C_COM_SC2_Sub,  //117
PQ_IP_VIP_Peaking_gain_ad_Y_COM_SC2_Sub,  //118
PQ_IP_VIP_Post_SNR_COM_SC2_Sub,  //119
PQ_IP_VIP_Post_CTI_COM_SC2_Sub,  //120
PQ_IP_VIP_Post_CTI_coef_COM_SC2_Sub,  //121
PQ_IP_VIP_Post_CTI_gray_COM_SC2_Sub,  //122
PQ_IP_VIP_FCC_full_range_COM_SC2_Sub,  //123
PQ_IP_VIP_FCC_T1_COM_SC2_Sub,  //124
PQ_IP_VIP_FCC_T2_COM_SC2_Sub,  //125
PQ_IP_VIP_FCC_T3_COM_SC2_Sub,  //126
PQ_IP_VIP_FCC_T4_COM_SC2_Sub,  //127
PQ_IP_VIP_FCC_T5_COM_SC2_Sub,  //128
PQ_IP_VIP_FCC_T6_COM_SC2_Sub,  //129
PQ_IP_VIP_FCC_T7_COM_SC2_Sub,  //130
PQ_IP_VIP_FCC_T8_COM_SC2_Sub,  //131
PQ_IP_VIP_FCC_T9_COM_SC2_Sub,  //132
PQ_IP_VIP_IHC_COM_SC2_Sub,  //133
PQ_IP_VIP_IHC_Ymode_COM_SC2_Sub,  //134
PQ_IP_VIP_IHC_dither_COM_SC2_Sub,  //135
PQ_IP_VIP_IHC_CRD_SRAM_COM_SC2_Sub,  //136
PQ_IP_VIP_IHC_SETTING_COM_SC2_Sub,  //137
PQ_IP_VIP_ICC_COM_SC2_Sub,  //138
PQ_IP_VIP_ICC_Ymode_COM_SC2_Sub,  //139
PQ_IP_VIP_ICC_dither_COM_SC2_Sub,  //140
PQ_IP_VIP_ICC_CRD_SRAM_COM_SC2_Sub,  //141
PQ_IP_VIP_ICC_SETTING_COM_SC2_Sub,  //142
PQ_IP_VIP_Ymode_Yvalue_ALL_COM_SC2_Sub,  //143
PQ_IP_VIP_Ymode_Yvalue_SETTING_COM_SC2_Sub,  //144
PQ_IP_VIP_IBC_COM_SC2_Sub,  //145
PQ_IP_VIP_IBC_dither_COM_SC2_Sub,  //146
PQ_IP_VIP_IBC_SETTING_COM_SC2_Sub,  //147
PQ_IP_VIP_DLC_COM_SC2_Sub,  //148
PQ_IP_VIP_DLC_dither_COM_SC2_Sub,  //149
PQ_IP_VIP_DLC_His_range_COM_SC2_Sub,  //150
PQ_IP_VIP_DLC_His_rangeH_COM_SC2_Sub,  //151
PQ_IP_VIP_DLC_His_rangeV_COM_SC2_Sub,  //152
PQ_IP_VIP_DLC_PC_COM_SC2_Sub,  //153
PQ_IP_VIP_BLE_COM_SC2_Sub,  //154
PQ_IP_VIP_WLE_COM_SC2_Sub,  //155
PQ_IP_VIP_BWLE_dither_COM_SC2_Sub,  //156
PQ_IP_VIP_UVC_COM_SC2_Sub,  //157
PQ_IP_VIP_Post_Yoffset_COM_SC2_Sub,  //158
PQ_IP_VIP_Post_Ygain_COM_SC2_Sub,  //159
PQ_IP_VIP_Post_Yoffset_2_COM_SC2_Sub,  //160
PQ_IP_VIP_Post_Cgain_COM_SC2_Sub,  //161
PQ_IP_VIP_Post_Cgain_by_Y_COM_SC2_Sub,  //162
PQ_IP_VIP_Hcoring_Y_COM_SC2_Sub,  //163
PQ_IP_VIP_Hcoring_C_COM_SC2_Sub,  //164
PQ_IP_VIP_Hcoring_dither_COM_SC2_Sub,  //165
PQ_IP_VIP_YCbCr_Clip_COM_SC2_Sub,  //166
PQ_IP_SwDriver_COM_SC2_Sub,  //167
PQ_IP_3x3_COM_SC2_Sub,  //168
PQ_IP_RGB_Offset_COM_SC2_Sub,  //169
PQ_IP_RGB_Clip_COM_SC2_Sub,  //170
PQ_IP_xvYCC_bypass_COM_SC2_Sub,  //171
PQ_IP_xvYCC_de_gamma_COM_SC2_Sub,  //172
PQ_IP_xvYCC_de_gamma_dither_COM_SC2_Sub,  //173
PQ_IP_xvYCC_3x3_COM_SC2_Sub,  //174
PQ_IP_xvYCC_Comp_COM_SC2_Sub,  //175
PQ_IP_xvYCC_Comp_dither_COM_SC2_Sub,  //176
PQ_IP_xvYCC_Clip_COM_SC2_Sub,  //177
PQ_IP_xvYCC_gamma_COM_SC2_Sub,  //178
PQ_IP_xvYCC_gamma_dither_COM_SC2_Sub,  //179
PQ_IP_rgb_3d_COM_SC2_Sub,  //180
PQ_IP_HBC_COM_SC2_Sub,  //181
PQ_IP_Pre_CON_BRI_COM_SC2_Sub,  //182
PQ_IP_Blue_Stretch_COM_SC2_Sub,  //183
PQ_IP_Blue_Stretch_dither_COM_SC2_Sub,  //184
PQ_IP_Gamma_COM_SC2_Sub,  //185
PQ_IP_Gamma_dither_COM_SC2_Sub,  //186
PQ_IP_Post_CON_BRI_COM_SC2_Sub,  //187
PQ_IP_SRAM_3x3matrix_PIC2_COM_SC2_Sub,  //188
PQ_IP_SRAM_DLC_PIC1_COM_SC2_Sub,  //189
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_COM_SC2_Sub,  //190
PQ_IP_SRAM_GammaTbl_R_PIC1_COM_SC2_Sub,  //191
PQ_IP_SRAM_GammaTbl_G_PIC1_COM_SC2_Sub,  //192
PQ_IP_SRAM_GammaTbl_B_PIC1_COM_SC2_Sub,  //193
PQ_IP_SRAM_Auto_Color_PIC2_COM_SC2_Sub,  //194
PQ_IP_SRAM_Color_Temp_PIC1_COM_SC2_Sub,  //195
PQ_IP_COM_NUMS_SC2_Sub
} PQ_IP_COM_Group_SC2_Sub;

typedef enum
{
PQ_IP_AFEC_ADCIN_SC2_Sub,
PQ_IP_AFEC_RFIN_SC2_Sub,
PQ_IP_AFEC_ADCIN_SECAM_SC2_Sub,
PQ_IP_AFEC_NUMS_SC2_Sub
} PQ_IP_AFEC_Group_SC2_Sub;

typedef enum
{
PQ_IP_Comb_NTSC_AV_SC2_Sub,
PQ_IP_Comb_PAL_AV_SC2_Sub,
PQ_IP_Comb_NTSC_RF_SC2_Sub,
PQ_IP_Comb_PAL_RF_SC2_Sub,
PQ_IP_Comb_PAL_RF_VIF_SC2_Sub,
PQ_IP_Comb_NTSC_SV_SC2_Sub,
PQ_IP_Comb_PAL_SV_SC2_Sub,
PQ_IP_Comb_NTSC443_SC2_Sub,
PQ_IP_Comb_NTSC443_SV_SC2_Sub,
PQ_IP_Comb_PAL_M_SC2_Sub,
PQ_IP_Comb_PAL60_SC2_Sub,
PQ_IP_Comb_SECAM_SC2_Sub,
PQ_IP_Comb_NTSC_AV_SONY_SC2_Sub,
PQ_IP_Comb_PAL_AV_SONY_SC2_Sub,
PQ_IP_Comb_NTSC_RF_SONY_SC2_Sub,
PQ_IP_Comb_PAL_RF_SONY_SC2_Sub,
PQ_IP_Comb_NUMS_SC2_Sub
} PQ_IP_Comb_Group_SC2_Sub;

typedef enum
{
PQ_IP_Comb2_NTSC_AV_SC2_Sub,
PQ_IP_Comb2_PAL_AV_SC2_Sub,
PQ_IP_Comb2_NTSC_RF_SC2_Sub,
PQ_IP_Comb2_PAL_RF_SC2_Sub,
PQ_IP_Comb2_PAL_RF_VIF_SC2_Sub,
PQ_IP_Comb2_NTSC_SV_SC2_Sub,
PQ_IP_Comb2_PAL_SV_SC2_Sub,
PQ_IP_Comb2_NTSC443_SC2_Sub,
PQ_IP_Comb2_NTSC443_SV_SC2_Sub,
PQ_IP_Comb2_PAL_M_SC2_Sub,
PQ_IP_Comb2_PAL60_SC2_Sub,
PQ_IP_Comb2_SECAM_SC2_Sub,
PQ_IP_Comb2_NTSC_AV_SONY_SC2_Sub,
PQ_IP_Comb2_PAL_AV_SONY_SC2_Sub,
PQ_IP_Comb2_NTSC_RF_SONY_SC2_Sub,
PQ_IP_Comb2_PAL_RF_SONY_SC2_Sub,
PQ_IP_Comb2_NUMS_SC2_Sub
} PQ_IP_Comb2_Group_SC2_Sub;

typedef enum
{
PQ_IP_SECAM_ALL_SC2_Sub,
PQ_IP_SECAM_NUMS_SC2_Sub
} PQ_IP_SECAM_Group_SC2_Sub;

typedef enum
{
PQ_IP_VD_Sampling_no_comm_N_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P6_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_S_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_N4_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PM_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_1135_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_N_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P6_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_S_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_N4_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PM_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_1135_1o5_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_N_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P_Dynmiac_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_P6_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_S_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_N4_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PM_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_PNC_Dynamic_SC2_Sub,
PQ_IP_VD_Sampling_no_comm_NUMS_SC2_Sub
} PQ_IP_VD_Sampling_no_comm_Group_SC2_Sub;

typedef enum
{
PQ_IP_SCinit_Init_SC2_Sub,
PQ_IP_SCinit_NUMS_SC2_Sub
} PQ_IP_SCinit_Group_SC2_Sub;

typedef enum
{
PQ_IP_CSC_OFF_SC2_Sub,
PQ_IP_CSC_L_RGB2YCC_SD_SC2_Sub,
PQ_IP_CSC_F_RGB2YCC_SD_SC2_Sub,
PQ_IP_CSC_L_RGB2YCC_HD_SC2_Sub,
PQ_IP_CSC_F_RGB2YCC_HD_SC2_Sub,
PQ_IP_CSC_NUMS_SC2_Sub
} PQ_IP_CSC_Group_SC2_Sub;

typedef enum
{
PQ_IP_CSC_Dither_OFF_SC2_Sub,
PQ_IP_CSC_Dither_ON_SC2_Sub,
PQ_IP_CSC_Dither_NUMS_SC2_Sub
} PQ_IP_CSC_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_YCdelay_OFF_SC2_Sub,
PQ_IP_YCdelay_YC_91_SC2_Sub,
PQ_IP_YCdelay_YC_92_SC2_Sub,
PQ_IP_YCdelay_NUMS_SC2_Sub
} PQ_IP_YCdelay_Group_SC2_Sub;

typedef enum
{
PQ_IP_PreFilter_OFF_SC2_Sub,
PQ_IP_PreFilter_NUMS_SC2_Sub
} PQ_IP_PreFilter_Group_SC2_Sub;

typedef enum
{
PQ_IP_PreFilter_Dither_OFF_SC2_Sub,
PQ_IP_PreFilter_Dither_NUMS_SC2_Sub
} PQ_IP_PreFilter_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_HDSDD_OFF_SC2_Sub,
PQ_IP_HDSDD_NUMS_SC2_Sub
} PQ_IP_HDSDD_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSD_Y_OFF_SC2_Sub,
PQ_IP_HSD_Y_CB_SC2_Sub,
PQ_IP_HSD_Y_LPF40Ap0As40_SC2_Sub,
PQ_IP_HSD_Y_LPF50Ap0As40_SC2_Sub,
PQ_IP_HSD_Y_LPF60Ap0As35_SC2_Sub,
PQ_IP_HSD_Y_LPF70Ap0As35_SC2_Sub,
PQ_IP_HSD_Y_LPF80Ap0As35_SC2_Sub,
PQ_IP_HSD_Y_LPF90Ap0As35_SC2_Sub,
PQ_IP_HSD_Y_ALLPASS1X_SC2_Sub,
PQ_IP_HSD_Y_FIR66_SC2_Sub,
PQ_IP_HSD_Y_FIR60_SC2_Sub,
PQ_IP_HSD_Y_FIR55_SC2_Sub,
PQ_IP_HSD_Y_FIR50_SC2_Sub,
PQ_IP_HSD_Y_NUMS_SC2_Sub
} PQ_IP_HSD_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSD_C_OFF_SC2_Sub,
PQ_IP_HSD_C_CB_SC2_Sub,
PQ_IP_HSD_C_HSDC_A_SC2_Sub,
PQ_IP_HSD_C_HSDC_B_SC2_Sub,
PQ_IP_HSD_C_ALLPASS1X_SC2_Sub,
PQ_IP_HSD_C_FIR50_SC2_Sub,
PQ_IP_HSD_C_FIR60_SC2_Sub,
PQ_IP_HSD_C_NUMS_SC2_Sub
} PQ_IP_HSD_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_444To422_OFF_SC2_Sub,
PQ_IP_444To422_ON_SC2_Sub,
PQ_IP_444To422_NUMS_SC2_Sub
} PQ_IP_444To422_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSD_OFF_SC2_Sub,
PQ_IP_VSD_CB_SC2_Sub,
PQ_IP_VSD_Bilinear_SC2_Sub,
PQ_IP_VSD_NUMS_SC2_Sub
} PQ_IP_VSD_Group_SC2_Sub;

typedef enum
{
PQ_IP_HVSD_Dither_OFF_SC2_Sub,
PQ_IP_HVSD_Dither_Vdith_SC2_Sub,
PQ_IP_HVSD_Dither_Hdith_SC2_Sub,
PQ_IP_HVSD_Dither_HVDith_SC2_Sub,
PQ_IP_HVSD_Dither_NUMS_SC2_Sub
} PQ_IP_HVSD_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_10to8_Dither_OFF_SC2_Sub,
PQ_IP_10to8_Dither_Rand_Dith_SC2_Sub,
PQ_IP_10to8_Dither_Fix_Dith_SC2_Sub,
PQ_IP_10to8_Dither_NUMS_SC2_Sub
} PQ_IP_10to8_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_MemFormat_422MF_SC2_Sub,
PQ_IP_MemFormat_444_10BIT_SC2_Sub,
PQ_IP_MemFormat_444_8BIT_SC2_Sub,
PQ_IP_MemFormat_NUMS_SC2_Sub
} PQ_IP_MemFormat_Group_SC2_Sub;

typedef enum
{
PQ_IP_PreSNR_OFF_SC2_Sub,
PQ_IP_PreSNR_NUMS_SC2_Sub
} PQ_IP_PreSNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_PreSNR_Patch_OFF_SC2_Sub,
PQ_IP_PreSNR_Patch_NUMS_SC2_Sub
} PQ_IP_PreSNR_Patch_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_NR_OFF_SC2_Sub,
PQ_IP_DNR_DNR_OFF_SC2_Sub,
PQ_IP_DNR_ON_SC2_Sub,
PQ_IP_DNR_NUMS_SC2_Sub
} PQ_IP_DNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_Motion_MR_NR_SC2_Sub,
PQ_IP_DNR_Motion_MR_nonNR_SC2_Sub,
PQ_IP_DNR_Motion_NUMS_SC2_Sub
} PQ_IP_DNR_Motion_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_Y_OFF_SC2_Sub,
PQ_IP_DNR_Y_DY_1_SC2_Sub,
PQ_IP_DNR_Y_DY_1_1_SC2_Sub,
PQ_IP_DNR_Y_DY_2_SC2_Sub,
PQ_IP_DNR_Y_DY_2_1_SC2_Sub,
PQ_IP_DNR_Y_DY_3_SC2_Sub,
PQ_IP_DNR_Y_DY_3_1_SC2_Sub,
PQ_IP_DNR_Y_DY_4_SC2_Sub,
PQ_IP_DNR_Y_DY_4_1_SC2_Sub,
PQ_IP_DNR_Y_DY_5_SC2_Sub,
PQ_IP_DNR_Y_DY_5_1_SC2_Sub,
PQ_IP_DNR_Y_DY_6_SC2_Sub,
PQ_IP_DNR_Y_DY_6_1_SC2_Sub,
PQ_IP_DNR_Y_DY_7_SC2_Sub,
PQ_IP_DNR_Y_DY_7_1_SC2_Sub,
PQ_IP_DNR_Y_DY_8_SC2_Sub,
PQ_IP_DNR_Y_DY_8_1_SC2_Sub,
PQ_IP_DNR_Y_DY_9_SC2_Sub,
PQ_IP_DNR_Y_DY_9_1_SC2_Sub,
PQ_IP_DNR_Y_DY_10_SC2_Sub,
PQ_IP_DNR_Y_DY_10_1_SC2_Sub,
PQ_IP_DNR_Y_NUMS_SC2_Sub
} PQ_IP_DNR_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_Y_COLOR_DEP_OFF_SC2_Sub,
PQ_IP_DNR_Y_COLOR_DEP_NUMS_SC2_Sub
} PQ_IP_DNR_Y_COLOR_DEP_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_INDEX_OFF_SC2_Sub,
PQ_IP_SRAM_COLOR_INDEX_Index0_SC2_Sub,
PQ_IP_SRAM_COLOR_INDEX_NUMS_SC2_Sub
} PQ_IP_SRAM_COLOR_INDEX_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_SNR_OFF_SC2_Sub,
PQ_IP_SRAM_COLOR_GAIN_SNR_GainSNR0_SC2_Sub,
PQ_IP_SRAM_COLOR_GAIN_SNR_NUMS_SC2_Sub
} PQ_IP_SRAM_COLOR_GAIN_SNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_COLOR_GAIN_DNR_OFF_SC2_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR0_SC2_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_GainDNR1_SC2_Sub,
PQ_IP_SRAM_COLOR_GAIN_DNR_NUMS_SC2_Sub
} PQ_IP_SRAM_COLOR_GAIN_DNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_Y_LUMA_ADAPTIVE_OFF_SC2_Sub,
PQ_IP_DNR_Y_LUMA_ADAPTIVE_NUMS_SC2_Sub
} PQ_IP_DNR_Y_LUMA_ADAPTIVE_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_POSTTUNE_OFF_SC2_Sub,
PQ_IP_DNR_POSTTUNE_NUMS_SC2_Sub
} PQ_IP_DNR_POSTTUNE_Group_SC2_Sub;

typedef enum
{
PQ_IP_DNR_C_OFF_SC2_Sub,
PQ_IP_DNR_C_DC_1_SC2_Sub,
PQ_IP_DNR_C_DC_1_1_SC2_Sub,
PQ_IP_DNR_C_DC_2_SC2_Sub,
PQ_IP_DNR_C_DC_2_1_SC2_Sub,
PQ_IP_DNR_C_DC_3_SC2_Sub,
PQ_IP_DNR_C_DC_3_1_SC2_Sub,
PQ_IP_DNR_C_DC_4_SC2_Sub,
PQ_IP_DNR_C_DC_4_1_SC2_Sub,
PQ_IP_DNR_C_DC_5_SC2_Sub,
PQ_IP_DNR_C_DC_5_1_SC2_Sub,
PQ_IP_DNR_C_DC_6_SC2_Sub,
PQ_IP_DNR_C_DC_6_1_SC2_Sub,
PQ_IP_DNR_C_DC_7_SC2_Sub,
PQ_IP_DNR_C_DC_7_1_SC2_Sub,
PQ_IP_DNR_C_DC_8_SC2_Sub,
PQ_IP_DNR_C_DC_8_1_SC2_Sub,
PQ_IP_DNR_C_DC_9_SC2_Sub,
PQ_IP_DNR_C_DC_9_1_SC2_Sub,
PQ_IP_DNR_C_DC_10_SC2_Sub,
PQ_IP_DNR_C_DC_10_1_SC2_Sub,
PQ_IP_DNR_C_NUMS_SC2_Sub
} PQ_IP_DNR_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_HISDNR_OFF_SC2_Sub,
PQ_IP_HISDNR_NUMS_SC2_Sub
} PQ_IP_HISDNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_PNR_OFF_SC2_Sub,
PQ_IP_PNR_PNR_PCCS_AVG_OFF_SC2_Sub,
PQ_IP_PNR_ON_SC2_Sub,
PQ_IP_PNR_AVG_ON_SC2_Sub,
PQ_IP_PNR_NUMS_SC2_Sub
} PQ_IP_PNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_PNR_Y_OFF_SC2_Sub,
PQ_IP_PNR_Y_PY1_SC2_Sub,
PQ_IP_PNR_Y_NUMS_SC2_Sub
} PQ_IP_PNR_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_PNR_C_OFF_SC2_Sub,
PQ_IP_PNR_C_PC1_SC2_Sub,
PQ_IP_PNR_C_NUMS_SC2_Sub
} PQ_IP_PNR_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_PostCCS_OFF_SC2_Sub,
PQ_IP_PostCCS_PC_0_SC2_Sub,
PQ_IP_PostCCS_PC_1_SC2_Sub,
PQ_IP_PostCCS_PC_2_SC2_Sub,
PQ_IP_PostCCS_PC_3_SC2_Sub,
PQ_IP_PostCCS_PC_4_SC2_Sub,
PQ_IP_PostCCS_PC_5_SC2_Sub,
PQ_IP_PostCCS_PC_6_SC2_Sub,
PQ_IP_PostCCS_PC_7_SC2_Sub,
PQ_IP_PostCCS_PC_8_SC2_Sub,
PQ_IP_PostCCS_PC_9_SC2_Sub,
PQ_IP_PostCCS_PC_10_SC2_Sub,
PQ_IP_PostCCS_PC_11_SC2_Sub,
PQ_IP_PostCCS_PC_12_SC2_Sub,
PQ_IP_PostCCS_PC_13_SC2_Sub,
PQ_IP_PostCCS_PC_14_SC2_Sub,
PQ_IP_PostCCS_PC_15_SC2_Sub,
PQ_IP_PostCCS_PC_16_SC2_Sub,
PQ_IP_PostCCS_PC_17_SC2_Sub,
PQ_IP_PostCCS_PC_18_SC2_Sub,
PQ_IP_PostCCS_PC_19_SC2_Sub,
PQ_IP_PostCCS_PC_20_SC2_Sub,
PQ_IP_PostCCS_PC_21_SC2_Sub,
PQ_IP_PostCCS_PC_22_SC2_Sub,
PQ_IP_PostCCS_PC_23_SC2_Sub,
PQ_IP_PostCCS_PC_24_SC2_Sub,
PQ_IP_PostCCS_NUMS_SC2_Sub
} PQ_IP_PostCCS_Group_SC2_Sub;

typedef enum
{
PQ_IP_PostCCS_Smooth_OFF_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_0_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_1_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_2_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_3_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_4_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_5_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_6_SC2_Sub,
PQ_IP_PostCCS_Smooth_PCS_7_SC2_Sub,
PQ_IP_PostCCS_Smooth_NUMS_SC2_Sub
} PQ_IP_PostCCS_Smooth_Group_SC2_Sub;

typedef enum
{
PQ_IP_420CUP_OFF_SC2_Sub,
PQ_IP_420CUP_ON_SC2_Sub,
PQ_IP_420CUP_NUMS_SC2_Sub
} PQ_IP_420CUP_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_24_4R_SC2_Sub,
PQ_IP_MADi_24_2R_SC2_Sub,
PQ_IP_MADi_25_4R_SC2_Sub,
PQ_IP_MADi_25_2R_SC2_Sub,
PQ_IP_MADi_27_4R_SC2_Sub,
PQ_IP_MADi_27_2R_SC2_Sub,
PQ_IP_MADi_P_MODE8_SC2_Sub,
PQ_IP_MADi_P_MODE10_SC2_Sub,
PQ_IP_MADi_P_MODE_MOT10_SC2_Sub,
PQ_IP_MADi_P_MODE_MOT8_SC2_Sub,
PQ_IP_MADi_NUMS_SC2_Sub
} PQ_IP_MADi_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_Motion_MOT_4R_5_SC2_Sub,
PQ_IP_MADi_Motion_MOT_4R_6_SC2_Sub,
PQ_IP_MADi_Motion_MOT_4R_7_SC2_Sub,
PQ_IP_MADi_Motion_MOT_2R_SC2_Sub,
PQ_IP_MADi_Motion_MOT_PMODE_SC2_Sub,
PQ_IP_MADi_Motion_NUMS_SC2_Sub
} PQ_IP_MADi_Motion_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_ADP3x3_OFF_SC2_Sub,
PQ_IP_MADi_ADP3x3_ADP1_SC2_Sub,
PQ_IP_MADi_ADP3x3_NUMS_SC2_Sub
} PQ_IP_MADi_ADP3x3_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_MORPHO_OFF_SC2_Sub,
PQ_IP_MADi_MORPHO_M1_SC2_Sub,
PQ_IP_MADi_MORPHO_NUMS_SC2_Sub
} PQ_IP_MADi_MORPHO_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_DFK_OFF_SC2_Sub,
PQ_IP_MADi_DFK_DFK1_SC2_Sub,
PQ_IP_MADi_DFK_DFK2_SC2_Sub,
PQ_IP_MADi_DFK_DFK3_SC2_Sub,
PQ_IP_MADi_DFK_DFK4_SC2_Sub,
PQ_IP_MADi_DFK_DFK5_SC2_Sub,
PQ_IP_MADi_DFK_DFK6_SC2_Sub,
PQ_IP_MADi_DFK_NUMS_SC2_Sub
} PQ_IP_MADi_DFK_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_SST_OFF_SC2_Sub,
PQ_IP_MADi_SST_SST1_SC2_Sub,
PQ_IP_MADi_SST_SST2_SC2_Sub,
PQ_IP_MADi_SST_SST3_SC2_Sub,
PQ_IP_MADi_SST_SST4_SC2_Sub,
PQ_IP_MADi_SST_SST5_SC2_Sub,
PQ_IP_MADi_SST_SST6_SC2_Sub,
PQ_IP_MADi_SST_SST_Rec1_SC2_Sub,
PQ_IP_MADi_SST_NUMS_SC2_Sub
} PQ_IP_MADi_SST_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_EODiW_OFF_SC2_Sub,
PQ_IP_MADi_EODiW_W1_SC2_Sub,
PQ_IP_MADi_EODiW_W2_SC2_Sub,
PQ_IP_MADi_EODiW_W3_SC2_Sub,
PQ_IP_MADi_EODiW_NUMS_SC2_Sub
} PQ_IP_MADi_EODiW_Group_SC2_Sub;

typedef enum
{
PQ_IP_MADi_Force_OFF_SC2_Sub,
PQ_IP_MADi_Force_YC_FullMotion_SC2_Sub,
PQ_IP_MADi_Force_YC_FullStill_SC2_Sub,
PQ_IP_MADi_Force_Y_FullMotion_SC2_Sub,
PQ_IP_MADi_Force_Y_FullStill_SC2_Sub,
PQ_IP_MADi_Force_C_FullMotion_SC2_Sub,
PQ_IP_MADi_Force_C_FullStill_SC2_Sub,
PQ_IP_MADi_Force_NUMS_SC2_Sub
} PQ_IP_MADi_Force_Group_SC2_Sub;

typedef enum
{
PQ_IP_EODi_OFF_SC2_Sub,
PQ_IP_EODi_SD_15_SC2_Sub,
PQ_IP_EODi_SD_14_SC2_Sub,
PQ_IP_EODi_SD_13_SC2_Sub,
PQ_IP_EODi_SD_12_SC2_Sub,
PQ_IP_EODi_SD_11_SC2_Sub,
PQ_IP_EODi_SD_10_SC2_Sub,
PQ_IP_EODi_SD_9_SC2_Sub,
PQ_IP_EODi_SD_8_SC2_Sub,
PQ_IP_EODi_SD_7_SC2_Sub,
PQ_IP_EODi_SD_6_SC2_Sub,
PQ_IP_EODi_SD_5_SC2_Sub,
PQ_IP_EODi_SD_4_SC2_Sub,
PQ_IP_EODi_SD_3_SC2_Sub,
PQ_IP_EODi_SD_2_SC2_Sub,
PQ_IP_EODi_SD_1_SC2_Sub,
PQ_IP_EODi_HD_15_SC2_Sub,
PQ_IP_EODi_HD_14_SC2_Sub,
PQ_IP_EODi_HD_13_SC2_Sub,
PQ_IP_EODi_HD_12_SC2_Sub,
PQ_IP_EODi_HD_11_SC2_Sub,
PQ_IP_EODi_HD_10_SC2_Sub,
PQ_IP_EODi_HD_9_SC2_Sub,
PQ_IP_EODi_HD_8_SC2_Sub,
PQ_IP_EODi_HD_7_SC2_Sub,
PQ_IP_EODi_HD_6_SC2_Sub,
PQ_IP_EODi_HD_5_SC2_Sub,
PQ_IP_EODi_HD_4_SC2_Sub,
PQ_IP_EODi_HD_3_SC2_Sub,
PQ_IP_EODi_HD_2_SC2_Sub,
PQ_IP_EODi_HD_1_SC2_Sub,
PQ_IP_EODi_NUMS_SC2_Sub
} PQ_IP_EODi_Group_SC2_Sub;

typedef enum
{
PQ_IP_Film_OFF_SC2_Sub,
PQ_IP_Film_SD_2_SC2_Sub,
PQ_IP_Film_SD_1_SC2_Sub,
PQ_IP_Film_SD_3_SC2_Sub,
PQ_IP_Film_HD_2_SC2_Sub,
PQ_IP_Film_HD_1_SC2_Sub,
PQ_IP_Film_HD_3_SC2_Sub,
PQ_IP_Film_NUMS_SC2_Sub
} PQ_IP_Film_Group_SC2_Sub;

typedef enum
{
PQ_IP_Film32_OFF_SC2_Sub,
PQ_IP_Film32_SD_1_SC2_Sub,
PQ_IP_Film32_SD_2_SC2_Sub,
PQ_IP_Film32_SD_3_SC2_Sub,
PQ_IP_Film32_SD_4_SC2_Sub,
PQ_IP_Film32_SD_5_SC2_Sub,
PQ_IP_Film32_SD_6_SC2_Sub,
PQ_IP_Film32_SD_7_SC2_Sub,
PQ_IP_Film32_SD_8_SC2_Sub,
PQ_IP_Film32_SD_9_SC2_Sub,
PQ_IP_Film32_SD_10_SC2_Sub,
PQ_IP_Film32_HD_1_SC2_Sub,
PQ_IP_Film32_HD_2_SC2_Sub,
PQ_IP_Film32_HD_3_SC2_Sub,
PQ_IP_Film32_HD_4_SC2_Sub,
PQ_IP_Film32_HD_5_SC2_Sub,
PQ_IP_Film32_HD_6_SC2_Sub,
PQ_IP_Film32_NUMS_SC2_Sub
} PQ_IP_Film32_Group_SC2_Sub;

typedef enum
{
PQ_IP_Film22_OFF_SC2_Sub,
PQ_IP_Film22_SD_1_SC2_Sub,
PQ_IP_Film22_SD_2_SC2_Sub,
PQ_IP_Film22_SD_3_SC2_Sub,
PQ_IP_Film22_SD_4_SC2_Sub,
PQ_IP_Film22_SD_5_SC2_Sub,
PQ_IP_Film22_SD_6_SC2_Sub,
PQ_IP_Film22_SD_7_SC2_Sub,
PQ_IP_Film22_SD_8_SC2_Sub,
PQ_IP_Film22_SD_9_SC2_Sub,
PQ_IP_Film22_SD_10_SC2_Sub,
PQ_IP_Film22_HD_1_SC2_Sub,
PQ_IP_Film22_HD_2_SC2_Sub,
PQ_IP_Film22_HD_3_SC2_Sub,
PQ_IP_Film22_HD_4_SC2_Sub,
PQ_IP_Film22_HD_5_SC2_Sub,
PQ_IP_Film22_HD_6_SC2_Sub,
PQ_IP_Film22_NUMS_SC2_Sub
} PQ_IP_Film22_Group_SC2_Sub;

typedef enum
{
PQ_IP_Film_any_OFF_SC2_Sub,
PQ_IP_Film_any_SD_1_SC2_Sub,
PQ_IP_Film_any_SD_2_SC2_Sub,
PQ_IP_Film_any_SD_3_SC2_Sub,
PQ_IP_Film_any_SD_4_SC2_Sub,
PQ_IP_Film_any_SD_5_SC2_Sub,
PQ_IP_Film_any_SD_6_SC2_Sub,
PQ_IP_Film_any_SD_7_SC2_Sub,
PQ_IP_Film_any_SD_8_SC2_Sub,
PQ_IP_Film_any_SD_9_SC2_Sub,
PQ_IP_Film_any_SD_10_SC2_Sub,
PQ_IP_Film_any_HD_1_SC2_Sub,
PQ_IP_Film_any_HD_2_SC2_Sub,
PQ_IP_Film_any_HD_3_SC2_Sub,
PQ_IP_Film_any_HD_4_SC2_Sub,
PQ_IP_Film_any_HD_5_SC2_Sub,
PQ_IP_Film_any_HD_6_SC2_Sub,
PQ_IP_Film_any_NUMS_SC2_Sub
} PQ_IP_Film_any_Group_SC2_Sub;

typedef enum
{
PQ_IP_DIPF_OFF_SC2_Sub,
PQ_IP_DIPF_DIPF1_SC2_Sub,
PQ_IP_DIPF_NUMS_SC2_Sub
} PQ_IP_DIPF_Group_SC2_Sub;

typedef enum
{
PQ_IP_VCLPF_OFF_SC2_Sub,
PQ_IP_VCLPF_ON_SC2_Sub,
PQ_IP_VCLPF_NUMS_SC2_Sub
} PQ_IP_VCLPF_Group_SC2_Sub;

typedef enum
{
PQ_IP_Spike_NR_OFF_SC2_Sub,
PQ_IP_Spike_NR_S1_SC2_Sub,
PQ_IP_Spike_NR_S2_SC2_Sub,
PQ_IP_Spike_NR_S3_SC2_Sub,
PQ_IP_Spike_NR_NUMS_SC2_Sub
} PQ_IP_Spike_NR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_OFF_SC2_Sub,
PQ_IP_SPF_NUMS_SC2_Sub
} PQ_IP_SPF_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_DBK_OFF_SC2_Sub,
PQ_IP_SPF_DBK_E2S2_SC2_Sub,
PQ_IP_SPF_DBK_NUMS_SC2_Sub
} PQ_IP_SPF_DBK_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_DBK_BKN_OFF_SC2_Sub,
PQ_IP_SPF_DBK_BKN_T0_SC2_Sub,
PQ_IP_SPF_DBK_BKN_NUMS_SC2_Sub
} PQ_IP_SPF_DBK_BKN_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_DBK_MR_OFF_SC2_Sub,
PQ_IP_SPF_DBK_MR_ON_SC2_Sub,
PQ_IP_SPF_DBK_MR_NUMS_SC2_Sub
} PQ_IP_SPF_DBK_MR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_SNR_OFF_SC2_Sub,
PQ_IP_SPF_SNR_E1S1_SC2_Sub,
PQ_IP_SPF_SNR_E1S2_SC2_Sub,
PQ_IP_SPF_SNR_E2S1_SC2_Sub,
PQ_IP_SPF_SNR_E3S1_SC2_Sub,
PQ_IP_SPF_SNR_E4S1_SC2_Sub,
PQ_IP_SPF_SNR_E4S2_SC2_Sub,
PQ_IP_SPF_SNR_E4S3_SC2_Sub,
PQ_IP_SPF_SNR_NUMS_SC2_Sub
} PQ_IP_SPF_SNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_SNR_MR_OFF_SC2_Sub,
PQ_IP_SPF_SNR_MR_ON_SC2_Sub,
PQ_IP_SPF_SNR_MR_NUMS_SC2_Sub
} PQ_IP_SPF_SNR_MR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_MR_LPF_OFF_SC2_Sub,
PQ_IP_SPF_MR_LPF_LPF3x3_SC2_Sub,
PQ_IP_SPF_MR_LPF_NUMS_SC2_Sub
} PQ_IP_SPF_MR_LPF_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_NMR_Y_OFF_SC2_Sub,
PQ_IP_SPF_NMR_Y_S1_SC2_Sub,
PQ_IP_SPF_NMR_Y_S2_SC2_Sub,
PQ_IP_SPF_NMR_Y_S3_SC2_Sub,
PQ_IP_SPF_NMR_Y_NUMS_SC2_Sub
} PQ_IP_SPF_NMR_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_NMR_Y_MR_OFF_SC2_Sub,
PQ_IP_SPF_NMR_Y_MR_ON_SC2_Sub,
PQ_IP_SPF_NMR_Y_MR_NUMS_SC2_Sub
} PQ_IP_SPF_NMR_Y_MR_Group_SC2_Sub;

typedef enum
{
PQ_IP_SPF_NMR_C_OFF_SC2_Sub,
PQ_IP_SPF_NMR_C_S1_SC2_Sub,
PQ_IP_SPF_NMR_C_S2_SC2_Sub,
PQ_IP_SPF_NMR_C_S3_SC2_Sub,
PQ_IP_SPF_NMR_C_NUMS_SC2_Sub
} PQ_IP_SPF_NMR_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_DMS_OFF_SC2_Sub,
PQ_IP_DMS_ON_SC2_Sub,
PQ_IP_DMS_NUMS_SC2_Sub
} PQ_IP_DMS_Group_SC2_Sub;

typedef enum
{
PQ_IP_DMS_H_OFF_SC2_Sub,
PQ_IP_DMS_H_S1_SC2_Sub,
PQ_IP_DMS_H_S2_SC2_Sub,
PQ_IP_DMS_H_S3_SC2_Sub,
PQ_IP_DMS_H_NUMS_SC2_Sub
} PQ_IP_DMS_H_Group_SC2_Sub;

typedef enum
{
PQ_IP_DMS_V_OFF_SC2_Sub,
PQ_IP_DMS_V_S1_SC2_Sub,
PQ_IP_DMS_V_S2_SC2_Sub,
PQ_IP_DMS_V_NUMS_SC2_Sub
} PQ_IP_DMS_V_Group_SC2_Sub;

typedef enum
{
PQ_IP_DMS_V_12L_OFF_SC2_Sub,
PQ_IP_DMS_V_12L_NUMS_SC2_Sub
} PQ_IP_DMS_V_12L_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_Y_Bypass_SC2_Sub,
PQ_IP_VSP_Y_Bilinear_SC2_Sub,
PQ_IP_VSP_Y_SRAM_3_4Tap_SC2_Sub,
PQ_IP_VSP_Y_SRAM_4_4Tap_SC2_Sub,
PQ_IP_VSP_Y_SRAM_3_6Tap_SC2_Sub,
PQ_IP_VSP_Y_SRAM_4_6Tap_SC2_Sub,
PQ_IP_VSP_Y_NUMS_SC2_Sub
} PQ_IP_VSP_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_C_Bypass_SC2_Sub,
PQ_IP_VSP_C_Bilinear_SC2_Sub,
PQ_IP_VSP_C_C_SRAM_1_SC2_Sub,
PQ_IP_VSP_C_C_SRAM_2_SC2_Sub,
PQ_IP_VSP_C_C_SRAM_3_SC2_Sub,
PQ_IP_VSP_C_C_SRAM_4_SC2_Sub,
PQ_IP_VSP_C_SRAM_3_4Tap_SC2_Sub,
PQ_IP_VSP_C_SRAM_4_4Tap_SC2_Sub,
PQ_IP_VSP_C_NUMS_SC2_Sub
} PQ_IP_VSP_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_CoRing_OFF_SC2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_1_SC2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_2_SC2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_3_SC2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_4_SC2_Sub,
PQ_IP_VSP_CoRing_Y_Coring_5_SC2_Sub,
PQ_IP_VSP_CoRing_NUMS_SC2_Sub
} PQ_IP_VSP_CoRing_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_DeRing_OFF_SC2_Sub,
PQ_IP_VSP_DeRing_DR1_SC2_Sub,
PQ_IP_VSP_DeRing_NUMS_SC2_Sub
} PQ_IP_VSP_DeRing_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_Dither_OFF_SC2_Sub,
PQ_IP_VSP_Dither_ON_SC2_Sub,
PQ_IP_VSP_Dither_NUMS_SC2_Sub
} PQ_IP_VSP_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VSP_PreVBound_OFF_SC2_Sub,
PQ_IP_VSP_PreVBound_ON_SC2_Sub,
PQ_IP_VSP_PreVBound_NUMS_SC2_Sub
} PQ_IP_VSP_PreVBound_Group_SC2_Sub;

typedef enum
{
PQ_IP_422To444_ON_SC2_Sub,
PQ_IP_422To444_OFF_SC2_Sub,
PQ_IP_422To444_NUMS_SC2_Sub
} PQ_IP_422To444_Group_SC2_Sub;

typedef enum
{
PQ_IP_PreCTI_OFF_SC2_Sub,
PQ_IP_PreCTI_CTI_0_SC2_Sub,
PQ_IP_PreCTI_CTI_1_SC2_Sub,
PQ_IP_PreCTI_NUMS_SC2_Sub
} PQ_IP_PreCTI_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSP_Y_Bypass_SC2_Sub,
PQ_IP_HSP_Y_Bilinear_SC2_Sub,
PQ_IP_HSP_Y_SRAM_3_4Tap_SC2_Sub,
PQ_IP_HSP_Y_SRAM_4_4Tap_SC2_Sub,
PQ_IP_HSP_Y_SRAM_3_6Tap_SC2_Sub,
PQ_IP_HSP_Y_SRAM_4_6Tap_SC2_Sub,
PQ_IP_HSP_Y_NUMS_SC2_Sub
} PQ_IP_HSP_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSP_C_Bypass_SC2_Sub,
PQ_IP_HSP_C_Bilinear_SC2_Sub,
PQ_IP_HSP_C_C_SRAM_1_SC2_Sub,
PQ_IP_HSP_C_C_SRAM_2_SC2_Sub,
PQ_IP_HSP_C_C_SRAM_3_SC2_Sub,
PQ_IP_HSP_C_C_SRAM_4_SC2_Sub,
PQ_IP_HSP_C_SRAM_3_4Tap_SC2_Sub,
PQ_IP_HSP_C_SRAM_4_4Tap_SC2_Sub,
PQ_IP_HSP_C_NUMS_SC2_Sub
} PQ_IP_HSP_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSP_CoRing_OFF_SC2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_1_SC2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_2_SC2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_3_SC2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_4_SC2_Sub,
PQ_IP_HSP_CoRing_Y_Coring_5_SC2_Sub,
PQ_IP_HSP_CoRing_NUMS_SC2_Sub
} PQ_IP_HSP_CoRing_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSP_DeRing_OFF_SC2_Sub,
PQ_IP_HSP_DeRing_DR1_SC2_Sub,
PQ_IP_HSP_DeRing_NUMS_SC2_Sub
} PQ_IP_HSP_DeRing_Group_SC2_Sub;

typedef enum
{
PQ_IP_HSP_Dither_OFF_SC2_Sub,
PQ_IP_HSP_Dither_ON_SC2_Sub,
PQ_IP_HSP_Dither_NUMS_SC2_Sub
} PQ_IP_HSP_Dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_HnonLinear_OFF_SC2_Sub,
PQ_IP_HnonLinear_H_1366_0_SC2_Sub,
PQ_IP_HnonLinear_H_1366_1_SC2_Sub,
PQ_IP_HnonLinear_H_1366_2_SC2_Sub,
PQ_IP_HnonLinear_H_1440_SC2_Sub,
PQ_IP_HnonLinear_H_1680_SC2_Sub,
PQ_IP_HnonLinear_H_1920_0_SC2_Sub,
PQ_IP_HnonLinear_H_1920_1_SC2_Sub,
PQ_IP_HnonLinear_H_1920_2_SC2_Sub,
PQ_IP_HnonLinear_NUMS_SC2_Sub
} PQ_IP_HnonLinear_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM1_OFF_SC2_Sub,
PQ_IP_SRAM1_NUMS_SC2_Sub
} PQ_IP_SRAM1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM2_OFF_SC2_Sub,
PQ_IP_SRAM2_NUMS_SC2_Sub
} PQ_IP_SRAM2_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM3_InvSinc4Tc4p4Fc45Apass01Astop40_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc50Apass01Astop55_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_SC2_Sub,
PQ_IP_SRAM3_InvSinc4Tc4p4Fc65Apass3Astop60G14_SC2_Sub,
PQ_IP_SRAM3_InvSinc3Tc0p0Fc75Ap001As60G13_SC2_Sub,
PQ_IP_SRAM3_InvSinc3Tc0p0Fc75Ap001As60_SC2_Sub,
PQ_IP_SRAM3_NUMS_SC2_Sub
} PQ_IP_SRAM3_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM4_InvSinc4Tc4p4Fc45Apass01Astop40_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc50Apass01Astop55_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc75Fstop124Apass0001Astop40_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G11_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G12_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc85Fstop134Apass01Astop50G13_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc95Fstop148Apass0001Astop40_SC2_Sub,
PQ_IP_SRAM4_InvSinc4Tc4p4Fc65Apass3Astop60G14_SC2_Sub,
PQ_IP_SRAM4_InvSinc3Tc0p0Fc75Ap001As60G13_SC2_Sub,
PQ_IP_SRAM4_InvSinc3Tc0p0Fc75Ap001As60_SC2_Sub,
PQ_IP_SRAM4_NUMS_SC2_Sub
} PQ_IP_SRAM4_Group_SC2_Sub;

typedef enum
{
PQ_IP_C_SRAM1_C2121_SC2_Sub,
PQ_IP_C_SRAM1_C121_SC2_Sub,
PQ_IP_C_SRAM1_NUMS_SC2_Sub
} PQ_IP_C_SRAM1_Group_SC2_Sub;

typedef enum
{
PQ_IP_C_SRAM2_C2121_SC2_Sub,
PQ_IP_C_SRAM2_C121_SC2_Sub,
PQ_IP_C_SRAM2_NUMS_SC2_Sub
} PQ_IP_C_SRAM2_Group_SC2_Sub;

typedef enum
{
PQ_IP_C_SRAM3_C2121_SC2_Sub,
PQ_IP_C_SRAM3_C121_SC2_Sub,
PQ_IP_C_SRAM3_NUMS_SC2_Sub
} PQ_IP_C_SRAM3_Group_SC2_Sub;

typedef enum
{
PQ_IP_C_SRAM4_C2121_SC2_Sub,
PQ_IP_C_SRAM4_C121_SC2_Sub,
PQ_IP_C_SRAM4_NUMS_SC2_Sub
} PQ_IP_C_SRAM4_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_OFF_SC2_Sub,
PQ_IP_VIP_NUMS_SC2_Sub
} PQ_IP_VIP_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_pseudo_OFF_SC2_Sub,
PQ_IP_VIP_pseudo_NUMS_SC2_Sub
} PQ_IP_VIP_pseudo_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_CSC_OFF_SC2_Sub,
PQ_IP_VIP_CSC_L_RGB2YCC_SD_SC2_Sub,
PQ_IP_VIP_CSC_F_RGB2YCC_SD_SC2_Sub,
PQ_IP_VIP_CSC_L_RGB2YCC_HD_SC2_Sub,
PQ_IP_VIP_CSC_F_RGB2YCC_HD_SC2_Sub,
PQ_IP_VIP_CSC_NUMS_SC2_Sub
} PQ_IP_VIP_CSC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_CSC_dither_OFF_SC2_Sub,
PQ_IP_VIP_CSC_dither_NUMS_SC2_Sub
} PQ_IP_VIP_CSC_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_YC_delay_OFF_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb0Cr0_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb1Cr0_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb2Cr0_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_Y0Cb3Cr0_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_Y1Cb0Cr0_SC2_Sub,
PQ_IP_VIP_Post_YC_delay_NUMS_SC2_Sub
} PQ_IP_VIP_Post_YC_delay_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_Y_OFF_SC2_Sub,
PQ_IP_VIP_HNMR_Y_S1_SC2_Sub,
PQ_IP_VIP_HNMR_Y_S2_SC2_Sub,
PQ_IP_VIP_HNMR_Y_S3_SC2_Sub,
PQ_IP_VIP_HNMR_Y_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_OFF_SC2_Sub,
PQ_IP_VIP_HNMR_C_S1_SC2_Sub,
PQ_IP_VIP_HNMR_C_S2_SC2_Sub,
PQ_IP_VIP_HNMR_C_S3_SC2_Sub,
PQ_IP_VIP_HNMR_C_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_OFF_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_Yon_Con_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_Yon_Coff_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_Yoff_Con_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_ad_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_ad_C_gain_S0_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S1_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S2_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_S3_SC2_Sub,
PQ_IP_VIP_HNMR_ad_C_gain_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_ad_C_gain_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win1_OFF_SC2_Sub,
PQ_IP_VIP_HNMR_C_win1_Flesh_1_SC2_Sub,
PQ_IP_VIP_HNMR_C_win1_Flesh_2_SC2_Sub,
PQ_IP_VIP_HNMR_C_win1_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_C_win1_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HNMR_C_win2_OFF_SC2_Sub,
PQ_IP_VIP_HNMR_C_win2_Blue_1_SC2_Sub,
PQ_IP_VIP_HNMR_C_win2_Blue_2_SC2_Sub,
PQ_IP_VIP_HNMR_C_win2_NUMS_SC2_Sub
} PQ_IP_VIP_HNMR_C_win2_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Yoffset_OFF_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_0x05_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF4_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF6_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_0xF7_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_0xFA_SC2_Sub,
PQ_IP_VIP_Pre_Yoffset_NUMS_SC2_Sub
} PQ_IP_VIP_Pre_Yoffset_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_OFF_SC2_Sub,
PQ_IP_VIP_Pre_Ygain_0x42_SC2_Sub,
PQ_IP_VIP_Pre_Ygain_NUMS_SC2_Sub
} PQ_IP_VIP_Pre_Ygain_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Pre_Ygain_dither_OFF_SC2_Sub,
PQ_IP_VIP_Pre_Ygain_dither_NUMS_SC2_Sub
} PQ_IP_VIP_Pre_Ygain_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HLPF_OFF_SC2_Sub,
PQ_IP_VIP_HLPF_0x1_SC2_Sub,
PQ_IP_VIP_HLPF_0x2_SC2_Sub,
PQ_IP_VIP_HLPF_0x3_SC2_Sub,
PQ_IP_VIP_HLPF_0x4_SC2_Sub,
PQ_IP_VIP_HLPF_0x5_SC2_Sub,
PQ_IP_VIP_HLPF_0x6_SC2_Sub,
PQ_IP_VIP_HLPF_0x7_SC2_Sub,
PQ_IP_VIP_HLPF_NUMS_SC2_Sub
} PQ_IP_VIP_HLPF_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_HLPF_dither_OFF_SC2_Sub,
PQ_IP_VIP_HLPF_dither_NUMS_SC2_Sub
} PQ_IP_VIP_HLPF_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_VNMR_OFF_SC2_Sub,
PQ_IP_VIP_VNMR_S1_SC2_Sub,
PQ_IP_VIP_VNMR_S2_SC2_Sub,
PQ_IP_VIP_VNMR_S3_SC2_Sub,
PQ_IP_VIP_VNMR_NUMS_SC2_Sub
} PQ_IP_VIP_VNMR_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_VNMR_dither_OFF_SC2_Sub,
PQ_IP_VIP_VNMR_dither_ON_SC2_Sub,
PQ_IP_VIP_VNMR_dither_NUMS_SC2_Sub
} PQ_IP_VIP_VNMR_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_VLPF_coef1_OFF_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x1_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x2_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x3_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x4_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x5_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x6_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_0x7_SC2_Sub,
PQ_IP_VIP_VLPF_coef1_NUMS_SC2_Sub
} PQ_IP_VIP_VLPF_coef1_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_VLPF_coef2_OFF_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x1_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x2_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x3_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x4_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x5_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x6_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_0x7_SC2_Sub,
PQ_IP_VIP_VLPF_coef2_NUMS_SC2_Sub
} PQ_IP_VIP_VLPF_coef2_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_VLPF_dither_OFF_SC2_Sub,
PQ_IP_VIP_VLPF_dither_NUMS_SC2_Sub
} PQ_IP_VIP_VLPF_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_EE_OFF_SC2_Sub,
PQ_IP_VIP_EE_S2_SC2_Sub,
PQ_IP_VIP_EE_NUMS_SC2_Sub
} PQ_IP_VIP_EE_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_OFF_SC2_Sub,
PQ_IP_VIP_Peaking_ON_SC2_Sub,
PQ_IP_VIP_Peaking_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_band_RF_NTSC_SC2_Sub,
PQ_IP_VIP_Peaking_band_RF_SECAM_SC2_Sub,
PQ_IP_VIP_Peaking_band_AV_NTSC_SC2_Sub,
PQ_IP_VIP_Peaking_band_AV_PAL_SC2_Sub,
PQ_IP_VIP_Peaking_band_AV_SECAM_SC2_Sub,
PQ_IP_VIP_Peaking_band_SV_NTSC_SC2_Sub,
PQ_IP_VIP_Peaking_band_480i_SC2_Sub,
PQ_IP_VIP_Peaking_band_720p_SC2_Sub,
PQ_IP_VIP_Peaking_band_1080i_SC2_Sub,
PQ_IP_VIP_Peaking_band_H_480i_SC2_Sub,
PQ_IP_VIP_Peaking_band_H_576i_SC2_Sub,
PQ_IP_VIP_Peaking_band_H_720p_SC2_Sub,
PQ_IP_VIP_Peaking_band_H_1080i_SC2_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_480is_SC2_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_720p_SC2_Sub,
PQ_IP_VIP_Peaking_band_DT_MPEG2_1080i_SC2_Sub,
PQ_IP_VIP_Peaking_band_DT_H264_480is_SC2_Sub,
PQ_IP_VIP_Peaking_band_DT_H264_720p_SC2_Sub,
PQ_IP_VIP_Peaking_band_PC_mode_SC2_Sub,
PQ_IP_VIP_Peaking_band_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_band_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_dering_OFF_SC2_Sub,
PQ_IP_VIP_Peaking_dering_W1_R0_SC2_Sub,
PQ_IP_VIP_Peaking_dering_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_dering_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_Pcoring_S00_0_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S21_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S30_0_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S30_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S31_0_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S31_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S20_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S32_0_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S32_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_S42_1_SC2_Sub,
PQ_IP_VIP_Peaking_Pcoring_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_Pcoring_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_gain_0x18_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x1C_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x20_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x24_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x28_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x2C_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x30_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x34_SC2_Sub,
PQ_IP_VIP_Peaking_gain_0x38_SC2_Sub,
PQ_IP_VIP_Peaking_gain_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_gain_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_gain_ad_C_OFF_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T1_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T2_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T3_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T4_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T5_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_T6_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_C_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_gain_ad_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Peaking_gain_ad_Y_OFF_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_Y_T2_SC2_Sub,
PQ_IP_VIP_Peaking_gain_ad_Y_NUMS_SC2_Sub
} PQ_IP_VIP_Peaking_gain_ad_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_SNR_OFF_SC2_Sub,
PQ_IP_VIP_Post_SNR_S1_SC2_Sub,
PQ_IP_VIP_Post_SNR_S2_SC2_Sub,
PQ_IP_VIP_Post_SNR_S3_SC2_Sub,
PQ_IP_VIP_Post_SNR_S4_SC2_Sub,
PQ_IP_VIP_Post_SNR_S5_SC2_Sub,
PQ_IP_VIP_Post_SNR_S6_SC2_Sub,
PQ_IP_VIP_Post_SNR_S7_SC2_Sub,
PQ_IP_VIP_Post_SNR_S8_SC2_Sub,
PQ_IP_VIP_Post_SNR_S9_SC2_Sub,
PQ_IP_VIP_Post_SNR_NUMS_SC2_Sub
} PQ_IP_VIP_Post_SNR_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_CTI_OFF_SC2_Sub,
PQ_IP_VIP_Post_CTI_S0L0_SC2_Sub,
PQ_IP_VIP_Post_CTI_S3L3_SC2_Sub,
PQ_IP_VIP_Post_CTI_S1L6_SC2_Sub,
PQ_IP_VIP_Post_CTI_S1L0_SC2_Sub,
PQ_IP_VIP_Post_CTI_S2L3_SC2_Sub,
PQ_IP_VIP_Post_CTI_S2L4_SC2_Sub,
PQ_IP_VIP_Post_CTI_S2L5_SC2_Sub,
PQ_IP_VIP_Post_CTI_S2L6_SC2_Sub,
PQ_IP_VIP_Post_CTI_NUMS_SC2_Sub
} PQ_IP_VIP_Post_CTI_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_CTI_coef_0x18_SC2_Sub,
PQ_IP_VIP_Post_CTI_coef_0x1C_SC2_Sub,
PQ_IP_VIP_Post_CTI_coef_0x20_SC2_Sub,
PQ_IP_VIP_Post_CTI_coef_0x24_SC2_Sub,
PQ_IP_VIP_Post_CTI_coef_0x28_SC2_Sub,
PQ_IP_VIP_Post_CTI_coef_NUMS_SC2_Sub
} PQ_IP_VIP_Post_CTI_coef_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_CTI_gray_OFF_SC2_Sub,
PQ_IP_VIP_Post_CTI_gray_NUMS_SC2_Sub
} PQ_IP_VIP_Post_CTI_gray_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_full_range_OFF_SC2_Sub,
PQ_IP_VIP_FCC_full_range_ON_SC2_Sub,
PQ_IP_VIP_FCC_full_range_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_full_range_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T1_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T1_S4_SC2_Sub,
PQ_IP_VIP_FCC_T1_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T1_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T2_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T2_S8_SC2_Sub,
PQ_IP_VIP_FCC_T2_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T2_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T3_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T3_S4_SC2_Sub,
PQ_IP_VIP_FCC_T3_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T3_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T4_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T4_S4_SC2_Sub,
PQ_IP_VIP_FCC_T4_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T4_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T5_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T5_S6_SC2_Sub,
PQ_IP_VIP_FCC_T5_S8_SC2_Sub,
PQ_IP_VIP_FCC_T5_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T5_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T6_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T6_S6_SC2_Sub,
PQ_IP_VIP_FCC_T6_S8_SC2_Sub,
PQ_IP_VIP_FCC_T6_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T6_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T7_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T7_S8_SC2_Sub,
PQ_IP_VIP_FCC_T7_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T7_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T8_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T8_S5_SC2_Sub,
PQ_IP_VIP_FCC_T8_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T8_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_FCC_T9_OFF_SC2_Sub,
PQ_IP_VIP_FCC_T9_S5_SC2_Sub,
PQ_IP_VIP_FCC_T9_NUMS_SC2_Sub
} PQ_IP_VIP_FCC_T9_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IHC_OFF_SC2_Sub,
PQ_IP_VIP_IHC_ON_SC2_Sub,
PQ_IP_VIP_IHC_NUMS_SC2_Sub
} PQ_IP_VIP_IHC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IHC_Ymode_OFF_SC2_Sub,
PQ_IP_VIP_IHC_Ymode_ON_SC2_Sub,
PQ_IP_VIP_IHC_Ymode_NUMS_SC2_Sub
} PQ_IP_VIP_IHC_Ymode_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IHC_dither_OFF_SC2_Sub,
PQ_IP_VIP_IHC_dither_NUMS_SC2_Sub
} PQ_IP_VIP_IHC_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IHC_CRD_SRAM_OFF_SC2_Sub,
PQ_IP_VIP_IHC_CRD_SRAM_NUMS_SC2_Sub
} PQ_IP_VIP_IHC_CRD_SRAM_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IHC_SETTING_OFF_SC2_Sub,
PQ_IP_VIP_IHC_SETTING_N_20_SC2_Sub,
PQ_IP_VIP_IHC_SETTING_P_20_SC2_Sub,
PQ_IP_VIP_IHC_SETTING_NUMS_SC2_Sub
} PQ_IP_VIP_IHC_SETTING_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_ICC_OFF_SC2_Sub,
PQ_IP_VIP_ICC_ON_SC2_Sub,
PQ_IP_VIP_ICC_NUMS_SC2_Sub
} PQ_IP_VIP_ICC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_ICC_Ymode_OFF_SC2_Sub,
PQ_IP_VIP_ICC_Ymode_ON_SC2_Sub,
PQ_IP_VIP_ICC_Ymode_NUMS_SC2_Sub
} PQ_IP_VIP_ICC_Ymode_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_ICC_dither_OFF_SC2_Sub,
PQ_IP_VIP_ICC_dither_NUMS_SC2_Sub
} PQ_IP_VIP_ICC_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_ICC_CRD_SRAM_OFF_SC2_Sub,
PQ_IP_VIP_ICC_CRD_SRAM_NUMS_SC2_Sub
} PQ_IP_VIP_ICC_CRD_SRAM_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_ICC_SETTING_OFF_SC2_Sub,
PQ_IP_VIP_ICC_SETTING_INC_8_SC2_Sub,
PQ_IP_VIP_ICC_SETTING_INC_3_SC2_Sub,
PQ_IP_VIP_ICC_SETTING_INC_B_SC2_Sub,
PQ_IP_VIP_ICC_SETTING_DEC_3_SC2_Sub,
PQ_IP_VIP_ICC_SETTING_NUMS_SC2_Sub
} PQ_IP_VIP_ICC_SETTING_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Ymode_Yvalue_ALL_Y1_SC2_Sub,
PQ_IP_VIP_Ymode_Yvalue_ALL_NUMS_SC2_Sub
} PQ_IP_VIP_Ymode_Yvalue_ALL_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Ymode_Yvalue_SETTING_Y1_SC2_Sub,
PQ_IP_VIP_Ymode_Yvalue_SETTING_NUMS_SC2_Sub
} PQ_IP_VIP_Ymode_Yvalue_SETTING_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IBC_OFF_SC2_Sub,
PQ_IP_VIP_IBC_ON_SC2_Sub,
PQ_IP_VIP_IBC_NUMS_SC2_Sub
} PQ_IP_VIP_IBC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IBC_dither_OFF_SC2_Sub,
PQ_IP_VIP_IBC_dither_NUMS_SC2_Sub
} PQ_IP_VIP_IBC_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_IBC_SETTING_OFF_SC2_Sub,
PQ_IP_VIP_IBC_SETTING_DEC_10_SC2_Sub,
PQ_IP_VIP_IBC_SETTING_DEC_18_SC2_Sub,
PQ_IP_VIP_IBC_SETTING_NUMS_SC2_Sub
} PQ_IP_VIP_IBC_SETTING_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_OFF_SC2_Sub,
PQ_IP_VIP_DLC_ON_SC2_Sub,
PQ_IP_VIP_DLC_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_dither_OFF_SC2_Sub,
PQ_IP_VIP_DLC_dither_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_range_OFF_SC2_Sub,
PQ_IP_VIP_DLC_His_range_ON_SC2_Sub,
PQ_IP_VIP_DLC_His_range_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_His_range_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeH_90pa_1366_SC2_Sub,
PQ_IP_VIP_DLC_His_rangeH_90pa_1920_SC2_Sub,
PQ_IP_VIP_DLC_His_rangeH_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_His_rangeH_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_His_rangeV_90pa_1366_SC2_Sub,
PQ_IP_VIP_DLC_His_rangeV_90pa_1920_SC2_Sub,
PQ_IP_VIP_DLC_His_rangeV_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_His_rangeV_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_DLC_PC_OFF_SC2_Sub,
PQ_IP_VIP_DLC_PC_ON_SC2_Sub,
PQ_IP_VIP_DLC_PC_NUMS_SC2_Sub
} PQ_IP_VIP_DLC_PC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_BLE_OFF_SC2_Sub,
PQ_IP_VIP_BLE_0x82_0x40_SC2_Sub,
PQ_IP_VIP_BLE_0x82_0x50_SC2_Sub,
PQ_IP_VIP_BLE_0x82_0x60_SC2_Sub,
PQ_IP_VIP_BLE_0x84_0x40_SC2_Sub,
PQ_IP_VIP_BLE_0x86_0x40_SC2_Sub,
PQ_IP_VIP_BLE_0x88_0x40_SC2_Sub,
PQ_IP_VIP_BLE_0x88_0x60_SC2_Sub,
PQ_IP_VIP_BLE_0x8A_0x40_SC2_Sub,
PQ_IP_VIP_BLE_NUMS_SC2_Sub
} PQ_IP_VIP_BLE_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_WLE_OFF_SC2_Sub,
PQ_IP_VIP_WLE_0x78_0x20_SC2_Sub,
PQ_IP_VIP_WLE_NUMS_SC2_Sub
} PQ_IP_VIP_WLE_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_BWLE_dither_OFF_SC2_Sub,
PQ_IP_VIP_BWLE_dither_NUMS_SC2_Sub
} PQ_IP_VIP_BWLE_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_UVC_OFF_SC2_Sub,
PQ_IP_VIP_UVC_ON_SC2_Sub,
PQ_IP_VIP_UVC_NUMS_SC2_Sub
} PQ_IP_VIP_UVC_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_Yoffset_OFF_SC2_Sub,
PQ_IP_VIP_Post_Yoffset_0x05_SC2_Sub,
PQ_IP_VIP_Post_Yoffset_NUMS_SC2_Sub
} PQ_IP_VIP_Post_Yoffset_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_Ygain_OFF_SC2_Sub,
PQ_IP_VIP_Post_Ygain_0x3C_SC2_Sub,
PQ_IP_VIP_Post_Ygain_NUMS_SC2_Sub
} PQ_IP_VIP_Post_Ygain_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_Yoffset_2_OFF_SC2_Sub,
PQ_IP_VIP_Post_Yoffset_2_0x05_SC2_Sub,
PQ_IP_VIP_Post_Yoffset_2_NUMS_SC2_Sub
} PQ_IP_VIP_Post_Yoffset_2_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_Cgain_OFF_SC2_Sub,
PQ_IP_VIP_Post_Cgain_0x44_SC2_Sub,
PQ_IP_VIP_Post_Cgain_NUMS_SC2_Sub
} PQ_IP_VIP_Post_Cgain_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Post_Cgain_by_Y_OFF_SC2_Sub,
PQ_IP_VIP_Post_Cgain_by_Y_NUMS_SC2_Sub
} PQ_IP_VIP_Post_Cgain_by_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_Y_OFF_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_C4_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_C3_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_C2_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_C1_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P6_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P5_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P4_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P3_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P2_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_P1_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P6_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P5_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P4_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P3_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P2_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_PC_P1_SC2_Sub,
PQ_IP_VIP_Hcoring_Y_NUMS_SC2_Sub
} PQ_IP_VIP_Hcoring_Y_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_C_OFF_SC2_Sub,
PQ_IP_VIP_Hcoring_C_C4_SC2_Sub,
PQ_IP_VIP_Hcoring_C_C3_SC2_Sub,
PQ_IP_VIP_Hcoring_C_C2_SC2_Sub,
PQ_IP_VIP_Hcoring_C_C1_SC2_Sub,
PQ_IP_VIP_Hcoring_C_P5_SC2_Sub,
PQ_IP_VIP_Hcoring_C_P4_SC2_Sub,
PQ_IP_VIP_Hcoring_C_P3_SC2_Sub,
PQ_IP_VIP_Hcoring_C_P2_SC2_Sub,
PQ_IP_VIP_Hcoring_C_P1_SC2_Sub,
PQ_IP_VIP_Hcoring_C_NUMS_SC2_Sub
} PQ_IP_VIP_Hcoring_C_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_Hcoring_dither_OFF_SC2_Sub,
PQ_IP_VIP_Hcoring_dither_NUMS_SC2_Sub
} PQ_IP_VIP_Hcoring_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_VIP_YCbCr_Clip_OFF_SC2_Sub,
PQ_IP_VIP_YCbCr_Clip_NUMS_SC2_Sub
} PQ_IP_VIP_YCbCr_Clip_Group_SC2_Sub;

typedef enum
{
PQ_IP_SwDriver_OFF_SC2_Sub,
PQ_IP_SwDriver_ALL_SC2_Sub,
PQ_IP_SwDriver_DynamicDNR_ON_SC2_Sub,
PQ_IP_SwDriver_NUMS_SC2_Sub
} PQ_IP_SwDriver_Group_SC2_Sub;

typedef enum
{
PQ_IP_3x3_OFF_SC2_Sub,
PQ_IP_3x3_SD_SC2_Sub,
PQ_IP_3x3_HD_SC2_Sub,
PQ_IP_3x3_NUMS_SC2_Sub
} PQ_IP_3x3_Group_SC2_Sub;

typedef enum
{
PQ_IP_RGB_Offset_OFF_SC2_Sub,
PQ_IP_RGB_Offset_NUMS_SC2_Sub
} PQ_IP_RGB_Offset_Group_SC2_Sub;

typedef enum
{
PQ_IP_RGB_Clip_OFF_SC2_Sub,
PQ_IP_RGB_Clip_NUMS_SC2_Sub
} PQ_IP_RGB_Clip_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_bypass_OFF_SC2_Sub,
PQ_IP_xvYCC_bypass_ON_SC2_Sub,
PQ_IP_xvYCC_bypass_NUMS_SC2_Sub
} PQ_IP_xvYCC_bypass_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_de_gamma_OFF_SC2_Sub,
PQ_IP_xvYCC_de_gamma_ON_SC2_Sub,
PQ_IP_xvYCC_de_gamma_NUMS_SC2_Sub
} PQ_IP_xvYCC_de_gamma_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_de_gamma_dither_OFF_SC2_Sub,
PQ_IP_xvYCC_de_gamma_dither_ON_SC2_Sub,
PQ_IP_xvYCC_de_gamma_dither_NUMS_SC2_Sub
} PQ_IP_xvYCC_de_gamma_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_3x3_OFF_SC2_Sub,
PQ_IP_xvYCC_3x3_NUMS_SC2_Sub
} PQ_IP_xvYCC_3x3_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_Comp_OFF_SC2_Sub,
PQ_IP_xvYCC_Comp_NUMS_SC2_Sub
} PQ_IP_xvYCC_Comp_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_Comp_dither_OFF_SC2_Sub,
PQ_IP_xvYCC_Comp_dither_ON_SC2_Sub,
PQ_IP_xvYCC_Comp_dither_NUMS_SC2_Sub
} PQ_IP_xvYCC_Comp_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_Clip_OFF_SC2_Sub,
PQ_IP_xvYCC_Clip_NUMS_SC2_Sub
} PQ_IP_xvYCC_Clip_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_gamma_OFF_SC2_Sub,
PQ_IP_xvYCC_gamma_ON_SC2_Sub,
PQ_IP_xvYCC_gamma_NUMS_SC2_Sub
} PQ_IP_xvYCC_gamma_Group_SC2_Sub;

typedef enum
{
PQ_IP_xvYCC_gamma_dither_OFF_SC2_Sub,
PQ_IP_xvYCC_gamma_dither_ON_SC2_Sub,
PQ_IP_xvYCC_gamma_dither_NUMS_SC2_Sub
} PQ_IP_xvYCC_gamma_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_rgb_3d_OFF_SC2_Sub,
PQ_IP_rgb_3d_ON_SC2_Sub,
PQ_IP_rgb_3d_NUMS_SC2_Sub
} PQ_IP_rgb_3d_Group_SC2_Sub;

typedef enum
{
PQ_IP_HBC_OFF_SC2_Sub,
PQ_IP_HBC_HBC1_SC2_Sub,
PQ_IP_HBC_NUMS_SC2_Sub
} PQ_IP_HBC_Group_SC2_Sub;

typedef enum
{
PQ_IP_Pre_CON_BRI_OFF_SC2_Sub,
PQ_IP_Pre_CON_BRI_NUMS_SC2_Sub
} PQ_IP_Pre_CON_BRI_Group_SC2_Sub;

typedef enum
{
PQ_IP_Blue_Stretch_OFF_SC2_Sub,
PQ_IP_Blue_Stretch_BS1_SC2_Sub,
PQ_IP_Blue_Stretch_NUMS_SC2_Sub
} PQ_IP_Blue_Stretch_Group_SC2_Sub;

typedef enum
{
PQ_IP_Blue_Stretch_dither_OFF_SC2_Sub,
PQ_IP_Blue_Stretch_dither_NUMS_SC2_Sub
} PQ_IP_Blue_Stretch_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_Gamma_OFF_SC2_Sub,
PQ_IP_Gamma_ON_SC2_Sub,
PQ_IP_Gamma_NUMS_SC2_Sub
} PQ_IP_Gamma_Group_SC2_Sub;

typedef enum
{
PQ_IP_Gamma_dither_OFF_SC2_Sub,
PQ_IP_Gamma_dither_NUMS_SC2_Sub
} PQ_IP_Gamma_dither_Group_SC2_Sub;

typedef enum
{
PQ_IP_Post_CON_BRI_OFF_SC2_Sub,
PQ_IP_Post_CON_BRI_NUMS_SC2_Sub
} PQ_IP_Post_CON_BRI_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_3x3matrix_PIC2_Build_IN_SC2_Sub,
PQ_IP_SRAM_3x3matrix_PIC2_NUMS_SC2_Sub
} PQ_IP_SRAM_3x3matrix_PIC2_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_DLC_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_DLC_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_DLC_PIC1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_Bri_Con_Hue_Sat_Sha_PIC1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_R_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_GammaTbl_R_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_GammaTbl_R_PIC1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_G_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_GammaTbl_G_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_GammaTbl_G_PIC1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_GammaTbl_B_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_GammaTbl_B_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_GammaTbl_B_PIC1_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_Auto_Color_PIC2_Build_IN_SC2_Sub,
PQ_IP_SRAM_Auto_Color_PIC2_NUMS_SC2_Sub
} PQ_IP_SRAM_Auto_Color_PIC2_Group_SC2_Sub;

typedef enum
{
PQ_IP_SRAM_Color_Temp_PIC1_Build_IN_SC2_Sub,
PQ_IP_SRAM_Color_Temp_PIC1_NUMS_SC2_Sub
} PQ_IP_SRAM_Color_Temp_PIC1_Group_SC2_Sub;

extern code U8 MST_SkipRule_IP_SC2_Sub[PQ_IP_NUM_SC2_Sub];
extern code EN_IPTAB_INFO PQ_IPTAB_INFO_SC2_Sub[];
extern code U8 QMAP_1920_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];
extern code U8 QMAP_1920_PIP_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];
extern code U8 QMAP_1920_POP_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];
extern code U8 QMAP_1366_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];
extern code U8 QMAP_1366_PIP_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];
extern code U8 QMAP_1366_POP_SC2_Sub[QM_INPUTTYPE_NUM_SC2_Sub][PQ_IP_NUM_SC2_Sub];

#endif
