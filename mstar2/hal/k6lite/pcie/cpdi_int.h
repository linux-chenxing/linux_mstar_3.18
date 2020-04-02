/**********************************************************************
 * copyright (C) 2011-2014 Cadence Design Systems
 * All rights reserved.
 ***********************************************************************
 * cpdi_int.h
 * internal definition file for PCIe Core Driver
 ***********************************************************************/

#ifndef _CFP_CPD_INT_H
#define _CFP_CPD_INT_H

#include "pcie_top.h"
#include "pcie_top_macro.h"
#include "cpdi.h" // include the public header

/****************************************************************************/
/* CPDI_Private_Data                                                        */
/****************************************************************************/
typedef struct {
    uint32_t *base;
    uint32_t  reserved [15];
} CPDI_Private_Data_t;

#endif
