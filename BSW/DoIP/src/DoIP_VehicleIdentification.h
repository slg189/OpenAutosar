

#ifndef DOIP_VEHICLEIDENTIFICATION_H
#define DOIP_VEHICLEIDENTIFICATION_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_VehicleIdentification_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);
void DoIP_VehicleIdentification_HandleEIDRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);
void DoIP_VehicleIdentification_HandleVINRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8);
void DoIP_VehicleIdentification_PrepareMessage(SoAd_SoConIdType soConId_uo, uint8* msg_pu8);
uint8 DoIP_VehicleIdentification_GetGIDVINStatusByte(void);
uint8 DoIP_VehicleIdentification_GetFurtherActionByte(void);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
