
code U8 MST_CSCRule_IP_Index_Sub[PQ_CSCRule_IP_NUM_Sub]=
{
    PQ_IP_CSC_Sub,
    PQ_IP_VIP_CSC_Sub,
};


code U8 MST_CSCRule_Array_Sub[PQ_CSCRule_NUM_Sub][PQ_CSCRule_IP_NUM_Sub]=
{
    {//Video_RGB_SD_0_255, 0
        PQ_IP_CSC_F_RGB2YCC_SD_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//Video_RGB_SD_16_235, 1
        PQ_IP_CSC_L_RGB2YCC_SD_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//Video_RGB_HD_0_255, 2
        PQ_IP_CSC_F_RGB2YCC_HD_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//Video_RGB_HD_16_235, 3
        PQ_IP_CSC_L_RGB2YCC_HD_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//Video_YUV_SD, 4
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//Video_YUV_HD, 5
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//PC_RGB_SD_0_255, 6
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_F_RGB2YCC_SD_Sub, 
    },
    {//PC_RGB_SD_16_235, 7
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_L_RGB2YCC_SD_Sub, 
    },
    {//PC_RGB_HD_0_255, 8
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_F_RGB2YCC_HD_Sub, 
    },
    {//PC_RGB_HD_16_235, 9
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_L_RGB2YCC_HD_Sub, 
    },
    {//PC_YUV_SD, 10
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
    {//PC_YUV_HD, 11
        PQ_IP_CSC_OFF_Sub, PQ_IP_VIP_CSC_OFF_Sub, 
    },
};
