////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

//! @file mdrv_rqct_st.h
//! @author MStar Semiconductior Inc.
//! @brief VHE Driver IOCTL User's Interface.
//! \ingroup rqct_group VHE driver
//! @{
#ifndef _MDRV_RQCT_ST_H_
#define _MDRV_RQCT_ST_H_

#define RQCTIF_MAJ              1   //!< major version: Major number of driver-I/F version.
#define RQCTIF_MIN              1   //!< minor version: Minor number of driver-I/F version.
#define RQCTIF_EXT              0   //!< extended code: Extended number of version. It should increase when "mdrv_rqct_io.h/mdrv_rqct_st.h" changed.

//! RQCT Interface version number.
#define RQCTIF_VERSION_ID       ((RQCTIF_MAJ<<22)|(RQCTIF_MIN<<12)|(RQCTIF_EXT))
//! Acquire version number.
#define RQCTIF_GET_VER(v)       (((v)>>12))
//! Acquire major version number.
#define RQCTIF_GET_MJR(v)       (((v)>>22)&0x3FF)
//! Acquire minor version number.
#define RQCTIF_GET_MNR(v)       (((v)>>12)&0x3FF)
//! Acquire extended number.
#define RQCTIF_GET_EXT(v)       (((v)>> 0)&0xFFF)

//! rqct-method : support 3 types - CQP, CBR, VBR.
enum rqct_method {
    RQCT_METHOD_CQP=0,  //!< constant QP.
    RQCT_METHOD_CBR,    //!< constant bitrate.
    RQCT_METHOD_VBR,    //!< variable bitrate.
};

//! rqct_conf is used to apply/query rq-configs during streaming period.
typedef union rqct_conf {
    //! rqct-config type.
    enum rqct_conf_e {
    RQCT_CONF_SEQ=0,    //!< set sequence rate-control.
    RQCT_CONF_DQP,      //!< set delta-qp between I/P.
    RQCT_CONF_QPR,      //!< set range-qp.
    RQCT_CONF_LOG,      //!< turn on/off rqct log message.
    RQCT_CONF_END,      //!< endof rqct-conf-enum.
    } type;             //!< indicating config. type.

    //! set rqct seq. config.
    struct _seq {
    enum rqct_conf_e    i_type;     //!< i_type MUST be RQCT_CFG_SEQ.
    enum rqct_method    i_method;   //!< indicating rqct-method.
    int                 i_period;   //!< ip-period.
    int                 i_leadqp;   //!< leadqp.
    int                 i_btrate;   //!< btrate.
    } seq;          //!< rqct configs of seq. setting.

    //! set rqct dqp. config.
    struct _dqp {
    enum rqct_conf_e    i_type;     //!< i_type MUST be RQCT_CFG_DQP.
    int                 i_dqp;      //!< dif-qp between I/P.
    } dqp;          //!< rqct configs of dqp. setting.

    //! set rqct qpr. config.
    struct _qpr {
    enum rqct_conf_e    i_type;     //!< i_type MUST be RQCT_CFG_QPR.
    int                 i_upperq;   //!< upperq.
    int                 i_lowerq;   //!< lowerq.
    } qpr;          //!< rqct configs of dqp. setting.

    //! set rqct log. config.
    struct _log {
    enum rqct_conf_e    i_type;     //!< i_type MUST be RQCT_CFG_LOG.
    int                 b_logm;     //!< switch of log-message.
    } log;          //!< rqct configs of dqp. setting.

} rqct_conf;

#endif//_MDRV_RQCT_ST_H_
//! @}
