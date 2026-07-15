

#ifndef TCPIP_TYPES_H
#define TCPIP_TYPES_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp_Cfg.h"      /* Required for TCPIP_CONFIGURED */

#ifdef TCPIP_CONFIGURED

#include "TcpIp_GeneralTypes.h"

#include "EthIf.h"

#if (TCPIP_IPV4_ENABLED == STD_ON)
#include "rba_EthIPv4_Types.h"
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
#include "rba_EthIPv6_Types.h"
#endif

#if (TCPIP_TCP_ENABLED == STD_ON)
#include "rba_EthTcp_Types.h"
#endif

#if (TCPIP_TLS_ENABLED == STD_ON)
#include "rba_EthTls_Types.h"
#endif

#if (TCPIP_UDP_ENABLED == STD_ON)
#include "rba_EthUdp_Types.h"
#endif

#if (TCPIP_ARP_ENABLED == STD_ON)
#include "rba_EthArp_Types.h"
#endif

#if (TCPIP_AUTOIP_ENABLED == STD_ON)
#include "rba_EthAutoIp_Types.h"
#endif

#if (TCPIP_ICMP_ENABLED == STD_ON)
#include "rba_EthIcmp_Types.h"
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCP_Types.h"
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCPv6_Types.h"
#endif


#if ( TCPIP_ICMPV6_ENABLED == STD_ON )
#include "rba_EthIcmpV6_Types.h"
#endif

#if ( TCPIP_NDP_ENABLED == STD_ON )
#include "rba_EthNdp_Types.h"
#endif


/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */

/* ZeroCopy - Mode of a ZeroCopyTx */
typedef enum
{
    TCPIP_ZEROCOPY_DISABLED         = 0x00U,    /* The ZeroCopy transmission is disabled */
    TCPIP_ZEROCOPY_STATIC_MODE      = 0x01U,    /* The ZeroCopy transmission executes in Static mode */
    TCPIP_ZEROCOPY_DYNAMIC_MODE     = 0x02U     /* The ZeroCopy transmission executes in Dynamic mode */
}TcpIp_ZeroCopyModeType_ten;

/* Duplicate Address Detection - Status of DAD */
typedef enum
{
    TCPIP_DAD_FAIL                   = 0U   /* Duplicate IP address was found by the Duplicate Address Detection(DAD) algorithm */
}TcpIp_DadStateType_ten;

/* This container is a sub-container of TcpIpLocalAddr and specifies the assignment policy for the IP address. */
typedef struct
{
    TcpIp_IpAddrAssignmentType                  AddrAsgnmentMeth_en;        /* Configured IP assignment Type (TcpIp_IpAddrAssignmentType)                                   */
    TcpIp_AssignmentTrigger_ten                 AddrAsgnmentTrig_en;        /* Configured IP assignment trigger  (TcpIpAssignmentTrigger)                               */
    TcpIp_AssignmentPrio_tu8                    AsgnmentPrio_u8;            /* Configured IP assignment priority (tcpIpAssignmentPriority). If a new address from an assignment method */
                                                                            /* with a higher priority is available,  it overwrites the IP address previously assigned by */
                                                                            /* an assignment method with a lower priority. Priority range - 1 .. 3   (1 is highest)      */
} TcpIp_AddrAssignment_tst;


/* This container is a sub-container of TcpIpLocalAddr and specifies a static IP address including directly related parameters. */
typedef struct
{
    uint32                                      IpAddr_u32;                 /* Assigned IPv4 address                                                                      */
    uint32                                      DftRtr_u32;                 /* Assigned IP address of default router (gateway)                                            */
    uint32                                      NetMask_u32;                /* Network mask of IPv4 address                                                               */
} TcpIp_IPv4AddrParamType_tst;


/** Structure to hold IPv6 addresses.*/
typedef struct
{
    uint32 Addr_au32[4];
} TcpIp_IPv6Addr_tst;


typedef struct
{
    TcpIp_IPv6Addr_tst                          IpAddr_st;                  /* Assigned  address                                                                  */
    TcpIp_IPv6Addr_tst                          DftRtr_st;                  /* Assigned IP address of default router (gateway)                                    */
    uint8                                       NetMask_u8;                 /* Prefix lenght of IPv6 (in CIDR notation)                                           */
} TcpIp_IPv6AddrParamType_tst;


/**
    IPversion specific IP addr parameters storage type.
    DD: use an union to allow both, IPv6 and IPv4 to be stored at the same time
        AND still to benefit from strong type checks of the C language.
    Alternative: use a void* which can be casted as needed. (less safe)
*/
typedef union
{
    TcpIp_IPv6AddrParamType_tst                 IPv6Par_st;
    TcpIp_IPv4AddrParamType_tst                 IPv4Par_st;
} TcpIp_IPAddrParamType_tun;

