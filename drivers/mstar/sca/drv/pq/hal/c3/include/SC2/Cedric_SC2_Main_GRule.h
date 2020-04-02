#if (PQ_GRULE_NR_ENABLE) || (PQ_GRULE_OSD_BW_ENABLE) || (PQ_GRULE_MPEG_NR_ENABLE) || (PQ_GRULE_FILM_MODE_ENABLE) || (PQ_GRULE_DYNAMIC_CONTRAST_ENABLE) || (PQ_GRULE_DDR_SELECT_ENABLE) || (PQ_GRULE_PTP_ENABLE)
typedef enum
{
#if PQ_GRULE_NR_ENABLE
    PQ_GRule_NR_SC2_Main,
#endif

#if PQ_GRULE_OSD_BW_ENABLE
    PQ_GRule_OSD_BW_SC2_Main,
#endif

#if PQ_GRULE_MPEG_NR_ENABLE
    PQ_GRule_MPEG_NR_SC2_Main,
#endif

#if PQ_GRULE_FILM_MODE_ENABLE
    PQ_GRule_FILM_MODE_SC2_Main,
#endif

#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
    PQ_GRule_DYNAMIC_CONTRAST_SC2_Main,
#endif

#if PQ_GRULE_DDR_SELECT_ENABLE
    PQ_GRule_DDR_SELECT_SC2_Main,
#endif

#if PQ_GRULE_PTP_ENABLE
    PQ_GRule_PTP_SC2_Main,
#endif

}
MST_GRule_Index_SC2_Main;
#endif

#if PQ_GRULE_NR_ENABLE
typedef enum
{
    PQ_GRule_NR_Off_SC2_Main,
    PQ_GRule_NR_Low_SC2_Main,
    PQ_GRule_NR_Middle_SC2_Main,
    PQ_GRule_NR_High_SC2_Main,
    PQ_GRule_NR_Auto_Low_L_SC2_Main,
    PQ_GRule_NR_Auto_Low_M_SC2_Main,
    PQ_GRule_NR_Auto_Low_H_SC2_Main,
    PQ_GRule_NR_Auto_Middle_L_SC2_Main,
    PQ_GRule_NR_Auto_Middle_M_SC2_Main,
    PQ_GRule_NR_Auto_Middle_H_SC2_Main,
    PQ_GRule_NR_Auto_High_L_SC2_Main,
    PQ_GRule_NR_Auto_High_M_SC2_Main,
    PQ_GRule_NR_Auto_High_H_SC2_Main,
    PQ_GRule_NR_Low_PIP_SC2_Main,
    PQ_GRule_NR_Middle_PIP_SC2_Main,
    PQ_GRule_NR_High_PIP_SC2_Main,
}
MST_GRule_NR_Index_SC2_Main;
#endif

#if PQ_GRULE_OSD_BW_ENABLE
typedef enum
{
    PQ_GRule_OSD_BW_On_1920_SC2_Main,
    PQ_GRule_OSD_BW_On_1920_PIP_SC2_Main,
    PQ_GRule_OSD_BW_On_1920_POP_SC2_Main,
    PQ_GRule_OSD_BW_On_1366_SC2_Main,
    PQ_GRule_OSD_BW_On_1366_PIP_SC2_Main,
    PQ_GRule_OSD_BW_On_1366_POP_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1920_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1920_PIP_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1920_POP_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1366_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1366_PIP_SC2_Main,
    PQ_GRule_OSD_BW_2DDR_On_1366_POP_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1920_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1920_PIP_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1920_POP_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1366_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1366_PIP_SC2_Main,
    PQ_GRule_OSD_BW_1DDR_On_1366_POP_SC2_Main,
}
MST_GRule_OSD_BW_Index_SC2_Main;
#endif

#if PQ_GRULE_MPEG_NR_ENABLE
typedef enum
{
    PQ_GRule_MPEG_NR_Off_SC2_Main,
    PQ_GRule_MPEG_NR_Low_SC2_Main,
    PQ_GRule_MPEG_NR_Middle_SC2_Main,
    PQ_GRule_MPEG_NR_High_SC2_Main,
}
MST_GRule_MPEG_NR_Index_SC2_Main;
#endif

#if PQ_GRULE_FILM_MODE_ENABLE
typedef enum
{
    PQ_GRule_FILM_MODE_Off_SC2_Main,
    PQ_GRule_FILM_MODE_On_SC2_Main,
}
MST_GRule_FILM_MODE_Index_SC2_Main;
#endif

#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
typedef enum
{
    PQ_GRule_DYNAMIC_CONTRAST_Off_SC2_Main,
    PQ_GRule_DYNAMIC_CONTRAST_On_SC2_Main,
}
MST_GRule_DYNAMIC_CONTRAST_Index_SC2_Main;
#endif

