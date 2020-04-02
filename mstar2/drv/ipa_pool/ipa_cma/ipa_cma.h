///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   ipa_cma.h
/// @brief  cma buffer and miu protect management
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __IPA_CMA_H__
#define __IPA_CMA_H__

//FIXME: may be not 4 by different chip
#define MIU_BLOCK_NUM 4

typedef enum
{
    MIU_BLOCK_IDLE = 0,
    MIU_BLOCK_BUSY
}MIU_PROTECT_BLOCK_STATUS;

struct cma_buffer{
    bool freed;             //freed by upper layer, but because miu hardware limitation, this buffer temply is hold by driver
    struct page * page;     //first page to buffer
    pid_t pid;              //who has this buffer

    unsigned long start_pa; //cpu bus address of this buffer
    unsigned long length;
    struct list_head list;  
};

//maintain the cma buffer list, all is allocated from system(may has hole) 
struct cma_alloc_list{
    unsigned long min_start;    //min of all start of cma buffers
    unsigned long max_end;      //max of all end of cma buffers
    unsigned long using_count;  //using buffer count
    unsigned long freed_count;  //freed buffer count: freed buffer must located at middle of list (hardware limitation, hold by driver temporary)
    struct list_head list_head; //all buffers in this list
};

//one miu protect entry, which protect a chunk of memory range
typedef struct
{
    unsigned char miuBlockIndex;    //index of miu protcet block
    unsigned long start_pa;         //kernel memory start cpu address
    unsigned long length;           //kernel memory length
    struct list_head list_node;
}MIU_ProtectRange;

//miu protcet blocks using status
struct MIU_ProtectRanges
{
    unsigned char miu;              //which miu
    MIU_PROTECT_BLOCK_STATUS miuBlockStatus[MIU_BLOCK_NUM]; //miu protect block status
    
    unsigned int krange_num;        //using count, totally 4 block
    struct list_head list_head;     //list of used
    struct mutex lock;              //different heaps may point to one miu, different processes access one struct. this for different heaps in one miu
};

//all info about cma heap & miu protect status 
//it as parameters register with IPA pool 
struct ipa_cma_para {
    int miu;
    char *name;
    struct device *dev;
    int ipa_id;
    
    struct MIU_ProtectRanges *miu_pro_sta;  //this miu kernel protect using status
    struct cma_alloc_list cma_buf_list;     //this ipa cma heap  buffer allocation status

    struct mutex ipa_cma_mutex;             //miu_pro_sta & cma_buf_list mutual access and consistency for one heap
};

typedef enum
{
    CMA_ALLOC = 0,
    CMA_FREE
}BUFF_OPS;


#define PHYSICAL_START_INIT     UL(-1)
#define PHYSICAL_END_INIT       0
#define INVALID_MIU             0xFF
#define INVALID_PID             0
#define MAX_ALLOC_TRY           30
#define MIU_PROTECT_ENABLE      1
#define MIU_PROTECT_DISABLE     0
#define IPA_CMA_KERNEL_PROT_DUMP    1
#define MIU_PROT_ALIGN_SIZE     8192  //miu protect limitation

#endif