/* This structure stores the parameters related to Local IP address assignment/unassignment notifications received via API TcpIp_LocalIpAddrAssignmentChg(). */
typedef struct
{
    TcpIp_IpAddrAssignmentType                 ReqAssignType_en;                /* IP address assignment method which was notified as assigned. */
    TcpIp_AssignmentPrio_tu8                   ReqAssignPrio_u8;                /* Assigned IP address priority. */
    TcpIp_IPAddrParamType_tun                  ReqAssignAddressParam_un;        /* Assigned IP address parameters: IP address, default router IP address and netmask. */
    uint8                                      ReqUnassignType_u8;              /* IP address assignment method bit map, which was notified as unassigned. */
}TcpIp_LocalIpChgAssignedType_tst;

/* This structure stores the parameters related to IP address assignment/ unassignment requests. */
typedef struct
{
    uint8                                       RequestAddrAssgnMeth_u8;        /* Set by TcpIp_RequestIpAddrAssignment() for MANUAL IP assignment trigger, set before IP assignment        */
                                                                                /* process for AUTOMATIC IP assignment trigger and cleared after IP assignment is successfully initiated.   */
    uint8                                       ReleaseAddrAssgnMeth_u8;        /* Set by TcpIp_ReleaseIpAddrAssignment() API for MANUAL IP assignment trigger, set for Automatic IP        */
                                                                                /* assignment processes when TcpIpCtrl is requested for OFFLINE state.                                      */
    TcpIp_IPAddrParamType_tun                   * ManualStaticIpAddr_pun;       /* Pointer to IP address structure generated only with the default values (for trigger MANUAL)              */
                                                                                /* and used to store the requested address during TcpIp_RequestIpAddrAssignment.                            */
}TcpIp_LocalIpReqRelType_tst;

/* This structure stores the current assigned IP address related paramters. */
typedef struct
{
    TcpIp_IpAddrAssignmentType                  CurrAsgnedAddrMeth_en;          /* IP address method assigned is enabled in TcpIp_ReqComMode() and only then                */
                                                                                /*  API calls are accepted.                                                                 */
    TcpIp_AssignmentPrio_tu8                    CurrAsgnedAddrPrio_u8;          /* Current assigned IP address priority.                                                    */

    TcpIp_IPAddrParamType_tun                   IpAddrParams_un;                /* Stores the IP address, the default router address and the netmask value.                 */
} TcpIp_CurrAsgnedAddr_tst;

typedef struct
{
    /* Callout function called by TcpIp in case the Duplicate Address Detection (DAD) is enabled and detecting a duplicate IP Address. */
    void ( * Up_DADAddressConflict_pcft ) ( TcpIp_LocalAddrIdType IpAddrId,
                                                        const TcpIp_SockAddrType* IpAddrPtr,
                                                        const uint8* LocalPhysAddrPtr,
                                                        const uint8* RemotePhysAddrPtr);
}TcpIp_Callouts_tst;

/* Variables are needed because binding or data transmission must not be in progress while there is closing of sockets in progress or vice-versa */
typedef struct
{
    uint16                                      BindingSockCount_u16;           /* Count indicating if there is binding of a UDP socket with same local addr Id and different socket id in progress */
    uint16                                      SendingDataCount_u16;           /* Count indicating if there is transmiting of data in progress */
    boolean                                     ClosingSockInProgress_b;        /* Flag indicating if there is closing of a UDP socket and releasing all related resourcesin progress               */
}TcpIp_SockOperations_tst;



