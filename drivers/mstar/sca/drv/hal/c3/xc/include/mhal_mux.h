// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// [mhal_mux.h]
// Date: 20090220
// Descriptions: Add a new mux layer for HW setting
//==============================================================================
#ifndef MHAL_MUX_H
#define MHAL_MUX_H

///////////////////////////////
//  Mux Hardware Option
///////////////////////////////
// Please set this #define after Mux tree is ready
#define MUX_TREE_HEIGHT 4

#define NUMBER_OF_ANALOG_PORT 3
#define NUMBER_OF_CVBS_PORT 11
#define NUMBER_OF_DVI_PORT 4//alex_tung
#define NUMBER_OF_MVOP_PORT 1
#define NUMBER_OF_ICP_PORT  1
#define NUMBER_OF_BT656_PORT 2
#define NUMBER_OF_SC_VOP_PORT 3
#define NUMBER_OF_CVBS_OUT_PORT 2
#define NUMBER_OF_YPBPR_OUT_PORT 0
#define PIP_SUPPORTED (MAX_WINDOW_NUM > 1)

#define HAL_DVI_IP_A    0 //alex_tung
#define HAL_DVI_IP_B    1
#define HAL_DVI_IP_C    2
#define HAL_DVI_IP_D    3

/*
    Input ports.
    It is interface between Hal and Driver level.
*/

typedef enum
{
    HAL_INPUTPORT_NONE = INPUT_PORT_NONE_PORT,

    HAL_INPUTPORT_ANALOG0 = INPUT_PORT_ANALOG0,
    HAL_INPUTPORT_ANALOG1,
    HAL_INPUTPORT_ANALOG2,

    HAL_INPUTPORT_ANALOG_SYNC0 = INPUT_PORT_ANALOG0_SYNC,
    HAL_INPUTPORT_ANALOG_SYNC1,
    HAL_INPUTPORT_ANALOG_SYNC2,
    HAL_INPUTPORT_ANALOG_SYNC3,

    HAL_INPUTPORT_YMUX_CVBS0 = INPUT_PORT_YMUX_CVBS0,
    HAL_INPUTPORT_YMUX_CVBS1,
    HAL_INPUTPORT_YMUX_CVBS2,
    HAL_INPUTPORT_YMUX_CVBS3,
    HAL_INPUTPORT_YMUX_CVBS4,
    HAL_INPUTPORT_YMUX_CVBS5,
    HAL_INPUTPORT_YMUX_CVBS6,
    HAL_INPUTPORT_YMUX_CVBS7,
    HAL_INPUTPORT_YMUX_G0,
    HAL_INPUTPORT_YMUX_G1,
    HAL_INPUTPORT_YMUX_G2,

    HAL_INPUTPORT_CMUX_CVBS0 = INPUT_PORT_CMUX_CVBS0,
    HAL_INPUTPORT_CMUX_CVBS1,
    HAL_INPUTPORT_CMUX_CVBS2,
    HAL_INPUTPORT_CMUX_CVBS3,
    HAL_INPUTPORT_CMUX_CVBS4,
    HAL_INPUTPORT_CMUX_CVBS5,
    HAL_INPUTPORT_CMUX_CVBS6,
    HAL_INPUTPORT_CMUX_CVBS7,
    HAL_INPUTPORT_CMUX_R0,
    HAL_INPUTPORT_CMUX_R1,
    HAL_INPUTPORT_CMUX_R2,

    HAL_INPUTPORT_DVI0 = INPUT_PORT_DVI0,
    HAL_INPUTPORT_DVI1,
    HAL_INPUTPORT_DVI2,
    HAL_INPUTPORT_DVI3,//alex_tung

    HAL_INPUTPORT_MVOP = INPUT_PORT_MVOP,

    HAL_INPUTPORT_ICP = INPUT_PORT_ICP,

    HAL_INPUTPORT_BT656_0 = INPUT_PORT_BT656_0,
    HAL_INPUTPORT_BT656_1,

    HAL_INPUTPORT_SC0_VOP = INPUT_PORT_SC0_VOP,
    HAL_INPUTPORT_SC1_VOP,
    HAL_INPUTPORT_SC2_VOP,
} E_INPUT_PORT_TYPE;

// Output ports.


typedef enum
{
    HAL_OUTPUTPORT_NONE_PORT = OUTPUT_PORT_NONE_PORT,

    HAL_OUTPUTPORT_SCALER_MAIN_WINDOW = OUTPUT_PORT_SCALER_MAIN_WINDOW,
    HAL_OUTPUTPORT_SCALER1_MAIN_WINDOW = OUTPUT_PORT_SCALER1_MAIN_WINDOW,
    HAL_OUTPUTPORT_SCALER2_MAIN_WINDOW = OUTPUT_PORT_SCALER2_MAIN_WINDOW,

    HAL_OUTPUTPORT_SCALER_SUB_WINDOW1 = OUTPUT_PORT_SCALER_SUB_WINDOW1,
    HAL_OUTPUTPORT_SCALER2_SUB_WINDOW1 = OUTPUT_PORT_SCALER2_SUB_WINDOW,


    HAL_OUTPUTPORT_CVBS1 = OUTPUT_PORT_CVBS1,
    HAL_OUTPUTPORT_CVBS2 ,

    HAL_OUTPUTPORT_YPBPR1 = OUTPUT_PORT_YPBPR1,

}E_OUTPUT_PORT_TYPE;


