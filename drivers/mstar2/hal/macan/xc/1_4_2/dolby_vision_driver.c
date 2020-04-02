#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mdrv_xc.h" // for using IS_OPEN_HDR macro
#include "dolby_vision_driver.h"
#include "dolby_vision_reg.h"
#include "mhal_xc.h"
#include "hwreg_xc.h"
#include <linux/time.h>
#include "fx_math.h"
#include "control_path_api.h"
#include "control_path_priv.h"
#include "KdmTypeFxp.h"
#include "CdmTypePriFxp.h"

//#define SupportHDR10
// since dolby might change their PQ bin file continuously
// and their bin file has no version control
// what we can do is simply use the bin file size to distinguish
MS_BOOL bNewPQBCMS_109727 = FALSE; // 109727 means the total size of byte of new BCMS
//extern DoVi_Config_t* _pstDmConfig;

extern ST_KDRV_XC_CFD_INIT _stCfdInit[2];
static BOOL _isOpenHdr = 0;;
//#define MENULOAD_ENABLE
//#define DS_ENABLE

static K_XC_DS_CMDCNT _stXC_DS_CmdCnt;
static MS_U8 _u8Version = 0;
#ifdef DS_ENABLE

extern void KHal_XC_StoreSWDSCommand(EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr,
                                     MS_U16 u16CmdRegValue, MS_U16 u16CmdRegMask, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);

#define DoVi_Addr(ADDR) ADDR
#define DoVi_WriteByteMask(ADDR, VAL, MASK, BIT) \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), (((VAL) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), (((MASK) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_WriteByte(ADDR, VAL) \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), ((VAL) & 0xFF) << (8 * (ADDR % 2)), 0xFF << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_Write2Bytes(ADDR, VAL) \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_Write4Bytes(ADDR, VAL) \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt); \
    KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ((ADDR) + 2), (((VAL)>>16) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_WaitByteMask(ADDR, VAL, MASK) \
    while((MHal_XC_ReadByte(0x130000 | (ADDR)) & (MASK)) != VAL) \
    {}

#define DoVi_Read2Bytes(ADDR) \
    MHal_XC_R2BYTE(0x130000 | (ADDR))
#define DoVi_ReadByte(ADDR) \
    MHal_XC_ReadByte(0x130000 | (ADDR))

#elif defined MENULOAD_ENABLE
extern MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type, MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type, MS_BOOL bImmeidate);

#define DoVi_Addr(ADDR) ADDR
#define DoVi_WriteByteMask(ADDR, VAL, MASK, BIT) \
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, (ADDR) - (ADDR % 2), (((VAL) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), (((MASK) << (BIT)) & 0xFF) << (8 * (ADDR % 2)));
#define DoVi_WriteByte(ADDR, VAL) \
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, (ADDR) - (ADDR % 2), ((VAL) & 0xFF) << (8 * (ADDR % 2)), 0xFF << (8 * (ADDR % 2)));
#define DoVi_Write2Bytes(ADDR, VAL) \
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF);
#define DoVi_Write4Bytes(ADDR, VAL) \
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF); \
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR, ((ADDR) + 2), (((VAL)>>16) & 0xFFFF), 0xFFFF);
#define DoVi_WaitByteMask(ADDR, VAL, MASK) \
    while((MHal_XC_ReadByte(0x130000 | (ADDR)) & (MASK)) != VAL) \
    {}

#define DoVi_Read2Bytes(ADDR) \
    MHal_XC_R2BYTE(0x130000 | (ADDR))
#define DoVi_ReadByte(ADDR) \
    MHal_XC_ReadByte(0x130000 | (ADDR))

#else
extern void KHal_XC_StoreSWDSCommand(EN_KDRV_WINDOW eWindow, E_DS_CLIENT client, MS_U32 u32CmdRegAddr,
                                     MS_U16 u16CmdRegValue, MS_U16 u16CmdRegMask, k_ds_reg_ip_op_sel IPOP_Sel, k_ds_reg_source_sel Source_Select, K_XC_DS_CMDCNT *pstXC_DS_CmdCnt);

#define DoVi_Addr(ADDR) (ADDR)
#define DoVi_WriteByteMask(ADDR, VAL, MASK, BIT) \
    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        {KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), (((VAL) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), (((MASK) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);}\
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))\
        {MHal_XC_WriteByteMask((0x130000 | (ADDR)), ((VAL) << (BIT)) & 0xFF, ((MASK) << (BIT)) & 0xFF);}
#define DoVi_WriteByte(ADDR, VAL) \
    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        {KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), ((VAL) & 0xFF) << (8 * (ADDR % 2)), 0xFF << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);}\
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))\
        {MHal_XC_WriteByte((0x130000 | (ADDR)), ((VAL) & 0xFF));}
#define DoVi_Write2Bytes(ADDR, VAL) \
    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        {KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);}\
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))\
        {MHal_XC_W2BYTE((0x130000 | (ADDR)), ((VAL) & 0xFFFF));}
#define DoVi_Write4Bytes(ADDR, VAL) \
    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
    { \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt); \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ((ADDR) + 2), (((VAL)>>16) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);\
    } \
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))\
    { \
        MHal_XC_W2BYTE((0x130000 | (ADDR)), ((VAL) & 0xFFFF)); \
        MHal_XC_W2BYTE(((0x130000 | (ADDR)) + 2), (((VAL)>>16) & 0xFFFF));\
    }
#define DoVi_WaitByteMask(ADDR, VAL, MASK) \
    while((MHal_XC_ReadByte(0x130000 | (ADDR)) & (MASK)) != VAL) \
    {}

#define DoVi_Read2Bytes(ADDR) \
    MHal_XC_R2BYTE(0x130000 | (ADDR))
#define DoVi_ReadByte(ADDR) \
    MHal_XC_ReadByte(0x130000 | (ADDR))
#endif
#define MsHdr_Malloc(n) \
    kmalloc((n), GFP_ATOMIC)
#define MsHdr_Free(n) \
    kfree((n))

//extern DoVi_TargetDisplayConfig_t*  _pstDoVi_TargetDisplayConfig;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MsHdr_Comp_Regtable_t* _pCompRegTable = NULL;
static h_cp_context_t _stDmContext = NULL;
static dm_metadata_t _stSrcDmMetadata = {{0}};
static MsHdr_RegTable_t* _pstDmRegTable = NULL;
static char *dm_ctx_buf = NULL;
static run_mode_t run_mode;
static DoVi_Config_t* _pstDmConfig = NULL;
static ui_menu_params_t _ui_menu_params;
//static src_param_t _src_param;
//static hdr10_param_t _hdr10_param;
static  pq_config_t* _pq_config = NULL;
static uint16_t _backlight_return_val = 0xff;
static MS_U8 _u8ViewModeId = 0;
static bool bIsInitalized = 0;
ST_KDRV_XC_DOLBY_HDMI_INPUT_TRIG _DV_HDMI_INPUT_TRIG[IPT_COUNT];

static MS_U32 g_au32DoViDegammaBypassLut[MSTAR_DEGAMMA_LUT_SIZE];
const MS_U32 g_au32DoViDegammaPqLut2[MSTAR_DEGAMMA_LUT_SIZE] =
{
    0x00000, 0x00440, 0x00e20, 0x01ce0, 0x03040, 0x048a0, 0x065e0, 0x08840, 0x0afe0, 0x0dce0, 0x10fa0, 0x14840, 0x18700, 0x1cbe0, 0x21760, 0x269c0,
    0x2c300, 0x32380, 0x38b60, 0x3fb20, 0x472e0, 0x4f300, 0x57b80, 0x60d00, 0x6a780, 0x74b80, 0x7f920, 0x45861, 0x4b961, 0x51fc1, 0x58ba1, 0x5fd01,
    0x67461, 0x6f1a1, 0x77521, 0x7fee1, 0x447a2, 0x49322, 0x4e222, 0x534a2, 0x58ae2, 0x5e4c2, 0x642a2, 0x6a462, 0x70a22, 0x77422, 0x7e282, 0x42a83,
    0x46623, 0x4a423, 0x4e463, 0x52723, 0x56c63, 0x5b443, 0x5fea3, 0x64bc3, 0x69bc3, 0x6ee83, 0x74423, 0x79cc3, 0x7f863, 0x42ba4, 0x45ca4, 0x48f44,
    0x4c384, 0x4f9a4, 0x53164, 0x56b04, 0x5a664, 0x5e3c4, 0x62304, 0x66444, 0x6a784, 0x6ece4, 0x73444, 0x77de4, 0x7c9c4, 0x40be5, 0x43425, 0x45d85,
    0x48825, 0x4b3e5, 0x4e105, 0x50f65, 0x53f25, 0x57025, 0x5a2a5, 0x5d685, 0x60bc5, 0x64285, 0x67ae5, 0x6b4a5, 0x6f005, 0x72d05, 0x76ba5, 0x7ac05,
    0x7ee05, 0x418e6, 0x43ba6, 0x45f46, 0x483e6, 0x4a986, 0x4d006, 0x4f786, 0x52006, 0x549a6, 0x57426, 0x59fe6, 0x5cca6, 0x5fa86, 0x62986, 0x659c6,
    0x68b26, 0x6bdc6, 0x6f1a6, 0x726a6, 0x75d06, 0x794c6, 0x7cdc6, 0x40407, 0x42207, 0x440a7, 0x45fe7, 0x48007, 0x4a0c7, 0x4c267, 0x4e4c7, 0x50807,
    0x52c07, 0x550c7, 0x57687, 0x59d07, 0x5c467, 0x5ecc7, 0x61607, 0x64027, 0x66b47, 0x69767, 0x6c467, 0x6f287, 0x721a7, 0x751c7, 0x78307, 0x7b567,
    0x7e8c7, 0x40ea8, 0x42988, 0x44508, 0x46108, 0x47da8, 0x49ae8, 0x4b8e8, 0x4d768, 0x4f6a8, 0x51688, 0x53708, 0x55848, 0x57a48, 0x59ce8, 0x5c068,
    0x5e488, 0x60968, 0x62f28, 0x655a8, 0x67ce8, 0x6a508, 0x6ce08, 0x6f7e8, 0x72288, 0x74e28, 0x77a88, 0x7a7e8, 0x7d648, 0x402c9, 0x41ae9, 0x43389,
    0x44ca9, 0x46649, 0x48069, 0x49b09, 0x4b649, 0x4d209, 0x4ee69, 0x50b49, 0x528c9, 0x546c9, 0x56589, 0x584c9, 0x5a4a9, 0x5c549, 0x5e669, 0x60849,
    0x62ac9, 0x64e09, 0x671e9, 0x69689, 0x6bbe9, 0x6e1e9, 0x708c9, 0x73049, 0x758a9, 0x781c9, 0x7abc9, 0x7d689, 0x4010a, 0x4174a, 0x42dea, 0x4450a,
    0x45c8a, 0x4748a, 0x48cea, 0x4a5ea, 0x4bf4a, 0x4d92a, 0x4f38a, 0x50e6a, 0x529ea, 0x545ca, 0x5624a, 0x57f4a, 0x59cca, 0x5baea, 0x5d9aa, 0x5f8ea,
    0x618ca, 0x6394a, 0x65a6a, 0x67c0a, 0x69e6a, 0x6c16a, 0x6e50a, 0x7096a, 0x72e6a, 0x7542a, 0x77a8a, 0x7a1aa, 0x7c98a, 0x7f22a, 0x40dcb, 0x422cb,
    0x4384b, 0x44e2b, 0x4646b, 0x47b0b, 0x4922b, 0x4a9ab, 0x4c1ab, 0x4da2b, 0x4f2eb, 0x50c4b, 0x5260b, 0x5406b, 0x55b2b, 0x5766b, 0x5922b, 0x5ae6b,
    0x5cb2b, 0x5e86b, 0x6064b, 0x624ab, 0x643ab, 0x6632b, 0x6834b, 0x6a3eb, 0x6c54b, 0x6e72b, 0x709ab, 0x72ccb, 0x7508b, 0x7750b, 0x79a0b, 0x7bfeb,
    0x7e64b, 0x406ac, 0x41aac, 0x42eec, 0x443ac, 0x458ac, 0x46e0c, 0x483ec, 0x49a0c, 0x4b0ac, 0x4c7ac, 0x4df2c, 0x4f70c, 0x50f4c, 0x5280c, 0x5412c,
    0x55acc, 0x574ec, 0x58f8c, 0x5aa8c, 0x5c60c, 0x5e20c, 0x5fe8c, 0x61bac, 0x6392c, 0x6574c, 0x675ec, 0x6952c, 0x6b4ec, 0x6d52c, 0x6f60c, 0x7178c,
    0x739ac, 0x75c6c, 0x77fac, 0x7a3ac, 0x7c84c, 0x7ed8c, 0x409ad, 0x41d0d, 0x430ad, 0x444ad, 0x4590d, 0x46dcd, 0x482cd, 0x4984d, 0x4ae2d, 0x4c46d,
    0x4db0d, 0x4f20d, 0x5098d, 0x5216d, 0x539cd, 0x5526d, 0x56bad, 0x5854d, 0x59f6d, 0x5b9ed, 0x5d4ed, 0x5f06d, 0x60c8d, 0x6290d, 0x6460d, 0x6638d,
    0x6818d, 0x6a02d, 0x6bf4d, 0x6df0d, 0x6ff4d, 0x7202d, 0x7418d, 0x7638d, 0x7862d, 0x7a96d, 0x7cd4d, 0x7f1cd, 0x40b8e, 0x41e6e, 0x431ae, 0x4454e,
    0x4592e, 0x46d8e, 0x4822e, 0x4972e, 0x4acae, 0x4c26e, 0x4d8ae, 0x4ef2e, 0x5062e, 0x51d8e, 0x5356e, 0x54dae, 0x5664e, 0x57f6e, 0x5990e, 0x5b30e,
    0x5cd8e, 0x5e86e, 0x603ee, 0x61fce, 0x63c4e, 0x6592e, 0x676ae, 0x694ae, 0x6b32e, 0x6d24e, 0x6f1ee, 0x7122e, 0x732ee, 0x7544e, 0x7764e, 0x798ce,
    0x7bc0e, 0x7dfce, 0x4022f, 0x414af, 0x427af, 0x43acf, 0x44e6f, 0x4626f, 0x476af, 0x48b4f, 0x4a06f, 0x4b5cf, 0x4cb8f, 0x4e1cf, 0x4f86f, 0x50f6f,
    0x526cf, 0x53eaf, 0x556ef, 0x56faf, 0x588cf, 0x5a26f, 0x5bc8f, 0x5d70f, 0x5f20f, 0x60d8f, 0x629af, 0x6462f, 0x6632f, 0x680cf, 0x69eef, 0x6bd8f,
    0x6dccf, 0x6fc8f, 0x71cef, 0x73def, 0x75f6f, 0x7818f, 0x7a46f, 0x7c7cf, 0x7ebef, 0x40850, 0x41b10, 0x42e10, 0x44170, 0x45530, 0x46950, 0x47dd0,
    0x492b0, 0x4a7f0, 0x4bd90, 0x4d390, 0x4e9f0, 0x500d0, 0x51810, 0x52fd0, 0x547f0, 0x56090, 0x57990, 0x59310, 0x5acf0, 0x5c770, 0x5e250, 0x5fdd0,
    0x619b0, 0x63630, 0x65330, 0x670b0, 0x68ed0, 0x6ad70, 0x6ccb0, 0x6ec70, 0x70cd0, 0x72dd0, 0x74f50, 0x77190, 0x79470, 0x7b7f0, 0x7dc30, 0x40091,
    0x41351, 0x42671, 0x439f1, 0x44dd1, 0x461f1, 0x47691, 0x48b91, 0x4a0f1, 0x4b6d1, 0x4ccf1, 0x4e3b1, 0x4fab1, 0x51231, 0x52a11, 0x54291, 0x55b51,
    0x574b1, 0x58e91, 0x5a8d1, 0x5c391, 0x5def1, 0x5fad1, 0x61731, 0x63411, 0x65191, 0x66f91, 0x68e31, 0x6ad51, 0x6cd11, 0x6ed91, 0x70e91, 0x73031,
    0x75291, 0x77571, 0x79931, 0x7bd71, 0x7e291, 0x40432, 0x41772, 0x42b12, 0x43f12, 0x45372, 0x46832, 0x47d72, 0x492f2, 0x4a912, 0x4bf72, 0x4d672
};

