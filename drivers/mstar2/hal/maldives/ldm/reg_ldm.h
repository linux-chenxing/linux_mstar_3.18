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
/// file    reg_ldm.h
/// @brief  local dimming Module Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_XC_LD_H_
#define _REG_XC_LD_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define MHAL_LD_PACKLENGTH    (16)    //manhattan 32, monaco 16, muji 16
#define MHAL_LD_MIU_BUS       (16)

#define REG_LD_ENABLE           (0x132E00 | (0x02))
#define REG_LD_ENABLE_BIT       (BIT0)

#define REG_LD_LDFW           (0x132E00 | (0x06))
#define REG_LD_LDFW_BITS       (0x1F)

//这两个还要修改已经修改
#define REG_LD_LDFH           (0x132E00 | (0x07))
#define REG_LD_LDFH_BITS_L       (0xE0)
#define REG_LD_LDFH_BITS_H       (0x03)
#define	REG_LD_LDFH_BITS 		(0x03E0)


#define REG_LD_LDFA0           (0x132E00 | (0x08))
#define REG_LD_LDFA1           (0x132E00 | (0x0C))
#define REG_LD_LDFAL0           (0x132E00 | (0x08))
#define REG_LD_LDFAL1           (0x132E00 | (0x0C))

//maldives没有这两个
#define REG_LD_LDFAR0           (0x132E00 | (0x10))
#define REG_LD_LDFAR1           (0x132E00 | (0x14))

#define REG_LD_LDFA_BITS       (0x07FFFFFF)

#define REG_LD_HSDRATIO           (0x132E00 | (0x32))
#define REG_LD_VSDRATIO           (0x132E00 | (0x36))
#define REG_LD_SDRATIO_BITS       (0x0007FFFF)



#define REG_LD_LDFHLPFTHD      (0x132E00 | (0x3A))
#define REG_LD_LDFHLPFEN       (0x132E00 | (0x3B))
#define REG_LD_LDFHLPFEN_BIT      (BIT4)


#define REG_LD_LDFHLPFSTEP       (0x132E00 | (0x3B))
#define REG_LD_LDFHLPFSTEP_BIT      (BIT5)

#define REG_LD_LDFDCMAXALP      (0x132E00 | (0x3E))
#define REG_LD_LDFDCMAXALP_BITS      (0x0F)


#define REG_LD_LDFPWMODEEN           (0x132E00 | (0x3E))
#define REG_LD_LDFPWMODEEN_BIT       (BIT7)


#define REG_LD_LDFWDCMAXEN           (0x132E00 | (0x3F))
#define REG_LD_LDFWDCMAXEN_BIT       (BIT0)

#define REG_LD_LDFWDCMAXOFEN           (0x132E00 | (0x3F))
#define REG_LD_LDFWDCMAXOFEN_BIT       (BIT1)

//dummy0 h22[0:11]
#define REG_LD_SWPULID           (0x132E00 | (0x44))
#define REG_LD_SWPULID_BITS       (0x0FFF)

//dummy0 h22[12:15]
#define REG_LD_SWPULMODE           (0x132E00 | (0x45))
#define REG_LD_SWPULMODE_BITS       (0xF0)
#define REG_LD_SWPULMODE_SHIFT       (4)

//dummy1 h23[0:7]
#define REG_LD_SWPULLEDINTENS           (0x132E00 | (0x46))

//dummy1 h23[8:15]
#define REG_LD_SWPULLDBINTENS           (0x132E00 | (0x47))

//dummy2 h24[0:7]
#define REG_LD_SWSPAFILTSTREN           (0x132E00 | (0x48))

//dummy2 h24[8:15]
#define REG_LD_SWTEMFILTSTRENDN           (0x132E00 | (0x49))

//dummy3 h25[0:7]
#define REG_LD_SWLDSTREN           (0x132E00 | (0x4A))

//dummy3 h25[8:15]
#define REG_LD_SWGDSTREN           (0x132E00 | (0x4B))

//dummy14 h28[0:15]
#define REG_LD_SWMAXTRD           (0x132E00 | (0x50))

//reg_ratio_h_out h30[0:19]
#define REG_LD_OHSURATIO          (0x132E00 | (0x60))
#define REG_LD_OHSURATIO_BITS       (0x7FFFF)

//reg_ratio_v_out h32[0:19]
#define REG_LD_OVSURATIO           (0x132E00 | (0x64))
#define REG_LD_OVSURATIO_BITS       (0x7FFFF)

//reg_ratio_h_lsf_cbmode h34[0:15]
#define REG_LD_LSFCBHRATIO           (0x132E00 | (0x68))

//reg_ratio_v_lsf_cbmode h35[0:15]
#define REG_LD_LSFCBVRATIO           (0x132E00 | (0x6A))

