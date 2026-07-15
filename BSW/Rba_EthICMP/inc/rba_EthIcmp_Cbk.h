

#ifndef RBA_ETHICMP_CBK_H_
#define RBA_ETHICMP_CBK_H_

#if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) )

/*****************************************************************************************************
*                                   Prototypes
*****************************************************************************************************/
#define RBA_ETHICMP_START_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

extern void rba_EthIcmp_RxIndication(
                                uint32 RmtIPAddr_u32,
                                uint8 LocalAddrId_u8,
                                uint8 * RxData_pu8,
                                uint16 RxDataLen_u16,
                                boolean IsBroadcast_b );

#define RBA_ETHICMP_STOP_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

#endif /* #if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHICMP_CBK_H_ */





