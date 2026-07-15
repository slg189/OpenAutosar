

#ifndef DOIP_HEADER_H
#define DOIP_HEADER_H

#include "DoIP_Cfg.h"
#include "rba_DiagLib_MemUtils.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_Header_Write(uint16 payloadType_u16, uint32 payloadLength_u32, uint8* headerData_pu8);
void DoIP_Header_WriteOnlyPayloadType(uint16 payloadType_u16, uint8* headerData_pu8);
void DoIP_Header_Read(const uint8* headerData_pcu8, DoIP_HeaderFields_tst* HeaderFields_pst);
uint16 DoIP_Header_GetPayloadType(const uint8* headerData_pcu8);
uint32 DoIP_Header_GetPayloadLength(const uint8* headerData_pcu8);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif
