

#ifndef RBA_ETHDHCPCLIENT_H
#define RBA_ETHDHCPCLIENT_H

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )


/*
****************************************************************************************************
*                                   Includes
****************************************************************************************************
*/

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

/*
****************************************************************************************************
*                                   Prototypes
****************************************************************************************************
*/

#define RBA_ETHDHCP_START_SEC_CODE
#include "rba_EthDHCP_MemMap.h"

extern void rba_EthDHCP_ClientInit( const rba_EthDHCP_ClntConfig_tst * rba_EthDHCP_ClientConfig_cst );

extern Std_ReturnType rba_EthDHCP_ClientStart( TcpIp_LocalAddrIdType lLocalAddrId_tu8  );

extern Std_ReturnType rba_EthDHCP_ClientStop( TcpIp_LocalAddrIdType lLocalAddrId_tu8  );

extern void rba_EthDHCP_ClientMainFunction( void );

extern void rba_EthDHCP_ClientRxIndication( TcpIp_SocketIdType lSocketIdOut_u16,
                                            const TcpIp_SockAddrType * lIPAddrPort_pst,
                                            const uint8 * lBufPtr_pu8,
                                            uint16 lLength_u16 );

extern Std_ReturnType rba_EthDHCP_ClientIpConflictDetected( uint8 lCtrlIdx_u8 );

extern Std_ReturnType rba_EthDHCP_ClientGetIpAddress( uint8 lCtrlIdx_u8,
                                                      uint32 * lLocalIpAddress_pu32,
                                                      TcpIp_IpAddrStateType * lCurrentIpState_pen);

extern Std_ReturnType rba_EthDHCP_ClientReadOption( TcpIp_LocalAddrIdType lLocalIpAddrId_u8,
                                                    uint8 lOption_u8,
                                                    uint8 * lDataLength_pu8,
                                                    uint8 * lDataPtr_pu8);

extern Std_ReturnType rba_EthDHCP_ClientWriteOption( TcpIp_LocalAddrIdType lLocalIpAddrId_u8,
                                                     uint8 lOption_u8,
                                                     uint8 lDataLength_u8,
                                                     const uint8 * lDataPtr_pu8);
#define RBA_ETHDHCP_STOP_SEC_CODE
#include "rba_EthDHCP_MemMap.h"

#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHDHCPCLIENT_H */