static const MS_U16 g_au16DoViGammaPqLut[MSTAR_GAMMA_LUT_SIZE] =
{
#if 1 // maxinput = 0x9c40_0000
    0x0000, 0x0010, 0x0018, 0x001F, 0x0024, 0x0029, 0x002E, 0x0032, 0x0036, 0x003A, 0x003E, 0x0041, 0x0045, 0x0048, 0x004B, 0x004F,
    0x0052, 0x0055, 0x0058, 0x005A, 0x005D, 0x0060, 0x0063, 0x0065, 0x0068, 0x006A, 0x006D, 0x006F, 0x0072, 0x0074, 0x0076, 0x0079,
    0x007B, 0x007F, 0x0084, 0x0088, 0x008C, 0x0090, 0x0094, 0x0098, 0x009C, 0x00A0, 0x00A3, 0x00A7, 0x00AB, 0x00AE, 0x00B2, 0x00B5,
    0x00B8, 0x00BF, 0x00C5, 0x00CC, 0x00D2, 0x00D8, 0x00DE, 0x00E3, 0x00E9, 0x00EF, 0x00F4, 0x00FA, 0x00FF, 0x0104, 0x0109, 0x010E,
    0x0113, 0x011D, 0x0126, 0x012F, 0x0138, 0x0141, 0x014A, 0x0152, 0x015B, 0x0163, 0x016B, 0x0172, 0x017A, 0x0182, 0x0189, 0x0190,
    0x0198, 0x01A6, 0x01B3, 0x01C1, 0x01CE, 0x01DA, 0x01E7, 0x01F3, 0x01FF, 0x020A, 0x0216, 0x0221, 0x022C, 0x0237, 0x0241, 0x024C,
    0x0256, 0x026A, 0x027E, 0x0291, 0x02A3, 0x02B5, 0x02C7, 0x02D8, 0x02E9, 0x02F9, 0x0309, 0x0319, 0x0329, 0x0338, 0x0347, 0x0356,
    0x0364, 0x0380, 0x039C, 0x03B6, 0x03D0, 0x03E9, 0x0402, 0x0419, 0x0431, 0x0448, 0x045E, 0x0474, 0x0489, 0x049E, 0x04B3, 0x04C7,
    0x04DB, 0x0502, 0x0528, 0x054C, 0x056F, 0x0592, 0x05B3, 0x05D4, 0x05F4, 0x0613, 0x0631, 0x064F, 0x066C, 0x0689, 0x06A5, 0x06C0,
    0x06DB, 0x0710, 0x0742, 0x0773, 0x07A3, 0x07D1, 0x07FE, 0x082A, 0x0854, 0x087E, 0x08A6, 0x08CE, 0x08F5, 0x091A, 0x0940, 0x0964,
    0x0988, 0x09CD, 0x0A10, 0x0A51, 0x0A8F, 0x0ACC, 0x0B07, 0x0B40, 0x0B78, 0x0BAE, 0x0BE3, 0x0C17, 0x0C49, 0x0C7A, 0x0CAB, 0x0CDA,
    0x0D08, 0x0D62, 0x0DB8, 0x0E0C, 0x0E5D, 0x0EAB, 0x0EF6, 0x0F40, 0x0F87, 0x0FCC, 0x1010, 0x1052, 0x1092, 0x10D1, 0x110F, 0x114B,
    0x1185, 0x11F7, 0x1265, 0x12CE, 0x1334, 0x1396, 0x13F5, 0x1451, 0x14AB, 0x1502, 0x1556, 0x15A9, 0x15F9, 0x1647, 0x1693, 0x16DE,
    0x1727, 0x17B4, 0x183C, 0x18BE, 0x193C, 0x19B5, 0x1A2A, 0x1A9B, 0x1B09, 0x1B73, 0x1BDB, 0x1C40, 0x1CA2, 0x1D01, 0x1D5E, 0x1DB9,
    0x1E12, 0x1EBD, 0x1F62, 0x1FFF, 0x2097, 0x2129, 0x21B6, 0x223E, 0x22C2, 0x2342, 0x23BE, 0x2436, 0x24AC, 0x251E, 0x258D, 0x25F9,
    0x2663, 0x272F, 0x27F2, 0x28AD, 0x2960, 0x2A0D, 0x2AB3, 0x2B53, 0x2BEF, 0x2C85, 0x2D17, 0x2DA4, 0x2E2D, 0x2EB3, 0x2F34, 0x2FB3,
    0x302E, 0x311C, 0x31FF, 0x32D8, 0x33A8, 0x3470, 0x3530, 0x35E9, 0x369C, 0x3749, 0x37F1, 0x3893, 0x3931, 0x39CA, 0x3A5F, 0x3AF0,
    0x3B7D, 0x3C8C, 0x3D8F, 0x3E86, 0x3F73, 0x4056, 0x4130, 0x4202, 0x42CC, 0x4390, 0x444D, 0x4504, 0x45B6, 0x4662, 0x470A, 0x47AC,
    0x484B, 0x497B, 0x4A9D, 0x4BB1, 0x4CB9, 0x4DB6, 0x4EA9, 0x4F92, 0x5073, 0x514C, 0x521E, 0x52E9, 0x53AD, 0x546C, 0x5524, 0x55D8,
    0x5686, 0x57D6, 0x5914, 0x5A43, 0x5B65, 0x5C7A, 0x5D84, 0x5E83, 0x5F78, 0x6065, 0x6149, 0x6226, 0x62FC, 0x63CB, 0x6493, 0x6556,
    0x6613, 0x677E, 0x68D6, 0x6A1E, 0x6B56, 0x6C80, 0x6D9E, 0x6EB0, 0x6FB8, 0x70B6, 0x71AB, 0x7297, 0x737B, 0x7459, 0x752F, 0x75FF,
    0x76C8, 0x784B, 0x79B9, 0x7B15, 0x7C61, 0x7D9D, 0x7ECC, 0x7FEE, 0x8105, 0x8211, 0x8313, 0x840C, 0x84FD, 0x85E6, 0x86C7, 0x87A2,
    0x8876, 0x8A0C, 0x8B8C, 0x8CF8, 0x8E53, 0x8F9D, 0x90D9, 0x9208, 0x932A, 0x9441, 0x954E, 0x9651, 0x974B, 0x983D, 0x9927, 0x9A09,
    0x9AE5, 0x9C89, 0x9E16, 0x9F8E, 0xA0F4, 0xA248, 0xA38E, 0xA4C5, 0xA5F0, 0xA70F, 0xA823, 0xA92C, 0xAA2D, 0xAB24, 0xAC14, 0xACFB,
    0xADDC, 0xAF89, 0xB11E, 0xB29E, 0xB40A, 0xB565, 0xB6B0, 0xB7EC, 0xB91B, 0xBA3E, 0xBB56, 0xBC63, 0xBD67, 0xBE61, 0xBF53, 0xC03E,
    0xC120, 0xC1FC, 0xC2D2, 0xC3A1, 0xC46A, 0xC52E, 0xC5ED, 0xC6A7, 0xC75C, 0xC80C, 0xC8B8, 0xC961, 0xCA05, 0xCAA6, 0xCB43, 0xCBDD,
    0xCC73, 0xCD07, 0xCD97, 0xCE25, 0xCEAF, 0xCF38, 0xCFBD, 0xD040, 0xD0C1, 0xD13F, 0xD1BC, 0xD236, 0xD2AE, 0xD324, 0xD398, 0xD40A,
    0xD47B, 0xD556, 0xD62B, 0xD6FA, 0xD7C3, 0xD886, 0xD944, 0xD9FD, 0xDAB1, 0xDB61, 0xDC0C, 0xDCB4, 0xDD57, 0xDDF7, 0xDE93, 0xDF2C,
    0xDFC1, 0xE054, 0xE0E3, 0xE16F, 0xE1F9, 0xE280, 0xE304, 0xE386, 0xE406, 0xE483, 0xE4FE, 0xE577, 0xE5EE, 0xE663, 0xE6D5, 0xE746,
    0xE7B5, 0xE88F, 0xE961, 0xEA2D, 0xEAF3, 0xEBB4, 0xEC70, 0xED26, 0xEDD8, 0xEE85, 0xEF2E, 0xEFD3, 0xF074, 0xF111, 0xF1AA, 0xF241,
    0xF2D4, 0xF363, 0xF3F0, 0xF47A, 0xF501, 0xF586, 0xF608, 0xF688, 0xF705, 0xF780, 0xF7F8, 0xF86F, 0xF8E3, 0xF956, 0xF9C6, 0xFA35,
    0xFAA2, 0xFC44, 0xFDCE, 0xFF42, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
#else // maxinput = 0xffff_0000
    0x0000, 0x000C, 0x0012, 0x0017, 0x001B, 0x001F, 0x0022, 0x0026, 0x0029, 0x002C, 0x002E, 0x0031, 0x0034, 0x0036, 0x0038, 0x003B,
    0x003D, 0x003F, 0x0041, 0x0044, 0x0046, 0x0048, 0x004A, 0x004C, 0x004E, 0x004F, 0x0051, 0x0053, 0x0055, 0x0057, 0x0058, 0x005A,
    0x005C, 0x005F, 0x0062, 0x0066, 0x0069, 0x006C, 0x006F, 0x0072, 0x0075, 0x0077, 0x007A, 0x007D, 0x0080, 0x0082, 0x0085, 0x0088,
    0x008A, 0x008F, 0x0094, 0x0099, 0x009D, 0x00A2, 0x00A6, 0x00AB, 0x00AF, 0x00B3, 0x00B7, 0x00BB, 0x00C0, 0x00C3, 0x00C7, 0x00CB,
    0x00CF, 0x00D6, 0x00DE, 0x00E5, 0x00EB, 0x00F2, 0x00F9, 0x00FF, 0x0106, 0x010C, 0x0112, 0x0118, 0x011E, 0x0123, 0x0129, 0x012F,
    0x0134, 0x013F, 0x014A, 0x0154, 0x015E, 0x0168, 0x0171, 0x017B, 0x0184, 0x018D, 0x0196, 0x019E, 0x01A7, 0x01AF, 0x01B8, 0x01C0,
    0x01C8, 0x01D7, 0x01E6, 0x01F5, 0x0204, 0x0212, 0x021F, 0x022D, 0x023A, 0x0247, 0x0254, 0x0260, 0x026C, 0x0278, 0x0284, 0x028F,
    0x029B, 0x02B1, 0x02C6, 0x02DB, 0x02F0, 0x0304, 0x0317, 0x032A, 0x033C, 0x034F, 0x0360, 0x0372, 0x0383, 0x0393, 0x03A4, 0x03B4,
    0x03C4, 0x03E3, 0x0401, 0x041E, 0x043A, 0x0456, 0x0471, 0x048B, 0x04A5, 0x04BE, 0x04D6, 0x04EE, 0x0505, 0x051D, 0x0533, 0x0549,
    0x055F, 0x0589, 0x05B2, 0x05DA, 0x0601, 0x0626, 0x064B, 0x066E, 0x0691, 0x06B3, 0x06D4, 0x06F5, 0x0714, 0x0733, 0x0752, 0x076F,
    0x078D, 0x07C6, 0x07FD, 0x0832, 0x0865, 0x0898, 0x08C8, 0x08F8, 0x0926, 0x0953, 0x097E, 0x09A9, 0x09D3, 0x09FC, 0x0A24, 0x0A4C,
    0x0A72, 0x0ABD, 0x0B05, 0x0B4B, 0x0B8E, 0x0BD0, 0x0C0F, 0x0C4D, 0x0C89, 0x0CC3, 0x0CFC, 0x0D34, 0x0D6A, 0x0D9F, 0x0DD3, 0x0E05,
    0x0E37, 0x0E97, 0x0EF4, 0x0F4E, 0x0FA4, 0x0FF8, 0x1049, 0x1097, 0x10E4, 0x112E, 0x1176, 0x11BC, 0x1201, 0x1244, 0x1286, 0x12C6,
    0x1304, 0x137E, 0x13F2, 0x1463, 0x14CF, 0x1538, 0x159D, 0x15FF, 0x165E, 0x16BA, 0x1714, 0x176C, 0x17C1, 0x1814, 0x1865, 0x18B4,
    0x1901, 0x1997, 0x1A27, 0x1AB1, 0x1B35, 0x1BB5, 0x1C31, 0x1CA9, 0x1D1D, 0x1D8D, 0x1DFB, 0x1E65, 0x1ECC, 0x1F31, 0x1F93, 0x1FF3,
    0x2050, 0x2105, 0x21B2, 0x2258, 0x22F8, 0x2391, 0x2425, 0x24B4, 0x253F, 0x25C5, 0x2647, 0x26C6, 0x2741, 0x27B8, 0x282D, 0x289E,
    0x290D, 0x29E2, 0x2AAE, 0x2B72, 0x2C2E, 0x2CE2, 0x2D90, 0x2E37, 0x2ED9, 0x2F76, 0x300E, 0x30A2, 0x3131, 0x31BC, 0x3243, 0x32C7,
    0x3347, 0x343F, 0x352B, 0x360C, 0x36E5, 0x37B4, 0x387C, 0x393D, 0x39F7, 0x3AAA, 0x3B58, 0x3C01, 0x3CA4, 0x3D43, 0x3DDD, 0x3E73,
    0x3F05, 0x401E, 0x412A, 0x422A, 0x431E, 0x4409, 0x44EA, 0x45C3, 0x4694, 0x475E, 0x4822, 0x48DF, 0x4996, 0x4A47, 0x4AF4, 0x4B9B,
    0x4C3F, 0x4D78, 0x4EA2, 0x4FBE, 0x50CE, 0x51D2, 0x52CC, 0x53BC, 0x54A3, 0x5582, 0x5659, 0x5729, 0x57F3, 0x58B6, 0x5974, 0x5A2C,
    0x5ADF, 0x5C36, 0x5D7D, 0x5EB3, 0x5FDB, 0x60F7, 0x6207, 0x630C, 0x6407, 0x64F9, 0x65E2, 0x66C4, 0x679E, 0x6871, 0x693E, 0x6A04,
    0x6AC5, 0x6C38, 0x6D96, 0x6EE4, 0x7022, 0x7152, 0x7275, 0x738D, 0x7499, 0x759B, 0x7694, 0x7785, 0x786D, 0x794E, 0x7A27, 0x7AFA,
    0x7BC7, 0x7D50, 0x7EC4, 0x8025, 0x8175, 0x82B6, 0x83E9, 0x850F, 0x862A, 0x8739, 0x883F, 0x893B, 0x8A2F, 0x8B1B, 0x8BFF, 0x8CDC,
    0x8DB2, 0x8F4D, 0x90D1, 0x9241, 0x939F, 0x94ED, 0x962C, 0x975E, 0x9883, 0x999D, 0x9AAC, 0x9BB1, 0x9CAD, 0x9DA1, 0x9E8D, 0x9F71,
    0xA04E, 0xA1F6, 0xA385, 0xA500, 0xA668, 0xA7BF, 0xA907, 0xAA40, 0xAB6C, 0xAC8C, 0xADA2, 0xAEAD, 0xAFAE, 0xB0A7, 0xB197, 0xB280,
    0xB362, 0xB510, 0xB6A7, 0xB828, 0xB995, 0xBAF1, 0xBC3D, 0xBD7A, 0xBEAA, 0xBFCE, 0xC0E6, 0xC1F4, 0xC2F7, 0xC3F2, 0xC4E5, 0xC5CF,
    0xC6B2, 0xC78E, 0xC864, 0xC933, 0xC9FC, 0xCAC0, 0xCB7F, 0xCC39, 0xCCEE, 0xCD9E, 0xCE4A, 0xCEF3, 0xCF97, 0xD038, 0xD0D5, 0xD16E,
    0xD205, 0xD298, 0xD328, 0xD3B5, 0xD440, 0xD4C8, 0xD54D, 0xD5D0, 0xD651, 0xD6CF, 0xD74B, 0xD7C5, 0xD83D, 0xD8B2, 0xD926, 0xD998,
    0xDA08, 0xDAE4, 0xDBB8, 0xDC86, 0xDD4E, 0xDE11, 0xDECF, 0xDF87, 0xE03B, 0xE0EA, 0xE195, 0xE23C, 0xE2DE, 0xE37E, 0xE419, 0xE4B1,
    0xE546, 0xE5D8, 0xE667, 0xE6F3, 0xE77C, 0xE802, 0xE886, 0xE907, 0xE986, 0xEA03, 0xEA7D, 0xEAF6, 0xEB6C, 0xEBE0, 0xEC52, 0xECC3,
    0xED31, 0xEE09, 0xEEDB, 0xEFA6, 0xF06B, 0xF12B, 0xF1E5, 0xF29A, 0xF34B, 0xF3F7, 0xF49F, 0xF543, 0xF5E3, 0xF67F, 0xF718, 0xF7AD,
    0xF83F, 0xF8CE, 0xF95A, 0xF9E3, 0xFA69, 0xFAED, 0xFB6E, 0xFBED, 0xFC69, 0xFCE3, 0xFD5B, 0xFDD0, 0xFE44, 0xFEB5, 0xFF25, 0xFF93,
#endif
};

