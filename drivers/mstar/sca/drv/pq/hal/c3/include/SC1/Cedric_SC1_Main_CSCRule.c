
code U8 MST_CSCRule_IP_Index_SC1_Main[PQ_CSCRule_IP_NUM_SC1_Main]=
{
    PQ_IP_CSC_SC1_Main,
    PQ_IP_VIP_CSC_SC1_Main,
};


code U8 MST_CSCRule_Array_SC1_Main[PQ_CSCRule_NUM_SC1_Main][PQ_CSCRule_IP_NUM_SC1_Main]=
{
    {//Video_RGB_SD_0_255, 0
        PQ_IP_CSC_F_RGB2YCC_SD_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//Video_RGB_SD_16_235, 1
        PQ_IP_CSC_L_RGB2YCC_SD_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//Video_RGB_HD_0_255, 2
        PQ_IP_CSC_F_RGB2YCC_HD_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//Video_RGB_HD_16_235, 3
        PQ_IP_CSC_L_RGB2YCC_HD_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//Video_YUV_SD, 4
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//Video_YUV_HD, 5
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//PC_RGB_SD_0_255, 6
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_F_RGB2YCC_SD_SC1_Main, 
    },
    {//PC_RGB_SD_16_235, 7
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_L_RGB2YCC_SD_SC1_Main, 
    },
    {//PC_RGB_HD_0_255, 8
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_F_RGB2YCC_HD_SC1_Main, 
    },
    {//PC_RGB_HD_16_235, 9
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_L_RGB2YCC_HD_SC1_Main, 
    },
    {//PC_YUV_SD, 10
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
    {//PC_YUV_HD, 11
        PQ_IP_CSC_OFF_SC1_Main, PQ_IP_VIP_CSC_OFF_SC1_Main, 
    },
};
