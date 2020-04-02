#ifndef __ENV__HANDLE_DEV_H__
#define __ENV__HANDLE_DEV_H__

typedef struct  
{            
        int rw_block_size;
        int erase_block_size;
        int erase_block_count;
}device_info;

typedef struct 
{   
        int block_id;
        int offset;
}read_write_pos;

typedef struct
{
     read_write_pos pos;
     char* data;
}read_write_struct;

#define ENV_HANDLER_MAGIC 55
#define ENV_HANDLER_GET_DEVICE_INFO _IOR(ENV_HANDLER_MAGIC,1,device_info )
#define ENV_HANDLER_READ _IOR(ENV_HANDLER_MAGIC,2,read_write_struct)
#define ENV_HANDLER_WRITE _IOW(ENV_HANDLER_MAGIC,3,read_write_struct)
#define ENV_HANDLER_ERASE _IOW(ENV_HANDLER_MAGIC,4, int)

#endif

