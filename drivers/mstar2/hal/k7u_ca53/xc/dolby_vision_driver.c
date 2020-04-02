#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mhal_xc.h"
#include "dolby_vision_driver.h"
#include "dolby_vision_lut.h"
#include "hwreg_xc.h"

extern ST_KDRV_XC_CFD_INIT _stCfdInit[2];
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
    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        MHal_XC_WriteByteMask((0x130000 | (ADDR)), ((VAL) << (BIT)) & 0xFF, ((MASK) << (BIT)) & 0xFF); \
    else \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), (((VAL) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), (((MASK) << (BIT)) & 0xFF) << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_WriteByte(ADDR, VAL) \
    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        MHal_XC_WriteByte((0x130000 | (ADDR)), ((VAL) & 0xFF)); \
    else \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, (ADDR) - (ADDR % 2), ((VAL) & 0xFF) << (8 * (ADDR % 2)), 0xFF << (8 * (ADDR % 2)), K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_Write2Bytes(ADDR, VAL) \
    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
        MHal_XC_W2BYTE((0x130000 | (ADDR)), ((VAL) & 0xFFFF));\
    else \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt);
#define DoVi_Write4Bytes(ADDR, VAL) \
    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION))) \
    { \
        MHal_XC_W2BYTE((0x130000 | (ADDR)), ((VAL) & 0xFFFF)); \
        MHal_XC_W2BYTE(((0x130000 | (ADDR)) + 2), (((VAL)>>16) & 0xFFFF));\
    } \
    else \
    { \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ADDR, ((VAL) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt); \
        KHal_XC_StoreSWDSCommand(E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, ((ADDR) + 2), (((VAL)>>16) & 0xFFFF), 0xFFFF, K_DS_IP, K_DS_XC, &_stXC_DS_CmdCnt); \
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

static MS_BOOL _bNeedDownload3DLut = TRUE;

// Composer
void DoVi_CompDumpConfig(DoVi_Comp_ExtConfig_t* pConfExt)
{
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
                //DoVi_Printf("//%llx //poly_coef_int[%d][%d][%d]\n",((MS_U32)pConfExt->poly_coef_int[cmp][i][j]), cmp, i, j);
                //DoVi_Printf("//%llx //poly_coef[%d][%d][%d]\n", ((MS_U32)pConfExt->poly_coef[cmp][i][j]),cmp, i, j);
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
            //DoVi_Printf("//%llx //MMR_coef_int[%d][%d]\n",(MS_U64)pConfExt->MMR_coef_int[cmp][i], cmp, i);
            //DoVi_Printf("//%llx //MMR_coef[%d][%d]\n", pConfExt->MMR_coef[cmp][i], cmp, i);
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
            //DoVi_Printf("//%x //NLQ_coeff_int[%d][%d]\n", (MS_U32)pConfExt->NLQ_coeff_int[cmp][i], cmp, i);
            //DoVi_Printf("//%x //NLQ_coeff[%d][%d]\n",  pConfExt->NLQ_coeff[cmp][i], cmp, i);
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
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MsHdr_Comp_Regtable_t* MsHdr_CompAllocRegTable(void)
{
    MsHdr_Comp_Regtable_t* pRegTable;
    pRegTable = (MsHdr_Comp_Regtable_t*)MsHdr_Malloc(sizeof(MsHdr_Comp_Regtable_t));
    memset(pRegTable, 0, sizeof(MsHdr_Comp_Regtable_t));
    return pRegTable;
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_CompFreeRegTable(MsHdr_Comp_Regtable_t* pRegTable)
{
    MsHdr_Free(pRegTable);
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
DoVi_Comp_ExtConfig_t* DoVi_CompAllocConfig(void)
{
    DoVi_Comp_ExtConfig_t* pConfig;
    pConfig = (DoVi_Comp_ExtConfig_t*)MsHdr_Malloc(sizeof(DoVi_Comp_ExtConfig_t));
    memset(pConfig, 0, sizeof(DoVi_Comp_ExtConfig_t));
    return pConfig;
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompFreeConfig(DoVi_Comp_ExtConfig_t* pConfig)
{
    MsHdr_Free(pConfig);
}

//-------------------------------------------------------------------------------------------------
/// Dolby Vision default setting and initialization.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompSetDefaultConfig(DoVi_Comp_ExtConfig_t* pConfig)
{
    memset(pConfig, 0, sizeof(DoVi_Comp_ExtConfig_t));
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_CompCheckMetadata(DoVi_Comp_ExtConfig_t* pConfExt)
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
int DoVi_CompReadMetadata(DoVi_Comp_ExtConfig_t* pConfig, const MS_U8* pu8MDS, MS_U16 u16Len)
{
    if (sizeof(DoVi_Comp_ExtConfig_t) > u16Len)
        return -(int)u16Len;
    memcpy(pConfig, pu8MDS, sizeof(DoVi_Comp_ExtConfig_t));
    if (DoVi_CompCheckMetadata(pConfig) < 0)
        return -1;

    return sizeof(DoVi_Comp_ExtConfig_t);
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_Composer(MsHdr_Comp_Regtable_t* pRegTable, const DoVi_Comp_ExtConfig_t* pConfExt)
{
    MS_U16 u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7C_L));
    MS_U16 u16RegUpdate = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7D_L));
    pRegTable->bUpdate = (u16RegUpdate >> 15) & 0x1;
    pRegTable->u8ModeBL = (u16RegEnable >> 12) & 0x3; // bit 12,13
    pRegTable->u8ModeEL = (u16RegEnable >> 14) & 0x3; // bit 14,15
    memcpy(&(pRegTable->stConfig), pConfExt, sizeof(DoVi_Comp_ExtConfig_t));
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Update_Composer(MsHdr_Comp_Regtable_t* pRegTable)
{
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



//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompFrameDeCalculate(MsHdr_Comp_Regtable_t* pRegTable, const DoVi_Comp_ExtConfig_t* pConfExt)
{
    DoVi_Prepare_Composer(pRegTable, pConfExt);
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_CompBlankingUpdate(const MsHdr_Comp_Regtable_t* pRegTable)
{
    MsHdr_Update_Composer(pRegTable);
}

// DM

void DoVi_DmDumpMetadata(const DoVi_MdsExt_t* pMdsExt)
{
    int i = 0;
	int j = 0;

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

	for (j = 0; j <= pMdsExt->TrimNum; j++)
	{
    for (i = 0; i < TrimTypeNum; i++)
		{
			DoVi_Printf("//%x //Trima[%x][%x]\n", pMdsExt->Trima[i][j], i,j);
		}
	}

    DoVi_Printf("//%x //min_PQ\n", pMdsExt->min_PQ);
    DoVi_Printf("//%x //max_PQ\n", pMdsExt->max_PQ);
    DoVi_Printf("//%x //mid_PQ\n", pMdsExt->mid_PQ);

    DoVi_Printf("//%x // pMdsExt->trimSlope_final      \n",  pMdsExt->trimSlope_final     );
    DoVi_Printf("//%x // pMdsExt->trimOffset_final     \n",  pMdsExt->trimOffset_final    );
    DoVi_Printf("//%x // pMdsExt->trimPower_final      \n",  pMdsExt->trimPower_final     );
    DoVi_Printf("//%x // pMdsExt->chromaWeight_final   \n",  pMdsExt->chromaWeight_final  );
    DoVi_Printf("//%x // pMdsExt->saturationGain_final \n",  pMdsExt->saturationGain_final);
    DoVi_Printf("//%x // pMdsExt->msWeight_final       \n",  pMdsExt->msWeight_final      );

    DoVi_Printf("\n");
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MsHdr_RegTable_t* MsHdr_DmAllocRegTable(void)
{
    MsHdr_RegTable_t* pRegTable;
    pRegTable = (MsHdr_RegTable_t*)MsHdr_Malloc(sizeof(MsHdr_RegTable_t));
    memset(pRegTable, 0, sizeof(MsHdr_RegTable_t));
    pRegTable->stDegamma.pu32Table = (MS_U32*)MsHdr_Malloc(sizeof(MS_U32) * DOVI_DEGAMMA_SIZE);
    memset(pRegTable->stDegamma.pu32Table, 0, sizeof(MS_U32) * DOVI_DEGAMMA_SIZE);
    pRegTable->stGamma.pu16Table = (MS_U16*)MsHdr_Malloc(sizeof(MS_U16) * DOVI_GAMMA_SIZE);
    memset(pRegTable->stGamma.pu16Table, 0, sizeof(MS_U16) * DOVI_GAMMA_SIZE);
    pRegTable->stTM.pu16Table = (MS_U16*)MsHdr_Malloc(sizeof(MS_U16) * DOVI_TMO_SIZE);
    memset(pRegTable->stTM.pu16Table, 0, sizeof(MS_U16) * DOVI_TMO_SIZE);
    pRegTable->st3D.pu16Table = (MS_U16*)MsHdr_Malloc(sizeof(MS_U16) * DOVI_3DLUT_SIZE*3);
    memset(pRegTable->st3D.pu16Table, 0, sizeof(MS_U16) * DOVI_3DLUT_SIZE*3);
    return pRegTable;
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_DmFreeRegTable(MsHdr_RegTable_t* pRegTable)
{
    MsHdr_Free(pRegTable->stDegamma.pu32Table);
    MsHdr_Free(pRegTable->stGamma.pu16Table);
    MsHdr_Free(pRegTable->stTM.pu16Table);
    MsHdr_Free(pRegTable->st3D.pu16Table);
    MsHdr_Free(pRegTable);
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
DoVi_Config_t* DoVi_DmAllocConfig(void)
{
    DoVi_Config_t* pConfig;
    pConfig = (DoVi_Config_t*)MsHdr_Malloc(sizeof(DoVi_Config_t));
    memset(pConfig, 0, sizeof(DoVi_Config_t));
    return pConfig;
}

//-------------------------------------------------------------------------------------------------
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmFreeConfig(DoVi_Config_t* pConfig)
{
    MsHdr_Free(pConfig);
}

//-------------------------------------------------------------------------------------------------
/// Calculate content of ordinary gamma-to-linear table. Adapt to Dolby code.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MS_U32 DoVi_Calc_DegammaLut(MS_U16 sA, MS_U16 sB, MS_U16 sGamma, MS_U32 sG, MS_U32 idx)
{
    // 0x0000 - 0x0FF0 : LSB 4b : 256 entries
    // 0x1000 - 0x1F80 : LSB 7b :  32 entries
    // 0x2000 - 0xFF00 : LSB 8b : 224 entries
    MS_U16 y;
    if (idx < 256)
        y = (MS_U16)idx << 4;
    else if (idx < 288)
        y = (((MS_U16)idx-256) << 7) + 0x1000;
    else // if (idx < 512)
        y = (((MS_U16)idx-288) << 8) + 0x2000;

    static const MS_U32 logInterpBitMask = (1<<DEF_DEGAMMAR_INTERP_BITS) - 1;
    static const MS_U32 logRate2logScale2p = DEF_DEGAMMAR_LOG_RATE_SCALE2P +
            DEF_DEGAMMAR_INTERP_BITS - DEF_DEGAMMAR_LOG_SCALE2P;
    static const MS_U32 toPowerLutInScale2p = 16;
    static const MS_U32 powerLutIdxMask =
        (1 << (DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)) - 1;

    //// X+b
    MS_U32 u32 = y + sB;
    MS_BOOL overOne = u32 >= (1<<DEF_DEGAMMAR_IN_SCALE2P);

    //// down to 16 bit
    MS_U16 u16;
    if (overOne)
    {
        u16 = (MS_U16)(u32>>1);
    }
    else
    {
        u16 = (MS_U16)u32;
    }

    //// log:
    // rate*delta, scale to log10lut scale
    u32 = (log10RateLut[u16>>DEF_DEGAMMAR_INTERP_BITS]*(u16 & logInterpBitMask)) >> logRate2logScale2p;
    // log(y+sB): liear interpolation: 'a*x + b', where the a = -Rate, b = log10Lut, x is the decimal part
    u32 = log10Lut[u16>>DEF_DEGAMMAR_INTERP_BITS] - u32;

    //// Sgamma*log(y + sB)
    u32 *= sGamma;

    //// power
    // scale to power lut input
    u32 >>= toPowerLutInScale2p;
    // power
    u16 = (MS_U16)u32;
    if (!(u16 == u32))
    {
        printk("%s %d: assert(u16 == u32)!!!!\n", __FUNCTION__, __LINE__);
        return 0;
    }
    if (!(u16 < (1<<DEF_DEGAMMAR_POWER_IN_BITS)))
    {
        printk("%s %d: assert(u16 < (1<<DEF_DEGAMMAR_POWER_IN_BITS))\n", __FUNCTION__, __LINE__);
        return 0;
    }
    int toPowerOutScale2p;
    if ((u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)) < 10)
    {
        u32 = power10Lut[u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)][u16 & powerLutIdxMask];
        // to output scale:
        toPowerOutScale2p = powerScale2p[u16>>(DEF_DEGAMMAR_POWER_IN_BITS - DEF_DEGAMMAR_POWER_ID_BITS)] - DEF_DEGAMMAR_OUT_SCALE2P;
    }
    else // optimized for hardware. The last 6 LUTs are all zero. Skip.
        return 0;

    //// put /2 back and keep the original scale
    if (overOne)
    {
        u32 = (MS_U32)(((MS_U64)sG*u32) >> DEF_DEGAMMAR_OUT_SCALE2P);
    }

    u32 = ((MS_U64)u32 * sA) >> (2 + toPowerOutScale2p);

    // convert 32bit to mantissa(14b)+exponent(5b)
    MS_U16 i;
    for (i = 0; i < 18; i++)
    {
        if (u32 < (0x1 << (i+14)))
            break;
    }
    return (u32 >> i << 5) | i;
}

//-------------------------------------------------------------------------------------------------
/// Calculate content of ordinary gamma-to-linear table. Adapt to Dolby code.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
MS_U32 DoVi_Bypass_DegammaLut(MS_U32 idx)
{
    MS_U32 u32 = idx << 23; // 256 entries : [0x0, 0x1FF] map to [0x0, 0xFF800000]

    // convert 32bit to mantissa(14b)+exponent(5b)
    MS_U16 i;
    for (i = 0; i < 18; i++)
    {
        if (u32 < (0x1 << (i+14)))
            break;
    }
    return (u32 >> i << 5) | i;
}

//-------------------------------------------------------------------------------------------------
/// Retrieve content of Dolby Vision 3DLUT.
/// note:
///     The number of pre-defined 3DLUTs is defined by /DOVI_NUM_3DLUT_INSTANCES
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_Replace_3DLut(MsHdr_3D_RegTable_t* p3DLutReg, const MS_U8* pu8Array, MS_U32 u32Size)
{
    if (u32Size != DOVI_3DLUT_SIZE * 3 * sizeof(MS_U16))
        return -DOVI_3DLUT_SIZE * 3 * sizeof(MS_U16);

    memcpy(((MS_U8 *)p3DLutReg->pu16Table), pu8Array, u32Size);

    p3DLutReg->u32Size = DOVI_3DLUT_SIZE * 3;
    p3DLutReg->bUpdate = TRUE;
    _bNeedDownload3DLut = TRUE;
    return 0;
}

int DoVi_Trigger3DLutUpdate(void)
{
    _bNeedDownload3DLut = TRUE;
    return 0;
}

//-------------------------------------------------------------------------------------------------
/// Dolby Vision default setting and initialization. From INI file?
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmSetDefaultConfig(DoVi_Config_t* pConfig)
{
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
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Y2R.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0102(MsHdr_Y2R_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt, DoVi_Comp_ExtConfig_t* pComConfig)
{
    DoVi_Printf("//[DoVi_Prepare_B0102]\n");
    pRegTable->u8Shift = 1; // 0:shift 10b, 1:shift 11b, 2:shift 12b, 3:shift 13b
    pRegTable->u8Round = 1;
    pRegTable->s16Coef0 = pMdsExt->YCCtoRGB_coef0;
    pRegTable->s16Coef1 = pMdsExt->YCCtoRGB_coef1;
    pRegTable->s16Coef2 = pMdsExt->YCCtoRGB_coef2;
    pRegTable->s16Coef3 = pMdsExt->YCCtoRGB_coef3;
    pRegTable->s16Coef4 = pMdsExt->YCCtoRGB_coef4;
    pRegTable->s16Coef5 = pMdsExt->YCCtoRGB_coef5;
    pRegTable->s16Coef6 = pMdsExt->YCCtoRGB_coef6;
    pRegTable->s16Coef7 = pMdsExt->YCCtoRGB_coef7;
    pRegTable->s16Coef8 = pMdsExt->YCCtoRGB_coef8;
    if (pMdsExt->signal_full_range_flag == SIG_RANGE_NARROW) // 64~940
    {
        pRegTable->u16Min = (MS_U16)16 << (pConfig->u8InputBits - 8);
        pRegTable->u16Max = (MS_U16)235 << (pConfig->u8InputBits - 8);
    }
    else if (pMdsExt->signal_full_range_flag == SIG_RANGE_SDI) // 4~1019
    {
        pRegTable->u16Min = (MS_U16)4 << (pConfig->u8InputBits - 10);
        pRegTable->u16Max = (MS_U16)1019 << (pConfig->u8InputBits - 10);
        if (DOVI_EOTFVAL_TO_EOTFENUM(pMdsExt->signal_eotf) != DOVI_DM_EOTF_MODE_PQ)
            pRegTable->u16Max += (((MS_U16)1 << (pMdsExt->signal_bit_depth - 10)) - 1) << (pConfig->u8InputBits - pMdsExt->signal_bit_depth);
    }
    else // Use SIG_RANGE_FULL -> 0~1023
    {
        pRegTable->u16Min = (MS_U16)0;
        pRegTable->u16Max = (((MS_U16)1 << pMdsExt->signal_bit_depth) - 1) << (pConfig->u8InputBits - pMdsExt->signal_bit_depth);
    }
    pRegTable->u16Min <<= (16 - pConfig->u8InputBits);
    pRegTable->u16Max <<= (16 - pConfig->u8InputBits);
    pRegTable->u32Inv = (MS_U32)0xFFFFFFFFul / (pRegTable->u16Max - pRegTable->u16Min);

    MS_S64 s64OffsetY = pMdsExt->YCCtoRGB_offset0 << (pConfig->u8InputBits - pMdsExt->signal_bit_depth);
    MS_S64 s64OffsetU = pMdsExt->YCCtoRGB_offset1 << (pConfig->u8InputBits - pMdsExt->signal_bit_depth);
    MS_S64 s64OffsetV = pMdsExt->YCCtoRGB_offset2 << (pConfig->u8InputBits - pMdsExt->signal_bit_depth);
    pRegTable->s32Offset0 = (s64OffsetY * pRegTable->s16Coef0 + s64OffsetU * pRegTable->s16Coef1 + s64OffsetV * pRegTable->s16Coef2) >> 16;
    pRegTable->s32Offset1 = (s64OffsetY * pRegTable->s16Coef3 + s64OffsetU * pRegTable->s16Coef4 + s64OffsetV * pRegTable->s16Coef5) >> 16;
    pRegTable->s32Offset2 = (s64OffsetY * pRegTable->s16Coef6 + s64OffsetU * pRegTable->s16Coef7 + s64OffsetV * pRegTable->s16Coef8) >> 16;

    /**/if (pComConfig->rpu_VDR_bit_depth > 14)
    {
       pRegTable->s32Offset0 = pRegTable->s32Offset0 >> (pComConfig->rpu_VDR_bit_depth-14);
       pRegTable->s32Offset1 = pRegTable->s32Offset1 >> (pComConfig->rpu_VDR_bit_depth-14);
       pRegTable->s32Offset2 = pRegTable->s32Offset2 >> (pComConfig->rpu_VDR_bit_depth-14);
    }
    else
    {
       pRegTable->s32Offset0 = pRegTable->s32Offset0 << (14-pComConfig->rpu_VDR_bit_depth);
       pRegTable->s32Offset1 = pRegTable->s32Offset1 << (14-pComConfig->rpu_VDR_bit_depth);
       pRegTable->s32Offset2 = pRegTable->s32Offset2 << (14-pComConfig->rpu_VDR_bit_depth);
    }/**/

#if 0
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->u8Shift(%d)\n",pRegTable->u8Shift);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->u8Round(%d)\n",pRegTable->u8Round);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef0(%d)\n",pRegTable->s16Coef0);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef1(%d)\n",pRegTable->s16Coef1);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef2(%d)\n",pRegTable->s16Coef2);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef3(%d)\n",pRegTable->s16Coef3);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef4(%d)\n",pRegTable->s16Coef4);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef5(%d)\n",pRegTable->s16Coef5);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef6(%d)\n",pRegTable->s16Coef6);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef7(%d)\n",pRegTable->s16Coef7);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s16Coef8(%d)\n",pRegTable->s16Coef8);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s32Offset0(%d)\n",pRegTable->s32Offset0);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s32Offset1(%d)\n",pRegTable->s32Offset1);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->s32Offset2(%d)\n",pRegTable->s32Offset2);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->u16Min(%d)\n",pRegTable->u16Min);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->u16Max(%d)\n",pRegTable->u16Max);
    DoVi_Printf("[DoVi_Prepare_B0102] pRegTable->u32Inv(%d)\n",pRegTable->u32Inv);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Degamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0103(MsHdr_Degamma_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B0103]\n");
    MS_U32 u32Idx;
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
    const MS_U8 u8EidbPq[16] = {8, 0, 1, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6};
    const MS_U8 u8EidbPq_sim[16] = {0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7};
    const MS_U8 u8EidbGa[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 6, 4, 4, 5, 6};
    const MS_U8 u8EidbGa_sim[16] = {0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6};
    const MS_U16 u16EaoffPq[16] = {0, 2, 4, 8, 16, 32, 48, 64, 96, 128, 160, 192, 224, 256, 320, 384};
    const MS_U16 u16EaoffPq_sim[16] = {0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 8, 16, 32, 64, 128, 256};
    const MS_U16 u16EaoffGa[16] = {0, 0, 0, 0,  0,  0,  0,  0,  0,   0,   0, 128, 256, 288, 320, 384};
    const MS_U16 u16EaoffGa_sim[16] = {0, 0, 0, 0,  0,  0,  0,  0,  0,   2,   4, 8, 16, 32, 64, 128};

    MS_U16 u16Sp = 0xFC00;
    const MS_U8* pu8Eidb = &u8EidbGa[0];
    const MS_U16* pu16Eaoff = &u16EaoffGa[0];
    MS_U16 u16RegEnable = 0;

    MS_U16 u16Gamma = 0;
    MS_U16 u16A = 0;
    MS_U16 u16B = 0;
    MS_U32 u32G = 0;

    pRegTable->u32Size = DOVI_DEGAMMA_SIZE;
    if (pRegTable->bEnable == FALSE)
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu32Table[u32Idx] = DoVi_Bypass_DegammaLut(u32Idx);
    }
    else if (DOVI_EOTFVAL_TO_EOTFENUM(pMdsExt->signal_eotf) == DOVI_DM_EOTF_MODE_PQ)
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu32Table[u32Idx] = g_au32DoViDegammaPqLut[u32Idx];
    }
    else if ((pMdsExt->source_max_PQ >= 3695) && (pMdsExt->source_max_PQ <= 3698)) // 4000 nits, pulsar
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu32Table[u32Idx] = g_au32DoViDegammaPulsarLut[u32Idx];
    }
    else if ((pMdsExt->source_max_PQ >= 2080) && (pMdsExt->source_max_PQ <= 2083)) // 100 nits, sdr
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu32Table[u32Idx] = g_au32DoViDegammaSdrLut[u32Idx];
    }
    else
    {
        u16Gamma = pMdsExt->signal_eotf;
        u16A = pMdsExt->signal_eotf_param0;
        u16B = pMdsExt->signal_eotf_param1;
        u32G = pMdsExt->signal_eotf_param2;
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu32Table[u32Idx] = DoVi_Calc_DegammaLut(u16A, u16B, u16Gamma, u32G, u32Idx);
    }

    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = DOVI_DEGAMMA_SIZE - 1;

        if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
        {
            MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu32Table, pRegTable->u32Size * sizeof(MS_U32), (void*)&stAutoDownloadFormat);
        }
        else
        {
            MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu32Table, pRegTable->u32Size * sizeof(MS_U32), (void*)&stAutoDownloadFormat);
        }
    }
    else // use RIU
    {
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
    }

    if (DOVI_EOTFVAL_TO_EOTFENUM(pMdsExt->signal_eotf) == DOVI_DM_EOTF_MODE_PQ)
    {
        //u16Sp = 0xFFFF;
		u16Sp = 0xFF80;
        //pu8Eidb = &u8EidbPq[0];
        //pu16Eaoff = &u16EaoffPq[0];
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

#if 0
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32Size(%d)\n",pRegTable->u32Size);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u16Sp(%d)\n",pRegTable->u16Sp);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[0](%d)\n",pRegTable->au8Eidb[0]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[1](%d)\n",pRegTable->au8Eidb[1]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[2](%d)\n",pRegTable->au8Eidb[2]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[3](%d)\n",pRegTable->au8Eidb[3]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[4](%d)\n",pRegTable->au8Eidb[4]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[5](%d)\n",pRegTable->au8Eidb[5]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[6](%d)\n",pRegTable->au8Eidb[6]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[7](%d)\n",pRegTable->au8Eidb[7]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[8](%d)\n",pRegTable->au8Eidb[8]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[9](%d)\n",pRegTable->au8Eidb[9]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[10](%d)\n",pRegTable->au8Eidb[10]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[11](%d)\n",pRegTable->au8Eidb[11]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[12](%d)\n",pRegTable->au8Eidb[12]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[13](%d)\n",pRegTable->au8Eidb[13]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[14](%d)\n",pRegTable->au8Eidb[14]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au8Eidb[15](%d)\n",pRegTable->au8Eidb[15]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[0](%d)\n",pRegTable->au16Eaoff[0]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[1](%d)\n",pRegTable->au16Eaoff[1]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[2](%d)\n",pRegTable->au16Eaoff[2]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[3](%d)\n",pRegTable->au16Eaoff[3]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[4](%d)\n",pRegTable->au16Eaoff[4]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[5](%d)\n",pRegTable->au16Eaoff[5]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[6](%d)\n",pRegTable->au16Eaoff[6]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[7](%d)\n",pRegTable->au16Eaoff[7]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[8](%d)\n",pRegTable->au16Eaoff[8]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[9](%d)\n",pRegTable->au16Eaoff[9]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[10](%d)\n",pRegTable->au16Eaoff[10]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[11](%d)\n",pRegTable->au16Eaoff[11]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[12](%d)\n",pRegTable->au16Eaoff[12]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[13](%d)\n",pRegTable->au16Eaoff[13]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[14](%d)\n",pRegTable->au16Eaoff[14]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->au16Eaoff[15](%d)\n",pRegTable->au16Eaoff[15]);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32EndDiff0(%d)\n",pRegTable->u32EndDiff0);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32EndDiff1(%d)\n",pRegTable->u32EndDiff1);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32EndDiff2(%d)\n",pRegTable->u32EndDiff2);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32Limit0(%d)\n",pRegTable->u32Limit0);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32Limit1(%d)\n",pRegTable->u32Limit1);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->u32Limit2(%d)\n",pRegTable->u32Limit2);
    DoVi_Printf("[DoVi_Prepare_B0103] pRegTable->pu32Table addr(%p)\n",pRegTable->pu32Table);

    DoVi_Printf("[DoVi_Prepare_B0103] u16Gamma(%d) \n",u16Gamma);
    DoVi_Printf("[DoVi_Prepare_B0103] u16A(%d) \n",u16A);
    DoVi_Printf("[DoVi_Prepare_B0103] u16B(%d) \n",u16B);
    DoVi_Printf("[DoVi_Prepare_B0103] u32G(%d) \n",u32G);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of CSC.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0104(MsHdr_CSC_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B0104]\n");
    pRegTable->u8Shift = 2; // 0:shift 12b, 1:shift 13b, 2:shift 14b, 3:shift 15b
    pRegTable->u8Round = 0;
    pRegTable->u16Offset0 = 0x0;
    pRegTable->u16Offset1 = 0x0;
    pRegTable->u16Offset2 = 0x0;
    pRegTable->u16Min0 = 0x0;
    pRegTable->u16Min1 = 0x0;
    pRegTable->u16Min2 = 0x0;
    pRegTable->u16Max0 = 0x9C40;
    pRegTable->u16Max1 = 0x9C40;
    pRegTable->u16Max2 = 0x9C40;

    MS_S32 M[3][3], C[3][3], Mout[3][3];
    M[0][0] = pMdsExt->RGBtoOpt_coef0;
    M[0][1] = pMdsExt->RGBtoOpt_coef1;
    M[0][2] = pMdsExt->RGBtoOpt_coef2;
    M[1][0] = pMdsExt->RGBtoOpt_coef3;
    M[1][1] = pMdsExt->RGBtoOpt_coef4;
    M[1][2] = pMdsExt->RGBtoOpt_coef5;
    M[2][0] = pMdsExt->RGBtoOpt_coef6;
    M[2][1] = pMdsExt->RGBtoOpt_coef7;
    M[2][2] = pMdsExt->RGBtoOpt_coef8;
    C[0][0] = (1 << 15) - 2*pConfig->s32CrossTalk;
    C[0][1] = pConfig->s32CrossTalk;
    C[0][2] = pConfig->s32CrossTalk;
    C[1][0] = pConfig->s32CrossTalk;
    C[1][1] = (1 << 15) - 2*pConfig->s32CrossTalk;
    C[1][2] = pConfig->s32CrossTalk;
    C[2][0] = pConfig->s32CrossTalk;
    C[2][1] = pConfig->s32CrossTalk;
    C[2][2] = (1 << 15) - 2*pConfig->s32CrossTalk;
    MS_U16 i, j;
    for (i = 0; i < 3; i ++)
        for (j = 0; j < 3; j ++)
            Mout[i][j] = (C[i][0]*M[0][j] + C[i][1]*M[1][j] + C[i][2]*M[2][j]) >> 15;

    pRegTable->s16Coef0 = (MS_S16)Mout[0][0];
    pRegTable->s16Coef1 = (MS_S16)Mout[0][1];
    pRegTable->s16Coef2 = (MS_S16)Mout[0][2];
    pRegTable->s16Coef3 = (MS_S16)Mout[1][0];
    pRegTable->s16Coef4 = (MS_S16)Mout[1][1];
    pRegTable->s16Coef5 = (MS_S16)Mout[1][2];
    pRegTable->s16Coef6 = (MS_S16)Mout[2][0];
    pRegTable->s16Coef7 = (MS_S16)Mout[2][1];
    pRegTable->s16Coef8 = (MS_S16)Mout[2][2];