#ifdef DOLBY140
void process_sram0(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = (r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 3
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S0_SIZE*3; count += 3) // [729, 736)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram1(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S1_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram2(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S2_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram3(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S3_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram4(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 3
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S4_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram5(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S5_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram6(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S6_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram7(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                //i = 3*(r + 17*g + 289*b);
                i = 1*(r + 17*g + 289*b);
                //mstar_3dlut[count+2] = dolby_3dlut[i+2];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1];
                mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ];
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S7_SIZE*3; count += 3) // [512, 512)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}
#else

#if 1
void process_sram0(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = (r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 3
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S0_SIZE*3; count += 3) // [729, 736)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram1(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S1_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram2(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S2_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram3(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S3_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram4(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 3
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S4_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram5(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S5_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram6(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S6_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram7(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S7_SIZE*3; count += 3) // [512, 512)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

#else
void process_sram0(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = (r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 3
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S0_SIZE*3; count += 3) // [729, 736)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram1(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S1_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram2(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S2_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram3(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 0; r < DOLBY_3DLUT_DIM0  ; r += 2) // 9
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S3_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram4(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 3
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S4_SIZE*3; count += 3) // [648, 656)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram5(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 0; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 2
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = DOLBY_3DLUT_DIM1-1; g < DOLBY_3DLUT_DIM1; g += 2) // 1
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S5_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram6(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 0; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // type 1
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = DOLBY_3DLUT_DIM2-1; b < DOLBY_3DLUT_DIM2; b += 2) // 1
            {
                i = 3*(r + 17*g + 289*b);
                //i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                //mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S6_SIZE*3; count += 3) // [576, 576)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}

void process_sram7(MS_U16* mstar_3dlut, const MS_U16* dolby_3dlut)
{
    MS_S32 r, g, b, i, count = 0;
    // type 0
    for (r = 1; r < DOLBY_3DLUT_DIM0  ; r += 2) // 8
        for (g = 1; g < DOLBY_3DLUT_DIM1-1; g += 2) // 8
            for (b = 1; b < DOLBY_3DLUT_DIM2-1; b += 2) // 8
            {
                i = 3*(r + 17*g + 289*b);
                //  i = 1*(r + 17*g + 289*b);
                mstar_3dlut[count+2] = dolby_3dlut[i+2]<<4;
                mstar_3dlut[count+1] = dolby_3dlut[i+1]<<4;
                //  mstar_3dlut[count+2] = dolby_3dlut[i+2*DOLBY_3DLUT_SIZE];
                // mstar_3dlut[count+1] = dolby_3dlut[i+1*DOLBY_3DLUT_SIZE];
                mstar_3dlut[count  ] = dolby_3dlut[i  ]<<4;
                count += 3;
            }
    // alignment
    for ( ; count < DOVI_3DLUT_S7_SIZE*3; count += 3) // [512, 512)
    {
        mstar_3dlut[count+2] = 0;
        mstar_3dlut[count+1] = 0;
        mstar_3dlut[count  ] = 0;
    }
}
#endif
#endif
int DoVi_3DLUT_parsing_reorder_noHeader( MS_U16* pu16MstarTable, const MS_U8* pu8DolbyTable)
{
    MS_U32 count = 0;
    //process_sram0(&mstar_3dlut[count], &dolby_3dlut[0]); count += MSTAR_SRAM0_SIZE*3;
    process_sram0(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S0_SIZE*3;
    process_sram1(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S1_SIZE*3;
    process_sram2(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S2_SIZE*3;
    process_sram3(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S3_SIZE*3;
    process_sram4(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S4_SIZE*3;
    process_sram5(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S5_SIZE*3;
    process_sram6(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S6_SIZE*3;
    process_sram7(&(pu16MstarTable[count]), pu8DolbyTable);
    count += DOVI_3DLUT_S7_SIZE*3;

    return 0;
}

// Composer
void DoVi_CompDumpConfig(void)
{
#if 0
    int i, j, cmp;
    DoVi_Printf("//%x //rpu_VDR_bit_depth\n", pConfExt->rpu_VDR_bit_depth);
    DoVi_Printf("//%x //rpu_BL_bit_depth \n", pConfExt->rpu_BL_bit_depth );
    DoVi_Printf("//%x //rpu_EL_bit_depth \n", pConfExt->rpu_EL_bit_depth );
    DoVi_Printf("//%x //coefficient_log2_denom\n", pConfExt->coefficient_log2_denom);
    for(cmp = 0; cmp <3; cmp++)
    {
        DoVi_Printf("//%x //num_pivots[%d]\n", pConfExt->num_pivots[cmp], cmp);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 9; i++)
            DoVi_Printf("//%x //pivot_value[%d][%d]\n", pConfExt->pivot_value[cmp][i], cmp, i);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        DoVi_Printf("//%x //mapping_idc[%d]\n", pConfExt->mapping_idc[cmp], cmp);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 8; i++)
        {
            DoVi_Printf("//%x //poly_order[%d][%d]\n", pConfExt->poly_order[cmp][i], cmp, i);
        }
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 8; i++)
        {
            for (j = 0; j < 3; j++)   // s6.23
            {
                DoVi_Printf("//%llx //poly_coef[%d][%d][%d]\n", (((MS_U32)pConfExt->poly_coef_int[cmp][i][j] << pConfExt->coefficient_log2_denom) | pConfExt->poly_coef[cmp][i][j]) & 0x3FFFFFFF, cmp, i, j);
            }
        }
    }
    for(cmp = 0; cmp <2; cmp++)
    {
        DoVi_Printf("//%x //MMR_order[%d]\n", pConfExt->MMR_order[cmp], cmp);
    }
    for(cmp = 0; cmp <2; cmp++)
    {
        for (i=0; i < 22; i++)   // s17.23
        {
            DoVi_Printf("//%llx //MMR_coef[%d][%d]\n", (((MS_U64)pConfExt->MMR_coef_int[cmp][i] << pConfExt->coefficient_log2_denom) | pConfExt->MMR_coef[cmp][i]) & 0xFFFFFFFFFFull, cmp, i);
        }
    }
    DoVi_Printf("//%x //NLQ_method_idc\n",   pConfExt->NLQ_method_idc);
    DoVi_Printf("//%x //disable_residual_flag\n",   pConfExt->disable_residual_flag);
    DoVi_Printf("//%x //el_spatial_resampling_filter_flag\n",   pConfExt->el_spatial_resampling_filter_flag   );
    DoVi_Printf("//%x //reserved_8bit\n",   pConfExt->reserved_8bit      );
    for(cmp = 0; cmp <3; cmp++)
    {
        DoVi_Printf("//%x //NLQ_offset[%d]\n",   pConfExt->NLQ_offset[cmp], cmp);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i < 3; i++)   // s1.23
        {
            DoVi_Printf("//%x //NLQ_coeff[%d][%d]\n", ((MS_U32)pConfExt->NLQ_coeff_int[cmp][i] << pConfExt->coefficient_log2_denom) | pConfExt->NLQ_coeff[cmp][i], cmp, i);
        }
    }
    DoVi_Printf("//%x //spatial_resampling_filter_flag         \n", pConfExt->spatial_resampling_filter_flag);
    DoVi_Printf("//%x //spatial_resampling_explicit_filter_flag\n", pConfExt->spatial_resampling_explicit_filter_flag);
    DoVi_Printf("//%x //spatial_filter_exp_coef_log2_denom     \n", pConfExt->spatial_filter_exp_coef_log2_denom     );
    DoVi_Printf("//%x //spatial_resampling_mode_hor_idc        \n", pConfExt->spatial_resampling_mode_hor_idc        );
    DoVi_Printf("//%x //spatial_resampling_mode_ver_idc        \n", pConfExt->spatial_resampling_mode_ver_idc        );
    DoVi_Printf("//%x //spatial_resampling_filter_hor_idc[0]   \n", pConfExt->spatial_resampling_filter_hor_idc[0]   );
    DoVi_Printf("//%x //spatial_resampling_filter_hor_idc[1]   \n", pConfExt->spatial_resampling_filter_hor_idc[1]   );
    DoVi_Printf("//%x //spatial_resampling_filter_hor_idc[2]   \n", pConfExt->spatial_resampling_filter_hor_idc[2]   );
    DoVi_Printf("//%x //spatial_resampling_filter_ver_idc[0]   \n", pConfExt->spatial_resampling_filter_ver_idc[0]   );
    DoVi_Printf("//%x //spatial_resampling_filter_ver_idc[1]   \n", pConfExt->spatial_resampling_filter_ver_idc[1]   );
    DoVi_Printf("//%x //spatial_resampling_filter_ver_idc[2]   \n", pConfExt->spatial_resampling_filter_ver_idc[2]   );
    DoVi_Printf("//%x //spatial_resampling_luma_pivot[0]       \n", pConfExt->spatial_resampling_luma_pivot[0]       );
    DoVi_Printf("//%x //spatial_resampling_luma_pivot[1]       \n", pConfExt->spatial_resampling_luma_pivot[1]       );
    for(i = 0; i <3; i++)
    {
        for (j=0; j < 8; j++)   // s6.20 -> not used due to cost-down
        {
            DoVi_Printf("//%llx //spatial_filter_coeff_hor[%d][%d]\n", (((MS_U32)pConfExt->spatial_filter_coeff_hor_int[i][j] << pConfExt->spatial_filter_exp_coef_log2_denom) | pConfExt->spatial_filter_coeff_hor[i][j]) & 0x7FFFFFF, i, j);
        }
    }
    for(i = 0; i <3; i++)
    {
        for (j=0; j < 6; j++)   // s6.20 -> not used due to cost-down
        {
            DoVi_Printf("//%llx //spatial_filter_coeff_ver[0][%d][%d]\n", (((MS_U32)pConfExt->spatial_filter_coeff_ver_int[0][i][j] << pConfExt->spatial_filter_exp_coef_log2_denom) | pConfExt->spatial_filter_coeff_ver[0][i][j]) & 0x7FFFFFF, i, j);
            DoVi_Printf("//%llx //spatial_filter_coeff_ver[1][%d][%d]\n", (((MS_U32)pConfExt->spatial_filter_coeff_ver_int[1][i][j] << pConfExt->spatial_filter_exp_coef_log2_denom) | pConfExt->spatial_filter_coeff_ver[1][i][j]) & 0x7FFFFFF, i, j);
        }
    }

#endif
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_CompAllocRegTable(void)
{
    if (_pCompRegTable == NULL)
    {
        _pCompRegTable = (MsHdr_Comp_Regtable_t*)MsHdr_Malloc(sizeof(MsHdr_Comp_Regtable_t));
        if (_pCompRegTable != NULL)
        {
            memset(_pCompRegTable, 0, sizeof(MsHdr_Comp_Regtable_t));
        }
        else
        {
            printk("%s: %d: malloc fail\n", __func__, __LINE__);
        }
    }
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_CompFreeRegTable(void)
{
    if (_pCompRegTable != NULL)
    {
        MsHdr_Free(_pCompRegTable);
    }
}

//-------------------------------------------------------------------------------------------------
/// Dolby Vision default setting and initialization.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompSetDefaultConfig(void)
{
    memset(&_pCompRegTable->stConfig, 0, sizeof(rpu_ext_config_fixpt_main_t));
}
//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_CompCheckMetadata(rpu_ext_config_fixpt_main_t* pConfExt)
{
    int rv = 0, frame = 0;
    int i, j, cmp;

#define CHK_RANGE_INT(espr, low, high) \
    do { \
        if ((espr < low) || (espr > high)) { \
            rv = -1; \
            DoVi_Printf("Frame #%d  %s is %d, allowed values are %d..%d\n", frame, #espr, espr, low, high); \
        } \
    } while(0)

#define CHK_LIST_INT_1(espr, value1) \
    do { \
        if (espr != value1) { \
            rv = -1; \
            DoVi_Printf("Frame #%d  %s is %d the only allowed value is %d\n", frame, #espr, espr, value1); \
        } \
    } while(0)

#define CHK_LIST_INT_2(espr, value1, value2) \
    do { \
        if ((espr != value1) && (espr != value2)) { \
            rv = -1; \
            DoVi_Printf("Frame #%d  %s is %d, allowed values are %d, %d\n", frame, #espr, espr, value1, value2); \
        } \
    } while(0)

#define CHK_LIST_INT_3(espr, value1, value2, value3) \
    do { \
        if ((espr != value1) && (espr != value2) && (espr != value3)) { \
            rv = -1; \
            DoVi_Printf("Frame #%d  %s is %d, allowed values are %d, %d, %d\n", frame, #espr, espr, value1, value2, value3); \
        } \
    } while(0)

    // Phase 1 : Check (code modified from Dolby IDK v1.4.1)
    const DoVi_Comp_ExtConfig_t* efrc = pConfExt;
    CHK_RANGE_INT(efrc->rpu_VDR_bit_depth, 8, 14);
    CHK_LIST_INT_2(efrc->rpu_BL_bit_depth, 8, 10);
    CHK_LIST_INT_2(efrc->rpu_EL_bit_depth, 8, 10);
    CHK_RANGE_INT(efrc->coefficient_log2_denom, 5+efrc->rpu_EL_bit_depth, 23);
    CHK_RANGE_INT(efrc->num_pivots[0], 2, 9);
    if (efrc->mapping_idc[1] == 1)
        CHK_LIST_INT_1(efrc->num_pivots[1], 2);
    else
        CHK_RANGE_INT(efrc->num_pivots[1], 2, 5);
    if (efrc->mapping_idc[2] == 1)
        CHK_LIST_INT_1(efrc->num_pivots[2], 2);
    else
        CHK_RANGE_INT(efrc->num_pivots[2], 2, 5);
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 9; i++)
            CHK_RANGE_INT(efrc->pivot_value[cmp][i], 0, ((MS_U32)1<<efrc->rpu_BL_bit_depth));
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        CHK_RANGE_INT(efrc->mapping_idc[cmp], 0, 1);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 8; i++)
        {
            CHK_RANGE_INT(efrc->poly_order[cmp][i], 0, 2);
        }
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        for (i=0; i< 8; i++)
        {
            CHK_RANGE_INT(efrc->poly_coef_int[cmp][i][0], -64, 63);
            CHK_RANGE_INT(efrc->poly_coef    [cmp][i][0],   0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
            CHK_RANGE_INT(efrc->poly_coef_int[cmp][i][1], -64, 63);
            CHK_RANGE_INT(efrc->poly_coef    [cmp][i][1],   0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
            CHK_RANGE_INT(efrc->poly_coef_int[cmp][i][2], -64, 63);
            CHK_RANGE_INT(efrc->poly_coef    [cmp][i][2],   0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
        }
    }
    for(cmp = 0; cmp <2; cmp++)
    {
        CHK_RANGE_INT(efrc->MMR_order[cmp], 0, 3);
    }
    for(cmp = 0; cmp <2; cmp++)
    {
        for (i=0; i < 22; i++)
        {
            CHK_RANGE_INT(efrc->MMR_coef_int[cmp][i], -65536, 65535);
            CHK_RANGE_INT(efrc->MMR_coef    [cmp][i],      0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
        }
    }
    CHK_RANGE_INT(efrc->NLQ_method_idc, 0, 0);
    CHK_RANGE_INT(efrc->disable_residual_flag, 0, 1);
    CHK_RANGE_INT(efrc->el_spatial_resampling_filter_flag, 0, 1);
    CHK_LIST_INT_1(efrc->reserved_8bit, 0);
    for(cmp = 0; cmp <3; cmp++)
    {
        CHK_RANGE_INT(efrc->NLQ_offset[cmp], 0, ((MS_U32)1<<efrc->rpu_EL_bit_depth) - 1);
    }
    for(cmp = 0; cmp <3; cmp++)
    {
        CHK_RANGE_INT(efrc->NLQ_coeff_int[cmp][0], 0, 1);
        CHK_RANGE_INT(efrc->NLQ_coeff    [cmp][0], 0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
        CHK_RANGE_INT(efrc->NLQ_coeff_int[cmp][1], 0, 1);
        CHK_RANGE_INT(efrc->NLQ_coeff    [cmp][1], 0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
        CHK_RANGE_INT(efrc->NLQ_coeff_int[cmp][2], 0, 1);
        CHK_RANGE_INT(efrc->NLQ_coeff    [cmp][2], 0,  ((MS_U32)1<<efrc->coefficient_log2_denom) - 1);
    }
    CHK_RANGE_INT(efrc->spatial_resampling_filter_flag         , 0, 1);
    CHK_RANGE_INT(efrc->spatial_resampling_explicit_filter_flag, 0, 1);
    if(efrc->spatial_filter_exp_coef_log2_denom != 0)
        CHK_RANGE_INT(efrc->spatial_filter_exp_coef_log2_denom, 6, 20);
    CHK_RANGE_INT(efrc->spatial_resampling_mode_hor_idc        , 0, 1);
    CHK_RANGE_INT(efrc->spatial_resampling_mode_ver_idc        , 0, 3);
    CHK_LIST_INT_2(efrc->spatial_resampling_filter_hor_idc[0]  , 0, 7);
    CHK_LIST_INT_1(efrc->spatial_resampling_filter_hor_idc[1], 0);
    CHK_LIST_INT_1(efrc->spatial_resampling_filter_hor_idc[2], 0);
    if (efrc->spatial_resampling_mode_ver_idc == 1)
    {
        CHK_LIST_INT_2(efrc->spatial_resampling_filter_ver_idc[0], 0, 7);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[1], 0);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[2], 0);
    }
    else if ((efrc->spatial_resampling_mode_ver_idc == 2) || (efrc->spatial_resampling_mode_ver_idc == 3))
    {
        CHK_LIST_INT_2(efrc->spatial_resampling_filter_ver_idc[0], 1, 7);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[1], 1);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[2], 1);
    }
    else
    {
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[0], 0);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[1], 0);
        CHK_LIST_INT_1(efrc->spatial_resampling_filter_ver_idc[2], 0);
    }

    CHK_RANGE_INT(efrc->spatial_resampling_luma_pivot[0]     , 0, ((MS_U32)1<<efrc->rpu_VDR_bit_depth)-1);
    CHK_RANGE_INT(efrc->spatial_resampling_luma_pivot[1]     , 0, ((MS_U32)1<<efrc->rpu_VDR_bit_depth)-1);
    for(i = 0; i <3; i++)
    {
        for (j=0; j < 8; j++)
        {
            CHK_RANGE_INT(efrc->spatial_filter_coeff_hor_int[i][j], -64, 63);
            CHK_RANGE_INT(efrc->spatial_filter_coeff_hor    [i][j],   0,  ((MS_U32)1<<efrc->spatial_filter_exp_coef_log2_denom) - 1);
        }
    }
    for(i = 0; i <3; i++)
    {
        for (j=0; j < 6; j++)
        {
            CHK_RANGE_INT(efrc->spatial_filter_coeff_ver_int[0][i][j], -64, 63);
            CHK_RANGE_INT(efrc->spatial_filter_coeff_ver    [0][i][j],   0,  ((MS_U32)1<<efrc->spatial_filter_exp_coef_log2_denom) - 1);
        }
    }
    for(i = 0; i <3; i++)
    {
        for (j=0; j < 6; j++)
        {
            CHK_RANGE_INT(efrc->spatial_filter_coeff_ver_int[1][i][j], -64, 63);
            CHK_RANGE_INT(efrc->spatial_filter_coeff_ver    [1][i][j],   0,  ((MS_U32)1<<efrc->spatial_filter_exp_coef_log2_denom) - 1);
        }
    }
#undef CHK_RANGE_INT
#undef CHK_LIST_INT_1
#undef CHK_LIST_INT_2
#undef CHK_LIST_INT_3

    /*
        // Phase 1 : Fit to HW
        for (i = 0; i < 3; i++)
        {
            for (j = pConfExt->num_pivots[i]; j <= 8; j ++)
                pConfExt->pivot_value[i][j] = pConfExt->pivot_value[i][j-1];
        }
        if (pConfExt->coefficient_log2_denom < 23)
        {
            for(cmp = 0; cmp < 3; cmp ++)
                for (i = 0; i < 8; i ++)
                    for (j = 0; j < 3; j ++)
                        pConfExt->poly_coef[cmp][i][j] <<= (23 - pConfExt->coefficient_log2_denom);
            for(cmp = 0; cmp < 2; cmp ++)
                for (i = 0; i < 22; i ++)
                    pConfExt->MMR_coef[cmp][i] <<= (23 - pConfExt->coefficient_log2_denom);
            for(cmp = 0; cmp < 3; cmp ++)
                pConfExt->NLQ_coeff[cmp][i] <<= (23 - pConfExt->coefficient_log2_denom);
            pConfExt->coefficient_log2_denom = 23;
        }
        if (pConfExt->spatial_filter_exp_coef_log2_denom < 20)
        {
            for(cmp = 0; cmp < 3; cmp ++)
                for (i = 0; i < 8; i ++)
                    pConfExt->spatial_filter_coeff_hor[cmp][i] <<= (20 - pConfExt->spatial_filter_exp_coef_log2_denom);
            for(cmp = 0; cmp < 2; cmp ++)
                for (i = 0; i < 3; i ++)
                    for (j = 0; j < 6; j ++)
                        pConfExt->spatial_filter_coeff_ver[cmp][i][j] <<= (20 - pConfExt->spatial_filter_exp_coef_log2_denom);
            pConfExt->spatial_filter_exp_coef_log2_denom = 20;
        }
    */
    return rv;
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_CompReadMetadata(const MS_U8* pu8MDS, MS_U16 u16Len)
{

    if (sizeof(rpu_ext_config_fixpt_main_t) > u16Len)
        return -(int)u16Len;
    memcpy(&_pCompRegTable->stConfig, pu8MDS, sizeof(rpu_ext_config_fixpt_main_t));
    if (DoVi_CompCheckMetadata(&_pCompRegTable->stConfig) < 0)
        return -1;

    return sizeof(rpu_ext_config_fixpt_main_t);
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_Composer()
{
    MS_U16 u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7C_L));
    MS_U16 u16RegUpdate = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7D_L));
    _pCompRegTable->bUpdate = (u16RegUpdate >> 15) & 0x1;
    _pCompRegTable->u8ModeBL = (u16RegEnable >> 12) & 0x3; // bit 12,13
    _pCompRegTable->u8ModeEL = (u16RegEnable >> 14) & 0x3; // bit 14,15
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Composer()
{

    MsHdr_Comp_Regtable_t *pRegTable = _pCompRegTable;
    composer_register_t *pCompReg = &_pCompRegTable->stReg;

    // Enhancement Layer
    MS_U16 u16Data;
    MS_U64 u64Data0, u64Data1, u64Data2;
    MS_U32 u32Data0, u32Data1, u32Data2, u32Data3;
    MS_S64 s64Data0, s64Data1, s64Data2;
    MS_S32 s32Data0;
    MS_U32 i,j;
    MS_U8  u8shift;

    if (!pRegTable->bUpdate)
        return;

    u16Data  = (MS_U16)(pRegTable->stConfig.spatial_resampling_filter_flag & 0x1) << 0;
    u16Data |= (MS_U16)(pRegTable->stConfig.el_spatial_resampling_filter_flag & 0x1) << 1;
    u16Data |= (MS_U16)(pRegTable->stConfig.rpu_BL_bit_depth == 8 ? 1 : 0) << 2;
    u16Data |= (MS_U16)(pRegTable->stConfig.rpu_EL_bit_depth == 8 ? 1 : 0) << 3;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_mode_hor_idc & 0x1) << 4;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_filter_hor_idc[0] & 0x1) << 5;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_mode_ver_idc & 0x1) << 8;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_filter_ver_idc[0] & 0x3) << 10;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_filter_ver_idc[1] & 0x1) << 12;
    //u16Data |= (MS_U16)(pRegTable->stConfig.spatial_resampling_filter_ver_idc[2] & 0x1) << 13;
    u16Data |= 0x110; // BL is always 4 times bigger than EL
    DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7D_01_L), u16Data);

    //protect
    if (pRegTable->stConfig.coefficient_log2_denom > 23)
    {
        u8shift = 0;
    }
    else
    {
        u8shift = 23 - pRegTable->stConfig.coefficient_log2_denom;
    }


    if (pRegTable->u8ModeEL == 2) // output = 0
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_01_L), (MS_U16)((pRegTable->stConfig.NLQ_offset[0] & 0x3FF) | 0x0000)); // disable EL
    }
    else if (pRegTable->u8ModeEL == 1) // ouput = input
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_01_L), (MS_U16)(0x0 | (((pRegTable->stConfig.disable_residual_flag & 0x1) ? 0 : 1) << 15))); // m = 0 and enable EL
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_02_L), 0x800000); // s = 1.0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_04_L), 0x7FFFFF); // r = max of 23b
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_06_L), 0x400000); // t = 0.5
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_09_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0A_L), 0x800000); // s = 1.0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0C_L), 0x7FFFFF); // r = max of 23b
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0E_L), 0x400000); // t = 0.5
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_11_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_12_L), 0x800000); // s = 1.0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_14_L), 0x7FFFFF); // r = max of 23b
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_16_L), 0x400000); // t = 0.5
    }
    else // from metadata
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_01_L), (MS_U16)((pRegTable->stConfig.NLQ_offset[0] & 0x3FF) | (((pRegTable->stConfig.disable_residual_flag & 0x1) ? 0 : 1) << 15)));
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_02_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_04_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_06_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_09_L), (MS_U16)pRegTable->stConfig.NLQ_offset[1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0A_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0C_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0E_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7E_11_L), (MS_U16)pRegTable->stConfig.NLQ_offset[2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_12_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_14_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_16_L), ((MS_U32)pRegTable->stConfig.NLQ_coeff_int[2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][2]);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][0];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_02_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][1];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_04_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[0][2];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_06_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][0];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0A_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][1];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0C_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[1][2];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_0E_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][0];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_12_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][1];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_14_L), u64Data0);

        u64Data0 = ((MS_U64)pRegTable->stConfig.NLQ_coeff_int[2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.NLQ_coeff[2][2];
        u64Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7E_16_L), u64Data0);
    }

    if (pRegTable->u8ModeBL == 2) // output = 0
    {

        // Base Layer : Polynomial predictor
        DoVi_WriteByte(DoVi_Addr(REG_SC_BK40_01_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_08_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_09_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0A_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0B_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0C_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0D_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0E_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0F_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_10_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_12_L), 0x80000); // a = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_14_L), 0x0); // b = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_16_L), 0x0); // c = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_18_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_20_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_22_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_24_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_26_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_28_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_30_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_32_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_34_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_36_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_38_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_40_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_42_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_43_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_44_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_45_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_46_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_48_L), 0x400000); // current value means 128
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_50_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_52_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_54_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_56_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_58_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5E_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_60_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_61_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_62_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_63_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_64_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_66_L), 0x400000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_68_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_70_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_72_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_74_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_76_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_78_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7C_L), 0x0);
    }
    else if (pRegTable->u8ModeBL == 1) // ouput = input
    {

        // Base Layer : Polynomial predictor
        DoVi_WriteByte(DoVi_Addr(REG_SC_BK40_01_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_08_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_09_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0A_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0B_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0C_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0D_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0E_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0F_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_10_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_12_L), 0x0); // a = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_14_L), 0x800000); // b = 1
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_16_L), 0x0); // c = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_18_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_20_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_22_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_24_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_26_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_28_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_30_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_32_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_34_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_36_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_38_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_40_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_42_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_43_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_44_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_45_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_46_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_48_L), 0x0); // a = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4A_L), 0x800000); // b = 1
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4C_L), 0x0); // c = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_50_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_52_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_54_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_56_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_58_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5E_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_60_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_61_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_62_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_63_L), 0x3FF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_64_L), 0x3FF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_66_L), 0x0); // a = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_68_L), 0x800000); // b = 1
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6A_L), 0x0); // c = 0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6C_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6E_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_70_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_72_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_74_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_76_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_78_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7A_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7C_L), 0x0);
    }
    else // from metadata
    {
        if(!_isOpenHdr)//(!IS_OPEN_HDR(0))
        {
            //rearrange pivot_value
            for (i = 0; i < 3; i++)
            {
                for (j = pRegTable->stConfig.num_pivots[i]; j <= 8; j ++) //num_pivots
                {
                    pRegTable->stConfig.pivot_value[i][j] = pRegTable->stConfig.pivot_value[i][j-1];
                }

                for (j = pRegTable->stConfig.num_pivots[i]-1; j < 8; j ++) //num_pivots
                {
                    pRegTable->stConfig.poly_coef_int[i][j][0] = pRegTable->stConfig.poly_coef_int[i][j-1][0];
                    pRegTable->stConfig.poly_coef_int[i][j][1] = pRegTable->stConfig.poly_coef_int[i][j-1][1];
                    pRegTable->stConfig.poly_coef_int[i][j][2] = pRegTable->stConfig.poly_coef_int[i][j-1][2];
                    pRegTable->stConfig.poly_coef[i][j][0] = pRegTable->stConfig.poly_coef[i][j-1][0];
                    pRegTable->stConfig.poly_coef[i][j][1] = pRegTable->stConfig.poly_coef[i][j-1][1];
                    pRegTable->stConfig.poly_coef[i][j][2] = pRegTable->stConfig.poly_coef[i][j-1][2];
                }
            }
        }

        // Base Layer : Polynomial predictor
        DoVi_WriteByte(DoVi_Addr(REG_SC_BK40_01_L), (MS_U8)((pRegTable->stConfig.mapping_idc[1] & 0x1) | ((pRegTable->stConfig.mapping_idc[2] & 0x1) << 1)));
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_08_L), (MS_U16)pRegTable->stConfig.pivot_value[0][0]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_09_L), (MS_U16)pRegTable->stConfig.pivot_value[0][1]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0A_L), (MS_U16)pRegTable->stConfig.pivot_value[0][2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0B_L), (MS_U16)pRegTable->stConfig.pivot_value[0][3]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0C_L), (MS_U16)pRegTable->stConfig.pivot_value[0][4]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0D_L), (MS_U16)pRegTable->stConfig.pivot_value[0][5]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0E_L), (MS_U16)pRegTable->stConfig.pivot_value[0][6]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_0F_L), (MS_U16)pRegTable->stConfig.pivot_value[0][7]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_10_L), (MS_U16)pRegTable->stConfig.pivot_value[0][8]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_12_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_14_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_16_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_18_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_20_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_22_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_24_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_26_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_28_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][4][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][4][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][4][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_30_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][5][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_32_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][5][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_34_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][5][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_36_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][6][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_38_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][6][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][6][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][7][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][7][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_40_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[0][7][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][2]);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_12_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_14_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][0][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_16_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_18_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][1][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_1E_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_20_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][2][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_22_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_24_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_26_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][3][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_28_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][4][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][4][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][4][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][4][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_2E_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][5][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_30_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][5][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_32_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][5][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][5][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_34_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][6][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_36_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][6][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_38_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][6][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][6][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][7][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][7][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_3E_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[0][7][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[0][7][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_40_L), s32Data0);

        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_42_L), (MS_U16)pRegTable->stConfig.pivot_value[1][0]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_43_L), (MS_U16)pRegTable->stConfig.pivot_value[1][1]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_44_L), (MS_U16)pRegTable->stConfig.pivot_value[1][2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_45_L), (MS_U16)pRegTable->stConfig.pivot_value[1][3]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_46_L), (MS_U16)pRegTable->stConfig.pivot_value[1][4]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_48_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_50_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_52_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_54_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_56_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_58_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[1][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][2]);


        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_48_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][0][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_4E_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_50_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][1][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_52_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_54_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_56_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][2][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_58_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[1][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[1][3][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_5E_L), s32Data0);

        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_60_L), (MS_U16)pRegTable->stConfig.pivot_value[2][0]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_61_L), (MS_U16)pRegTable->stConfig.pivot_value[2][1]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_62_L), (MS_U16)pRegTable->stConfig.pivot_value[2][2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_63_L), (MS_U16)pRegTable->stConfig.pivot_value[2][3]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK40_64_L), (MS_U16)pRegTable->stConfig.pivot_value[2][4]);

        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_66_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_68_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6E_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_70_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_72_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_74_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_76_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][2]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_78_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][0]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7A_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][1]);
        //DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7C_L), ((MS_U32)pRegTable->stConfig.poly_coef_int[2][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][2]);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][0][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_66_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][0][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_68_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][0][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][0][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][1][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6C_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][1][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_6E_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][1][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][1][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_70_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][2][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_72_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][2][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_74_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][2][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][2][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_76_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][3][0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][0];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_78_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][3][1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][1];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7A_L), s32Data0);

        s32Data0 = ((MS_S32)pRegTable->stConfig.poly_coef_int[2][3][2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.poly_coef[2][3][2];
        s32Data0 <<= u8shift;
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK40_7C_L), s32Data0);

        // Base Layer : MMR predictor
