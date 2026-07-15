

#ifndef DOIP_UDPCONNECTIONTYPES_H
#define DOIP_UDPCONNECTIONTYPES_H

#include "DoIP_Prv_Types.h"

/* UDP queue state*/
#define DOIP_UDP_DATA_EMPTY                 0x00u
#define DOIP_UDP_DATA_RECEIVED              0x01u
#define DOIP_UDP_DATA_PROCESSED             0x02u
#define DOIP_UDP_DATA_READY                 0x03u
typedef uint8 DoIP_UDPQueueStateType_tu8;

typedef struct
{
    uint8 buffer_au8[DOIP_CFG_UDP_Buffer_Size]; /* always in network endianess (big endian) */
    DoIP_UDPQueueStateType_tu8 udpQueueState_u8;

} DoIP_UDPQueueType_tst;

/* UDP connection state */
#define DOIP_UDP_IDLE                       0x00u
#define DOIP_UDP_DISCOVERY                  0x01u
typedef uint8 DoIP_UDPStateType;

/*Extra one location is provided for storing NACK when the queue is full*/
#define DOIP_CFG_MaxUDPQueueSize    (DOIP_CFG_MaxUDPRequestPerMessage + 1u)

typedef struct
{
    const DoIP_ConnectionType_tst* soCon_pcst;
    uint32 timerVehicleAnnouncement_u32;
    DoIP_UDPQueueType_tst DoIP_UDPQueue_au8[DOIP_CFG_MaxUDPQueueSize];
    DoIP_UDPStateType udpState_en;
	boolean isAnnouncementMessageReady_b;
    boolean setUdpRemoteAddressToWildcardWhenQueueEmpty_b;
    boolean isCurrentPhaseIdentification_b; //Make difference between Announcement and Identification!
    boolean DoIP_IsResetVehicleAnnTimer_b; /*Set this flag so that Vehicle Announcement starts once connections are online.*/
} DoIP_Type_UDPConnection_tst;


#endif
