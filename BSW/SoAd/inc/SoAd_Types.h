

#ifndef SOAD_TYPES_H
#define SOAD_TYPES_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ComStack_Types.h"
#include "TcpIp.h"
#include "SoAd_Cfg.h"  /* needed for PDU_ROUTE_DEST_MAX value */

/**************************************************************************************************/
/*  Common Published Information                                                                  */
/**************************************************************************************************/

/*
 ***************************************************************************************************
 * STUBS
 ***************************************************************************************************
 */

#define SOAD_CONST
#define SOAD_VAR
#define SOAD_CODE

/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

typedef uint8 SoAd_ParamIdType;

/*SoAd_UpperLayerType_ten*/
typedef enum
{
    SOAD_IF,        /* UpperLayer type is IF*/
    SOAD_TP         /* UpperLayer type is TP*/
}SoAd_UpperLayerType_ten;

/*SoAdTxUdpTriggerMode_ten*/
typedef enum
{
    SOAD_TRIGGER_ALWAYS,        /* PDU triggers the transmission */
    SOAD_TRIGGER_NEVER          /* PDU does not trigger the transmission*/
}SoAdTxUdpTriggerMode_ten;

/*SoAd_SoConModeType*/
typedef enum
{
    SOAD_SOCON_OFFLINE,             /* Socket connection state is closed and no transmission and reception is possible */
    SOAD_SOCON_RECONNECT,           /* Socket connection in this state is trying to come to online */
    SOAD_SOCON_ONLINE               /* Socket connection state is open and transmission and reception is possible */
}SoAd_SoConModeType;

/*Enum for SocketAdaptor's sub modes */
/* Please NOTE : Donot change enum values or add any enum element without ananlysis as these elements are used for arithmetic comparisons*/
typedef enum
{
    SOAD_SOCON_IDLE,
    SOAD_SOCON_CLOSE,                   /* State in which a socket connection completely closed */
    SOAD_SOCON_TCPIP_ALLOCATED,         /* State in which socket is successfully created but waiting to bind */
    SOAD_SOCON_BOUND,                   /* State in which socket is successfully bound but waiting to go online */
    SOAD_SOCON_TCPWAITFORREMOTE,        /* State in which opening for a socket connection is requested and request is sent to TCPIP layer */
    SOAD_SOCON_ACTIVE,                  /* State in which socket connection is ready to transmit and receive pdus */
    SOAD_SOCON_WAITFORCLOSE,            /* State in which socket connection is waiting to get SoAd_TcpIpEvent to close */
    SOAD_SOCON_READYTOCLOSE             /* State in which socket connection is ready to close in next main function */
}SoAd_SoConSubMode_ten;

/* Enum which describes the triggering event types of SoAd */
typedef enum
{
    SOAD_EVENT_IDLE,                /* event for which socket connection is in idle state */
    SOAD_EVENT_ONLINEREQ,           /* event for which socket connection shall take action to go online */
    SOAD_EVENT_OFFLINEREQ_UL,       /* event for which socket connection shall take action to go off line, because trigger event shall initiated by upper layer*/
    SOAD_EVENT_OFFLINEREQ_SOAD,     /* event for which socket connection shall take action to go reconnect, because trigger event shall initiated internally by SoAd*/
    SOAD_EVENT_RECONNECTREQ_SOAD,   /* event for which socket connection shall take action to go to reconnect, because trigger event initiated internally by Soad*/
    SOAD_EVENT_OFFLINEREQ_TCPIP     /* event for which socket connection shall be closed in main function as TcpIp close was not performed in TcpIpEvent*/
}SoAd_TriggerEventType_ten;

/*SoAd_SocTpDataCopyStatus_ten*/
typedef enum
{
    SOAD_TP_DATAFRAGMENT_COPY_START,        /* Used for the first time entry into Loop for a fragment copy*/
    SOAD_TP_DATAFRAGMENT_COPY_ONGOING,      /* CopyTxData of a fragment is going on (not done completely) */
    SOAD_TP_DATAFRAGMENT_COPY_FINISHED,     /* CopyTxData is done completely for a fragment of data */
    SOAD_TP_DATAFRAGMENT_COPY_RETRY,        /* CopyTxData shall be retreid in the next main function */
    SOAD_TP_DATACOPY_ERROR                  /* Some error in copying the data */
}SoAd_SocTpDataCopyStatus_ten;

/* User Types */
typedef enum
{
    SOAD_USER_PDUR = 0,          /* PDUR is the upper layer of SoAd*/
    SOAD_USER_DOIP,              /* Diagnostics over IP is the upper layer of SoAd*/
    SOAD_USER_XCP,               /* XCP is the upper layer of SoAd*/
    SOAD_USER_CDD,               /* Complex Devide Driver is the upper layer of SoAd*/
    SOAD_USER_CDDTCP,            /* Complex Devide Driver(for TCP) is the upper layer of SoAd*/
    SOAD_USER_SD,                /* Service Discovery is the upper layer of SoAd*/
    SOAD_USER_UDPNM,             /* UDPNM is the upper layer of SoAd*/
    SOAD_USER_ETHDIAGMM,         /* ETHDIAGMM is the upper layer of SoAd. (Non Autosar)*/
    SOAD_USER_INVALID            /* Invalid Upper layer */
} SoAd_UserType_ten;

typedef enum
{
    SOAD_PDU_OK,                    /* Pdu is valid for further processing (received Pdu is completely present in the message) */
    SOAD_PDU_FRAGMENTED_HEADER,     /* Pdu header is fragmented */
    SOAD_PDU_FRAGMENTED_DATA,       /* Pdu data is fragmented */
    SOAD_PDU_DISCARD,                /* Pdu needs to be discarded */
    SOAD_PDU_DISCARD_REST_MSG        /* Dicard the rest of the message along with the Pdu needs to be discarded */
}SoAd_RxPduAcceptance_ten;

typedef enum
{
    SOAD_IF_TRANSMISSION_OK,             /* Pdu data is transmitted successfully in SoAd_IfTransmit( ) API itself */
    SOAD_IF_TRANSMISSION_E_NOT_OK,       /* Pdu data is not able to transmit via corresponding socket */
    SOAD_IF_NPDU_BUFFERING_OK            /* Pdu data is buffered successfully in socket specific buffer */
}SoAd_IfTransmitReqStatus_ten;

/* SoAd_RxPduDesc_tst:  Description of a received Pdu */
typedef struct
{
    uint32                          PduDataLength_u32;          /* Data length of the received Pdu */
    PduIdType                       RxPduId_uo;                 /* Rx Pdu Id of the corresponding socket route */
} SoAd_RxPduDesc_tst;

