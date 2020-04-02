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
/// @file   pa_space.h
/// @brief  manage physical memory interface
/// @author MStar Semiconductor Inc.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX_PA_SPACE_H
#define _LINUX_PA_SPACE_H
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define PA_SPACE_BUG_ON(cond)  \
do { \
      if(cond) \
        printk(KERN_ERR "pa space bug in %s @ %d \n", __FUNCTION__, __LINE__); \
      BUG_ON(cond); \
   } while(0)


#define PA_SPACE_ERR(fmt, args...) printk(KERN_ERR "error %s:%d " fmt,__FUNCTION__,__LINE__,## args)
#define PA_SPACE_WARNING(fmt, args...) printk(KERN_ERR "warning %s:%d " fmt,__FUNCTION__,__LINE__,## args)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
struct pa_using_addr_space_node {
    unsigned long start;             //index (page size unit)
    unsigned long size;              //how many pages in this node
    struct list_head node;           //in list for tracing using pa
};

struct pa_using_addr_space {
    struct list_head list_head;      //tracing using pa node
    unsigned long count;             //nodes in list
};
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif

