

#ifndef DOIP_IPADDRESSASSIGNMENT_H
#define DOIP_IPADDRESSASSIGNMENT_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern boolean DoIP_IsIPAddressAssigned_b;
extern boolean DoIP_IsIPAddressAssignmentRequested_b;
extern DoIP_Type_GIDSyncState_tu8 DoIP_GIDSyncState_u8;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_IpAddressAssignmentDHCPHostname(SoAd_SoConIdType SoConId_uo);
void DoIP_IpAddressAssignmentMainFunction(void);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

