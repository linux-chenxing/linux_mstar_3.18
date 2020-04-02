#ifndef __MHAL_ZDEC_H__
#define __MHAL_ZDEC_H__

#ifndef GET_LOWORD
#define GET_LOWORD(value)    ((unsigned short)(((unsigned int)(value)) & 0xffff))
#endif
#ifndef GET_HIWORD
#define GET_HIWORD(value)    ((unsigned short)((((unsigned int)(value)) >> 16) & 0xffff))
#endif
#ifndef ALIGN16
#define ALIGN16(value)       ((((value) + 15) >> 4) << 4)
#endif
#ifndef BITS2BYTE
#define BITS2BYTE(x)         ((x) >> 3)
#endif

#define DECODING                 0UL
#define LOADING_PRESET_DICT_MIU  1UL
#define LOADING_PRESET_DICT_RIU  2UL

#define CONTIGUOUS_MODE          0UL
#define SCATTER_MODE             1UL

#define EMMC_TABLE               0UL
#define NAND_TABLE               1UL

// These functions configure a copy of ZDEC reg table in memory.
// The configuration will not take effect until 'MHal_ZDEC_Start_Operation' is called.
void MHal_ZDEC_Conf_Reset(void);
void MHal_ZDEC_Conf_Zmem(U8 miu, U32 addr, U32 size);
void MHal_ZDEC_Conf_FCIE_Handshake(U8 enable);
void MHal_ZDEC_Conf_Input_Shift(U32 skip_words_cnt, U8 shift_byte, U8 shift_bit);
void MHal_ZDEC_Conf_Preset_Dictionary(U32 size);
void MHal_ZDEC_Conf_Contiguous_Mode(U8 obuf_miu, U32 obuf_addr, U32 obuf_size);
void MHal_ZDEC_Conf_Scatter_Mode(U8 dst_tbl_miu, U32 dst_tbl_addr, U8 nand_table, U8 in_nand_page_size, U8 out_nand_page_size);


// Applies configuration stored in memory reg table and starts ZDEC operation.
// After the call, ZDEC will be ready to receive input data.
int MHal_ZDEC_Start_Operation(U8 op_mode);


void MHal_ZDEC_Feed_Data(U8 last, U8 miu, U32 sadr, U32 size);
int MHal_ZDEC_Check_Internal_Buffer(void);
int MHal_ZDEC_Check_ADMA_Table_Done(void);
int MHal_ZDEC_Check_Last_ADMA_Table_Done(void);
int MHal_ZDEC_Check_MIU_Load_Dict_Done(void);
int MHal_ZDEC_Check_Decode_Done(void);

int MHal_ZDEC_RIU_Load_Preset_Dictionary(U8* dict);


#endif

