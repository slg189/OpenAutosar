

#ifndef RBA_ETHIPV4_H
#define RBA_ETHIPV4_H

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

/* Vendor Id */
#define RBA_ETHIPV4_VENDOR_ID                   6U

/* rba_EthIPv4 module Id */
#define RBA_ETHIPV4_MODULE_ID                   255U

/* rba_EthIPv4 instance Id */
#define RBA_ETHIPV4_INSTANCE_ID                 170U

#define RBA_ETHIPV4_SW_MAJOR_VERSION                10U
#define RBA_ETHIPV4_SW_MINOR_VERSION                0U
#define RBA_ETHIPV4_SW_PATCH_VERSION                0U

#define RBA_ETHIPV4_HDRLENGTH                       20U

/* Multicast IP address area: Start 224.0.0.0 */
#define     RBA_ETHIPV4_MULTICASTADDR_MIN               0xE0000000UL

/* Multicast IP address area: End   239.255.255.255 */
#define     RBA_ETHIPV4_MULTICASTADDR_MAX               0xEFFFFFFFUL

/* Position of upper layer transport protocol value in IPv4 header */
#define     RBA_ETHIPV4_TP_PROTOCOL                     9U
/* Position of source IPv4 address in IPv4 header */
#define     RBA_ETHIPV4_SRCIPV4ADDR_OFFSET              12U
/* Position of destination IPv4 address in IPv4 header */
#define     RBA_ETHIPV4_DESTIPV4ADDR_OFFSET             16U

/*
****************************************************************************************************
*                                   Prototypes
****************************************************************************************************
*/
#define RBA_ETHIPV4_START_SEC_CODE
#include "rba_EthIPv4_MemMap.h"


/* Function declaration*/
extern void rba_EthIPv4_Init (
                                  const rba_EthIPv4_Config_to * CurrConfig_pco );

extern void rba_EthIPv4_MainFunction( void );


extern TcpIp_ReturnType rba_EthIPv4_Transmit(
                                            uint8 LocalAddrId_u8,
                                            uint32 DestIpAddr_u32,
                                            uint8 * Buffer_pu8,
                                            uint8 BufIdx_u8,
                                            const TcpIp_IpHeader_tst * IpHeader_pcst,
                                            uint16 TotLength_u16 );



extern BufReq_ReturnType rba_EthIPv4_ProvideTxBuffer(
                                            uint8 LocalAddrId_u8,
                                            uint32 DestIpAddr_u32,
                                            uint8 FramePrio_u8,
                                            uint8 * BufIdx_pu8,
                                            uint8 * * Buffer_ppu8,
                                            uint16 * ReqLengthIn_pu16 );

extern TcpIp_AddressType rba_EthIPv4_ChkIPv4AddrType(
                                            uint32 DestIpAddr_u32,
                                            uint32 SrcIpAddr_u32,
                                            uint32 NetMask_u32 );

extern void rba_EthIPv4_GetIPMulticastDestEthAddr(
                                            uint32 DestMultiCastIpAddr_u32,
                                            uint8 * DestEthAddr_pau8 );

extern Std_ReturnType rba_EthIPv4_GetPseudoHeader( uint8 LocalAddrId_u8,
                                                   uint8 ULSocketOwner_u8,
                                                   uint16 BufLength_u16,
                                                   TcpIp_PseudoHdr_tst * lPseudoHdr_pst);

#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

extern Std_ReturnType rba_EthIPv4_GetAndResetMeasurementData(
                                            TcpIp_MeasurementIdxType MeasurementIdx_u8,
                                            boolean MeasurementResetNeeded_b,
                                            uint32 * MeasurementDataPtr_pu32 );

#endif

#define RBA_ETHIPV4_STOP_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

#define RBA_ETHIPV4_START_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

extern Std_ReturnType rba_EthIPv4_CheckFragmentation
                                              ( uint8 LocalAddrId_u8,
                                                uint16 DataLength_u16,
                                                uint8 * NumOfIPFrags_pu8  );


#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )

extern TcpIp_ReturnType rba_EthIPv4_ZeroCopyTransmit
                                            ( uint8 LocalAddrId_u8,
                                              uint16 UdpDataLength_u16,
                                              const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                              uint32 * TxId_pu32 );

extern TcpIp_ReturnType rba_EthIPv4_ZeroCopyFillHeader
                                            ( uint8 LocalAddrId_u8,
                                              uint8 Priority_u8,
                                              uint32 DestIpAddr_u32,
                                              const TcpIp_IpHeader_tst * IpHeader_pcst,
                                              uint16 UdpDataLength_u16,
                                              const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst );
#endif

#define RBA_ETHIPV4_STOP_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHIPV4_H */