#if 0
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u8Shift(%d)\n",pRegTable->u8Shift);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u8Round(%d)\n",pRegTable->u8Round);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef0(%d)\n",pRegTable->s16Coef0);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef1(%d)\n",pRegTable->s16Coef1);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef2(%d)\n",pRegTable->s16Coef2);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef3(%d)\n",pRegTable->s16Coef3);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef4(%d)\n",pRegTable->s16Coef4);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef5(%d)\n",pRegTable->s16Coef5);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef6(%d)\n",pRegTable->s16Coef6);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef7(%d)\n",pRegTable->s16Coef7);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->s16Coef8(%d)\n",pRegTable->s16Coef8);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Offset0(%d)\n",pRegTable->u16Offset0);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Offset1(%d)\n",pRegTable->u16Offset1);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Offset2(%d)\n",pRegTable->u16Offset2);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Min0(%d)\n",pRegTable->u16Min0);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Min1(%d)\n",pRegTable->u16Min1);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Min2(%d)\n",pRegTable->u16Min2);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Max0(%d)\n",pRegTable->u16Max0);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Max1(%d)\n",pRegTable->u16Max1);
    DoVi_Printf("[DoVi_Prepare_B0104] pRegTable->u16Max2(%d)\n",pRegTable->u16Max2);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Gamma.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0105(MsHdr_Gamma_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
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

    pRegTable->u32Size = DOVI_GAMMA_SIZE;
    if (pRegTable->bEnable == FALSE)
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu16Table[u32Idx] = u32Idx << 7;
    }
    else
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu16Table[u32Idx] = g_au16DoViGammaPqLut[u32Idx];
    }

    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = DOVI_GAMMA_SIZE - 1;
    }
    else
    {
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
    }

    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
    {
        MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
    }
    else
    {
        MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
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

#if 0
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u32Size(%d)\n",pRegTable->u32Size);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u32Sp(%x)\n",pRegTable->u32Sp);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[0](%d)\n",pRegTable->au8Eidb[0]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[1](%d)\n",pRegTable->au8Eidb[1]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[2](%d)\n",pRegTable->au8Eidb[2]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[3](%d)\n",pRegTable->au8Eidb[3]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[4](%d)\n",pRegTable->au8Eidb[4]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[5](%d)\n",pRegTable->au8Eidb[5]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[6](%d)\n",pRegTable->au8Eidb[6]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[7](%d)\n",pRegTable->au8Eidb[7]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[8](%d)\n",pRegTable->au8Eidb[8]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[9](%d)\n",pRegTable->au8Eidb[9]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[10](%d)\n",pRegTable->au8Eidb[10]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[11](%d)\n",pRegTable->au8Eidb[11]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[12](%d)\n",pRegTable->au8Eidb[12]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[13](%d)\n",pRegTable->au8Eidb[13]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[14](%d)\n",pRegTable->au8Eidb[14]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[15](%d)\n",pRegTable->au8Eidb[15]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[16](%d)\n",pRegTable->au8Eidb[16]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[17](%d)\n",pRegTable->au8Eidb[17]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[18](%d)\n",pRegTable->au8Eidb[18]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[19](%d)\n",pRegTable->au8Eidb[19]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[20](%d)\n",pRegTable->au8Eidb[20]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[21](%d)\n",pRegTable->au8Eidb[21]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[22](%d)\n",pRegTable->au8Eidb[22]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[23](%d)\n",pRegTable->au8Eidb[23]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[24](%d)\n",pRegTable->au8Eidb[24]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[25](%d)\n",pRegTable->au8Eidb[25]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[26](%d)\n",pRegTable->au8Eidb[26]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[27](%d)\n",pRegTable->au8Eidb[27]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[28](%d)\n",pRegTable->au8Eidb[28]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[29](%d)\n",pRegTable->au8Eidb[29]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[30](%d)\n",pRegTable->au8Eidb[30]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au8Eidb[31](%d)\n",pRegTable->au8Eidb[31]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[0](%d)\n",pRegTable->au16Eaoff[0]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[1](%d)\n",pRegTable->au16Eaoff[1]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[2](%d)\n",pRegTable->au16Eaoff[2]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[3](%d)\n",pRegTable->au16Eaoff[3]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[4](%d)\n",pRegTable->au16Eaoff[4]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[5](%d)\n",pRegTable->au16Eaoff[5]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[6](%d)\n",pRegTable->au16Eaoff[6]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[7](%d)\n",pRegTable->au16Eaoff[7]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[8](%d)\n",pRegTable->au16Eaoff[8]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[9](%d)\n",pRegTable->au16Eaoff[9]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[10](%d)\n",pRegTable->au16Eaoff[10]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[11](%d)\n",pRegTable->au16Eaoff[11]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[12](%d)\n",pRegTable->au16Eaoff[12]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[13](%d)\n",pRegTable->au16Eaoff[13]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[14](%d)\n",pRegTable->au16Eaoff[14]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[15](%d)\n",pRegTable->au16Eaoff[15]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[16](%d)\n",pRegTable->au16Eaoff[16]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[17](%d)\n",pRegTable->au16Eaoff[17]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[18](%d)\n",pRegTable->au16Eaoff[18]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[19](%d)\n",pRegTable->au16Eaoff[19]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[20](%d)\n",pRegTable->au16Eaoff[20]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[21](%d)\n",pRegTable->au16Eaoff[21]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[22](%d)\n",pRegTable->au16Eaoff[22]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[23](%d)\n",pRegTable->au16Eaoff[23]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[24](%d)\n",pRegTable->au16Eaoff[24]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[25](%d)\n",pRegTable->au16Eaoff[25]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[26](%d)\n",pRegTable->au16Eaoff[26]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[27](%d)\n",pRegTable->au16Eaoff[27]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[28](%d)\n",pRegTable->au16Eaoff[28]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[29](%d)\n",pRegTable->au16Eaoff[29]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[30](%d)\n",pRegTable->au16Eaoff[30]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->au16Eaoff[31](%d)\n",pRegTable->au16Eaoff[31]);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16EndDiff0(%d)\n",pRegTable->u16EndDiff0);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16EndDiff1(%d)\n",pRegTable->u16EndDiff1);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16EndDiff2(%d)\n",pRegTable->u16EndDiff2);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16Limit0(%d)\n",pRegTable->u16Limit0);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16Limit1(%d)\n",pRegTable->u16Limit1);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->u16Limit2(%d)\n",pRegTable->u16Limit2);
    DoVi_Printf("[DoVi_Prepare_B0105] pRegTable->pu16Table addr(%p)\n",pRegTable->pu16Table);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of R2Y.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0106(MsHdr_R2Y_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B0106]\n");
    pRegTable->u8Shift = 1; // 0:shift 12b, 1:shift 13b, 2:shift 14b, 3:shift 15b
    pRegTable->u8Round = 0;
    pRegTable->s16Coef0 = 0x666;
    pRegTable->s16Coef1 = 0x666;
    pRegTable->s16Coef2 = 0x333;
    pRegTable->s16Coef3 = 0x4748;
    pRegTable->s16Coef4 = 0xb262;
    pRegTable->s16Coef5 = 0x656;
    pRegTable->s16Coef6 = 0xce4;
    pRegTable->s16Coef7 = 0x5b7;
    pRegTable->s16Coef8 = 0xed65;
    pRegTable->s16Offset0 = 0x0;
    pRegTable->s16Offset1 = 0x0;
    pRegTable->s16Offset2 = 0x0;
    pRegTable->s16Min0 = 0x8001;
    pRegTable->s16Min1 = 0x8001;
    pRegTable->s16Min2 = 0x8001;
    pRegTable->s16Max0 = 0x7fff;
    pRegTable->s16Max1 = 0x7fff;
    pRegTable->s16Max2 = 0x7fff;