#if PQ_GRULE_DDR_SELECT_ENABLE
typedef enum
{
    PQ_GRule_DDR_SELECT_2DDR_1920_SC2_Main,
    PQ_GRule_DDR_SELECT_2DDR_1920_PIP_SC2_Main,
    PQ_GRule_DDR_SELECT_2DDR_1920_POP_SC2_Main,
    PQ_GRule_DDR_SELECT_2DDR_1366_SC2_Main,
    PQ_GRule_DDR_SELECT_2DDR_1366_PIP_SC2_Main,
    PQ_GRule_DDR_SELECT_2DDR_1366_POP_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1920_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1920_PIP_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1920_POP_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1366_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1366_PIP_SC2_Main,
    PQ_GRule_DDR_SELECT_1DDR_1366_POP_SC2_Main,
}
MST_GRule_DDR_SELECT_Index_SC2_Main;
#endif

#if PQ_GRULE_PTP_ENABLE
typedef enum
{
    PQ_GRule_PTP_On_PTP_SC2_Main,
}
MST_GRule_PTP_Index_SC2_Main;
#endif

#if PQ_GRULE_NR_ENABLE
typedef enum
{
    PQ_GRule_Lvl_NR_Off_SC2_Main,
    PQ_GRule_Lvl_NR_Low_SC2_Main,
    PQ_GRule_Lvl_NR_Middle_SC2_Main,
    PQ_GRule_Lvl_NR_High_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Low_L_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Low_M_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Low_H_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Middle_L_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Middle_M_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_Middle_H_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_High_L_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_High_M_SC2_Main,
    PQ_GRule_Lvl_NR_Auto_High_H_SC2_Main,
}
MST_GRule_NR_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_OSD_BW_ENABLE
typedef enum
{
    PQ_GRule_Lvl_OSD_BW_On_SC2_Main,
    PQ_GRule_Lvl_OSD_BW_2DDR_On_SC2_Main,
    PQ_GRule_Lvl_OSD_BW_1DDR_On_SC2_Main,
}
MST_GRule_OSD_BW_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_MPEG_NR_ENABLE
typedef enum
{
    PQ_GRule_Lvl_MPEG_NR_Off_SC2_Main,
    PQ_GRule_Lvl_MPEG_NR_Low_SC2_Main,
    PQ_GRule_Lvl_MPEG_NR_Middle_SC2_Main,
    PQ_GRule_Lvl_MPEG_NR_High_SC2_Main,
}
MST_GRule_MPEG_NR_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_FILM_MODE_ENABLE
typedef enum
{
    PQ_GRule_Lvl_FILM_MODE_Off_SC2_Main,
    PQ_GRule_Lvl_FILM_MODE_On_SC2_Main,
}
MST_GRule_FILM_MODE_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
typedef enum
{
    PQ_GRule_Lvl_DYNAMIC_CONTRAST_Off_SC2_Main,
    PQ_GRule_Lvl_DYNAMIC_CONTRAST_On_SC2_Main,
}
MST_GRule_DYNAMIC_CONTRAST_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_DDR_SELECT_ENABLE
typedef enum
{
    PQ_GRule_Lvl_DDR_SELECT_2DDR_SC2_Main,
    PQ_GRule_Lvl_DDR_SELECT_1DDR_SC2_Main,
}
MST_GRule_DDR_SELECT_LvL_Index_SC2_Main;
#endif

#if PQ_GRULE_PTP_ENABLE
typedef enum
{
    PQ_GRule_Lvl_PTP_On_SC2_Main,
}
MST_GRule_PTP_LvL_Index_SC2_Main;
#endif


#define PQ_GRULE_RULE_NUM_SC2_Main 7

#if PQ_GRULE_NR_ENABLE
#define PQ_GRULE_NR_IP_NUM_SC2_Main 9
#define PQ_GRULE_NR_NUM_SC2_Main 16
#define PQ_GRULE_NR_LVL_NUM_SC2_Main 13
#endif

#if PQ_GRULE_OSD_BW_ENABLE
#define PQ_GRULE_OSD_BW_IP_NUM_SC2_Main 2
#define PQ_GRULE_OSD_BW_NUM_SC2_Main 18
#define PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main 3
#endif

#if PQ_GRULE_MPEG_NR_ENABLE
#define PQ_GRULE_MPEG_NR_IP_NUM_SC2_Main 7
#define PQ_GRULE_MPEG_NR_NUM_SC2_Main 4
#define PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main 4
#endif

