#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include <linux/clk.h>
#include <linux/clk-provider.h>

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>

#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>

#include <linux/delay.h>

#include <linux/time.h>

#include "ms_platform.h"

#include "mdrv_cwa.h"
#include "drv_cwa.h"
#include "hal_cwa.h"

#define CWA_MAX_IMAGE_WIDTH     8189
#define CWA_MIN_IMAGE_WIDTH     16
#define CWA_MAX_IMAGE_HEIGHT        8189
#define CWA_MIN_IMAGE_HEIGHT        16
#define COEF_PREC               12
#define WS_COEF_PREC            24
#define ADDRESS_ALIGNMENT_VALUE 16
#define ALIGNMENT_MASK          (ADDRESS_ALIGNMENT_VALUE - 1)
#define CHECK_ALIGNEMT(a)       (((u32)(a) & ALIGNMENT_MASK) != 0)
#define ABS(a)   ((a)>(0) ? (a) : (-(a)))

typedef struct
{
    struct list_head        list;
    cwa_file_data           *file_data;
} cwa_request_data;

#if LOG_WARP_TIMING
struct timespec gT_start;
struct timespec gT_end;
#endif
/*******************************************************************************************************************
 * cwa_drv_start
 *   check config status before HW engine run
 *
 * Parameters:
 *   handle: device handle
 *   config: CWA configurations
 *
 * Return:
 *  CWA_IOC_ERROR
 */
