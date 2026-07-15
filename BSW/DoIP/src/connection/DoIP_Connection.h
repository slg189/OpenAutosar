

#ifndef DOIP_CONNECTION_H
#define DOIP_CONNECTION_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern boolean DoIP_OpenConnections_b;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* Handling of sockets*/
void DoIP_Connection_Init(void);
void DoIP_Connection_Close(DoIP_ConnectionType_tst* soCon_pst);
void DoIP_Connection_Open(DoIP_ConnectionType_tst* soCon_pst);
void DoIP_Connection_SetCloseNeeded(DoIP_ConnectionType_tst *soCon_pst);
void DoIP_Connection_SetResetNeeded(DoIP_ConnectionType_tst *soCon_pst);

/* Getter / Setter */
boolean DoIP_SoCon_IsSoAdSoConIdValid(SoAd_SoConIdType soConId_uo);
DoIP_ConnectionType_tst* DoIP_SoCon_GetForSoConId(SoAd_SoConIdType soConId_uo);
DoIP_ConnectionType_tst* DoIP_SoCon_GetSocketConnection(uint16 connIdx_u16);
void DoIP_Connection_SetUdpRemoteAddressToWildcard(const DoIP_ConnectionType_tst* soCon_pcst);
void DoIP_SetRemoteAddressToWildCard(DoIP_Type_UDPConnection_tst* udpCon_pst);

boolean DoIP_Connection_GetConnIdxForSoConId(SoAd_SoConIdType soConId_uo, uint16* connIdx_u16);

PduIdType DoIP_SoCon_GetSoAdPdu(const DoIP_ConnectionType_tst* soCon_pcst);

boolean DoIP_SoCon_IsActive(const DoIP_ConnectionType_tst* soCon_pcst);

boolean DoIP_SoCon_IsUdpIndex(uint16 connIdx_u16);
boolean DoIP_SoCon_IsTcpIndex(uint16 connIdx_u16);

boolean DoIP_SoCon_AllRelevantConnectionsOffline(void);

void DoIP_Connection_MainFunction(void);

/* Get Sub-Connections */
DoIP_Type_UDPConnection_tst* DoIP_SoCon_GetUDPCon(const DoIP_ConnectionType_tst* soCon_pcst);
DoIP_Type_TCPConnection* DoIP_SoCon_GetTCPCon(const DoIP_ConnectionType_tst* soCon_pcst);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

