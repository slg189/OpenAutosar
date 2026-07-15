

#ifndef RBA_ETHICMP_H
#define RBA_ETHICMP_H

#if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) )

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

/* Vendor Id */
#define RBA_ETHICMP_VENDOR_ID                   6U

/* rba_EthIcmp module Id */
#define RBA_ETHICMP_MODULE_ID                   255U

/* rba_EthIcmp instance Id */
#define RBA_ETHICMP_INSTANCE_ID                 172U

/* SOFTWARE VERSION INFORMATION */
#define RBA_ETHICMP_SW_MAJOR_VERSION                10U
#define RBA_ETHICMP_SW_MINOR_VERSION                0U
#define RBA_ETHICMP_SW_PATCH_VERSION                0U
/*End: Common Published information*/

/*****************************************************************************************************
*                                   Prototypes
*****************************************************************************************************/
#define RBA_ETHICMP_START_SEC_CODE
#include "rba_EthIcmp_MemMap.h"
extern void rba_EthIcmp_Init(
                                const rba_EthIcmp_Config_to * IcmpConfig_pco );

extern  Std_ReturnType rba_EthIcmp_Transmit (
                                TcpIp_LocalAddrIdType LocalAddrId_u8,
                                const TcpIp_SockAddrInetType * RemoteAddr_cpst,
                                uint8 Ttl_u8,
                                uint8 Type_u8,
                                uint8 Code_u8,
                                uint16 DataLength_u16,
                                const uint8 * TxData_cpu8 );

#define RBA_ETHICMP_STOP_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

#endif /* #if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHICMP_H */
