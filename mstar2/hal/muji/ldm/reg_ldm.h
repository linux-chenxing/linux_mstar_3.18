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

#define LD_SUPPORT_1XN_NX1
#define LD_SUPPORT_60HZ
#define LD_HW_PINGPONG_MODE 1


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define MHAL_LD_PACKLENGTH    (16)    //manhattan 32, monaco 16, muji 16
#define MHAL_LD_MIU_BUS       (16)

#define REG_LD_ENABLE           (LD_BK | (0x02))
#define REG_LD_ENABLE_BIT       (BIT0)

#define REG_LD_LDFW           (LD_BK | (0x06))
#define REG_LD_LDFW_BITS       (0x1F)
//
#define REG_LD_LDFH           (LD_BK | (0x07))
#define REG_LD_LDFH_BITS_L       (0xE0)
#define REG_LD_LDFH_BITS_H       (0x03)
#define REG_LD_LDFH_BITS        (0x03E0)


#define REG_LD_LDFA0           (LD_BK | (0x08))
#define REG_LD_LDFA1           (LD_BK | (0x0C))
#define REG_LD_LDFAL0           (LD_BK | (0x08))
#define REG_LD_LDFAL1           (LD_BK | (0x0C))
#define REG_LD_LDFAR0           (LD_BK | (0x10))
#define REG_LD_LDFAR1           (LD_BK | (0x14))
#define REG_LD_LDFA_BITS       (0x07FFFFFF)

#define REG_LD_HSDRATIO           (LD_BK | (0x32))
#define REG_LD_VSDRATIO           (LD_BK | (0x36))
#define REG_LD_SDRATIO_BITS       (0x0007FFFF)

#define REG_LD_LDFHLPFTHD      (LD_BK | (0x3A))
#define REG_LD_LDFHLPFEN       (LD_BK | (0x3B))
#define REG_LD_LDFHLPFEN_BIT      (BIT4)
#define REG_LD_LDFHLPFSTEP       (LD_BK | (0x3B))
#define REG_LD_LDFHLPFSTEP_BIT      (BIT5)
#define REG_LD_LDFDCMAXALP      (LD_BK | (0x3E))
#define REG_LD_LDFDCMAXALP_BITS      (0x0F)
#define REG_LD_LDFPWMODEEN           (LD_BK | (0x3E))
#define REG_LD_LDFPWMODEEN_BIT       (BIT7)
#define REG_LD_LDFWDCMAXEN           (LD_BK | (0x3F))
#define REG_LD_LDFWDCMAXEN_BIT       (BIT0)
#define REG_LD_LDFWDCMAXOFEN           (LD_BK | (0x3F))
#define REG_LD_LDFWDCMAXOFEN_BIT       (BIT1)

#define REG_LD_SWPULID           (LD_BK | (0x44))
#define REG_LD_SWPULID_BITS       (0x0FFF)
#define REG_LD_SWPULMODE           (LD_BK | (0x45))
#define REG_LD_SWPULMODE_BITS       (0xF0)
#define REG_LD_SWPULMODE_SHIFT       (4)
#define REG_LD_SWPULLEDINTENS           (LD_BK | (0x46))
#define REG_LD_SWPULLDBINTENS           (LD_BK | (0x47))

#define REG_LD_SWSPAFILTSTREN           (LD_BK | (0x48))
#define REG_LD_SWTEMFILTSTRENDN           (LD_BK | (0x49))
#define REG_LD_SWLDSTREN           (LD_BK | (0x4A))
#define REG_LD_SWGDSTREN           (LD_BK | (0x4B))
#define REG_LD_SWMAXTRD           (LD_BK | (0x50))

#define REG_LD_OHSURATIO          (LD_BK | (0x60))
#define REG_LD_OHSURATIO_BITS       (0x7FFFF)
#define REG_LD_OVSURATIO           (LD_BK | (0x64))
#define REG_LD_OVSURATIO_BITS       (0x7FFFF)

#define REG_LD_LSFCBHRATIO           (LD_BK | (0x68))
#define REG_LD_LSFCBVRATIO           (LD_BK | (0x6A))

