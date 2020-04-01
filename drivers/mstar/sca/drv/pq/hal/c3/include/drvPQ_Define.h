////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DRVPQ_DEFINE_H_
#define _DRVPQ_DEFINE_H_

#define PQ_ONLY_SUPPORT_BIN 0
#define PQ_ENABLE_DEBUG     0
#define PQ_ENABLE_CHECK     0

#define ENABLE_PQ_BIN           1
#define ENABLE_PQ_MLOAD         0
#define ENABLE_PQ_EX            1

#define ENABLE_MULTI_SCALER     1

#define PQ_ENABLE_VD_SAMPLING   0
#define PQ_ENABLE_PIP           1
#define PQ_ENABLE_3D_VIDEO      1
#define PQ_ENABLE_IOCTL         1
#define PQ_ENABLE_COLOR         1

//!! for SC1 PIP setting !!//
#define PQ_ENABLE_PIP_SC1		0

#define PQ_QM_CVBS          1
#define PQ_QM_YPBPR         1
#define PQ_QM_PC            1
#define PQ_QM_HMDI          1
#define PQ_QM_HDMI_PC       1
#define PQ_QM_DTV           1
#define PQ_QM_MM_VIDEO      1
#define PQ_QM_MM_PHOTO      1
#define PQ_QM_3D            0
#define PQ_SKIPRULE_ENABLE  1
#define PQ_GRULE_NR_ENABLE  1
#define PQ_GRULE_MPEG_NR_ENABLE 1
#define PQ_GRULE_DDR_SELECT_ENABLE 0
#define PQ_GRULE_OSD_BW_ENABLE  1
#define PQ_GRULE_FILM_MODE_ENABLE  0
#define PQ_GRULE_DYNAMIC_CONTRAST_ENABLE  0
#define PQ_GRULE_ULTRAT_CLEAR_ENABLE 0
#define PQ_GRULE_PTP_ENABLE 0
#define PQ_XRULE_DB_ENABLE      0
#define ESTIMATE_AVAILABLE_RATE 85
#define PQ_ENABLE_FORCE_MADI    0
#define PQ_NEW_HSD_SAMPLING_TYPE 1
#define PQ_BW_H264_ENABLE 0
#define PQ_BW_MM_ENABLE 0
#define PQ_MADI_88X_MODE        1
#define PQ_UC_CTL               1
#define PQ_MADI_DFK             1
#define PQ_VIP_CTL              0
#define PQ_VIP_RGBCMY_CTL       0


#define PQ_EN_DMS_SW_CTRL	FALSE
#define PQ_ENABLE_RFBL		FALSE
#define PQ_EN_UCNR_OFF TRUE

#ifndef U8
#define U8 MS_U8
#else
#undef  U8
#define U8 MS_U8
#endif

#ifndef code
#define code
#else
#undef  code
#define code
#endif

#ifndef U16
#define U16 MS_U16
#else
#undef  U16
#define U16 MS_U16
#endif

extern MS_S32 _PQ_Mutex ;

#if defined (MSOS_TYPE_LINUX)
extern pthread_mutex_t _PQ_MLoad_Mutex;
#endif

#if PQ_ENABLE_DEBUG
extern MS_BOOL _u16DbgSwitch;
#define PQ_DBG(x)  \
do{                \
    if (_u16DbgSwitch){ \
        x;         \
    }              \
}while(0)

#define PQ_DUMP_DBG(x) x
#define PQ_DUMP_FILTER_DBG(x) x
#else
#define PQ_DBG(x) //x
#define PQ_DUMP_DBG(x)          //x
#define PQ_DUMP_FILTER_DBG(x)   //x
#endif

#define PQ_MAP_REG(reg) (((reg)>>8)&0xFF), ((reg)&0xFF)
#define REG_ADDR_SIZE   2
#define REG_MASK_SIZE   1

#define PQ_IP_NULL          0xFF

#define PQTBL_EX            0x10
#define PQTBL_NORMAL        0x11

#define E_XRULE_HSD         0
#define E_XRULE_VSD         1
#define E_XRULE_HSP         2
#define E_XRULE_VSP         3
#define E_XRULE_CSC         4

#if PQ_XRULE_DB_ENABLE
#define E_XRULE_DB_NTSC     5
#define E_XRULE_DB_PAL      6
#define E_XRULE_NUM         7
#else
#define E_XRULE_NUM         5
#endif