#if (SOAD_RX_BUFF_ENABLE != STD_OFF)
/* SoAd_DynRxBuffTableType_tst:  Dynamic Rx Buff Table */
typedef struct
{
    /* Buffer pointer */
    uint8* const                    RxBuffStrt_cpu8;                    /* Pointer to the starting of the reception buffer */
    uint8*                          RxNxtData_pu8;                      /* Pointer to the next data location of the reception buffer */
    uint8*                          RxNxtFree_pu8;                      /* Pointer to the next free location of the reception buffer */

#if (SOAD_TP_PRESENT != STD_OFF)
    uint32                          TpDataRemainingLenToTransfer_u32;   /* Remaining length of the TP Pdu to be transfered from Rx buffer to UL */
    uint16                          TpDataCurrentSize_u16;              /* Length of TP data stored in the Rx buffer */
#endif

    /* Buffer size */
    const uint16                    TotalRxBuffSize_cu16;               /* Size of the Rx buffer */

    /* Rx Pdu desc */
    SoAd_RxPduDesc_tst              DataFragCurrentRxPduDesc_st;        /* Description of the fragmented Pdu under reception */

    /* Current size */
    uint16                          DataFragCurrentSize_u16;            /* Length of fragmented data stored in the Rx buffer */

    uint8                           HeaderFragCurrentSize_u8;           /* Length of fragmented Pdu header stored in the Rx buffer */

    /* Flags */
    /* (flags are needed because fragmentation can be in progress while there is no fragment in buffer yet) */
    boolean                         DataFragReceptionInProgress_b;      /* Flag indicating if there is a reception of fragmented data */
    boolean                         HeaderFragReceptionInProgress_b;    /* Flag indicating if there is a reception of fragmented Pdu header */

} SoAd_DynRxBuffTableType_tst;
#endif/* SOAD_RX_BUFF_ENABLE != STD_OFF */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
typedef struct
{
    uint32  ifTcpTxConfRemainingLen_au32; /* No of bytes has been transmitted for this pdu, it can be via single socket or multiple sockets */
    boolean ifTcpTxConfReceivedStatus_b; /* Tx Confimation received status for IF TCP pdus. If multiple PduRouteDests are configured(fan-out) and
                                                           active(ONLINE), then this flag will be set if TxConfirmation received atleast from one remote */
    boolean Reserved_b[3];               /* Reserved for future use */
}SoAd_IfTcpPduTxPendingStatus_tst;
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
typedef struct
{
    uint8 * tpUdpTxBuffer_pu8;        /* TP Udp Tx Buffer pointer */
    uint16  tpUdpTxBufferSize_u16;    /* TP Udp Tx Buffer size */
    uint16  Reserved_u16;             /* Reserved for future use */

}SoAd_TpUdpTxBufferInfo_tst;
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
typedef struct
{   uint32                              nPduUdpTxPduHeaderId_u32;   /* Pdu header id of the latest PduRoute for which nPdu Transmit was requested (needed for LAST_IS_BEST semantics) */
    PduLengthType                       nPduUdpTxPduDataLength_uo;  /* Requested data length to be transmited for this Tx Pdu Id (needed for LAST_IS_BEST semantics) */
    PduIdType                           nPduUdpTxPduId_uo;          /* Tx Pdu Id which is stored in the nPduUdpTxBuffer */

}SoAd_nPduUdpTxPduDescriptor_tst;

typedef struct
{
    uint8* const                        nPduUdpTxBuffer_cpu8;       /* Pointer to nPdu Udp Tx buffer */

    SoAd_nPduUdpTxPduDescriptor_tst* const nPduUdpTxPduDescriptor_cpst; /* Pointer to Pdu descriptor (contains the information about the Pdu stored in the nPdu Udp Tx buffer)*/

    uint32                              nPduUdpTxTimer_u32;         /* It specific the timeout value for nPduUdpTxBuffer to
                                                                       which timer will be running, once it is expired, copied data in nPduUdpTxBuffer
                                                                       should be transmitted immediately.
                                                                       Note: It is converted to multiple of SoAd main period to easy manipulation */

    const uint16                        nPduUdpTxBufferSize_cu16;   /* nPdu Udp Tx Buffer size */

    uint16                              nPduUdpTxBuffCopiedLen_u16; /* It specifies the no of bytes copied to nPduUdpTxBuffer */
    const uint8                         nPduUdpTxPduIdSize_cu8;     /* Size of the Pdu descriptor */
    uint8                               Reserved_au8[3];            /* Reserved for word allignment */
}SoAd_nPduUdpTxInfo_tst;
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
/*SoAd_RoutingGroupSoConMap_tst : Structure used to store SoConId's which are mapped to specific Routing Group's and Routing Group status as well */
typedef struct
{
    SoAd_SoConIdType mappedSoConId_uo; /*Mapped SoCon to Specific Routing Group*/
    boolean specificRgStatus_b;        /*Specific Routing Group status*/
}SoAd_RoutingGroupSoConMap_tst;

/*SoAd_RoutingGroupSoConStructMap_tst : Structure used to store the RoutingGroupSoConMap structure pointer and length of the SoCon which are mapped to corresponding RG Id */
typedef struct
{
    SoAd_RoutingGroupSoConMap_tst *routingGrpSoConMap_pst;  /*RoutingGroupSoConMap structure pointer */
    SoAd_SoConIdType numOfSoConIdMapped_uo;            /*Number of SoConId mapped in RoutingGroupSoConMap structure for particular index */
    boolean rgStatus_b;                                /* Routing group status */
    boolean specificRgCheck_b;                       /* It specifies if specific routing group is enabled */
}SoAd_RoutingGroupSoConStructMap_tst;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

/* Structure to contain PduRouteDest information */
typedef struct
{
#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
    SoAd_SoConIdType           triggerTransmitSocketIdx_uo;    /* reference to the static socket table to which PDU needs to be transmitted.
                                                                   if socket connection group reference is mapped and
                                                                   SoAd_TriggerIfSpecificRgTransmit( ) API called, then It will be updated with
                                                                   SoConId which passes as arguments for the function */
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

    uint8                      triggerTransmitRequest_u8;     /* trigger transmit request */

}SoAd_TriggerTransmitInfo_tst;

