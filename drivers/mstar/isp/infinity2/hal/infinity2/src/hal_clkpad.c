////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <errno.h>
//#include <signal.h>
#include <string.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <hal_isp.h>
#include <hal_clkpad.h>
#include <isp_types.h>
#include <hal_clkpad.h>
//#include "drv_csi.h"
//#include <drv_clkgen_cmu.h>
#include <kernel_chiptop.h>
#include <kernel_padtop.h>
#include <isp_log.h>
#include <isp_sys_api.h>

#define TAG ("Build Info:"__DATE__"|"__TIME__"|"__FILE__)

#define CLKPAD_DBG 0
#if CLKPAD_DBG
#define CLKPAD_DMSG(args...) LOGD(args)
#define CLKPAD_EMSG(args...) LOGE(args)
#define CLKPAD_VMSG(args...) LOGV(args)
#else
#define CLKPAD_DMSG(args...) do{}while(0)
#define CLKPAD_EMSG(args...) UartSendTrace(args)
#define CLKPAD_VMSG(args...) UartSendTrace(args)
#endif

//volatile csimac_cfg_t *csimac_cfg;
//volatile csidphy_cfg_t *csidphy_cfg;

typedef struct
{
    u8  reg_clk_en      : 1;
    u8  reg_clk_invert  : 1;
    u8  reg_clk_sel     : 5;
}ClkCtrl_t;

#define OFFSET_CLKGEN_ISPSC_FCLK1   0
#define OFFSET_CLKGEN_ISPSC_FCLK2   0
#define OFFSET_CLKGEN_ISPSC_ODCLK   0
#define OFFSET_CLKGEN_DMAGEN0       0
#define OFFSET_CLKGEN_DMAGEN1       0
#define OFFSET_CLKGEN_ISP_IMG       ((0x53<<2)+1)
#define OFFSET_CLKGEN_SR0_MCLK      (0x55<<2)
#define OFFSET_CLKGEN_SR1_MCLK      ((0x55<<2)+1)
#define OFFSET_CLKGEN_SR2_MCLK      (0x56<<2)
#define OFFSET_CLKGEN_SR3_MCLK      ((0x56<<2)+1)

int HalClkgenCtrl(ClkgenId_e nId,u32 nFreq,u8 nEn)
{
    ClkCtrl_t volatile *clk = 0;
    u32 nBank;
    u32 nOffset;

    switch(nId)
    {
    case CLKGEN_ID_ISP_IMG:
        nBank   = RIUBASE_CLKGEN2;
        nOffset = OFFSET_CLKGEN_ISP_IMG;
    break;
    case CLKGEN_ID_SR0_MCLK:
        nBank   = RIUBASE_CLKGEN2;
        nOffset = OFFSET_CLKGEN_SR0_MCLK;
    break;
    case CLKGEN_ID_SR1_MCLK:
        nBank   = RIUBASE_CLKGEN2;
        nOffset = OFFSET_CLKGEN_SR1_MCLK;
    break;
    case CLKGEN_ID_SR2_MCLK:
        nBank   = RIUBASE_CLKGEN2;
        nOffset = OFFSET_CLKGEN_SR2_MCLK;
    break;
    case CLKGEN_ID_SR3_MCLK:
        nBank   = RIUBASE_CLKGEN2;
        nOffset = OFFSET_CLKGEN_SR3_MCLK;
    break;
    default:
        return -1;
    break;
    }

    clk = (volatile ClkCtrl_t *)(BANK_BASE_ADDR(nBank) + nOffset);

    if(nEn)
        clk->reg_clk_en = 1;
    else
        clk->reg_clk_en = 0;

    clk->reg_clk_sel = nFreq;

    return 0;
}
#if 0
void HalClkpad_Set_ISP_Clk(int ion, int iclkidx, int iratehz)
{
#if 0 //RTK clkgen driver.
    //return; //test only

    if(ion == 0){
        DrvClkgenDrvClockCtl(CLKGEN_DRV_ISP, CLKGEN_CLK_REQ_OFF);
        return;
    }

    DrvClkgenDrvClockCtl(CLKGEN_DRV_ISP, CLKGEN_CLK_REQ_ON);

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, TOP_CLK_ISP, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, TOP_CLK_ISP, iratehz);
    }

