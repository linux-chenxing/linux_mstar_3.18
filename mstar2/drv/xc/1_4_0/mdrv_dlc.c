///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_dlc.c
// @brief  DLC driver interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef  _MDRV_DLC_C
#define _MDRV_DLC_C
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "mdrv_mstypes.h"
#include "mdrv_dlc.h"
#include "mhal_xc.h"
#ifdef SUPPORT_XC_DLC
#include "mhal_dlc.h"
#endif

#define DLC_INI_FILE        "/config/DLC/DLC.ini"
#define DLC_INI_FILE_UPDATE "/Customer/DLC/DLC.ini"

#define DLC_BUF_SIZE         5000  // number of DLC.ini member length
#define DLC_RULE_BUF_SIZE    4 // number of rule_buf length
#define DLC_DATA_BUF_SIZE    6 // number of data_buf length
#define XC_DCL_DBG(x)        //(x)

// HDR metadata structure version. If you modify MS_HDR_INFO_EXTEND structure, please plus 1 to MS_HDR_VERSION and check MS_HDR_ITEM_COUNT value.
#define MS_HDR_VERSION  4
// HDR metadata structure item count. If you modify MS_HDR_INFO_EXTEND structure, please recalculate HDR metadata item count and modify MS_HDR_ITEM_COUNT.
#define MS_HDR_ITEM_COUNT 47 + 10 + 16

