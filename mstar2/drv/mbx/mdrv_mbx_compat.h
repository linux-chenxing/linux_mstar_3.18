#ifndef _DRV_MBX_COMPAT_H
#define _DRV_MBX_COMPAT_H

#ifdef _DRV_MBX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE int mdrv_mbx_sendmsg_compat_get_ion_allaction_data(COMPAT_MS_MBX_SEND_MSG __user *data32, MS_MBX_SEND_MSG __user *data);
INTERFACE int mdrv_mbx_sendmsg_compat_put_ion_allaction_data(COMPAT_MS_MBX_SEND_MSG __user *data32, MS_MBX_SEND_MSG __user *data);
INTERFACE int mdrv_mbx_recvmsg_compat_get_ion_allaction_data(COMPAT_MS_MBX_RECV_MSG __user *data32, MS_MBX_RECV_MSG __user *data);
INTERFACE int mdrv_mbx_recvmsg_compat_put_ion_allaction_data(COMPAT_MS_MBX_RECV_MSG __user *data32, MS_MBX_RECV_MSG __user *data);
INTERFACE int mdrv_mbx_init_compat_get_ion_allaction_data(COMPAT_MS_MBX_INIT_INFO __user *data32, MS_MBX_INIT_INFO __user *data);
INTERFACE int mdrv_mbx_init_compat_put_ion_allaction_data(COMPAT_MS_MBX_INIT_INFO __user *data32, MS_MBX_INIT_INFO __user *data);
INTERFACE int mdrv_mbx_register_msg_compat_get_ion_allaction_data(COMPAT_MS_MBX_REGISTER_MSG __user *data32, MS_MBX_REGISTER_MSG __user *data);
INTERFACE int mdrv_mbx_register_msg_compat_put_ion_allaction_data(COMPAT_MS_MBX_REGISTER_MSG __user *data32, MS_MBX_REGISTER_MSG __user *data);
INTERFACE int mdrv_mbx_get_msgqstatus_compat_get_ion_allaction_data(COMPAT_MS_MBX_GET_MSGQSTATUS __user *data32, MS_MBX_GET_MSGQSTATUS __user *data);
INTERFACE int mdrv_mbx_get_msgqstatus_compat_put_ion_allaction_data(COMPAT_MS_MBX_GET_MSGQSTATUS __user *data32, MS_MBX_GET_MSGQSTATUS __user *data);
INTERFACE int mdrv_mbx_crosync_info_compat_get_ion_allaction_data(COMPAT_MS_MBX_CPROSYNC_INFORMATION __user *data32, MS_MBX_CPROSYNC_INFORMATION __user *data);
INTERFACE int mdrv_mbx_crosync_info_compat_put_ion_allaction_data(COMPAT_MS_MBX_CPROSYNC_INFORMATION __user *data32, MS_MBX_CPROSYNC_INFORMATION __user *data);
INTERFACE int mdrv_mbx_get_drvstatus_compat_get_ion_allaction_data(COMPAT_MS_MBX_GET_DRVSTATUS __user *data32, MS_MBX_GET_DRVSTATUS __user *data);
INTERFACE int mdrv_mbx_get_drvstatus_compat_put_ion_allaction_data(COMPAT_MS_MBX_GET_DRVSTATUS __user *data32, MS_MBX_GET_DRVSTATUS __user *data);


#endif