//Here must be the same order as MST_GRule_Index_Main
typedef enum {
        E_GRULE_NR,
#if(PQ_GRULE_OSD_BW_ENABLE == 1)
        E_GRULE_OSD_BW,
#endif
#if(PQ_GRULE_MPEG_NR_ENABLE == 1)
        E_GRULE_MPEG_NR,
#endif
#if(PQ_GRULE_FILM_MODE_ENABLE == 1)
        E_GRULE_FilmMode,
#endif
#if(PQ_GRULE_DYNAMIC_CONTRAST_ENABLE == 1)
        E_GRULE_DynContr,
#endif
#if(PQ_GRULE_DDR_SELECT_ENABLE == 1)
        E_GRULE_DDR_SELECT,
#endif
#if(PQ_GRULE_ULTRAT_CLEAR_ENABLE == 1)
        E_GRULE_ULTRACLEAR,
#endif
#if(PQ_GRULE_PTP_ENABLE == 1)
        E_GRULE_PTP,
#endif
        E_GRULE_NUM,
} EN_GRULE_INDEX;


#define _END_OF_TBL_        0xFFFF


typedef enum {
    PQ_TABTYPE_GENERAL,
    PQ_TABTYPE_COMB,
    PQ_TABTYPE_SCALER,
    PQ_TABTYPE_SRAM1,
    PQ_TABTYPE_SRAM2,
    PQ_TABTYPE_SRAM3,
    PQ_TABTYPE_SRAM4,
    PQ_TABTYPE_C_SRAM1,
    PQ_TABTYPE_C_SRAM2,
    PQ_TABTYPE_C_SRAM3,
    PQ_TABTYPE_C_SRAM4,
    PQ_TABTYPE_SRAM_COLOR_INDEX,
    PQ_TABTYPE_SRAM_COLOR_GAIN_SNR,
    PQ_TABTYPE_SRAM_COLOR_GAIN_DNR,
    PQ_TABTYPE_VIP_IHC_CRD_SRAM,
    PQ_TABTYPE_VIP_ICC_CRD_SRAM,
    PQ_TABTYPE_XVYCC_DE_GAMMA_SRAM,
    PQ_TABTYPE_XVYCC_GAMMA_SRAM,
    PQ_TABTYPE_SWDRIVER,
    PQ_TABTYPE_PICTURE_1,
    PQ_TABTYPE_PICTURE_2,
} EN_PQ_TABTYPE;


typedef enum
{
    PQ_MD_720x480_60I,      // 00
    PQ_MD_720x480_60P,      // 01
    PQ_MD_720x576_50I,      // 02
    PQ_MD_720x576_50P,      // 03
    PQ_MD_1280x720_50P,     // 04
    PQ_MD_1280x720_60P,     // 05
    PQ_MD_1920x1080_50I,    // 06
    PQ_MD_1920x1080_60I,    // 07
    PQ_MD_1920x1080_24P,    // 08
    PQ_MD_1920x1080_25P,    // 09
    PQ_MD_1920x1080_30P,    // 10
    PQ_MD_1920x1080_50P,    // 11
    PQ_MD_1920x1080_60P,    // 12
    PQ_MD_Num,
}PQ_MODE_INDEX;


typedef struct
{
    MS_U8 *pIPCommTable;
    MS_U8 *pIPTable;
    MS_U8 u8TabNums;
    MS_U8 u8TabType;
} EN_IPTAB_INFO;

typedef struct
{
    MS_U8 *pIPTable;
    MS_U8 u8TabNums;
    MS_U8 u8TabType;
    MS_U8 u8TabIdx;
} EN_IP_Info;

enum
{
    PQ_FUNC_DUMP_REG,
    PQ_FUNC_CHK_REG,
};

#define PQ_MUX_DEBUG    0

#define SCALER_REGISTER_SPREAD 1