#define REG_LD_EDGE2DGAINEN           (LD_BK | (0x6E))
#define REG_LD_EDGE2DGAINEN_BIT       (BIT0)
#define REG_LD_EDGE2DEN           (LD_BK | (0x6E))
#define REG_LD_EDGE2DEN_BIT       (BIT1)
#define REG_LD_EDGE2DIRTYDEN           (LD_BK | (0x6E))
#define REG_LD_EDGE2DDIRTYEN_BIT       (BIT2)
#define REG_LD_EDGE2DLCTYEN           (LD_BK | (0x6F))
#define REG_LD_EDGE2DLCTYEN_BIT       (BIT0)

#define REG_BL_DEPACK16B_MOD            (LD_BK | (0x6E))
#define REG_BL_DEPACK16B_MOD_BIT        (BIT5)

#define REG_LD_LSFHINITP           (LD_BK | (0x70))
#define REG_LD_LSFHINITP_BITS       (0x7FFFF)
#define REG_LD_LSFHINITP_R           (LD_BK | (0xAA))
#define REG_LD_LSFHINITP_BITS_R        (0x7FFFF)
#define REG_LD_LSFVINITP           (LD_BK | (0x74))
#define REG_LD_LSFVINITP_BITS       (0x7FFFF)
#define REG_LD_LSFHBD           (LD_BK | (0x78))
#define REG_LD_LSFHBD_BIT       (BIT0)
#define REG_LD_LSFHBD_R           (LD_BK | (0x79))
#define REG_LD_LSFHBD_R_BIT       (BIT7)
#define REG_LD_LSFVBD           (LD_BK | (0x78))
#define REG_LD_LSFVBD_BIT       (BIT1)
#define REG_LD_LSFSF           (LD_BK | (0x8E))
#define REG_LD_LSFML           (LD_BK | (0x8F))

#define REG_LD_EDGELEVSHIFT           (LD_BK | (0x78))
#define REG_LD_EDGELEVSHIFT_MASK       (0x0C)
#define REG_LD_EDGELEVSHIFT_BITS       (2)

#define REG_LD_COMPBLEND           (LD_BK | (0x78))
#define REG_LD_COMPBLEND_MSK       (0x01F0)
#define REG_LD_COMPBLEND_SHIFT       (4)
#define REG_LD_BLENDDITHEREN           (LD_BK | (0x79))
#define REG_LD_BLENDDITHEREN_BIT       (BIT2)
#define REG_LD_COMPDITHEREN           (LD_BK | (0x79))
#define REG_LD_COMPDITHEREN_BIT       (BIT5)
#define REG_LD_COMPLUTW           (LD_BK | (0x7A))
#define REG_LD_COMPLUTR           (LD_BK | (0x7C))

#define REG_LD_COMPLUTR           (LD_BK | (0x7C))
#define REG_LD_COMPLUT_MSK       (0x0FFF)
#define REG_LD_COMPLUTADDR           (LD_BK | (0x7E))
#define REG_LD_COMPWEN          (LD_BK | (0x7F))
#define REG_LD_COMPWEN_BIT       (BIT2)
#define REG_LD_COMPREN          (LD_BK | (0x7F))
#define REG_LD_COMPREN_BIT       (BIT3)
#define REG_LD_COMPMOD          (LD_BK | (0x7F))
#define REG_LD_COMPMOD_BIT       (BIT4)

#define REG_LD_CURFRAMIDX           (LD_BK | (0x80))
#define REG_LD_CURFRAMIDX_BIT       (BIT4)

#define REG_LD_LEDBLW           (LD_BK | (0x82))
#define REG_LD_LEDBLH           (LD_BK | (0x83))

#define REG_LD_SWTEMPFILTSTRUP           (LD_BK | (0x87))
#define REG_LD_SWWWSPIEN           (LD_BK | (0x88))
#define REG_LD_SWWWSPIEN_BIT       (BIT3)
#define REG_LD_SWWLDBEN           (LD_BK | (0x88))
#define REG_LD_SWWLDBEN_BIT       (BIT7)
#define REG_LD_SWALGEN           (LD_BK | (0x89))
#define REG_LD_SWALGEN_BIT       (BIT4)
#define REG_LD_SWRLDFEN           (LD_BK | (0x89))
#define REG_LD_SWRLDFEN_BIT       (BIT5)

#define REG_LD_LEDTYP           (LD_BK | (0x89))
#define REG_LD_LEDTYP_SMSK       (BIT6 | BIT7)
#define REG_LD_LEDTYP_GMSK       (0x3)
#define REG_LD_LEDTYP_SHIFT       (6)