#define PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3) \
            u32Data0 = (MS_U32)(s64Data0 & 0xFFFFFFFF); \
            u32Data1 = (MS_U32)((s64Data0 >> 32) & 0xFF) | (MS_U32)((s64Data1 << 8) & 0xFFFFFF00); \
            u32Data2 = (MS_U32)((s64Data1 >> 24) & 0xFFFF) | (MS_U32)((s64Data2 << 16) & 0xFFFF0000); \
            u32Data3 = (MS_U32)((s64Data2 >> 16) & 0xFFFFFF);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 0];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 1];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 2];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_08_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_0A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_0C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_0E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 3] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 3];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 4] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 4];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 5] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 5];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_10_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_12_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_14_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_16_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 6] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 6];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 7] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 7];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 8] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 8];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_18_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_1A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_1C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_1E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][ 9] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][ 9];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][10] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][10];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][11] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][11];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_20_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_22_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_24_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_26_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][12] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][12];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][13] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][13];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][14] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][14];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_28_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_2A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_2C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_2E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][15] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][15];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][16] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][16];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][17] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][17];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_30_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_32_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_34_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_36_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][18] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][18];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][19] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][19];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][20] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][20];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_38_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_3A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_3C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_3E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[0][21] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[0][21];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 0] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 0];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 1] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 1];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_40_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_42_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_44_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_46_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 2] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 2];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 3] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 3];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 4] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 4];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_48_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_4A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_4C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_4E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 5] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 5];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 6] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 6];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 7] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 7];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_50_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_52_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_54_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_56_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 8] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 8];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][ 9] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][ 9];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][10] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][10];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_58_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_5A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_5C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_5E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][11] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][11];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][12] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][12];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][13] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][13];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_60_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_62_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_64_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_66_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][14] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][14];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][15] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][15];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][16] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][16];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_68_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_6A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_6C_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_6E_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][17] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][17];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][18] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][18];
        s64Data1 <<= u8shift;
        s64Data2 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][19] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][19];
        s64Data2 <<= u8shift;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_70_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_72_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_74_L), u32Data2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_76_L), u32Data3);

        s64Data0 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][20] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][20];
        s64Data0 <<= u8shift;
        s64Data1 = ((MS_S64)pRegTable->stConfig.MMR_coef_int[1][21] << pRegTable->stConfig.coefficient_log2_denom) | pRegTable->stConfig.MMR_coef[1][21];
        s64Data1 <<= u8shift;
        s64Data2 = 0;

        PACK_MMR_COEF(s64Data0, s64Data1, s64Data2, u32Data0, u32Data1, u32Data2, u32Data3);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_78_L), u32Data0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_7A_L), u32Data1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK41_7C_L), u32Data2);
    }

}




void DoVi_CompFrameDeCalculate()
{
    //printf("Dolby DoVi_CompFrameDeCalculate is running...\n");
    DoVi_Prepare_Composer();
    src_param_t srcParam;
    hdr10_param_t stHdr10Param;
    srcParam.src_bit_depth = 10;
    srcParam.src_chroma_format =  0;
    srcParam.src_yuv_range = SIG_RANGE_SDI;
    srcParam.hdr10_param = stHdr10Param;
    //commit_comp_reg( _stDmContext,FORMAT_DOVI, &_pCompRegTable->stConfig, &_pCompRegTable->stReg,&srcParam);//MsHdr_Update_Composer() has the same functionality
    //printf("Dolby DoVi_CompFrameDeCalculate is done...\n");

}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompBlankingUpdate()
{

    MsHdr_Update_Composer();
}

// DM

