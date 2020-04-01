#if (PQ_GRULE_NR_ENABLE)
typedef enum
{
#if PQ_GRULE_NR_ENABLE
    PQ_GRule_NR_Sub,
#endif

}
MST_GRule_Index_Sub;
#endif

#if PQ_GRULE_NR_ENABLE
typedef enum
{
    PQ_GRule_NR_Off_Sub,
    PQ_GRule_NR_Low_Sub,
    PQ_GRule_NR_Middle_Sub,
    PQ_GRule_NR_High_Sub,
}
MST_GRule_NR_Index_Sub;
#endif

#if PQ_GRULE_NR_ENABLE
typedef enum
{
    PQ_GRule_Lvl_NR_Off_Sub,
    PQ_GRule_Lvl_NR_Low_Sub,
    PQ_GRule_Lvl_NR_Middle_Sub,
    PQ_GRule_Lvl_NR_High_Sub,
}
MST_GRule_NR_LvL_Index_Sub;
#endif


#define PQ_GRULE_RULE_NUM_Sub 1

#if PQ_GRULE_NR_ENABLE
#define PQ_GRULE_NR_IP_NUM_Sub 3
#define PQ_GRULE_NR_NUM_Sub 4
#define PQ_GRULE_NR_LVL_NUM_Sub 4
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_NR_IP_Index_Sub[PQ_GRULE_NR_IP_NUM_Sub];
extern code U8 MST_GRule_NR_Sub[QM_INPUTTYPE_NUM_Sub][PQ_GRULE_NR_NUM_Sub][PQ_GRULE_NR_IP_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_PIP_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1920_POP_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_PIP_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

#if PQ_GRULE_NR_ENABLE
extern code U8 MST_GRule_1366_POP_NR_Sub[PQ_GRULE_NR_LVL_NUM_Sub];
#endif

