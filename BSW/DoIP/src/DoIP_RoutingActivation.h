

#ifndef DOIP_ROUTINGACTIVATION_H
#define DOIP_ROUTINGACTIVATION_H

#include "DoIP_Prv_Types.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_RoutingActivation_Reset(DoIP_RoutingActivationType_tst *ra_pst);
void DoIP_RoutingActivation_MainFunction(DoIP_Type_TCPConnection *tcpCon_pst);
BufReq_ReturnType DoIP_RoutingActivation_RoutingActivationHandler(DoIP_Type_TCPConnection* tcpCon_pst, const uint8* header_pu8, const uint8* payload_pu8);
void DoIP_RoutingActivation_TxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst);

boolean DoIP_RoutingActivation_IsSourceAddressKnown(uint16 sourceAddress_u16, uint8* testerIdx_pu8);
boolean DoIP_RoutingActivation_IsSourceAddressActive(const DoIP_RoutingActivationType_tst *ra_pcst, uint16 sourceAddress_u16);
boolean DoIP_RoutingActivation_IsSourceAddressRegistered(const DoIP_RoutingActivationType_tst *ra_pcst, uint16 sourceAddress_u16);

boolean DoIP_RoutingActivation_IsActivationTypeActive(const DoIP_RoutingActivationType_tst *ra_pcst, uint8 routingActivationNumber_u8);

uint16 DoIP_RoutingActivation_GetAckNackResponseDataLength(const DoIP_Cfg_TesterType_tst* tester_pcst);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

