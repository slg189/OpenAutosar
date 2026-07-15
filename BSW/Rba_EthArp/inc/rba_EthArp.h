

#ifndef RBA_ETHARP_H
#define RBA_ETHARP_H

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

#if ( defined(TCPIP_ARP_ENABLED) && ( TCPIP_ARP_ENABLED == STD_ON ) )

/* Vendor Id */
#define RBA_ETHARP_VENDOR_ID                    6U

/* rba_EthArp module Id */
#define RBA_ETHARP_MODULE_ID                    255U

/* rba_EthArp instance Id */
#define RBA_ETHARP_INSTANCE_ID                  171U


/* SOFTWARE VERSION INFORMATION */
#define RBA_ETHARP_SW_MAJOR_VERSION                 10U
#define RBA_ETHARP_SW_MINOR_VERSION                 0U
#define RBA_ETHARP_SW_PATCH_VERSION                 0U
/*End: Common Published information*/

/*********************************************/
/* ARP code - Define Value                   */
/*********************************************/

/* Type of ARP Frame to send */
#define RBA_ETHARP_ARP_PROBE_FRAME              2U

/*
****************************************************************************************************
*                                   Prototypes
****************************************************************************************************
*/
#define RBA_ETHARP_START_SEC_CODE
#include "rba_EthArp_MemMap.h"
extern void rba_EthArp_Init( const rba_EthArp_Config_to * rba_EthArp_Config_cst );

extern void rba_EthArp_MainFunction( void );

extern TcpIp_ReturnType rba_EthArp_GetDestEthAddr( uint8 lCtrlIdx_u8,
                                                   uint32 lDestIpAddr_u32,
                                                   uint8 * lDestEthAddr_au8 );

extern TcpIp_ReturnType rba_EthArp_UpdateArpTable( uint8 lCtrlIdx_u8,
                                                   const uint8 * lSrcEthAddr_au8,
                                                   uint32 lSrcIpAddr_u32 );

extern Std_ReturnType rba_EthArp_SendArpFrame( uint8 lCtrlIdx_u8,
                                               uint16 lIndexTable_u16,
                                               uint32 lDestIPv4Addr_u32,
                                               const uint8 * lDestEthAddr_au8,
                                               uint8 lTypeFrame_u8);

extern Std_ReturnType rba_EthArp_SendArpGratuitousFrame( uint8 lCtrlIdx_u8,
                                                         uint32 lSrcIPv4Addr_u32);

extern Std_ReturnType rba_EthArp_ReadArpTable( uint8 EthIfCtrlIdx_u8,
                                               uint8 * const ArpTable_cpu8,
                                               uint16 * const NumberEntries_cpu16 );

#if( (TCPIP_IP_ADDR_CONFLICT_DETECTION_IP_FRAMES == STD_ON) || (RBA_ETHARP_ADDR_CONFLICT_DETECTION_ARP_FRAMES == STD_ON) )
extern Std_ReturnType rba_EthArp_IpConflictDetection( uint8 lCtrlIdx_u8,
                                                      uint32 lRxSrcIpAddress_u32,
                                                      uint32 lRxDstIpAddress_u32,
                                                      const uint8 * lRxSrcEthAddress_au8,
                                                      uint16 lTypeOfFrame_u16);
#endif

extern void rba_EthArp_CleanArpTable ( uint8 EthIfCtrlIdxCurr_u8 );

extern Std_ReturnType rba_EthArp_GetArpTablesize( uint16 * lArpTableSize_pu16 );

#define RBA_ETHARP_STOP_SEC_CODE
#include "rba_EthArp_MemMap.h"
#endif /* #if ( defined(TCPIP_ARP_ENABLED) && ( TCPIP_ARP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHARP_H */

/*<BASDKey>
*********************************************************************************************************
* $History___:$
*********************************************************************************************************
</BASDKey>*/

