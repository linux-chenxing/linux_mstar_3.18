#include "mdrv_cwa.h"
#include "hal_cwa.h"
#include "ms_platform.h"

#if (!DUMP_REGISTER )
#define REGR(base,idx)      ms_readl(((uint)base+(idx)))
#define REGW(base,idx,val)  ms_writel(val,((uint)base+(idx)))
#else
#define REGR(base,idx)      ms_readl(((uint)base+(idx)))
#define REGW(base,idx,val)  do{CWA_MSG(CWA_MSG_DBG, "write 0x%08X = 0x%04X\n", ((uint)base+(idx)), val);} while(0)
#endif

/*******************************************************************************************************************
 * cwa_hal_AXI_set
 *   Set AXI bus
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_AXI(cwa_hal_handle* handle, cwa_ioc_config* config)
 {

    handle->reg_bank.axi_cfg2.fields.mwb= (u8)(config->axi_max_write_burst_size & 0xff  );
    handle->reg_bank.axi_cfg2.fields.mrb = (u8)(config->axi_max_read_burst_size   & 0xff);
    handle->reg_bank.axi_cfg2.fields.moutstw = (u8)(config->axi_max_write_outstanding & 0xff);
    handle->reg_bank.axi_cfg2.fields.moutstr = (u8)(config->axi_max_read_outstanding & 0xff);

    REGW(handle->base_addr,WARP_AXI_CFG2_REG_ADDR, handle->reg_bank.axi_cfg2.overlay); //0x0c,  AXI_CFG2

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_AXI_CFG2_REG_ADDR, handle->reg_bank.axi_cfg2.overlay);
#endif //#if (CHECK_REGISTER)

 }
/*******************************************************************************************************************
 * cwa_hal_image_set
 *   Set image buffer pointer
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_image_point(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    cwa_image_data_t*         p_input_data;
    cwa_image_data_t*         p_output_data;

    p_input_data = (cwa_image_data_t*)&config->input_data;
    p_output_data = (cwa_image_data_t*)&config->output_data;

    //point to image plane
    handle->reg_bank.dlua_yrgb = (s32)p_input_data->p_data[CWA_IMAGE_PLANE_Y];
    handle->reg_bank.dlua_uvg = (s32)p_input_data->p_data[CWA_IMAGE_PLANE_UV];
    handle->reg_bank.dsua_yrgb = (s32)p_output_data->p_data[CWA_IMAGE_PLANE_Y];
    handle->reg_bank.dsua_uvg = (s32)p_output_data->p_data[CWA_IMAGE_PLANE_UV];

    REGW(handle->base_addr,WARP_DLUA_YRGB_REG_ADDR, handle->reg_bank.dlua_yrgb );   //0x10, RGBA or Y base address of input
    REGW(handle->base_addr,WARP_DLUA_UV_REG_ADDR, handle->reg_bank.dlua_uvg );      //0x14, UV base address of input
    REGW(handle->base_addr,WARP_DSUA_YRGB_REG_ADDR, handle->reg_bank.dsua_yrgb );   //0x20, RGBA or Y base address of output
    REGW(handle->base_addr,WARP_DSUA_UV_REG_ADDR, handle->reg_bank.dsua_uvg );      //0x24, UV base address of output

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_DLUA_YRGB_REG_ADDR, handle->reg_bank.dlua_yrgb);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_DLUA_UV_REG_ADDR, handle->reg_bank.dlua_uvg);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_DSUA_YRGB_REG_ADDR, handle->reg_bank.dsua_yrgb);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_DSUA_UV_REG_ADDR, handle->reg_bank.dsua_uvg);

#endif //#if (CHECK_REGISTER)

 }
/*******************************************************************************************************************
 * cwa_hal_image_size_set
 *   Set image size
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_image_size(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    cwa_image_desc_t*          p_input_image;     //< Input image
    cwa_image_desc_t*          p_output_image;    //< Output image

    p_input_image = (cwa_image_desc_t*)&config->input_image;
    p_output_image = (cwa_image_desc_t*)&config->output_image;

    //point to image plane
    handle->reg_bank.ifsz.fields.ifszx = (u16)p_input_image->width;
    handle->reg_bank.ifsz.fields.ifszy = (u16)p_input_image->height;
    handle->reg_bank.ofsz.fields.ofszx= (u16)p_output_image->width;
    handle->reg_bank.ofsz.fields.ofszy = (u16)p_output_image->height;

    REGW(handle->base_addr,WARP_IFSZ_REG_ADDR, handle->reg_bank.ifsz.overlay);  //0x38, Input frame heigth and width
    REGW(handle->base_addr,WARP_OFSZ_REG_ADDR, handle->reg_bank.ofsz.overlay);  //0x3c, Output frame heigth and width

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_IFSZ_REG_ADDR, handle->reg_bank.ifsz.overlay);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_OFSZ_REG_ADDR, handle->reg_bank.ofsz.overlay);
#endif //#if (CHECK_REGISTER)

 }
/*******************************************************************************************************************
 * cwa_hal_output_tile_set
 *   Set output tile size
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_output_tile(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    handle->reg_bank.ocs.fields.ocsx = (u16)config->_output_tiles_width;
    handle->reg_bank.ocs.fields.ocsy = (u16)config->_output_tiles_height;

    REGW(handle->base_addr,WARP_OCS_REG_ADDR, handle->reg_bank.ocs.overlay);    //0x30, Output tile scanning block height and width

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_OCS_REG_ADDR, handle->reg_bank.ocs.overlay);
#endif //#if (CHECK_REGISTER)

 }
 /*******************************************************************************************************************
 * cwa_hal_set_disp
 *   Point to displacement  map
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_disp(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    cwa_displacement_table_t*  p_disp_table;      //< Displacement table descriptor

    p_disp_table = (cwa_displacement_table_t* )&config->disp_table;

    handle->reg_bank.distba = (s32)p_disp_table->tab.overlay;

    REGW(handle->base_addr,WARP_DISTBA_REG_ADDR, handle->reg_bank.distba ); //0x40, dist. table base address

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_DISTBA_REG_ADDR, handle->reg_bank.distba);
#endif //#if (CHECK_REGISTER)

 }
 /*******************************************************************************************************************
 * cwa_hal_set_bb
 *   Point to  bounding box table
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_bb(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    cwa_bound_box_table_t* p_bb_table;      //< Bounding box table descriptor

    p_bb_table = (cwa_bound_box_table_t* )&config->bb_table;

    handle->reg_bank.bba = (s32)p_bb_table->p_bb_tab;

    REGW(handle->base_addr,WARP_BBA_REG_ADDR, handle->reg_bank.bba );       //0x44, B.B base address

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_BBA_REG_ADDR, handle->reg_bank.bba);
#endif //#if (CHECK_REGISTER)

 }
  /*******************************************************************************************************************
 * cwa_hal_pers_matirx_set
 *   Set perspective transform coefficient
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
 void cwa_hal_set_pers_matirx(cwa_hal_handle* handle, cwa_ioc_config* config)
 {
    handle->reg_bank.c00 = config->coeff[CWA_PERSPECTIVE_C00];
    handle->reg_bank.c01 = config->coeff[CWA_PERSPECTIVE_C01];
    handle->reg_bank.c02 = config->coeff[CWA_PERSPECTIVE_C02];
    handle->reg_bank.c10 = config->coeff[CWA_PERSPECTIVE_C10];
    handle->reg_bank.c11 = config->coeff[CWA_PERSPECTIVE_C11];
    handle->reg_bank.c12 = config->coeff[CWA_PERSPECTIVE_C12];
    handle->reg_bank.c20 = config->coeff[CWA_PERSPECTIVE_C20];
    handle->reg_bank.c21 = config->coeff[CWA_PERSPECTIVE_C21];
    handle->reg_bank.c22 = config->coeff[CWA_PERSPECTIVE_C22];

    REGW(handle->base_addr,WARP_PDC_C00_REGS_ADDR, handle->reg_bank.c00 ); //0x48, perspective transform coefficient(0x48~0x68)
    REGW(handle->base_addr,WARP_PDC_C01_REGS_ADDR, handle->reg_bank.c01 );//0x4c
    REGW(handle->base_addr,WARP_PDC_C02_REGS_ADDR, handle->reg_bank.c02 );//0x50
    REGW(handle->base_addr,WARP_PDC_C10_REGS_ADDR, handle->reg_bank.c10 ); //0x54
    REGW(handle->base_addr,WARP_PDC_C11_REGS_ADDR, handle->reg_bank.c11 );//0x58
    REGW(handle->base_addr,WARP_PDC_C12_REGS_ADDR, handle->reg_bank.c12 );//0x5C
    REGW(handle->base_addr,WARP_PDC_C20_REGS_ADDR, handle->reg_bank.c20 ); //0x60
    REGW(handle->base_addr,WARP_PDC_C21_REGS_ADDR, handle->reg_bank.c21 );//0x64
    REGW(handle->base_addr,WARP_PDC_C22_REGS_ADDR, handle->reg_bank.c22 );//0x68

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C00_REGS_ADDR, handle->reg_bank.c00);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C01_REGS_ADDR, handle->reg_bank.c01);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C02_REGS_ADDR, handle->reg_bank.c02);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C10_REGS_ADDR, handle->reg_bank.c10);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C11_REGS_ADDR, handle->reg_bank.c11);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C12_REGS_ADDR, handle->reg_bank.c12);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C20_REGS_ADDR, handle->reg_bank.c20);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C21_REGS_ADDR, handle->reg_bank.c21);
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_PDC_C22_REGS_ADDR, handle->reg_bank.c22);
#endif //#if (CHECK_REGISTER)

 }
/*******************************************************************************************************************
 * cwa_hal_set_out_of_range
 *   Set out of range pixel fill value
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
void cwa_hal_set_out_of_range(cwa_hal_handle* handle, cwa_ioc_config* config)
{
    handle->reg_bank.csfv.overlay= *((u32*)(config->fill_val));
    REGW(handle->base_addr,WARP_CSFV_REG_ADDR, handle->reg_bank.csfv.overlay );//0x70, out of range pixel fill value

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_CSFV_REG_ADDR, handle->reg_bank.csfv.overlay);
#endif //#if (CHECK_REGISTER)
}
/*******************************************************************************************************************
 * cwa_hal_set_config
 *   Set configure
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *
 * Return:
 *
 */
