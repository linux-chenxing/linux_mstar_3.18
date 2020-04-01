#define PQ_CSCRule_ID_Sub 4
#define PQ_CSCRule_IP_NUM_Sub 2
#define PQ_CSCRule_NUM_Sub 12


typedef enum
{
PQ_CSCRule_Video_RGB_SD_0_255_Sub,
PQ_CSCRule_Video_RGB_SD_16_235_Sub,
PQ_CSCRule_Video_RGB_HD_0_255_Sub,
PQ_CSCRule_Video_RGB_HD_16_235_Sub,
PQ_CSCRule_Video_YUV_SD_Sub,
PQ_CSCRule_Video_YUV_HD_Sub,
PQ_CSCRule_PC_RGB_SD_0_255_Sub,
PQ_CSCRule_PC_RGB_SD_16_235_Sub,
PQ_CSCRule_PC_RGB_HD_0_255_Sub,
PQ_CSCRule_PC_RGB_HD_16_235_Sub,
PQ_CSCRule_PC_YUV_SD_Sub,
PQ_CSCRule_PC_YUV_HD_Sub,
}
MST_CSCRule_Index_Sub;
extern code U8 MST_CSCRule_IP_Index_Sub[PQ_CSCRule_IP_NUM_Sub];
extern code U8 MST_CSCRule_Array_Sub[PQ_CSCRule_NUM_Sub][PQ_CSCRule_IP_NUM_Sub];