#if 0
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->u8Shift(%d)\n",pRegTable->u8Shift);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->u8Round(%d)\n",pRegTable->u8Round);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef0(%d)\n",pRegTable->s16Coef0);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef1(%d)\n",pRegTable->s16Coef1);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef2(%d)\n",pRegTable->s16Coef2);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef3(%d)\n",pRegTable->s16Coef3);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef4(%d)\n",pRegTable->s16Coef4);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef5(%d)\n",pRegTable->s16Coef5);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef6(%d)\n",pRegTable->s16Coef6);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef7(%d)\n",pRegTable->s16Coef7);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Coef8(%d)\n",pRegTable->s16Coef8);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Offset0(%d)\n",pRegTable->s16Offset0);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Offset1(%d)\n",pRegTable->s16Offset1);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Offset2(%d)\n",pRegTable->s16Offset2);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Min0(%d)\n",pRegTable->s16Min0);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Min1(%d)\n",pRegTable->s16Min1);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Min2(%d)\n",pRegTable->s16Min2);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Max0(%d)\n",pRegTable->s16Max0);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Max1(%d)\n",pRegTable->s16Max1);
    DoVi_Printf("[DoVi_Prepare_B0106] pRegTable->s16Max2(%d)\n",pRegTable->s16Max2);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Color Adjustment.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B0107(MsHdr_CA_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B0107]\n");
    //pRegTable->u16ChromaWeight = DoVi_Clamp((pMdsExt->Trima[TrimTypeChromaWeight][0] - 2048 + pConfig->s16ChromaWeightBias) << 4, 0, 0xFFFF);
	pRegTable->u16ChromaWeight = DoVi_Clamp(((MS_S32)pMdsExt->chromaWeight_final - 2048 + pConfig->s16ChromaWeightBias) << 4, 0, 0xFFFF);

