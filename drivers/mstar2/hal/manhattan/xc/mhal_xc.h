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

#ifndef _MHAL_XC_H_
#define _MHAL_XC_H_

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

//-------------------------------------------------------------------------------------------------
//  Driver Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define XC_FILM_DRIVER_VER               2

#define XC_FUNCTION                      1

#define XC_FRC_R2_SW_TOGGLE				 1

#if XC_FUNCTION
#define BYTE                                    MS_U8
#define WORD                                    MS_U16
#define DWORD                                   MS_U32
#define printf                                  printk
#define DLC_DEBUG(x)                            //(x)


#define REG_ADDR_VOP_SCREEN_CONTROL             L_BK_VOP(0x19)

#define REG_ADDR_OP_SW_SUB_ENABLE               L_BK_OP(0x10)

#define PRJ_MCNR

#define REG_LPLL_11_L                           (0x103100 | 0x11<< 1)
#define REG_LPLL_13_L                           (0x103100 | 0x13<< 1)

#define REG_SC_BK00_10_L                        (0x130000 | 0x10<< 1)
#define REG_SC_BK00_12_L                        (0x130000 | 0x12<< 1)
#define REG_SC_BK00_14_L                        (0x130000 | 0x14<< 1)

#define REG_SC_BK10_19_L                        (0x131000 | 0x19<< 1)

#define REG_SC_BK12_01_L                        (0x131200 | 0x01<< 1)

#define REG_SC_BK0A_02_L                        (0x130A00 | 0x02<< 1)
#define REG_SC_BK0A_05_L                        (0x130A00 | 0x05<< 1)
#define REG_SC_BK0A_06_L                        (0x130A00 | 0x06<< 1)
#define REG_SC_BK0A_07_L                        (0x130A00 | 0x07<< 1)
#define REG_SC_BK0A_0A_L                        (0x130A00 | 0x0A<< 1)
#define REG_SC_BK0A_0B_L                        (0x130A00 | 0x0B<< 1)
#define REG_SC_BK0A_0C_L                        (0x130A00 | 0x0C<< 1)
#define REG_SC_BK0A_0D_L                        (0x130A00 | 0x0D<< 1)
#define REG_SC_BK0A_0F_L                        (0x130A00 | 0x0F<< 1)

#define REG_SC_BK0A_10_L                        (0x130A00 | 0x10<< 1)
#define REG_SC_BK0A_15_L                        (0x130A00 | 0x15<< 1)
#define REG_SC_BK0A_16_L                        (0x130A00 | 0x16<< 1)
#define REG_SC_BK0A_17_L                        (0x130A00 | 0x17<< 1)
#define REG_SC_BK0A_1E_L                        (0x130A00 | 0x1E<< 1)
#define REG_SC_BK0A_1F_L                        (0x130A00 | 0x1F<< 1)

#define REG_SC_BK0A_20_L                        (0x130A00 | 0x20<< 1)
#define REG_SC_BK0A_21_L                        (0x130A00 | 0x21<< 1)
#define REG_SC_BK0A_23_L                        (0x130A00 | 0x23<< 1)
#define REG_SC_BK0A_24_L                        (0x130A00 | 0x24<< 1)
#define REG_SC_BK0A_25_L                        (0x130A00 | 0x25<< 1)
#define REG_SC_BK0A_41_L                        (0x130A00 | 0x41<< 1)
#define REG_SC_BK0A_4A_L                        (0x130A00 | 0x4A<< 1)
#define REG_SC_BK0A_4B_L                        (0x130A00 | 0x4B<< 1)

#define REG_SC_BK0A_57_L                        (0x130A00 | 0x57<< 1)
#define REG_SC_BK0A_5C_L                        (0x130A00 | 0x5C<< 1)
#define REG_SC_BK0A_5E_L                        (0x130A00 | 0x5E<< 1)

#define REG_SC_BK12_03_L                        (0x131200 | 0x03<< 1)
#define REG_SC_BK12_0F_L                        (0x131200 | 0x0F<< 1)
#define REG_SC_BK12_30_L                        (0x131200 | 0x30<< 1)
#define REG_SC_BK12_33_L                        (0x131200 | 0x33<< 1)
#define REG_SC_BK12_3A_L                        (0x131200 | 0x3A<< 1)
#define REG_SC_BK22_2A_L                        (0x132200 | 0x2A<< 1)
#define REG_SC_BK22_7A_L                        (0x132200 | 0x7A<< 1)
#define REG_SC_BK22_7C_L                        (0x132200 | 0x7C<< 1)
#define REG_SC_BK22_7E_L                        (0x132200 | 0x7E<< 1)

#define REG_SC_BK2A_02_L                        (0x132A00 | 0x02<< 1)
#define REG_SC_BK2A_29_L                        (0x132A00 | 0x29<< 1)
#define REG_SC_BK2A_2A_L                        (0x132A00 | 0x2A<< 1)
#define REG_SC_BK2A_2F_L                        (0x132A00 | 0x2F<< 1)
#define REG_SC_BK2A_7C_L                        (0x132A00 | 0x7C<< 1)
#define REG_SC_BK2A_7D_L                        (0x132A00 | 0x7D<< 1)
#define REG_SC_BK2A_7E_L                        (0x132A00 | 0x7E<< 1)


#define REG_SC_BK30_01_L                        (0x133000 | (0x01<< 1))
#define REG_SC_BK30_02_L                        (0x133000 | (0x02<< 1))
#define REG_SC_BK30_02_H                        (0x133000 | ((0x02<< 1)+1))
#define REG_SC_BK30_03_L                        (0x133000 | (0x03<< 1))
#define REG_SC_BK30_04_L                        (0x133000 | (0x04<< 1))

