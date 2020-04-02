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
/// file    iniparser.c
/// @brief  Parse Ini config file
/// @author Vick.Sun@MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////



#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/slab.h>

#include "iniparser.h"

#define MALLOC _malloc
#define FREE kfree
#define PRINT printk

static struct kmem_cache *_malloc(size_t size)
{
    return kmalloc(size,GFP_KERNEL);
}

static const char* strtrim(char* str)
{
    char* p = str + strlen(str) - 1;
    while(p != str && (isspace(*p) || *p == '\"'))
    {
        *p = '\0';
        p--;
    }

    p = str;
    while(*p != '\0' && (isspace(*p) || *p == '\"'))
    {
        p++;
    }

    if (p != str)
    {
        char* d = str;
        char* s = p;
        while(*s != '\0')
        {
            *d = *s;
            s++;
            d++;
        }
        *d = '\0';
    }

    return str;
}

int alloc_and_init_ini_tree(IniSectionNode **root,char*p)
{
    enum _State
    {
        STAT_NONE = 0,
        STAT_GROUP,
        STAT_KEY,
        STAT_VAL,
        STAT_COMMENT
    }state = STAT_NONE;
    char *group_start = p;
    char *key_start = p;
    char *val_start = p;
    IniSectionNode *current_section = NULL;
    IniKeyNode *current_key = NULL;
    while (*p != '\0')
    {
        if (*p == '#' || *p == ';')
        {
            /*skip comment*/
            while (*p != '\0' &&* p != '\n')
            {
                *p = 0;
                p++;
            }
            if (*p == '\0')
            {
                break;
            }
        }
        switch(state)
        {
            case STAT_NONE:
            {
                if (*p == '[')
                {
                    state = STAT_GROUP;
                    group_start = p + 1;
                    if (current_section == NULL)
                    {
                        current_section = (IniSectionNode *)MALLOC(sizeof(IniSectionNode));
                        if (!current_section)
                        {
                            PRINT("return on OOM!\n");
                            return -1;
                        }
                        memset(current_section, 0, sizeof(IniSectionNode));
                        *root = current_section;
                    }
                    else
                    {
                        current_section->next = (IniSectionNode*)MALLOC(sizeof(IniSectionNode));
                        if (!current_section->next)
                        {
                            PRINT("return on OOM!\n");
                            return -1;
                        }
                        memset(current_section->next, 0, sizeof(IniSectionNode));
                        current_section = current_section->next;
                    }
                }
                else if (!isspace(*p))
                {
                    key_start = p;
                    if (current_key == NULL)
                    {
                        current_key = (IniKeyNode*)MALLOC(sizeof(IniKeyNode));
                        if (!current_key)
                        {
                            PRINT("return on OOM!\n");
                            return -1;
                        }
                        memset(current_key, 0, sizeof(IniKeyNode));
                        if (current_section == NULL)
                        {
                            current_section = (IniSectionNode *)MALLOC(sizeof(IniSectionNode));
                            if (!current_section)
                            {
                                PRINT("return on OOM!\n");
                                return -1;
                            }
                            memset(current_section, 0, sizeof(IniSectionNode));
                            *root = current_section;
                        }
                        current_section->keys = current_key;
                    }
                    else
                    {
                        current_key->next = (IniKeyNode*)MALLOC(sizeof(IniKeyNode));
                        if (!current_key->next)
                        {
                            PRINT("return on OOM!\n");
                            return -1;
                        }
                        memset(current_key->next, 0, sizeof(IniKeyNode));
                        current_key = current_key->next;
                    }
                    state = STAT_KEY;
                }
                break;
            }
            case STAT_GROUP:
            {
                if (*p == ']')
                {
                    *p = '\0';
                    current_section->name = strtrim(group_start);
                    current_key = NULL;
                    state = STAT_NONE;
                }
                break;
            }
            case STAT_KEY:
            {
                if (*p == '=')
                {
                    *p = '\0';
                    val_start = p + 1;
                    current_key->name = strtrim(key_start);
                    state = STAT_VAL;
                }
                break;
            }
            case STAT_VAL:
            {
                if (*p == '\n' || *p == '#' || *p == ';')
                {
                    if (*p != '\n')
                    {
                        /*skip comment*/
                        while (*p != '\0' && *p != '\n')
                        {
                            *p = '\0';
                            p++;
                        }
                    }
                    *p = '\0';
                    current_key->value = strtrim(val_start);
                    state = STAT_NONE;

                    break;
                }
            }
            default:
                break;
        }
        p++;
    }
    if (STAT_VAL == state)
    {
        current_key->value = strtrim(val_start);
    }
    return 0;
}
EXPORT_SYMBOL_GPL(alloc_and_init_ini_tree);
void release_ini_tree(IniSectionNode *root)
{
    IniSectionNode *current_section = root;
    IniKeyNode *current_key;
    IniSectionNode *last_section = NULL;
    IniKeyNode *last_key = NULL;
    while (current_section != NULL)
    {
        current_key = current_section->keys;
        while (current_key != NULL)
        {
            last_key = current_key;
            current_key = current_key->next;
            FREE(last_key);
            last_key = NULL;
        }
        last_section = current_section;
        current_section = current_section->next;
        FREE(last_section);
        last_section = NULL;
    }
}
EXPORT_SYMBOL_GPL(release_ini_tree);
IniSectionNode* get_section(IniSectionNode *root, const char *name)
{
    IniSectionNode *current_section;
    current_section = root;
    while (current_section != NULL)
    {
        if (!strcmp(name, current_section->name))
        {
            return current_section;
        }
        current_section = current_section->next;
    }
    return NULL;
}
EXPORT_SYMBOL_GPL(get_section);
size_t get_section_num(IniSectionNode *root)
{
    size_t i = 0;
    IniSectionNode *current_section = root;
    while (current_section != NULL)
    {
        i++;
        current_section = current_section->next;
    }

    return i;
}
EXPORT_SYMBOL_GPL(get_section_num);
void foreach_section(IniSectionNode *root, void (*fun)(IniSectionNode *, IniSectionNode *, const char *))
{
    IniSectionNode *current_section;
    current_section = root;
    while (current_section != NULL)
    {
        fun(root, current_section, current_section->name);
        current_section = current_section->next;
    }
}
EXPORT_SYMBOL_GPL(foreach_section);
void foreach_key(IniSectionNode *root, IniSectionNode *node, void (*fun)(IniSectionNode *, const char *, const char *))
{
    IniKeyNode *current_key;
    if (node && node->keys)
    {
        current_key = node->keys;
        while (current_key != NULL)
        {
            fun(root,current_key->name, current_key->value);
            current_key = current_key->next;
        }
    }
}
EXPORT_SYMBOL_GPL(foreach_key);
const char* get_key_value(IniSectionNode *node, const char *name)
{
    IniKeyNode *current_key;
    if (node && node->keys)
    {
        current_key = node->keys;
        while (current_key != NULL)
        {
            if (!strcmp(current_key->name,name))
            {
                return current_key->value;
            }
            current_key = current_key->next;
        }
    }
    return NULL;
}
EXPORT_SYMBOL_GPL(get_key_value);
size_t get_key_num(IniSectionNode *node)
{
    size_t i = 0;
    IniKeyNode *current_key;
    if (node && node->keys)
    {
        current_key = node->keys;
        while (current_key != NULL)
        {
            i++;
            current_key = current_key->next;
        }
    }
    return i;
}
EXPORT_SYMBOL_GPL(get_key_num);
static void print_key_value(IniSectionNode *root,const char *name, const char *value)
{
    PRINT("%s=%s\n", name, value);
}
static void print_section(IniSectionNode *root, IniSectionNode *node,const char *name)
{
    PRINT("[%s]\n", name);
    foreach_key(root, node, print_key_value);
}

void dump_ini(IniSectionNode *root)
{
    foreach_section(root, print_section);
}
EXPORT_SYMBOL_GPL(dump_ini);
