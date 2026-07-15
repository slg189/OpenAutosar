

#ifndef DOIP_ACTIVATIONLINE_H
#define DOIP_ACTIVATIONLINE_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern DoIP_ActivationLineRequestType DoIP_ActivationLineStatus_u8;
extern uint8 DoIP_AL_StatusChg_u8;

#define DOIP_AL_LOW_IDLE                    0x00u // This state indicates, that the ip address is unassigned successfully and a new AL change can be stored
#define DOIP_AL_HIGH_IDLE                   0x01u // This state indicates, that the ip address is assigned successfully and a new AL change can be stored
#define DOIP_AL_SWITCH_OFFLINE2ONLINE       0x02u // This state indicates, that AL switched from INACTIVE to ACTIVE
#define DOIP_AL_SWITCH_ONLINE2OFFLINE       0x03u // This state indicates, that AL switched from ACTIVE to INACTIVE

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#endif