CWA_IOC_ERROR drv_cwa_check_config(cwa_ioc_config *p_config)
{
    cwa_image_data_t* p_input_data;
    cwa_image_data_t* p_output_data;
    cwa_image_desc_t* p_input_image;     //< Input image
    cwa_image_desc_t* p_output_image;    //< Output image

    cwa_displacement_table_t* p_disp_table;
    bound_box_st*p_bound_box;
    s32* p_coeff; //< Perspective transform coefficients
    cwa_operation_mode_e op;    //warp mode, perspective or disp. map

    p_input_data = (cwa_image_data_t*)&p_config->input_data;
    p_output_data = (cwa_image_data_t*)&p_config->output_data;
    p_input_image = (cwa_image_desc_t*)&p_config->input_image;
    p_output_image = (cwa_image_desc_t*)&p_config->output_image;

    p_disp_table = (cwa_displacement_table_t*)&p_config->disp_table;
    p_bound_box = (bound_box_st*)&p_config->bb_table;
    p_coeff = (s32*)&p_config->coeff;
    op = (cwa_operation_mode_e)p_config->op_mode;

    //check config null or not
    if (    NULL == p_config ||
        NULL == p_input_data || NULL == p_output_data ||
        NULL == p_disp_table ||
        NULL == p_bound_box
       )
    {
        return CWA_STATUS_PARAM_ERROR;
    }


    //check image format
    if (p_input_image->format != p_output_image->format)
    {
        return CWA_STATUS_PARAM_ERROR;
    }

    //check image size : input and output
    if (  CWA_MIN_IMAGE_WIDTH  > p_input_image->width  ||
        CWA_MIN_IMAGE_HEIGHT > p_input_image->height ||
        CWA_MAX_IMAGE_WIDTH  < p_input_image->width  ||
        CWA_MAX_IMAGE_HEIGHT < p_input_image->height
       )
    {
        return CWA_STATUS_PARAM_ERROR;
    }
    if (  CWA_MIN_IMAGE_WIDTH  > p_output_image->width  ||
        CWA_MIN_IMAGE_HEIGHT > p_output_image->height ||
        CWA_MAX_IMAGE_WIDTH  < p_output_image->width  ||
        CWA_MAX_IMAGE_HEIGHT < p_output_image->height
       )
    {
        return CWA_STATUS_PARAM_ERROR;
    }

    //check image buffer aligned or not
    if (  CHECK_ALIGNEMT(p_input_data->p_data[CWA_IMAGE_PLANE_RGBA]) ||
        CHECK_ALIGNEMT(p_output_data->p_data[CWA_IMAGE_PLANE_RGBA]) //RGBA or Y
        )
    {
        return CWA_STATUS_ADDRESS_NOT_ALIGNED_ERROR;
    }
    if (p_config->input_image.format != CWA_IMAGE_FORMAT_RGBA)              //UV
    {
        if (CHECK_ALIGNEMT(p_input_data->p_data[CWA_IMAGE_PLANE_UV]) ||
                CHECK_ALIGNEMT(p_output_data->p_data[CWA_IMAGE_PLANE_UV]))
        {
            return CWA_STATUS_ADDRESS_NOT_ALIGNED_ERROR;
        }
    }

    switch(op)
    {
        case CWA_OPERATION_MODE_MAP:
            //check disp. table aligned or not
            if (CHECK_ALIGNEMT(p_disp_table->tab.overlay))
            {
                return CWA_STATUS_ADDRESS_NOT_ALIGNED_ERROR;
            }
            break;

        case CWA_OPERATION_MODE_PERSPECTIVE:

            //check persepective value range
            if ( ABS(p_coeff[CWA_PERSPECTIVE_C00]) >= (8 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C01]) >= (8 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C10]) >= (8 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C11]) >= (8 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C22]) >= (8 << WS_COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C20]) >= (1 << WS_COEF_PREC)>>9 ) // 1.0/512
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C21]) >= (1 << WS_COEF_PREC)>>9 ) // 1.0/512
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C02]) > (8191 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            else if ( ABS(p_coeff[CWA_PERSPECTIVE_C12]) > (8191 << COEF_PREC) )
            {
                return CWA_STATUS_MATRIX_VALUES_ERROR;
            }
            break;

        default :
                return CWA_STATUS_OP_ERROR;
            break;
    }//end switch(op)


    ////HW engine status flg
    //if( is_wrp_started )
    //{
    //  return CWA_STATUS_HW_UNAVAILABLE;
    //}

    return CWA_STATUS_OK;

}
/*******************************************************************************************************************
 * cwa_drv_start
 *   Start HW engine
 *
 * Parameters:
 *   handle: device handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
static void cwa_drv_start(cwa_drv_handle *handle, cwa_ioc_config *p_config)
{
    cwa_hal_handle* hal_handle;     // HAL handle for real HW configuration
    cwa_operation_mode_e op_mode;

    hal_handle = (cwa_hal_handle *)&handle->hal_handle;
    op_mode = p_config->op_mode;

    CWA_MSG(CWA_MSG_DBG, "op_mode: %d\n", op_mode ); //disp. map or perspective

#if DUMP_REGISTER
    CWA_MSG(CWA_MSG_DBG, "base address = 0x%08x\n", hal_handle->base_addr); //base address
#endif

    ///cwa_hal_get_hw_status((cwa_hal_handle *)hal_handle);

    //set AXI bus
    cwa_hal_set_AXI((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set image buffer pointer
    cwa_hal_set_image_point((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set output tile size
    cwa_hal_set_output_tile((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set image size
    cwa_hal_set_image_size((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //point to displacement map
    cwa_hal_set_disp((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //point to B.B table
    cwa_hal_set_bb((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set perspective transform coefficient matrix
    cwa_hal_set_pers_matirx((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set out of range pixel fill value
    cwa_hal_set_out_of_range((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    //set configure, and enable hw engine
    cwa_hal_set_config((cwa_hal_handle *)hal_handle, (cwa_ioc_config*)p_config);

    ///cwa_hal_get_hw_status((cwa_hal_handle *)hal_handle);

}
/*******************************************************************************************************************
 * drv_cwa_extract_request
 *   Extract a request from waiting list
 *   The request is removed from list
 *
 * Parameters:
 *   ive_drv_handle: driver handle
 *
 * Return:
 *   File data of IVE driver
 */