#else
    //TODO: Use clock tree
    clk_isp volatile *isp = (volatile clk_isp *)(BANK_BASE_ADDR(RIUBASE_CLKGEN) + OFFSET_CLKGEN_TO_CLK_ISP);
    /*
    00: 123.4 MHz
    01: 86.4 MHz
    10: 72 MHz
    11: 54 MHz
    100: 144MHz
    */
    if(clk == ISP_CLK_OFF)
    {
        isp->reg_ckg_icp_en = 1;    //disable
    }
    else
    {
        isp->reg_ckg_icp_en = 0;    //enable
        isp->reg_ckg_icp = clk; // 72M
    }
#endif
}
#endif

#if 0
void HalClkpad_Set_SR_MCLK(int isensor_num, int ion, int iclkidx, int iratehz)
{
#if 0 //RTK
    //return; //test only

    ClkgenClkUpdate_e sclkgen_sr_mclk =TOP_CLK_SR0_MCLK;

    switch(isensor_num){
        case 0:
            sclkgen_sr_mclk =TOP_CLK_SR0_MCLK;
            break;
        case 1:
            sclkgen_sr_mclk =TOP_CLK_SR1_MCLK;
            break;
        default:
            //Error
            break;
    }

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, sclkgen_sr_mclk, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, sclkgen_sr_mclk, iratehz);
    }

#else
#if 0
    clk_sensor volatile *sensor = (volatile clk_sensor *)(BANK_BASE_ADDR(RIUBASE_CLKGEN) + OFFSET_CLKGEN_TO_CLK_SENSOR);
    /*
    [4:2]: Select clock source
    000: 27 MHz
    001: 21.6 MHz
    010: xtal (12) MHz
    011: 5.4 MHz
    100: 36 MHz
    101: 54 MHz
    110: 43.2 MHz
    111: 61.7 MHz"
    */
    switch(mclk)
    {
        case SR_ISP_MCLK_5P4M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_5P4M; //5.4MHz
            break;
        case SR_ISP_MCLK_12M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_12M; //12MHz
            break;
        case SR_ISP_MCLK_21P6M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_21P6M; //21.6MHz
            break;
        case SR_ISP_MCLK_27M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_27M; //27MHz
            break;
        case SR_ISP_MCLK_36M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_36M; //36MHz
            break;
        case SR_ISP_MCLK_54M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_54M; //54MHz
            break;
        case SR_ISP_MCLK_61P7M:
            sensor->reg_ckg_sr_mclk_select_clock_source = SEN_MAIN_CLK_61P7M; //61.7MHz
            break;
        default:
            CLKPAD_EMSG("Unknown sensor MCLK.\n");
            break;
    }

    //UartSendTrace("[%s]Show  sensor->reg_ckg_sr_mclk_select_clock_source  = 0x%x \n",__FUNCTION__, sensor->reg_ckg_sr_mclk_select_clock_source);
    if(bONOFF == 0)
        sensor->reg_ckg_sr_mclk_disable_clock = 1;
    else
        sensor->reg_ckg_sr_mclk_disable_clock = 0;
#endif
#endif
}
#endif

#if 0
void HalClkpad_Set_SR_PCLK(int isensor_num, int ion, int iclkidx, int iratehz)
{
#if 0 //RTK
    //return; //test only

    ClkgenClkUpdate_e sclkgen_sr_pclk =TOP_CLK_SR0;

    switch(isensor_num){
        case 0:
            sclkgen_sr_pclk =TOP_CLK_SR0;
            break;
        case 1:
            sclkgen_sr_pclk =TOP_CLK_SR1;
            break;
        case 2:
            sclkgen_sr_pclk =TOP_CLK_SR2;
            break;
        default:
            //Error
            break;
    }

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, sclkgen_sr_pclk, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, sclkgen_sr_pclk, iratehz);
    }

