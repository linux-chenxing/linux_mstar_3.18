#ifndef _DOLBYTYPETOMSTARTYPE_
#define _DOLBYTYPETOMSTARTYPE_
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#define int64_t S64 
#define int32_t S32 
#define int16_t S16
#define uint16_t U16 
#define uint32_t U32 
#define uint64_t U64

//#define INT32_MIN   0x80000000 
//#define INT32_MAX   0x7fffffff
//#define UINT32_MAX  0xffffffff 
//#define INT64_MIN   0x8000000000000000
//define INT64_MAX   0x7fffffffffffffff

#define INT32_MAX         INT_MAX
#define INT32_MIN         INT_MIN
#define UINT32_MAX        UINT_MAX
#define INT64_MAX         LONG_MAX
#define INT64_MIN         LONG_MIN

#define printf printk
#define malloc(x) kmalloc(x,GFP_KERNEL)
#define free kfree
#define fclose(...) //kfree
#define assert(...) 
#define fread(target,size,number,source) (number); memcpy(target, source, size * number); source+=(size*number)

#endif