static cwa_file_data* drv_cwa_extract_request(cwa_drv_handle *handle)
{
    struct platform_device *pdev = handle->pdev;
    cwa_request_data *list_data = NULL;
    cwa_file_data    *file_data = NULL;

    //check resuest list is empty.
    if (list_empty(&handle->request_list))
    {
        return NULL;
    }
    //get data from list
    list_data = list_first_entry(&handle->request_list, cwa_request_data, list);
    if (list_data != NULL)
    {
        list_del(handle->request_list.next);    //delete first data in list
        file_data = list_data->file_data;
        CWA_MSG(CWA_MSG_DBG, "extract: 0x%p, 0x%p\n", list_data, file_data);
        devm_kfree(&pdev->dev, list_data);
    }
    else
    {
        CWA_MSG(CWA_MSG_DBG, "extract: 0x%p\n", list_data);
    }

    return file_data;
}
/*******************************************************************************************************************
 * drv_cwa_get_request
 *   Get a request from waiting list
 *   The request is still kpet in list
 *
 * Parameters:
 *   cwa_drv_handle: driver handle
 *
 * Return:
 *   File data of CWA driver
 */
static cwa_file_data* drv_cwa_get_request(cwa_drv_handle *handle)
{
    cwa_request_data *list_data = NULL;

    //check if request list is empty
    if (list_empty(&handle->request_list))
    {
        return NULL;
    }

    //get data from request list
    list_data = list_first_entry(&handle->request_list, cwa_request_data, list);
    if (list_data != NULL)
    {
        CWA_MSG(CWA_MSG_DBG, "get: 0x%p, 0x%p\n", list_data, list_data->file_data);
        return list_data->file_data;
    }
    else
    {
        CWA_MSG(CWA_MSG_DBG, "get: 0x%p\n", list_data);
    }

    return NULL;
}
/*******************************************************************************************************************
 * drv_cwa_add_request
 *   Add a request to waiting list
 *
 * Parameters:
 *   cwa_drv_handle: driver handle
 *   file_data: File data of CWA driver
 *
 * Return:
 *   none
 */
static void drv_cwa_add_request(cwa_drv_handle *handle, cwa_file_data *file_data)
{
    struct platform_device *pdev = handle->pdev;
    cwa_request_data *list_data;

    //setup request data
    list_data = devm_kcalloc(&pdev->dev, 1, sizeof(cwa_request_data), GFP_KERNEL);
    list_data->file_data = file_data;

    CWA_MSG(CWA_MSG_DBG, "add: 0x%p, 0x%p\n", list_data, file_data);

    //add request data to list
    if (list_data != NULL)
    {
        list_add_tail(&list_data->list, &handle->request_list);
    }
}
/*******************************************************************************************************************
 * cwa_drv_process
 *   Start CWA process image
 *
 * Parameters:
 *   handle: device handle
 *
 * Return:
 *   CWA_IOC_ERROR
 */
CWA_IOC_ERROR cwa_drv_process(cwa_drv_handle *handle, cwa_file_data *file_data)
{
    CWA_IOC_ERROR ret = CWA_STATUS_OK;

    //Check the configuration, such as image, disp. table, ......
    ret = drv_cwa_check_config(&file_data->ioc_config);

    if(ret != CWA_STATUS_OK)
    {
        return ret;
    }

    //add a request to linked list
    file_data->state = CWA_FILE_STATE_IN_QUEUE;
    drv_cwa_add_request(handle, file_data);

    // do nothing if hw is not ready
    if (handle->dev_state != CWA_DRV_STATE_READY)
    {
        CWA_MSG(CWA_MSG_DBG, "HW is busy\n");
        return CWA_STATUS_OK;
    }

    //get a request from liked list
    file_data = drv_cwa_get_request(handle);

    if (file_data == NULL) //no more request in queue
    {
        CWA_MSG(CWA_MSG_DBG, "no more request in queue\n");
        return CWA_STATUS_MEMORY_FAILURE;
    }

    //run warp
    file_data->state = CWA_FILE_STATE_PROCESSING;
    handle->dev_state = CWA_DRV_STATE_PROCESSING;

    CWA_MSG(CWA_MSG_DBG, "process: %p\n", file_data->ioc_config.input_data.p_data[0]);

    //set miu bus
    ceva_hal_set_axi2miu(&handle->ceva_hal_handle);

    //enable ceva warp of wrapper
    ceva_hal_enable_warp(&handle->ceva_hal_handle);

#if LOG_WARP_TIMING
    getnstimeofday(&gT_start);
#endif
    //enable warp
    cwa_drv_start(handle, &file_data->ioc_config);//start HW engine

    return ret;
}

