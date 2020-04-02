/* changhong-tv.h - Keytable for changhong_tv Remote Controller
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
#include "../ir_common.h"

/*
 * Jimmy Hsu <jimmy.hsu@mstarsemi.com>
 * this is the remote control that comes with the changhong smart tv
 * which based on STAOS standard.
 */

static struct key_map_table changhong_tv[] = {
    // 1st IR controller.
    { 0x12, KEY_POWER },
    { 0x00, KEY_0 },
    { 0x01, KEY_1 },
    { 0x02, KEY_2 },
    { 0x03, KEY_3 },
    { 0x04, KEY_4 },
    { 0x05, KEY_5 },
    { 0x06, KEY_6 },
    { 0x07, KEY_7 },
    { 0x08, KEY_8 },
    { 0x09, KEY_9 },
    { 0x4A, KEY_RED },
    { 0x4B, KEY_GREEN },
    { 0x4C, KEY_YELLOW },
    { 0x4D, KEY_BLUE },
    { 0x19, KEY_UP },
    { 0x1D, KEY_DOWN },
    { 0x46, KEY_LEFT },
    { 0x47, KEY_RIGHT },
    { 0x0A, KEY_ENTER },
    { 0x1B, KEY_CHANNELUP },
    { 0x1F, KEY_CHANNELDOWN },
    { 0x1A, KEY_VOLUMEUP },
    { 0x1E, KEY_VOLUMEDOWN },
    { 0x51, KEY_HOME},
    { 0x5B, KEY_MENU },
    { 0x40, KEY_BACK },
    { 0x10, KEY_MUTE },
    { 0x16, KEY_INFO },
    { 0xC5, KEY_TV },
    { 0x50, KEY_DELETE },
    { 0x5C, KEY_KP0 },    // TV_INPUT
    { 0x67, KEY_KP1 },    // 3D_MODE
    { 0x0B, KEY_EPG },
    { 0x55, KEY_F1 },     // CHANGHONGIR_HELP
    { 0x5E, KEY_F2 },     // CHANGHONGIR_APP
    { 0x41, KEY_F3 },     // CHANGHONGIR_SPREAD
    { 0x0F, KEY_F4 },     // CHANGHONGIR_PINCH
    { 0x4F, KEY_F5 },     // CHANGHONGIR_FLCK_SL
    { 0x4E, KEY_F6 },     // CHANGHONGIR_FLCK_SR
    { 0x53, KEY_F7 },     // CHANGHONGIR_INPUT
    { 0x5B, KEY_F8 },     // CHANGHONGIR_BARCODE

    // 2nd IR controller.
};

static struct key_map_list changhong_tv_map = {
	.map = {
		.scan    = changhong_tv,
		.size    = ARRAY_SIZE(changhong_tv),
		.name    = NAME_KEYMAP_CHANGHONG_TV,
        .headcode     = NUM_KEYMAP_CHANGHONG_TV,
	}
};

static int __init init_key_map_changhong_tv(void)
{
	return MIRC_Map_Register(&changhong_tv_map);
}

static void __exit exit_key_map_changhong_tv(void)
{
	MIRC_Map_UnRegister(&changhong_tv_map);
}

module_init(init_key_map_changhong_tv)
module_exit(exit_key_map_changhong_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");