#if 0
    DoVi_Printf("[DoVi_Prepare_B0107] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B0107] pRegTable->u16ChromaWeight(%d)\n",pRegTable->u16ChromaWeight);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Tone Mapping.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B02(MsHdr_TM_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B02]\n");
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
    DoVi_TmoInfo_t stTmoInfo;
    MS_U32 u32Idx = 0;
    MS_U16 u16RegEnable = 0;

    pRegTable->u32Size = DOVI_TMO_SIZE;
    pRegTable->u8InGain = 0x10;
    pRegTable->u8OutGain = 0x10;
    pRegTable->u8UserGain = 0x10;

    if (pRegTable->bEnable == FALSE)
    {
        for (u32Idx = 0; u32Idx < pRegTable->u32Size; u32Idx++)
            pRegTable->pu16Table[u32Idx] = u32Idx << 3; // 9b index to 12b data
    }
    else
    {
        stTmoInfo.u16Mode = pConfig->u8RunMode;
        stTmoInfo.u16Crush = pMdsExt->min_PQ;
        stTmoInfo.u16Mid = pMdsExt->mid_PQ;
        stTmoInfo.u16Clip = pMdsExt->max_PQ;
        stTmoInfo.u16SminPq = pMdsExt->source_min_PQ;
        stTmoInfo.u16SmaxPq = pMdsExt->source_max_PQ;
        stTmoInfo.u16SdiagInches = pMdsExt->source_diagonal;
        //stTmoInfo.s16Slope  = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->Trima[TrimTypeSlope][0] - 2048 + pConfig->s16TrimSlopeBias, -2048, 2047);
        //stTmoInfo.s16Offset = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->Trima[TrimTypeOffset][0] - 2048 + pConfig->s16TrimOffsetBias, -2048, 2047);
        //stTmoInfo.s16Power  = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->Trima[TrimTypePower][0] - 2048 + pConfig->s16TrimPowerBias, -2048, 2047);
		stTmoInfo.s16Slope  = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->trimSlope_final - 2048 + pConfig->s16TrimSlopeBias, -2048, 2047);
        stTmoInfo.s16Offset = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->trimOffset_final - 2048 + pConfig->s16TrimOffsetBias, -2048, 2047);
        stTmoInfo.s16Power  = (MS_S16)DoVi_Clamp((MS_S32)pMdsExt->trimPower_final - 2048 + pConfig->s16TrimPowerBias, -2048, 2047);
        stTmoInfo.u16TminPq = pConfig->u16TminPQ;
        stTmoInfo.u16TmaxPq = pConfig->u16TmaxPQ;
        stTmoInfo.u16TdiagInches = pConfig->u16Tdiagonalinches;
        stTmoInfo.s16TminPqBias = pConfig->s16TMinPQBias;
        stTmoInfo.s16TmidPqBias = pConfig->s16TMidPQBias;
        stTmoInfo.s16TmaxPqBias = pConfig->s16TMaxPQBias;
        stTmoInfo.s16Contrast = pConfig->s16TContrast;
        stTmoInfo.s16Brightness = pConfig->s16TBrightness;
        stTmoInfo.s32Rolloff = pConfig->s32Rolloff;
        //DoVi_CreateToneCurve(&stTmoInfo, pRegTable->pu16Table);
    }

    u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK7A_70_L));
    if ((u16RegEnable >> 15) & 0x1) // use auto-download
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = DOVI_TMO_SIZE - 1;
        if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
        {
            MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
        }
        else
        {
            MHal_XC_StoreHDRAutoDownload((MS_U8*)pRegTable->pu16Table, pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
        }
    }
    else
    {
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
    }
