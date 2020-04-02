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
/// file    iniparser.h
/// @brief  Parse Ini config file
/// @author Vick.Sun@MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __INI_PARSER__
#define __INI_PARSER__
typedef struct _IniKeyNode_List {
    const char *name;
    const char *value;
    struct _IniKeyNode_List *next;
}IniKeyNode;

typedef struct _IniSectionNode_List {
    const char *name;
    IniKeyNode *keys;
    struct _IniSectionNode_List *next;
}IniSectionNode;

int alloc_and_init_ini_tree(IniSectionNode **root,char*p);
void release_ini_tree(IniSectionNode *root);
IniSectionNode* get_section(IniSectionNode *root, const char *name);
size_t get_section_num(IniSectionNode *root);
void foreach_section(IniSectionNode *root, void (*fun)(IniSectionNode *, IniSectionNode *, const char *));
void foreach_key(IniSectionNode *root, IniSectionNode *node, void (*fun)(IniSectionNode *, const char *, const char *));
const char* get_key_value(IniSectionNode *node, const char *name);
size_t get_key_num(IniSectionNode *node);
void dump_ini(IniSectionNode *root);
#endif