void DoVi_DmDumpMetadata()
{
#if 0
    int i = 0;

    DoVi_Printf("//%x //affected_dm_metadata_id\n", pMdsExt->affected_dm_metadata_id);
    DoVi_Printf("//%x //scene_refresh_flag\n", pMdsExt->scene_refresh_flag);
    //DoVi_Printf("%x //m33Yuv2RgbScale2P\n", pMdsExt->m33Yuv2RgbScale2P);
    DoVi_Printf("//%x //YCCtoRGB_coef0\n", pMdsExt->YCCtoRGB_coef0);
    DoVi_Printf("//%x //YCCtoRGB_coef1\n", pMdsExt->YCCtoRGB_coef1);
    DoVi_Printf("//%x //YCCtoRGB_coef2\n", pMdsExt->YCCtoRGB_coef2);
    DoVi_Printf("//%x //YCCtoRGB_coef3\n", pMdsExt->YCCtoRGB_coef3);
    DoVi_Printf("//%x //YCCtoRGB_coef4\n", pMdsExt->YCCtoRGB_coef4);
    DoVi_Printf("//%x //YCCtoRGB_coef5\n", pMdsExt->YCCtoRGB_coef5);
    DoVi_Printf("//%x //YCCtoRGB_coef6\n", pMdsExt->YCCtoRGB_coef6);
    DoVi_Printf("//%x //YCCtoRGB_coef7\n", pMdsExt->YCCtoRGB_coef7);
    DoVi_Printf("//%x //YCCtoRGB_coef8\n", pMdsExt->YCCtoRGB_coef8);
    DoVi_Printf("//%x //YCCtoRGB_offset0\n", pMdsExt->YCCtoRGB_offset0);
    DoVi_Printf("//%x //YCCtoRGB_offset1\n", pMdsExt->YCCtoRGB_offset1);
    DoVi_Printf("//%x //YCCtoRGB_offset2\n", pMdsExt->YCCtoRGB_offset2);
    //DoVi_Printf("%x //m33Rgb2WpLmsScale2P\n", pMdsExt->m33Rgb2WpLmsScale2P);
    DoVi_Printf("//%x //RGBtoOpt_coef0\n", pMdsExt->RGBtoOpt_coef0);
    DoVi_Printf("//%x //RGBtoOpt_coef1\n", pMdsExt->RGBtoOpt_coef1);
    DoVi_Printf("//%x //RGBtoOpt_coef2\n", pMdsExt->RGBtoOpt_coef2);
    DoVi_Printf("//%x //RGBtoOpt_coef3\n", pMdsExt->RGBtoOpt_coef3);
    DoVi_Printf("//%x //RGBtoOpt_coef4\n", pMdsExt->RGBtoOpt_coef4);
    DoVi_Printf("//%x //RGBtoOpt_coef5\n", pMdsExt->RGBtoOpt_coef5);
    DoVi_Printf("//%x //RGBtoOpt_coef6\n", pMdsExt->RGBtoOpt_coef6);
    DoVi_Printf("//%x //RGBtoOpt_coef7\n", pMdsExt->RGBtoOpt_coef7);
    DoVi_Printf("//%x //RGBtoOpt_coef8\n", pMdsExt->RGBtoOpt_coef8);
    DoVi_Printf("//%x //signal_eotf\n", pMdsExt->signal_eotf);
    DoVi_Printf("//%x //signal_eotf_param0\n", pMdsExt->signal_eotf_param0);
    DoVi_Printf("//%x //signal_eotf_param1\n", pMdsExt->signal_eotf_param1);
    DoVi_Printf("//%x //signal_eotf_param2\n", pMdsExt->signal_eotf_param2);
    DoVi_Printf("//%x //signal_bit_depth\n", pMdsExt->signal_bit_depth);
    DoVi_Printf("//%x //signal_color_space\n", pMdsExt->signal_color_space);
    DoVi_Printf("//%x //signal_chroma_format\n", pMdsExt->signal_chroma_format);
    DoVi_Printf("//%x //signal_full_range_flag\n", pMdsExt->signal_full_range_flag);
    DoVi_Printf("//%x //source_min_PQ\n", pMdsExt->source_min_PQ);
    DoVi_Printf("//%x //source_max_PQ\n", pMdsExt->source_max_PQ);
    DoVi_Printf("//%x //source_diagonal\n", pMdsExt->source_diagonal);
    DoVi_Printf("//%x //num_ext_blocks\n", pMdsExt->num_ext_blocks);
    DoVi_Printf("//%x //TrimNum\n", pMdsExt->TrimNum);
    for (i = 0; i < TrimTypeNum2; i++)
        DoVi_Printf("//%x //Trima[0][%x]\n", pMdsExt->Trima[0][i], i);
    DoVi_Printf("//%x //min_PQ\n", pMdsExt->min_PQ);
    DoVi_Printf("//%x //max_PQ\n", pMdsExt->max_PQ);
    DoVi_Printf("//%x //mid_PQ\n", pMdsExt->mid_PQ);
    DoVi_Printf("\n");
#endif
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_DmAllocRegTable(void)
{
    if (_pstDmRegTable == NULL)
    {
        _pstDmRegTable = (MsHdr_RegTable_t*)MsHdr_Malloc(sizeof(MsHdr_RegTable_t));
        if (_pstDmRegTable != NULL)
        {
            memset(_pstDmRegTable, 0, sizeof(MsHdr_RegTable_t));
            if (_pstDmRegTable->stTM.pu16Table == NULL)
            {
                _pstDmRegTable->stTM.pu16Table = (MS_U16*)MsHdr_Malloc(sizeof(MS_U16) * DOVI_TMO_SIZE);
                if (_pstDmRegTable->stTM.pu16Table != NULL)
                {
                    memset(_pstDmRegTable->stTM.pu16Table, 0, sizeof(MS_U16) * DOVI_TMO_SIZE);
                }
            }
            else
            {
                printk("%s: %d: malloc fail\n", __func__, __LINE__);
            }

            if (_pstDmRegTable->st3D.pu16Table == NULL)
            {
                _pstDmRegTable->st3D.pu16Table = (MS_U16*)MsHdr_Malloc(sizeof(MS_U16) * DOVI_3DLUT_SIZE*3);
                if (_pstDmRegTable->st3D.pu16Table != NULL)
                {
                    memset(_pstDmRegTable->st3D.pu16Table, 0, sizeof(MS_U16) * DOVI_3DLUT_SIZE*3);
                }
            }
            else
            {
                printk("%s: %d: malloc fail\n", __func__, __LINE__);
            }
        }
        else
        {
            printk("%s: %d: malloc fail\n", __func__, __LINE__);
        }
    }
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_DmFreeRegTable()
{
    if (_pstDmRegTable->stTM.pu16Table != NULL)
    {
        MsHdr_Free(_pstDmRegTable->stTM.pu16Table);
    }
    if (_pstDmRegTable->st3D.pu16Table != NULL)
    {
        MsHdr_Free(_pstDmRegTable->st3D.pu16Table);
    }
    if (_pstDmRegTable != NULL)
    {
        MsHdr_Free(_pstDmRegTable);
    }
}

//-------------------------------------------------------------------------------------------------
/// Calculate content of ordinary gamma-to-linear table. Adapt to Dolby code.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MS_U32 DoVi_Bypass_DegammaLut()
{
    MS_U32 idx;
    for (idx = 0; idx < MSTAR_DEGAMMA_LUT_SIZE; idx++)
    {
        MS_U32 u32 = idx << 23; // 256 entries : [0x0, 0x1FF] map to [0x0, 0xFF800000]

        // convert 32bit to mantissa(14b)+exponent(5b)
        MS_U16 i;
        for (i = 0; i < 18; i++)
        {
            if (u32 < (0x1 << (i+14)))
                break;
        }
        g_au32DoViDegammaBypassLut[idx] = (u32 >> i << 5) | i;
    }
}

//-------------------------------------------------------------------------------------------------
/// Retrieve content of Dolby Vision 3DLUT.
/// note:
///     The number of pre-defined 3DLUTs is defined by /DOVI_NUM_3DLUT_INSTANCES
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_Replace_3DLut(const MS_U8* pu8Array, MS_U32 u32Size)
{
    MS_U8 u8NumberOfViewModes,u8I;
    MS_U16 u16I;
    MS_U8* pu8Array2 = pu8Array;
    MS_U32 u32WastedSpace,u32SizeOfOneViewMode;
    DoVi_Printf("***************************** (*v*) ******************************\n");

    if (bIsInitalized == 1)
    {
        return 0;
    }

    u32WastedSpace = sizeof(bcms_lut_t) * 3 + sizeof(int32_t)*PQ2G_LUT_SIZE;
    u32SizeOfOneViewMode = sizeof(pq_config_t) + u32WastedSpace;
    u8NumberOfViewModes = u32Size/u32SizeOfOneViewMode;

    if(0 == bIsInitalized)
    {
        cp_mmg_t cp_mmg;

        _ui_menu_params.u16BackLightUIVal  = 50;
        _ui_menu_params.u16BrightnessUIVal = 50;
        _ui_menu_params.u16ContrastUIVal   = 50;
        memset(&run_mode, 0, sizeof(run_mode));

        if (_pq_config == NULL)
        {
            _pq_config = (pq_config_t *)malloc(sizeof(pq_config_t)*16);
        }

        DoVi_Printf("init_cp starting\n");

        if (_pstDmConfig == NULL)
        {
            _pstDmConfig = (DoVi_Config_t*)MsHdr_Malloc(sizeof(DoVi_Config_t));
            if (_pstDmConfig != NULL)
            {
                memset(_pstDmConfig, 0, sizeof(DoVi_Config_t));
            }
            else
            {
                printk("%s: %d: malloc fail\n", __func__, __LINE__);
            }
        }
        if (_pCompRegTable != NULL)
        {
            memset(_pCompRegTable, 0, sizeof(DoVi_Config_t));
        }

        /******************************************************************/
        /* Allocate memory for default and global dimming LUTs           */
        /******************************************************************/
        memset(&cp_mmg, 0, sizeof(cp_mmg_t));
        init_cp_mmg(&cp_mmg);

        //lut_buf = (char *)malloc(cp_mmg.lut_mem_size);
        //  if (!lut_buf) {
        //  printf("Allocate memory for DM LUTs failed\n");
        // return(-1);
        //}

        /******************************************************************/
        /* Allocate memory for control path and dm context context                       */
        /******************************************************************/
        if (_stDmContext == NULL)
        {
            _stDmContext = (h_cp_context_t)malloc(cp_mmg.cp_ctx_size);
            if (_stDmContext == NULL)
            {
                return -1;
            }
            memset(_stDmContext, 0, cp_mmg.cp_ctx_size);
        }

        if (dm_ctx_buf == NULL)
        {
            dm_ctx_buf = (char *)malloc(cp_mmg.dm_ctx_size);
            if (dm_ctx_buf == NULL)
            {
                return -1;
            }
            memset(dm_ctx_buf, 0, cp_mmg.dm_ctx_size);
        }

        if(0 != init_cp(_stDmContext, &run_mode, /*lut_buf*/NULL, dm_ctx_buf))
        {
            printk("init_cp failed!!!!!!!!!!!!!!!!\n");
            return 4;
        }
        else
        {
            DoVi_Printf("init_cp sucessful!!!!!!!!!!!!!!!!\n");
        }
        bIsInitalized = 1;
    }


    DoVi_Printf("u8NumberOfViewModes = %d\n", u8NumberOfViewModes);
    for(u8I = 0; u8I< min(u8NumberOfViewModes,16); u8I++)
    {
        pu8Array2 +=u32WastedSpace;  //skips unused parts of the PQ binary file
        fread(&_pq_config[u8I] ,sizeof(pq_config_t),1,pu8Array2);
    }
    for(u16I = 0; u16I< u8NumberOfViewModes; u16I++)
    {
        _stDmContext->cur_pq_config = &_pq_config[u16I];
        DoVi_Printf("max pq of mode %d =%d\n",u16I,_stDmContext->cur_pq_config->target_display_config.maxPq);
        DoVi_Printf("Global Dimming Enable of mode %d =%d\n",u16I,_stDmContext->cur_pq_config->target_display_config.gdConfig.gdEnable);
    }

    _stDmContext->cur_pq_config = &_pq_config[_u8ViewModeId];

    return 0;
}


void DoVi_Set_HDRInputType(MS_U8* pConfig, MS_U8 u8InputType)
{
    if (_pstDmConfig != NULL)
    {
        _pstDmConfig->u8InputType = u8InputType;
    }
}
//-------------------------------------------------------------------------------------------------
/// Dolby Vision default setting and initialization. From INI file?
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmSetDefaultConfig()
{
#if 0
    pConfig->u8RunMode = 0;
    pConfig->u8InputBits = 14;
    pConfig->u8OutputBits = 12;
    pConfig->phyLutBaseAddr = 0x0;
    pConfig->u8Lut3DIdx = 0;
    pConfig->u16SminPQ = 62; // 0.005nits
    pConfig->u16SmaxPQ = 3696; // 4000nits
    pConfig->u16Sgamma = 39322; // 2.4
    pConfig->u8SMode = 1; // P3D65
    pConfig->u8SEOTF = 1; // PQ
    pConfig->u8SSignalRange = 2; // SDI

    // target
    pConfig->u16Sdiagonalinches = 42;
    pConfig->s32CrossTalk = 655; // 0.02

    pConfig->u16TminPQ = 62; // 0.05nits
    pConfig->u16TmaxPQ = 2081;//2547 for 300nits, 2081 for 100 nits
    pConfig->u16Tgamma = 39322; // 2.4
    pConfig->u8TMode = 0; // Rec709
    pConfig->u8TEOTF = 0; // Bt1886
    pConfig->u8TSignalRange = 1; // Full
    pConfig->u16Tdiagonalinches = 42;

    pConfig->u8RunMode = 0;
    pConfig->s16TMinPQBias = 0;
    pConfig->s16TMidPQBias = 0;
    pConfig->s16TMaxPQBias = 0;
    pConfig->s16TrimSlopeBias = 0;
    pConfig->s16TrimOffsetBias = 0;
    pConfig->s16TrimPowerBias = 0;
    pConfig->s16TContrast = 0;
    pConfig->s16TBrightness = 0;
    pConfig->s32Rolloff = (((MS_U32)1 << 31) - 1) / 3; // 1/3
    pConfig->u8MsMethod = 4; // 0 : MS_METHOD_OFF 4 : MS_METHOD_DB_EDGE
    pConfig->s16MsWeight = 2048;
    pConfig->s16MsWeightBias = 0;
    pConfig->u16MsWeightEdge = 16380;
    pConfig->u16CrossTalk = 655; // = 0.02 // Format 1.15
    pConfig->u16ChromaWeight = 0; //
    pConfig->s16ChromaWeightBias = 0;
    pConfig->u16CcGain = 2048; // = 0.5 // Format 0.12
    pConfig->u16CcOffset = 4095; // = 1.0 // Format 0.15
    pConfig->u16SaturationGain = 4095;
    pConfig->s16SaturationGainBias = 0;
    pConfig->s16Lut3DMin[0] = (MS_S16)0x01EE; // 0x5e7;
    pConfig->s16Lut3DMin[1] = (MS_S16)0xCF3A; // 0xb067;
    pConfig->s16Lut3DMin[2] = (MS_S16)0xCF3A; // 0xb067;
    pConfig->s16Lut3DMax[0] = (MS_S16)0x4108; // 0x4f99;
    pConfig->s16Lut3DMax[1] = (MS_S16)0x30C6; // 0x4f99;
    pConfig->s16Lut3DMax[2] = (MS_S16)0x30C6; // 0x4f99;
#endif

    DoVi_Bypass_DegammaLut();
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Y2R.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0102( MsHdr_Y2R_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs,h_cp_context_t   pDmContext)
{
    const DmKsIMapFxp_t *pKsIMap = &pDmKs->ksIMap;
    const MS_U8  u8InputBits = pDmKs->ksFrmFmtI.bdp;
    const MS_U8 u8ConfigType = _pstDmConfig->u8InputType;
    const MS_U8 u8VdrBitDepth = _pCompRegTable->stConfig.rpu_VDR_bit_depth;
    MS_S64 s64OffsetY ;
    MS_S64 s64OffsetU ;
    MS_S64 s64OffsetV ;
    MS_U8 u8ExtraShift = 0;

    DoVi_Printf("//[DoVi_Prepare_B0102]\n");
    // printf("u8VdrBitDepth = %d\n",u8VdrBitDepth);

    pRegTable->u8Shift = pKsIMap->m33Yuv2RgbScale2P-12;//pKsIMap->m33Yuv2RgbScale2P;// 2; // 0:shift 10b, 1:shift 11b, 2:shift 12b, 3:shift 9b

    pRegTable->u8Round = 1;
    if(0 == u8ExtraShift)
    {
        pRegTable->s16Coef0 = pKsIMap->m33Yuv2Rgb[0][0];
        pRegTable->s16Coef1 = pKsIMap->m33Yuv2Rgb[0][1];
        pRegTable->s16Coef2 = pKsIMap->m33Yuv2Rgb[0][2];
        pRegTable->s16Coef3 = pKsIMap->m33Yuv2Rgb[1][0];
        pRegTable->s16Coef4 = pKsIMap->m33Yuv2Rgb[1][1];
        pRegTable->s16Coef5 = pKsIMap->m33Yuv2Rgb[1][2];
        pRegTable->s16Coef6 = pKsIMap->m33Yuv2Rgb[2][0];
        pRegTable->s16Coef7 = pKsIMap->m33Yuv2Rgb[2][1];
        pRegTable->s16Coef8 = pKsIMap->m33Yuv2Rgb[2][2];
        pRegTable->s32Offset0 = pKsIMap->v3Yuv2RgbOffInRgb[0];
        pRegTable->s32Offset1 = pKsIMap->v3Yuv2RgbOffInRgb[1];
        pRegTable->s32Offset2 = pKsIMap->v3Yuv2RgbOffInRgb[2];
    }
    else
    {
        pRegTable->s16Coef0 = pKsIMap->m33Yuv2Rgb[0][0] >> u8ExtraShift;
        pRegTable->s16Coef1 = pKsIMap->m33Yuv2Rgb[0][1] >> u8ExtraShift;
        pRegTable->s16Coef2 = pKsIMap->m33Yuv2Rgb[0][2] >> u8ExtraShift;
        pRegTable->s16Coef3 = pKsIMap->m33Yuv2Rgb[1][0] >> u8ExtraShift;
        pRegTable->s16Coef4 = pKsIMap->m33Yuv2Rgb[1][1] >> u8ExtraShift;
        pRegTable->s16Coef5 = pKsIMap->m33Yuv2Rgb[1][2] >> u8ExtraShift;
        pRegTable->s16Coef6 = pKsIMap->m33Yuv2Rgb[2][0] >> u8ExtraShift;
        pRegTable->s16Coef7 = pKsIMap->m33Yuv2Rgb[2][1] >> u8ExtraShift;
        pRegTable->s16Coef8 = pKsIMap->m33Yuv2Rgb[2][2] >> u8ExtraShift;
        pRegTable->s32Offset0 = pKsIMap->v3Yuv2RgbOffInRgb[0] >> u8ExtraShift;
        pRegTable->s32Offset1 = pKsIMap->v3Yuv2RgbOffInRgb[1] >> u8ExtraShift;
        pRegTable->s32Offset2 = pKsIMap->v3Yuv2RgbOffInRgb[2] >> u8ExtraShift;
    }


    pRegTable->u16Min = pKsIMap->eotfParam.rangeMin << (16 - u8InputBits);
    pRegTable->u16Max = (pKsIMap->eotfParam.range + pKsIMap->eotfParam.rangeMin) << (16 - u8InputBits);
    pRegTable->u32Inv = pKsIMap->eotfParam.rangeInv;

    if(!_isOpenHdr)
    {
        if (DOVI_DM_INPUT_TYPE_OTT == u8ConfigType)
        {
            if (u8VdrBitDepth > 14)
            {
                pRegTable->s32Offset0 = pRegTable->s32Offset0 >> (u8VdrBitDepth-14);
                pRegTable->s32Offset1 = pRegTable->s32Offset1 >> (u8VdrBitDepth-14);
                pRegTable->s32Offset2 = pRegTable->s32Offset2 >> (u8VdrBitDepth-14);
            }
            else
            {
                pRegTable->s32Offset0 = pRegTable->s32Offset0 << (14-u8VdrBitDepth);
                pRegTable->s32Offset1 = pRegTable->s32Offset1 << (14-u8VdrBitDepth);
                pRegTable->s32Offset2 = pRegTable->s32Offset2 << (14-u8VdrBitDepth);
            }
        }
        else
        {
            pRegTable->s32Offset0 = pRegTable->s32Offset0 << 2;
            pRegTable->s32Offset1 = pRegTable->s32Offset1 << 2;
            pRegTable->s32Offset2 = pRegTable->s32Offset2 << 2;
        }
    }
    else
    {
        pRegTable->s32Offset0 = pRegTable->s32Offset0 << 4;
        pRegTable->s32Offset1 = pRegTable->s32Offset1 << 4;
        pRegTable->s32Offset2 = pRegTable->s32Offset2 << 4;
    }

    if(5 == DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_01_L)))
    {
        pRegTable->s32Offset0 = pRegTable->s32Offset0 << DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_02_L));
        pRegTable->s32Offset1 = pRegTable->s32Offset1 <<  DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_02_L));
        pRegTable->s32Offset2 = pRegTable->s32Offset2 <<  DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_02_L));

        pRegTable->s32Offset0 = pRegTable->s32Offset0 >> DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_03_L));
        pRegTable->s32Offset1 = pRegTable->s32Offset1 >>  DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_03_L));
        pRegTable->s32Offset2 = pRegTable->s32Offset2 >>  DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_03_L));
    }
    //for HDR10 , add by Brian
#ifdef SupportHDR10
    //MS_BOOL bUpdate, bEnable;
    //MS_U8 u8Shift, u8Round;
    pRegTable->s16Coef0 = 9575;
    pRegTable->s16Coef1 = 0;
    pRegTable->s16Coef2 = 13804;
    pRegTable->s16Coef3 = 9575;
    pRegTable->s16Coef4 = -1541;
    pRegTable->s16Coef5 = -5349;
    pRegTable->s16Coef6 = 9576;
    pRegTable->s16Coef7 = 17613;
    pRegTable->s16Coef8 = 0;
    pRegTable->s32Offset0 = 122887168;
    pRegTable->s32Offset1 = -46638080;
    pRegTable->s32Offset2 = 154091520;
    pRegTable->u16Min = 4096U;
    pRegTable->u16Max = 60160U;
    pRegTable->u32Inv = 76608U;