/* Structure to contain PduRouteDest information */
typedef struct
{
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )

    SoAd_TriggerTransmitInfo_tst*   triggerTransmissionInfo_puo;    /* reference to the static socket table to which PDU needs to be transmitted.
                                                                       if socket connection group reference is mapped and
                                                                       SoAd_TriggerIfSpecificRgTransmit( ) API called, then It will be updated with
                                                                       SoConId which passes as arguments for the function */
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0  )
    uint8*                      nPduUdpTxBufferedSocketCnt_pu8;       /* It indicates that in how many socket buffers, Pdu data is stored.
                                                                         If it is stored atleast in any one of the socket buffer, SoAd has to provide the Ul_TxConfirmation callback
                                                                         once buffered data is transmitted to LL successfully.
                                                                         For example, if PduRouteDest is mapped to socket connection group and
                                                                         3 sockets are belongs to the group, then it shall be updated
                                                                         to 3 if pdudata is buffered in all 3 sockets buffer, else it shall be updated to
                                                                         some value based on the no of socket buffer in which Pdu data was stored */

    SoAdTxUdpTriggerMode_ten        triggerTransmission_en;         /* Specifies whether a PDU triggers the transmission of the nPduUdpTxBuffer (TRIGGER_NEVER or TRIGGER_ALWAYS) */

    uint32                          txUdpTriggerTimeout_32;         /* Specifies the timeout value for nPduUdpTxBuffer for specific pdu and it shall be compared ,
                                                                       with SoAdSocketUdpTriggerTimeout parameter if it is configured and chooses the small one.
                                                                       Suppose if SoAdSocketUdpTriggerTimeout parameter is not configured, then it shall be direcly
                                                                       assign as nPduUdpTimeout value for the particular socket.
                                                                       Note: It shall be multiple of SoAd main period for easy manupulation at run time */
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

    uint32                          txPduHeaderId_u32;              /* TxPdu Header Id of the PduRouteDest */

#if ( SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    const SoAd_RoutingGroupIdType*  txRoutingGroupConf_cpuo;        /* List of Routing Groups which are configured for the particular Tx PduRouteDest */

    SoAd_RoutingGroupIdType         numOfTxRoutingGroups_uo;        /* Number of Tx Routing groups configured under the PduRouteDest */
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

    SoAd_SoConIdType                txStaticSocketOrGroupIdx_uo;    /* reference to the static socket table or static socket group table
                                                                       if identifyTxSocketConnOrGroupRef_b is TRUE means it will be static socket table
                                                                       if identifyTxSocketConnOrGroupRef_b is TRUE means it will be static socket group table*/

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
    boolean                     identifyTxSocketConnOrGroupRef_b;     /* It indicates that PduRouteDest is mapped to Socket connection or group
                                                                       TRUE - socket connection ref, FALSE - socket connection Group ref */
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

}PduRouteDestType_tst;

/*13.03.2012*/
/*Definition of Configuration structures */

/* Dynamic Info about TxPdu*/
typedef struct
{
#if (SOAD_IF_PRESENT != STD_OFF)
    boolean     isIfTxCalled_b;         /* it indicates if IF transmit is called for a PDU */
#endif
#if (SOAD_TP_PRESENT != STD_OFF)
    boolean     isTpTxCalled_b;         /* it indicates if TP transmit is called for a PDU */
    boolean     isCancelTpTxCalled_b;   /* it indicates if TP cancel Tx is called for a PDU */
#endif

}SoAd_TxPduDynInfo_tst;

/* SoAd_TxPduConfig :  Structure for Pdu Configuration Table */
typedef struct
{
    PduIdType                   txPduCfgTableIdx_uo;            /* index of the SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst table */
    PduIdType                   txPduId_uo;                     /* Tx PDU ID of the PDU coming from the PDU Router */
    PduIdType                   ulTxPduId_uo;                   /* Tx PDU ID of the Upper layer, used in call backs of upper layers*/
    PduIdType                   PduRouteDestNum_uo;             /* Number of PduRouteDest configured with this PduRoute */
    SoAd_TxPduDynInfo_tst *     txPduDyninfo_pst;               /* Dynamic information of the PDU*/
    SoAd_UpperLayerType_ten     txUpperLayerType_en;            /* Specifies the upper layer interface type */
    SoAd_UserType_ten           txUser_en;                      /* User of the TxPdu */

    const PduRouteDestType_tst* PduRouteDestConf_cpst;          /* Data of all PduRouteDests of the PduRoute (TxPdu) */

    PduLengthType               txPduLength_uo;                 /* Tx PDU Length of the PDU coming from the PDU Router*/

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    uint8                       txIfTcpPduIndex_u8;             /* Index to the array SoAd_IfTcpPduTxPendingStatus_tst
                                                                   which has the length pending for confirmation for IF TCP and fanout status */
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if (SOAD_IF_PRESENT != STD_OFF)
    uint8                       idxIfTxConfirm_u8;              /* Index to the IfTxConfirm table */
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF)
    uint8                       idxTpTxConfirm_u8;              /* Index to the TpTxConfirm table */
    uint8                       idxTpCopyTxDataCallback_u8;       /* Index of the Soc connection CopyTxDataCallback */
#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) )
    uint8                       idxTriggerTransmitCallback_u8;  /* Index of the Soc connection TriggerTransmit callback */
                                                                /* The TriggerTransmit can be used for IfRoutingGroupTriggerTransmit feature or for nPdu feature */
#endif /* (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) */

#if (SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
    uint8                       zeroCopyCallback_u8;       /* Index of the Soc connection CopyTxDataCallback */
#endif /* SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF */

#if(STD_ON == SOAD_METADATA_ENABLED)
    boolean                     isTxPduMetaDataCfgd_b;  /* Flag indicating if MetaData item configured is of type SOCKET_CONNECTION_ID_16 */
#endif
} SoAd_TxPduConfigType_tst;

#if (SOAD_TP_PRESENT != STD_OFF)
/* Dynamic Info about RxPdu*/
typedef struct
{
    boolean     isCancelTpRxCalled_b;   /* it indicates if TP cancel Rx is called for a PDU */

}SoAd_RxPduDynInfo_tst;
#endif

