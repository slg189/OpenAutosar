

#ifndef DOIP_DIAGNOSTICMESSAGE_H
#define DOIP_DIAGNOSTICMESSAGE_H

#include "DoIP_Prv_Types.h"

#define DOIP_DM_ACKCODE_SUCCESS             0x00u

#define DOIP_DM_NACKCODE_INVALID_SA         0x02u
#define DOIP_DM_NACKCODE_UNKNOWN_TA         0x03u
#define DOIP_DM_NACKCODE_MSG_TOO_LARGE      0x04u
#define DOIP_DM_NACKCODE_OUT_OF_MEM         0x05u
#define DOIP_DM_NACKCODE_TA_UNREACHABLE     0x06u
//#define DOIP_DM_NACKCODE_UNKNOWN_NET      0x07u // unsupported
#define DOIP_DM_NACKCODE_TP_ERROR           0x08u
#define DOIP_DM_NACKCODE_UNDEFINED          0xFFu

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"


boolean DoIP_DiagnosticMessageCheckSaTaAtConfiguration(uint16 sa_u16, uint16 ta_u16, const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst);
void DoIP_DiagnosticMessage_AcknowledgeTxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst);
void DoIP_DiagnosticMessage_TxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst, Std_ReturnType result_u8);
void DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint8 respCode_u8, uint16 channel_u16, uint8* ackBuffer_pu8);

BufReq_ReturnType DoIP_DiagnosticMessage_MessageHandler( DoIP_Type_TCPConnection*   tcpCon_pst,
                                                         const uint8*               header_pu8,
                                                         uint8*                     payload_pu8,
                                                         PduLengthType              PayloadLength_uo,
                                                         PduLengthType*             bufferSize_puo);

void DoIP_DiagnosticMessage_DiagnosticMessageHandler(DoIP_Type_TCPConnection* tcpCon_pst, uint8* payload_pu8, PduLengthType PayloadLength_u16, PduLengthType* bufferSize_puo);
BufReq_ReturnType DoIP_DiagnosticMessage_ContinuePreviousMessage(DoIP_Type_TCPConnection *tcpCon_pst, const PduInfoType* pduInfo_pcst, PduLengthType* bufferSize_puo);
void DoIP_DiagnosticMessage_UpdateAcknowledgeMessageData(const uint8* sdu_pcu8, PduLengthType pdurSduLength_uo, const DoIP_Type_TCPConnection *tcpCon_pst);
#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