//reg_edge2d_gain_en h37[0]
#define REG_LD_EDGE2DGAINEN           (0x132E00 | (0x6E))
#define REG_LD_EDGE2DGAINEN_BIT       (BIT0)
//reg_edge2d_en h37[1]
#define REG_LD_EDGE2DEN           (0x132E00 | (0x6E))
#define REG_LD_EDGE2DEN_BIT       (BIT1)

//reg_edge2d_direct_type_en h37[2]
#define REG_LD_EDGE2DIRTYDEN           (0x132E00 | (0x6E))
#define REG_LD_EDGE2DDIRTYEN_BIT       (BIT2)

//reg_new_edge2d_en h37[8]
#define REG_LD_EDGE2DLCTYEN           (0x132E00 | (0x6F))
#define REG_LD_EDGE2DLCTYEN_BIT       (BIT0)

//reg_lsf_h_init h38[0:19]
#define REG_LD_LSFHINITP           (0x132E00 | (0x70))
#define REG_LD_LSFHINITP_BITS       (0x7FFFF)

//reg_lsf_h_init h3A[0:19]
#define REG_LD_LSFVINITP           (0x132E00 | (0x74))
#define REG_LD_LSFVINITP_BITS       (0x7FFFF)

//reg_lsf_h_shift h3C[0]
#define REG_LD_LSFHBD           (0x132E00 | (0x78))
#define REG_LD_LSFHBD_BIT       (BIT0)
//reg_lsf_h_shift h3C[1]
#define REG_LD_LSFVBD           (0x132E00 | (0x78))
#define REG_LD_LSFVBD_BIT       (BIT1)

#define REG_LD_EDGELEVSHIFT           (0x132E00 | (0x78))
#define REG_LD_EDGELEVSHIFT_MASK       (0x0C)
#define REG_LD_EDGELEVSHIFT_BITS       (2)

#define REG_LD_COMPBLEND           (0x132E00 | (0x78))
#define REG_LD_COMPBLEND_MSK       (0x01F0)
#define REG_LD_COMPBLEND_SHIFT       (4)

#define REG_LD_BLENDDITHEREN           (0x132E00 | (0x79))
#define REG_LD_BLENDDITHEREN_BIT       (BIT2)

#define REG_LD_COMPDITHEREN           (0x132E00 | (0x79))
#define REG_LD_COMPDITHEREN_BIT       (BIT5)

#define REG_LD_COMPLUTW           (0x132E00 | (0x7A))

#define REG_LD_COMPLUTR           (0x132E00 | (0x7C))
#define REG_LD_COMPLUT_MSK       (0x0FFF)
#define REG_LD_COMPLUTADDR           (0x132E00 | (0x7E))
#define REG_LD_COMPWEN          (0x132E00 | (0x7F))
#define REG_LD_COMPWEN_BIT       (BIT2)
#define REG_LD_COMPREN          (0x132E00 | (0x7F))
#define REG_LD_COMPREN_BIT       (BIT3)
#define REG_LD_COMPMOD          (0x132E00 | (0x7F))
#define REG_LD_COMPMOD_BIT       (BIT4)

#define REG_LD_CURFRAMIDX           (0x132E00 | (80))
#define REG_LD_CURFRAMIDX_BIT       (BIT4)

#define REG_LD_LEDBLW           (0x132E00 | (0x82))
#define REG_LD_LEDBLH           (0x132E00 | (0x83))

#define REG_LD_SWTEMPFILTSTRUP           (0x132E00 | (0x87))
#define REG_LD_SWWWSPIEN           (0x132E00 | (0x88))
#define REG_LD_SWWWSPIEN_BIT       (BIT3)
#define REG_LD_SWWLDBEN           (0x132E00 | (0x88))
#define REG_LD_SWWLDBEN_BIT       (BIT7)
#define REG_LD_SWALGEN           (0x132E00 | (0x89))
#define REG_LD_SWALGEN_BIT       (BIT4)
#define REG_LD_SWRLDFEN           (0x132E00 | (0x89))
#define REG_LD_SWRLDFEN_BIT       (BIT5)

#define REG_LD_LEDTYP           (0x132E00 | (0x89))
#define REG_LD_LEDTYP_SMSK       (BIT6 | BIT7)
#define REG_LD_LEDTYP_GMSK       (0x3)
#define REG_LD_LEDTYP_SHIFT       (6)

#define REG_LD_COMPEN           (0x132E00 | (0x8C))
#define REG_LD_COMPEN_BIT       (BIT6)

