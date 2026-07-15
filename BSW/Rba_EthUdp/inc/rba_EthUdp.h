

#ifndef RBA_ETHUDP_H
#define RBA_ETHUDP_H

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )

/* Length of UDP header */
#define RBA_ETHUDP_HDRLENGTH                    8U

/* Vendor Id */
#define RBA_ETHUDP_VENDOR_ID                    6U

/* rba_EthUdp module Id */
#define RBA_ETHUDP_MODULE_ID                    255U

/* rba_EthUdp instance Id */
#define RBA_ETHUDP_INSTANCE_ID                  173U

/* SOFTWARE VERSION INFORMATION */
#define RBA_ETHUDP_SW_MAJOR_VERSION             10U
#define RBA_ETHUDP_SW_MINOR_VERSION             0U
#define RBA_ETHUDP_SW_PATCH_VERSION             0U
/*End: Common Published information*/


#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"
extern void rba_EthUdp_Init ( const rba_EthUdp_Config_to * CurrConfig_pcst );

extern void rba_EthUdp_MainFunction( void );

extern BufReq_ReturnType rba_EthUdp_ProvideTxBuffer( TcpIp_SocketIdType SocketId_u16,
                                                     uint32 DestIPv4Addr_u32,
                                                     uint8 * BufIdx_pu8,
                                                     uint8 * * Buffer_ppu8,
                                                     const uint16 * GrantLength_pcu16 );

extern TcpIp_ReturnType rba_EthUdp_Transmit( TcpIp_SocketIdType SocketId_u16,
                                             const TcpIp_SockAddrType * RemoteAddr_pst,
                                             const uint8 * DataPtr_pcu8,
                                             uint8 * Buffer_pu8,
                                             uint8 BufIdx_u8,
                                             uint16 BufLength_u16,
                                             const void * MetaDataPtr_pcv );


extern Std_ReturnType rba_EthUdp_GetSocket( TcpIp_DomainType Domain_u32,
                                            TcpIp_SocketIdType * SocketIdPtr_pu16,
                                            uint8 SocketOwner_u8 );


extern Std_ReturnType rba_EthUdp_Bind( TcpIp_SocketIdType SocketId_u16,
                                       TcpIp_LocalAddrIdType LocalAddrId_u8,
                                       uint16 * Port_pu16,
                                       uint8 FramePrio_u8);

extern void rba_EthUdp_Close(    TcpIp_SocketIdType SocketId_u16 );


extern Std_ReturnType rba_EthUdp_ChangeParameter( TcpIp_SocketIdType SocketId_u16,
                                                  TcpIp_ParamIdType ParameterId_en,
                                                  const uint8 * ParameterValue_pcu8 );


extern Std_ReturnType rba_EthUdp_GetSocketInfo( TcpIp_SocketIdType SocketId_u16,
                                                TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                                uint16 * LocalPort_pu16 );

extern void rba_EthUdp_TerminateSocUsingLocalAddr(TcpIp_LocalAddrIdType LocalAddrId_u8);

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
/* Allows to read and reset UDP measurement data for diagnostic purposes. */
extern Std_ReturnType rba_EthUdp_GetAndResetMeasurementData( TcpIp_MeasurementIdxType MeasurementIdx_u8,
                                                             boolean MeasurementResetNeeded_b,
                                                             uint32 * MeasurementDataPtr_pu32);
#endif
#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#define RBA_ETHUDP_START_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
extern Std_ReturnType rba_EthUdp_CheckFragmentation( TcpIp_SocketIdType SocketId_u16,
                                                     uint16 DataLength_u16,
                                                     uint8 * NumberOfIpFragments_pu8 );
#endif

extern TcpIp_ReturnType rba_EthUdp_ZeroCopyFillHeader ( TcpIp_SocketIdType SocketId_u16,
                                                        const TcpIp_SockAddrType * RemoteAddr_pcst,
                                                        const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                        uint8 * UL_SockOwnerIdx_pu8 );

extern TcpIp_ReturnType rba_EthUdp_ZeroCopyTransmit ( TcpIp_SocketIdType SocketId_u16,
                                                      const TcpIp_SockAddrType * RemoteAddr_pcst,
                                                      const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                      uint32 * TxId_pu32 );
#endif

#define RBA_ETHUDP_STOP_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"

#endif /* ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHUDP_H */
