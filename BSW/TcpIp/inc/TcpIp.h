

#ifndef TCPIP_H
#define TCPIP_H

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#include "TcpIp_Cfg.h"              /* Required for TCPIP_DEV_ERROR_DETECT */

#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cbk.h"
#include "TcpIp_Types.h"
#include "TcpIp_PbCfg.h"


/*
***************************************************************************************************
* Defines
***************************************************************************************************
*/

/* Vendor ID */
#define TCPIP_VENDOR_ID                         6u
/* Module ID */
#define TCPIP_MODULE_ID                         170u
/* Instance ID */
#define TCPIP_INSTANCE_ID                       0u

/* AUTOSAR SPECIFICATION VERSION */
#define TCPIP_AR_RELEASE_MAJOR_VERSION          4u
#define TCPIP_AR_RELEASE_MINOR_VERSION          2u
#define TCPIP_AR_RELEASE_REVISION_VERSION       2u

/* SOFTWARE VERSION INFORMATION */
#define TCPIP_SW_MAJOR_VERSION                  10u
#define TCPIP_SW_MINOR_VERSION                  0u
#define TCPIP_SW_PATCH_VERSION                  0u

#define TCPIP_TYPE_ARP                          0x0806u
#define TCPIP_TYPE_IPV4                         0x0800u
#define TCPIP_TYPE_IPV6                         0x86DDu

/* List of values for index to select specific measurement data*/
#define TCPIP_MEAS_DROP_TCP                     0x01U
#define TCPIP_MEAS_DROP_UDP                     0x02U
#define TCPIP_MEAS_DROP_IPV4                    0x03U
#define TCPIP_MEAS_DROP_IPV6                    0x04U
#define TCPIP_MEAS_ALL                          0xFFU


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern void TcpIp_Init( const TcpIp_ConfigType * ConfigPtr );

#if( TCPIP_VERSION_INFO_API == STD_ON )
extern void TcpIp_GetVersionInfo( Std_VersionInfoType * versioninfo );
#endif

extern Std_ReturnType TcpIp_Bind( TcpIp_SocketIdType SocketId,
                                  TcpIp_LocalAddrIdType LocalAddrId,
                                  uint16 * PortPtr );

extern Std_ReturnType TcpIp_Close( TcpIp_SocketIdType SocketId,
                                   boolean Abort );

#if ( TCPIP_UDP_ENABLED == STD_ON )
extern TcpIp_ReturnType TcpIp_UdpTransmit ( TcpIp_SocketIdType SocketId,
                                            const uint8 * DataPtr,
                                            const TcpIp_SockAddrType * RemoteAddrPtr,
                                            uint16 TotalLength );

/* Dedicated UdpTransmit API reentrant for the same socket */
/* MetaData argument is required because parallel execution of TcpIp_UdpTransmitMetaData can be triggered by UL */
TcpIp_ReturnType TcpIp_UdpTransmitMetaData( TcpIp_SocketIdType SocketId_u16,
                                            const uint8 * DataPtr_pcu8,
                                            const TcpIp_SockAddrType * RemoteAddrPtr_pcst,
                                            uint16 TotalLength_u16,
                                            const void * MetaDataPtr_pcv );
#endif /* ( TCPIP_UDP_ENABLED == STD_ON ) */


#if ( TCPIP_TCP_ENABLED == STD_ON )
extern Std_ReturnType TcpIp_TcpTransmit( TcpIp_SocketIdType SocketId,
                                         const uint8 * DataPtr,
                                         uint32 AvailableLength,
                                         boolean ForceRetrieve );

extern Std_ReturnType TcpIp_TcpReceived( TcpIp_SocketIdType SocketId,
                                         uint16 Length );

extern Std_ReturnType TcpIp_TcpConnect( TcpIp_SocketIdType SocketId,
                                        const TcpIp_SockAddrType * RemoteAddrPtr );

extern Std_ReturnType TcpIp_TcpListen( TcpIp_SocketIdType SocketId,
                                       uint16 MaxChannels );
#endif /* ( TCPIP_TCP_ENABLED == STD_ON ) */

