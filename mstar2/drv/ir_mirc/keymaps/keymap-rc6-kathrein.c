/* keymap-mstar-tv.h - Keytable for mstar_tv Remote Controller
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
 * this is the remote control that comes with the mstar smart tv
 * which based on STAOS standard.
 */

static struct key_map_table Kathrein_tv[] = {
    { 0x0C, KEY_POWER },
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
    { 0x6D, KEY_RED },
    { 0x6E, KEY_GREEN },
    { 0x6F, KEY_YELLOW },
    { 0x70, KEY_BLUE },
    { 0x58, KEY_UP },
    { 0x59, KEY_DOWN },
    { 0x5A, KEY_LEFT },
    { 0x5B, KEY_RIGHT },
    { 0x5C, KEY_ENTER },
    { 0x1E, KEY_CHANNELUP },
    { 0x1F, KEY_CHANNELDOWN },
    { 0x10, KEY_VOLUMEUP },
    { 0x11, KEY_VOLUMEDOWN },
    { 0x54, KEY_MENU },
    { 0x55, KEY_BACK },
    { 0x0D, KEY_MUTE },
    { 0x3C, KEY_EXIT },
    { 0x37, KEY_RECORD },     // REC
    { 0x0F, KEY_INFO },
    { 0x21, KEY_REWIND },
    { 0x20, KEY_FORWARD },
    { 0x38, KEY_PLAY },
    { 0x37, KEY_PAUSE },
    { 0x31, KEY_STOP },
    { 0xCC, KEY_EPG },        // (C)EPG
    { 0xD5, KEY_MEDIA },
    { 0x46, KEY_ARCHIVE },
    { 0x81, KEY_CONTEXT_MENU },
    // 2nd IR controller.
};

static struct key_map_list RC6_Kathrein_map = {
    .map = {
        .scan    = Kathrein_tv,
        .size    = ARRAY_SIZE(Kathrein_tv),
        .name    = NAME_KEYMAP_KATHREIN_TV,
        .headcode     = NUM_KEYMAP_KATHREIN_TV,
    }
};

static int __init init_key_map_mstar_tv(void)
{
    return MIRC_Map_Register(&RC6_Kathrein_map);
}

static void __exit exit_key_map_mstar_tv(void)
{
    MIRC_Map_UnRegister(&RC6_Kathrein_map);
}

module_init(init_key_map_mstar_tv)
module_exit(exit_key_map_mstar_tv)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");
