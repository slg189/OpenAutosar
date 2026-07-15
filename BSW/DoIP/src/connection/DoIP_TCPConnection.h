

#ifndef DOIP_TCPCONNECTION_H
#define DOIP_TCPCONNECTION_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern uint8 DoIP_NumOfActiveTCPConnections_u8;
extern DoIP_Type_TCPConnection DoIP_TCPConnection_ast[DOIP_CFG_MAX_NUM_OF_TCP_CONN];

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* General functionality */
void DoIP_TCPConnection_Create(DoIP_ConnectionType_tst* soCon_pst);
void DoIP_TCPConnection_Init(void);
void DoIP_TCPConnection_MainFunction(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_TCPConnection_HandleMessages(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_TCPConnection_HandleTimer(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_TCPConnection_Open(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_TCPConnection_Close(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_TCPConnection_SetCloseNeeded(const DoIP_Type_TCPConnection* tcpCon_pcst);
void DoIP_TCPConnection_SetResetNeeded(const DoIP_Type_TCPConnection* tcpCon_pcst);
boolean DoIP_TCPConnection_SendMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint16 channelIdx_u16, const uint8* msg_pu8, boolean isDiagMessage);
boolean DoIP_TCPConnection_GetTCPConnForChannelIdx(uint16 channelIdx_u16, DoIP_Type_TCPConnection** tcpCon_ppst);
LOCAL_INLINE DoIP_Type_TCPConnection* DoIP_TCPConnection_Get(uint16 tcpIdx_u16);
LOCAL_INLINE void DoIP_TCPConnection_SetGeneralInactivity(DoIP_Type_TCPConnection* tcpCon_pst);

LOCAL_INLINE DoIP_Type_TCPConnection* DoIP_TCPConnection_Get(uint16 tcpIdx_u16)
{
    return &DoIP_TCPConnection_ast[tcpIdx_u16];
}

LOCAL_INLINE void DoIP_TCPConnection_SetGeneralInactivity(DoIP_Type_TCPConnection* tcpCon_pst)
{
    tcpCon_pst->timerInactivity_u32 = DOIP_CFG_GENERAL_INACTIVITY_TIME_CYCLES;
}
#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