#else
#if 0
    clk_sensor volatile *sensor = (volatile clk_sensor *)(BANK_BASE_ADDR(RIUBASE_CLKGEN) + OFFSET_CLKGEN_TO_CLK_SENSOR);
    /*
    [0]: disable clock
    [1]: invert clock
    [3:2]: select clock source
    00: clk_sr from sr_pad
    01: clk_sr from mipi_atop
    10: 40MHz
    11: 86MHz
    */
    switch(pclk)
    {
        case ISP_PCLK_SR_PAD:
            sensor->reg_ckg_sr_select_clock_source = 0x00;
            break;
        case ISP_PCLK_MIPI_TOP:
            sensor->reg_ckg_sr_select_clock_source = 0x01;
            break;
        case ISP_PCLK_40HZ:
            sensor->reg_ckg_sr_select_clock_source = 0x02;
            break;
        case ISP_PCLK_86HZ:
            sensor->reg_ckg_sr_select_clock_source = 0x03;
            break;
        default:
            CLKPAD_EMSG("Unknown sensor PCLK.\n");
            break;
    }

    if(pclk == ISP_PCLK_OFF)
    {
        sensor->reg_ckg_sr_disable_clock = 1;
    }
    else
    {
        sensor->reg_ckg_sr_disable_clock = 0;
    }
#endif
#endif
}
#endif


void HalClkpad_Set_CSI_MAC_CLK(int isensor_num, int ion, int iclkidx, int iratehz)
{
#if 0 //RTK
    //return; //test only

    ClkgenClkUpdate_e sclkgen_csi_mac =TOP_CLK_CSI_MAC_0;

    switch(isensor_num){
        case 0:
            sclkgen_csi_mac =TOP_CLK_CSI_MAC_0;
            break;
        case 1:
            sclkgen_csi_mac =TOP_CLK_CSI_MAC_1;
            break;
        default:
            //Error
            break;
    }

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, sclkgen_csi_mac, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, sclkgen_csi_mac, iratehz);
    }

#else
#endif
}

void HalClkpad_Set_CSI_NS_CLK(int isensor_num, int ion, int iclkidx, int iratehz)
{
#if 0 //RTK
    //return; //test only

    ClkgenClkUpdate_e sclkgen_ns =TOP_CLK_NS_0;

    switch(isensor_num){
        case 0:
            sclkgen_ns =TOP_CLK_NS_0;
            break;
        case 1:
            sclkgen_ns =TOP_CLK_NS_1;
            break;
        default:
            //Error
            break;
    }

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, sclkgen_ns, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, sclkgen_ns, iratehz);
    }

#else
#endif
}

void HalClkpad_Set_CSI_LPTX_CLK(int isensor_num, int ion, int iclkidx, int iratehz)
{
#if 0 //RTK
    //return; //test only

    ClkgenClkUpdate_e sclkgen_lptx =TOP_CLK_MAC_LPTX_0;

    switch(isensor_num){
        case 0:
            sclkgen_lptx =TOP_CLK_MAC_LPTX_0;
            break;
        case 1:
            sclkgen_lptx =TOP_CLK_MAC_LPTX_1;
            break;
        default:
            //Error
            break;
    }

    if (iclkidx >= 0){
        DrvClkgenDrvClockSelectSrc(CLKGEN_DRV_ISP, sclkgen_lptx, iclkidx);
    }
    else{
        DrvClkgenDrvClockUpdate(CLKGEN_DRV_ISP, sclkgen_lptx, iratehz); //TBD
    }

#else
#endif
}

#if 0
void Select_SR_IOPad(u32 ulSnrPadNum, u32 ulSnrType, u32 ulSnrPadCfg)
{
#if 1 //RTK
    switch(ulSnrPadNum){
        case 0: //Sensor 0
            switch(ulSnrType){
                case 0: //parallel sensor
                    HalClkpad_Sensor0ParallelPad(ulSnrPadCfg);
                    break;
                case 0: //BT601 sensor
                    HalClkpad_Sensor0BT601Pad(ulSnrPadCfg);
                    break;
                case 0: //BT656 sensor
                    HalClkpad_Sensor0BT656Pad(ulSnrPadCfg);
                    break;
                case 0: //MIPI sensor
                    HalClkpad_Sensor0MIPIPad(ulSnrPadCfg);
                    break;
                default:
                    //Error
                    break;
            }
            break;
        case 1:  //Sensor 1
            switch(ulSnrType){
                case 0: //BT656 sensor
                    HalClkpad_Sensor1BT656Pad(ulSnrPadCfg);
                    break;
                case 0: //MIPI sensor
                    HalClkpad_Sensor1MIPIPad(ulSnrPadCfg);
                    break;
                default:
                    //Error
                    break;
            }
            break;
        default:
            //Error
            break;
    }

#else
    //chiptop_sensor_mode *sensor_mode = (chiptop_sensor_mode *)(MsIoMapGetRiuBase(RIUBASE_CHIPTOP) + OFFSET_CHIPTOP_TO_SENSOR_MODE);
    chiptop_sensor_mode *sensor_mode = (chiptop_sensor_mode *)(BANK_BASE_ADDR(RIUBASE_CHIPTOP) + OFFSET_CHIPTOP_TO_SENSOR_MODE);
    sensor_mode->reg_sr_mode     = cfg;
#endif
}
#endif

