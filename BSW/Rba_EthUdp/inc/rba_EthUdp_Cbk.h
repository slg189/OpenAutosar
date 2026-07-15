

#ifndef RBA_ETHUDP_CBK_H
#define RBA_ETHUDP_CBK_H

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"
extern void rba_EthUdp_RxIndication( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                     TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                     uint8 * RxData_pu8,
                                     uint16 RxDataLen_u16,
                                     boolean IsBroadcast_b );
#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif  /* ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHUDP_CBK_H */

