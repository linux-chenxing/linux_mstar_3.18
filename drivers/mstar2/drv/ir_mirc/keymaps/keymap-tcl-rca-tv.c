/* keymap-tcl-tv.h - Keytable for tcl_tv Remote Controller
 *
 * keymap imported from ir-keymaps.c
 *
 * Copyright (c) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */



#include <linux/module.h>
#include <linux/kernel.h>
#include "../ir_core.h"
#include "../ir_common.h"
/*
 * Jimmy Hsu <jimmy.hsu@mstarsemi.com>
 * this is the remote control that comes with the tcl smart tv
 * which based on STAOS standard.
 */

static struct key_map_table tcl_tv[] = {
    // 1st IR controller.
    { 0x54, KEY_POWER },
    { 0x3E, KEY_0 },
    { 0x8C, KEY_1 },
    { 0x4C, KEY_2 },
    { 0xCC, KEY_3 },
    { 0x2C, KEY_4 },
    { 0xAC, KEY_5 },
    { 0x6C, KEY_6 },
    { 0xEC, KEY_7 },
    { 0x1C, KEY_8 },
    { 0x9C, KEY_9 },
    { 0x00, KEY_RED },
    { 0x17, KEY_GREEN },
    { 0x27, KEY_YELLOW },
    { 0x1B, KEY_BLUE },
    { 0x9A, KEY_UP },
    { 0x1A, KEY_DOWN },
    { 0x6A, KEY_LEFT },
    { 0xEA, KEY_RIGHT },
    { 0x2F, KEY_ENTER },
    { 0xB4, KEY_CHANNELUP },
    { 0x34, KEY_CHANNELDOWN },
    { 0xF4, KEY_VOLUMEUP },
    { 0x74, KEY_VOLUMEDOWN },
    { 0x10, KEY_HOME },
    { 0x60, KEY_BACK },
    { 0x37, KEY_MUTE },
    { 0x8B, KEY_INFO },
    { 0xC5, KEY_KP0 },        // TV_INPUT
    { 0x48, KEY_CAMERA },     // (C)PICTURE_MODE
    { 0x09, KEY_ZOOM },       // (C)ASPECT RATIO

    // 2nd IR controller.
};

static struct key_map_list tcl_tv_map = {
    .map = {
        .scan    = tcl_tv,
        .size    = ARRAY_SIZE(tcl_tv),
        .name    = NAME_KEYMAP_TCL_RCA_TV,
        .headcode     = NUM_KEYMAP_TCL_RCA_TV,
    }
};

static int __init init_key_map_tcl_tv(void)
{
    return MIRC_Map_Register(&tcl_tv_map);
}

static void __exit exit_key_map_tcl_tv(void)
{
    MIRC_Map_UnRegister(&tcl_tv_map);
}

module_init(init_key_map_tcl_tv)
module_exit(exit_key_map_tcl_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");
