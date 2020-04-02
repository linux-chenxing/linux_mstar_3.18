#include <linux/types.h>
#include <linux/string.h>



#define MMA_HEAP_NAME_LENG  32

//enable max mma areas be a large value .
#define MAX_MMA_AREAS 30

struct MMA_BootArgs_Config {
     int miu;//input :from bootargs or dts
     unsigned long size;//input :from bootargs or dts
     char name[MMA_HEAP_NAME_LENG];//input :from bootargs or dts

     phys_addr_t reserved_start;//out: reserved_start
};
