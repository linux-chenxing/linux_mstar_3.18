// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef MHAL_MENULOAD_H
#define MHAL_MENULOAD_H

//Select the source to trigger menuload
#define TRIG_SRC_FE_VFDE        0
#define TRIG_SRC_RE_VSYNC       1
#define TRIG_SRC_FE_VSYNC       2
#define TRIG_SRC_DELAY_LINE     3

#define OPM_LOCK_INIT_STATE     0
#define OPM_LOCK_FE_VSYNC       1
#define OPM_LOCK_TRAIN_TRIG_P   2
#define OPM_LOCK_DS             3

#define  MS_MLOAD_CMD_ALIGN     4
#define  MS_MLOAD_REG_LEN       0x04 // how many data in one MIU request
#define  MS_MLOAD_CMD_LEN       BYTE_PER_WORD
#define  MS_MLOAD_MEM_BASE_UNIT BYTE_PER_WORD
#define  MS_MLOAD_MAX_CMD_CNT   192
#define  MS_MLOAD_NULL_CMD      0xFF010000
#define  MS_MLOAD_DUMMY_CMD_CNT(x)  ((x+0x0003)&~0x0003)
#define  MS_MLOAD_END_CMD(x)     (0x1F020000 | (MS_MLOAD_REG_LEN))
#define  MS_MLOAD_END_CMD_SC1(x) (0x9F020000 | (MS_MLOAD_REG_LEN))

#define  MS_MLG_REG_LEN         0x40 // how many data in one MIU request
#define  MS_MLG_CMD_LEN         BYTE_PER_WORD
#define  MS_MLG_MEM_BASE_UNIT   BYTE_PER_WORD

MS_U16 Hal_XC_MLoad_get_status(SCALER_WIN eWindow);
void Hal_XC_MLoad_set_on_off(MS_BOOL bEn, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_len(MS_U16 u16Len, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_depth(MS_U16 u16depth, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_miusel(MS_U8 u8MIUSel, SCALER_WIN eWindow);
void Hal_XC_MLoad_Set_req_th(MS_U8 u8val, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_base_addr(MS_U32 u32addr, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_trigger_timing(MS_U16 u16sel, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_opm_lock(MS_U16 u16sel, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_trigger_delay(MS_U16 u16delay, SCALER_WIN eWindow);
void Hal_XC_MLoad_set_trig_p(MS_U16 u16train, MS_U16 u16disp, SCALER_WIN eWindow);
void Hal_XC_MLoad_Set_riu(MS_BOOL bEn, SCALER_WIN eWindow);
MS_BOOL Hal_XC_MLoad_GetCaps(SCALER_WIN eWin);
void Hal_XC_MLoad_set_riu_cs(MS_BOOL bEn);
void Hal_XC_MLoad_set_sw_dynamic_idx_en(MS_BOOL ben);

MS_BOOL Hal_XC_MLG_GetCaps(void);
MS_U16 Hal_XC_MLG_get_status(void);
void Hal_XC_MLG_set_on_off(MS_BOOL bEn);
void Hal_XC_MLG_set_len(MS_U16 u16Len);
void Hal_XC_MLG_set_depth(MS_U16 u16depth);
void Hal_XC_MLG_set_base_addr(MS_U32 u32addr);
void Hal_XC_MLG_set_trigger_timing(MS_U16 u16sel);
void Hal_XC_MLG_set_trigger_delay(MS_U16 u16delay);
void Hal_XC_MLG_set_init_addr(MS_U16 u16addr);


#endif