/*******************************************************************************************************************
 * cwa_drv_check
 *   Check status register for poll.
 *
 * Parameters:
 *   handle: device handle
 *
 * Return:
 *   CWA_IOC_ERROR
 */
CWA_IOC_ERROR cwa_drv_check(cwa_drv_handle *handle)
{
    CWA_IOC_ERROR ret = CWA_STATUS_OK;

    cwa_hal_handle* p_hal_handle;     // HAL handle for real HW configuration
    //cwa_ioc_config* p_config;
    u32 oc_status = 0;
    u32 axi_status = 0;

    p_hal_handle = (cwa_hal_handle *)&handle->hal_handle;

    //read hardware status
    cwa_hal_get_hw_status((cwa_hal_handle*) p_hal_handle);

    oc_status = p_hal_handle->reg_bank.sta.fields.oc;
    axi_status  = p_hal_handle->reg_bank.sta.fields.axierr;

    //if( !( (oc_status&1) || (axi_status&1) ) )
    if( !( oc_status & 1)  )
    {
        ret = CWA_STATUS_HW_UNAVAILABLE;
    }
    else
    {
        ret = CWA_STATUS_OK;
    }

    return ret;

}
/*******************************************************************************************************************
 * cwa_drv_isr_handler
 *   ISR handler, check and clear ISR
 *
 * Parameters:
 *   irq:    interrupt number
 *   handle: device handle
 *
 * Return:
 *   None
 */
CWA_DRV_STATE  cwa_drv_isr_handler(s32 irq, cwa_drv_handle *handle)
{
    u32 irq_status;
    u32 irq_ceva;

#if LOG_WARP_TIMING
    getnstimeofday(&gT_end);
    CWA_MSG(CWA_MSG_WRN, "Warp proc %lld us\n", (long long)gT_end.tv_sec * 1000000 + (long long)gT_end.tv_nsec/1000 - (long long)gT_start.tv_sec*1000000 - (long long)gT_start.tv_nsec/1000);
#endif

    //get warp IRQ status from target.
    irq_ceva = (u32)ceva_hal_get_irq(&handle->ceva_hal_handle, handle->ceva_irq_target);//CEVA_HAL_IRQ_TARGET_ARM

    //check and clear IRQ status
    cwa_hal_get_hw_status(&handle->hal_handle);         //read status register (read /clear)

    irq_status = (u32)handle->hal_handle.reg_bank.sta.fields.oc;//operation complete bit

    if( (irq_ceva == handle->ceva_irq) //CEVA_HAL_IRQ_WARP
            &&
      irq_status)
    {
        handle->dev_state = CWA_DRV_STATE_DONE;

        return CWA_DRV_STATE_DONE;
    }

    return CWA_DRV_STATE_PROCESSING;
}
/*******************************************************************************************************************
 * cwa_drv_copy_work_buffer
 *   Copy work buffer from drv to file data
 *
 * Parameters:
 *   handle: device handle
 *   work_buffer: destination
 *
 * Return:
 *   none
 */
void cwa_drv_copy_work_buffer(cwa_drv_handle *handle, cwa_work_buffer *work_buffer)
{
    memcpy(work_buffer, handle->work_buffer, sizeof(cwa_work_buffer));
}
/*******************************************************************************************************************
 * cwa_drv_post_process
 *   Post process after CWA HW done
 *
 * Parameters:
 *   handle: device handle
 *
 * Return:
 *   cwa_file_data : processed file_data
 */