//reg_bl_width h4c[0:6]
#define REG_LD_LSFBLW           (0x132E00 | (0x98))
#define REG_LD_LSFBLW_MSK       (0x7F)
//reg_bl_height h4c[8:14]
#define REG_LD_LSFBLH           (0x132E00 | (0x99))
#define REG_LD_LSFBLH_MSK       (0x7F)

#define REG_LD_LDBFBA0           (0x132E00 | (0x9A))
#define REG_LD_LDBFBA0_MSK       (0x7FFFFFF)

#define REG_LD_LDBFBA1           (0x132E00 | (0x9E))
#define REG_LD_LDBFBA1_MSK       (0x7FFFFFF)

#define REG_LD_LDBFBAL0           (0x132E00 | (0x9A))
#define REG_LD_LDBFBAL0_MSK       (0x7FFFFFF)
#define REG_LD_LDBFBAL1           (0x132E00 | (0x9E))
#define REG_LD_LDBFBAL1_MSK       (0x7FFFFFF)

//maldives没有这两个
#define REG_LD_LDBFBAR0           (0x132E00 | (0xA2))
#define REG_LD_LDBFBAR0_MSK       (0x7FFFFFF)
#define REG_LD_LDBFBAR1           (0x132E00 | (0xA6))
#define REG_LD_LDBFBAR1_MSK       (0x7FFFFFF)


#define REG_LD_TEMPFILTLTH           (0x132E00 | (0x4E))
#define REG_LD_TEMPFILTHTH           (0x132E00 | (0x4F))
#define REG_LD_TEMPFILTSTR2           (0x132E00 | (0x4C))
#define REG_LD_TEMPFILTSTR3           (0x132E00 | (0x4D))
#define REG_LD_TEMPFILTSTR4           (0x132E00 | (0x84))
#define REG_LD_TEMPFILTSTR5           (0x132E00 | (0x85))

//reg_bl_width_led h63 [0:4]
#define REG_LD_LDBLW           (0x132E00 | (0xC6))
#define REG_LD_LDBLW_MSK       (0x1F)

//reg_bl_height_led h63[5:9]
#define REG_LD_LDBLH           (0x132E00 | (0xC7))
#define REG_LD_LDBLH_MSK       (0x03E0)
#define REG_LD_LDBLH_MSK_L       (0xE0)
#define REG_LD_LDBLH_MSK_H       (0x03)



//reg_frm_width h70[0:12]
#define REG_LD_FW           (0x132E00 | (0xE0))
#define REG_LD_FW_MSK       (0x1FFF)

//reg_frm_height h71[0:11]
#define REG_LD_FH           (0x132E00 | (0xE2))
#define REG_LD_FH_MSK       (0x0FFF)

//reg_debug_mod h72
#define REG_LD_LSFOM           (0x132E00 | 0xE4)
#define REG_LD_LSFOM_MSK       (0x0E)
#define REG_LD_LSFOM_SHIFT       (1)

#define REG_LD_SWMINCLAMPV           (0x132E00 | (0x86))

#define REG_LD_SWBLIGAM           (0x132E00 | (0xED))
#define REG_LD_SWBLIGAM_MSK       (0x0F)
#define REG_LD_SWBLOGAM           (0x132E00 | (0xED))
#define REG_LD_SSWBLOGAM_MSK       (0xF0)
#define REG_LD_GSWBLOGAM_MSK       (0x0F)
#define REG_LD_SWBLOGAM_SHIFT       (4)

#define REG_LD_LINEEN           (0x132E00 | (0xEC))
#define REG_LD_LINEEN_BIT       (BIT2)

#define REG_LD_EG2DBA           (0x132E00 | (0xF2))
#define REG_LD_EG2DBA_MSK       (0x7FFFFFF)

#define REG_LD_REPORTWIN                (0x132E00 | (0xF8)) //Set report window start horizontal pel_L
#define REG_LD_REPORTWIN_MSK            (0xFFF)
#define REG_LD_REPORTWIN_SWAP_BIT       (BIT6)          //Swap report  window_L
#define REG_LD_REPORTWIN_SWAP_MSK       (0xF0)          //Swap report  window_L

#define REG_LD_TEMPMAXSPEED           (0x132E00 | (0x52))



#ifndef BIT
#define BIT(x) (1UL << (x))
#endif

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_ADDR(addr)  (*((volatile U16 *)(mstar_pm_base + (addr << 1))))
#else
#define REG_ADDR(addr)  (*((volatile U16 *)(0xFD000000 + (addr << 1))))
#endif


// read 2 byte
#define REG_R2B(_reg_)          (REG_ADDR(_reg_))

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_reg_)  & 0x00FF) | ((_val_) << 8); }while(0)

 // write 2 byte
#define REG_W2B(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) =(_val_) ; }while(0)


#endif // _REG_XC_LD_H_