/* SoAd_RxPduConfig :  Structure for Pdu Configuration Table */
typedef struct
{
    PduIdType               rxPduCfgTableIdx_uo;                /* index of the SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst table */
    PduIdType               rxPduId_uo;                         /* Rx PDU ID used by the upper layer to call cancel reception of this pdu id */
    PduIdType               ulRxPduId_uo;                       /* Rx PDU ID of the Upper layer, used in call backs of upper layers*/
    PduIdType               reservedPduId_uo;                   /* reserved for padding and will be used in future */
#if (SOAD_TP_PRESENT != STD_OFF)
    SoAd_RxPduDynInfo_tst * rxPduDynInfo_pst;                   /* pointer to rx Pdu Dynamic Info*/
#endif
    uint32                  rxPduHeaderId_u32;                  /* Id contained in the packet received on the TCP/IP connection
                                                                   if the PDU header option is enabled */

    SoAd_UpperLayerType_ten rxUpperLayerType_en;                /* Specifies the upper layer interface type */
    SoAd_UserType_ten       rxUser_en;                          /* User of the TxPdu */

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    const SoAd_RoutingGroupIdType*  rxRoutingGroupConf_cpuo;    /* List of Routing Groups which are configured for the particular Rx SocketRouteDest */
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

    PduLengthType           rxPduLength_uo;                     /* Rx PDU Length of the PDU coming from the PDU Router*/
    PduLengthType           reservedPduLen_uo;                  /* reserved for padding and will be used in future */

    SoAd_SoConIdType        rxStaticSocketOrGroupIdx_uo;        /* reference to the static socket table or static socket group table
                                                                   if identifyRxSocketConnOrBundRef_b is TRUE means it will be static socket table
                                                                   if identifyRxSocketConnOrBundRef_b is TRUE means it will be static socket group table*/

    SoAd_SoConIdType        reservedSoConIdx_uo;                /* reserved for padding and will be used in future */

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    SoAd_RoutingGroupIdType   numOfRxRoutingGroups_uo;          /* Number of Rx Routing groups configured under the SocketRouteDest */
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if (SOAD_IF_PRESENT != STD_OFF)
    uint8                   idxIfRxIndication_u8;               /* Index to the IfTxConfirm table */
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF)
    uint8                   idxTpRxIndication_u8;               /* Index to the TpTxConfirm table */
    uint8                   idxTpCopyRxDataCbk_u8;              /* Index of SoAdTp_copyRxDataCallbacks_cast */
    uint8                   idxTpStartReceptionCbk_u8;          /* Index of SoAdTpStartOfReception call backs*/
#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( SOAD_SOCON_RX_BUNDLE_REF_ENABLE != STD_OFF )
    boolean                 identifyRxSocketConnOrBundRef_b;      /* It indicates that SocketRoute is mapped to Socket connection or group
                                                                   TRUE - socket connection ref, FALSE - socket bundle ref */
#endif /* SOAD_SOCON_RX_BUNDLE_REF_ENABLE != STD_OFF */

#if(STD_ON == SOAD_METADATA_ENABLED)
    boolean                 isRxPduMetaDataCfgd_b;  /* Flag indicating if MetaData item configured is of type SOCKET_CONNECTION_ID_16 */
#endif
} SoAd_RxPduConfigType_tst ;

/* TcpIp local address id info */
typedef struct
{
    TcpIp_IpAddrStateType localAddrIdx_State_en;/* It indicates the state of local address Id */

    boolean               isLocalAddrIdCfgd_b;    /* It indicates if local address id is present in SoAd config */

}SoAd_LocalAddrIdInfoType_tst;

/* structure to hold the indexes of Local ip address assignment change and socket mode change callbacks */
typedef struct
{
    uint8                   NumberOfUlCfgd_u8;                    /* count of the upper layers which has to receive the callbacks*/
    uint8*                  CbkULsIdxList_pu8;                    /* pointer to the array of index of the ULs which need to receive this call back*/
}SoAd_CallBackULInfo_tst;

/* SoAd_SocConfig :  Static Socket Configuration Entry */
typedef struct
{
    SoAd_SoConIdType            soConCfgTableIdx_uo;         /* index of the SoAd_StaticSocConfig_cast table */
    SoAd_SoConIdType            soConId_uo ;                 /* symbolicNameValue; The Socket ID is used as a reference
                                                                to a particular connection when transferring data to and
                                                                from the PDU Router*/
    SoAd_SoConIdType            soConGrpId_uo ;              /* Id of the group to which this socket connection belongs to */
    SoAd_SoConIdType            reserved_uo ;                /* reserved member for padding and will be used in future */

    SoAd_CallBackULInfo_tst*    SocModeChgCbkULInfo_pst;         /* Index of the Soc connection mode change notification call back */
    SoAd_CallBackULInfo_tst*    LocalIpAddrChgCbkULInfo_pst;     /* Index of Local Ip Address assignment change call bcak to UL */

    SoAd_SoConIdType*           SoAd_Prv_idxDynSocTable_puo; /* Index to SoAd_DyncSocConfigType structure which is handled dynamically */

#if (SOAD_RX_BUFF_ENABLE != STD_OFF)
    SoAd_DynRxBuffTableType_tst* dynRxBuffTableEntry_pst;    /* Pointer to an address of corresponding index of Rx Buff table which is handled dynamically */
#endif /* SOAD_RX_BUFF_ENABLE != STD_OFF */

    const PduIdType*            rxPduIds_pcuo;                /* pointer to an array of rxPduid list */

    /* DD: We will not have a field that directly stores the remote IP address in this structure. Such field will require 16 bytes to be able to store both IPv4 addresses (4 bytes)
           and IPv6 addresses (16 bytes). Storing an IPv4 address in such a field will waste memory. Instead, we will store all IPv4 addresses (together with port) in the array
           statSoConRemoteAddrInet_cpst and all IPv6 addresses in the array statSoConRemoteAddrInet6_cpst (both are defined in SoAd_ConfigType). In this structure we will only hold
           an index to an element in one of those arrays (depending on the address domain). */
    SoAd_SoConIdType        idxStatSoConRemoteAddr_uo;   /* Index to get an SoAd_SockAddrInetType or SoAd_SockAddrInet6Type structure which holds the remote IP address and port */

#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
    SoAd_TpUdpTxBuffIndexType   tpUdpTxBuffIdx_uo;           /* Index of the TP UDP Tx Buffer array SoAdTp_UdpTxSocketBufferInfo_cpauo */
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    SoAd_nPduUdpTxIndexType     nPduUdpTxTableIndex_uo;         /* Index of the nPdu UDP Tx Buffer array SoAdIf_nPduUdpTxSocketBufferInfo_cpauo */
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

    uint8                       numberofULsConfigured_u8;    /* Specifies the number of upper layers configured for this socket connection */
    uint8                       numberOfRxPdusConfigured_u8;  /* number of RxPdu's configured for the socket */
} SoAd_StaticSocConfigType_tst;