#define REG_LD_COMPEN           (LD_BK | (0x8C))
#define REG_LD_COMPEN_BIT       (BIT6)

#define REG_LD_LSFBLW           (LD_BK | (0x98))
#define REG_LD_LSFBLW_MSK       (0x1F)
//reg_bl_height h4c[8:14]
#define REG_LD_LSFBLH           (LD_BK | (0x99))
#define REG_LD_LSFBLH_BITS_L       (0xE0)
#define REG_LD_LSFBLH_BITS_H       (0x03)
#define REG_LD_LSFBLH_BITS      (0x03E0)

#define REG_LD_LDBFBA0           (LD_BK | (0x9A))
#define REG_LD_LDBFBA0_MSK       (0x7FFFFFF)

#define REG_LD_LDBFBA1           (LD_BK | (0x9E))
#define REG_LD_LDBFBA1_MSK       (0x7FFFFFF)

#define REG_LD_LDBFBAL0           (LD_BK | (0x9A))
#define REG_LD_LDBFBAL0_MSK       (0x7FFFFFF)
#define REG_LD_LDBFBAL1           (LD_BK | (0x9E))
#define REG_LD_LDBFBAL1_MSK       (0x7FFFFFF)

#define REG_LD_LDBFBAR0           (LD_BK | (0xA2))
#define REG_LD_LDBFBAR0_MSK       (0x7FFFFFF)
#define REG_LD_LDBFBAR1           (LD_BK | (0xA6))
#define REG_LD_LDBFBAR1_MSK       (0x7FFFFFF)


#define REG_LD_TEMPFILTLTH           (LD_BK | (0x4E))
#define REG_LD_TEMPFILTHTH           (LD_BK | (0x4F))
#define REG_LD_TEMPFILTSTR2           (LD_BK | (0x4C))
#define REG_LD_TEMPFILTSTR3           (LD_BK | (0x4D))
#define REG_LD_TEMPFILTSTR4           (LD_BK | (0x84))
#define REG_LD_TEMPFILTSTR5           (LD_BK | (0x85))

#define REG_BL_WIDTH_DMA                (LD_BK | (0xC0))
#define REG_BL_WIDTH_DMA_MASK           (0x3F)
#define REG_BL_HEIGHT_DMA               (LD_BK | (0xC1))
#define REG_BL_HEIGHT_DMA_MASK          (0x3F)

#define REG_BL_GLOBE                    (LD_BK | (0x96))
#define REG_BL_GLOBE_MSK                (0xFF)
#define REG_BL_GLOBE_MOD                (LD_BK | (0x97))
#define REG_BL_GLOBE_MOD_MSK            (BIT0)
#define REG_BL_DATA_ALIG                (LD_BK | (0x97))
#define REG_BL_DATA_ALIG_MSK            (0x06)


//reg_bl_width_led h63 [0:4]
#define REG_LD_LDBLW           (LD_BK | (0xC6))
#define REG_LD_LDBLW_MSK       (0x1F)

#define REG_LD_LDBLH           (LD_BK | (0xC7))
#define REG_LD_LDBLH_MSK       (0x03E0)
#define REG_LD_LDBLH_MSK_L       (0xE0)
#define REG_LD_LDBLH_MSK_H       (0x03)



//reg_frm_width h70[0:12]
#define REG_LD_FW           (LD_BK | (0xE0))
#define REG_LD_FW_MSK       (0x1FFF)
#define REG_LD_FH           (LD_BK | (0xE2))
#define REG_LD_FH_MSK       (0x0FFF)

#define REG_LD_LSFOM           (LD_BK | 0xE4)
#define REG_LD_LSFOM_MSK       (0x0E)
#define REG_LD_LSFOM_SHIFT       (1)

#define REG_LD_SWMINCLAMPV           (LD_BK | (0x86))

#define REG_LD_SWBLIGAM           (LD_BK | (0xED))
#define REG_LD_SWBLIGAM_MSK       (0x0F)
#define REG_LD_SWBLOGAM           (LD_BK | (0xED))
#define REG_LD_SSWBLOGAM_MSK       (0xF0)
#define REG_LD_GSWBLOGAM_MSK       (0x0F)
#define REG_LD_SWBLOGAM_SHIFT       (4)