void cwa_hal_set_config(cwa_hal_handle* handle, cwa_ioc_config* config)
{
//handle->reg_bank.ctl.overlay = 0x4205; //default
    if(config->op_mode == CWA_OPERATION_MODE_MAP)
    {
        handle->reg_bank.ctl.fields.dispm = CWA_OPERATION_MODE_MAP;
    }
    else
    {
        handle->reg_bank.ctl.fields.dispm = CWA_OPERATION_MODE_PERSPECTIVE;
    }

    handle->reg_bank.ctl.fields.dxym = config->disp_table.format;//0 is abs mode, 1 is relative mode
    handle->reg_bank.ctl.fields.eofie = 1;   //End of frame interrupt enable
    handle->reg_bank.ctl.fields.oie = 1;     //Output interrupt enable
    handle->reg_bank.ctl.fields.axierie = 1; //AXI error interrupt enable
    handle->reg_bank.ctl.fields.dtibsz = config->disp_table.resolution; //gride size : 8x8 or 16x16
    handle->reg_bank.ctl.fields.iif = config->input_image.format; //RGBA, YUV422 NV16 or YUV420 NV12
    handle->reg_bank.ctl.fields.bpdu = config->debug_bypass_displacement_en;  //Bypass Displacement unit (Debug hook)
    handle->reg_bank.ctl.fields.bpiu = config->debug_bypass_interp_en;    //Bypass Interpolation unit (Debug hook)
    handle->reg_bank.ctl.fields.go = 1; // WARP accelerator starts

    REGW(handle->base_addr,WARP_CTL_REG_ADDR, handle->reg_bank.ctl.overlay );//0x00, warp config setting

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGW: addr 0x%02X,  value 0x%08X\n", WARP_CTL_REG_ADDR, handle->reg_bank.ctl.overlay);
#endif //#if (CHECK_REGISTER)
}
/*******************************************************************************************************************
 * cwa_hal_start
 *   Enable hw engine
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *   en : enable hardware engine
 *
 * Return:
 *
 */
