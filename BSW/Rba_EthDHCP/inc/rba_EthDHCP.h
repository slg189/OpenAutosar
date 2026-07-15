

#ifndef RBA_ETHDHCP_H
#define RBA_ETHDHCP_H

/*
****************************************************************************************************
*                                   Includes
****************************************************************************************************
*/
#include "rba_EthDHCP_Client.h"

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/


/* Vendor Id */
#define RBA_ETHDHCP_VENDOR_ID                   6U

/* Module ID for reporting DET error, as AutoIp is a SubModule of TCPIP */
#define RBA_ETHDHCP_MODULE_ID               255U

/* Instance Id for Reporting the DET error */
#define RBA_ETHDHCP_INSTANCE_ID             (0xAF)


/* SOFTWARE VERSION INFORMATION */
#define RBA_ETHDHCP_SW_MAJOR_VERSION                10U
#define RBA_ETHDHCP_SW_MINOR_VERSION                0U
#define RBA_ETHDHCP_SW_PATCH_VERSION                0U
/*End: Common Published information*/


/*
****************************************************************************************************
*                                   Prototypes
****************************************************************************************************
*/
#define RBA_ETHDHCP_START_SEC_CODE
#include "rba_EthDHCP_MemMap.h"

extern void rba_EthDHCP_Init( const rba_EthDHCP_Config_to * rba_EthDHCP_Config_cst );

extern Std_ReturnType rba_EthDHCP_Start( TcpIp_LocalAddrIdType lLocalAddrId_tu8 );

extern Std_ReturnType rba_EthDHCP_Stop( TcpIp_LocalAddrIdType lLocalAddrId_tu8 );

extern void rba_EthDHCP_MainFunction( void );

extern void rba_EthDHCP_RxIndication( TcpIp_SocketIdType lSocketIdOut_u16,
                                      const TcpIp_SockAddrType * lIPAddrPort_ps,
                                      uint8 * lBufPtr_pu8,
                                      uint16 lLength_u16 );

/* IP Conflict Detection APIs*/
extern Std_ReturnType rba_EthDHCP_IpConflictDetected( uint8 lCtrlIdx_u8 );

extern Std_ReturnType rba_EthDHCP_GetIpAddress( uint8 lCtrlIdx_u8,
                                                uint32 * lLocalIpAddress_pu32,
                                                TcpIp_IpAddrStateType * lCurrentIpState_pen);
/* Handling Options APIs */
extern Std_ReturnType rba_EthDHCP_ReadOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                              uint8 Option,
                                              uint8 * DataLength,
                                              uint8 * DataPtr);

extern Std_ReturnType rba_EthDHCP_WriteOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                               uint8 Option,
                                               uint8 DataLength,
                                               const uint8 * DataPtr);

#define RBA_ETHDHCP_STOP_SEC_CODE
#include "rba_EthDHCP_MemMap.h"

#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )*/

#endif /* RBA_ETHDHCP_H */

