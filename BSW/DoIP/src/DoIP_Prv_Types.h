

#ifndef DOIP_PRV_TYPES_H
#define DOIP_PRV_TYPES_H

#include "DoIP_Cfg.h"
#include "DoIP_Prv_Cfg_Types.h"

//msg header structure
#define DOIP_MSG_POS_PROTOCOL_VERSION           0u
#define DOIP_MSG_SIZE_PROTOCOL_VERSION          1u
#define DOIP_MSG_POS_INV_PROTOCOL_VERSION       1u//(DOIP_MSG_POS_PROTOCOL_VERSION+DOIP_MSG_SIZE_PROTOCOL_VERSION)
#define DOIP_MSG_SIZE_INV_PROTOCOL_VERSION      1u
#define DOIP_MSG_POS_PAYLOAD_TYPE               2u//(DOIP_MSG_POS_INV_PROTOCOL_VERSION+DOIP_MSG_SIZE_INV_PROTOCOL_VERSION)
#define DOIP_MSG_SIZE_PAYLOAD_TYPE              2u
#define DOIP_MSG_POS_PAYLOAD_LENGTH             4u//(DOIP_MSG_POS_PAYLOAD_TYPE+DOIP_MSG_SIZE_PAYLOAD_TYPE)
#define DOIP_MSG_SIZE_PAYLOAD_LENGTH            4u
#define DOIP_MSG_POS_CONTENT                    8u//(DOIP_MSG_POS_PAYLOAD_LENGTH+DOIP_MSG_SIZE_PAYLOAD_LENGTH)

#define DOIP_HEADER_SIZE                        DOIP_MSG_POS_CONTENT

//payload types
#define DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK       0x0000u
#define DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ            0x0001u
#define DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ        0x0002u
#define DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ        0x0003u
#define DOIP_PAYLOAD_TYPE_VEHICLE_ID_RESP           0x0004u
#define DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_REQ    0x0005u
#define DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_RESP   0x0006u
#define DOIP_PAYLOAD_TYPE_ALIVE_CHECK_REQ           0x0007u
#define DOIP_PAYLOAD_TYPE_ALIVE_CHECK_RESP          0x0008u
#define DOIP_PAYLOAD_TYPE_ENTITY_STATUS_REQ         0x4001u
#define DOIP_PAYLOAD_TYPE_ENTITY_STATUS_RESP        0x4002u
#define DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_REQ        0x4003u
#define DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_RESP       0x4004u
#define DOIP_PAYLOAD_TYPE_DIAG_MSG                  0x8001u
#define DOIP_PAYLOAD_TYPE_DIAG_MSG_ACK              0x8002u
#define DOIP_PAYLOAD_TYPE_DIAG_MSG_NACK             0x8003u

//payload length
#define DOIP_PAYLOAD_LENGTH_VEHICLE_ID_REQ              0x00000000u
#define DOIP_PAYLOAD_LENGTH_VEHICLE_ID_EID_REQ          0x00000006u
#define DOIP_PAYLOAD_LENGTH_VEHICLE_ID_VIN_REQ          0x00000011u
#if ( DOIP_CFG_UseVehicleIdentificationSyncStatus != FALSE )
#define DOIP_PAYLOAD_LENGTH_VEHICLE_ID_RESP             0x00000021u
#else
#define DOIP_PAYLOAD_LENGTH_VEHICLE_ID_RESP             0x00000020u
#endif
#define DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_REQ_7    0x00000007u
#define DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_REQ_11   0x0000000Bu
#define DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_RESP_9   0x00000009u
#define DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_RESP_13  0x0000000Du
#define DOIP_PAYLOAD_LENGTH_ALIVE_CHECK_RESP            0x00000002u
#define DOIP_PAYLOAD_LENGTH_ALIVE_CHECK_REQ             0x00000000u
#define DOIP_PAYLOAD_LENGTH_ENTITY_STATUS_REQ           0x00000000u
#if ( DOIP_CFG_EntityStatusMaxByteFieldUse != FALSE )
#define DOIP_PAYLOAD_LENGTH_ENTITY_STATUS_RESP          0x00000007u
#else
#define DOIP_PAYLOAD_LENGTH_ENTITY_STATUS_RESP          0x00000003u
#endif
#define DOIP_PAYLOAD_LENGTH_DIAG_POWERMODE_REQ          0x00000000u
#define DOIP_PAYLOAD_LENGTH_DIAG_POWERMODE_RESP         0x00000001u
#define DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN                0x00000004u
#define DOIP_PAYLOAD_LENGTH_GENERIC_HEADER_NACK         0x00000001u

