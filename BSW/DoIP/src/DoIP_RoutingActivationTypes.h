

#ifndef DOIP_ROUTINGACTIVATIONTYPES_H
#define DOIP_ROUTINGACTIVATIONTYPES_H

#include "DoIP_Prv_Types.h"

/* Routing Activation */
/* Response codes */
#define DOIP_RA_RESPCODE_UNKNOWN_SA                  0x00u
#define DOIP_RA_RESPCODE_NO_FREE_SOCKET              0x01u
#define DOIP_RA_RESPCODE_CONNECTION_IN_USE           0x02u
#define DOIP_RA_RESPCODE_SA_IN_USE                   0x03u
#define DOIP_RA_RESPCODE_AUTH_FAILED                 0x04u
#define DOIP_RA_RESPCODE_CONF_FAILED                 0x05u
#define DOIP_RA_RESPCODE_UNKNWON_TYPE                0x06u
#define DOIP_RA_RESPCODE_SECURED_CONNECTION_REQUIRED 0x07u
#define DOIP_RA_RESPCODE_SUCCESS                     0x10u
#define DOIP_RA_RESPCODE_PENDING                     0x11u

/* Central security */
#define DOIP_CENTRAL_SECURITY_ID                     0xE0u
#define DOIP_ROUTEACTIVE_NO_FURTHER_ACTION           0x00u
#define DOIP_ROUTENOTACTIVE_INIT_CENTRAL_SECURITY    0x10u

/* Statemachine */
typedef enum
{
    DOIP_RA_IDLE_E = 0u,
    DOIP_RA_ALIVE_E,
    DOIP_RA_WAITALIVE_E,
    DOIP_RA_AUTH_E,
    DOIP_RA_CONF_E
	
} DoIP_RoutingActivationStateType_ten;

#define DOIP_ROUTINGACTIVATION_BUFFERSIZE          (DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_RESP_13)

typedef struct
{
    const DoIP_Cfg_TesterType_tst* testerCfg_pcst;
    const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst;
    uint8 buffer_au8[DOIP_ROUTINGACTIVATION_BUFFERSIZE]; /* always in network byte order (big endian) */
    uint8 oemReqBuffer_au8[4]; /* byte stream, endianess not relevant */
    uint8 oemResBuffer_au8[4]; /* byte stream, endianess not relevant */
    DoIP_ComStateType_ten comState_en;
    DoIP_RoutingActivationStateType_ten raState_en;
    boolean isActive_b;
    boolean oemBytesReceived_b;
    boolean oemBytesResponse_b;
	
} DoIP_RoutingActivationType_tst;

#endif