#if PQ_GRULE_FILM_MODE_ENABLE
#define PQ_GRULE_FILM_MODE_IP_NUM_SC2_Main 3
#define PQ_GRULE_FILM_MODE_NUM_SC2_Main 2
#define PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main 2
#endif

#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
#define PQ_GRULE_DYNAMIC_CONTRAST_IP_NUM_SC2_Main 1
#define PQ_GRULE_DYNAMIC_CONTRAST_NUM_SC2_Main 2
#define PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main 2
#endif

#if PQ_GRULE_DDR_SELECT_ENABLE
#define PQ_GRULE_DDR_SELECT_IP_NUM_SC2_Main 1
#define PQ_GRULE_DDR_SELECT_NUM_SC2_Main 12
#define PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main 2
#endif

#if PQ_GRULE_PTP_ENABLE
#define PQ_GRULE_PTP_IP_NUM_SC2_Main 2
#define PQ_GRULE_PTP_NUM_SC2_Main 1
#define PQ_GRULE_PTP_LVL_NUM_SC2_Main 1
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_NR_IP_Index_SC2_Main[PQ_GRULE_NR_IP_NUM_SC2_Main];
extern code U8 MST_GRule_NR_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_NR_NUM_SC2_Main][PQ_GRULE_NR_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_OSD_BW_IP_Index_SC2_Main[PQ_GRULE_OSD_BW_IP_NUM_SC2_Main];
extern code U8 MST_GRule_OSD_BW_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_OSD_BW_NUM_SC2_Main][PQ_GRULE_OSD_BW_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_MPEG_NR_IP_Index_SC2_Main[PQ_GRULE_MPEG_NR_IP_NUM_SC2_Main];
extern code U8 MST_GRule_MPEG_NR_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_MPEG_NR_NUM_SC2_Main][PQ_GRULE_MPEG_NR_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_FILM_MODE_IP_Index_SC2_Main[PQ_GRULE_FILM_MODE_IP_NUM_SC2_Main];
extern code U8 MST_GRule_FILM_MODE_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_FILM_MODE_NUM_SC2_Main][PQ_GRULE_FILM_MODE_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_DYNAMIC_CONTRAST_IP_Index_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_IP_NUM_SC2_Main];
extern code U8 MST_GRule_DYNAMIC_CONTRAST_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_DYNAMIC_CONTRAST_NUM_SC2_Main][PQ_GRULE_DYNAMIC_CONTRAST_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_DDR_SELECT_IP_Index_SC2_Main[PQ_GRULE_DDR_SELECT_IP_NUM_SC2_Main];
extern code U8 MST_GRule_DDR_SELECT_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_DDR_SELECT_NUM_SC2_Main][PQ_GRULE_DDR_SELECT_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_PTP_IP_Index_SC2_Main[PQ_GRULE_PTP_IP_NUM_SC2_Main];
extern code U8 MST_GRule_PTP_SC2_Main[QM_INPUTTYPE_NUM_SC2_Main][PQ_GRULE_PTP_NUM_SC2_Main][PQ_GRULE_PTP_IP_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1920_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1920_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1920_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1920_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1920_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1920_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_PIP_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1920_PIP_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1920_PIP_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1920_PIP_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1920_PIP_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1920_PIP_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1920_PIP_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_POP_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1920_POP_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1920_POP_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1920_POP_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1920_POP_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1920_POP_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1920_POP_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1366_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1366_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1366_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1366_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1366_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1366_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_PIP_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1366_PIP_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1366_PIP_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1366_PIP_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1366_PIP_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1366_PIP_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1366_PIP_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_POP_NR_SC2_Main[PQ_GRULE_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_OSD_BW_ENABLE
extern code U8 MST_GRule_1366_POP_OSD_BW_SC2_Main[PQ_GRULE_OSD_BW_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_MPEG_NR_ENABLE
extern code U8 MST_GRule_1366_POP_MPEG_NR_SC2_Main[PQ_GRULE_MPEG_NR_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_FILM_MODE_ENABLE
extern code U8 MST_GRule_1366_POP_FILM_MODE_SC2_Main[PQ_GRULE_FILM_MODE_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DYNAMIC_CONTRAST_ENABLE
extern code U8 MST_GRule_1366_POP_DYNAMIC_CONTRAST_SC2_Main[PQ_GRULE_DYNAMIC_CONTRAST_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_DDR_SELECT_ENABLE
extern code U8 MST_GRule_1366_POP_DDR_SELECT_SC2_Main[PQ_GRULE_DDR_SELECT_LVL_NUM_SC2_Main];
#endif
#if PQ_GRULE_PTP_ENABLE
extern code U8 MST_GRule_1366_POP_PTP_SC2_Main[PQ_GRULE_PTP_LVL_NUM_SC2_Main];
#endif

