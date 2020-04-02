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
/// file    ir_dynamic_config_main.c
/// @brief  Load IR config from INI config file
/// @author Vick.Sun@MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include <linux/reboot.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/input.h>
#include <linux/delay.h>

#include "ir_dynamic_config.h"
#include "iniparser.h"

static int debug = 1;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "debug:\n\t0: error only\n\t1:debug info\n\t2:all\n");

static char *config_file = "/config/ir_config.ini";
module_param(config_file,charp, 0644);
MODULE_PARM_DESC(config_file, "config file path");

#define DBG_ALL(__fmt,args...)              do{if (2 <= debug){printk("[IR config]"__fmt,##args);}}while(0)
#define DBG_INFO(__fmt,args...)             do{if (1 <= debug){printk("[IR config]"__fmt,##args);}}while(0)
#define DBG_ERR(__fmt,args...)              do{if (0 <= debug){printk("[IR config]"__fmt,##args);}}while(0)

#define SIZE(array)  (sizeof(array)/sizeof(array[0]))

static IR_Profile_t *ir_profiles = NULL;
static int ir_profile_num = 0;
static int ir_profile_parse_num = 0;
struct key_map_list *key_map_lists = NULL;

key_value_t IR_KeysList[] = {
#include "input_keys_available.h"
};

static void load_ir_config_thread(struct work_struct *work);
static int load_ir_config_thread_need_stop = 0;
static DECLARE_DELAYED_WORK(load_ir_config_work, load_ir_config_thread);

static void fileread(char *filename, char **data)
{
    struct file *filp;
    mm_segment_t fs;
    loff_t size;
    ssize_t ret;
    filp = filp_open(filename, O_RDONLY, 0644);
    if (IS_ERR(filp))
    {
        DBG_ALL("open %s error...\n", filename);
        return;
    }
    fs = get_fs();
    set_fs(KERNEL_DS);
    size = filp->f_op->llseek(filp, 0, SEEK_END);
    *data = (char *)kmalloc(size + 1, GFP_KERNEL);
    filp->f_op->llseek(filp, 0, SEEK_SET);
    ret = vfs_read(filp, *data, size, &filp->f_pos);
    if (ret != size)
    {
        DBG_ERR(KERN_WARNING"read %s error!\n", filename);
        kfree(*data);
        *data = NULL;
        goto out;
    }
    (*data)[size] = 0;
out:
    set_fs(fs);
    filp_close(filp, NULL);
}
static int str2ul(const char *str, u32 *val)
{
    if (sscanf(str, "%i", val) == -1)
    {
        return -1;
    }
    return 0;
}