#if 0
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->u32Size(%d)\n",pRegTable->u32Size);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->u8InGain(%d)\n",pRegTable->u8InGain);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->u8OutGain(%d)\n",pRegTable->u8OutGain);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->u8UserGain(%d)\n",pRegTable->u8UserGain);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Coef02(%d)\n",pRegTable->s16Coef02);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Coef11(%d)\n",pRegTable->s16Coef11);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Coef12(%d)\n",pRegTable->s16Coef12);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Coef21(%d)\n",pRegTable->s16Coef21);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Offset0(%d)\n",pRegTable->s16Offset0);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Offset1(%d)\n",pRegTable->s16Offset1);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->s16Offset2(%d)\n",pRegTable->s16Offset2);
    DoVi_Printf("[DoVi_Prepare_B02] pRegTable->pu16Table addr(%p)\n",pRegTable->pu16Table);

    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16Mode(%d)\n", stTmoInfo.u16Mode);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16Crush(%d)\n", stTmoInfo.u16Crush);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16Mid(%d)\n", stTmoInfo.u16Mid);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16Clip(%d)\n", stTmoInfo.u16Clip);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16Slope(%d)\n", stTmoInfo.s16Slope);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16Offset(%d)\n", stTmoInfo.s16Offset);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16Power(%d)\n", stTmoInfo.s16Power);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16SminPq(%d)\n", stTmoInfo.u16SminPq);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16SmaxPq(%d)\n", stTmoInfo.u16SmaxPq);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16SdiagInches(%d)\n", stTmoInfo.u16SdiagInches);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16TminPq(%d)\n", stTmoInfo.u16TminPq);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16TmaxPq(%d)\n", stTmoInfo.u16TmaxPq);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.u16TdiagInches(%d)\n", stTmoInfo.u16TdiagInches);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16TminPqBias(%d)\n", stTmoInfo.s16TminPqBias);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16TmidPqBias(%d)\n", stTmoInfo.s16TmidPqBias);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16TmaxPqBias(%d)\n", stTmoInfo.s16TmaxPqBias);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16Contrast(%d)\n", stTmoInfo.s16Contrast);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s16Brightness(%d)\n", stTmoInfo.s16Brightness);
    DoVi_Printf("[DoVi_Prepare_B02] stTmoInfo.s32Rolloff(%d)\n", stTmoInfo.s32Rolloff);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Detail Restoration.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B03(MsHdr_DR_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{

    MS_S32 temp;
    MS_S16 temp2;

    DoVi_Printf("//[DoVi_Prepare_B03]\n");
    pRegTable->u16MsWeightEdge = pConfig->u16MsWeightEdge;
    //pRegTable->s16MsWeight = (pMdsExt->extLvl >= 1) ? pMdsExt->Trima[TrimTypeMsWeight][0] : pConfig->s16MsWeight;

	//pRegTable->s16MsWeight = pMdsExt->msWeight_final;
    //DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%x)\n",pRegTable->s16MsWeight);
    //DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%d)\n",pRegTable->s16MsWeight);

    DoVi_Printf("[DoVi_Prepare_B03] pMdsExt->msWeight_final(%x)\n",pMdsExt->msWeight_final);
    DoVi_Printf("[DoVi_Prepare_B03] pMdsExt->msWeight_final(%d)\n",pMdsExt->msWeight_final);

    temp = pMdsExt->msWeight_final*2 + pConfig->s16MsWeightBias;
    temp2 = DoVi_Clamp(temp, 0, 0x1FFF);

    //pRegTable->s16MsWeight = 0xFFF - DoVi_Clamp((pRegTable->s16MsWeight*2 + pConfig->s16MsWeightBias), 0, 0x1FFF);
    pRegTable->s16MsWeight = ((MS_S16)0xFFF) - temp2;

    DoVi_Printf("[DoVi_Prepare_B03] temp(%x)\n",temp);
    DoVi_Printf("[DoVi_Prepare_B03] temp(%d)\n",temp);

    DoVi_Printf("[DoVi_Prepare_B03] temp2(%x)\n",temp2);
    DoVi_Printf("[DoVi_Prepare_B03] temp2(%d)\n",temp2);

    DoVi_Printf("[DoVi_Prepare_B03] pMdsExt->pConfig->s16MsWeightBias(%x)\n",pConfig->s16MsWeightBias);
    DoVi_Printf("[DoVi_Prepare_B03] pMdsExt->pConfig->s16MsWeightBias(%d)\n",pConfig->s16MsWeightBias);

    //DoVi_Printf("[DoVi_Prepare_B03] cal temp(%x)\n",(pRegTable->s16MsWeight*2 + pConfig->s16MsWeightBias));
    //DoVi_Printf("[DoVi_Prepare_B03] cal temp(%d)\n",(pRegTable->s16MsWeight*2 + pConfig->s16MsWeightBias));

    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%x)\n",pRegTable->s16MsWeight);
    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%d)\n",pRegTable->s16MsWeight);

    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->bEnable(%d)\n",pRegTable->bEnable);

    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->u16MsWeightEdge(%x)\n",pRegTable->u16MsWeightEdge);
    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->u16MsWeightEdge(%d)\n",pRegTable->u16MsWeightEdge);


    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%x)\n",pRegTable->s16MsWeight);
    DoVi_Printf("[DoVi_Prepare_B03] pRegTable->s16MsWeight(%d)\n",pRegTable->s16MsWeight);

}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of Color Correction.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B04(MsHdr_CC_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B04]\n");
    pRegTable->bClampEn = ENABLE;
    pRegTable->u16HuntGain = 0x800;
    pRegTable->u16HuntOffset = 0xFFF;
    pRegTable->u16SatGain = 0x1000;
    pRegTable->u16HighTh = 0xFFFF;
    pRegTable->u16LowTh = 0x4000;
    pRegTable->u16ZeroTh = 0x7FFF;
    pRegTable->u16CbOffset = 0;
    pRegTable->u16CrOffset = 0;
    //pRegTable->u16SatGain = DoVi_Clamp(((MS_S32)pMdsExt->Trima[TrimTypeSaturationGain][0] + 2048 + pConfig->s16SaturationGainBias), 0x800, 0x17FF);
	pRegTable->u16SatGain = DoVi_Clamp(((MS_S32)pMdsExt->saturationGain_final + 2048 + pConfig->s16SaturationGainBias), 0x800, 0x17FF);