#if (SOAD_UDP_PRESENT != STD_OFF)
/* SoAd_SocGroupUdpConfig:  Static Socket Connection Group Configuration Entry for UDP Protocol specific parameters */
typedef struct
{
#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    uint32                      socketUdpTriggerTimeout_u32;        /* Specifies the timeout value for nPduUdpTxBuffer for specific socket and it shall be compared ,
                                                                       with SoAdTxUdpTriggerTimeout parameter if it is configured and always chooses the small one.
                                                                       Suppose if SoAdTxUdpTriggerTimeout parameter is not configured, then it shall be direcly
                                                                       assign as nPduUdpTimeout value for the particular socket.
                                                                       Note: It shall be multiple of SoAd main period for easy manupulation at run time */

    uint16                      nPduUdpTxBufferMin_u16;             /* Specifies the data in bytes to be buffered for tx
                                                                       if the UDP message be buffered for tx of multiple PDUs per UDP*/
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */
    uint8                       listenOnlyUDP_u8;                   /* Used to disable the transmit functionality on this UDP port.
                                                                       This parameter is only relevant for UDP connections */
#if (SOAD_STRICT_UDP_HDR_LEN_CHECK_ENABLE == STD_ON)
    boolean                     udpStrictHdrLenChkEnabled_b;       /* Specifies if UDP messages shall be dropped (TRUE)
                                                                      if the length of all contained PDUs does not match the length of
                                                                      the whole message or not (FALSE). Shall only be set to TRUE  */
#endif

#if ( SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 )
    SoAd_UdpSocGrpIndexType     udpAliveSupervisionTimeoutIdx_uo;   /* Specifies the Index of Udp Alive Supervision Timeout Socket Group Array */
#endif /* SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 */
    SoAd_UdpSocGrpIndexType     udpTriggerTimeoutIdx_uo;            /* Specifies the Index of timeout in [s] a nPduUdpTxBuffer
                                                                       is waiting for a PDU with TriggerMode = TRIGGER_ALWAYS */
} SoAd_StaticUdpConfigType_tst;
#endif /* SOAD_UDP_PRESENT != STD_OFF */

#if (SOAD_TCP_PRESENT != STD_OFF)
/* SoAd_SocGroupTcpConfig:  Static Socket Connection Group Configuration Entry for TCP Protocol specific parameters */
typedef struct
{
    uint32                      tcpTxQuota_u32;                     /* Specifies the maximum amout of bytes (PDU data provided
                                                                       by the upper layer and PDU Header if used) the SoAd may queue for transmission via TCP
                                                                        at the TcpIp module for each socket connection of this socket connection group.*/

#if((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF ))
    TcpIp_TlsConnectionIdType   tcpTlsConnId_uo;     			  /* Tls connection index */
#endif/* ((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF )) */

#if (SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF)
    SoAd_TcpSocGrpIndexType     tcpKeepAliveTimeIdx_uo;             /* Specifies the Index of Tcp Keep Alive Time Socket Group Array */
    SoAd_TcpSocGrpIndexType     tcpKeepAliveIntervalIdx_uo;         /* Specifies the Index of Tcp Keep Alive Time Interval Socket Group Array */
    SoAd_TcpSocGrpIndexType     tcpKeepAliveProbeMaxIdx_uo;         /* Specifies the Index of Tcp Keep Alive Probe Max Socket Group Array */
    uint8                       tcpKeepAlive_u8;                    /* Specifies if keepalive feature is enabled. This is only for TCP connections */
#endif/* SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF */

#if (SOAD_TP_TCP_PRESENT != STD_OFF)
    uint8                       tcpImmediateTpTxConfirm_u8;         /* specifies whether the immediate Tcp confirmation is required for TP transmissions*/
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

    uint8                       tcpInitiate_u8;                     /* Specifies the initiator for this TCP connection.
                                                                       This parameter is only relevant for TCP connections */
    uint8                       tcpNoDelay_u8;                      /* Specifies not to use the congestion control mechanism
                                                                       for this connection. This is only for TCP connections*/
    boolean                     tcpOutofOrder_b;                    /* Specifies if out of order is enabled*/

} SoAd_StaticTcpConfigType_tst;
#endif /* SOAD_TCP_PRESENT != STD_OFF */

/* SoAd_SocGroupConfig :  Static Socket Connection Group Configuration Entry */
typedef struct
{
    SoAd_SoConIdType                    soConGrpCfgTableIdx_uo;                 /* index of the SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst table */
    SoAd_SoConIdType                    soConGrpId_uo ;                         /* symbolicNameValue; The GroupId shall be useful to recognize the
                                                                                   socket connection belongs to which Group */
    SoAd_SoConIdType                    startSoConIdx_uo;                       /* Index of the Start socket connection of this group */
    SoAd_SoConIdType                    maxSoConChannel_uo;                     /* Max socket connection configured for this group */

#if ( (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF) )
    TcpIp_SocketIdType*                 groupTcpIpSocketId_puo;                 /* To store the Tcp listen socket id or UDP group socket id */
#endif /* (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF) */

#if (SOAD_UDP_PRESENT != STD_OFF)
    const SoAd_StaticUdpConfigType_tst* udpProtocolConf_cpst;                   /* Pointer to an address of Corresponding index of UDP Protocol structure array */
#endif /* SOAD_UDP_PRESENT != STD_OFF */

#if (SOAD_TCP_PRESENT != STD_OFF)
    const SoAd_StaticTcpConfigType_tst* tcpProtocolConf_cpst;                   /* Pointer to an address of Corresponding index of TCP Protocol structure array */
#endif /* SOAD_TCP_PRESENT != STD_OFF */

    TcpIp_ProtocolType                  soConGrpProtocol_en;                    /* Specifies the transport protocol (UDP or TCP) */
    TcpIp_DomainType                    domain_en;                              /* Specifies the domain (IPV4 or IPV6)*/

#if (SOAD_IPv6_SUPPORT_ENABLE != STD_OFF)
    uint32                              soConGrpFlowLabel_u32;                  /* Specifies the flow label of the ethernet frame */
#endif
    uint16                              soConGrpLocalPort_u16;                  /* Local UDP or TCP port used for this connection */

    TcpIp_LocalAddrIdType               soConGrpLocalAddressIdx_u8;             /* Index to be passed to lower layer to refer local address.
                                                                                   Used to identify the local IP address and EthIf controller configured in TcipIp. */
    uint8                               soConGrpFramePriority_u8;               /* Specifies the priority of the Ethernet frame */
#if (SOAD_IPv6_SUPPORT_ENABLE != STD_OFF)
    uint8                               soConGrpDscp_u8;                        /* Specifies the Differentiated Services Field in IP Header */
#endif
    boolean                             headerEnable_b;                         /* Enables the transmission of the SoAd PDU header (ID, length) on this socket connection */
    boolean                             resourceMgtEnable_b;                    /* Enables the resource management option for this socket. May not be activated for UDP sockets in receive */

    boolean                             soConGrpAutoConSetup_b;                 /* Specifies if the setup of the socket connection shall be done automatically (TRUE) or manually (FALSE)*/
    boolean                             socMsgAcceptanceFiletEnable_b;          /* Specifies if the message acceptance filter is enabled (TRUE) or not (FALSE)*/
#if (SOAD_BYPASS_MSGACCEPTFILTER_POLICY == STD_ON)
    boolean                             bypassMsgAcceptanceFilterPolicy_b;      /* Specifies if the bypass for the message acceptance filter policy is enabled (TRUE) or not (FALSE)*/
#endif
} SoAd_StaticSoConGrpConfigType_tst;