//negative acknowledge codes
#define DOIP_NACK_PROTOCOL_INFO_INCORRECT   0x00u
#define DOIP_NACK_UNKNOWN_PAYLOAD_TYPE      0x01u
#define DOIP_NACK_MSG_TOO_LARGE             0x02u
#define DOIP_NACK_OUT_OF_MEMORY             0x03u
#define DOIP_NACK_INVALID_PAYLOAD_LENGTH    0x04u

//protocol version
#define DOIP_PROTOCOL_VERSION                0x02u
#define DOIP_PROTOCOL_VERSION_INV            0xFDu//(~DOIP_PROTOCOL_VERSION)
#define DOIP_PROTOCOL_VERSION_DEFAULT        0xFFu
#define DOIP_PROTOCOL_VERSION_DEFAULT_INV    0x00u//(~DOIP_PROTOCOL_VERSION_DEFAULT)

//further action
#define DOIP_FURTHER_ACTION_NOT_REQUIRED                    0x00u
#define DOIP_FURTHER_ACTION_ROUTING_ACTIVATION_REQUIRED     0x10u

/* VIN & GID synchronized */
#define DOIP_VIN_GID_SYNCHRONIZED        0x00u
#define DOIP_VIN_GID_NOT_SYNCHRONIZED    0x10u

/* VIN & GID sync state */
#define DOIP_GIDSYNC_IDLE               0x00u
#define DOIP_GIDSYNC_PENDING            0x01u
#define DOIP_GIDSYNC_SUCCESS            0x02u
typedef uint8 DoIP_Type_GIDSyncState_tu8;


//UDP port range acc. to ISO13400-2 [DoIP-135]
#define DOIP_UDP_PORT_MIN           0xC000u
#define DOIP_UDP_PORT_MAX           0xFFFFu

#define DOIP_UDP_PORT_DISCOVERY     13400u //0x3458
#define DOIP_TCP_PORT_DATA          13400u

#define DOIP_GATEWAY                0x00u
#define DOIP_NODE                   0x01u
/* Position of DoIP version no */
#define DoIP_PRV_PROTOCOL_VERSION_POS               0x00u

/* Position of DoIP Inverse version no */
#define DoIP_PRV_INV_PROTOCOL_VERSION_POS           0x01u

/* Position of DoIP Payload Type */
#define DoIP_PRV_PAYLOAD_TYPE_POS                   0x02u

/* Position of DoIP Payload Type */
#define DoIP_PRV_PAYLOAD_LENGTH_POS                 0x04u

/* Position of source address in DoIP Diagnostic msg in payload */
#define DoIP_PRV_DIAG_MSG_SRC_ADDR_IN_PAYLOAD_POS   0x00u

/* Position of target address in DoIP Diagnostic msg in payload */
#define DoIP_PRV_DIAG_MSG_TRG_ADDR_IN_PAYLOAD_POS   0x02u

/* Read 32 bits at position */
#define DoIP_PRV_READ_32BITS_AT_POS( BaseAddress, BytePos )                                    \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) ) ) << 24u )      |                    \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) + 1u ) ) << 16u )  |                   \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) + 2u ) ) << 8u )   |                   \
        (uint32)( * ( (BaseAddress) + (BytePos) + 3u ) )                                         \

