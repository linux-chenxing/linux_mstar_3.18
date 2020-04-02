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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <generated/autoconf.h>
//#include <linux/undefconf.h>

//------------------------------IR_TYPE_SEL--------------
#define IR_TYPE_OLD                 0
#define IR_TYPE_NEW                 1
#define IR_TYPE_MSTAR_DTV           2
#define IR_TYPE_MSTAR_RAW           3
#define IR_TYPE_RC_V16              4
#define IR_TYPE_CUS03_DTV           5
#define IR_TYPE_MSTAR_FANTASY       6
#define IR_TYPE_MSTAR_SZ1           7
#define IR_TYPE_SKYWORTH            8 //_SLIDE
#define IR_TYPE_HISENSE             9
#define IR_TYPE_CUS08_RC5           10
#define IR_TYPE_KONKA               11
#define IR_TYPE_CUS21SH             21
#define IR_TYPE_HAIER               22 // TOSHIBA
#define IR_TYPE_TCL                 23 //_RCA
#define IR_TYPE_RCMM                24
#define IR_TYPE_TOSHIBA             25
#define IR_TYPE_CHANGHONG           26
#define IR_TYPE_SWRC6               28
//------------------------------BOARD_TYPE_SEL-----------
// 0x00 ~ 0x1F LCD Demo board made in Taiwan
#define BD_FPGA                     0x01
#define BD_GENERIC                  0x02

#define BD_UNKNOWN                  0xFF

#define SVD_CLOCK_250MHZ            0x00
#define SVD_CLOCK_240MHZ            0x01
#define SVD_CLOCK_216MHZ            0x02
#define SVD_CLOCK_SCPLL             0x03
#define SVD_CLOCK_MIU               0x04
#define SVD_CLOCK_144MHZ            0x05
#define SVD_CLOCK_123MHZ            0x06
#define SVD_CLOCK_108MHZ            0x07

#define SVD_CLOCK_ENABLE            TRUE
#define SVD_CLOCK_INVERT            FALSE

#ifndef MS_BOARD_TYPE_SEL

#if defined(CONFIG_MSTAR_ARM_BD_FPGA)
    #define MS_BOARD_TYPE_SEL       BD_FPGA
#elif defined(CONFIG_MSTAR_ARM_BD_GENERIC)
    #define MS_BOARD_TYPE_SEL       BD_GENERIC
#else
    #error "BOARD define not found"
#endif

#endif

//-------------------------------------------------------


///////////////////////////////////////////////////////////
#if   (MS_BOARD_TYPE_SEL == BD_FPGA)
    #include "BD_FPGA.h"
#elif (MS_BOARD_TYPE_SEL == BD_GENERIC)
    #include "BD_GENERIC.h"
#endif


/////////////////////////////////////////////////////////

#endif /* _BOARD_H_ */

