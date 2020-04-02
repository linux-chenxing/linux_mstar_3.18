#ifndef __MSTAR_ZDEC_H__
#define __MSTAR_ZDEC_H__

#define ZDEC_OK                        ( 0L)
#define ZDEC_E_NOT_READY               (-1L)
#define ZDEC_E_INVALID_MODE            (-2L)
#define ZDEC_E_INVALID_DICTIONARY_SIZE (-3L)
#define ZDEC_E_OUT_OF_MEMORY           (-4L)


typedef enum zdec_mode_e
{
    ZMODE_DEC_CONT = 0,      // decode in contiguous mode
    ZMODE_DEC_SCATTER_EMMC,  // decode in scatter mode with eMMC table
    ZMODE_DEC_SCATTER_NAND,  // decode in scatter mode with NAND table
    ZMODE_LOAD_DICT_MIU,     // load preset dictionary in MIU mode
    ZMODE_LOAD_DICT_RIU,     // load preset dictionary in RIU mode
} zdec_mode_t;


// Initializes ZDEC HW.
// Parameters:
//     zmode:            see enum definition
//     preset_dict_size: byte size of preset dictionary
//     fcie_handshake:   ZDEC-FCIE handshake mode. 1 to turn on and 0 to turn off.
//     shift_byte:       byte shift
//     shift_bit:        bit shift
//     obuf_miu:         miu bank of output buffer
//     obuf_addr:        physical memory address of output buffer
//     obuf_size:        byte size of output buffer
//     in_page_size:     byte size per NAND page for input NAND table;  only used with ZMODE_DEC_SCATTER_NAND
//     out_page_size:    byte size per NAND page for output NAND table; only used with ZMODE_DEC_SCATTER_NAND
// Note:
//     The NAND page sizes (in_page_size, out_page_size) must be obtainable by left shifting 512.
//     i.e. Valid values include (512 << 0), (512 << 1), (512 << 2), (512 << 3), etc.
// Return:
//     0 if successful, otherwise a negative error code.
int mstar_zdec_init(
    zdec_mode_t   zmode,
    unsigned int  preset_dict_size,
    unsigned char fcie_handshake,
    unsigned int  shift_byte,
    unsigned char shift_bit,
    unsigned char obuf_miu,
    unsigned int  obuf_addr,
    unsigned int  obuf_size,
    unsigned int  in_page_size,
    unsigned int  out_page_size);

// Feeds data buffer to ZDEC HW.
// Parameters:
//     last: 1 if this is the last buffer or ADMA table for the bitstream. 0 otherwise.
//     miu:  miu bank of the data buffer
//     sadr: starting physical address of the buffer
//     size: byte size of the buffer
// Return:
//     0 if successful, otherwise a negative error code.
int mstar_zdec_feed(unsigned char last, unsigned char miu, unsigned int sadr, unsigned int size);

// Checks ZDEC buffer availability when using contiguous mode
//  to decode bitstream or load preset dictionary.
// Return:
//     0 if an available buffer exists.
int mstar_zdec_check_internal_buffer(void);

// Checks whether ADMA table is completely processed when using scatter mode.
// Return:
//     0 if ADMA table is completely processed.
int mstar_zdec_check_adma_table_done(void);

// Checks whether preset dictionary is loaded when using MIU mode.
// Return:
//     0 if preset dictionary is completely loaded.
int mstar_zdec_check_dict_miu_load_done(void);

// Checks whether decoding is completed.
// Return:
//     -1 if decoding is not complete.
//     Otherwise, byte size of decoded data.
int mstar_zdec_check_decode_done(void);

// Loads dictionary in RIU mode.
// Return:
//     0 if preset dictionary is successfully loaded.
int mstar_zdec_riu_load_dict(unsigned char* dict);

// Releases resources allocated by ZDEC api.
void mstar_zdec_release(void);


#endif

