

#ifndef TCPIP_PRV_H
#define TCPIP_PRV_H

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#include "TcpIp_Cfg.h"
#ifdef TCPIP_CONFIGURED

#include "TcpIp_Types.h"
#include "TcpIp_Prv_Types.h"

#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif


/*
***************************************************************************************************
* Defines
***************************************************************************************************
*/

/* DHCP Client port number defined by IANA */
#define TCPIP_DHCPSERVER_PORT                       67U

/* DHCP Server port number defined by IANA */
#define TCPIP_DHCPCLIENT_PORT                       68U

/* Source port offset in Transport Protocol header */
#define TCPIP_TP_SRCPORT_OFFSET                     0U

/* Destination port offset in Transport Protocol header*/
#define TCPIP_TP_DESTPORT_OFFSET                    2U

/* Offset to be added to Udp socket index to give the feeling of single socket table being used for Tcp and Udp */
#define TCPIP_UDP_SOCKET_OFFSET                     TCPIP_TCPSOCKETMAX

/* 169.254.0.0 */
#define TCPIP_AUTOIP_NET                            0xA9FE0000UL

/* 169.254.1.0 */
#define TCPIP_AUTOIP_RANGE_START                    (TCPIP_AUTOIP_NET | 0x0100U)

/* 169.254.254.255 */
#define TCPIP_AUTOIP_RANGE_END                      (TCPIP_AUTOIP_NET | 0xFEFFU)

/* 169.254.255.255 */
#define TCPIP_AUTOIP_BROADCAST                      0xA9FEFFFFUL

/* Mask to check AutoIp range */
#define TCPIP_AUTOIP_MASK                           0xFFFF0000UL

#define TCPIP_NETMASK_MAX_VAL                       32U

/* IP address ANY */
#define     TCPIP_IPV4_ADDR_ANY                     0x00000000UL

/* Loopback address */
#define     TCPIP_IPV4_LOOPBACK                     0x7F000000UL

/* Mask to check loop back IP */
#define     TCPIP_IPV4_LOOPBACK_MASK                0xFF000000UL

/* Reserved IP range 240.0.0.0/4 */
#define     TCPIP_IPV4_RESERVED_IP                  0xF0000000UL

/* IPv6 generic multicast address mask (RFC 4291 - Section 2.7) */
#define     TCPIP_IPV6_MULTICAST_MASK               0xFF000000uL

/* IPv6 multicast SCOP flag mask (RFC 4291 - Section 2.7)  */
#define     TCPIP_IPV6_MULTICAST_SCOP_FLAG_MASK     0xFF0F0000uL

/* Reserved Multicast IPv6 addresses, based on the SCOP flag (RFC 4291 - Section 2.7) */
#define     TCPIP_IPV6_MULTICAST_RESERVED_SCOP_0   0xFF000000uL
#define     TCPIP_IPV6_MULTICAST_RESERVED_SCOP_3   0xFF030000uL
#define     TCPIP_IPV6_MULTICAST_RESERVED_SCOP_F   0xFF0F0000uL

/* MINIMUM MTU SIZE THAT IPv6 SHALL ALWAYS SUPPORT */
#define     TCPIP_IPV6_MINIMUM_MTU_SIZE            1280U

/* CallBack required or not */
#define TCPIP_CBK_NOT_REQD                         0x00U
#define TCPIP_CBK_REQD                             0xFFU

/* Checksum field offset */
#define TCPIP_UDPFRAMELENGTH_UB                    4U
#define TCPIP_UDPCHKSUM_UB                         6U
#define TCPIP_TCPCHKSUM_UB                         16U
#define TCPIP_ICMPCHKSUM_UB                        2U
#define TCPIP_UDPHDRLENGTH                         8U

#define TCPIP_MAXUINT16                            (0xFFFFU)
#define TCPIP_MAXUINT32                            (0xFFFFFFFFUL)
#define TCPIP_MINUINT32                            (0x0UL)

/* FramePrio is a 3 bit field in Vlan header, it's maximun value is 7 */
#define TCPIP_FRAMEPRIO_MAXVAL                     7U