#if(SCALER_REGISTER_SPREAD)
    #define BK_SCALER_BASE              0x130000

    // no need to store bank for spread reg
    #define SC_BK_STORE_NOMUTEX

    #define SC_BK_RESTORE_NOMUTEX


    #if(PQ_MUX_DEBUG)
    // no need to store bank for spread reg
    #define SC_BK_STORE_MUTEX     \
            if(!MsOS_ObtainMutex(_PQ_Mutex, MSOS_WAIT_FOREVER))                     \
            {                                                                        \
                printf("==========================\n");                              \
                printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
            }

    // restore bank
    #define SC_BK_RESTORE_MUTEX   \
            MsOS_ReleaseMutex(_PQ_Mutex);
    #else
    #define SC_BK_STORE_MUTEX     \
            if(!MsOS_ObtainMutex(_PQ_Mutex, MSOS_WAIT_FOREVER))                     \
            {                                                                        \
            }

    // restore bank
    #define SC_BK_RESTORE_MUTEX   \
            MsOS_ReleaseMutex(_PQ_Mutex);
    #endif



    // switch bank
    #define SC_BK_SWITCH(_x_)

    #define SC_BK_CURRENT   (u8CurBank)
#else
    #define BK_SCALER_BASE              0x102F00

    #define SC_BK_STORE_NOMUTEX     \
            MS_U8 u8Bank;      \
            u8Bank = MApi_XC_ReadByte(BK_SCALER_BASE)

    // restore bank
    #define SC_BK_RESTORE_NOMUTEX   MApi_XC_WriteByte(BK_SCALER_BASE, u8Bank);

#if(PQ_MUX_DEBUG)
// store bank
    #define SC_BK_STORE_MUTEX     \
        MS_U8 u8Bank;      \
        if(!MsOS_ObtainMutex(_PQ_Mutex, MSOS_WAIT_FOREVER))                     \
        {                                                                        \
            printf("==========================\n");                              \
            printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
        }        \
        u8Bank = MApi_XC_ReadByte(BK_SCALER_BASE)

// restore bank
    #define SC_BK_RESTORE_MUTEX   \
        MApi_XC_WriteByte(BK_SCALER_BASE, u8Bank); \
        MsOS_ReleaseMutex(_PQ_Mutex);
#else
    #define SC_BK_STORE_MUTEX     \
        MS_U8 u8Bank;      \
        if(!MsOS_ObtainMutex(_PQ_Mutex, MSOS_WAIT_FOREVER))                     \
        {                                                                        \
        }        \
        u8Bank = MApi_XC_ReadByte(BK_SCALER_BASE)

// restore bank
    #define SC_BK_RESTORE_MUTEX   \
        MApi_XC_WriteByte(BK_SCALER_BASE, u8Bank); \
        MsOS_ReleaseMutex(_PQ_Mutex);
#endif

// switch bank
#define SC_BK_SWITCH(_x_)\
        MApi_XC_WriteByte(BK_SCALER_BASE, _x_)

#define SC_BK_CURRENT   \
        MApi_XC_ReadByte(BK_SCALER_BASE)
#endif



// store bank
#define COMB_BK_STORE     \
            MS_U8 u8Bank;      \
            u8Bank = MApi_XC_ReadByte(COMB_REG_BASE)

// restore bank
#define COMB_BK_RESTORE   \
            MApi_XC_WriteByte(COMB_REG_BASE, u8Bank)
// switch bank
#define COMB_BK_SWITCH(_x_)\
            MApi_XC_WriteByte(COMB_REG_BASE, _x_)

#define COMB_BK_CURRENT   \
            MApi_XC_ReadByte(COMB_REG_BASE)



#define PQ_IP_COMM  0xfe
#define PQ_IP_ALL   0xff


typedef struct {
  PQ_WIN eWin;
  MS_U8 u8PQ_InputType_Num;
  MS_U8 u8PQ_IP_Num;
  MS_U8 *pQuality_Map_Aray;
  EN_IPTAB_INFO *pIPTAB_Info;
  MS_U8 *pSkipRuleIP;

  MS_U8 u8PQ_XRule_IP_Num[E_XRULE_NUM];
  MS_U8 *pXRule_IP_Index[E_XRULE_NUM];
  MS_U8 *pXRule_Array[E_XRULE_NUM];

  MS_U8 u8PQ_GRule_Num[E_GRULE_NUM];
  MS_U8 u8PQ_GRule_IPNum[E_GRULE_NUM];
  MS_U8 u8PQ_GRule_LevelNum[E_GRULE_NUM];
  MS_U8 *pGRule_IP_Index[E_GRULE_NUM];
  MS_U8 *pGRule_Array[E_GRULE_NUM];
  MS_U8 *pGRule_Level[E_GRULE_NUM];
} PQTABLE_INFO;

#endif /* _DRVPQ_DEFINE_H_ */

