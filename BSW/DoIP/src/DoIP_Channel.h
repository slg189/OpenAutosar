

#ifndef DOIP_CHANNEL_H
#define DOIP_CHANNEL_H

#include "DoIP_Prv_Types.h"

/* DOIP_HEADER + SA + TA */
#define DOIP_CHANNEL_BUFFERSIZE             (DOIP_HEADER_SIZE + 2u + 2u)
typedef enum
{
    DOIP_CHANNEL_TX_IDLE_E = 0u,
    DOIP_CHANNEL_TX_LOCK_E,
    DOIP_CHANNEL_TX_READY2SEND_E,
    DOIP_CHANNEL_TX_SENDING_E

}DoIP_Enum_ChannelTransmitState_ten;

typedef struct
{
    DoIP_Enum_ChannelTransmitState_ten channelState_en;         /* state of the TX channel from PduR toSoAd */
    DoIP_Enum_ChannelTransmitState_ten channelACKState_en;      /* state of the TX channel  for ACK/NACK from PduR toSoAd */
    uint8 ackBuffer_au8[DOIP_CFG_TCP_BUFFER_SIZE];              /* buffer to store the ACK from incoming rx message  */
    uint8 txBuffer_au8[DOIP_CHANNEL_BUFFERSIZE];                /* buffer to store the incoming Tx Request from target address_u16 */

} DoIP_Type_ChannelTransmit_tst;

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern DoIP_Type_ChannelTransmit_tst DoIP_Channel_ast[DOIP_CFG_CHANNEL_ARRAY_SIZE];

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"
Std_ReturnType DoIP_Channel_GetChannelIdxForACKBufferReserve(DoIP_Type_TCPConnection* tcpCon_pst, uint16* channelIdx_pu16);
void DoIP_Channel_PrepareDiagnosticMessageHeader(uint16 channelIdx_u16, const PduInfoType* pduInfoType_pcst);
void DoIP_Channel_Release(uint16 channelIdx_u16, Std_ReturnType result_u8);
void DoIP_Channel_ReleaseAllChannelsForTester(const DoIP_Cfg_TesterType_tst* tester_pcst, Std_ReturnType result_u8);
void DoIP_Channel_SendMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint16 channelIdx_u16);
boolean DoIP_Channel_NewMessage(uint16 channelIdx_u16, const PduInfoType* pduInfoType_pcst);

boolean DoIP_Channel_GetChannelIdxForRxPduId(PduIdType pduId_uo, uint16* channelIdx_pu16);
boolean DoIP_Channel_GetChannelIdxForTxPduId(PduIdType pduId_uo, uint16* channelIdx_pu16);

boolean DoIP_Channel_GetPduRTxPduId(uint16 channelIdx_u16, PduIdType* pduId_puo);
boolean DoIP_Channel_GetPduRRxPduId(uint16 channelIdx_u16, PduIdType* pduId_puo);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