static MS_BOOL _bHistogramReady = FALSE , _bDlcInitReady = FALSE;
static MS_U16  _au16DlcInit[59] = { 0 };
static MS_U8   _au8DlcCurveInit[65] = { 0 };
//static MS_U8   _u8DlcCont=0;
static MS_BOOL _bSetDlcOn = TRUE;   //TURE : Enable DLC handler  ; FALSE : Disnable DLC handler
static volatile ST_KDRV_XC_DLC_INFO  _stDlcInfo ;
static volatile ST_KDRV_XC_DLC_BLE_INFO  _stBleInfo ;
static volatile ST_KDRV_XC_DLC_INIT_INFO  _stDlcInitInfo ;
#ifdef SUPPORT_XC_DLC
extern StuDlc_HDRinit g_HDRinitParameters;
#endif
//-------------------------------------------------------------------------------------------------
///Get the Ble slop point from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_SetBlePoint(void)
{
#ifdef SUPPORT_XC_DLC
    WORD _u16BLESlopPoint[6] = { 0 };
    BYTE ucTmp;

    for (ucTmp=0; ucTmp<6; ucTmp++)
    {
        _u16BLESlopPoint[ucTmp] = _stBleInfo.u16BLESlopPoint[ucTmp];
    }

    msDlcSetBleSlopPoint(_u16BLESlopPoint);
#endif
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param DLC_con_file
///@return TRUE: success
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_init(void)
{
#ifdef SUPPORT_XC_DLC
#if defined(CONFIG_ARM64)
    extern ptrdiff_t mstar_pm_base;
    msDlcSetPMBase((MS_U64)mstar_pm_base);
#endif
#endif
}

MS_BOOL MDrv_DLC_Ini(char *con_file ,char *con_fileUpdate)
{
    MS_BOOL  bReturn= FALSE;
#ifdef SUPPORT_XC_DLC
    char *buf;
    struct file *fp;
    int ret=0;
#if defined(CONFIG_ARM64)
    extern ptrdiff_t mstar_pm_base;
#endif
    //When accessing user memory, we need to make sure the entire area really is in user-level space.
    //KERNEL_DS addr user-level space need less than TASK_SIZE
    mm_segment_t old_fs=get_fs();
    set_fs(KERNEL_DS);

    //Used USB update DLC.ini
    fp = filp_open(con_fileUpdate, O_RDONLY , 0x400);

    if (fp==-2) //Can't find /Customer/DLC/DLC.ini
    {
        //Used initial DLC.ini
        fp = filp_open(con_file, O_RDONLY , 0x400);

        if (!fp)
        {
            return FALSE;
        }
    }

    buf = kzalloc(DLC_BUF_SIZE+1, GFP_KERNEL);
    if(!buf)
        return FALSE;

    if (fp->f_op && fp->f_op->read)
    {
        ret = fp->f_op->read(fp, buf, DLC_BUF_SIZE, &fp->f_pos);
    }

    if (ret < 0)
    {
        set_fs(old_fs);
        filp_close(fp, NULL);
        return FALSE;
    }

#if defined(CONFIG_ARM64)
    msDlcSetPMBase((MS_U64)mstar_pm_base);
#endif

    MDrv_DLC_ReadCurveInfo(buf);

    MDrv_DLC_ReadInfo(buf);

    msDlcSetCurve(_au8DlcCurveInit);

    msDlcInitEx();

    bReturn = msDlcInit(_au16DlcInit );

    set_fs(old_fs);
    filp_close(fp, NULL);
    kfree(buf);
#endif

    return bReturn;
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_ReadInfo(char *buf)
{
    MS_U16 u16BufIdx=0,u16BufIdx_2=0;
    MS_U8 u8DataIdx=0, u8RuleIdx=0, u8CntSwitch=0;
    char data_buf[DLC_DATA_BUF_SIZE]= { 0 }, rule_buf[DLC_RULE_BUF_SIZE]= { 0 };

    for(u16BufIdx=0 ; u16BufIdx<(DLC_BUF_SIZE-4) ; u16BufIdx++)
    {
        if(u8CntSwitch==0)
        {
            //The three symbols of '#@@' is start to load init value for Kernel.
            if((buf[u16BufIdx] == '#')&&(buf[u16BufIdx+1] == '@')&&(buf[u16BufIdx+2] == '@'))
                u8CntSwitch=1;

            continue;
        }

        // Get the Data value
        if(buf[u16BufIdx] == '=')
        {
            //Clear the data_buf
            for(u8DataIdx=0; u8DataIdx<(DLC_DATA_BUF_SIZE-1); u8DataIdx++)
            {
                data_buf[u8DataIdx]=0;
            }
            data_buf[DLC_DATA_BUF_SIZE-1]='\0';

            //Get the value between '=' and ';' , if the value is ' ' , can't to save.
            for(u16BufIdx_2=u16BufIdx+1,u8DataIdx=0; (buf[u16BufIdx_2]!=';')&&(u8DataIdx<DLC_DATA_BUF_SIZE); u16BufIdx_2++)
            {
                if(buf[u16BufIdx_2]!=' ')
                {
                    data_buf[u8DataIdx]=buf[u16BufIdx_2];
                    u8DataIdx++;
                }
            }
            u8CntSwitch=2;
        }

        if(u8CntSwitch==2)
        {
            // Get the Rule value after '#@'
            if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='@'))
            {
                //Clear the rule_buf
                for(u8RuleIdx=0; u8RuleIdx<(DLC_RULE_BUF_SIZE-1); u8RuleIdx++)
                {
                    rule_buf[u8RuleIdx]=0;
                }
                rule_buf[DLC_RULE_BUF_SIZE-1] = '\0';
                //Get the rule value between '#@' and ' ' .
                for(u16BufIdx_2=u16BufIdx+2,u8RuleIdx=0; (buf[u16BufIdx_2]!=' ')&&(u8RuleIdx<DLC_RULE_BUF_SIZE); u16BufIdx_2++)
                {
                    rule_buf[u8RuleIdx]=buf[u16BufIdx_2];
                    u8RuleIdx++;
                }
                u8CntSwitch=1;

                //Save the Data and rule value to _au16DlcInit .
                _au16DlcInit[simple_strtol(rule_buf, NULL, 10)] = simple_strtol(data_buf, NULL, 10);
            }
        }
        //The three symbols of '#!@' is end to load init value for Kernel .
        if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='!')&&(buf[u16BufIdx+2]=='@'))
            break;
    }
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_SetCurve(void)
{
#ifdef SUPPORT_XC_DLC
    BYTE ucTmp;

    for(ucTmp=0; ucTmp<16; ucTmp++)
    {
        _au8DlcCurveInit[ucTmp]=_stDlcInfo.u8LumaCurve[ucTmp] ;
        _au8DlcCurveInit[ucTmp+16]=_stDlcInfo.u8LumaCurve2_a[ucTmp];
        _au8DlcCurveInit[ucTmp+32]=_stDlcInfo.u8LumaCurve2_b[ucTmp] ;
    }
    msDlcSetCurve(_au8DlcCurveInit);
#endif
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init table info from utopia
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_CopyInitInfo(void)
{
#ifdef SUPPORT_XC_DLC
    BYTE ucTmp;

    for(ucTmp=0; ucTmp<16; ucTmp++)
    {
        _au8DlcCurveInit[ucTmp]=_stDlcInitInfo.au8LumaCurve[ucTmp] ;
        _au8DlcCurveInit[ucTmp+16]=_stDlcInitInfo.au8LumaCurve2_a[ucTmp];
        _au8DlcCurveInit[ucTmp+32]=_stDlcInitInfo.au8LumaCurve2_b[ucTmp] ;
    }

    for (ucTmp=0; ucTmp<17; ucTmp++)
    {
        _au8DlcCurveInit[ucTmp+48]  = _stDlcInitInfo.au8DlcHistogramLimitCurve[ucTmp];
    }

    msDlcSetCurve(_au8DlcCurveInit);

    _au16DlcInit[10]= _stDlcInitInfo.u8DlcPureImageMode; // Compare difference of max and min bright
    _au16DlcInit[11]= _stDlcInitInfo.u8DlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    _au16DlcInit[12]= _stDlcInitInfo.u8DlcAvgDelta; // n = 0 ~ 50, default value: 12
    _au16DlcInit[13]= _stDlcInitInfo.u8DlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
    _au16DlcInit[14]= _stDlcInitInfo.u8DlcFastAlphaBlending; // min 17 ~ max 32
    _au16DlcInit[15]= _stDlcInitInfo.u8DlcYAvgThresholdL; // default value: 0
    _au16DlcInit[16]= _stDlcInitInfo.u8DlcYAvgThresholdH; // default value: 128
    _au16DlcInit[17]= _stDlcInitInfo.u8DlcBLEPoint; // n = 24 ~ 64, default value: 48
    _au16DlcInit[18]= _stDlcInitInfo.u8DlcWLEPoint; // n = 24 ~ 64, default value: 48
    _au16DlcInit[19]= _stDlcInitInfo.bEnableBLE; // 1: enable; 0: disable
    _au16DlcInit[20]= _stDlcInitInfo.bEnableWLE; // 1: enable; 0: disable
    _au16DlcInit[21]= _stDlcInitInfo.u8DlcYAvgThresholdM;
    _au16DlcInit[22]= _stDlcInitInfo.u8DlcCurveMode;
    _au16DlcInit[23]= _stDlcInitInfo.u8DlcCurveModeMixAlpha;
    _au16DlcInit[24]= _stDlcInitInfo.u8DlcAlgorithmMode;
    _au16DlcInit[25]= _stDlcInitInfo.u8DlcSepPointH;
    _au16DlcInit[26]= _stDlcInitInfo.u8DlcSepPointL;
    _au16DlcInit[27]= _stDlcInitInfo.u16DlcBleStartPointTH;
    _au16DlcInit[28]= _stDlcInitInfo.u16DlcBleEndPointTH;
    _au16DlcInit[29]= _stDlcInitInfo.u8DlcCurveDiff_L_TH;
    _au16DlcInit[30]= _stDlcInitInfo.u8DlcCurveDiff_H_TH;
    _au16DlcInit[31]= _stDlcInitInfo.u16DlcBLESlopPoint_1;
    _au16DlcInit[32]= _stDlcInitInfo.u16DlcBLESlopPoint_2;
    _au16DlcInit[33]= _stDlcInitInfo.u16DlcBLESlopPoint_3;
    _au16DlcInit[34]= _stDlcInitInfo.u16DlcBLESlopPoint_4;
    _au16DlcInit[35]= _stDlcInitInfo.u16DlcBLESlopPoint_5;
    _au16DlcInit[36]= _stDlcInitInfo.u16DlcDark_BLE_Slop_Min;
    _au16DlcInit[37]= _stDlcInitInfo.u8DlcCurveDiffCoringTH;
    _au16DlcInit[38]= _stDlcInitInfo.u8DlcAlphaBlendingMin;
    _au16DlcInit[39]= _stDlcInitInfo.u8DlcAlphaBlendingMax;
    _au16DlcInit[40]= _stDlcInitInfo.u8DlcFlicker_alpha;
    _au16DlcInit[41]= _stDlcInitInfo.u8DlcYAVG_L_TH;
    _au16DlcInit[42]= _stDlcInitInfo.u8DlcYAVG_H_TH;
    _au16DlcInit[43]= _stDlcInitInfo.u8DlcDiffBase_L;
    _au16DlcInit[44]= _stDlcInitInfo.u8DlcDiffBase_M;
    _au16DlcInit[45]= _stDlcInitInfo.u8DlcDiffBase_H;
    _au16DlcInit[46]= _stDlcInitInfo.u8LMaxThreshold;
    _au16DlcInit[47]= _stDlcInitInfo.u8LMinThreshold;
    _au16DlcInit[48]= _stDlcInitInfo.u8LMaxCorrection;
    _au16DlcInit[49]= _stDlcInitInfo.u8LMinCorrection;
    _au16DlcInit[50]= _stDlcInitInfo.u8RMaxThreshold;
    _au16DlcInit[51]= _stDlcInitInfo.u8RMinThreshold;
    _au16DlcInit[52]= _stDlcInitInfo.u8RMaxCorrection;
    _au16DlcInit[53]= _stDlcInitInfo.u8RMinCorrection;
    _au16DlcInit[54]= _stDlcInitInfo.u8AllowLoseContrast;

    msDlcInit(_au16DlcInit );
#endif
}

//-------------------------------------------------------------------------------------------------
///Get the DLC init curve table from DLC.ini
///@param buf
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_ReadCurveInfo(char *buf)
{
    MS_U16 u16BufIdx=0;
    MS_U8 u8DataIdx=0, u8CntSwitch=0, u8InitCnt=0;
    char data_buf[3]= { 0 };
    data_buf[2] = '\0';

    for(u16BufIdx=0 ; u16BufIdx<(DLC_BUF_SIZE-4) ; u16BufIdx++)
    {
        if(u8CntSwitch==0)
        {
            //The two symbols of '#&' is start to load curve table for Kernel.
            if((buf[u16BufIdx] == '#')&&(buf[u16BufIdx+1] == '&'))
                u8CntSwitch=1;//The u8CntSwitch = 1 between '#&' and '#%' .

            continue;
        }

        // Get the four curves init value .
        if((buf[u16BufIdx] == '0')&&(buf[u16BufIdx+1] == 'x'))
        {
            //Clear the data_buf
            for(u8DataIdx=0 ; u8DataIdx<2 ; u8DataIdx++)
            {
                data_buf[u8DataIdx]=0;
            }
            //Get the two values after '0x' .
            data_buf[0] = buf[u16BufIdx+2];
            data_buf[1] = buf[u16BufIdx+3];

            //Save the data value to _au8DlcCurveInit .
            _au8DlcCurveInit[u8InitCnt] = simple_strtol(data_buf, NULL, 16) ;
            u8InitCnt++;
        }
        //The two symbols of '#%' is end to load curve table for Kernel .
        if((buf[u16BufIdx]=='#')&&(buf[u16BufIdx+1]=='%'))
            break;
    }
}

//-------------------------------------------------------------------------------------------
///Set Ble On Off Info
///@param bSetBleOn
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_SetBleOnOffInfo(MS_BOOL bSetBleOn )
{
#ifdef SUPPORT_XC_DLC
    msDlcSetBleOnOff(bSetBleOn);
#endif

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get Dlc C Gain
/// @param  bWindow                \b IN: which window we are going to set
/// @return @ref MS_U8            \b OUT:  Dlc C Gain
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_DLC_GetCGain( BOOL bWindow)
{
    MS_U8 u8CGain = 0;
#ifdef SUPPORT_XC_DLC
    u8CGain = msDlcGetCGain(bWindow);
#endif

    return u8CGain;
}

//-------------------------------------------------------------------------------------------------
/// Get Dlc Average Value
/// @param  bWindow                \b IN: which window we are going to set
/// @return @ref MS_U8            \b OUT:  Dlc average value
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_DLC_GetAverageValue(BOOL bWindow)
{
    MS_U8 u8Ret = 0;
#ifdef SUPPORT_XC_DLC
    if(msIsBlackVideoEnable(bWindow))
    {
        u8Ret = 0;
    }
    else
    {
        if( MAIN_WINDOW == bWindow)
        {
            u8Ret = msGetAverageLuminous();
        }
    }
#endif

    return u8Ret;
}

//-------------------------------------------------------------------------------------------------
/// Set Dlc Y Gain
/// @param  u8YGain                 \b IN: control the Y gain: Init is 0x40
/// @param  bWindow                \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_SetYGain(MS_U8 u8YGain, BOOL bWindow)
{
#ifdef SUPPORT_XC_DLC
    msDlcSetYGain(u8YGain,bWindow);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Get Dlc Y Gain
/// @param  bWindow                \b IN: which window we are going to set
/// @return @ref MS_U8            \b OUT:  Dlc Y Gain
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_DLC_GetYGain( BOOL bWindow)
{
    MS_U8 u8YGain = 0;
#ifdef SUPPORT_XC_DLC
    u8YGain =  msDlcGetYGain(bWindow);
#endif
    return u8YGain;
}

//-------------------------------------------------------------------------------------------------
/// Set Dlc C Gain
/// @param  u8CGain                 \b IN: control the C gain: Init is 0x40
/// @param  bWindow                \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MDrv_DLC_SetCGain(MS_U8 u8CGain, BOOL bWindow)
{
#ifdef SUPPORT_XC_DLC
    msDlcSetCGain(u8CGain,bWindow);
#endif
}

//-------------------------------------------------------------------------------------------
///Set Dlc Change Curve Info
///@param stDlcInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_SetChangeCurveInfo(ST_KDRV_XC_DLC_INFO *pstDlcInfo)
{
#ifdef SUPPORT_XC_DLC
    MS_U8 i;
    _stDlcInfo.bCurveChange =  pstDlcInfo->bCurveChange;

    for (i=0; i<16; i++)
    {
        _stDlcInfo.u8LumaCurve[i]     = pstDlcInfo->u8LumaCurve[i];
        _stDlcInfo.u8LumaCurve2_a[i]  = pstDlcInfo->u8LumaCurve2_a[i];
        _stDlcInfo.u8LumaCurve2_b[i]  = pstDlcInfo->u8LumaCurve2_b[i];
    }
#endif

    return TRUE;
}

//-------------------------------------------------------------------------------------------
///Set Dlc Init Info
///@param stDlcInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL KDrv_DLC_SetInitInfo(ST_KDRV_XC_DLC_INIT_INFO *pstDlcInitInfo)
{
#ifdef SUPPORT_XC_DLC
    MS_U8 i;
    _stDlcInitInfo.bInitInfoChange =  pstDlcInitInfo->bInitInfoChange;

    for (i=0; i<16; i++)
    {
        _stDlcInitInfo.au8LumaCurve[i]     = pstDlcInitInfo->au8LumaCurve[i];
        _stDlcInitInfo.au8LumaCurve2_a[i]  = pstDlcInitInfo->au8LumaCurve2_a[i];
        _stDlcInitInfo.au8LumaCurve2_b[i]  = pstDlcInitInfo->au8LumaCurve2_b[i];
    }

    for (i=0; i<17; i++)
    {
        _stDlcInitInfo.au8DlcHistogramLimitCurve[i]  = pstDlcInitInfo->au8DlcHistogramLimitCurve[i];
    }

    _stDlcInitInfo.u8DlcPureImageMode     = pstDlcInitInfo->u8DlcPureImageMode; // Compare difference of max and min bright
    _stDlcInitInfo.u8DlcLevelLimit        = pstDlcInitInfo->u8DlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    _stDlcInitInfo.u8DlcAvgDelta          = pstDlcInitInfo->u8DlcAvgDelta; // n = 0 ~ 50, default value: 12
    _stDlcInitInfo.u8DlcAvgDeltaStill     = pstDlcInitInfo->u8DlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15: enable still curve
    _stDlcInitInfo.u8DlcFastAlphaBlending = pstDlcInitInfo->u8DlcFastAlphaBlending; // min 17 ~ max 32
    _stDlcInitInfo.u8DlcYAvgThresholdL    = pstDlcInitInfo->u8DlcYAvgThresholdL; // default value: 0
    _stDlcInitInfo.u8DlcYAvgThresholdH    = pstDlcInitInfo->u8DlcYAvgThresholdH; // default value: 128
    _stDlcInitInfo.u8DlcBLEPoint          = pstDlcInitInfo->u8DlcBLEPoint; // n = 24 ~ 64, default value: 48
    _stDlcInitInfo.u8DlcWLEPoint          = pstDlcInitInfo->u8DlcWLEPoint; // n = 24 ~ 64, default value: 48
    _stDlcInitInfo.bEnableBLE             = pstDlcInitInfo->bEnableBLE; // 1: enable; 0: disable
    _stDlcInitInfo.bEnableWLE             = pstDlcInitInfo->bEnableWLE; // 1: enable; 0: disable
    _stDlcInitInfo.u8DlcYAvgThresholdM    = pstDlcInitInfo->u8DlcYAvgThresholdM;
    _stDlcInitInfo.u8DlcCurveMode         = pstDlcInitInfo->u8DlcCurveMode;
    _stDlcInitInfo.u8DlcCurveModeMixAlpha = pstDlcInitInfo->u8DlcCurveModeMixAlpha;
    _stDlcInitInfo.u8DlcAlgorithmMode     = pstDlcInitInfo->u8DlcAlgorithmMode;
    _stDlcInitInfo.u8DlcSepPointH         = pstDlcInitInfo->u8DlcSepPointH;
    _stDlcInitInfo.u8DlcSepPointL         = pstDlcInitInfo->u8DlcSepPointL;
    _stDlcInitInfo.u16DlcBleStartPointTH   = pstDlcInitInfo->u16DlcBleStartPointTH;
    _stDlcInitInfo.u16DlcBleEndPointTH     = pstDlcInitInfo->u16DlcBleEndPointTH;
    _stDlcInitInfo.u8DlcCurveDiff_L_TH    = pstDlcInitInfo->u8DlcCurveDiff_L_TH;
    _stDlcInitInfo.u8DlcCurveDiff_H_TH    = pstDlcInitInfo->u8DlcCurveDiff_H_TH;
    _stDlcInitInfo.u16DlcBLESlopPoint_1    = pstDlcInitInfo->u16DlcBLESlopPoint_1;
    _stDlcInitInfo.u16DlcBLESlopPoint_2    = pstDlcInitInfo->u16DlcBLESlopPoint_2;
    _stDlcInitInfo.u16DlcBLESlopPoint_3    = pstDlcInitInfo->u16DlcBLESlopPoint_3;
    _stDlcInitInfo.u16DlcBLESlopPoint_4    = pstDlcInitInfo->u16DlcBLESlopPoint_4;
    _stDlcInitInfo.u16DlcBLESlopPoint_5    = pstDlcInitInfo->u16DlcBLESlopPoint_5;
    _stDlcInitInfo.u16DlcDark_BLE_Slop_Min = pstDlcInitInfo->u16DlcDark_BLE_Slop_Min;
    _stDlcInitInfo.u8DlcCurveDiffCoringTH = pstDlcInitInfo->u8DlcCurveDiffCoringTH;
    _stDlcInitInfo.u8DlcAlphaBlendingMin  = pstDlcInitInfo->u8DlcAlphaBlendingMin;
    _stDlcInitInfo.u8DlcAlphaBlendingMax  = pstDlcInitInfo->u8DlcAlphaBlendingMax;
    _stDlcInitInfo.u8DlcFlicker_alpha     = pstDlcInitInfo->u8DlcFlicker_alpha;
    _stDlcInitInfo.u8DlcYAVG_L_TH         = pstDlcInitInfo->u8DlcYAVG_L_TH;
    _stDlcInitInfo.u8DlcYAVG_H_TH         = pstDlcInitInfo->u8DlcYAVG_H_TH;
    _stDlcInitInfo.u8DlcDiffBase_L        = pstDlcInitInfo->u8DlcDiffBase_L;
    _stDlcInitInfo.u8DlcDiffBase_M        = pstDlcInitInfo->u8DlcDiffBase_M;
    _stDlcInitInfo.u8DlcDiffBase_H        = pstDlcInitInfo->u8DlcDiffBase_H;
    _stDlcInitInfo.u8LMaxThreshold        = pstDlcInitInfo->u8LMaxThreshold;
    _stDlcInitInfo.u8LMinThreshold        = pstDlcInitInfo->u8LMinThreshold;
    _stDlcInitInfo.u8LMaxCorrection       = pstDlcInitInfo->u8LMaxCorrection;
    _stDlcInitInfo.u8LMinCorrection       = pstDlcInitInfo->u8LMinCorrection;
    _stDlcInitInfo.u8RMaxThreshold        = pstDlcInitInfo->u8RMaxThreshold;
    _stDlcInitInfo.u8RMinThreshold        = pstDlcInitInfo->u8RMinThreshold;
    _stDlcInitInfo.u8RMaxCorrection       = pstDlcInitInfo->u8RMaxCorrection;
    _stDlcInitInfo.u8RMinCorrection       = pstDlcInitInfo->u8RMinCorrection;
    _stDlcInitInfo.u8AllowLoseContrast    = pstDlcInitInfo->u8AllowLoseContrast;

    _bDlcInitReady = TRUE;
#endif
    return TRUE;
}
EXPORT_SYMBOL(KDrv_DLC_SetInitInfo);

MS_BOOL KDrv_DLC_SetTMOInfo(ST_KDRV_XC_TMO_INFO *pstTMOInfo)
{
#ifdef SUPPORT_XC_DLC
    MS_U16 i = 0;
    MS_U16 u16TMOSize = sizeof(ST_KDRV_XC_TMO_INFO);
    MS_U16  *_pu16TMOData = (MS_U16*)kmalloc(u16TMOSize, GFP_ATOMIC);

    _pu16TMOData[i++] = pstTMOInfo->u16SrcMinRatio;
    _pu16TMOData[i++] = pstTMOInfo->u16SrcMedRatio;
    _pu16TMOData[i++] = pstTMOInfo->u16SrcMaxRatio;
    _pu16TMOData[i++] = pstTMOInfo->u16TgtMin;
    _pu16TMOData[i++] = pstTMOInfo->u16TgtMed;
    _pu16TMOData[i++] = pstTMOInfo->u16TgtMax;
    _pu16TMOData[i++] = pstTMOInfo->u16FrontSlopeMin;
    _pu16TMOData[i++] = pstTMOInfo->u16FrontSlopeMax;
    _pu16TMOData[i++] = pstTMOInfo->u16BackSlopeMin;
    _pu16TMOData[i++] = pstTMOInfo->u16BackSlopeMax;
    _pu16TMOData[i++] = pstTMOInfo->u16SceneChangeThrd;
    _pu16TMOData[i++] = pstTMOInfo->u16SceneChangeRatioMax;
    _pu16TMOData[i++] = pstTMOInfo->u8IIRRatio;
    _pu16TMOData[i++] = pstTMOInfo->u8TMOTargetMode;
    _pu16TMOData[i++] = pstTMOInfo->u16SDRPanelGain;
    _pu16TMOData[i++] = pstTMOInfo->u16Smin;
    _pu16TMOData[i++] = pstTMOInfo->u16Smed;
    _pu16TMOData[i++] = pstTMOInfo->u16Smax;
    _pu16TMOData[i++] = pstTMOInfo->u16Tmin;
    _pu16TMOData[i++] = pstTMOInfo->u16Tmed;
    _pu16TMOData[i++] = pstTMOInfo->u16Tmax;
    _pu16TMOData[i++] = pstTMOInfo->bRefMode;
    msTMOSet(_pu16TMOData, u16TMOSize);

    kfree((void*)_pu16TMOData);

#endif
    return TRUE;
}
EXPORT_SYMBOL(KDrv_DLC_SetTMOInfo);

//-------------------------------------------------------------------------------------------
///Set Dlc On Off Info
///@param bSetDlcOn
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_SetOnOffInfo(MS_BOOL bSetDlcOn )
{
    _bSetDlcOn =  bSetDlcOn;

    return TRUE;
}

//-------------------------------------------------------------------------------------------
///Set BLE point Change Info
///@param stBleInfo
///@return FALSE :fail
//---------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_SetBlePointChangeInfo(ST_KDRV_XC_DLC_BLE_INFO stBleInfo)
{
#ifdef SUPPORT_XC_DLC
    MS_U8 i;

    _stBleInfo.bBLEPointChange =  stBleInfo.bBLEPointChange;

    for (i=0; i<6; i++)
    {
        _stBleInfo.u16BLESlopPoint[i] = stBleInfo.u16BLESlopPoint[i];
    }
#endif

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get histogram data
/// @param pu16Histogram                \b OUT: the value of histogram
/// @return FALSE :fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_DLC_GetHistogram32Info(MS_U16 *pu16Histogram)
{
    MS_BOOL bRet = FALSE;
#ifdef SUPPORT_XC_DLC
    bRet = msDlcGetHistogram32Info(pu16Histogram);
#endif

    return bRet;
}
void MDrv_DLC_ProcessIRQ(void)
{
#ifdef SUPPORT_XC_DLC
    if(_stDlcInfo.bCurveChange == TRUE)
    {
        MDrv_DLC_SetCurve();
        _stDlcInfo.bCurveChange = FALSE;
    }

    if(_stDlcInitInfo.bInitInfoChange == TRUE)
    {
        MDrv_DLC_CopyInitInfo();
        _stDlcInitInfo.bInitInfoChange = FALSE;
    }

    if(_stBleInfo.bBLEPointChange == TRUE)
    {
        MDrv_DLC_SetBlePoint();
        _stBleInfo.bBLEPointChange = FALSE;
    }

    //DLC Bank 0x1A . Check for MSTV_Tool debug.
    if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
    {
        //DLC Loop Flage for MSTV_Tool debug
        MApi_GFLIP_XC_W2BYTEMSK(REG_SC_BK30_02_L, 0, 0x100);
    }

    //DLC Bank 0x1A. Check for MSTV_Tool debug.
    if((MApi_GFLIP_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0xFF))==0x1A)
    {
        msDLCMSTVToolReadWriteValue();
    }

    //The new DLC algorithm flow, only support main windows.
    if((_bHistogramReady == TRUE) && (_bDlcInitReady == TRUE) && (_bSetDlcOn ==  TRUE))
    {
        msDlcHandler(MAIN_WINDOW);
        _bHistogramReady = FALSE;
    }

    if(_bDlcInitReady == FALSE)
    {
        XC_DCL_DBG(printk("\n Error in  [Kernel DLC][ %s  , %d ]  DLC Init is Fail  !!!\n",__FUNCTION__,__LINE__));
    }
    msTmoHandler(MAIN_WINDOW);

    if((_bHistogramReady == FALSE) && (_bDlcInitReady == TRUE))
    {
        if(msGetHistogramHandler(MAIN_WINDOW) == TRUE)
        {
            _bHistogramReady = TRUE;
        }
    }
#endif
    return TRUE;
}
#endif //_MDRV_DLC_C_