void cwa_hal_start(cwa_hal_handle* handle, uint en)
{
    handle->reg_bank.ctl.fields.go = (en & 1);

    REGW(handle->base_addr,WARP_CTL_REG_ADDR, handle->reg_bank.ctl.overlay );//0x00, warp config setting
}
/*******************************************************************************************************************
 * cwa_hal_get_hw_status
 *   Get hardware status
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   config: CWA configurations
 *   en : enable hardware engine
 *
 * Return:
 *
 */
void cwa_hal_get_hw_status(cwa_hal_handle* handle)
{
    u32 status = 0;
    status = (u32)REGR(handle->base_addr,WARP_STA_REG_ADDR);
    handle->reg_bank.sta.overlay = status;

#if (CHECK_REGISTER)
    CWA_MSG(CWA_MSG_DBG, "REGR: addr 0x%02X,  value 0x%08X\n", WARP_STA_REG_ADDR, handle->reg_bank.sta.overlay);
#endif //#if (CHECK_REGISTER)
}
/*******************************************************************************************************************
 * cwa_hal_init
 *   init cwa HAL layer
 *
 * Parameters:
 *   handle: CWA HAL handle
 *   base_addr: base address
 *
 * Return:
 *   none
 */
void cwa_hal_init(cwa_hal_handle *handle, phys_addr_t base_addr)
{
    memset(handle, 0, sizeof(cwa_hal_handle));
    handle->base_addr = base_addr;
}
