#include <hal_isp.h>
#include <drv_ms_isp_general.h>
#include <iq_struct.h>
#if 0
void WriteRegs_GEV2(isp_handle_t *handle, u8 *src, u32 tag)
{
    iq_cfg_info info;
    int bank, offset, mask, byte, size;
    u16 *dst;
    u16 src16;
    int tmpMask, i, j;

    static u16 MergeTHStr;
    u16 LastSrc[1] = {0};


    cfg_getcfg_info_byenum(tag, (void*)NULL, &info);

    bank = Regmap[tag].bank;
    offset = Regmap[tag].offset;
    mask = Regmap[tag].mask;
    if(info.type == eDBG_ITEM_TYPE_U8 ||
            info.type == eDBG_ITEM_TYPE_U8_ARRAY ||
            info.type == eDBG_ITEM_TYPE_U8_MATRIX
      )
        byte = 1;
    else if(info.type == eDBG_ITEM_TYPE_U16 ||
            info.type == eDBG_ITEM_TYPE_S16 ||
            info.type == eDBG_ITEM_TYPE_U16_ARRAY ||
            info.type == eDBG_ITEM_TYPE_S16_ARRAY ||
            info.type == eDBG_ITEM_TYPE_S16_MATRIX ||
            info.type == eDBG_ITEM_TYPE_U16_MATRIX
           )
        byte = 2;
    size = info.col;


    if(tag == DBG_IQ_ANTICT_TH)
    {
        MergeTHStr = src[0];
    }
    if(tag == DBG_IQ_ANTICT_SFT)
    {
        MergeTHStr = (src[0] << 8 | MergeTHStr);
        LastSrc[0] = MergeTHStr;
        byte = 2;
        mask = 0xFFFF;
    }


    // dst = ~((~dst & ~mask) | (~src & mask));
    switch(bank)
    {
        case ISP_REG_BANK0:
            dst = reg_addrW(handle->isp0_cfg, offset);
            break;
        case ISP_REG_BANK1:
            dst = reg_addrW(handle->isp1_cfg, offset);
            break;
        case ISP_REG_BANK2:
            dst = reg_addrW(handle->isp2_cfg, offset);
            break;
        case ISP_REG_BANK3:
            dst = reg_addrW(handle->isp3_cfg, offset);
            break;
        case ISP_REG_BANK4:
            dst = reg_addrW(handle->isp4_cfg, offset);
            break;
        case ISP_REG_BANK5:
            dst = reg_addrW(handle->isp5_cfg, offset);
            break;
        case ISP_REG_BANK6:
            dst = reg_addrW(handle->isp6_cfg, offset);
            break;
        case ISP_REG_BANK7:
            dst = reg_addrW(handle->isp7_cfg, offset);
            break;
        case ISP_REG_BANK8:
            dst = reg_addrW(handle->isp8_cfg, offset);
            break;
        case ISP_REG_BANK9:
            dst = reg_addrW(handle->isp9_cfg, offset);
            break;
        case ISP_REG_BANK_NONE:
            return;
    }

    for(i = 0, j = 0; i < size;)
    {
        tmpMask = mask;
        //bit shift

        //padding 8bit in 16bit =========================================================
        if(
            //lsc
            tag == DBG_IQ_LSC_R_GAIN_TABLE || tag == DBG_IQ_LSC_G_GAIN_TABLE ||
            tag == DBG_IQ_LSC_B_GAIN_TABLE ||
            //dpc
            tag == DBG_IQ_DPC_NEI_DELTA_SFT_X || tag == DBG_IQ_DPC_NEI_DELTA_GAIN ||
            tag == DBG_IQ_DPC_LIGHT_TH_ADD || tag == DBG_IQ_DPC_DARK_TH_ADD ||
            //mcnr
            tag == DBG_IQ_MCNR_DNRLUT_Y || tag == DBG_IQ_MCNR_DNRLUT_C ||
            tag == DBG_IQ_MCNR_LUMALUT_Y || tag == DBG_IQ_MCNR_LUMALUT_C ||
            //nlm
            tag == DBG_IQ_NLM_LUMA_ADAP_GAIN_LUT //check h/l
        )
        {
            //padding 8bit in 16bit
            src16 = (((u8*)src)[i]) | (((u8*)src)[i + 1] << 8);
            ((u16*)dst)[j] = src16;
            i += 2;
            j += 2; //dst 16bit data, 16bit dummy
        }
        //padding 4bit in 16bit ==========================================================
        else if(
            //dpc
            tag == DBG_IQ_DPC_SORT_LUMTBL_X ||
            //bdnr
            tag == DBG_IQ_BDNR_LUM_TABLE || tag == DBG_IQ_BDNR_TABLEY ||
            //bsnr
            tag == DBG_IQ_BSNR_TABLE_R || tag == DBG_IQ_BSNR_TABLE_G || tag == DBG_IQ_BSNR_TABLE_B ||
            //rgbir
            tag == DBG_IQ_RGBIR_OFT_RATIO_BY_Y
        )
        {

            //padding 4bit in 16bit
            src16 = (((u8*)src)[i]) | (((u8*)src)[i + 1] << 4) | (((u8*)src)[i + 2] << 8) | (((u8*)src)[i + 3] << 12);
            ((u16*)dst)[j] = src16;
            i += 4;
            j += 2; //dst 16bit data, 16bit dummy
        }
        //normal, padding 16bit in 16bit =================================================
        else
        {
            //normal, padding 16bit in 16bit
            if(byte == 1)       src16 = ((u8*)src)[i];
            else if(byte == 2)  src16 = ((u16*)LastSrc)[i]; //for TH+Str

            for(int j = 0; j < 16; j++)
            {
                if((tmpMask & 0x01) == 0)
                {
                    src16 <<= 1;
                    tmpMask >>= 1;
                }
                else break;
            }
            ((u16*)dst)[j] = ~((~((u16*)dst)[j] & ~mask) | (~src16 & mask));
            i += 1;
            j += 2; //dst 16bit data, 16bit dummy
        }
    }
}
#endif