#if 0
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->bClampEn(%d)\n",pRegTable->bClampEn);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16HuntGain(%d)\n",pRegTable->u16HuntGain);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16HuntOffset(%d)\n",pRegTable->u16HuntOffset);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16SatGain(%d)\n",pRegTable->u16SatGain);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16HighTh(%d)\n",pRegTable->u16HighTh);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16LowTh(%d)\n",pRegTable->u16LowTh);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16ZeroTh(%d)\n",pRegTable->u16ZeroTh);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16CbOffset(%d)\n",pRegTable->u16CbOffset);
    DoVi_Printf("[DoVi_Prepare_B04] pRegTable->u16CrOffset(%d)\n",pRegTable->u16CrOffset);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Prepare registers of 3DLUT. Do not provide content update nor SW bypass.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_Prepare_B05(MsHdr_3D_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
{
    DoVi_Printf("//[DoVi_Prepare_B05]\n");

    pRegTable->s16MinY = pConfig->s16Lut3DMin[0];
    pRegTable->s16MaxY = pConfig->s16Lut3DMax[0];
    pRegTable->u32InvY = ((MS_U32)1 << 30) / (pRegTable->s16MaxY - pRegTable->s16MinY);
    pRegTable->s16MinC = pConfig->s16Lut3DMin[1];
    pRegTable->s16MaxC = pConfig->s16Lut3DMax[1];
    pRegTable->u32InvC = ((MS_U32)1 << 30) / (pRegTable->s16MaxC - pRegTable->s16MinC);
#if 0
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->bEnable(%d)\n",pRegTable->bEnable);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->u32Size(%d)\n",pRegTable->u32Size);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->s16MinY(%d)\n",pRegTable->s16MinY);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->s16MaxY(%d)\n",pRegTable->s16MaxY);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->u32InvY(%d)\n",pRegTable->u32InvY);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->s16MinC(%d)\n",pRegTable->s16MinC);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->s16MaxC(%d)\n",pRegTable->s16MaxC);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->u32InvC(%d)\n",pRegTable->u32InvC);
    DoVi_Printf("[DoVi_Prepare_B05] pRegTable->pu16Table addr(%p)\n",pRegTable->pu16Table);

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
void MsHdr_Update_3D(const MsHdr_3D_RegTable_t* pRegTable)
{
    ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO stAutoDownloadFormat;
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
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_54_L), (MS_U16)pRegTable->s16MinY);
        DoVi_Write2Bytes(DoVi_Addr(REG_SC_BK7A_55_L), (MS_U16)pRegTable->s16MaxY);
        DoVi_Write4Bytes(DoVi_Addr(REG_SC_BK7A_56_L), pRegTable->u32InvY);
    }

    if (pRegTable->bEnable == FALSE)
    { // this is a debug feature !!
      // it is usually TRUE but in debug mode to be FALSE
        _bNeedDownload3DLut = TRUE;
    }

    if (_bNeedDownload3DLut)
    {
        stAutoDownloadFormat.enSubClient = E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT;
        stAutoDownloadFormat.bEnableRange = ENABLE;
        stAutoDownloadFormat.u16StartAddr = 0;
        stAutoDownloadFormat.u16EndAddr = DOVI_3DLUT_SIZE - 1;
        MHal_XC_WriteAutoDownload(E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR, (MS_U8 *)(pRegTable->pu16Table), pRegTable->u32Size * sizeof(MS_U16), (void*)&stAutoDownloadFormat);
        _bNeedDownload3DLut = FALSE;
    }
}

//-------------------------------------------------------------------------------------------------
/// Pre-set auto-download.
/// note:
///     Need to be paired with /MsHdr_Update_Autodownload. Called before writing data to DRAM.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void MsHdr_Prepare_Autodownload(MsHdr_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt)
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
///
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
int DoVi_DmCheckMetadata(DoVi_MdsExt_t* pMdsExt)
{
    // TODO
    int rv = 0;
    return rv;
}

//-------------------------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------------------------

// bubble sort to ascending order according to pTrim->Trima[TrimTypeTMaxPq][*]
//static void SortTrims(Trim_t *pTrim, unsigned short *inIdxa, unsigned short *tMaxPqa) //from C model v1.4
static void SortTrims(DoVi_MdsExt_t* pMdsExt, unsigned short *inIdxa, unsigned short *tMaxPqa) //modifed for current definition
{
#define SWAP_AT_IDX(buf, i, t)	t = buf[i]; buf[i] = buf[i+1]; buf[i+1] = t

	//assert(TrimTypeTMaxPq == 0); // a must be

	unsigned short trim;

	/// prepare an re-odering idx vector, make a copy of TMaxPq for sorting
	// (Trima[TrimTypeTMaxPq][0] is SMaxPq
	for (trim = 0; trim <= pMdsExt->TrimNum; ++trim) {
		inIdxa[trim] = trim;
		tMaxPqa[trim] = pMdsExt->Trima[TrimTypeTMaxPq][trim]; // pTrim->Trima[TrimTypeTMaxPq][0] = current SMaxPq
	}

	//// bubble sorting tMaxPqa
	unsigned short c;
	unsigned short tmp;
	bool cnt = true;
	// pTrim->tMaxPqa[0][0] is the SMaxPq pTrim->tMaxPqa[1..][0] is the default value
	for (trim = pMdsExt->TrimNum; trim > 0 && cnt; --trim) { // compare to range
		// each loop get max at trim for [0, trim]
		cnt = false;
		for (c = 0; c < trim; ++c) { // candidate
			if (tMaxPqa[c] > tMaxPqa[c+1]) {
				SWAP_AT_IDX(tMaxPqa, c, tmp);
				SWAP_AT_IDX(inIdxa, c, tmp);
				cnt = true;
			}
		}
	}
}


