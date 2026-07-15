

#ifndef DOIP_ALIVECHECK_H
#define DOIP_ALIVECHECK_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_AliveCheck_MainFunction(DoIP_Type_TCPConnection* tcpCon_pst);
BufReq_ReturnType DoIP_AliveCheck_ReceivedNewMessage(DoIP_Type_TCPConnection* tcpCon_pst, const uint8* header_pu8, const uint8* payload_pu8);
void DoIP_AliveCheck_TxConfirmation(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_AliveCheck_RxHandle(DoIP_Type_TCPConnection* tcpCon_pst);
void DoIP_AliveCheck_Request(DoIP_Type_TCPConnection* tcpCon_pst);
boolean DoIP_AliveCheck_IsIdle(const DoIP_Type_TCPConnection* tcpCon_pcst);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
