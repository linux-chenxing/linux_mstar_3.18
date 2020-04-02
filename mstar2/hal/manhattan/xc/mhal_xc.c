///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------


#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#include "chip_int.h"



//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------



//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MHal_XC_W2BYTE(DWORD u32Reg, WORD u16Val )
{
    REG_W2B(u32Reg, u16Val);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MHal_XC_R2BYTE(DWORD u32Reg )
{
    return REG_RR(u32Reg) ;
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
void MHal_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask )
{
    WORD u16Data=0 ;
    u16Data = REG_RR(u32Reg);
    u16Data = (u16Data & (0xFFFF-u16Mask))|(u16Val &u16Mask);
    REG_W2B(u32Reg, u16Data);
}

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
WORD MHal_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask )
{
    return (REG_RR(u32Reg) & u16Mask);
}



//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
BOOL MHal_XC_IsBlackVideoEnable( MS_BOOL bWindow )
{
    MS_BOOL bReturn=FALSE;

    if( MAIN_WINDOW == bWindow)
    {
        if(MHal_XC_R2BYTEMSK(REG_SC_BK10_19_L,BIT1))
        {
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
    }

    return bReturn;
}


//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
#if (CONFIG_MSTAR_FRC_SOFTWARE_TOGGLE == 1)
void MHal_XC_FRCR2SoftwareToggle( void )
{	
	MHal_XC_W2BYTEMSK(REG_SC_BK00_14_L, 0x00, BIT4);
	
	if (MHal_XC_R2BYTEMSK(REG_SC_BK00_10_L, BIT4) == BIT4) // output Vsync case
	{
	    //MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, BIT4, BIT4); // Clean job will be done in scaler
	    FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = INT_HKCPU_FRCR2_INPUT_SYNC; 	//to FRC-R2
	    FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = INT_HKCPU_FRCR2_OUTPUT_SYNC; 	//to FRC-R2
	    mb();
	    FRC_CPU_INT_REG(REG_FRCINT_HKCPUFIRE) = 0;
    }
}
#endif
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