//int CommitTrim(Trim_t *pTrim, DmCtxtFxp_t *pDmCtxt) //from C model v1.4
int CommitTrim(DoVi_MdsExt_t* pMdsExt,const DoVi_Config_t* pConfig) //modifed for current definition
{
    MS_U16 inIdxa[DOVI_TRIM_TYPE_MAX];
    MS_U16 tMaxPqa[DOVI_TRIM_TYPE_MAX];

    memset(inIdxa, 0, sizeof(MS_U16)*DOVI_TRIM_TYPE_MAX);
    memset(tMaxPqa, 0, sizeof(MS_U16)*DOVI_TRIM_TYPE_MAX);

    SortTrims(pMdsExt, inIdxa, tMaxPqa);
    MS_U8 u8Temp = 0;
    DoVi_Printf("[CommitTrim] sortTrims inIdxa: ");
    for(u8Temp =0;u8Temp<DOVI_TRIM_TYPE_MAX;u8Temp++)
    {
        DoVi_Printf("%d ",inIdxa[u8Temp]);
    }
    DoVi_Printf("\n");

    DoVi_Printf("[CommitTrim] sortTrims tMaxPqa: ");
    for(u8Temp =0;u8Temp<DOVI_TRIM_TYPE_MAX;u8Temp++)
    {
        DoVi_Printf("%d ",tMaxPqa[u8Temp]);
    }
    DoVi_Printf("\n");

    //MS_U16 TMaxPq = pDmCtxt->dmCfg.tgtSigEnv.MaxPq; //In C model
    MS_U16 TMaxPq = pConfig->u16TmaxPQ;
    MS_S32 theTrim = -1;
    MS_S32 valsAtTMax[DOVI_TRIM_TYPE_MAX];
    uint16_t ty = 0;
    // T >= S
    if (pMdsExt->TrimNum ==0 || TMaxPq >= pMdsExt->Trima[TrimTypeTMaxPq][0]) {
        theTrim = 0; // the default
    }
    //  TMaxPq <= tMaxPqa[0]
    else if (TMaxPq <= tMaxPqa[0]) {
        theTrim = inIdxa[0];
    }

    //  pTrim->TrimNum >= tMaxPqa[pTrim->TrimNum]
    else if (TMaxPq >= tMaxPqa[pMdsExt->TrimNum]) {
        theTrim = inIdxa[pMdsExt->TrimNum];
    }
    else {
        // TMaxPq in bound && pTrim->TrimNum > 0
        uint16_t idx1, idx2;
        for (idx2 = 1; TMaxPq >= tMaxPqa[idx2]; ++idx2);
        idx1 = idx2-1;

        //assert(idx2 <= pMdsExt->TrimNum);

        if (tMaxPqa[idx1] == tMaxPqa[idx2]) {
            // shall not happen, but let it be
            theTrim = inIdxa[idx1];
        }
        else {
            // inter/extra-polation into
            for ( ty = 1; ty < TrimTypeNum; ++ty) {
                valsAtTMax[ty] = (((int32_t)pMdsExt->Trima[ty][inIdxa[idx2]] - (int32_t)pMdsExt->Trima[ty][inIdxa[idx1]])<<15)
                        / ((int32_t)tMaxPqa[idx2] - (int32_t)tMaxPqa[idx1]);
                valsAtTMax[ty] = (valsAtTMax[ty] * (TMaxPq - tMaxPqa[idx1])) >> 15;
                valsAtTMax[ty] += pMdsExt->Trima[ty][inIdxa[idx1]];
            }
        }
    }

    if (theTrim != -1) {
        for ( ty = 1; ty < TrimTypeNum; ++ty) {
            valsAtTMax[ty] = pMdsExt->Trima[ty][theTrim];
            }
        }

        valsAtTMax[0] = TMaxPq;

    // distribute the derived value convert back to floating domain value
    pMdsExt->trimSlope_final        =valsAtTMax[TrimTypeSlope];
    pMdsExt->trimOffset_final       =valsAtTMax[TrimTypeOffset];
    pMdsExt->trimPower_final        =valsAtTMax[TrimTypePower] ;
    pMdsExt->chromaWeight_final     =valsAtTMax[TrimTypeChromaWeight];
    pMdsExt->saturationGain_final   =valsAtTMax[TrimTypeSaturationGain];
    pMdsExt->msWeight_final         =valsAtTMax[TrimTypeMsWeight];


	//pDmCtxt->tcCtrl.trimSlope        = (int16_t) CLAMPS(valsAtTMax[TrimTypeSlope]          - 2048 + pDmCtxt->tcCtrl.tTrimSlopeBias,               -2048,  (int32_t)DLB_UINT_MAX(11));
    //pDmCtxt->tcCtrl.trimOffset       = (int16_t) CLAMPS(valsAtTMax[TrimTypeOffset]         - 2048 + pDmCtxt->tcCtrl.tTrimOffsetBias,              -2048,  (int32_t)DLB_UINT_MAX(11));
	//pDmCtxt->tcCtrl.trimPower        = (int16_t) CLAMPS(valsAtTMax[TrimTypePower]          - 2048 + pDmCtxt->tcCtrl.tTrimPowerBias,               -2048,  (int32_t)DLB_UINT_MAX(11));
    //pDmCtxt->dmExec.chromaWeight     = (uint16_t)CLAMPS(((valsAtTMax[TrimTypeChromaWeight] - 2048 + pDmCtxt->dmCfg.dmCtrl.ChromaWeightBias) << 4), 0,     (int32_t)DLB_UINT_MAX(16));
    //pDmCtxt->dmExec.saturationGain   = (uint16_t)CLAMPS(valsAtTMax[TrimTypeSaturationGain] + 2048 + pDmCtxt->dmCfg.dmCtrl.SaturationGainBias,     2048,   (int32_t)DLB_UINT_MAX(12) + 2048);
    //pDmCtxt->dmExec.msWeight         = (uint16_t)CLAMPS(valsAtTMax[TrimTypeMsWeight] * 2          + pDmCtxt->dmCfg.bldCtrl.MSWeightBias,           0,     (int32_t)DLB_UINT_MAX(13));

    return 0;
}


//-------------------------------------------------------------------------------------------------
/// Extract metadata from input array.
/// @param  An array /pu8MDS
/// @return the number of read bytes if success, or a negative number if fail.
//-------------------------------------------------------------------------------------------------
int DoVi_DmReadMetadata(DoVi_MdsExt_t* pMdsExt, const MS_U8* pu8MDS, MS_U16 u16Len, const DoVi_Config_t* pConfig)
{
    MS_U16 u16Count = 0;
    MS_U32 extLen;
    MS_U8 n;
    int i = 0;

#if 1 // change endianness
#define ASIGN_AND_EXIT_ON_FALSE_RETURN(data, type) \
    if (sizeof(type) == 4) \
        data = (type)MsHdr_ChangeEndianness_4Bytes(*(MS_U32 *)(pu8MDS + u16Count)); \
    else if (sizeof(type) == 2) \
        data = (type)MsHdr_ChangeEndianness_2Bytes(*(MS_U16 *)(pu8MDS + u16Count)); \
    else \
        data = *(type *)(pu8MDS + u16Count); \
    u16Count += sizeof(type); \
    if (u16Count > u16Len) \
        return -(int)u16Count;
#define SKIP_AND_EXIT_ON_FALSE_RETURN(len) \
    u16Count += len; \
    if (u16Count > u16Len) \
        return -(int)u16Count;
#else
#define ASIGN_AND_EXIT_ON_FALSE_RETURN(data, type) \
    data = *(type *)(pu8MDS + u16Count); \
    u16Count += sizeof(type); \
    if (u16Count > u16Len) \
        return -(int)u16Count;
#define SKIP_AND_EXIT_ON_FALSE_RETURN(len) \
    u16Count += len; \
    if (u16Count > u16Len) \
        return -(int)u16Count;
#endif

    // control
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->affected_dm_metadata_id, MS_U8 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->scene_refresh_flag, MS_U8 );

    // yuv=>rgb
    //pMdsExt->m33Yuv2RgbScale2P = 13;
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef0, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef1, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef2, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef3, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef4, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef5, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef6, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef7, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_coef8, MS_S16 );

    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_offset0, MS_U32 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_offset1, MS_U32 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->YCCtoRGB_offset2, MS_U32 );

    // rgb=>lms
    //pMdsExt->m33Rgb2WpLmsScale2P = 14;
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef0, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef1, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef2, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef3, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef4, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef5, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef6, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef7, MS_S16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->RGBtoOpt_coef8, MS_S16 );

    // EOTF gamma, a, b
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_eotf, MS_U16 );      // 1<<14
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_eotf_param0, MS_U16 ); // 1<<2
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_eotf_param1, MS_U16 ); // 1<<16
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_eotf_param2, MS_U32 ); // 1<<18

    // signal info
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_bit_depth, MS_U8 );
	//force 14
	pMdsExt->signal_bit_depth = 14;

    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_color_space, MS_U8 );

    pMdsExt->signal_bit_depth = 14;

    if (pMdsExt->signal_color_space > 1)
    {
        // as of today, only case is 2, but just in case ..., use > 1
        pMdsExt->signal_color_space = 0;
    }
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_chroma_format, MS_U8 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->signal_full_range_flag, MS_U8 );

    // source minitor: all PQ scale 4095
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->source_min_PQ, MS_U16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->source_max_PQ, MS_U16 );
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->source_diagonal, MS_U16 );

    // init level 1 default value
    pMdsExt->min_PQ = pMdsExt->source_min_PQ;
    pMdsExt->max_PQ = pMdsExt->source_max_PQ;
    pMdsExt->mid_PQ = (pMdsExt->min_PQ + pMdsExt->max_PQ) >> 1;

    // init level 2: no trim
    pMdsExt->TrimNum = 0;
    // default value
    pMdsExt->Trima[TrimTypeTMaxPq][0] = pMdsExt->source_max_PQ;
    pMdsExt->Trima[TrimTypeSlope][0] = 0x800;
    pMdsExt->Trima[TrimTypeOffset][0] = 0x800;
    pMdsExt->Trima[TrimTypePower][0] = 0x800;
    pMdsExt->Trima[TrimTypeChromaWeight][0] = 0x800;
    pMdsExt->Trima[TrimTypeSaturationGain][0] = 0x800;
    pMdsExt->Trima[TrimTypeMsWeight][0] = 0x800;
    // [TrimTypeTMaxPq][1...] inited in InitSceneInfoValue and independent of MDS info

    ////// parsing externsion block
    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->num_ext_blocks, MS_U8 );

    for (n = 0; n < pMdsExt->num_ext_blocks; ++n)
    {
        ASIGN_AND_EXIT_ON_FALSE_RETURN( extLen, MS_U32 );
        ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->extLvl, MS_U8 );

        if (pMdsExt->extLvl == 1)
        {
            //// which is level 1 in new mds
            // the ccm
            ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->min_PQ, MS_U16 );
            ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->max_PQ, MS_U16 );
            ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->mid_PQ, MS_U16 );

            extLen -= 3*2;
        }
        else if (pMdsExt->extLvl == 2)
        {
            //// level 2
            if (pMdsExt->TrimNum < DOVI_TRIM_NUM_MAX - 1)
            {
                // only get the first TRIM_NUM_MAX - 1 max value (== 17 - 1 = 16)
                ++(pMdsExt->TrimNum);

                for (i = 0; i < TrimTypeNum; ++i)
                {
                    ASIGN_AND_EXIT_ON_FALSE_RETURN( pMdsExt->Trima[i][pMdsExt->TrimNum], MS_U16 );
                }

                extLen -= TrimTypeNum*2;
            }
        }
        // else unknow, skip everything

        SKIP_AND_EXIT_ON_FALSE_RETURN( extLen );
    }

