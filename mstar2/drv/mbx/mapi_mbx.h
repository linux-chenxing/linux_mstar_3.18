
#ifndef _API_MBX_H
#define _API_MBX_H

MBX_Result  MApi_MBX_SendMsg(MBX_Msg *pMsg);
MBX_Result  MApi_MBX_RecvMsg(MBX_Class eTargetClass, MBX_Msg *pMsg, MS_U32 u32WaitMillSecs, MS_U32 u32Flag);
MBX_Result  MApi_MBX_RegisterMSG(MBX_Class eMsgClass, MS_U16 u16MsgQueueSize);
MBX_Result  MApi_MBX_QueryDynamicClass(MBX_ROLE_ID eRoleID, char *name, MS_U8 *pmbx_class);
#ifdef CONFIG_MP_R2_STR_ENABLE
void        MApi_MBX_NotifyTeetoSuspend(unsigned long u64TEESTRBOOTFLAG);
#endif

#endif