/* FlowLabel is a 20 bit field in Vlan header, it's maximun value is 1048575 */
#define TCPIP_FLOWLABEL_MAXVAL                     (0x000FFFFFUL)

/* FlowLabel is a 6 bit field in IPv6 header, it's maximun value is 0x3FU (63). */
#define TCPIP_DSCP_MAXVAL                     (0x3FU)


/*
 ***************************************************************************************************
 * DET Defines
 ***************************************************************************************************
 */

/* API IDs for DET Support */

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_Init()  */
#define TCPIP_E_INIT_API_ID                                       (0x01U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetVersionInfo()  */
#define TCPIP_E_GET_VERSIONINFO_API_ID                            (0x02U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_Prv_GetSocket() */
#define TCPIP_E_PRV_GET_SOCKET_API_ID                             (0x03U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_Close() */
#define TCPIP_E_CLOSE_API_ID                                      (0x04U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_Bind() */
#define TCPIP_E_BIND_API_ID                                       (0x05U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_TcpConnect()  */
#define TCPIP_E_TCP_CONNECT_API_ID                                (0x06U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_TcpListen()     */
#define TCPIP_E_TCP_LISTEN_API_ID                                 (0x07U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_TcpReceived()  */
#define TCPIP_E_TCP_RECEIVED_API_ID                               (0x08U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_RequestComMode()  */
#define TCPIP_E_REQUEST_COM_MODE_API_ID                           (0x09U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_RequestIpAddrAssignment()   */
#define TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID                  (0x0AU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ReleaseIpAddrAssignment()   */
#define TCPIP_E_RELEASE_IP_ADDRASSIGNMENT_API_ID                  (0x0BU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_IcmpTransmit()    */
#define TCPIP_E_ICMP_TRANSMIT_API_ID                              (0x0CU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_DhcpReadOption()   */
#define TCPIP_E_DHCP_READOPTION_API_ID                            (0x0DU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_DhcpWriteOption()   */
#define TCPIP_E_DHCP_WRITEOPTION_API_ID                           (0x0EU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ChangeParameter()   */
#define TCPIP_E_CHANGE_PARAMETER_API_ID                           (0x0FU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetIpAddr()   */
#define TCPIP_E_GET_IPADDR_API_ID                                 (0x10U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetPhysAddr()   */
#define TCPIP_E_GET_PHYSADDR_API_ID                               (0x11U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_UdpTransmit()   */
#define TCPIP_E_UDP_TRANSMIT_API_ID                               (0x12U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_TcpTransmit()   */
#define TCPIP_E_TCP_TRANSMIT_API_ID                               (0x13U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_RxIndication()   */
#define TCPIP_E_RXINDICATION_API_ID                               (0x14U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_MainFunction()   */
#define TCPIP_E_MAIN_FUNCTION_API_ID                              (0x15U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetCtrlIdx()     */
#define TCPIP_E_GET_CTRLIDX_API_ID                                (0x17U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_AllocBaseIpAddr()   */
#define TCPIP_E_ALLOC_BASE_IPADDR_API_ID                          (0x18U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ReadArpTable()   */
#define TCPIP_E_READ_ARPTABLE_API_ID                              (0x19U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_OnesComplChkSum()  */
#define TCPIP_E_ONES_COMPL_CHKSUM_API_ID                          (0x1AU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_LocalIpAddrAssignmentChg()  */
#define TCPIP_E_LOCAL_IP_ADDRASSIGNMENTCHG_API_ID                 (0x1BU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_SubnetMskChkRemoteIpAddr() */
#define TCPIP_E_SUBNETMSK_CHK_REMOTEIPADDR_API_ID                 (0x21U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ChkComAllowed() */
#define TCPIP_E_CHK_COMALLOWED_API_ID                             (0x22U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetIPAddrAsgnMethFrmCrtlIdx() */
#define TCPIP_E_GET_IPADDR_ASGNMETH_FRM_CRTLIDX_API_ID            (0x28U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetIPv4AddrFrmCtrlIdx() */
#define TCPIP_E_GET_IPV4ADDR_FRM_CTRLIDX_API_ID                   (0x2CU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetLocalAddrIdForRxFrame() */
#define TCPIP_E_GET_LOCALADDRID_FOR_RXFRAME_API_ID                (0x2DU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_CheckValidityOfLocalAddrId() */
#define TCPIP_E_CHECK_VALIDITY_OF_LOCALADDRID_API_ID              (0x2FU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ReadU16() */
#define TCPIP_E_READ_U16_API_ID                                   (0x32U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ReadU32() */
#define TCPIP_E_READ_U32_API_ID                                   (0x33U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_WriteU16() */
#define TCPIP_E_WRITE_U16_API_ID                                  (0x34U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_WriteU32() */
#define TCPIP_E_WRITE_U32_API_ID                                  (0x35U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetFramePrioFrmLocalAddrId() */
#define TCPIP_E_GET_FRAMEPRIO_FRM_LOCALADDRID_API_ID              (0x36U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetLocalAddrIdProperties()   */
#define TCPIP_E_GET_LOCALADDRID_PROPERTIES_API_ID                 (0x39U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_IPv6AddrCompareWithMask() */
#define TCPIP_E_IPV6_ADDR_COMPARE_WITH_MASK_API_ID                (0x40U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_IsIPv6AddrEqualToNetworkAddr() */
#define TCPIP_E_IS_IPV6_ADDR_EQUAL_TO_NETWORK_ADDR_ID             (0x41U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_RandNumXor64() */
#define TCPIP_E_RANDNUM_XOR_64_API_ID                             (0x43U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetEthIfCtrlState() */
#define TCPIP_E_GET_ETHIF_CTRL_STATE_API_ID                       (0x44U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_GetAndResetMeasurementData() */
#define TCPIP_E_GETANDRESET_MEAS_DATA_API_ID                      (0x45U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_IsIPv6LinkLocalAddress() */
#define TCPIP_E_ISIPV6LINKLOCALADDRESS_API_ID                     (0x46U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ZeroCopyTransmit() */
#define TCPIP_E_ZEROCOPYTRANSMIT_API_ID                           (0x80U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ZeroCopyReset() */
#define TCPIP_E_ZEROCOPYRESET_API_ID                              (0x81U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_ZeroCopyTxConfirmation() */
#define TCPIP_E_ZEROCOPYTXCONFIRMATION_API_ID                     (0x82U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_DuplicateAddressIndication() */
#define TCPIP_E_DUPLICATEADDRESSINDICATION_API_ID                 (0x83U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to TcpIp_UdpTransmitMetaData() */
#define TCPIP_E_UDP_TRANSMIT_METADATA_API_ID                      (0x84U)

