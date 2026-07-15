

#ifndef DOIP_PRV_CFG_TYPES_H
#define DOIP_PRV_CFG_TYPES_H

#include "DoIP_Cfg.h"

typedef enum
{
    DoIP_Udp = 0,
    DoIP_Tcp
} DoIP_Cfg_ConnectionKind_ten;

/**
 * @ingroup DOIP_TYPES_H
 * Collects a connection.
 */
typedef struct
{
    PduIdType txPduId_uo;
    DoIP_Cfg_ConnectionKind_ten connectionKind_en;
    boolean hasUdpWildcardRemoteAddress_b;
    boolean requestAddressAssignment_b;
    boolean tcpConnectionSecurityRequired_b;

} DoIP_Cfg_ConnectionType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Defines the prototype of a "routing authentication function" function pointer.
 */
typedef Std_ReturnType (*DoIP_Cfg_RoutingActivationAuthenticationFunctionType_tpfct)(boolean *const Authentified,
        const uint8* AuthenticationReqData, uint8 *const AuthenticationResData);

/**
 * @ingroup DOIP_TYPES_H
 * Defines the prototype of a "routing confirmation function" function pointer.
 */
typedef Std_ReturnType (*DoIP_Cfg_RoutingActivationConfirmationFunctionType_tpfct)(boolean *const Confirmed,
        const uint8* ConfirmedReqData, uint8 *const ConfirmedResData);

/**
 * @ingroup DOIP_TYPES_H
 * Collects all the data necessary to handle a routing activation.
 */
typedef struct
{
    uint8 routingActivationNumber_u8;
    uint16 targetAddressJumpTableIdx_u16;
    uint8 numOfTargetAddresses_u8;
    DoIP_Cfg_RoutingActivationAuthenticationFunctionType_tpfct authenticationFunction_pfct;
    uint8 authenticationReqLength_u8;
    uint8 authenticationResLength_u8;
    DoIP_Cfg_RoutingActivationConfirmationFunctionType_tpfct confirmationFunction_pfct;
    uint8 confirmationReqLength_u8;
    uint8 confirmationResLength_u8;
    boolean routingActivationSecurityRequired_b;

} DoIP_Cfg_RoutingActivationType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Collects a target address.
 */
typedef struct
{
    uint16 targetAddress_u16;

} DoIP_Cfg_TargetAddressJumpTableType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Collects all the data necessary to handle a tester equipment.
 */
typedef struct
{
    uint16 testerSA_u16;
    uint16 numByteDiagAckNack_u16; //according to DoIP big enough to store 0x1999.9999.9999.9999
    uint16 routingActivationJumpTableIndex_u16;
    uint8 numOfRoutingActivations_u8;
    uint16 ChannelJumpTableIdx_u16;
    uint16 numOfchannel_u16;

} DoIP_Cfg_TesterType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Collects the index into the routing activation array.
 */
typedef struct
{
    uint8 routingActivationIdx_u8;

} DoIP_Cfg_RoutingActivationJumpTableType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Collects all the data necessary to handle a channel.
 */
typedef struct
{
    /* Ids which are passed from PduR*/
    PduIdType doIPPduRRxPduId_uo;
    PduIdType doIPPduRTxPduId_uo;
    /* Ids for calling PduR*/
    PduIdType pduRRxPdu_uo;
    PduIdType pduRTxPdu_uo;
    /* Channelinfo*/
    uint16 targetAddr_u16;
    uint8 testerIdx_u8;
	boolean isTpPdu_b;

} DoIP_Cfg_ChannelType_tst;

/**
 * @ingroup DOIP_TYPES_H
 * Callback function for GetGidCallback
 */
typedef Std_ReturnType (*DoIP_Cfg_GetGidCallbackType_tpfct)(uint8* Data);

/**
 * @ingroup DOIP_TYPES_H
 * Callback function for PowerModeCallback
 */
typedef Std_ReturnType (*DoIP_Cfg_PowerModeCallbackType_tpfct)(DoIP_PowerStateType* PowerState);

/**
 * @ingroup DOIP_TYPES_H
 * Callback function for TriggerGidSyncCallback
 */
typedef Std_ReturnType (*DoIP_Cfg_TriggerGidSyncCallbackType_tpfct)(void);

/**
 * @ingroup DOIP_TYPES_H
 * Definition of the configuration structure that is passed to DoIP_Init().
 */
struct DoIP_ConfigType
{
    const DoIP_Cfg_ConnectionType_tst* conn;
    const DoIP_Cfg_TargetAddressJumpTableType_tst* targetAddressJT_pcst;
    const DoIP_Cfg_TesterType_tst* testerCfg_pcst;
    const DoIP_Cfg_RoutingActivationJumpTableType_tst* raJT_pcst;
    const DoIP_Cfg_ChannelType_tst* channel_pcst;
    const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst;
    DoIP_Cfg_GetGidCallbackType_tpfct callbackGetGid_pfct;
    DoIP_Cfg_PowerModeCallbackType_tpfct callbackPowerMode_pfct;
    DoIP_Cfg_TriggerGidSyncCallbackType_tpfct callbackTriggerGidSync_pfct;
    uint16 logicalAddress_u16;
	uint16 connSize_u16;
    uint16 channelSize_u16;
    uint8 eid_au8[6];
    uint8 gid_au8[6];
    uint8 udpConnSize_u8;
    uint8 testerSize_u8;
    boolean oemCentralsecurity_b; //for further action byte in Vehicle identification response
};


#endif

