

#ifndef DOIP_UDPCONNECTION_H
#define DOIP_UDPCONNECTION_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* General functionality */
void DoIP_UDPConnection_Create(const DoIP_ConnectionType_tst* soCon_pcst);
void DoIP_UDPConnection_Init(void);
void DoIP_UDPConnection_MainFunction(DoIP_Type_UDPConnection_tst* udpCon_pst);
void DoIP_UDPConnection_HandleMessages(DoIP_Type_UDPConnection_tst* udpCon_pst);
void DoIP_UDPConnection_HandleTimer(DoIP_Type_UDPConnection_tst* udpCon_pst);
void DoIP_UDPConnection_SendMessage(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);
void DoIP_UDPConnection_Open(DoIP_Type_UDPConnection_tst* udpCon_pst);
void DoIP_UDPConnection_Close(DoIP_Type_UDPConnection_tst* udpCon_pst);
DoIP_Type_UDPConnection_tst* DoIP_UDPConnection_Get(uint16 udpIdx_u16);

/* Timer */
void DoIP_UDPConnection_InitVehicleAnnouncementTime(DoIP_Type_UDPConnection_tst* udpCon_pst);
void DoIP_UDPConnection_InitVehicleIdentificationTime(DoIP_Type_UDPConnection_tst* udpCon_pst);

/* TODO move to separate file?*/
void DoIP_EntityStatus_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);
void DoIP_Powermode_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);


#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
