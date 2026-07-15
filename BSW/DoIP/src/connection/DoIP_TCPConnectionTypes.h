

#ifndef DOIP_TCPCONNECTIONTYPES_H
#define DOIP_TCPCONNECTIONTYPES_H

#include "DoIP_Prv_Types.h"

typedef enum
{
    DOIP_TCP_TX_READY_E,
    DOIP_TCP_TX_BLOCKED_E ,
    DOIP_TCP_TX_SENDING_E,
    DOIP_TCP_TX_WAIT4CONFIRMATION_E
	
} DoIP_Enum_TcpTxState_ten;

typedef enum
{
    DOIP_CHANNEL_NEW_MESSAGE_E,
    DOIP_CHANNEL_MULTIFRAME_MESSAGE_E,
    DOIP_CHANNEL_CONTINUE_FRAGMENT_E,
    DOIP_CHANNEL_DISCARD_MESSAGE_E
	
} DoIP_Enum_ChannelState_ten;

typedef struct
{
    uint8 genericNackBuffer_au8[DOIP_HEADER_SIZE + 2u + 2u + 1u];             /* buffer to store the generic NACK. The last byte is for the generic nack response code */
	boolean genericNackAvailable_b;                                           /* identifier for the TX side, that a generic NACK is available and has to proceed with higher prio */

}DoIP_TxGenericNACKBuffer_tst;

typedef struct
{
    DoIP_Enum_TcpTxState_ten nackState_en;
    uint8 txDiagNACKBuffer_au8[DOIP_CFG_TCP_BUFFER_SIZE];                       /* Buffer to store invalid diag messages */
	
}DoIP_TxDiagNACKBuffer_tst;

typedef struct
{
    uint8 RxFragHdrBuffer_au8[DOIP_HEADER_SIZE];                                /* To store the incoming generic header packet, it could be fragmented */
    uint8 RxFragDiagMsg_au8[DOIP_MSG_SIZE_PAYLOAD_LENGTH];                      /* store here diag message fragment, max 4 bytes as payload */
	uint16 HeaderFragCurrentSize_u16;                                             /* current size of header fregment */
    uint16 DiagMsgFragCurrentSize_u16;                                            /* current size of diag message fragment (this means, that SA or TA is not completly received) */

}DoIP_RxFragHdrBufferInfo_tst;

typedef struct
{
    PduLengthType lastKnownBufferSize_uo;                                     /* number of bytes all ready copied */
    uint32 totalPayloadLength_u32;                                              /* total payloadLength_u32 */
    uint16 payloadType_u16;                                                     /* payloadType_u16 of the diag message */
    uint16 channelIdx_u16;                                                    /* index of target address_u16. with this index you will get access to TX and RX channel information */
    DoIP_Enum_TcpTxState_ten tcpState_en;                                         /* state_en of tcp TX connection to SoAd*/
    DoIP_Enum_ChannelState_ten channelState_en;                                     /* state of the TX channel to the PduR */
    DoIP_TxGenericNACKBuffer_tst genericNACKBuffer_st;                             /* runtime structure, which handles the generic NACK messages  */
    DoIP_TxDiagNACKBuffer_tst txDiagACKBuffer_st;                                   /* runtime structure, which handles the diag NACK messages */
	
} DoIP_Type_TCPConnectionTXStatemachine;

typedef struct
{
    PduLengthType lastKnownBufferSize_uo;                                     /* number of bytes all ready copied */
	uint32 totalPayloadLength_u32;                                                  /* total payloadLength_u32 */
    uint16 payloadType_u16;                                                     /* payloadType_u16 of the diag message */
    uint32 payloadLengthCounter_u32;                                            /* Counter for how many bytes of the total payloadLength_u32 of one diag message are already send to PduR */
    uint16 ProcessedLength_u16;                                                 /* it is possible to receive multiple messages per one segment. In that case, this variable counts the relative amount of sent data of the complete SDU */
    uint16 channelIdx_u16;                                                    /* index of target address_u16. with this index you will get access to TX and RX channel information */
    DoIP_Enum_ChannelState_ten channelState_en;                                     /* state of the RX channel to the PduR */
	
} DoIP_Type_TCPConnectionRXStatemachine;

typedef struct
{
    DoIP_ConnectionType_tst* soCon_pst;                                               /* runtime structure connection activities on the connection level */
    uint32 timerInactivity_u32;                                                 /* inactivity timer */
    DoIP_Type_AliveCheck_tst aliveCheck_st;                                        /* runtime structure for alive check activities*/
    DoIP_RoutingActivationType_tst ra_st;                                          /* runtime structure for routing activation activities */
    DoIP_Type_TCPConnectionTXStatemachine TX_st;                               /* runtime structure for transmit tcp messages */
    DoIP_Type_TCPConnectionRXStatemachine RX_st;                               /* runtime structure for receive tcp messages */
    DoIP_RxFragHdrBufferInfo_tst  RxFragHdrBuffer_st;                               /* runtime structure to handle fragmented tcp messages */
	
} DoIP_Type_TCPConnection;

#endif