#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Degamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0103(MsHdr_Degamma_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B0103]\n");
    MS_U32 u32Idx;
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;

    //const MS_U8 u8EidbGa[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 6}; // 256 entries G2L
    //const MS_U8 u8EidbPq[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 7}; // 512 entries PQ2L

    const MS_U8 u8EidbPq[16] = {8, 0, 1, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6};
    const MS_U16 u16EaoffPq[16] = {0, 2, 4, 8, 16, 32, 48, 64, 96, 128, 160, 192, 224, 256, 320, 384};
    const MS_U8 u8EidbPq_sim[16] = {0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7};
    const MS_U16 u16EaoffPq_sim[16] = {0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 8, 16, 32, 64, 128, 256};
    const MS_U8 u8EidbGa[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 6, 4, 4, 5, 6};
    const MS_U16 u16EaoffGa[16] = {0, 0, 0, 0,  0,  0,  0,  0,  0,   0,   0, 128, 256, 288, 320, 384};

    MS_U16 u16Sp = 0xFC00;
    const MS_U8* pu8Eidb = &u8EidbGa[0];
    const MS_U16* pu16Eaoff = &u16EaoffGa[0];
    MS_U16 u16RegEnable = 0;
    MS_U8 i;
    //for HDR10 , add by Brian
#ifdef SupportHDR10
    //pMdsExt->signal_eotf = 0xffff; //force PQ2L
#endif

    pRegTable->u32Size = MSTAR_DEGAMMA_LUT_SIZE;
    if (pRegTable->bEnable == FALSE)
    {
        pRegTable->pu32Table = &g_au32DoViDegammaBypassLut[0];
    }
    else
    {
        // PQ or Gamma
        pRegTable->pu32Table = &pDmKs->ksIMap.g2L[0];
    }

    if(_isOpenHdr)
    {
        DoVi_Printf("degamma is using HDR10 degamma\n");
        // for(i = 0; i<512; i++)
        // {
        //     pRegTable->pu32Table[i] =g_au32DoViDegammaPqLut[i]; //HDR10
        //  }
        pRegTable->pu32Table =&g_au32DoViDegammaPqLut2[0]; //HDR10
    }

    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = MSTAR_DEGAMMA_LUT_SIZE - 1;

        if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
        {
            MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu32Table, pRegTable->u32Size * sizeof(MS_U32), (void*)&stAutoDownloadFormat);
        }
        else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))
        {
            MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu32Table, pRegTable->u32Size * sizeof(MS_U32), (void*)&stAutoDownloadFormat);
        }
    }
    else // use RIU
    {
#if 0
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 6); // lut_rd_en
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x0, 0x3, 0); // lut_sel
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 5); // lut_wd_dup_md
        //DoVi_WriteByteMask(DoVi_Addr_H(0x7A, 0x60), 0x1, 0x1, 4); // lut_fast_md
        //DoVi_Write2Bytes(DoVi_Addr_L(0x7A, 0x69), 0); // lut_init_addr

        MS_U32* pu32Lut = pRegTable->pu32Table;
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 7); // lut_load_en
        for (u32Idx = 0; u32Idx < 512; u32Idx++)
        {
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_61_L), u32Idx); // lut_addr
            DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_62_L), pu32Lut[u32Idx]); // wd0
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x1, 0x1, 4); // lut_w_pulse
            DoVi_WaitByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0); // wait lut_flag_rb
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x2, 0x2, 0); // lut_flag_clr
        }
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 7); // lut_load_en
#endif
    }

    if ((pDmKs->ksIMap.eotfParam.eotf) == EOTF_MODE_PQ || _isOpenHdr)
    {
        u16Sp = 0xFF80;
        pu8Eidb = &u8EidbPq_sim[0];
        pu16Eaoff = &u16EaoffPq_sim[0];
    }
    pRegTable->u16Sp = u16Sp;
    for (u32Idx = 0; u32Idx < 16; u32Idx++)
    {
        pRegTable->au8Eidb[u32Idx] = pu8Eidb[u32Idx];
        pRegTable->au16Eaoff[u32Idx] = pu16Eaoff[u32Idx];
    }
    pRegTable->u32EndDiff0 = 0;
    pRegTable->u32EndDiff1 = 0;
    pRegTable->u32EndDiff2 = 0;
    pRegTable->u32Limit0 = 0x9C400000;
    pRegTable->u32Limit1 = 0x9C400000;
    pRegTable->u32Limit2 = 0x9C400000;
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of CSC.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0104(MsHdr_CSC_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    const DmKsIMapFxp_t *pKsIMap = &pDmKs->ksIMap;
    DoVi_Printf("//[DoVi_Prepare_B0104]\n");
    pRegTable->u8Shift = (pKsIMap->m33Rgb2LmsScale2P)-12;//2; // 0:shift 12b, 1:shift 13b, 2:shift 14b, 3:shift 15b

    pRegTable->u8Round = 0;
    pRegTable->u16Offset0 = 0x0;
    pRegTable->u16Offset1 = 0x0;
    pRegTable->u16Offset2 = 0x0;
    pRegTable->u16Min0 = 0x0;
    pRegTable->u16Min1 = 0x0;
    pRegTable->u16Min2 = 0x0;
    pRegTable->u16Max0 = 0x9C40; // 40000, clamp at Val << 16
    pRegTable->u16Max1 = 0x9C40;
    pRegTable->u16Max2 = 0x9C40;

    pRegTable->s16Coef0 = pKsIMap->m33Rgb2Lms[0][0];
    pRegTable->s16Coef1 = pKsIMap->m33Rgb2Lms[0][1];
    pRegTable->s16Coef2 = pKsIMap->m33Rgb2Lms[0][2];
    pRegTable->s16Coef3 = pKsIMap->m33Rgb2Lms[1][0];
    pRegTable->s16Coef4 = pKsIMap->m33Rgb2Lms[1][1];
    pRegTable->s16Coef5 = pKsIMap->m33Rgb2Lms[1][2];
    pRegTable->s16Coef6 = pKsIMap->m33Rgb2Lms[2][0];
    pRegTable->s16Coef7 = pKsIMap->m33Rgb2Lms[2][1];
    pRegTable->s16Coef8 = pKsIMap->m33Rgb2Lms[2][2];

    //for HDR10 , add by Brian
#ifdef SupportHDR10
    //MS_BOOL bUpdate, bEnable;
    //MS_U8 u8Shift, u8Round;
    pRegTable->s16Coef0 = 6987; //13972>>1
    pRegTable->s16Coef1 = 8678; //17355>>1
    pRegTable->s16Coef2 = 719; //1438>>1
    pRegTable->s16Coef3 = 2691; //5383>>1
    pRegTable->s16Coef4 = 12118; //24233>>1
    pRegTable->s16Coef5 = 1575; //3149>>1
    pRegTable->s16Coef6 = 197; //394>>1
    pRegTable->s16Coef7 = 829; //1657>>1
    pRegTable->s16Coef8 = 15358; //30714>>1

#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Gamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0105(MsHdr_Gamma_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B0105]\n");
    MS_U32 u32Idx;
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
    const MS_U8 u8EidbPq[32] = {8, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 3
                               };
    const MS_U16 u16EaoffPq[32] = {0,   2,   4,   8,  16,  32,  48,  64,  80,  96, 112, 128, 144, 160, 176, 192,
                                   208, 224, 240, 256, 272, 288, 304, 320, 336, 352, 368, 384, 400, 432, 464, 496
                                  };

    MS_U32 u32Sp = 0xFFFFFFFF;
    const MS_U8* pu8Eidb = &u8EidbPq[0];
    const MS_U16* pu16Eaoff = &u16EaoffPq[0];
    MS_U16 u16RegEnable = 0;

    pRegTable->u32Size = MSTAR_GAMMA_LUT_SIZE;
    if (pRegTable->bEnable == FALSE)
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu16Table[u32Idx] = u32Idx << 7;
    }
    else
    {
        pRegTable->pu16Table = &g_au16DoViGammaPqLut[0];
    }

    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = MSTAR_GAMMA_LUT_SIZE - 1;
    }
    else
    {
#if 0
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 6); // lut_rd_en
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x1, 0x3, 0); // lut_sel
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 5); // lut_wd_dup_md
        //DoVi_WriteByteMask(DoVi_Addr_H(0x7A, 0x60), 0x1, 0x1, 4); // lut_fast_md
        //DoVi_Write2Bytes(DoVi_Addr_L(0x7A, 0x69), 0); // lut_init_addr

        MS_U16* pu16Lut = pRegTable->pu16Table;
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 7); // lut_load_en
        for (u32Idx = 0; u32Idx < 512; u32Idx++)
        {
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_61_L), u32Idx); // lut_addr
            DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_62_L), pu16Lut[u32Idx]); // wd0
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x1, 0x1, 4); // lut_w_pulse
            DoVi_WaitByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0); // wait lut_flag_rb
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x2, 0x2, 0); // lut_flag_clr
        }
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 7); // lut_load_en
#endif
    }

    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
    {
        MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
    }
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))
    {
        MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
    }

    pRegTable->u32Sp = u32Sp;
    for (u32Idx = 0; u32Idx < 32; u32Idx++)
    {
        pRegTable->au8Eidb[u32Idx] = pu8Eidb[u32Idx];
        pRegTable->au16Eaoff[u32Idx] = pu16Eaoff[u32Idx];
    }
    pRegTable->u16EndDiff0 = 0x0;
    pRegTable->u16EndDiff1 = 0x0;
    pRegTable->u16EndDiff2 = 0x0;
    pRegTable->u16Limit0 = 0xFFFF;
    pRegTable->u16Limit1 = 0xFFFF;
    pRegTable->u16Limit2 = 0xFFFF;
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of R2Y.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0106(MsHdr_R2Y_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    const DmKsIMapFxp_t *pKsIMap = &pDmKs->ksIMap;
//  int16_t m33Lms2IptScale2P;
// int16_t m33Lms2Ipt[3][3];
    DoVi_Printf("//[DoVi_Prepare_B0106]\n");
    pRegTable->u8Shift = pKsIMap->m33Lms2IptScale2P-11;//1; // 0:shift 12b, 1:shift 13b, 2:shift 14b, 3:shift 15b

    pRegTable->u8Round = 0;
    pRegTable->s16Coef0 = pKsIMap->m33Lms2Ipt[0][0];//0x666;
    pRegTable->s16Coef1 = pKsIMap->m33Lms2Ipt[0][1];// 0x666;
    pRegTable->s16Coef2 = pKsIMap->m33Lms2Ipt[0][2];// 0x333;
    pRegTable->s16Coef3 = pKsIMap->m33Lms2Ipt[1][0];// 0x4748;
    pRegTable->s16Coef4 = pKsIMap->m33Lms2Ipt[1][1];// 0xb262;
    pRegTable->s16Coef5 = pKsIMap->m33Lms2Ipt[1][2];// 0x656;
    pRegTable->s16Coef6 = pKsIMap->m33Lms2Ipt[2][0];// 0xce4;
    pRegTable->s16Coef7 = pKsIMap->m33Lms2Ipt[2][1];// 0x5b7;
    pRegTable->s16Coef8 = pKsIMap->m33Lms2Ipt[2][2];// 0xed65;
    pRegTable->s16Offset0 = 0x0;
    pRegTable->s16Offset1 = 0x0;
    pRegTable->s16Offset2 = 0x0;
    pRegTable->s16Min0 = 0x8001;
    pRegTable->s16Min1 = 0x8001;
    pRegTable->s16Min2 = 0x8001;
    pRegTable->s16Max0 = 0x7fff;
    pRegTable->s16Max1 = 0x7fff;
    pRegTable->s16Max2 = 0x7fff;
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Color Adjustment.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0107(MsHdr_CA_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B0107]\n");
    pRegTable->u16ChromaWeight = pDmKs->ksTMap.chromaWeight;

    //for HDR10 , add by Brian
#ifdef SupportHDR10
    pRegTable->u16ChromaWeight = 0;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Tone Mapping.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B02(MsHdr_TM_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B02]\n");
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
    MS_U32 u32Idx = 0;
    MS_U16 u16RegEnable = 0;
    int i;
    pRegTable->u32Size = DOVI_TMO_SIZE;
    pRegTable->u8InGain = 0x10;
    pRegTable->u8OutGain = 0x10;
    pRegTable->u8UserGain = 0x10;

    if (pRegTable->bEnable == FALSE || 0 == pDmKs->ksTMap.tmLutMaxVal  )
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu16Table[u32Idx] = u32Idx << 3; // 9b index to 12b data
    }
    else
    {
        //memcpy(target, source, size * number)
        memcpy( pRegTable->pu16Table, pDmKs->ksTMap.tmInternal515Lut, DOVI_TMO_SIZE*sizeof(MS_U16) );
    }
    //pRegTable->pu16Table[5] = pRegTable->pu16Table[6];
    //pRegTable->pu16Table[511] = pDmKs->ksTMap.tmLutMaxVal;
    for(i = 0; i<511; i++)
    {
        pRegTable->pu16Table[i] = pRegTable->pu16Table[i+1];

    }
    for(i = 0; i<510; i++)
    {

        if(pRegTable->pu16Table[i+1]<pRegTable->pu16Table[i])
            pRegTable->pu16Table[i+1] = pRegTable->pu16Table[i];
    }
    if(0 != pDmKs->ksTMap.tmLutMaxVal)
    {
        pRegTable->pu16Table[511] = pDmKs->ksTMap.tmLutMaxVal;
    }

    if(0xf000 <= pRegTable->pu16Table[511] || 0xf000 <= pRegTable->pu16Table[0])
    {
        DoVi_Printf("TMO will not be updated\n");
        DoVi_Printf("pRegTable->pu16Table[0] = %d\n",pRegTable->pu16Table[0]);
        DoVi_Printf("pRegTable->pu16Table[511] = %d\n",pRegTable->pu16Table[511]);
        return;
    }
    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {

        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = DOVI_TMO_SIZE - 1;

        if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
        {
            MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
        }
        else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))
        {
            MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
        }
    }
    else
    {
#if 0
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 6); // lut_rd_en
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x2, 0x3, 0); // lut_sel
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 5); // lut_wd_dup_md
        //DoVi_WriteByteMask(DoVi_Addr_H(0x7A, 0x60), 0x1, 0x1, 4); // lut_fast_md
        //DoVi_Write2Bytes(DoVi_Addr_L(0x7A, 0x69), 0); // lut_init_addr

        MS_U16* pu16Lut = pRegTable->pu16Table;
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0x1, 7); // lut_load_en
        for (u32Idx = 0; u32Idx < 512; u32Idx++)
        {
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_61_L), u32Idx); // lut_addr
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_62_L), pu16Lut[u32Idx]); // wd0
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_L), 0x1, 0x1, 4); // lut_w_pulse
            DoVi_WaitByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x1, 0); // wait lut_flag_rb
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x2, 0x2, 0); // lut_flag_clr
        }
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7A_60_H), 0x0, 0x1, 7); // lut_load_en
#endif

    }
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Detail Restoration.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B03(MsHdr_DR_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B03]\n");
    pRegTable->u16MsWeightEdge = pDmKs->ksMs.msEdgeWeight;
    pRegTable->s16MsWeight = 0xFFF - pDmKs->ksMs.msWeight;
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Color Correction.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B04(MsHdr_CC_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    DoVi_Printf("//[DoVi_Prepare_B04]\n");

    //uint16_t tRangeMin, tRange;
    //uint32_t tRangeOverOne;
    //uint32_t tRangeInv;
    pRegTable->bClampEn = ENABLE; //unknown
    pRegTable->u16HuntGain = pDmKs->ksOMap.gain;
    pRegTable->u16HuntOffset = pDmKs->ksOMap.offset;
    pRegTable->u16SatGain = pDmKs->ksOMap.satGain;
    pRegTable->u16HighTh = 0xFFFF;
    pRegTable->u16LowTh = 0x4000;
    pRegTable->u16ZeroTh = 0x7FFF;
    pRegTable->u16CbOffset = 0;
    pRegTable->u16CrOffset = 0;

    //for HDR10 , add by Brian
#ifdef SupportHDR10
    pRegTable->bClampEn = ENABLE;
    pRegTable->u16SatGain = 0x1000;
#endif
}

void MsHdr_Update_3D(const MsHdr_3D_RegTable_t* pRegTable)
{
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
#ifdef IPT
    static MS_U16 _dolby_hdmi_ip_trig_count = 0;
#endif
    DoVi_Printf("//[MsHdr_Update_3D]\n");
    if (FALSE == pRegTable->bEnable)
    {
        // Note: 2 options to bypass 3D-LUT
        //   (1) HW-bypass : You should manually turn on h02[4] reg_b06_bypass_en to bypass 3D-LUT (without updating LUT).
        //       Also let h59[12:0] reg_b05_uv_post_ofs be 0x800 to change UV from signed to unsigned.
        //   (2) SW-bypass : You should manually load bypass_3D-LUT
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_50_L), 0x3FFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_51_L), 0xC000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_52_L), 0x8000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_54_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_55_L), 0x7FFF);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_56_L), 0x8000);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_50_L), (MS_U16)pRegTable->s16MaxC);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_51_L), (MS_U16)pRegTable->s16MinC);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_52_L), pRegTable->u32InvC);
#ifdef IPT
        if (MHal_XC_GetDolbyStatus() & BIT(1))
        {
            _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].s16MaxY = (MS_U16)pRegTable->s16MinY;
            _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].s16MinY = (MS_U16)pRegTable->s16MaxY;
            _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].u32InvY = (MS_U32)pRegTable->u32InvY;
            _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].u16Count = IPT_COUNT;
            //printk(KERN_CRIT"\033[1;32m[%s:%d][%d]0x%x:0x%x:0x%x\033[m\n",__FUNCTION__,__LINE__,_dolby_hdmi_ip_trig_count,
            //                                                                                    _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].s16MaxY,
            //                                                                                    _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].s16MinY,
            //                                                                                    _DV_HDMI_INPUT_TRIG[_dolby_hdmi_ip_trig_count].u32InvY
            //                                                                                    );
            _dolby_hdmi_ip_trig_count++;
            if(_dolby_hdmi_ip_trig_count > (IPT_COUNT-1))
            {
                _dolby_hdmi_ip_trig_count = 0;
            }
        }
        else
        {
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_54_L), (MS_U16)pRegTable->s16MinY);
            DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_55_L), (MS_U16)pRegTable->s16MaxY);
            DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_56_L), pRegTable->u32InvY);
        }