#if 0

void WriteRegs(isp_handle_t *handle, u8 *src, u32 tag)
{
    iq_cfg_info info;
    int bank, offset, mask, byte, size;
    u16 *dst;
    u16 src16;
    int tmpMask, i, j;

    cfg_getcfg_info_byenum(tag, (void*)NULL, &info);

    bank = Regmap[tag].bank;
    offset = Regmap[tag].offset;
    mask = Regmap[tag].mask;
    if(info.type == eDBG_ITEM_TYPE_U8 ||
            info.type == eDBG_ITEM_TYPE_U8_ARRAY ||
            info.type == eDBG_ITEM_TYPE_U8_MATRIX
      )
        byte = 1;
    else if(info.type == eDBG_ITEM_TYPE_U16 ||
            info.type == eDBG_ITEM_TYPE_S16 ||
            info.type == eDBG_ITEM_TYPE_U16_ARRAY ||
            info.type == eDBG_ITEM_TYPE_S16_ARRAY ||
            info.type == eDBG_ITEM_TYPE_S16_MATRIX ||
            info.type == eDBG_ITEM_TYPE_U16_MATRIX
           )
        byte = 2;
    size = info.col;

    // dst = ~((~dst & ~mask) | (~src & mask));
    switch(bank)
    {
        case ISP_REG_BANK0:
            dst = reg_addrW(handle->isp0_cfg, offset);
            break;
        case ISP_REG_BANK1:
            dst = reg_addrW(handle->isp1_cfg, offset);
            break;
        case ISP_REG_BANK2:
            dst = reg_addrW(handle->isp2_cfg, offset);
            break;
        case ISP_REG_BANK3:
            dst = reg_addrW(handle->isp3_cfg, offset);
            break;
        case ISP_REG_BANK4:
            dst = reg_addrW(handle->isp4_cfg, offset);
            break;
        case ISP_REG_BANK5:
            dst = reg_addrW(handle->isp5_cfg, offset);
            break;
        case ISP_REG_BANK6:
            dst = reg_addrW(handle->isp6_cfg, offset);
            break;
        case ISP_REG_BANK7:
            dst = reg_addrW(handle->isp7_cfg, offset);
            break;
        case ISP_REG_BANK8:
            dst = reg_addrW(handle->isp8_cfg, offset);
            break;
        case ISP_REG_BANK9:
            dst = reg_addrW(handle->isp9_cfg, offset);
            break;
        case ISP_REG_BANK_NONE:
            return;
    }

    for(i = 0, j = 0; i < size;)
    {
        tmpMask = mask;
        //bit shift

        //padding 8bit in 16bit =========================================================
        if(
            //lsc
            tag == DBG_IQ_LSC_R_GAIN_TABLE || tag == DBG_IQ_LSC_G_GAIN_TABLE ||
            tag == DBG_IQ_LSC_B_GAIN_TABLE ||
            //dpc
            tag == DBG_IQ_DPC_NEI_DELTA_SFT_X || tag == DBG_IQ_DPC_NEI_DELTA_GAIN ||
            tag == DBG_IQ_DPC_LIGHT_TH_ADD || tag == DBG_IQ_DPC_DARK_TH_ADD ||
            //mcnr
            tag == DBG_IQ_MCNR_DNRLUT_Y || tag == DBG_IQ_MCNR_DNRLUT_C ||
            tag == DBG_IQ_MCNR_LUMALUT_Y || tag == DBG_IQ_MCNR_LUMALUT_C ||
            //nlm
            tag == DBG_IQ_NLM_LUMA_ADAP_GAIN_LUT //check h/l
        )
        {
            //padding 8bit in 16bit
            src16 = (((u8*)src)[i]) | (((u8*)src)[i + 1] << 8);
            ((u16*)dst)[j] = src16;
            i += 2;
            j += 2; //dst 16bit data, 16bit dummy
        }
        //padding 4bit in 16bit ==========================================================
        else if(
            //dpc
            tag == DBG_IQ_DPC_SORT_LUMTBL_X ||
            //bdnr
            tag == DBG_IQ_BDNR_LUM_TABLE || tag == DBG_IQ_BDNR_TABLEY ||
            //bsnr
            tag == DBG_IQ_BSNR_TABLE_R || tag == DBG_IQ_BSNR_TABLE_G || tag == DBG_IQ_BSNR_TABLE_B ||
            //rgbir
            tag == DBG_IQ_RGBIR_OFT_RATIO_BY_Y
        )
        {

            //padding 4bit in 16bit
            src16 = (((u8*)src)[i]) | (((u8*)src)[i + 1] << 4) | (((u8*)src)[i + 2] << 8) | (((u8*)src)[i + 3] << 12);
            ((u16*)dst)[j] = src16;
            i += 4;
            j += 2; //dst 16bit data, 16bit dummy
        }
        //normal, padding 16bit in 16bit =================================================
        else
        {
            //normal, padding 16bit in 16bit
            if(byte == 1)       src16 = ((u8*)src)[i];
            else if(byte == 2)  src16 = ((u16*)src)[i];

            for(int j = 0; j < 16; j++)
            {
                if((tmpMask & 0x01) == 0)
                {
                    src16 <<= 1;
                    tmpMask >>= 1;
                }
                else break;
            }
            ((u16*)dst)[j] = ~((~((u16*)dst)[j] & ~mask) | (~src16 & mask));
            i += 1;
            j += 2; //dst 16bit data, 16bit dummy
        }
    }
}

