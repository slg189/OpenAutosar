

#ifndef DOIP_ALIVECHECKTYPES_H
#define DOIP_ALIVECHECKTYPES_H

#include "DoIP_Prv_Types.h"

/* Alive Check */
#define DOIP_ALIVECHECK_IDLE                0x00u
#define DOIP_ALIVECHECK_REQUESTED           0x01u
#define DOIP_ALIVECHECK_READY2SEND          0x02u
#define DOIP_ALIVECHECK_WAIT4CONFIRMATION   0x03u
#define DOIP_ALIVECHECK_WAIT4RESPONSE       0x04u
#define DOIP_ALIVECHECK_RECEIVED            0x05u

#define DOIP_ALIVECHECK_BUFFERSIZE          (DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_ALIVE_CHECK_RESP)

typedef struct
{
    uint32 aliveCheckTimer_u32;
    uint8 aliveCheckBuffer_au8[DOIP_ALIVECHECK_BUFFERSIZE]; /* always in network byte order (big endian) */
    uint8 aliveCheckState_u8;
} DoIP_Type_AliveCheck_tst;

#endif