#else
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_54_L), (MS_U16)pRegTable->s16MinY);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_55_L), (MS_U16)pRegTable->s16MaxY);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_56_L), pRegTable->u32InvY);
#endif
    }

    stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT;
    stAutoDownloadFormat.bEnableRange = ENABLE;
    stAutoDownloadFormat.u16StartAddr = 0;
    stAutoDownloadFormat.u16EndAddr = DOVI_3DLUT_SIZE - 1;
    if ((CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
    {
        MHal_XC_StoreHDRAutoDownload((MS_U8 *)(pRegTable->pu16Table),  /*pRegTable->u32Size*/DOVI_3DLUT_SIZE*3 * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
    }
    else if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))
    {
        MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8 *)(pRegTable->pu16Table),  /*pRegTable->u32Size*/DOVI_3DLUT_SIZE*3 * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
    }
}

void DoVi_isOpenHdr(MS_BOOL bOn)
{
    _isOpenHdr = bOn;
}

void DoVi_Prepare_B05(MsHdr_3D_RegTable_t* pRegTable, const DmKsFxp_t *pDmKs)
{
    int i;
    DoVi_Printf("//[DoVi_Prepare_B05]\n");
    pRegTable->bEnable = TRUE;
    pRegTable->u32Size =  pDmKs->ksOMap.ksGmLut.dimC1 * pDmKs->ksOMap.ksGmLut.dimC2 * pDmKs->ksOMap.ksGmLut.dimC3*3;
    pRegTable->s16MaxY = pDmKs->ksOMap.ksGmLut.iMaxC1;
    pRegTable->s16MinY = pDmKs->ksOMap.ksGmLut.iMinC1;
    pRegTable->u32InvY =  (pDmKs->ksOMap.ksGmLut.iDistC1Inv);
    pRegTable->s16MinC = pDmKs->ksOMap.ksGmLut.iMinC2;
    pRegTable->s16MaxC = pDmKs->ksOMap.ksGmLut.iMaxC2;
    pRegTable->u32InvC = (pDmKs->ksOMap.ksGmLut.iDistC2Inv);

    /*   pRegTable->st3D.s16MinY = pConfig->s16Lut3DMin[0];
    pRegTable->st3D.s16MaxY = pConfig->s16Lut3DMax[0];
    pRegTable->st3D.u32InvY = ((MS_U32)1 << 30) / (pRegTable->st3D.s16MaxY - pRegTable->st3D.s16MinY);
    pRegTable->st3D.s16MinC = pConfig->s16Lut3DMin[1];
    pRegTable->st3D.s16MaxC = pConfig->s16Lut3DMax[1];
    pRegTable->st3D.u32InvC = ((MS_U32)1 << 30) / (pRegTable->st3D.s16MaxC - pRegTable->st3D.s16MinC);*/

    //pRegTable->pu16Table = pDmKs->ksOMap.ksGmLut.lutMap;


    /*
    typedef struct DmKs2GmLutFxp_t_ {
      int16_t dimC1, dimC2, dimC3; // dim for C1, C2, C3

      uint16_t valTp; // assume value from GmLutType_t

      int16_t iMinC1, iMaxC1, iMinC2, iMaxC2, iMinC3, iMaxC3;
      int32_t iDistC1Inv, iDistC2Inv, iDistC3Inv;

        // the pre-calculated  value
      int16_t pitch, slice;

      ////// lut data
      uint16_t lutMap[3*GMLUT_MAX_DIM*GMLUT_MAX_DIM*GMLUT_MAX_DIM];
    } DmKs2GmLutFxp_t;
    */

#if 1
    const uint16_t *lutBefore = pDmKs->ksOMap.ksGmLut.lutMap;

    //for(i = 0; i < 10; i++)
    //{
    // printf("lutBefore%d = 0x%x\n",i,lutBefore[i]);
    //}
    /*
    int errorHappened = 0;
    for(i = 0; i<17*17*17*3 ;i++)
    {
        if(lutBefore[i] !=GdLUT[i])
        {
            printf("Error in entry number %d, driver version = %d Cmodel version = %d\n",i,lutBefore[i],GdLUT[i]);
            errorHappened = 1;
        }


    }
    if(!errorHappened)
    {
        printf("Global Dimming 3D LUT before re ordering is correct\n");
    }


       */

    uint16_t *lutAfter = pRegTable->pu16Table;
    DoVi_3DLUT_parsing_reorder_noHeader(lutAfter, lutBefore);

    // for(i = 0; i < 10; i++)
    // {
    //  printf("lutAfter%d = 0x%x\n",i,lutAfter[i]);
    // }

#endif

#if 0

    int errorHappened = 0;
    for(i = 0; i<17*17*17*3 ; i++)
    {
        if(abs(lutBefore[i] -noGDLut[i])>16)
        {
            printk("Error in entry number %d, driver version = 0x%x Cmodel version = 0x%x\n",i,lutBefore[i],GdLUT[i]);
            errorHappened = 1;
        }


    }
    if(!errorHappened)
    {
        printk("3D LUT after re ordering is correct\n");
    }
#endif

//for HDR10 , add by Brian
#ifdef SupportHDR10
    pRegTable->s16MinY = 0x01ee;
    pRegTable->s16MaxY = 0x4108;
    pRegTable->u32InvY = 0x103a5;
    pRegTable->s16MinC = 0xcf3a;
    pRegTable->s16MaxC = 0x30c6;
    pRegTable->u32InvC = 0xa7f5;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Y2R.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Y2R(const MsHdr_Y2R_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_Y2R]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), 0x1, 0x3, 0);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), 0x1, 0x1, 1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_08_L), 0x2000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_09_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0A_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0B_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0C_L), 0x2000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0D_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0E_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0F_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_10_L), 0x2000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_11_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_13_L), 0x0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_15_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_17_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_18_L), 0xffff);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_19_L), 0x10000);
    }
    else
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), pRegTable->u8Shift, 0x3, 0);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), pRegTable->u8Round, 0x1, 1);
        //  printf("REG_SC_BK7B_08_L should equal to 0x%x\n",(MS_U16)pRegTable->s16Coef0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_08_L), (MS_U16)pRegTable->s16Coef0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_09_L), (MS_U16)pRegTable->s16Coef1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0A_L), (MS_U16)pRegTable->s16Coef2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0B_L), (MS_U16)pRegTable->s16Coef3);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0C_L), (MS_U16)pRegTable->s16Coef4);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0D_L), (MS_U16)pRegTable->s16Coef5);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0E_L), (MS_U16)pRegTable->s16Coef6);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_0F_L), (MS_U16)pRegTable->s16Coef7);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_10_L), (MS_U16)pRegTable->s16Coef8);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_11_L), (MS_U32)pRegTable->s32Offset0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_13_L), (MS_U32)pRegTable->s32Offset1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_15_L), (MS_U32)pRegTable->s32Offset2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_17_L), pRegTable->u16Min);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_18_L), pRegTable->u16Max);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7B_19_L), pRegTable->u32Inv);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Degamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Degamma(const MsHdr_Degamma_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_Degamma]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_42_L), 0xC000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_44_L), 0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_46_L), 0x9C400000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_48_L), 0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4A_L), 0x9C400000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4C_L), 0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4E_L), 0x9C400000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_50_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_51_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_52_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_53_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_54_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_55_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_56_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_57_L), (0x7 << 8) | 15);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_60_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_61_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_62_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_63_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_64_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_65_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_66_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_67_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_68_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_69_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6A_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6B_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6C_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6D_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6E_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6F_L), 256);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_42_L), pRegTable->u16Sp);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_44_L), pRegTable->u32EndDiff0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_46_L), pRegTable->u32Limit0);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_48_L), pRegTable->u32EndDiff1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4A_L), pRegTable->u32Limit1);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4C_L), pRegTable->u32EndDiff2);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_4E_L), pRegTable->u32Limit2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_50_L), (pRegTable->au8Eidb[0x1] << 8) | pRegTable->au8Eidb[0x0]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_51_L), (pRegTable->au8Eidb[0x3] << 8) | pRegTable->au8Eidb[0x2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_52_L), (pRegTable->au8Eidb[0x5] << 8) | pRegTable->au8Eidb[0x4]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_53_L), (pRegTable->au8Eidb[0x7] << 8) | pRegTable->au8Eidb[0x6]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_54_L), (pRegTable->au8Eidb[0x9] << 8) | pRegTable->au8Eidb[0x8]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_55_L), (pRegTable->au8Eidb[0xB] << 8) | pRegTable->au8Eidb[0xA]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_56_L), (pRegTable->au8Eidb[0xD] << 8) | pRegTable->au8Eidb[0xC]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_57_L), (pRegTable->au8Eidb[0xF] << 8) | pRegTable->au8Eidb[0xE]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_60_L), pRegTable->au16Eaoff[0x0]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_61_L), pRegTable->au16Eaoff[0x1]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_62_L), pRegTable->au16Eaoff[0x2]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_63_L), pRegTable->au16Eaoff[0x3]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_64_L), pRegTable->au16Eaoff[0x4]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_65_L), pRegTable->au16Eaoff[0x5]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_66_L), pRegTable->au16Eaoff[0x6]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_67_L), pRegTable->au16Eaoff[0x7]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_68_L), pRegTable->au16Eaoff[0x8]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_69_L), pRegTable->au16Eaoff[0x9]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6A_L), pRegTable->au16Eaoff[0xA]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6B_L), pRegTable->au16Eaoff[0xB]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6C_L), pRegTable->au16Eaoff[0xC]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6D_L), pRegTable->au16Eaoff[0xD]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6E_L), pRegTable->au16Eaoff[0xE]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_6F_L), pRegTable->au16Eaoff[0xF]);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of CSC.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_CSC(const MsHdr_CSC_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_CSC]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), 0x2, 0x3, 2);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), 0x0, 0x1, 2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_20_L), 0x4000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_21_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_22_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_23_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_24_L), 0x4000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_25_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_26_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_27_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_28_L), 0x4000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_40_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_41_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_42_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_43_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_44_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_45_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_46_L), 0xFFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_47_L), 0xFFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_48_L), 0xFFFF);
    }
    else
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), pRegTable->u8Shift, 0x3, 2);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), pRegTable->u8Round, 0x1, 2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_20_L), pRegTable->s16Coef0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_21_L), pRegTable->s16Coef1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_22_L), pRegTable->s16Coef2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_23_L), pRegTable->s16Coef3);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_24_L), pRegTable->s16Coef4);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_25_L), pRegTable->s16Coef5);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_26_L), pRegTable->s16Coef6);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_27_L), pRegTable->s16Coef7);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_28_L), pRegTable->s16Coef8);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_40_L), pRegTable->u16Offset0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_41_L), pRegTable->u16Offset1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_42_L), pRegTable->u16Offset2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_43_L), pRegTable->u16Min0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_44_L), pRegTable->u16Min1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_45_L), pRegTable->u16Min2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_46_L), pRegTable->u16Max0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_47_L), pRegTable->u16Max1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_48_L), pRegTable->u16Max2);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Gamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Gamma(const MsHdr_Gamma_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_Gamma]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_02_L), 0xC0000000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_04_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_06_L), 0xFFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_08_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0A_L), 0xFFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0C_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0E_L), 0xFFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_10_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_11_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_12_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_13_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_14_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_15_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_16_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_17_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_18_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_19_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1A_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1B_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1C_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1D_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1E_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1F_L), (0x7 << 8) | 15);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_20_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_21_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_22_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_23_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_24_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_25_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_26_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_27_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_28_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_29_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2A_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2B_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2C_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2D_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2E_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2F_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_30_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_31_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_32_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_33_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_34_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_35_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_36_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_37_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_38_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_39_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3A_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3B_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3C_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3D_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3E_L), 0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3F_L), 256);
    }
    else
    {
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7C_02_L), pRegTable->u32Sp);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_04_L), pRegTable->u16EndDiff0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_06_L), pRegTable->u16Limit0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_08_L), pRegTable->u16EndDiff1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0A_L), pRegTable->u16Limit1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0C_L), pRegTable->u16EndDiff2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_0E_L), pRegTable->u16Limit2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_10_L), (pRegTable->au8Eidb[0x01] << 8) | pRegTable->au8Eidb[0x00]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_11_L), (pRegTable->au8Eidb[0x03] << 8) | pRegTable->au8Eidb[0x02]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_12_L), (pRegTable->au8Eidb[0x05] << 8) | pRegTable->au8Eidb[0x04]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_13_L), (pRegTable->au8Eidb[0x07] << 8) | pRegTable->au8Eidb[0x06]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_14_L), (pRegTable->au8Eidb[0x09] << 8) | pRegTable->au8Eidb[0x08]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_15_L), (pRegTable->au8Eidb[0x0B] << 8) | pRegTable->au8Eidb[0x0A]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_16_L), (pRegTable->au8Eidb[0x0D] << 8) | pRegTable->au8Eidb[0x0C]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_17_L), (pRegTable->au8Eidb[0x0F] << 8) | pRegTable->au8Eidb[0x0E]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_18_L), (pRegTable->au8Eidb[0x11] << 8) | pRegTable->au8Eidb[0x10]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_19_L), (pRegTable->au8Eidb[0x13] << 8) | pRegTable->au8Eidb[0x12]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1A_L), (pRegTable->au8Eidb[0x15] << 8) | pRegTable->au8Eidb[0x14]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1B_L), (pRegTable->au8Eidb[0x17] << 8) |  pRegTable->au8Eidb[0x16]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1C_L), (pRegTable->au8Eidb[0x19] << 8) | pRegTable->au8Eidb[0x18]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1D_L), (pRegTable->au8Eidb[0x1B] << 8) | pRegTable->au8Eidb[0x1A]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1E_L), (pRegTable->au8Eidb[0x1D] << 8) | pRegTable->au8Eidb[0x1C]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_1F_L), (pRegTable->au8Eidb[0x1F] << 8) | pRegTable->au8Eidb[0x1E]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_20_L), pRegTable->au16Eaoff[0x00]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_21_L), pRegTable->au16Eaoff[0x01]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_22_L), pRegTable->au16Eaoff[0x02]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_23_L), pRegTable->au16Eaoff[0x03]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_24_L), pRegTable->au16Eaoff[0x04]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_25_L), pRegTable->au16Eaoff[0x05]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_26_L), pRegTable->au16Eaoff[0x06]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_27_L), pRegTable->au16Eaoff[0x07]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_28_L), pRegTable->au16Eaoff[0x08]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_29_L), pRegTable->au16Eaoff[0x09]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2A_L), pRegTable->au16Eaoff[0x0A]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2B_L), pRegTable->au16Eaoff[0x0B]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2C_L), pRegTable->au16Eaoff[0x0C]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2D_L), pRegTable->au16Eaoff[0x0D]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2E_L), pRegTable->au16Eaoff[0x0E]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_2F_L), pRegTable->au16Eaoff[0x0F]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_30_L), pRegTable->au16Eaoff[0x10]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_31_L), pRegTable->au16Eaoff[0x11]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_32_L), pRegTable->au16Eaoff[0x12]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_33_L), pRegTable->au16Eaoff[0x13]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_34_L), pRegTable->au16Eaoff[0x14]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_35_L), pRegTable->au16Eaoff[0x15]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_36_L), pRegTable->au16Eaoff[0x16]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_37_L), pRegTable->au16Eaoff[0x17]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_38_L), pRegTable->au16Eaoff[0x18]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_39_L), pRegTable->au16Eaoff[0x19]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3A_L), pRegTable->au16Eaoff[0x1A]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3B_L), pRegTable->au16Eaoff[0x1B]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3C_L), pRegTable->au16Eaoff[0x1C]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3D_L), pRegTable->au16Eaoff[0x1D]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3E_L), pRegTable->au16Eaoff[0x1E]);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7C_3F_L), pRegTable->au16Eaoff[0x1F]);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of R2Y.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_R2Y(const MsHdr_R2Y_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_R2Y]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), 0x1, 0x3, 4);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), 0x0, 0x1, 3);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_29_L), 0x1000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2A_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2B_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2C_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2D_L), 0x1000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2E_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2F_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_30_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_31_L), 0x1000);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_32_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_33_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_34_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_35_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_36_L), 0x8001);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_37_L), 0x8001);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_38_L), 0x7FFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_39_L), 0x7FFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_3A_L), 0x7FFF);
    }
    else
    {
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_H), pRegTable->u8Shift, 0x3, 4);
        DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK7B_07_L), pRegTable->u8Round, 0x1, 3);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_29_L), pRegTable->s16Coef0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2A_L), pRegTable->s16Coef1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2B_L), pRegTable->s16Coef2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2C_L), pRegTable->s16Coef3);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2D_L), pRegTable->s16Coef4);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2E_L), pRegTable->s16Coef5);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_2F_L), pRegTable->s16Coef6);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_30_L), pRegTable->s16Coef7);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_31_L), pRegTable->s16Coef8);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_32_L), pRegTable->s16Offset0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_33_L), pRegTable->s16Offset1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_34_L), pRegTable->s16Offset2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_35_L), pRegTable->s16Min0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_36_L), pRegTable->s16Min1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_37_L), pRegTable->s16Min2);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_38_L), pRegTable->s16Max0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_39_L), pRegTable->s16Max1);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_3A_L), pRegTable->s16Max2);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Color Adjustment.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_CA(const MsHdr_CA_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_CA]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_3F_L), 0x0);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7B_3F_L), pRegTable->u16ChromaWeight);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Tone Mapping.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_TM(const MsHdr_TM_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_TM]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_39_L), (0x10 << 8) | 0x10);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3A_L), 0x10);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_39_L), (pRegTable->u8OutGain << 8) | pRegTable->u8InGain);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3A_L), pRegTable->u8UserGain);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Detail Restoration.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_DR(const MsHdr_DR_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_DR]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3B_L), 0x0);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3C_L), 0x0);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3B_L), pRegTable->u16MsWeightEdge);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_3C_L), pRegTable->s16MsWeight);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of Color Correction.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_CC(const MsHdr_CC_RegTable_t* pRegTable)
{
    DoVi_Printf("//[MsHdr_Update_CC]\n");
    if (FALSE == pRegTable->bEnable)
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_40_L), 0x800);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_41_L), 0xFFF);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_42_L), 0x1000);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_43_L), 0x8000); // = 1.0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_45_L), 0x8000); // = 1.0
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_48_L), 0x7FFF);
        //DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_4A_L), 0x0);
        //DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_4B_L), 0x0);
    }
    else
    {
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_40_L), pRegTable->u16HuntGain);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_41_L), pRegTable->u16HuntOffset);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_42_L), pRegTable->u16SatGain);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_43_L), pRegTable->u16HighTh);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_45_L), pRegTable->u16LowTh);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_48_L), pRegTable->u16ZeroTh);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_4A_L), ((MS_U16)pRegTable->bClampEn << 15));
        //DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_4A_L), pRegTable->u16CbOffset | ((MS_U16)pRegTable->bClampEn << 15));
        //DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_4B_L), pRegTable->u16CrOffset);
    }
}