#if 0 //Move to drv_csi.c
void Set_csi_lane(u16 lane)
{
    volatile csimac_cfg_t* MAC_address;
    //MAC_address = ((volatile csimac_cfg_t*)MsIoMapGetRiuBase(RIUBASE_CSI_MAC));//1204
    MAC_address = (volatile csimac_cfg_t*) BANK_BASE_ADDR(RIUBASE_CSI_MAC);//1204

#if defined(__MV5_FPGA__)
    return;
#endif

    if(lane == 1)
        MAC_address->reg_csimac_00 = 0x0246;
    else if(lane == 2)
        MAC_address->reg_csimac_00 = 0x1e4e;//0x064e;
}

void Set_csi_vc0_hs_mode(u16 vc0_hs_mode)
{
    volatile csimac_cfg_t* MAC_address;
    //MAC_address = ((volatile csimac_cfg_t*)MsIoMapGetRiuBase(RIUBASE_CSI_MAC));//1204
    MAC_address = (volatile csimac_cfg_t*)BANK_BASE_ADDR(RIUBASE_CSI_MAC);//1204
    MAC_address->reg_csimac_01 &= (vc0_hs_mode & 0x3);
}

void Set_csi_long_packet(u64 ctl_cfg)
{
    volatile csimac_cfg_t* MAC_address;
    //MAC_address = ((volatile csimac_cfg_t*)MsIoMapGetRiuBase(RIUBASE_CSI_MAC));//1204
    MAC_address = (volatile csimac_cfg_t*)BANK_BASE_ADDR(RIUBASE_CSI_MAC);//1204
    MAC_address->reg_csimac_1D = (ctl_cfg & 0xFFFF);
    MAC_address->reg_csimac_1E = ((ctl_cfg >> 16) & 0xFFFF);
    MAC_address->reg_csimac_1F = ((ctl_cfg >> 32) & 0xFFFF);
}

void Set_csi_clk_data_skip(u8 rx_clk_skip_ns, u8 rx_data_skip_ns)
{
    //volatile csidphy_cfg_t* DPHY_address = ((volatile csidphy_cfg_t*)MsIoMapGetRiuBase(RIUBASE_DPHY_CSI_ANA));//1202
    volatile csidphy_cfg_t* DPHY_address = (volatile csidphy_cfg_t*)BANK_BASE_ADDR(RIUBASE_DPHY_CSI_ANA);//1202
    DPHY_address->reg_csidphy_16 = (rx_data_skip_ns << 7) | rx_clk_skip_ns;
}

