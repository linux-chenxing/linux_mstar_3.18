#ifndef _IPA_PRIV_H_
#define _IPA_PRIV_H_

enum RET_VALUE{
    RET_OK = 0,
    Error_Invalid_Para = -1,
    Error_Node_Exist = -2,
    Error_No_Mem = -3,
    Error_Mem_Invalid = -4,
    Error_No_MIU_Prot_index = -5,
    Error_Unknown = -6
};

#define HEAP_TYPE_SHIFT 6
#define MAX_MIU_NUM     3

#define PA_SPACE_MIU0 0
#define PA_SPACE_MIU1 8
#define PA_SPACE_MIU2 16

#define MTLB_CMA_MIU0 1
#define MTLB_CMA_MIU1 9
#define MTLB_CMA_MIU2 17

#define IPA_CMA_MIU0  2
#define IPA_CMA_MIU1  10
#define IPA_CMA_MIU2  18

#endif
