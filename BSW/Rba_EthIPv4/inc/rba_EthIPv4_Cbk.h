

#ifndef RBA_ETHIPV4_CBK_H
#define RBA_ETHIPV4_CBK_H

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

#define RBA_ETHIPV4_START_SEC_CODE
#include "rba_EthIPv4_MemMap.h"
extern void rba_EthIPv4_RxIndication(
                                        uint8 EthIfCtrlIdx_u8,
                                        boolean IsBroadcast_b,
                                        uint8 * RmtPhysAddr_pu8,
                                        uint8 * RxData_pu8,
                                        uint16 RxDataLen_u16  );
#define RBA_ETHIPV4_STOP_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHIPV4_CBK_H */