#if ( TCPIP_ICMP_ENABLED == STD_ON )
extern Std_ReturnType TcpIp_IcmpTransmit( TcpIp_LocalAddrIdType LocalIpAddrId,
                                          const TcpIp_SockAddrType * RemoteAddrPtr,
                                          uint8 Ttl,
                                          uint8 Type,
                                          uint8 Code,
                                          uint16 DataLength,
                                          const uint8 * DataPtr );
#endif /* ( TCPIP_ICMP_ENABLED == STD_ON ) */


#if ( TCPIP_ARP_ENABLED == STD_ON )
extern Std_ReturnType TcpIp_ReadArpTable( TcpIp_LocalAddrIdType LocalAddrId_u8 ,
                                          uint8 * const ArpTable_cpu8,
                                          uint16 * const NumberEntries_cpu16 );
#endif /* ( TCPIP_ARP_ENABLED == STD_ON ) */

extern Std_ReturnType TcpIp_ChangeParameter( TcpIp_SocketIdType SocketId,
                                             TcpIp_ParamIdType ParameterId,
                                             const uint8 * ParameterValue );

/* If RTE is enabled, the main function declaration is present in SchM_TcpIp.h. */
#if( TCPIP_ECUC_RB_RTE_IN_USE == STD_OFF)
extern void TcpIp_MainFunction( void );
#endif

extern Std_ReturnType TcpIp_RequestIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId,
                                                     TcpIp_IpAddrAssignmentType Type,
                                                     const TcpIp_SockAddrType * LocalIpAddrPtr,
                                                     uint8 Netmask,
                                                     const TcpIp_SockAddrType * DefaultRouterPtr );

extern Std_ReturnType TcpIp_ReleaseIpAddrAssignment( const TcpIp_LocalAddrIdType LocalAddrId );

extern Std_ReturnType TcpIp_GetIpAddr( TcpIp_LocalAddrIdType LocalAddrId,
                                       TcpIp_SockAddrType * IpAddrPtr,
                                       uint8 * NetmaskPtr,
                                       TcpIp_SockAddrType * DefaultRouterPtr );

extern Std_ReturnType TcpIp_GetPhysAddr( TcpIp_LocalAddrIdType LocalAddrId,
                                         uint8 * PhysAddrPtr );

#if (  TCPIP_DHCP_CLIENT_ENABLED == STD_ON  )
extern Std_ReturnType TcpIp_DhcpWriteOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                             uint8 Option,
                                             uint8 DataLength,
                                             const uint8 * DataPtr );

extern Std_ReturnType TcpIp_DhcpReadOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                            uint8 Option,
                                            uint8 * DataLength,
                                            uint8 * DataPtr );
#endif /* ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) */

extern Std_ReturnType TcpIp_AllocBaseIpAddr( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                             TcpIp_IpAddrAssignmentType IpAddrAsgnMethType_en,
                                             const TcpIp_SockAddrType * LocalIpAddr_pst );

extern Std_ReturnType TcpIp_GetCtrlIdx( TcpIp_LocalAddrIdType LocalAddrId,
                                        uint8 * CtrlIdxPtr );

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
/* Allows to read and reset measurement data for diagnostic purposes. */
extern Std_ReturnType TcpIp_GetAndResetMeasurementData( TcpIp_MeasurementIdxType MeasurementIdx,
                                                        boolean MeasurementResetNeeded,
                                                        uint32 * MeasurementDataPtr);
#endif

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
extern TcpIp_ReturnType TcpIp_ZeroCopyUdpTransmit ( TcpIp_SocketIdType SocketId_u16,
                                                    const Eth_ZeroCopyTxDataInfo_tst * ZeroCopyData_pcst,
                                                    const TcpIp_SockAddrType * RemoteAddr_pcst,
                                                    boolean TxConfFlag_b,
                                                    boolean ConnChanged_b,
                                                    uint32 * TxId_pu32 );

extern void TcpIp_ZeroCopyReset( TcpIp_SocketIdType SocketId_u16 );
#endif

#define TCPIP_STOP_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

#endif /* TCPIP_H */