////////// IQ Cfgs ///////////////
int ISP_IQ_FPN_Cfg(isp_handle_t *handle, IQ_FPN_CFG cfg)
{

    int str = DBG_IQ_FPN_START + 1;
    int cnt = DBG_IQ_FPN_END - DBG_IQ_FPN_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_FPN_offset[i]], str + i);
    /*
        handle->isp4_cfg->reg_sw_offset_on = 1;
        handle->isp4_cfg->reg_fpn_width = 0xFFF;
        handle->isp4_cfg->reg_fpn_height = 0;
    */
    return SUCCESS;
}

int ISP_IQ_SDC_Cfg(isp_handle_t *handle, IQ_SDC_CFG cfg)
{

    int str = DBG_IQ_SDC_START + 1;
    int cnt = DBG_IQ_SDC_END - DBG_IQ_SDC_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_SDC_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_DPC_Cfg(isp_handle_t *handle, IQ_DPC_CFG cfg)
{

    int str = DBG_IQ_DPC_START + 1;
    int cnt = DBG_IQ_DPC_END - DBG_IQ_DPC_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_DPC_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_GE_Cfg(isp_handle_t *handle, IQ_GE_CFG cfg)
{

    int str = DBG_IQ_GE_START + 1;
    int cnt = DBG_IQ_GE_END - DBG_IQ_GE_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_GE_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_ANTICT_Cfg(isp_handle_t *handle, IQ_ANTICT_CFG cfg)
{

    int str = DBG_IQ_ANTICT_START + 1;
    int cnt = DBG_IQ_ANTICT_END - DBG_IQ_ANTICT_START - 1;
    /*
        for (int i=0; i<cnt; i++)
            WriteRegs(handle, &cfg.data[iq_ANTICT_offset[i]], str+i);
    */
    for(int i = 0; i < cnt; i++)
        WriteRegs_GEV2(handle, &cfg.data[iq_ANTICT_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_RGBIR_Cfg(isp_handle_t *handle, IQ_RGBIR_CFG cfg)
{

    int str = DBG_IQ_RGBIR_START + 1;
    int cnt = DBG_IQ_RGBIR_END - DBG_IQ_RGBIR_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_RGBIR_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_OBC_Cfg(isp_handle_t *handle, IQ_OBC_CFG cfg)
{

    int str = DBG_IQ_OBC_START + 1;
    int cnt = DBG_IQ_OBC_END - DBG_IQ_OBC_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_OBC_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_GAMA_C2A_ISP_Cfg(isp_handle_t *handle, IQ_GAMA_C2A_ISP_CFG cfg)
{

    int str = DBG_IQ_GAMA_C2A_ISP_START + 1;
    int cnt = DBG_IQ_GAMA_C2A_ISP_END - DBG_IQ_GAMA_C2A_ISP_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_GAMA_C2A_ISP_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_LSC_Cfg(isp_handle_t *handle, IQ_LSC_CFG cfg)
{

    int str = DBG_IQ_LSC_START + 1;
    int cnt = DBG_IQ_LSC_END - DBG_IQ_LSC_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_LSC_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_ALSC_Cfg(isp_handle_t *handle, IQ_ALSC_CFG cfg)
{

    int str = DBG_IQ_ALSC_START + 1;
    int cnt = DBG_IQ_ALSC_END - DBG_IQ_ALSC_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_ALSC_offset[i]], str + i);

    //tmp
    handle->isp3_cfg->reg_alsc_gtbl_width_m1 = 61;
    handle->isp3_cfg->reg_alsc_gtbl_height_m1 = 69;
    handle->isp3_cfg->reg_alsc_gtbl_pitch = 61;     //tbl_pitch = tbl_width

    return SUCCESS;
}

int ISP_IQ_BDNR_Cfg(isp_handle_t *handle, IQ_BDNR_CFG cfg)
{

    int str = DBG_IQ_BDNR_START + 1;
    int cnt = DBG_IQ_BDNR_END - DBG_IQ_BDNR_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_BDNR_offset[i]], str + i);

    //
    handle->isp3_cfg->reg_dnr_fb_pitch_in_tile =
        (handle->isp0_cfg->reg_isp_crop_width + 191) / 192;

    handle->isp3_cfg->reg_dnr_ofrm_sel = 1;

    return SUCCESS;
}

int ISP_IQ_SpikeNR_Cfg(isp_handle_t *handle, IQ_SpikeNR_CFG cfg)
{

    int str = DBG_IQ_SpikeNR_START + 1;
    int cnt = DBG_IQ_SpikeNR_END - DBG_IQ_SpikeNR_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_SpikeNR_offset[i]], str + i);

    //
    if(cfg.data[IQ_SpikeNR_EN] == true)
        handle->isp0_cfg->reg_en_isp_denoise = true;

    return SUCCESS;
}

int ISP_IQ_BSNR_Cfg(isp_handle_t *handle, IQ_BSNR_CFG cfg)
{

    int str = DBG_IQ_BSNR_START + 1;
    int cnt = DBG_IQ_BSNR_END - DBG_IQ_BSNR_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_BSNR_offset[i]], str + i);

    if(cfg.data[IQ_BSNR_EN] == true)
        handle->isp0_cfg->reg_en_isp_denoise = true;

    return SUCCESS;
}

int ISP_IQ_NM_Cfg(isp_handle_t *handle, IQ_NM_CFG cfg)
{

    int str = DBG_IQ_NM_START + 1;
    int cnt = DBG_IQ_NM_END - DBG_IQ_NM_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_NM_offset[i]], str + i);

    //
    if(cfg.data[IQ_NM_EN] == true)
        handle->isp0_cfg->reg_en_isp_denoise = true;

    return SUCCESS;
}