/* SoAd_DyncSocConfig :  Dynamic Socket Table of Active Sockets */
typedef struct
{
    const PduInfoType               * rxPduInfoPtr_cpst;        /* The reference to Rx Packet Buffer */

    const PduInfoType               * txPduInfoPtr_cpst;        /* The reference to Tx Packet Buffer */

    SoAd_SoConModeType              socConMode_en ;             /* The State of the Socket */
    SoAd_SoConSubMode_ten           socConSubMode_en ;          /* The sub state of the soc connection */
    SoAd_TriggerEventType_ten       socConRequestedMode_en ;    /* To store the triggering event requested for the socket connection */

#if (SOAD_TP_PRESENT != STD_OFF )
    PduLengthType                   tpTxRemainingLength_uo;     /* Variable to hold the remaining length info in Tp_Tx */
    PduLengthType                   tpTransmittedLength_uo;     /* Variable to hold the transmitted length info in Tp_Tx */
    PduLengthType                   tpTcpCopyTxDataLength_uo;   /* Variable to hold the length requested in SoAd_CopyTxData call back */
    PduLengthType                   tpTxTotPduLen_uo;           /* Total pdu length TP packet, this member will be used only for TP case */

    boolean                         tpReceptionOngoing_b;       /* Status to store while TP reception is ongoing */
    boolean                         tpTransmissionOnGoing_b;    /* Status to stote TP transmission is ongoing */
    boolean                         tpTransmitRequest_b;        /* flag to store the  TpTransmit request to be processed in main function */
    boolean                         Reserved_b;                 /* Reserved for word allignment */
#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 )
    uint32                          UdpAliveSupervisionTimer_u32; /* Variable to hold UdpActiveSupervision timeout value */
#endif  /* SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 */

    uint32                          txPduHeaderId_u32;          /* If multiple PduRouteDests are configured per pdu and header was configured,
                                                                   SoAd_CopyTxData needs the header Id for the active socket */

#if (SOAD_TCP_PRESENT != STD_OFF)
    uint32                          rxTcpRemainingPduLengthToDicard_u32;  /* Remaining length to discard for a received Pdu */
#endif

    uint16                          socConOpenReqCnt_u16;       /* No of open SoCon requests comes from UL */
    uint16                          socConCloseReqCnt_u16;      /* No of close SoCon requests comes from UL */

    SoAd_SoConIdType                idxDynSocConfigTable_uo;    /* Index of dynamic socket table */
    SoAd_SoConIdType                idxStaticSoc_uo;            /* Index to static Socket table */

    /* DD: We will not have a field that directly stores the remote IP address in this structure. Such field will require 16 bytes to be able to store both IPv4 addresses (4 bytes)
           and IPv6 addresses (16 bytes). Storing an IPv4 address in such a field will waste memory. Instead, we will store all IPv4 addresses (together with port) in the array
           SoAd_DynSoConRemoteAddrInet_pst and all IPv6 addresses in the array SoAd_DynSoConRemoteAddrInet_pst (both are external variables defined in SoAd_Prv.h).
           In this structure we will only hold an index to an element in one of those arrays (depending on the address domain). */
 const  SoAd_SoConIdType            idxDynSoConRemoteAddr_uo;   /* Index to get an SoAd_SockAddrInetType or SoAd_SockAddrInet6Type structure which holds the remote IP address and port */
     SoAd_SoConIdType               ReservedSoConIdType_uo;     /* Index to static Socket table */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    PduLengthType                   ifTxTcpPduLenReqByUL_uo;    /* To store the length of the packet requested by UL and it will be more necessary to support IF TCP Fanout feature */
    PduLengthType                   ifTxTcpAckedDataLen_uo;     /* To store the Acked data length from lower layer */
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

    PduLengthType                   ifTxCopiedLength_uo;        /* To store the length of buffer copied to TCPIP layer */
    PduLengthType                   reserved_uo;                /* reserved for byte alligned */

    PduIdType                       idxTxPduTable_uo;           /* Index of txPduId Table which needs to be remembered for tx confirmation*/
    PduIdType                       idxRxPduTable_uo;           /* Index of rxPduId Table which needs to be remembered for rx indication*/

    TcpIp_SocketIdType              socketHandleTcpIp_uo;       /* The Socket Handle of the dynamic socket table in TcpIp module*/
    uint16                          soConDynLocalPort_u16 ;     /* Local UDP or TCP port used for this connection in runtime */

    uint8                           rxOngoingCnt_u8;           /*count of ongoing receptions as RxIndication is reentrant */
    uint8                           reserved1_uo;             /* reserved for word alligned */
#if (SOAD_TP_TCP_PRESENT != STD_OFF)
    uint16                          tpTxTcpTransmitRetryCnt_u16;  /* Number of retries done by SoAd*/
    boolean                         tpTxTcpBufferAvailable_b;     /* Flag to indicate availability of buffer during TcpTransmit.*/
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

#if (SOAD_IF_PRESENT != STD_OFF )
    boolean                         ifTransmissionOnGoing_b;    /* flag to store the  IfTransmit request to be processed in main function */
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_TP_UDP_PRESENT != STD_OFF)
    boolean                         tpUdpTxTriggered_b;         /* TP Transmission is triggered */