cwa_file_data* cwa_drv_post_process(cwa_drv_handle *handle)
{
    cwa_file_data* pre_filedata = NULL;
    cwa_file_data* cur_filedata = NULL;
    u32 axi_state = 0;

    //reset ceva warp of wrapper
    ceva_hal_reset_warp(&handle->ceva_hal_handle);

    //get processed file_data from request list, then delete it in the list.
    pre_filedata = drv_cwa_extract_request(handle);

    //get current file_data from request list, it will be used by cwa_dev_start().
    cur_filedata = drv_cwa_get_request(handle);

    //copy work_buffer of handle to pre_filedata's (processed work buffer)
    if (pre_filedata != NULL)
    {
         cwa_drv_copy_work_buffer(handle, &(pre_filedata->work_buffer)); //copy working buffer, handle to  pre_filedata
         pre_filedata->state = CWA_FILE_STATE_DONE;
    }//end if (pre_filedata != NULL)

    //read AXI bus error bit of satus register
    axi_state = handle->hal_handle.reg_bank.sta.fields.axierr ;
    if(axi_state )
    {
         pre_filedata->state = CWA_FILE_STATE_AXI_ERROR;
    }

    if (cur_filedata == NULL)
    {
         CWA_MSG(CWA_MSG_DBG, "no more request in queue\n");
         handle->dev_state = CWA_DRV_STATE_READY;
    }
    else
    {
         //run warp
         cur_filedata->state = CWA_FILE_STATE_PROCESSING;
         handle->dev_state = CWA_DRV_STATE_PROCESSING;
         CWA_MSG(CWA_MSG_DBG, "process: %p\n", cur_filedata->ioc_config.input_data.p_data[0]);

         //set miu bus
         ceva_hal_set_axi2miu(&handle->ceva_hal_handle);

         //enable ceva warp of wrapper
         ceva_hal_enable_warp(&handle->ceva_hal_handle);

         //enable warp
         cwa_drv_start(handle, &cur_filedata->ioc_config);
    }

    return pre_filedata;
}
/*******************************************************************************************************************
 * cwa_drv_release
 *   Release CWA settings
 *
 * Parameters:
 *   handle: device handle
 *
 * Return:
 *   none
 */
void cwa_drv_release(cwa_drv_handle *handle)
{
    devm_kfree(&handle->pdev->dev, handle->work_buffer);
}
/*******************************************************************************************************************
 * cwa_drv_init
 *   Init CWA settings
 *
 * Parameters:
 *   handle: device handle
 *   pdev:          platform device
 *   base_addr:    base addr of HW register bank
 *
 * Return:
 *   none
 */
s32 cwa_drv_init(cwa_drv_handle *handle, struct platform_device *pdev, phys_addr_t base_addr, phys_addr_t base_sys, phys_addr_t base_axi2miu0, phys_addr_t base_axi2miu1, phys_addr_t base_axi2miu2, phys_addr_t base_axi2miu3)
{

    CWA_MSG(CWA_MSG_DBG, "name: %s, addr 0x%08X\n", pdev->name, base_addr);

    memset(handle, 0, sizeof(cwa_drv_handle));

    //initialize hal_handle, setting physical address
    cwa_hal_init(&handle->hal_handle, base_addr);
    cava_hal_init(&handle->ceva_hal_handle, base_sys, base_axi2miu0, base_axi2miu1, base_axi2miu2, base_axi2miu3);

    //set warp IRQ for ceva wrapper
    handle->ceva_irq_target = CEVA_HAL_IRQ_TARGET_ARM;
    handle->ceva_irq =  CEVA_HAL_IRQ_WARP;

    //initialize request list
    INIT_LIST_HEAD(&handle->request_list);
    CWA_MSG(CWA_MSG_DBG, "list_empty = %d\n", list_empty(&handle->request_list));

    handle->pdev = pdev;
    handle->dev_state = CWA_DRV_STATE_READY;

    // create working buffer
    // Here we assume kmalloc() has 16 byte alignment
    handle->work_buffer = devm_kmalloc(&handle->pdev->dev, sizeof(cwa_work_buffer), GFP_KERNEL);
    if (handle->work_buffer == NULL)
    {
        CWA_MSG(CWA_MSG_ERR, "can't allocate working buffer\n");
        return -ENOMEM; //?
    }

    CWA_MSG(CWA_MSG_DBG, "work buffer = 0x%p\n", handle->work_buffer);
    return 0;
}
/*******************************************************************************************************************
 * cwa_drv_enable_irq
 *   Enable warp IRQ of wrapper
 *
 * Parameters:
 *   handle: device handle
 *
 * Return:
 *   none
 */
s32 cwa_drv_enable_irq(cwa_drv_handle *handle)
{
    CWA_MSG(CWA_MSG_DBG, "enable IRQ\n");
    ceva_hal_enable_irq(&handle->ceva_hal_handle,  handle->ceva_irq_target,  handle->ceva_irq);
    return 0;
}