static void parse_all_keymaps(IniSectionNode *root, const char *name, const char *value)
{
    IniKeyNode *k;
    const char *enable;
    const char *protocol;
    const char *header;
    const char *speed;
    const char *keymap_name;
    IniSectionNode *keymap_sec;
    int keymap_size;
    struct key_map_table *table;
    int i;
    IR_Profile_t *curr_ir_profile = &ir_profiles[ir_profile_parse_num];
    IniSectionNode *ir_config = get_section(root, value);
    if(ir_profile_parse_num > 15)return;
    if (ir_config == NULL)
    {
        DBG_ERR("no section named %s\n", value);
        return;
    }
    /**
     * Keymap is Enabled?
     */
    enable = get_key_value(ir_config,"Enable");
    if(enable && (*enable == 'f' || *enable == 'F' || *enable == '0' || *enable == 'n' || *enable == 'N'))
    {
        return;
    }
    /**
     * Parse Protocol
     */
    protocol = get_key_value(ir_config, "Protocol");
    if (protocol == NULL)
    {
        DBG_ERR("no Protocol for %s\n", ir_config->name);
        return;
    }
    if(str2ul(protocol,&curr_ir_profile->eIRType))
    {
        DBG_ERR("Protocol for %s format error.\n", ir_config->name);
        return;
    }
    DBG_INFO("Protocol value:0x%02x\n",curr_ir_profile->eIRType);
    curr_ir_profile->eIRType--;
    /**
     * Parse Header code
     */
    header = get_key_value(ir_config, "Header");
    if (header == NULL)
    {
        DBG_ERR("no Header for %s\n", ir_config->name);
        return;
    }
    if (str2ul(header, &curr_ir_profile->u32HeadCode))
    {
        DBG_ERR("header code format err for %s\n", ir_config->name);
        return;
    }
    DBG_INFO("Header code:0x%x\n", curr_ir_profile->u32HeadCode);
    /**
     * Parse IR Speed
     */
    speed = get_key_value(ir_config, "Speed");
    if (speed == NULL)
    {
        DBG_ERR("Speed for %s is empty. use default.\n", ir_config->name);
    }
    else
    {
        if (str2ul(speed, &curr_ir_profile->u32IRSpeed))
        {
            DBG_ERR("IR Speed format err for %s\n", ir_config->name);
            return;
        }
        DBG_INFO("Speed:0x%x\n", curr_ir_profile->u32IRSpeed);
    }
    /**
     * Parse Keymap
     */
    keymap_name = get_key_value(ir_config, "Keymap");
    if (keymap_name == NULL)
    {
        DBG_ERR("no Keymap key for %s\n", ir_config->name);
        return;
    }
    keymap_sec = get_section(root, keymap_name);
    if (keymap_sec == NULL)
    {
        DBG_ERR("no Keymap section for %s\n", keymap_name);
        return;
    }
    keymap_size = get_key_num(keymap_sec);
    if (keymap_size == 0)
    {
        DBG_ERR("no keys in section %s\n", keymap_name);
        return;
    }
    table = kmalloc(sizeof(struct key_map_table) * keymap_size, GFP_KERNEL);
    if (table == NULL)
    {
        DBG_ERR(KERN_ERR"OOM!\n");
        return;
    }
    memset(table, 0, sizeof(struct key_map_table) * keymap_size);
    key_map_lists[ir_profile_parse_num].map.scan = table;
    for (k = keymap_sec->keys;k != NULL; k = k->next)
    {
        for ( i = 0; i < SIZE(IR_KeysList); i++ )
        {
            if (!strcmp(k->name, IR_KeysList[i].key))
            {
                table->keycode = IR_KeysList[i].value;

                if (str2ul(k->value, &table->scancode))
                {
                    DBG_ERR("scan code format err for %s\n", keymap_sec->name);
                    return;
                }
                DBG_ALL("KEY name:%s,KEY val:0x%x,scan code:0x%x\n", k->name, table->keycode, table->scancode);
                table++;
                break;
            }
        }
        if(i == SIZE(IR_KeysList))
        {
            if(str2ul(k->name,&table->keycode))
            {
                DBG_ERR("non-standard key format err for %s\n", k->name);
                return;
            }
            if(str2ul(k->value,&table->scancode))
            {
                DBG_ERR("non-standard key's value format err for %s=%s\n", k->name,k->value);
                return;
            }
            DBG_ALL("KEY name:%s,KEY val:0x%x,scan code:0x%x\n", k->name, table->keycode, table->scancode);
            table++;
        }
    }
    key_map_lists[ir_profile_parse_num].map.headcode = curr_ir_profile->u32HeadCode;
    key_map_lists[ir_profile_parse_num].map.size = keymap_size;
    key_map_lists[ir_profile_parse_num].map.name = kstrdup(keymap_name, GFP_KERNEL);
    DBG_INFO("register config:%s with Header code 0x%x\n", ir_config->name, curr_ir_profile->u32HeadCode);
    DBG_INFO("register keymap:%s\n", keymap_name);
    MIRC_IRCustomer_Add(curr_ir_profile);
    MIRC_Map_Register(&key_map_lists[ir_profile_parse_num]);
    ir_profile_parse_num++;
}
static void load_ir_config_thread(struct work_struct *work)
{
    while (!load_ir_config_thread_need_stop)
    {
        char *contents = NULL;
        IniSectionNode *root;
        IniSectionNode *kernel_sec;

        if(config_file != NULL)
            fileread(config_file, &contents);
        if (contents == NULL)
        {
            msleep(400);
            continue;
        }
        if (alloc_and_init_ini_tree(&root, contents))
        {
            DBG_ERR(KERN_ERR"OOM!\n");
            goto out;
        }
        if (debug > 1)
        {
            dump_ini(root);
        }
        kernel_sec = get_section(root, "Kernel");
        if (kernel_sec == NULL)
        {
            DBG_ERR("no section named Kernel\n");
            goto out;
        }
        ir_profile_num = get_section_num(kernel_sec);
        if (ir_profile_num == 0)
        {
            DBG_ERR("no no keys in section Kernel\n");
            goto out;
        }
        ir_profiles = kmalloc(sizeof(IR_Profile_t) * ir_profile_num, GFP_KERNEL);
        key_map_lists = kmalloc(sizeof(struct key_map_list) * ir_profile_num, GFP_KERNEL);
        if ((key_map_lists == NULL) || (ir_profiles == NULL))
        {
            DBG_ERR(KERN_ERR"OOM!\n");
        }
        memset(ir_profiles, 0, sizeof(IR_Profile_t) * ir_profile_num);
        memset(key_map_lists, 0, sizeof(struct key_map_list) * ir_profile_num);
        foreach_key(root, kernel_sec, parse_all_keymaps);
        MIRC_IRCustomer_Init();
        mstar_ir_reinit();
    out:
        release_ini_tree(root);
        kfree(contents);

        return;
    }
}




static int __init ir_dynamic_config_init(void)
{
    schedule_delayed_work(&load_ir_config_work,10);
    return 0;
}

static void __exit ir_dynamic_config_exit(void)
{
    int i;
    load_ir_config_thread_need_stop = 1;
    cancel_delayed_work_sync(&load_ir_config_work);
    for ( i = 0; i < ir_profile_parse_num; i++ )
    {
        if (key_map_lists != NULL)
        {
            if (key_map_lists[i].map.scan != NULL)
            {
                MIRC_Map_UnRegister(&key_map_lists[i]);
                kfree(key_map_lists[i].map.scan);
                key_map_lists[i].map.scan = NULL;
            }
            if (key_map_lists[i].map.name != NULL)
            {
                DBG_INFO("unregister keymap:%s\n", key_map_lists[i].map.name);
                kfree(key_map_lists[i].map.name);
                key_map_lists[i].map.name = NULL;
            }
        }
    }

    if (key_map_lists)
    {
        kfree(key_map_lists);
        key_map_lists = NULL;
    }
    if (ir_profiles)
    {
        for ( i = 0; i < ir_profile_parse_num; i++ )
        {
            DBG_INFO("register config with Header code:%x\n", ir_profiles[i].u32HeadCode);
            MIRC_IRCustomer_Remove(&ir_profiles[i]);
        }
        kfree(ir_profiles);
        ir_profiles = NULL;
    }
}

module_init(ir_dynamic_config_init);
module_exit(ir_dynamic_config_exit);

MODULE_DESCRIPTION("IR dynamic config module for MSTAR");
MODULE_AUTHOR("Vick Sun <vick.sun@mstarsemi.com>");
MODULE_LICENSE("GPL v2");