typedef enum
{
    SC_MAINWIN_IPMUX_ADC_A       = 0,           ///< ADC A
    SC_MAINWIN_IPMUX_HDMI_DVI    = 1,           ///< DVI
    SC_MAINWIN_IPMUX_VD          = 2,           ///< VD
    SC_MAINWIN_IPMUX_MVOP        = 3,           ///< MPEG/DC0
    SC_MAINWIN_IPMUX_SC_VOP      = 4,           ///< Scaler VOP output
    SC_MAINWIN_IPMUX_EXT_VD      = 5,           ///< External VD
    SC_MAINWIN_IPMUX_ADC_B       = 6,           ///< ADC B
    SC_MAINWIN_IPMUX_CAPTURE_SC0 = 7,           ///< capture SC0
    SC_MAINWIN_IPMUX_CAPTURE_SC1 = 10,          ///< capture SC1
    SC_MAINWIN_IPMUX_CAPTURE_SC2 = 13,          ///< capture SC2
    SC_MAINWIN_IPMUX_SENSOR_ICP  = 14,          ///< sensor ICP
    SC_MAINWIN_IPMUX_EXT_VD2     = 15,          ///< External VD2
    SC_MAINWIN_IPMUX_NONE        = 8,           ///< NONE
} SC_MAINWIN_IPMUX_TYPE;

typedef enum
{
    SC_SUBWIN_IPMUX_ADC_A       = 0,           ///< ADC A
    SC_SUBWIN_IPMUX_HDMI_DVI    = 1,           ///< DVI
    SC_SUBWIN_IPMUX_VD          = 2,           ///< VD
    SC_SUBWIN_IPMUX_MVOP        = 3,           ///< MPEG/DC0
    SC_SUBWIN_IPMUX_SC_VOP      = 4,           ///< Scaler VOP output
    SC_SUBWIN_IPMUX_EXT_VD      = 5,           ///< External VD
    SC_SUBWIN_IPMUX_ADC_B       = 6,           ///< ADC B
    SC_SUBWIN_IPMUX_CAPTURE_SC0 = 7,           ///< capture SC0
    SC_SUBWIN_IPMUX_MLINK       = 8,           ///< MLINK
    SC_SUBWIN_IPMUX_CAPTURE_SC1 = 10,          ///< capture SC0
    SC_SUBWIN_IPMUX_CAPTURE_SC2 = 13,          ///< capture SC2
    SC_SUBWIN_IPMUX_SENSOR_ICP  = 14,          ///< sensor ICP
    SC_SUBWIN_IPMUX_EXT_VD2     = 15,          ///< External VD2
    SC_SUBWIN_IPMUX_NONE        = 8,           ///< NONE
} SC_SUBWIN_IPMUX_TYPE;

typedef enum
{
    SC_OFFLINE_IPMUX_ADC_A      = 0,            ///< ADC A
    SC_OFFLINE_IPMUX_HDMI_DVI   = 1,            ///< DVI
    SC_OFFLINE_IPMUX_VD         = 2,            ///< VD
    SC_OFFLINE_IPMUX_MVOP       = 3,            ///< MPEG/DC0
    SC_OFFLINE_IPMUX_SC_VOP     = 4,            ///< Scaler VOP output
    SC_OFFLINE_IPMUX_EXT_VD     = 5,            ///< External VD
    SC_OFFLINE_IPMUX_ADC_B      = 6,            ///< ADC B
} SC_OFFLINE_IPMUX_TYPE;

void Hal_SC_mux_dispatch(E_MUX_INPUTPORT src , E_MUX_OUTPUTPORT dest);
void Hal_SC_mux_set_dvi_mux(MS_U8 PortId);
void Hal_SC_mux_set_adc_y_mux(MS_U8 PortId);
void Hal_SC_mux_set_adc_c_mux(MS_U8 PortId);
void Hal_SC_mux_set_adc_AV_ymux(MS_U8 PortId);
void Hal_SC_set_sync_port_by_dataport(E_MUX_INPUTPORT src_port );
MS_BOOL Hal_SC_mux_get_mainwin_ip_mux( MS_U8 *pU8Data_Mux, MS_U8 *pU8Clk_Mux);
void Hal_SC_set_subwin_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux);
void Hal_SC_mux_set_mainwin_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux);
void Hal_SC_set_ip_mux( MS_U8 u8Data_Mux, MS_U8 u8Clk_Mux, E_OUTPUT_PORT_TYPE eDest);
MS_BOOL Hal_SC_get_ip_mux( MS_U8 *pU8Data_Mux, MS_U8 *pU8Clk_Mux, E_OUTPUT_PORT_TYPE eDest);
void Hal_BT656test_bus( void );

#endif // MHAL_MUX_H