#define REG_SC_BK30_06_L                        (0x133000 | (0x06<< 1))
#define REG_SC_BK30_06_H                        (0x133000 | ((0x06<< 1)+1))
#define REG_SC_BK30_07_L                        (0x133000 | (0x07<< 1))
#define REG_SC_BK30_07_H                        (0x133000 | ((0x07<< 1)+1))
#define REG_SC_BK30_08_L                        (0x133000 | (0x08<< 1))
#define REG_SC_BK30_08_H                        (0x133000 | ((0x08<< 1)+1))
#define REG_SC_BK30_09_L                        (0x133000 | (0x09<< 1))
#define REG_SC_BK30_09_H                        (0x133000 | ((0x09<< 1)+1))
#define REG_SC_BK30_0A_L                        (0x133000 | (0x0A<< 1))
#define REG_SC_BK30_0A_H                        (0x133000 | ((0x0A<< 1)+1))
#define REG_SC_BK30_0B_L                        (0x133000 | (0x0B<< 1))
#define REG_SC_BK30_0B_H                        (0x133000 | ((0x0B<< 1)+1))
#define REG_SC_BK30_0C_L                        (0x133000 | (0x0C<< 1))
#define REG_SC_BK30_0C_H                        (0x133000 | ((0x0C<< 1)+1))
#define REG_SC_BK30_0D_L                        (0x133000 | (0x0D<< 1))
#define REG_SC_BK30_0D_H                        (0x133000 | ((0x0D<< 1)+1))
#define REG_SC_BK30_0E_L                        (0x133000 | (0x0E<< 1))
#define REG_SC_BK30_0E_H                        (0x133000 | ((0x0E<< 1)+1))
#define REG_SC_BK30_0F_L                        (0x133000 | (0x0F<< 1))
#define REG_SC_BK30_0F_H                        (0x133000 | ((0x0F<< 1)+1))

#define REG_SC_BK62_06_L                        (0x136200 | 0x06<< 1)
#define REG_SC_BK62_0E_L                        (0x136200 | 0x0E<< 1)
#define REG_SC_BK62_08_L                        (0x136200 | 0x08<< 1)
#define REG_SC_BK63_61_L                        (0x136300 | 0x61<< 1)

#define _BIT0                                   (0x0001)
#define _BIT1                                   (0x0002)
#define _BIT2                                   (0x0004)
#define _BIT3                                   (0x0008)
#define _BIT4                                   (0x0010)
#define _BIT5                                   (0x0020)
#define _BIT6                                   (0x0040)
#define _BIT7                                   (0x0080)

#define L_BK_VOP(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_VOP << 8) | (_x_ << 1))
#define L_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | (_x_ << 1))
#define H_BK_DLC(_x_)                           (REG_SCALER_BASE | (REG_SC_BK_DLC << 8) | ((_x_ << 1)+1))
#define L_BK_OP(_x_)                            (REG_SCALER_BASE | (REG_SC_BK_OP << 8) | (_x_ << 1))

#define REG_SCALER_BASE                         0x130000
#define REG_SC_BK_VOP                           0x10
#define REG_SC_BK_DLC                           0x1A
#define REG_SC_BK_OP                            0x20

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(mstar_pm_base + (addr << 1))))
#else
#define REG_ADDR(addr)                          (*((volatile unsigned short int*)(0xFD000000 + (addr << 1))))
#endif

#if XC_FRC_R2_SW_TOGGLE
#define REG_FRC_CPUINT_BASE 					(REG_INT_FRC_BASE + (0x640UL<<1)) 
#define FRC_CPU_INT_REG(address)		 		(*((volatile MS_U16 *)(REG_FRC_CPUINT_BASE + ((address)<<2) )))
#define REG_FRCINT_HKCPUFIRE					0x0000UL //hst0 to hst1
#define INT_HKCPU_FRCR2_INPUT_SYNC				BIT1
#define INT_HKCPU_FRCR2_OUTPUT_SYNC				BIT2
#endif

// read 2 byte
#define REG_RR(_reg_)                           ({REG_ADDR(_reg_);})

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_)  & 0x00FF) | ((_val_) << 8); }while(0)

 // write 2 byte
#define REG_W2B(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) =(_val_) ; }while(0)

#define MAIN_WINDOW                             0
#define SUB_WINDOW                              1
#define msDlc_FunctionExit()
#define msDlc_FunctionEnter()

#define DLC_DEFLICK_BLEND_FACTOR                32UL
#define XC_DLC_ALGORITHM_KERNEL                 2

#define XC_DLC_SET_DLC_CURVE_BOTH_SAME          0   // 0:Main and Sub use the same DLC curve
#endif


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
INTERFACE void MHal_XC_W2BYTE(DWORD u32Reg, WORD u16Val );
INTERFACE WORD MHal_XC_R2BYTE(DWORD u32Reg );
INTERFACE void MHal_XC_W2BYTEMSK(DWORD u32Reg, WORD u16Val, WORD u16Mask );
INTERFACE WORD MHal_XC_R2BYTEMSK(DWORD u32Reg, WORD u16Mask );
INTERFACE BOOL MHal_XC_IsBlackVideoEnable( MS_BOOL bWindow );
INTERFACE void MHal_XC_FRCR2SoftwareToggle( void );

#endif // _MHAL_TEMP_H_

