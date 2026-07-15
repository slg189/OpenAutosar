

#ifndef DOIP_GENERICACKNOWLEDGE_H
#define DOIP_GENERICACKNOWLEDGE_H

#include "DoIP_Prv_Types.h"

#define DOIP_GA_RESPCODE_INVALID_PROTOCOL               0x00u
#define DOIP_GA_RESPCODE_PAYLOAD_TYPE_UNSUPPORTED       0x01u
#define DOIP_GA_RESPCODE_EXCEEDS_MAX_REQ_BYTES          0x02u
//#define DOIP_GA_RESPCODE_EXCEEDS_DOIP_BUFFER            0x03u // unsupported
#define DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE    0x04u

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

Std_ReturnType DoIP_GenericAcknowledge_CheckVersionInfo(const DoIP_HeaderFields_tst* HeaderFields_pst);
Std_ReturnType DoIP_GenericAcknowledge_CheckPayloadType(uint16 payloadType_u16, boolean isTP_b);
Std_ReturnType DoIP_GenericAcknowledge_CheckPayloadLength(const DoIP_HeaderFields_tst* HeaderFields_pst);
Std_ReturnType DoIP_GenericAcknowledge_ConnectionNotRegisteredCheckPayloadLength(const DoIP_Type_TCPConnection* tcpCon_pst, const DoIP_HeaderFields_tst* HeaderFields_st, boolean* isNACKset, uint8* respCode_pu8);
Std_ReturnType DoIP_GenericAcknowledge_TCPHeaderHandler(const DoIP_Type_TCPConnection*   tcpCon_pst, const uint8* header_pu8, boolean* isNACKset_b, uint8* respCode_pu8);
Std_ReturnType DoIP_GenericAcknowledge_UDPHeaderHandler(const uint8* header_pu8, uint8* respCode_pu8);
void DoIP_GenericAcknowledge_PrepareResponse(uint8* buffer_pu8, uint8 responseCode_u8);
void DoIP_GenericAcknowledge_TpTxConfirmation(DoIP_Type_TCPConnection* tcpCon_pst);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