/* This container is a sub-container of TcpIpConfig and specifies the local IP (Internet Protocol) addresses used for IP communication. */
typedef struct
{
    TcpIp_DomainType                            IpDomainType_u32;               /* Address family                                                                                           */
    TcpIp_AddressType                           IpAddressType_en;               /* Address type                                                                                             */
    TcpIp_CurrAsgnedAddr_tst                    * CurrAsgnedAddr_pst;           /* Specifies a dynamic IP address including directly related parameters                                     */
    const TcpIp_AddrAssignment_tst              * AddrAsgnment_pcst;            /* Specifies the assignment policy for the IP address                                                       */
    const TcpIp_IPAddrParamType_tun             * CfgStaticIpAddr_pcun;         /* Pointer to IP address structure which stores IP address parameters to be assigned with                   */
                                                                                /* pre-configured values, if TcpIpStaticIpAddressConfig is available and Assignment Type is STATIC.         */
    TcpIp_LocalIpChgAssignedType_tst            * LocalIpChgAssigned_pst;       /* Pointer to Local IP address change structure which stores the assignment and un-assignment notifications */
                                                                                /* received via API TcpIp_LocalIpAddrAssignmentChg().                                                       */
    TcpIp_LocalIpReqRelType_tst                 * LocalIpReqRel_pst;            /* Pointer to structure which stores local IP address assignment and un-assignment requests.                */
    uint8                                       EthIfCtrlRef_u8;                /* Reference to a TcpIpCtrl specifying the EthIf Controller where the IP address shall be                   */
                                                                                /* assigned and DEM errors that shall be reported in case of an error on this controller.                   */
    uint8                                       TcpIpCtrlFramePrio_u8;          /* Frame priority configured per TcpIpCtrl (TcpIpIpFramePrioDefault)                                        */
    uint8                                       NumAddrAsgnment_u8;             /* Number of IP address assignments in a TcpIpAddrAssignments container                                     */
    TcpIp_SockOperations_tst                    * SockOptStatus_pst;            /* Specifies the progress status of socket bind and Close and Data send operation                           */
#if (TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON)
    TcpIp_ZeroCopyModeType_ten                  ZeroCopyTxModeType_en;          /* Specifies the transmission mode type is Static or Dynamic if the Zero copy transmission is enabled       */
#endif
} TcpIp_LocalAddrConfig_to;

/* Structure to hold the function pointers to all the upper layer callback functions */
typedef struct
{
    BufReq_ReturnType (*Up_CopyTxData_pfct) ( TcpIp_SocketIdType SocketId,
                                              uint8 * BufPtr,
                                              uint16 BufLength );

    BufReq_ReturnType (*Up_CopyTxDataMetaData_pfct) ( TcpIp_SocketIdType SocketId_u16,
                                                      uint8 * BufPtr_pu8,
                                                      uint16 BufLength_u16,
                                                      const void * MetaDataPtr_pcv );

    void (*Up_LocalIpAddrAssignmentChg_pfct) ( TcpIp_LocalAddrIdType IpAddrId,
                                               TcpIp_IpAddrStateType State );

    void (*Up_RxIndication_pfct) ( TcpIp_SocketIdType SocketId,
                                   const TcpIp_SockAddrType * RemoteAddrPtr,
                                   uint8 * BufPtr,
                                   uint16 Length );

    Std_ReturnType (*Up_TcpAccepted_pfct) ( TcpIp_SocketIdType SocketId,
                                            TcpIp_SocketIdType SocketIdConnected,
                                            const TcpIp_SockAddrType * RemoteAddrPtr );

    void (*Up_TcpConnected_pfct) ( TcpIp_SocketIdType SocketId );

    void (*Up_TcpIpEvent_pfct) ( TcpIp_SocketIdType SocketId,
                                 TcpIp_EventType Event );

    void (*Up_TxConfirmation_pfct) ( TcpIp_SocketIdType SocketId,
                                     uint16 Length );

    void (*Up_ZeroCopyTxConfirmation_pfct) ( TcpIp_SocketIdType SocketId_u16,
                                             uint32 TxId_u32 );
}TcpIp_SockOwnerConfig_tst;