#define REG_LD_LINEEN           (LD_BK | (0xEC))
#define REG_LD_LINEEN_BIT       (BIT2)

#define REG_LD_EG2DBA           (LD_BK | (0xF2))
#define REG_LD_EG2DBA_MSK       (0x7FFFFFF)

#define REG_LD_REPORTWIN                (LD_BK | (0xF8)) //Set report window start horizontal pel_L
#define REG_LD_REPORTWIN_MSK            (0xFFF)
#define REG_LD_REPORTWIN_SWAP_BIT       (BIT6)          //Swap report  window_L
#define REG_LD_REPORTWIN_SWAP_MSK       (0xF0)          //Swap report  window_L

#define REG_LD_TEMPMAXSPEED             (LD_BK | (0x52))

#define REG_BASEADDR_LED_OFFSET         (LD_BK | (0xFC))
#define REG_BASEADDR_LED_OFFSET_BITS    (0x07FFFFFF)

#define  MIU_PACK_NUM_MAX               (0x1F)
#define  REG_MIU_PACK_OFFSET_0          (LD_BK|0xC8)
#define  REG_MIU_PACK_OFFSET_0_MSK      (0x1F)
#define  REG_MIU_PACK_LENGTH_0          (LD_BK|0xCA)
#define  REG_MIU_PACK_LENGTH_0_MSK      (0x1F)
#define  REG_DMA0_EN                    (LD_BK|0xCB)
#define  REG_DMA_EN_BIT                 (BIT0)

#define  REG_MIU_PACK_OFFSET_1          (LD_BK|0xCC)
#define  REG_MIU_PACK_OFFSET_1_MSK      (0x1F)
#define  REG_MIU_PACK_LENGTH_1          (LD_BK|0xCE)
#define  REG_MIU_PACK_LENGTH_1_MSK      (0x1F)
#define  REG_DMA1_EN                    (LD_BK|0xCF)

#define  REG_MIU_PACK_OFFSET_2          (LD_BK|0xD0)
#define  REG_MIU_PACK_OFFSET_2_MSK      (0x1F)
#define  REG_MIU_PACK_LENGTH_2          (LD_BK|0xD2)
#define  REG_MIU_PACK_LENGTH_2_MSK      (0x1F)
#define  REG_DMA2_EN                    (LD_BK|0xD3)

#define  REG_MIU_PACK_OFFSET_3          (LD_BK|0xD4)
#define  REG_MIU_PACK_OFFSET_3_MSK      (0x1F)
#define  REG_MIU_PACK_LENGTH_3          (LD_BK|0xD6)
#define  REG_MIU_PACK_LENGTH_3_MSK      (0x1F)
#define  REG_DMA3_EN                    (LD_BK|0xD7)

#define REG_DMA_YOFF_END_0              (LD_BK|0xD8)
#define REG_DMA_YOFF_END_MSK            (0x3F)
#define REG_DMA_YOFF_END_1              (LD_BK|0xDA)
#define REG_DMA_YOFF_END_2              (LD_BK|0xDC)
#define REG_DMA_YOFF_END_3              (LD_BK|0xDE)

#define REG_SPI_DATA_INV                (LD_BK|0xF7)
#define REG_SPI_DATA_INV_MSK            (0x60)
#define REG_SPI_DATA_INV_SHIFT          (5)

//LD2
#define REG_LD_LSFCBHINIT               (LD2_BK | (0x04))
#define REG_LD_LEDSF                    (LD2_BK | (0x06))

#define REG_LD_LSFENCEN                 (LD2_BK | (0x40))
#define REG_LD_LSFENCEN_MSK             (0x0001)
#define REG_LD_COMPLOWGAINEN            (LD2_BK | (0x40))
#define REG_LD_COMPLOWGAINEN_MSK        (0x0004)
#define REG_LD_COMPLOWGAINEN_SHIFT      (2)
#define REG_LD_SATEN                   (LD2_BK | (0xA0))
#define REG_LD_SATEN_MSK                (0x0080)
#define REG_LD_SATEN_SHIFT             (7)
#define REG_LD_SATA                    (LD2_BK | (0xA2))
#define REG_LD_SATA_MSK                 (0x00FF)
#define REG_LD_SATC                    (LD2_BK | (0xAA))
#define REG_LD_SATC_MSK                 (0x0FFF)
#define REG_LD_SATCSFT                 (LD2_BK | (0xAC))
#define REG_LD_SATCSFT_MSK              (0x000F)