#undef ASIGN_AND_EXIT_ON_FALSE_RETURN
#undef SKIP_AND_EXIT_ON_FALSE_RETURN

	CommitTrim(pMdsExt,pConfig);


    if (DoVi_DmCheckMetadata(pMdsExt) < 0)
        return -1;

    return u16Count;
}

//-------------------------------------------------------------------------------------------------
/// Calculate Registers and LUT contents during frame DE.
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmFrameDeCalculate(MsHdr_RegTable_t* pRegTable, const DoVi_Config_t* pConfig, const DoVi_MdsExt_t* pMdsExt, DoVi_Comp_ExtConfig_t* pComConfig)
{
    MS_U16 u16RegEnable = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7C_L));
    if (u16RegEnable & 0x1) // BK79 h7c[0] for B01 big bypass
    {
        pRegTable->stY2R.bEnable     = (u16RegEnable >>  1) & 0x1; // h7c[ 1]
        pRegTable->stDegamma.bEnable = (u16RegEnable >>  2) & 0x1; // h7c[ 2]
        pRegTable->stCSC.bEnable     = (u16RegEnable >>  3) & 0x1; // h7c[ 3]
        pRegTable->stGamma.bEnable   = (u16RegEnable >>  4) & 0x1; // h7c[ 4]
        pRegTable->stR2Y.bEnable     = (u16RegEnable >>  5) & 0x1; // h7c[ 5]
        pRegTable->stCA.bEnable      = (u16RegEnable >>  6) & 0x1; // h7c[ 6]
        pRegTable->stTM.bEnable      = (u16RegEnable >>  8) & 0x1; // h7c[ 8]
        pRegTable->stDR.bEnable      = (u16RegEnable >>  9) & 0x1; // h7c[ 9]
        pRegTable->stCC.bEnable      = (u16RegEnable >> 10) & 0x1; // h7c[10]
        pRegTable->st3D.bEnable      = (u16RegEnable >> 11) & 0x1; // h7c[11]
    }
    else
    {
        pRegTable->stY2R.bEnable     = FALSE; // h7c[ 1]
        pRegTable->stDegamma.bEnable = FALSE; // h7c[ 2]
        pRegTable->stCSC.bEnable     = FALSE; // h7c[ 3]
        pRegTable->stGamma.bEnable   = FALSE; // h7c[ 4]
        pRegTable->stR2Y.bEnable     = FALSE; // h7c[ 5]
        pRegTable->stCA.bEnable      = FALSE; // h7c[ 6]
        pRegTable->stTM.bEnable      = FALSE; // h7c[ 8]
        pRegTable->stDR.bEnable      = FALSE; // h7c[ 9]
        pRegTable->stCC.bEnable      = FALSE; // h7c[10]
        pRegTable->st3D.bEnable      = FALSE; // h7c[11]
    }

    MS_U16 u16RegUpdate = DoVi_Read2Bytes(DoVi_Addr(REG_SC_BK79_7D_L));
    if (pMdsExt->scene_refresh_flag | ((u16RegUpdate >> 15) & 0x1))
    {
        pRegTable->stY2R.bUpdate     = (u16RegUpdate >>  1) & 0x1; // h7d[ 1]
        pRegTable->stDegamma.bUpdate = (u16RegUpdate >>  2) & 0x1; // h7d[ 2]
        pRegTable->stCSC.bUpdate     = (u16RegUpdate >>  3) & 0x1; // h7d[ 3]
        pRegTable->stGamma.bUpdate   = (u16RegUpdate >>  4) & 0x1; // h7d[ 4]
        pRegTable->stR2Y.bUpdate     = (u16RegUpdate >>  5) & 0x1; // h7d[ 5]
        pRegTable->stCA.bUpdate      = (u16RegUpdate >>  6) & 0x1; // h7d[ 6]
        pRegTable->stTM.bUpdate      = (u16RegUpdate >>  8) & 0x1; // h7d[ 8]
        pRegTable->stDR.bUpdate      = (u16RegUpdate >>  9) & 0x1; // h7d[ 9]
        pRegTable->stCC.bUpdate      = (u16RegUpdate >> 10) & 0x1; // h7d[10]
        pRegTable->st3D.bUpdate      = (u16RegUpdate >> 11) & 0x1; // h7d[11]
    }
    else
    {
        pRegTable->stY2R.bUpdate = FALSE;
        pRegTable->stDegamma.bUpdate = FALSE;
        pRegTable->stCSC.bUpdate = FALSE;
        pRegTable->stGamma.bUpdate = FALSE;
        pRegTable->stR2Y.bUpdate = FALSE;
        pRegTable->stCA.bUpdate = FALSE;
        pRegTable->stTM.bUpdate = FALSE;
        pRegTable->stDR.bUpdate = FALSE;
        pRegTable->stCC.bUpdate = FALSE;
        pRegTable->st3D.bUpdate = FALSE;
    }

    MsHdr_Prepare_Autodownload(pRegTable, pConfig, pMdsExt);
    if (pRegTable->stY2R.bUpdate)
        DoVi_Prepare_B0102(&pRegTable->stY2R, pConfig, pMdsExt, pComConfig);
    if (pRegTable->stDegamma.bUpdate)
        DoVi_Prepare_B0103(&pRegTable->stDegamma, pConfig, pMdsExt);
    if (pRegTable->stCSC.bUpdate)
        DoVi_Prepare_B0104(&pRegTable->stCSC, pConfig, pMdsExt);
    if (pRegTable->stGamma.bUpdate)
        DoVi_Prepare_B0105(&pRegTable->stGamma, pConfig, pMdsExt);
    if (pRegTable->stR2Y.bUpdate)
        DoVi_Prepare_B0106(&pRegTable->stR2Y, pConfig, pMdsExt);
    if (pRegTable->stCA.bUpdate)
        DoVi_Prepare_B0107(&pRegTable->stCA, pConfig, pMdsExt);
    if (pRegTable->stTM.bUpdate)
        DoVi_Prepare_B02(&pRegTable->stTM, pConfig, pMdsExt);
    if (pRegTable->stDR.bUpdate)
        DoVi_Prepare_B03(&pRegTable->stDR, pConfig, pMdsExt);
    if (pRegTable->stCC.bUpdate)
        DoVi_Prepare_B04(&pRegTable->stCC, pConfig, pMdsExt);
    if (pRegTable->st3D.bUpdate)
        DoVi_Prepare_B05(&pRegTable->st3D, pConfig, pMdsExt);
}

//-------------------------------------------------------------------------------------------------
/// Update Registers and LUT contents at blanking interval.
/// note:
///     If metadata has any problem, do not call this method to update. (= Keep previous setting.)
/// @param
/// @return
//-------------------------------------------------------------------------------------------------
void DoVi_DmBlankingUpdate(const MsHdr_RegTable_t* pRegTable)
{
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

    if (!(CFD_IS_MM(_stCfdInit[0].u8InputSource) && (_u8Version == DOLBY_DS_VERSION)))
    {
        MsHdr_Update_Autodownload(pRegTable);
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
