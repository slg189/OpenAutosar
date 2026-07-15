

#ifndef RBA_ETHAUTOIP_H
#define RBA_ETHAUTOIP_H

/*
****************************************************************************************************
*                                   Includes
****************************************************************************************************
*/


#if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) )

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

/* Vendor Id */
#define RBA_ETHAUTOIP_VENDOR_ID                     6U

/* Module ID for reporting DET error - TcpIp module ID, as AutoIp is a SubModule of TCPIP */
#define RBA_ETHAUTOIP_MODULE_ID             255U

/* Instance Id for Reporting the DET error */
#define RBA_ETHAUTOIP_INSTANCE_ID           (0xB0)


/* SOFTWARE VERSION INFORMATION */
#define RBA_ETHAUTOIP_SW_MAJOR_VERSION              10U
#define RBA_ETHAUTOIP_SW_MINOR_VERSION              0U
#define RBA_ETHAUTOIP_SW_PATCH_VERSION              0U
/*End: Common Published information*/


/*
****************************************************************************************************
*                                   Prototypes
****************************************************************************************************
*/
#define RBA_ETHAUTOIP_START_SEC_CODE
#include "rba_EthAutoIp_MemMap.h"

extern void rba_EthAutoIp_Init( const rba_EthAutoIp_Config_to * rba_EthAutoIp_Config_cst );
extern Std_ReturnType rba_EthAutoIp_Start( TcpIp_LocalAddrIdType lLocalAddrId_tu8 );
extern Std_ReturnType rba_EthAutoIp_Stop( TcpIp_LocalAddrIdType lLocalAddrId_tu8 );
extern void rba_EthAutoIp_MainFunction( void );
extern Std_ReturnType rba_EthAutoIp_IpConflictDetected( uint8 lCtrlIdx_u8 );
extern Std_ReturnType rba_EthAutoIp_GetIpAddress( uint8 lCtrlIdx_u8,
                                                  uint32 * lLocalIpAddress_pu32,
                                                  TcpIp_IpAddrStateType * lCurrentIpState_pen);
extern Std_ReturnType rba_EthAutoIp_AllocBaseIpAddr( TcpIp_LocalAddrIdType lLocalAddrId_tu8,
                                                     const TcpIp_SockAddrInetType * LocalIpAddr_pst );
#define RBA_ETHAUTOIP_STOP_SEC_CODE
#include "rba_EthAutoIp_MemMap.h"

#endif  /* #if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHAUTOIP_H */

