#define PQ_CSCRule_ID_SC2_Main 4
#define PQ_CSCRule_IP_NUM_SC2_Main 2
#define PQ_CSCRule_NUM_SC2_Main 12


typedef enum
{
PQ_CSCRule_Video_RGB_SD_0_255_SC2_Main,
PQ_CSCRule_Video_RGB_SD_16_235_SC2_Main,
PQ_CSCRule_Video_RGB_HD_0_255_SC2_Main,
PQ_CSCRule_Video_RGB_HD_16_235_SC2_Main,
PQ_CSCRule_Video_YUV_SD_SC2_Main,
PQ_CSCRule_Video_YUV_HD_SC2_Main,
PQ_CSCRule_PC_RGB_SD_0_255_SC2_Main,
PQ_CSCRule_PC_RGB_SD_16_235_SC2_Main,
PQ_CSCRule_PC_RGB_HD_0_255_SC2_Main,
PQ_CSCRule_PC_RGB_HD_16_235_SC2_Main,
PQ_CSCRule_PC_YUV_SD_SC2_Main,
PQ_CSCRule_PC_YUV_HD_SC2_Main,
}
MST_CSCRule_Index_SC2_Main;
extern code U8 MST_CSCRule_IP_Index_SC2_Main[PQ_CSCRule_IP_NUM_SC2_Main];
extern code U8 MST_CSCRule_Array_SC2_Main[PQ_CSCRule_NUM_SC2_Main][PQ_CSCRule_IP_NUM_SC2_Main];