void Set_csi_if(bool enable, CSI_CLK clk)
{
#define RIUBASE_CHIPTOP (0x101E) //Remove it later
#define RIUBASE_CLKGEN (0x1038) //Remove it later

    //volatile csimac_cfg_t* MAC_address;
    //volatile csidphy_cfg_t* DPHY_address;
    //volatile csidphy_cfg_t* DIG_address;
    volatile clk_mac* CLKGEN_address;
    volatile clk_sensor* CLKGEN_Sensor_address;
    volatile chiptop_sensor_pad_control* CHIPTOP_address;
    int i = 0;

    CLKGEN_address = (volatile clk_mac*)(BANK_BASE_ADDR(RIUBASE_CLKGEN) + OFFSET_CLKGEN_TO_CLK_MAC);//1038
    CLKGEN_Sensor_address = (volatile clk_sensor*)(BANK_BASE_ADDR(RIUBASE_CLKGEN) + OFFSET_CLKGEN_TO_CLK_SENSOR);//1038
    CHIPTOP_address = (volatile chiptop_sensor_pad_control*)(BANK_BASE_ADDR(RIUBASE_CHIPTOP) + OFFSET_CHIPTOP_TO_SENSOR_PAD);//101e

    if(enable == TRUE)
    {
        isp_pr_info("Set_csi_if\r\n");

        //reset CSI phy
    }
    else
    {

        isp_pr_info("Set parallel interface\r\n");

        //*(CLKGEN_address+0x6c)=0x0404;
        CLKGEN_address->reg_ckg_csi_mac = 0x00;
        CLKGEN_address->reg_ckg_mac_lptx = 0x00;
        //*(CLKGEN_address+0x6b)=0x0004;
        CLKGEN_address->reg_ckg_ns = 0x0000;

        // *(CHIPTOP_address+0x24) = (*(CHIPTOP_address + 0x24) & ~0xFF ) | 0x0000;// disable pull high
        CHIPTOP_address->reg_sr_ie = (CHIPTOP_address->reg_sr_ie & ~0xFF) | 0x003f;
        //*(CHIPTOP_address+0x25) = (*(CHIPTOP_address + 0x25) & ~0xFF ) | 0x0000;// disable pull high
        CHIPTOP_address->reg_sr_pe = (CHIPTOP_address->reg_sr_pe & ~0xFF) | 0x003f;
        //*(CHIPTOP_address+0x26) = (*(CHIPTOP_address + 0x26) & ~0xFF ) | 0x0000;// disable pull high
        CHIPTOP_address->reg_sr_ps = (CHIPTOP_address->reg_sr_ps & ~0xFF) | 0x003f;

        //0x1038c4  0x0404
        CLKGEN_Sensor_address->reg_ckg_sr_select_clock_source = clk_sr_from_sr_pad;
        //*(CHIPTOP_address1+0x25)=0x003f;//pull low
    }
}
#endif

void HalClkpad_Sensor0ParallelPad(CHIPTOP_SR0_PAR_MODE_e ssr0_par_mode)
{
    volatile PADTOP1_t *pchiptop_handle = g_ptPADTOP1; //RTK
    pchiptop_handle->reg_snr0_in_mode = ssr0_par_mode;
}

void HalClkpad_Sensor0BT656Pad(CHIPTOP_SR0_BT656_MODE_e ssr0_bt656_mode)
{
    volatile PADTOP1_t *pchiptop_handle = g_ptPADTOP1; //RTK
    pchiptop_handle->reg_ccir0_8b_mode = ssr0_bt656_mode;
}

void HalClkpad_Sensor1BT656Pad(CHIPTOP_SR1_BT656_MODE_e ssr1_bt656_mode)
{
    volatile PADTOP1_t *pchiptop_handle = g_ptPADTOP1; //RTK
    pchiptop_handle->reg_ccir1_8b_mode = ssr1_bt656_mode;
}

void HalClkpad_Sensor0BT601Pad(CHIPTOP_SR0_BT601_MODE_e ssr0_bt601_mode)
{
    //CHIPTOP_t *pchiptop_handle = g_ptCHIPTOP; //RTK
    //pchiptop_handle->reg_sr0_bt601_mode = ssr0_bt601_mode;
}

void HalClkpad_Sensor0MIPIPad(CHIPTOP_SR0_MIPI_MODE_e ssr0_mipi_mode)
{
    //CHIPTOP_t *pchiptop_handle = g_ptCHIPTOP; //RTK
    //pchiptop_handle->reg_sr0_mipi_mode = ssr0_mipi_mode;
}

void HalClkpad_Sensor1MIPIPad(CHIPTOP_SR1_MIPI_MODE_e ssr1_mipi_mode)
{
    //CHIPTOP_t *pchiptop_handle = g_ptCHIPTOP; //RTK
    //pchiptop_handle->reg_sr1_mipi_mode = ssr1_mipi_mode;
}

void HalClkpad_I2CSetPad(CHIPTOP_I2C_SET_e si2c_set, CHIPTOP_I2C_MODE_e si2c_mode)
{
    volatile PADTOP1_t *pchiptop_handle = g_ptPADTOP1; //RTK
    switch(si2c_set){
        case CHIPTOP_I2C0:
            pchiptop_handle->reg_i2cm0_mode = si2c_mode;
            break;
        case CHIPTOP_I2C1DUAL:
            pchiptop_handle->reg_i2cm1_mode = CHIPTOP_I2C_MODE_3;
            break;
        case CHIPTOP_I2C2:
            pchiptop_handle->reg_i2cm2_mode = si2c_mode;
            break;
        case CHIPTOP_I2C3:
            pchiptop_handle->reg_i2cm3_mode = si2c_mode;
            break;
        default:
            //Error
            break;
    }
}
