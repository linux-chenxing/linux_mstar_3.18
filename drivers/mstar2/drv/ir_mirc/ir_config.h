#ifndef _IR_CONFIG_H_
#define _IR_CONFIG_H_
#include "mstar_ir.h"
#include "ir_core.h"
#include "ir_common.h"

/****************** For Customer Config  Start [can modify by customers] ********************/

//Number of IR should this chip supported
#define IR_SUPPORT_NUM 1

//Add & Modify Customer IR with Differ Headcode Here
static IR_Profile_t ir_config[IR_SUPPORT_NUM]=
{
    {IR_TYPE_NEC,NUM_KEYMAP_MSTAR_TV,0},           // Mstar IR customer code
    //{IR_TYPE_TOSHIBA,NUM_KEYMAP_SKYWORTH_TV,0},        //skyworth toshiba ir
    //{IR_TYPE_NEC,NUM_KEYMAP_CHANGHONG_TV,0},           // changhong_RL78B /Toshiba CT-90436 IR customer code
    //{IR_TYPE_NEC,NUM_KEYMAP_HISENSE_TV,0},           // Hisense IR customer code
    //{IR_TYPE_RCA,NUM_KEYMAP_TCL_RCA_TV,0},           // TCL RCA  customer code
    //{IR_TYPE_P7051,NUM_KEYMAP_P7051_STB,0},       // Panasonic 7051 IR customer code
    //{IR_TYPE_RC5,NUM_KEYMAP_RC5_TV,0},           // RC5 customer code
    //{IR_TYPE_RC6,NUM_KEYMAP_KATHREIN_TV,0},           //Kathrein RC6 customer code
};

//IR Debug level for customer setting
static IR_DBG_LEVEL_e ir_dbglevel = IR_DBG_ERR;

//IR Speed level for customer setting
static IR_SPEED_LEVEL_e ir_speed = IR_SPEED_FAST_H;

/****************** For Customer Config  End [can modify by customers] ********************/

#endif