#define REG_LD_COMPLOWLUTADDR        (LD2_BK | (0x7E))
#define REG_LD_COMPLOWLUTW           (LD2_BK | (0x7A))
#define REG_LD_COMPLOWLUTR           (LD2_BK | (0x7C))
#define REG_LD_COMPLUT_MSK          (0x0FFF)
#define REG_LD_COMPLOWWEN          (LD2_BK | (0x7F))
#define REG_LD_COMPWEN_BIT          (BIT2)
#define REG_LD_COMPLOWREN          (LD2_BK | (0x7F))
#define REG_LD_COMPREN_BIT          (BIT3)
#define REG_LD_COMPLOWMOD          (LD2_BK | (0x7F))
#define REG_LD_COMPMOD_BIT          (BIT4)

//Add read/write backlight gamma  table of local dimming by MSTV_TOOL.
#define REG_LD_SWRSTGAMMAEN             (LD_BK | (0xEC))
#define REG_LD_SWRSTGAMMAEN_BIT         (BIT4)
#define REG_LD_SWRSTGAMMAMODE           (LD_BK | (0xEC))
#define REG_LD_SWRSTGAMMAMODE_BIT       (BIT5)
#define REG_LD_SWRSTGAMMARW             (LD_BK | (0xEC))
#define REG_LD_SWRSTGAMMARW_BIT         (BIT6)
#define REG_LD_SWRSTGAMMAINDEX          (LD2_BK | (0x50))
#define REG_LD_SWRSTGAMMADATA           (LD2_BK | (0x51))



#define REG_LD_LR_GBDEEN          (LD_LR_GB_BK | (0x2F))    //reg_gb_de_en
#define REG_LD_LR_GBDEEN_BIT      (BIT0)
#define REG_LD_LR_GBDEXST_L       (LD_LR_GB_BK | (0x84))    //reg_gb_de_xst_l
#define REG_LD_LR_GBDEXST_L_MSK         (0x1FFF)
#define REG_LD_LR_GBDEXEND_L      (LD_LR_GB_BK | (0x86))    //reg_gb_de_xend_l
#define REG_LD_LR_GBDEXEND_L_MSK        (0x1FFF)
#define REG_LD_LR_GBDEXST_R       (LD_LR_GB_BK | (0x88))    //reg_gb_de_xst_r
#define REG_LD_LR_GBDEXST_R_MSK         (0x1FFF)
#define REG_LD_LR_GBDEXEND_R      (LD_LR_GB_BK | (0x8A))    //reg_gb_de_xend_r
#define REG_LD_LR_GBDEXEND_R_MSK        (0x1FFF)

#define REG_LD_SRAM_PD_ENABLE           (LD_SRAM_PD_BANK |(0x24))
#define REG_LD_SRAM_PD_ENABLE_MSK       (0x0003)
#define REG_LD_CLOCK_GATE               (LD_SRAM_PD_BANK |(0xA0))
#define REG_LD_CLOCK_GATE_MSK           (0x3800)
#define REG_LD_BYPASS_ENABLE            (LD_BK |(0x6E))
#define REG_LD_BYPASS_ENABLE_MSK        (0x8000)


#define REG_LD_FO_SRAM_PD_ENABLE        (LD_FO_BKC9 |(0xCA))
#define REG_LD_FO_SRAM_PD_ENABLE_MSK    (0x0001)
#define REG_LD_FO_CLOCK_GATE            (LD_BK |(0x02))
#define REG_LD_FO_CLOCK_GATE_MSK        (0x1000)




#ifndef BIT
#define BIT(x) (1UL << (x))
#endif


#if defined(LD_SUPPORT_120HZ)
#define LD_BK           0x13CE00
#define LD2_BK          0x13CF00
#else
#define LD_BK           0x132E00
#define LD2_BK          0x134E00
#endif

#define LD_LR_GB_BK     0x13CB00    //guard band IN SC register sub bank CB
#define LD_SRAM_PD_BANK 0x133F00    //SRAM power down bank
#define LD_FO_BKC9      0x13C900




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
