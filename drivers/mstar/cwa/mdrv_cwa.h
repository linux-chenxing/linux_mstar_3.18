#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/mutex.h>

#include "drv_cwa.h"
#include "hal_cwa.h"
#include "hal_ceva.h"
#include "hal_clk.h"
#include "hal_debug.h"

#include "mdrv_cwa_io_st.h"

#ifndef _MDRV_CWA_H_
#define _MDRV_CWA_H_

#define BANK_CAL(addr)   ( (addr<<9)   + (RIU_BASE_ADDR) )

//for debug
#define LOG_WARP_TIMING    (1)
#define CHECK_REGISTER  (0)
#define DUMP_REGISTER   (0)

// Defines reference kern levels of printfk
#define CWA_MSG_ERR     3
#define CWA_MSG_WRN     4
#define CWA_MSG_DBG     5

#define CWA_MSG_LEVL    CWA_MSG_WRN

///////////////////////////////////////////////////////////////////////////////////////////////////
#define CWA_MSG_ENABLE

#if defined(CWA_MSG_ENABLE)
#define CWA_MSG_FUNC_ENABLE

#define CWA_STRINGIFY(x) #x
#define CWA_TOSTRING(x) CWA_STRINGIFY(x)

#if defined(CWA_MSG_FUNC_ENABLE)
#define CWA_MSG_TITLE   "[CWA, %s] "
#define CWA_MSG_FUNC    __func__
#else   // NOT defined(CWA_MSG_FUNC_ENABLE)
#define CWA_MSG_TITLE   "[CWA] %s"
#define CWA_MSG_FUNC    ""
#endif  // NOT defined(CWA_MSG_FUNC_ENABLE)

#define CWA_ASSERT(_con)   \
    do {\
        if (!(_con)) {\
            printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                    __FILE__, __LINE__, #_con);\
            BUG();\
        }\
    } while (0)

#define CWA_MSG(dbglv, _fmt, _args...)                          \
    do if(dbglv <= CWA_MSG_LEVL) {                              \
        printk(KERN_SOH CWA_TOSTRING(dbglv) CWA_MSG_TITLE  _fmt, CWA_MSG_FUNC, ## _args);   \
    } while(0)

#else   // NOT defined(CWA_MSG_ENABLE)
#define     CWA_ASSERT(arg)
#define     CWA_MSG(dbglv, _fmt, _args...)
#endif  // NOT defined(CWA_MSG_ENABLE)


///////////////////////////////////////////////////////////////////////////////////////////////////
// Driver Data Structure
//
//   Enum and structure of CWA dirver
//   We declare enum & structure here because enum and structures are
//   shared in all three layers (mdrv, drv, and hal)
//
//   It is not good way for modulization, but most members of structure are not pointer
//   because it can simplify the memory managent
//   and container_of() is a key funtion to access data strucure in operators
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * There are 2 state enumeration
 *   CWA_DRV_STATE: indicate the state of HW
 *   CWA_FILE_STATE: indicate the state of File
 *
 *   File and HW has different state because it support multi instance
 *   and file request may be queued if HW is busy
 */
typedef enum
{
    CWA_DRV_STATE_READY         = 0,
    CWA_DRV_STATE_PROCESSING    = 1,
    CWA_DRV_STATE_DONE          = 2
} CWA_DRV_STATE;

typedef enum
{
    CWA_FILE_STATE_READY        = 0,
    CWA_FILE_STATE_PROCESSING   = 1,
    CWA_FILE_STATE_DONE          = 2,
    CWA_FILE_STATE_IN_QUEUE  = 3,
    CWA_FILE_STATE_AXI_ERROR = 4
} CWA_FILE_STATE;

typedef union
{
    //cwa_ioc_coeff_ncc       ncc_buffer;
    __u8                    map_buffer[256];
} cwa_work_buffer;

/// @brief Data structure for driver
typedef struct
{
    struct platform_device  *pdev;          // platform device data
    cwa_hal_handle          hal_handle;         // HAL handle for real HW configuration
    ceva_hal_handle   ceva_hal_handle;
    s32                   ceva_irq_target;
    s32               ceva_irq;
    CWA_DRV_STATE           dev_state;    // HW state
    struct list_head        request_list;   // request list to queue waiting requst
    cwa_work_buffer         *work_buffer;  // working buffer
    dma_addr_t              work_buffer_dma;// working buffer pysical address
} cwa_drv_handle;


// Device data
typedef struct
{
    struct platform_device  *pdev;          // platform device data
    struct cdev             cdev;           // character device
    struct clk              **clk;          // clock
    s32                     clk_num;        // clock number
    u32            irq;            // IRQ number
    cwa_drv_handle          drv_handle;     // device handle
    struct work_struct      work_queue;     // work queue for post process after ISR
    struct mutex            mutex;          // for critical section
} cwa_dev_data;


// File private data
typedef struct
{
    cwa_dev_data            *dev_data;      // Device data
    cwa_ioc_config          ioc_config;     // IO configuation, i.e. one device file can service one request at the same time
    CWA_FILE_STATE          state;          // File state
    wait_queue_head_t       wait_queue;     // Wait queue for polling operation
    cwa_ioc_config          *user_io_config;// IO configuation pointer from user space
    cwa_work_buffer         work_buffer;
} cwa_file_data;

#endif
