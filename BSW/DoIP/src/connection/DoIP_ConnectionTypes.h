

#ifndef DOIP_CONNECTIONTYPES_H
#define DOIP_CONNECTIONTYPES_H

#include "DoIP_Prv_Types.h"

/* Socket connection state */
typedef enum
{
    DOIP_CON_OFFLINE_E = 0u,
    DOIP_CON_OFFLINE_OPEN_REQUESTED_E,
    DOIP_CON_ONLINE_E,
    DOIP_CON_CLOSE_NEEDED_E,
    DOIP_CON_CLOSE_REQUESTED_E
	
} DoIP_ConnectionStateType_ten;

typedef struct
{
    const DoIP_Cfg_ConnectionType_tst* cfg_pcst;
    SoAd_SoConIdType soConId_uo;
    uint16 connIdx_u16;
    DoIP_ConnectionStateType_ten state_en;
    SoAd_SoConModeType soAdConMode_en;
    boolean closeNeeded_b;
    boolean resetNeeded_b;
} DoIP_ConnectionType_tst;

#endif