/* Development Error Codes for DET Support */
#define TCPIP_E_NOTINIT                                           0x01U       /* API service called before initializing the module */
#define TCPIP_E_NULL_PTR                                          0x02U       /* API service called with NULL pointer */
#define TCPIP_E_INV_ARG                                           0x03U       /* Invalid argument */
#define TCPIP_E_NOBUFS                                            0x04U       /* No buffer space available */
#define TCPIP_E_INV_SOCKADDR                                      0x05U       /* Invalid socket address */
#define TCPIP_E_DESTADDRREQ                                       0x06U       /* Destination address required */
#define TCPIP_E_MSGSIZE                                           0x07U       /* Message too long */
#define TCPIP_E_PROTOTYPE                                         0x08U       /* Protocol wrong type for socket */
#define TCPIP_E_ADDRINUSE                                         0x09U       /* Address already in use */
#define TCPIP_E_ADDRNOTAVAIL                                      0x0AU       /* Can't assign requested address */
#define TCPIP_E_ISCONN                                            0x0BU       /* Socket is already connected */
#define TCPIP_E_NOTCONN                                           0x0CU       /* Socket is not connected */
#define TCPIP_E_NOPROTOOPT                                        0x0DU       /* Protocol not available */
#define TCPIP_E_AFNOSUPPORT                                       0x0EU       /* Address family not supported by protocol family */
#define TCPIP_E_INIT_FAILED                                       0x0FU       /* Invalid configuration set selection */

