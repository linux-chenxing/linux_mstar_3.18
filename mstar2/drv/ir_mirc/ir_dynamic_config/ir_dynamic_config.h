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
/// file    ir_dynamic_config.h
/// @brief  Load IR config from INI config file
/// @author Vick.Sun@MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef __IR_DYNAMIC_CONFIG__
#define __IR_DYNAMIC_CONFIG__

#include <linux/input.h>

/***********************************************************************************/
//&step2:  IR  protocols Type enum
typedef enum{
    IR_TYPE_NEC = 0,        /* NEC protocol */
    IR_TYPE_RC5,            /* RC5 protocol*/
    IR_TYPE_RC6,            /* RC6 protocol*/
    IR_TYPE_RCMM,           /* RCMM protocol*/
    IR_TYPE_KONKA,          /* Konka protocol*/
    IR_TYPE_HAIER,          /* Haier protocol*/
    IR_TYPE_RCA,            /*TCL RCA protocol**/
    IR_TYPE_P7051,          /*Panasonic 7051 protocol**/
    IR_TYPE_TOSHIBA,        /*Toshiba protocol*/
    IR_TYPE_RC5X,           /* RC5 ext protocol*/
    IR_TYPE_RC6_MODE0,      /* RC6  mode0 protocol*/
    IR_TYPE_MAX,
}IR_Type_e;


//Description  of IR
typedef struct IR_Profile_s {
    IR_Type_e eIRType;
    u32 u32HeadCode;
    u32 u32IRSpeed;
}IR_Profile_t;

struct key_map_table {
    u32 scancode;
    u32 keycode;
};

struct key_map {
    struct key_map_table    *scan;
    unsigned int        size;   /* Max number of entries */
    const char      *name;
    u32              headcode;
};

struct key_map_list {
    struct list_head     list;
    struct key_map map;
};

#define KEYMAP_PAIR(x)      {#x,x}
#define IRPROTOCOL_PAIR(x)  {#x,IR_TYPE_##x}
typedef struct {
    char *key;
    u32 value;
}key_value_t;


int MIRC_Map_Register(struct key_map_list *map);
void MIRC_Map_UnRegister(struct key_map_list *map);
int MIRC_IRCustomer_Add(IR_Profile_t *stIr);
int MIRC_IRCustomer_Remove(IR_Profile_t *stIr);
int MIRC_IRCustomer_Init(void);
void mstar_ir_reinit(void);

#endif //__IR_DYNAMIC_CONFIG__
