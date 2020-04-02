#ifndef _KEYMAP_H_
#define _KEYMAP_H_
#include <linux/input.h>

#define IR_MAP_MSTAR_TV                  "ir-mstar-tv"
#define IR_MAP_TCL_RCA_TV                "ir-tcl-rca-tv"
#define IR_MAP_PANASONIC_TV              "ir-panasonic-tv"

enum {
    KEYMAP_MSTAR_TV     =0x01,
    KEYMAP_TCL_RCA_TV   =0x02,
    KEYMAP_PANASONIC_TV =0x03,
};
#endif