#define TCPIP_E_DADCONFLICT                                       0x10U       /* Duplicate address detected */

/*
 ***************************************************************************************************
 * DET Check
 ***************************************************************************************************
 */

/* No reporting to DET module if Development Detection Error is OFF */
/* According to CAP Coding Guideline, Development Error detection shall not be controlled by configuration switch DevErrorDetect for safety reasons. */
/* This behavior is compliant to runtime-error behavior and a deviation from AUTOSAR */
#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#define TCPIP_CALL_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(TCPIP_MODULE_ID, TCPIP_INSTANCE_ID, API, ERROR);
#else
#define TCPIP_CALL_DET_REPORTERROR(API, ERROR)
#endif

/* DET Macro for functions with return type as 'void' */
#define TCPIP_DET_REPORT_ERROR_RET_VOID( CONDITION, API, ERROR )            \
    if(CONDITION)                                                           \
    {                                                                       \
        TCPIP_CALL_DET_REPORTERROR(API, ERROR)                              \
        return;                                                             \
    }                                                                       \

/* DET Macro for functions with a return value */
#define TCPIP_DET_REPORT_ERROR_RET_VALUE( CONDITION, API, ERROR, RETVAL )   \
    if(CONDITION)                                                           \
    {                                                                       \
        TCPIP_CALL_DET_REPORTERROR(API, ERROR)                              \
        return (RETVAL);                                                    \
    }                                                                       \

/*  Macro For functions to report DET but does not return to calling function */
#define TCPIP_DET_REPORT_ERROR_NO_RET( CONDITION, API, ERROR )              \
    if(CONDITION)                                                           \
    {                                                                       \
        TCPIP_CALL_DET_REPORTERROR(API, ERROR)                              \
    }                                                                       \

/*  Macro for functions to report DET always, but does not return to calling function */
#define TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( API, ERROR )                    \
    TCPIP_CALL_DET_REPORTERROR(API, ERROR)                                  \

/* DET Macro for functions to report DET always, with a return value  */
#define TCPIP_DET_REPORT_ERROR_TRUE_RET_VALUE( API, ERROR, RETVAL )         \
    TCPIP_CALL_DET_REPORTERROR(API, ERROR)                                  \
    return (RETVAL);                                                        \


/*
 ***************************************************************************************************
 * Macros
 ***************************************************************************************************
 */

/* Set a single bit in uint8 variable */
#define TCPIP_UINT8_SET_BIT_WITH_SHIFT( Variable, Shift )    \
        Variable |= (uint8)( (uint8)(1U << (Shift)) )        \

/* Clear a single bit in uint8 variable */
#define TCPIP_UINT8_CLEAR_BIT_WITH_SHIFT( Variable, Shift )  \
        Variable &= (uint8)( ~(uint8)(1U << (Shift)) )       \

/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_VAR_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"
extern const TcpIp_ConfigType *          TcpIp_CurrConfig_pco;
#define TCPIP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_VAR_INIT_8
#include "TcpIp_MemMap.h"
extern boolean                           TcpIp_InitFlag_b;
#define TCPIP_STOP_SEC_VAR_INIT_8
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_VAR_INIT_32
#include "TcpIp_MemMap.h"
extern uint32                            TcpIp_RandNumSeed1_u32;
extern uint32                            TcpIp_RandNumSeed2_u32;
#define TCPIP_STOP_SEC_VAR_INIT_32
#include "TcpIp_MemMap.h"

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
#define TCPIP_START_SEC_VAR_FAST_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"
extern TcpIp_ZeroCopyDescriptor_tst      TcpIp_ZeroCopyDescriptors_ast[TCPIP_ZEROCOPY_DESCRIPTOR_COUNT];
extern uint16                            TcpIp_DynSockAllocatedZeroCopyDesc_au16[TCPIP_SOCKETMAX];
#define TCPIP_STOP_SEC_VAR_FAST_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"
#endif


