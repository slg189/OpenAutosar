

#ifndef RBA_ETHTCP_H
#define RBA_ETHTCP_H

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )


/*
 ***************************************************************************************************
 * Common Published Information
 ***************************************************************************************************
 */

/* rba_EthTcp vendor Id */
#define RBA_ETHTCP_VENDOR_ID                    6U

/* rba_EthTcp module Id */
#define RBA_ETHTCP_MODULE_ID                    255U

/* rba_EthTcp instance Id */
#define RBA_ETHTCP_INSTANCE_ID                  174U

/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

extern void rba_EthTcp_Init                       ( const rba_EthTcp_Config_to * TcpConfig_cpo );

extern void rba_EthTcp_MainFunction               ( void );

extern Std_ReturnType rba_EthTcp_GetSocket        ( TcpIp_DomainType     Domain_u32,
                                                    TcpIp_SocketIdType * SocketIdPtr_puo,
                                                    uint8                SocketOwner_u8 );

extern Std_ReturnType rba_EthTcp_FindTcpSockId    ( const TcpIp_SockAddrType * LocalAddr_cpst,
                                                    const TcpIp_SockAddrType * RemoteAddr_cpst,
                                                    TcpIp_SocketIdType *       SocketIdPtr_puo );

extern Std_ReturnType rba_EthTcp_ChangeParameter  ( TcpIp_SocketIdType TcpSockId_uo,
                                                    TcpIp_ParamIdType  ParamId_u8,
                                                    const uint8 *      ParamValue_cpu8 );

extern Std_ReturnType rba_EthTcp_Connect          ( TcpIp_SocketIdType         TcpSockId_uo,
                                                    const TcpIp_SockAddrType * RemoteAddr_cpst );

extern Std_ReturnType rba_EthTcp_Listen           ( TcpIp_SocketIdType TcpSockId_uo,
                                                    uint16             MaxChannels_u16 );

extern Std_ReturnType rba_EthTcp_Bind             ( TcpIp_SocketIdType    TcpSockId_uo,
                                                    TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                    uint16 *              PortPtr_pu16,
                                                    uint8                 FramePrio_u8 );

extern Std_ReturnType rba_EthTcp_Transmit         ( TcpIp_SocketIdType TcpSockId_uo,
                                                    const uint8 *      TxData_cpu8,
                                                    uint32             AvlLen_u32,
                                                    boolean            ForceRetrieve_b );

extern Std_ReturnType rba_EthTcp_Received         ( TcpIp_SocketIdType TcpSockId_uo,
                                                    uint16             DataLen_u16 );

extern Std_ReturnType rba_EthTcp_Close            ( TcpIp_SocketIdType TcpSockId_uo,
                                                    boolean            Abort_b );

extern void rba_EthTcp_AbortByLocalAddrId         ( TcpIp_LocalAddrIdType LocalAddrId_u8 );

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
extern Std_ReturnType rba_EthTcp_GetAndResetMeasurementData(TcpIp_MeasurementIdxType MeasurementIdx, \
                                                            boolean                  MeasurementResetNeeded, \
                                                            uint32 *                 MeasurementDataPtr);
#endif /* (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON) */

#if( TCPIP_TLS_ENABLED == STD_ON )

extern Std_ReturnType rba_EthTcp_SecureTransmit( TcpIp_SocketIdType TcpSockId_uo,
                                                 const uint8 *      TlsEncryptedData_cpu8,
                                                 uint16             TlsEncryptedDataLength_u16);

extern Std_ReturnType  rba_EthTcp_GetAvailTxBuffSize( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                      uint16 *           TcpFreeTxBuffSize_pu16);

extern Std_ReturnType  rba_EthTcp_GetMSS ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                           uint16 *           TcpMssValue_pu16);

extern Std_ReturnType rba_EthTcp_SecureClose( TcpIp_SocketIdType TcpSockId_uo, \
                                               boolean            Abort_b );
#endif

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


#endif  /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHTCP_H */