//-------------------------------------------------------------------------------------------------
/// Set registers of 3DLUT.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// Pre-set auto-download.
/// note:
///     Need to be paired with /MsHdr_Update_Autodownload. Called before writing data to DRAM.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Prepare_Autodownload(MsHdr_RegTable_t* pRegTable)
{
#if 0 // config auto download by SN.
    //MHal_XC_ConfigAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, ENABLE, E_KDRV_XC_AUTODOWNLOAD_TRIGGER_MODE, pConfig->phyLutBaseAddr, 0xFFFFF, MS_U32 u32MiuNo);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Fire auto-download.
/// note:
///     Need to be paired with /MsHdr_Prepare_Autodownload. Called after writing data to DRAM.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Autodownload(const MsHdr_RegTable_t* pRegTable)
{
    MHal_XC_FireAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR);
}

//-------------------------------------------------------------------------------------------------
/// Change the endian of a 16-bit data.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MS_U16 MsHdr_ChangeEndianness_2Bytes(MS_U16 data_in)
{
    MS_U16 data_out = ((data_in & 0xFF00) >> 8) | ((data_in & 0xFF) << 8);
    return data_out;
}

//-------------------------------------------------------------------------------------------------
/// Change the endian of a 32-bit data.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MS_U32 MsHdr_ChangeEndianness_4Bytes(MS_U32 data_in)
{
    MS_U32 data_out = ((data_in & 0xFF000000) >> 24) | ((data_in & 0x00FF0000) >> 8) | ((data_in & 0x0000FF00) << 8) | ((data_in & 0x000000FF) << 24);
    return data_out;
}

//-------------------------------------------------------------------------------------------------
/// Extract metadata from input array.
/// @param  An array /pu8MDS
/// @return the number of read bytes if success, or a negative number if fail.
//-------------------------------------------------------------------------------------------------
int DoVi_DmSetDolbyMetaData(MS_U8* pu8Data, MS_U32 u32Size)
{
    // parse binary metadata into struct _stSrcDmMetadata

    int dm_md_size;
    if (pu8Data == NULL)
    {
        memset(&_stSrcDmMetadata, 0, sizeof(_stSrcDmMetadata)); //_stSrcDmMetadata
        dm_md_size = 0;
    }
    //else
    {
        dm_md_size = read_md(&_stSrcDmMetadata, pu8Data);
    }

    //if (DOVI_DM_INPUT_TYPE_HDMI == _pstDmConfig->u8InputType)
    //{
    //reg_dolby_hami_pac_md
    //DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK79_06_H), 0x1, 0x3, 0);
    //_stSrcDmMetadata.base.signal_bit_depth..  = 12;
    //}
    /*
            if (DOVI_DM_INPUT_TYPE_OTT == _pstDmConfig->u8InputType)
        {
        _stSrcDmMetadata.base.signal_bit_depth = 14;
        }
        else if (DOVI_DM_INPUT_TYPE_HDMI == _pstDmConfig->u8InputType)
        {
            //reg_dolby_hami_pac_md
            DoVi_WriteByteMask(DoVi_Addr(REG_SC_BK79_06_H), 0x1, 0x3, 0);
            _stSrcDmMetadata.base.signal_bit_depth = 12;
        }
        else
        {
            _stSrcDmMetadata.base.signal_bit_depth = 14;
        }
    */

    if( (MS_U32)dm_md_size > u32Size )
        return -dm_md_size;
    else
        return 0;
}

//-------------------------------------------------------------------------------------------------
/// Calculate Registers and LUT contents during frame DE.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmFrameDeCalculate()
{
    const h_cp_context_t   pDmContext  = _stDmContext;

    MS_U16 u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7C_L));

    if(1 == DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_08_L)))
    {
        _u8ViewModeId = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK30_09_L));
        DoVi_Printf("view mode = %d\n",_u8ViewModeId);
    }

    // parsing metadata into HW setting
    const int cfgStatus = commit_dm_reg(_stDmContext,&_stSrcDmMetadata,(_isOpenHdr) ? FORMAT_HDR10 : FORMAT_DOVI,_pq_config, /*&_src_param*/NULL,&_ui_menu_params, &_backlight_return_val,_u8ViewModeId);

    const DmKsFxp_t      *pDmKs       = pDmContext->h_dm->hDmKs;


    DoVi_Printf("backlight_return_val = 0x%x\n",_backlight_return_val);

    //DOLBY HDR10 case
    //const int cfgStatus = commit_dm_reg(&_stDmContext, /*&_stSrcDmMetadata*/NULL);

    if (u16RegEnable & 0x1) // BK79 h7c[0] for B01 big bypass
    {

        //msleep(100);
        _pstDmRegTable->stY2R.bEnable     = (u16RegEnable >>  1) & 0x1; // h7c[ 1]
        _pstDmRegTable->stDegamma.bEnable = (u16RegEnable >>  2) & 0x1; // h7c[ 2]
        _pstDmRegTable->stCSC.bEnable     = (u16RegEnable >>  3) & 0x1; // h7c[ 3]
        _pstDmRegTable->stGamma.bEnable   = (u16RegEnable >>  4) & 0x1; // h7c[ 4]
        _pstDmRegTable->stR2Y.bEnable     = (u16RegEnable >>  5) & 0x1; // h7c[ 5]
        _pstDmRegTable->stCA.bEnable      = (u16RegEnable >>  6) & 0x1; // h7c[ 6]
        _pstDmRegTable->stTM.bEnable      = (u16RegEnable >>  8) & 0x1; // h7c[ 8]
        _pstDmRegTable->stDR.bEnable      = (u16RegEnable >>  9) & 0x1; // h7c[ 9]
        _pstDmRegTable->stCC.bEnable      = (u16RegEnable >> 10) & 0x1; // h7c[10]
        _pstDmRegTable->st3D.bEnable      = (u16RegEnable >> 11) & 0x1; // h7c[11]
    }
    else
    {

        //msleep(100);
        _pstDmRegTable->stY2R.bEnable     = FALSE; // h7c[ 1]
        _pstDmRegTable->stDegamma.bEnable = FALSE; // h7c[ 2]
        _pstDmRegTable->stCSC.bEnable     = FALSE; // h7c[ 3]
        _pstDmRegTable->stGamma.bEnable   = FALSE; // h7c[ 4]
        _pstDmRegTable->stR2Y.bEnable     = FALSE; // h7c[ 5]
        _pstDmRegTable->stCA.bEnable      = FALSE; // h7c[ 6]
        _pstDmRegTable->stTM.bEnable      = FALSE; // h7c[ 8]
        _pstDmRegTable->stDR.bEnable      = FALSE; // h7c[ 9]
        _pstDmRegTable->stCC.bEnable      = FALSE; // h7c[10]
        _pstDmRegTable->st3D.bEnable      = FALSE; // h7c[11]
    }

    //msleep(100);
    MS_U16 u16RegUpdate = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7D_L));

    //msleep(100);
    if ( pDmContext->h_mds_ext->scene_refresh_flag| ((u16RegUpdate >> 15) & 0x1))
    {

        //msleep(100);
        _pstDmRegTable->stY2R.bUpdate     = (u16RegUpdate >>  1) & 0x1; // h7d[ 1]
        _pstDmRegTable->stDegamma.bUpdate = (u16RegUpdate >>  2) & 0x1; // h7d[ 2]
        _pstDmRegTable->stCSC.bUpdate     = (u16RegUpdate >>  3) & 0x1; // h7d[ 3]
        _pstDmRegTable->stGamma.bUpdate   = (u16RegUpdate >>  4) & 0x1; // h7d[ 4]
        _pstDmRegTable->stR2Y.bUpdate     = (u16RegUpdate >>  5) & 0x1; // h7d[ 5]
        _pstDmRegTable->stCA.bUpdate      = (u16RegUpdate >>  6) & 0x1; // h7d[ 6]
        _pstDmRegTable->stTM.bUpdate      = (u16RegUpdate >>  8) & 0x1; // h7d[ 8]
        _pstDmRegTable->stDR.bUpdate      = (u16RegUpdate >>  9) & 0x1; // h7d[ 9]
        _pstDmRegTable->stCC.bUpdate      = (u16RegUpdate >> 10) & 0x1; // h7d[10]
        _pstDmRegTable->st3D.bUpdate      = (u16RegUpdate >> 11) & 0x1; // h7d[11]

    }
    else
    {

        //msleep(100);
        _pstDmRegTable->stY2R.bUpdate = FALSE;
        _pstDmRegTable->stDegamma.bUpdate = FALSE;
        _pstDmRegTable->stCSC.bUpdate = FALSE;
        _pstDmRegTable->stGamma.bUpdate = FALSE;
        _pstDmRegTable->stR2Y.bUpdate = FALSE;
        _pstDmRegTable->stCA.bUpdate = FALSE;
        _pstDmRegTable->stTM.bUpdate = FALSE;
        _pstDmRegTable->stDR.bUpdate = FALSE;
        _pstDmRegTable->stCC.bUpdate = FALSE;
        _pstDmRegTable->st3D.bUpdate = FALSE;

    }
#if 0
    MsHdr_Prepare_Autodownload(_pstDmRegTable, pConfig, pMdsExt);
#endif

    if (_pstDmRegTable->stY2R.bUpdate)
    {
        DoVi_Prepare_B0102(&_pstDmRegTable->stY2R, pDmKs, pDmContext);
    }

    if (_pstDmRegTable->stDegamma.bUpdate)
        DoVi_Prepare_B0103(&_pstDmRegTable->stDegamma, pDmKs);
    if (_pstDmRegTable->stCSC.bUpdate)
        DoVi_Prepare_B0104(&_pstDmRegTable->stCSC, pDmKs);
    if (_pstDmRegTable->stGamma.bUpdate)
        DoVi_Prepare_B0105(&_pstDmRegTable->stGamma, pDmKs);
    if (_pstDmRegTable->stR2Y.bUpdate)
        DoVi_Prepare_B0106(&_pstDmRegTable->stR2Y, pDmKs);
    if (_pstDmRegTable->stCA.bUpdate)
        DoVi_Prepare_B0107(&_pstDmRegTable->stCA, pDmKs);
    if (_pstDmRegTable->stTM.bUpdate)
        DoVi_Prepare_B02(&_pstDmRegTable->stTM, pDmKs);
    if (_pstDmRegTable->stDR.bUpdate)
        DoVi_Prepare_B03(&_pstDmRegTable->stDR, pDmKs);
    if (_pstDmRegTable->stCC.bUpdate)
        DoVi_Prepare_B04(&_pstDmRegTable->stCC, pDmKs);
    if (_pstDmRegTable->st3D.bUpdate)
        DoVi_Prepare_B05(&_pstDmRegTable->st3D,pDmKs);

}

//-------------------------------------------------------------------------------------------------
/// Update Registers and LUT contents at blanking interval.
/// note:
///     If metadata has any problem, do not call this method to update. (= Keep previous setting.)
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmBlankingUpdate(void)
{
    const MsHdr_RegTable_t* pRegTable = _pstDmRegTable;

    DoVi_Printf("Dolby 1.4.3 is running...\n");
    if (pRegTable->stY2R.bUpdate)
        MsHdr_Update_Y2R(&pRegTable->stY2R);
    if (pRegTable->stDegamma.bUpdate)
        MsHdr_Update_Degamma(&pRegTable->stDegamma);
    if (pRegTable->stCSC.bUpdate)
        MsHdr_Update_CSC(&pRegTable->stCSC);
    if (pRegTable->stGamma.bUpdate)
        MsHdr_Update_Gamma(&pRegTable->stGamma);
    if (pRegTable->stR2Y.bUpdate)
        MsHdr_Update_R2Y(&pRegTable->stR2Y);
    if (pRegTable->stCA.bUpdate)
        MsHdr_Update_CA(&pRegTable->stCA);
    if (pRegTable->stTM.bUpdate)
        MsHdr_Update_TM(&pRegTable->stTM);
    if (pRegTable->stDR.bUpdate)
        MsHdr_Update_DR(&pRegTable->stDR);
    if (pRegTable->stCC.bUpdate)
        MsHdr_Update_CC(&pRegTable->stCC);
    if (pRegTable->st3D.bUpdate)
        MsHdr_Update_3D(&pRegTable->st3D);

#ifdef MENULOAD_ENABLE
    if (KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE) != TRUE)
    {
        printk("MLoad fire fail.\n");
    }
#endif

    if (CFD_IS_HDMI(_stCfdInit[0].u8InputSource))
    {
        MsHdr_Update_Autodownload(pRegTable);
    }
}

void DoVi_setHdrMetadata(MS_U8* pu8Data)
{
    static src_param_t src_param;
    static hdr10_param_t hdr10_param;
    if(_isOpenHdr)
    {
#if 1
        hdr10_param.max_display_mastering_luminance = ( pu8Data[26] | (pu8Data[27]<<8) | (pu8Data[28]<<16) |(pu8Data[29]<<24));
        if(1000000 > hdr10_param.max_display_mastering_luminance)// if max lum is less than 100 nit, use 100 nits
        {
            hdr10_param.max_display_mastering_luminance = 1000000;
        }
        if(100000000 < hdr10_param.max_display_mastering_luminance)//if max lum is less than 10000 nit, use 10000 nits
        {
            hdr10_param.max_display_mastering_luminance = 100000000;
        }
        hdr10_param.min_display_mastering_luminance =  ( pu8Data[30] | (pu8Data[31]<<8) | (pu8Data[32]<<16) |(pu8Data[33]<<24));
        hdr10_param.Rx = 0;//(pu8Data[10] | (pu8Data[11]<<8) );
        hdr10_param.Ry = 0;//(pu8Data[12] | (pu8Data[13]<<8) );
        hdr10_param.Gx = 0;//(pu8Data[14] | (pu8Data[15]<<8) );
        hdr10_param.Gy = 0;//(pu8Data[16] | (pu8Data[17]<<8) );
        hdr10_param.Bx = 0;//(pu8Data[18] | (pu8Data[19]<<8) );
        hdr10_param.By = 0;//(pu8Data[20] | (pu8Data[21]<<8) );
        hdr10_param.Wx = 0;//(pu8Data[22] | (pu8Data[23]<<8) );
        hdr10_param.Wy = 0;//(pu8Data[24] | (pu8Data[25]<<8) );
        hdr10_param.max_content_light_level = 0;//(pu8Data[31] | (pu8Data[32]<<8) );
        hdr10_param.max_pic_average_light_level = 0;//(pu8Data[37] | (pu8Data[38]<<8) );
        src_param.src_bit_depth      = pu8Data[39];
        src_param.src_chroma_format  = 0;
        src_param.src_yuv_range      = pu8Data[4];
        src_param.hdr10_param        = hdr10_param;
        DoVi_Printf("max_display_mastering_luminance = %d nits \n", (pu8Data[26] | (pu8Data[27]<<8) | (pu8Data[28]<<16) |(pu8Data[29]<<24))/10000);
        DoVi_Printf("min_display_mastering_luminance = %d nits\n", ( pu8Data[30] | (pu8Data[31]<<8) | (pu8Data[32]<<16) |(pu8Data[33]<<24)));
        DoVi_Printf("range =  %d\n",  pu8Data[4]);
        DoVi_Printf("src_bit_depth =  %d\n",  pu8Data[39]);
#else
        hdr10_param.max_display_mastering_luminance = 10000000;//( pu8Data[26] | (pu8Data[27]<<8) | (pu8Data[28]<<16) |(pu8Data[29]<<24));
        hdr10_param.min_display_mastering_luminance =50; /**< @brief */
        hdr10_param.Rx = 0;
        hdr10_param.Ry =0;
        hdr10_param.Gx = 0;
        hdr10_param.Gy =0;
        hdr10_param.Bx = 0;
        hdr10_param.By = 0;
        hdr10_param.Wx =0;
        hdr10_param.Wy = 0;
        hdr10_param.max_content_light_level =5000;
        hdr10_param.max_pic_average_light_level = 100;
        src_param.src_bit_depth      = 10;
        src_param.src_chroma_format  = CChrm420;
        src_param.src_yuv_range      = CRngFull;
        src_param.hdr10_param        = hdr10_param;
        DoVi_Printf("max_display_mastering_luminance = %d\n", (pu8Data[26] | (pu8Data[27]<<8) | (pu8Data[28]<<16) |(pu8Data[29]<<24)));
        DoVi_Printf("range =  = %d\n",  pu8Data[4]);
        DoVi_Printf("src_bit_depth =  = %d\n",  pu8Data[39]);
#endif
        setup_src_config_external(_stDmContext, FORMAT_HDR10, &src_param);
    }
    else
    {

        src_param.src_bit_depth      =10;
        src_param.src_chroma_format  = 0;
        src_param.src_yuv_range      = CRngNarrow;
        src_param.hdr10_param        = hdr10_param;
        setup_src_config_external(_stDmContext, FORMAT_DOVI, &src_param);
    }
}

void DoVi_UpdateBackLight(MS_U16  u16BackLightUiVal)
{
    _ui_menu_params.u16BackLightUIVal  = u16BackLightUiVal;
}

void DoVi_UpdateBrightness(MS_U16  u16BrightnessUiVal)
{
    _ui_menu_params.u16BrightnessUIVal = u16BrightnessUiVal;
}

void DoVi_UpdateContrast(MS_U16  u16ContrastUiVal)
{
    _ui_menu_params.u16ContrastUIVal   = u16ContrastUiVal;
}

void DoVi_UpdateViewMode(MS_U8 u8ModeId)
{
    _u8ViewModeId = u8ModeId;
}

MS_U16 DoVi_Backlight_Value(void)
{
    return _backlight_return_val;
}

MS_BOOL DoVi_IsSupportGD()
{
    if (bIsInitalized)
    {
        return _stDmContext->cur_pq_config->target_display_config.gdConfig.gdEnable;
    }
    else
    {
        return FALSE;
    }
}

void DoVi_Prepare_DS(MS_U8 u8Version)
{
    _u8Version = u8Version;
    memset(&_stXC_DS_CmdCnt, 0, sizeof(K_XC_DS_CMDCNT));
}

void DoVi_GetCmdCnt(K_XC_DS_CMDCNT *pstXCDSCmdCnt)
{
    *pstXCDSCmdCnt = _stXC_DS_CmdCnt;
}