int ISP_IQ_DM_Cfg(isp_handle_t *handle, IQ_DM_CFG cfg)
{

    int str = DBG_IQ_DM_START + 1;
    int cnt = DBG_IQ_DM_END - DBG_IQ_DM_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_DM_offset[i]], str + i);

    //
    handle->isp0_cfg->reg_cfai_en = IQ_ON; //dm_iq->bByPass; (It will cause gray images)
    handle->isp0_cfg->reg_cfai_bypass = IQ_OFF; //dm_iq->bByPass; (It will cause gray images)

    return SUCCESS;
}

int ISP_IQ_PostDN_Cfg(isp_handle_t *handle, IQ_PostDN_CFG cfg)
{

    int str = DBG_IQ_PostDN_START + 1;
    int cnt = DBG_IQ_PostDN_END - DBG_IQ_PostDN_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_PostDN_offset[i]], str + i);

    //
    if(cfg.data[IQ_PostDN_EN] == true)
        handle->isp0_cfg->reg_en_isp_denoise = true;

    return SUCCESS;
}

int ISP_IQ_FalseColor_Cfg(isp_handle_t *handle, IQ_FalseColor_CFG cfg)
{

    int str = DBG_IQ_FalseColor_START + 1;
    int cnt = DBG_IQ_FalseColor_END - DBG_IQ_FalseColor_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_FalseColor_offset[i]], str + i);

    handle->isp0_cfg->reg_rgb_falsecolor_minmax_round = 1;  //minmax rounding enable
    return SUCCESS;
}

int ISP_IQ_GAMA_A2A_ISP_Cfg(isp_handle_t *handle, IQ_GAMA_A2A_ISP_CFG cfg)
{

    int str = DBG_IQ_GAMA_A2A_ISP_START + 1;
    int cnt = DBG_IQ_GAMA_A2A_ISP_END - DBG_IQ_GAMA_A2A_ISP_START - 1;

    for(int i = 0; i < cnt; i++)
        WriteRegs(handle, &cfg.data[iq_GAMA_A2A_ISP_offset[i]], str + i);

    return SUCCESS;
}

int ISP_IQ_CsTk_Cfg(isp_handle_t *handle, IQ_CSTK_CFG cstk)
{
    return SUCCESS;
}

#endif