/* Configuration data structure of the TcpIp module. */
typedef struct
{
#if (TCPIP_UDP_ENABLED == STD_ON)
    const rba_EthUdp_Config_to                  * EthUdpConfig_pco;         /* Pointer to configuration data structure of the UDP module.                                 */
#endif

#if (TCPIP_TCP_ENABLED == STD_ON)
    const rba_EthTcp_Config_to                  * EthTcpConfig_pco;         /* Pointer to configuration data structure of the TCP module.                                 */
#endif

#if (TCPIP_TLS_ENABLED == STD_ON)
    const rba_EthTls_Config_to                  * EthTlsConfig_pco;         /* Pointer to configuration data structure of the TLS module.                                 */
#endif

#if (TCPIP_IPV4_ENABLED == STD_ON)
    const rba_EthIPv4_Config_to                 * EthIPv4Config_pco;        /* Pointer to configuration data structure of the IPv4 module.                                */
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    const rba_EthIPv6_Config_to                 * EthIPv6Config_pco;        /* Pointer to configuration data structure of the IPv6 module.                                */
#endif

#if (TCPIP_ARP_ENABLED == STD_ON)
    const rba_EthArp_Config_to                  * EthArpConfig_pco;         /* Pointer to configuration data structure of the ARP module.                                  */
#endif

#if (TCPIP_AUTOIP_ENABLED == STD_ON)
    const rba_EthAutoIp_Config_to               * EthAutoIpConfig_pco;      /* Pointer to configuration data structure of the AUTOIP module.                              */
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    const rba_EthDHCP_Config_to                 * EthDHCPConfig_pco;        /* Pointer to configuration data structure of the DHCP module.                                */
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
    const rba_EthDHCPv6_Config_to               * EthDHCPv6Config_pco;        /* Pointer to configuration data structure of the DHCP module.                                */
#endif

#if (TCPIP_ICMP_ENABLED == STD_ON)
    const rba_EthIcmp_Config_to                 * EthIcmpConfig_pco;
#endif

    const TcpIp_LocalAddrConfig_to              * EthLocalAddrConfig_paco;  /* Pointer to configuration data structure of the local IPv4 addresses.                       */

#if ( TCPIP_ICMPV6_ENABLED == STD_ON )
    const rba_EthIcmpV6_Config_to               * EthIcmpV6Config_pco;      /* Pointer to configuration data structure of the ICMPv6 module.                              */
#endif

#if ( TCPIP_NDP_ENABLED == STD_ON )
    const rba_EthNdp_Config_to                  * EthNdpConfig_pco;         /* Pointer to configuration data structure of the NDP module.                              */
#endif

    const TcpIp_LocalAddrIdType                 NumLocalAddrConfig_u8;      /* Number of localAddress configured and internally generated by TcpIp for IPv6 TcpIpCtrls ( link local and all node multicast). */

    const TcpIp_LocalAddrIdType                 NumLocalAddrUserConfigured_u8;      /* Number of localAddress manually configured by the user.                           */

    const TcpIp_SockOwnerConfig_tst             * SockOwnerConfig_pacst;    /* Pointer to configuration data structure containing upper layer callback function pointers */

    const uint8                                 NumSockOwners_cu8;          /* Number of upper layers to TcpIp configured */

    const TcpIp_Callouts_tst                    * CalloutsConfig_pcst;     /* Pointer to configuration data structure containing upper layer callout function pointers */

} TcpIp_ConfigType;


typedef struct
{
    uint32                                      TcpIpMainFunctionPeriod;
} TcpIp_GeneralConfig_to;


/* Structure used by APIs to specify IP header content during TX */
typedef struct
{
    TcpIp_ProtocolType                          ProtoType_en;               /* protocol type: Udp/Tcp/Icmp   */
    uint32                                      FlowLabel_u32;              /* IPv6 Flow label field value, not used in case of IPv4  */
    uint8                                       Ttl_u8;                     /* time to live value            */
    uint8                                       SockOwner_u8;               /* Specifies which upper layer requested for transmission */
    uint8                                       DiffServiceCodepoint_u8;    /* DSCP field value for the IPv6 header, not used in case of IPv4  */
} TcpIp_IpHeader_tst;

/* Used for IPv6 standard header manipulation in RX Indication */
typedef struct
{
    uint16                                      PayloadLen_u16;             /* IPv6 Payload length                                                               */
    uint8                                       NextHeader_u8;              /* Next header within the standard IPv6 header                                       */
    uint8                                       HopLimit_u8;                /* Hop Limit                                                                         */
    TcpIp_IPv6Addr_tst                          SrcAddr_st;                 /* Source IPv6 address                                                               */
    TcpIp_IPv6Addr_tst                          DestAddr_st;                /* Destination IPv6 address                                                          */
} TcpIp_IPv6StdHdr_tst;

/* Union to store either an IPv4 address or a pointer to an IPv6 address. */
typedef union
{
    uint32                                      IPv4Addr_u32;
    const uint32*                               IPv6Addr_pcu32;
} TcpIp_IpAddr_tun;

/* Structure contains pseudo header information ( protocol type, local IP and Remote IP in Tx/Rx buffer) required for Udp and Tcp checksum calculation */
typedef struct
{
    TcpIp_DomainType                            IpDomainType_t;             /* AF_INET or AF_INET6                                                                      */
    TcpIp_ProtocolType                          ProtoType_en;               /* Transport layer protocol                                                                 */
    TcpIp_IpAddr_tun                            LocalIpAddr_un;             /* The local IP address (IPv4 or IPv6).                                                     */
    TcpIp_IpAddr_tun                            RemoteIpAddr_un;            /* The local IP address (IPv4 or IPv6).                                                     */
    uint16                                      IPHdrLen_u16;               /* IP header length                                                                         */
    boolean                                     IsRxFragmented_b;           /* Indication from IPv4 layer to transport layer that the received frame is reassembled one.*/
} TcpIp_PseudoHdr_tst;

/* Union to hold the index of either local AddrId or EthIf controller */
typedef union
{
    TcpIp_LocalAddrIdType                       LocalAddrId_u8;             /* Local Address Id */
    uint8                                       EthIfCtrlIdx_u8;            /* EthIf controller index */
} TcpIp_LocalAddrId_EthIfCtrl_tun;

#endif /* #ifdef TCPIP_CONFIGURED */
#endif /* TCPIP_TYPES_H */