/* Read 24 bits at position */
#define DoIP_PRV_READ_24BITS_AT_POS( BaseAddress, BytePos )                                    \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos)  ) ) << 16u )  |                       \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) + 1u ) ) << 8u )   |                   \
        (uint32)( * ( (BaseAddress) + (BytePos) + 2u ) )                                         \

/* Read 16 bits at position */
#define DoIP_PRV_READ_16BITS_AT_POS( BaseAddress, BytePos )                                    \
        (uint16)( (uint16)( * ( (BaseAddress) + (BytePos) ) ) << 8u )       |                    \
        (uint16)( * ( (BaseAddress) + (BytePos) + 1u ) )                                         \

/* Read 8 bits at position */
#define DoIP_PRV_READ_8BITS_AT_POS( BaseAddress, BytePos )                                     \
        (uint8)( * ( (BaseAddress) + (BytePos) ) )                                               \

/* Read 1 bit at position */
#define DoIP_PRV_READ_BIT_AT_POS( BaseAddress, BytePos, BitPos )                               \
        ( (uint8)( * ( (BaseAddress) + (BytePos) ) ) & ( 0x01u << (BitPos) ) ) >> (BitPos)       \


/* Macro for writing at given address */

/* Write 32 bits at position */
#define DoIP_PRV_WRITE_32BITS_AT_POS( BaseAddress, BytePos, Value )                            \
        ( * ( (BaseAddress) + (BytePos) ) )      = ( (uint8)( (Value) >> 24u ) );                     \
        ( * ( (BaseAddress) + (BytePos) + 1u ) ) = ( (uint8)( (Value) >> 16u ) );                \
        ( * ( (BaseAddress) + (BytePos) + 2u ) ) = ( (uint8)( (Value) >> 8u ) );                 \
        ( * ( (BaseAddress) + (BytePos) + 3u ) ) = ( (uint8)( (Value) ) );                       \

/* Write 24 bits at position */
#define DoIP_PRV_WRITE_24BITS_AT_POS( BaseAddress, BytePos, Value )                            \
        ( * ( (BaseAddress) + (BytePos) ) )      = ( (uint8)( (Value) >> 16u ) );                     \
        ( * ( (BaseAddress) + (BytePos) + 1u ) ) = ( (uint8)( (Value) >> 8u ) );                 \
        ( * ( (BaseAddress) + (BytePos) + 2u ) ) = ( (uint8)( (Value) ) );                       \


/* Write 16 bits at position */
#define DoIP_PRV_WRITE_16BITS_AT_POS( BaseAddress, BytePos, Value )                            \
        ( * ( (BaseAddress) + (BytePos) ) )      = ( (uint8)( (Value) >> (uint8)8u ) );               \
        ( * ( (BaseAddress) + (BytePos) + 1u ) ) = ( (uint8)( (Value) ) );                       \

/* Write 8 bits at position */
#define DoIP_PRV_WRITE_8BITS_AT_POS( BaseAddress, BytePos, Value )                             \
        ( * ( (BaseAddress) + (BytePos) ) ) = ( (uint8)( (Value) ) );

/* Communication statemachine */
typedef enum
{
    DOIP_COM_IDLE_E = 0u,
    DOIP_COM_RECEIVED_E,
    DOIP_COM_PROCESSING_E,
    DOIP_COM_READY2SEND_E,
    DOIP_COM_WAIT4CONFIRMATION_E,
    DOIP_COM_FAILED_E
	
} DoIP_ComStateType_ten;

typedef struct
{
    uint8 ProtoVer_u8;
    uint8 InvProtoVer_u8;
    uint16 payloadType_u16;
    uint32 payloadLength_u32;

}DoIP_HeaderFields_tst;

#include "DoIP_AliveCheckTypes.h"
#include "DoIP_ConnectionTypes.h"
#include "DoIP_RoutingActivationTypes.h"
#include "DoIP_TCPConnectionTypes.h"
#include "DoIP_UDPConnectionTypes.h"

#endif