/*
 ***************************************************************************************************
 * Private functions used only by the TcpIp module
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern void TcpIp_StateMachine( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                const TcpIp_StateType * EthIfCtrlStBeforeTrans_caen,
                                uint8 * AssgnedLocalAddrPerEthIfCtrl_au8 );

extern void TcpIp_IpReqRelMainFuncProc( TcpIp_LocalAddrIdType LocalAddrId_u8 );

extern void TcpIp_LocalIpChgMainFuncProc( TcpIp_LocalAddrIdType LocalAddrId_u8 );

extern Std_ReturnType TcpIp_Prv_GetSocket( TcpIp_DomainType Domain,
                                           TcpIp_ProtocolType Protocol,
                                           TcpIp_SocketIdType * SocketIdPtr,
                                           uint8 SocketOwner );

extern Std_ReturnType TcpIp_UpdateMulticastPhysFilter( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                                       Eth_FilterActionType FilterActionType_en,
                                                       const TcpIp_IPAddrParamType_tun * IPAddrParam_pcun);

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
extern Std_ReturnType TcpIp_ZeroCopyIsResetAllowed( uint16 DescId_u16 );

extern void TcpIp_ZeroCopyClearDesc( uint16 DescId_u16 );
#endif

#define TCPIP_STOP_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

/*
 ***************************************************************************************************
 * Private functions provided to the sub-modules of TcpIp
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern Std_ReturnType TcpIp_CheckValidityOfLocalAddrId( TcpIp_LocalAddrIdType LocalAddrId_u8 );

extern Std_ReturnType TcpIp_GetFramePrioFrmLocalAddrId( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                        uint8 * FramePrio_pu8 );

extern TcpIp_StateType TcpIp_GetEthIfCtrlState( uint8 EthIfCtrlIdx_u8 );

extern Std_ReturnType TcpIp_GetIPAddrAsgnMethFrmCrtlIdx( uint8 EthIfCtrlIdx_u8 ,
                                                         TcpIp_IpAddrAssignmentType * IpAddrAsgnMethType_pen );

extern Std_ReturnType TcpIp_ChkComAllowed( TcpIp_LocalAddrIdType LocalAddrId_u8 );

extern Std_ReturnType TcpIp_GetLocalAddrIdProperties( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                      const TcpIp_LocalAddrConfig_to * *TcpIp_LocalAddrConfig_ppcst );

#if ( TCPIP_IPV4_ENABLED == STD_ON )
extern Std_ReturnType TcpIp_GetIPv4AddrFrmCtrlIdx( uint8 EthIfCtrlIdx_u8 ,
                                                   uint32 * SrcIPv4Addr_pu32,
                                                   uint32 * NetMask_pu32 );


extern Std_ReturnType TcpIp_SubnetMskChkRemoteIpAddr( uint8 EthIfCtrlIdx_u8,
                                                      uint32 RemoteIPv4Addr_u32,
                                                      TcpIp_IPv4AddrType_ten * IPv4AddrType_pen );

extern Std_ReturnType TcpIp_GetLocalAddrIdForRxFrame( uint8 EthIfCtrlIdx_u8,
                                                      uint32 IPv4Addr_u32,
                                                      TcpIp_LocalAddrIdType * LocalAddrId_pu8 );
#endif

#if( TCPIP_IPV6_ENABLED == STD_ON )
Std_ReturnType TcpIp_IsIPv6LinkLocalAddress( TcpIp_LocalAddrIdType LocalAddrId_u8 );

extern boolean TcpIp_IPv6AddrCompareWithMask( const uint32 * IPv6Addr1_pcu32,
                                              const uint32 * IPv6Addr2_pcu32,
                                              uint8 Netmask_u8 );

extern boolean TcpIp_IsIPv6AddrEqualToNetworkAddr( const uint32 * IPv6Addr_pcu32,
                                                   uint8 Netmask_u8 );
#endif

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

#endif /* TCPIP_PRV_H */
