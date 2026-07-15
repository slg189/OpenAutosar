

#ifndef RBA_ETHTCP_CBK_H
#define RBA_ETHTCP_CBK_H

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

extern void rba_EthTcp_RxIndication ( const TcpIp_PseudoHdr_tst * PseudoHdr_cpst,
                                      TcpIp_LocalAddrIdType       LocalAddrId_u8,
                                      uint8 *                     RxData_pu8,
                                      uint16                      LenByte_u16 );

#if( TCPIP_TLS_ENABLED == STD_ON )
extern void rba_EthTcp_TlsEvent( TcpIp_SocketIdType  TcpIpSocId_uo,
                                 TcpIp_EventType     Event_en );

extern void rba_EthTcp_TlsRxIndication( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                        uint8 *            DecryptedData_pu8,
                                        uint16             DecryptedDataLen_u16 );
#endif/*  TCPIP_TLS_ENABLED == STD_ON  */

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


#endif  /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHTCP_CBK_H */