#endif /* SOAD_TP_UDP_PRESENT != STD_OFF */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    boolean                         nPduUdpTxTriggered_b;       /* nPdu Transmission is triggered */
                                                                /* This flag is set before calling TcpIp_UdpTransmit() with NULL pointer in oder to know that */
                                                                /* the transmission is for nPdu and the UL data need to be get via UL_TriggerTransmit() API */
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if ( SOAD_IF_FANOUT_ENABLED != STD_OFF )
    boolean                         ifSkipFanOutTransmission_b;  /* flag to store the IfTransmit request needs to be processed in SoAd_IfTransmit() function after SoAd_ValidateAllPduRouteDestSocketsActive() function */
#endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

    boolean                         statusAbort_b;              /* To store the Abort status */
    boolean                         discardNewTxRxReq_b;        /* Status to store new transmission and reception shall be rejected[SWS_SoAd_00651] */
    boolean                         ifTpTxSoAdHdrCopied_b;      /* flag to store the whether SoAd header has been copied already for transmission */
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
	boolean                         ifRgTxOngoing_b;            /* flag to check IF rg Tx is ongoing */
    boolean                         ifRgTxPending_b;            /* flag to check IF rg Tx is pending */
#endif
#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
    boolean                         ZeroCopyConnChanged_b;      /* flag to store whether connection for ZeroCopy transmission changes or not */
#endif /*SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF*/

    boolean                         PostponeReleaseRemoteAddr_b;

#if( SOAD_TCP_TLS_ENABLED != STD_OFF )
    boolean                         isSecureConnection_b;      /* flag to indicate if connection is a secure connection */
#endif/* SOAD_TCP_TLS_ENABLED != STD_OFF  */

} SoAd_DyncSocConfigType_tst;


typedef struct
{
    const PduInfoType*    txPduInfoPtr_cpst;        /* The reference to Tx Packet Buffer */
	PduIdType             TxPduId_uo;
	SoAd_SoConIdType      idxStaticSoc_uo;            /* Index to static Socket table */
}SoAd_CopyTxMetaData_tst;


#if (SOAD_IF_PRESENT != STD_OFF )
/* These typedefs should be part of priv file*/
typedef struct SoAd_IfTxCallbackFunctions
{
    void (*TxCallback_pfn) (PduIdType TxPduId);
} SoAd_IfTxCallbacks_tst;

typedef struct SoAd_IfRxCallbackFunctions
{
    void (*RxCallback_pfn) (PduIdType           RxPduId, \
                            const PduInfoType * PduInfoPtr);
} SoAd_IfRxCallbacks_tst;
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF )
typedef struct SoAd_TpTxCallbackFunctions
{
    void (*TxCallback_pfn) (PduIdType      TxPduId ,\
                            Std_ReturnType result );
} SoAd_TpTxCallbacks_tst;

typedef struct SoAd_TpRxCallbackFunctions
{
    void (*RxCallback_pfn) (PduIdType      RxPduId ,\
                            Std_ReturnType result );
} SoAd_TpRxCallbacks_tst;

typedef struct SoAd_TpStartOfReceptionCbkFunctions
{
    BufReq_ReturnType (*StartOfReception_pfn) (PduIdType           RxPduId ,
                                               const PduInfoType * info,\
                                               PduLengthType       TpSduLength,\
                                               PduLengthType *     bufferSizePtr  );
} SoAd_TpStartOfReceptionCallbacks_tst;

typedef struct SoAd_TpcopyTxCallbackFunctions
{
    BufReq_ReturnType (*soAdCopyTxDataCbk_pfn) (PduIdType           id ,\
                                                const PduInfoType * info,\
                                                RetryInfoType *     retry,\
                                                PduLengthType *     availableDataPtr );
} SoAd_TpcopyTxDataCallbacks_tst;

typedef struct SoAd_TpcopyRxCallbackFunctions
{
    BufReq_ReturnType (*soAdCopyRxDataCbk_pfn) (PduIdType           id ,\
                                                const PduInfoType * info,\
                                                PduLengthType *     bufferSizePtr );
} SoAd_TpcopyRxDataCallbacks_tst;
#endif /* SOAD_TP_PRESENT != STD_OFF */

typedef struct SoAd_SoConModeChgCallbackFunctions
{
    void (*SoConModeChgCallback_pfn) ( SoAd_SoConIdType   SoConId,\
                                       SoAd_SoConModeType Mode );
} SoAd_SoConModeChgCallbacks_tst;

typedef struct SoAd_LocalIpAddrAsgnChgCbkFunctions
{
    void (*soAdLocalIpAddrAsgnChgCbk_pfn) (SoAd_SoConIdType      SoConId ,\
                                           TcpIp_IpAddrStateType State );
} SoAd_LocalIpAddrAsgnChgCbk_tst;

#if ( (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) )
typedef struct SoAd_IfTriggerTransmitCbkFunctions
{
    Std_ReturnType (*TriggerTransmit_pfn) (PduIdType     TxPduId,\
                                           PduInfoType * PduInfoPtr );
} SoAd_IfTriggerTransmitCallbacks_tst;
#endif /* ( (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) ) */

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF )
typedef struct SoAd_ZeroCopyTxConfirmCbkFunctions
{
    void (*ZeroCopyTxConfirm_pfn) (PduIdType TxPduId,\
                                   uint32    TxId_u32 );
} SoAd_ZeroCopyTxConfirmCallbacks_tst;
#endif /* (SOAD_ZERO_CEOPY_IF_UDP_PDU != STD_OFF ) */


/* DD: We will hold the IP address and port in a common structures since they are almost always read and written together.
       We won't reuse TcpIp_SockAddrInetType and TcpIp_SockAddrInet6Type since those structures also store the domain (which we don't need here and wastes memory). */
/* Structure to store an IPv4 address (together with port). */
typedef struct
{
    uint32              addr_u32;           /* IPv4 address */
    uint16              port_u16;           /* UDP or TCP port number             */
} SoAd_SockAddrInetType;

/* Structure to store an IPv6 address (together with port). */
typedef struct
{
    uint32              addr_au32[4];        /* IPv6 address */
    uint16              port_u16;           /* UDP or TCP port number             */
} SoAd_SockAddrInet6Type;

