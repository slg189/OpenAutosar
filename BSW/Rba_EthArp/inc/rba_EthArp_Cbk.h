
#ifndef RBA_ETHARP_CBK_H_
#define RBA_ETHARP_CBK_H_

#if ( defined(TCPIP_ARP_ENABLED) && ( TCPIP_ARP_ENABLED == STD_ON ) )

#define RBA_ETHARP_START_SEC_CODE
#include "rba_EthArp_MemMap.h"
extern Std_ReturnType rba_EthArp_RxIndication( uint8 lCtrlIdx_u8,
                                               const uint8 * lRxDataPtr,
                                               uint16 lRxDataLen_u16  );
#define RBA_ETHARP_STOP_SEC_CODE
#include "rba_EthArp_MemMap.h"

#endif /* #if ( defined(TCPIP_ARP_ENABLED) && ( TCPIP_ARP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHARP_CBK_H_ */


