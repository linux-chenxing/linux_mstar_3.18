/* keymap-panasonic.h - Keytable for mstar_tv Remote Controller
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

#include <linux/kernel.h>
#include <linux/module.h>
#include "../ir_core.h"
#include "keymap.h"

/*
 * this is the remote control that comes with the panasonic  smart tv
 * which based on panasonic  standard.
 */

static struct key_map_table panasonic_tv[] = {
    { 0x5057, KEY_POWER },

    { 0x3030, KEY_0 },
    { 0x3031, KEY_1 },
    { 0x3032, KEY_2 },
    { 0x3033, KEY_3 },
    { 0x3034, KEY_4 },
    { 0x3035, KEY_5 },
    { 0x3036, KEY_6 },
    { 0x3037, KEY_7 },
    { 0x3038, KEY_8 },
    { 0x3039, KEY_9 },

    { 0x5550, KEY_UP },
    { 0x444E, KEY_DOWN },
    { 0x4C45, KEY_LEFT },
    { 0x5249, KEY_RIGHT },
    { 0x454E, KEY_ENTER },
    //{ 0x454E, KEY_OK },         // KEY_OK

    { 0x562B, KEY_VOLUMEUP },
    { 0x562D, KEY_VOLUMEDOWN },

    { 0x4D45, KEY_HOME},
    { 0x5250, KEY_MENU },
    { 0x5254, KEY_BACK },
    { 0x4633, KEY_KP1 },        // TV_INPUT
};

static struct key_map_list panasonic_tv_map = {
    .map = {
        .scan    = panasonic_tv,
        .size    = ARRAY_SIZE(panasonic_tv),
        .name    = IR_MAP_PANASONIC_TV,
    }
};

static int __init init_key_map_panasonic_tv(void)
{
    return MIRC_Map_Register(&panasonic_tv_map);
}

static void __exit exit_key_map_panasonic_tv(void)
{
    MIRC_Map_UnRegister(&panasonic_tv_map);
}

module_init(init_key_map_panasonic_tv)
module_exit(exit_key_map_panasonic_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");