typedef struct
{
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    /* Routing Grp's in configuration */
    const boolean *                                 SoAd_RoutingGroup_ROM_cpb;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
    /* Triggerable Routing Grp's in configuration */
    const boolean *                                 SoAd_RoutingGrTriggerable_ROM_cpb;
#endif
#if ( (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) )
    /* List of callbacks for IfTriggerTransmit  */
    const SoAd_IfTriggerTransmitCallbacks_tst *     SoAd_IfTriggerTransmitCallbacks_cpst;
#endif /* ( (SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF) || (SOAD_NUM_nPDU_UDP_TX_SOCKET > 0) ) */

    /* Tx PDUs Configuration */
    const SoAd_TxPduConfigType_tst *                SoAd_TxPduConfig_cpst;

    /* Rx PDUs Configuration */
    const SoAd_RxPduConfigType_tst *                SoAd_RxPduConfig_cpst;

    /* Static SoAd Socket Configuration */
    const SoAd_StaticSocConfigType_tst *            SoAd_StaticSocConfig_cpst;

    /* List of all remote IPv4 address (together with port) for static sockets */
    const SoAd_SockAddrInetType *                   statSoConRemoteAddrInet_cpst;

    /* List of all remote IPv6 address (together with port) for dynamic sockets */
    const SoAd_SockAddrInet6Type *                  statSoConRemoteAddrInet6_cpst;

    /* Static SoAd SocketConnectionGroup Configuration */
    const SoAd_StaticSoConGrpConfigType_tst *       SoAd_StaticSocGroupConfig_cpst;

#if (SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF)
    /* TCP Keep Alive Time array in multiples of SoAd Main periods*/
    const uint32*                                   SoAd_Config_TcpKeepAliveTime_cpu32;

#if (SOAD_TCP_KEEP_ALIVE_INTERVAL_PRESENT != STD_OFF)
    /* TCP Keep Alive Interval array in multiples of SoAd Main periods*/
    const uint32*                                   SoAd_Config_TcpKeepAliveInterval_cpu32;
#endif /* SOAD_TCP_KEEP_ALIVE_INTERVAL_PRESENT != STD_OFF */

#if (SOAD_TCP_KEEP_ALIVE_PROBEMAX_PRESENT != STD_OFF)
    /* TCP Keep Alive Probe Max array in multiples of SoAd Main periods*/
    const uint16*                                   SoAd_Config_TcpKeepAliveProbeMax_cpu16;
#endif /* SOAD_TCP_KEEP_ALIVE_PROBEMAX_PRESENT != STD_OFF */
#endif /* SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
    /* index for Tx confirm  for Tx IF UDP Pdus*/
    const uint16 *                                  SoAdIf_UdpTxPduIndex_cpu16;
#endif /* SOAD_CONFIG_NUM_TX_IF_UDP_PDU > 0 */

#if ( SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 )
    /*  UdpAliveSupervisionTimeOut timer array   */
    const uint32 *                                  SoAd_Config_UdpAliveSupervisionTimeOut_cpu32;
#endif /* SOAD_CONFIG_NUM_TX_IF_UDP_PDU > 0 */

#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
    /* TP UDP TX Buffer array */
    const SoAd_TpUdpTxBufferInfo_tst *              SoAdTp_UdpTransmitBufferInfo_cpu8;
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* nPDU UDP TX Buffer array */
    SoAd_nPduUdpTxInfo_tst *                        SoAdIf_nPduUdpTransmitInfo_pst;
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if (SOAD_IF_PRESENT != STD_OFF )
    /* List of callbacks for IF-Tx Confirmation */
    const SoAd_IfTxCallbacks_tst *                  SoAdIf_TxCallbacks_cpst;

    /* List of callbacks for IF-Rx Indication  */
    const SoAd_IfRxCallbacks_tst *                  SoAdIf_RxCallbacks_cpst;
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF )
    /* List of callbacks for ZeroCopyTx Confirmation */
    const SoAd_ZeroCopyTxConfirmCallbacks_tst *     SoAdIf_ZeroCopyTxConfirms_cpst;
#endif /* SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF )
    /* List of callbacks for TP-Tx Confirmation */
    const SoAd_TpTxCallbacks_tst *                  SoAdTp_TxCallbacks_cpst;

    /* List of callbacks for TP-Rx Indication  */
    const SoAd_TpRxCallbacks_tst *                  SoAdTp_RxCallbacks_cpst;

    /* List of callbacks for TpRxStartofReception  */
    const SoAd_TpStartOfReceptionCallbacks_tst *    SoAdTp_StartOfReceptionCallbacks_cpst;

    /* List of callbacks for soc copy tx data  to upper layer */
    const SoAd_TpcopyTxDataCallbacks_tst *          SoAdTp_copyTxDataCallbacks_cpst;

    /* List of callbacks for soc copy Rx data to upper layer */
    const SoAd_TpcopyRxDataCallbacks_tst *          SoAdTp_copyRxDataCallbacks_cpst;
#endif /* SOAD_TP_PRESENT != STD_OFF */

    /* List of callbacks for soc connection mode change notification to upper layer */
    const SoAd_SoConModeChgCallbacks_tst *          SoAd_SoConModeChgCallbacks_cpst;

    /* List of callbacks for soc copy tx data to upper layer */
    const SoAd_LocalIpAddrAsgnChgCbk_tst *          SoAd_LocalIpAddrAsgnChgCbk_cpst;

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    /* List of SoConId mapping for each Routing Group Id */
     SoAd_RoutingGroupSoConStructMap_tst *          SoAd_RoutingGroupSoConStructMap_pst;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

     /* Info about TcpIp local address Id state and mapping of local address id in SoAd */
     SoAd_LocalAddrIdInfoType_tst *                 SoAd_LocalAddrIdInfo_past;

    /* Number of Static SoAd Socket in Configuration */
    const SoAd_SoConIdType                          SoAd_noStaticSockets_uo;

    /* Number of Static SoAd SocketConnectionGroup in Configuration */
    const SoAd_SoConIdType                          SoAd_noStaticSocketGroups_uo;

    /* Number of Active Socket in Configuration */
    const SoAd_SoConIdType                          SoAd_noActiveSockets_uo;

    /* Number of IPv4 Active Socket in Configuration */
    const SoAd_SoConIdType                          SoAd_noInetActiveSockets_uo;

    /* Number of Rx PDUs in Configuration */
    const PduIdType                                 SoAd_noRxPdus_uo;

    /* Number of Tx PDUs in Configuration */
    const PduIdType SoAd_noTxPdus_cuo;

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
    /* Number of UDP Tx IF PDUs in Configuration */
    const PduIdType SoAdIf_noUdpTxPdus_cuo;

    /* Reserved for future use and word aligned purpose */
    const PduIdType SoAd_reserved_uo;
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* Number of nPduUdpTxBufferSocket Configured */
    const PduIdType SoAdIf_noOfnPduUdpTxBuffers_cuo;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    /* Number of Routing Grp's in Configuration */
    const SoAd_RoutingGroupIdType SoAd_noRoutingGrps_cuo;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */


}SoAd_ConfigType;

#endif
