#define PQ_VSDRule_ID_Sub 1
#define PQ_VSDRule_IP_NUM_Sub 2
#define PQ_VSDRule_NUM_Sub 24


typedef enum
{
PQ_VSDRule_PreV_ScalingDown_Interlace_Sub,
PQ_VSDRule_PreV_ScalingDown_Progressive_Sub,
PQ_VSDRule_ScalingDown_00x_YUV_Sub,
PQ_VSDRule_ScalingDown_00x_RGB_Sub,
PQ_VSDRule_ScalingDown_01x_YUV_Sub,
PQ_VSDRule_ScalingDown_01x_RGB_Sub,
PQ_VSDRule_ScalingDown_02x_YUV_Sub,
PQ_VSDRule_ScalingDown_02x_RGB_Sub,
PQ_VSDRule_ScalingDown_03x_YUV_Sub,
PQ_VSDRule_ScalingDown_03x_RGB_Sub,
PQ_VSDRule_ScalingDown_04x_YUV_Sub,
PQ_VSDRule_ScalingDown_04x_RGB_Sub,
PQ_VSDRule_ScalingDown_05x_YUV_Sub,
PQ_VSDRule_ScalingDown_05x_RGB_Sub,
PQ_VSDRule_ScalingDown_06x_YUV_Sub,
PQ_VSDRule_ScalingDown_06x_RGB_Sub,
PQ_VSDRule_ScalingDown_07x_YUV_Sub,
PQ_VSDRule_ScalingDown_07x_RGB_Sub,
PQ_VSDRule_ScalingDown_08x_YUV_Sub,
PQ_VSDRule_ScalingDown_08x_RGB_Sub,
PQ_VSDRule_ScalingDown_09x_YUV_Sub,
PQ_VSDRule_ScalingDown_09x_RGB_Sub,
PQ_VSDRule_ScalingDown_10x_YUV_Sub,
PQ_VSDRule_ScalingDown_10x_RGB_Sub,
}
MST_VSDRule_Index_Sub;
extern code U8 MST_VSDRule_IP_Index_Sub[PQ_VSDRule_IP_NUM_Sub];
extern code U8 MST_VSDRule_Array_Sub[PQ_VSDRule_NUM_Sub][PQ_VSDRule_IP_NUM_Sub];

