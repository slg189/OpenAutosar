

#ifndef RBA_ETHTCP_PRV_H
#define RBA_ETHTCP_PRV_H

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "rba_EthTcp_Prv_Domain.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/* Value for unused sockets */
#define     RBA_ETHTCP_TCPSOCK_UNUSED               0xFFFFU

/* Masks for the Tcp header */
#define     RBA_ETHTCP_FIN_MSK                      0x01U
#define     RBA_ETHTCP_SYN_MSK                      0x02U
#define     RBA_ETHTCP_RST_MSK                      0x04U
#define     RBA_ETHTCP_PSH_MSK                      0x08U
#define     RBA_ETHTCP_ACK_MSK                      0x10U
#define     RBA_ETHTCP_URG_MSK                      0x20U
#define     RBA_ETHTCP_SYN_ACK_MSK                  0x12U
#define     RBA_ETHTCP_PSH_ACK_MSK                  0x18U
#define     RBA_ETHTCP_FIN_ACK_MSK                  0x11U
#define     RBA_ETHTCP_RST_ACK_MSK                  0x14U

/* Default Tcp header length (in bytes) */
#define     RBA_ETHTCP_DFL_HDRLENGTH                20U

/* Kind of the Tcp options */
#define     RBA_ETHTCP_OPT_KIND_EOP                 0U
#define     RBA_ETHTCP_OPT_KIND_NOP                 1U
#define     RBA_ETHTCP_OPT_KIND_MSS                 2U
#define     RBA_ETHTCP_OPT_KIND_WSF                 3U
#define     RBA_ETHTCP_OPT_KIND_SACK_PERM           4U
#define     RBA_ETHTCP_OPT_KIND_TIMESTAMP           8U
#define     RBA_ETHTCP_OPT_KIND_UTO                 28U

/* Length of the Tcp options (in bytes) */
#define     RBA_ETHTCP_OPT_LENGTH_EOP               1U
#define     RBA_ETHTCP_OPT_LENGTH_NOP               1U
#define     RBA_ETHTCP_OPT_LENGTH_MSS               4U
#define     RBA_ETHTCP_OPT_LENGTH_UTO               4U

/* Default Value of socket owner index */
#define     RBA_ETHTCP_SOCKOWNER_IDX                0xFFU
#define     RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID     (TcpIp_SocketIdType)0xFFFFU

/* rba_EthTls connection index default value */
#define RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE        (TcpIp_TlsConnectionIdType)0xFFFFU

/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_VAR_INIT_8
#include "rba_EthTcp_MemMap.h"
/* Flag to indicate if Tcp_Init has been executed or not since power ON */
extern boolean                          rba_EthTcp_InitFlag_b;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_8
#include "rba_EthTcp_MemMap.h"

#define RBA_ETHTCP_START_SEC_VAR_INIT_16
#include "rba_EthTcp_MemMap.h"
/* TCP port range*/
extern uint16                            rba_EthTcp_PortAny_u16;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_16
#include "rba_EthTcp_MemMap.h"

#define RBA_ETHTCP_START_SEC_VAR_CLEARED_16
#include "rba_EthTcp_MemMap.h"
/* Total number of opened Tcp sockets */
extern uint16                            rba_EthTcp_SockConnCntr_u16;
#define RBA_ETHTCP_STOP_SEC_VAR_CLEARED_16
#include "rba_EthTcp_MemMap.h"

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
#define RBA_ETHTCP_START_SEC_VAR_INIT_32
#include "rba_EthTcp_MemMap.h"
/* Total number of TCP datagrams which cannot be mapped to a valid local IP/Port */
extern uint32                            rba_EthTcp_MeasurementData_u32;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_32
#include "rba_EthTcp_MemMap.h"
#endif /* #if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON) */

#define RBA_ETHTCP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
/* Pointer to the currently used Tcp configuration */
extern const rba_EthTcp_Config_to *       rba_EthTcp_CurrConfig_cpo;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"

#if ( RBA_ETHTCP_ISN_CLOCK_DRIVEN_ENABLED == STD_ON || RBA_ETHTCP_ISN_CLOCK_HASH_BASED_ENABLED == STD_ON )
#define RBA_ETHTCP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
/* Context used for the generation of the Initial Sequence Number */
extern rba_EthTcp_IsnGeneratorContext_tst rba_EthTcp_IsnContext_st;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
#endif


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/* CheckFrameAndCopyToSocket */

extern Std_ReturnType rba_EthTcp_CheckFrameAndCopyToSocket   ( TcpIp_LocalAddrIdType       LocalAddrId_u8,
                                                               const TcpIp_PseudoHdr_tst * PseudoHdr_cpst,
                                                               uint8 *                     RxData_pu8,
                                                               uint16                      LenByte_u16,
                                                               boolean                     IsFromReorderBuf_b,
                                                               TcpIp_SocketIdType *        TcpDynSockTblIdx_puo );

/* ConnectionState */

extern void rba_EthTcp_SetConnectionState                     ( TcpIp_SocketIdType        TcpDynSockTblIdx_uo,
                                                                 rba_EthTcp_ConnState_ten NewConnState_en );

/* CopyHdrToFrame */

extern void rba_EthTcp_SetHdrToFrame                          ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint8 *            TxEthDrvBuf_pu8 );

extern uint16 rba_EthTcp_GetOptionLength                      ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern void rba_EthTcp_SetOptionToFrame                       ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint8 *            TxEthDrvBuf_pu8 );

#if ( RBA_ETHTCP_SW_CHKSUM_TX == STD_ON )
extern Std_ReturnType rba_EthTcp_SetChecksumToFrame           ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint8 *            TxEthDrvBuf_pu8,
                                                                uint16             TotalLength_u16 );
#endif

/* CopyPayloadToFrame */

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
extern void rba_EthTcp_SetPayloadToFrame                      ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint8 *            TxEthDrvBuf_pu8,
                                                                uint16             PayloadLength_u16 );
#endif

/* ExtractHdrFromFrame */

extern void rba_EthTcp_GetHdrFromFrame                        ( const uint8 *                 RxData_cpu8,
                                                                rba_EthTcp_HeaderFields_tst * RcvdHdrFields_pst );

extern Std_ReturnType rba_EthTcp_GetOptionFromFrame           ( const uint8 *                 RxData_cpu8,
                                                                rba_EthTcp_OptionFields_tst * RcvdOptionFields_pst );

extern rba_EthTcp_SeqVal_ten rba_EthTcp_RcvdSeqNumValidity    ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             RcvdSeqNum_u32,
                                                                uint16             PayloadLenByte_u16 );

extern rba_EthTcp_AckVal_ten rba_EthTcp_RcvdAckNumValidity    ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             RcvdAckNum_u32 );

#if (( RBA_ETHTCP_SW_CHKSUM_RX == STD_ON ) || ( RBA_ETHTCP_REASSMBLY_RX_ENABLED == STD_ON ))
extern Std_ReturnType rba_EthTcp_RcvdChecksumValidity         ( const TcpIp_PseudoHdr_tst * PseudoHdr_cpst,
                                                                uint8 *                     RxData_pu8,
                                                                uint16                      LenByte_u16);
#endif

/* ExtractPayloadFromFrame */

extern uint16 rba_EthTcp_GetPayloadLenFromFrame               ( const uint8 * RxData_cpu8,
                                                                uint16        LenByte_u16 );

extern uint16 rba_EthTcp_GetRepeatedPayloadLen                ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             RcvdSeqNum_u32 );

extern uint16 rba_EthTcp_GetOutOfWindowPayloadLen             ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             RcvdSeqNum_u32,
                                                                uint16             PayloadLenByte_u16 );

/* IsnGenerator */

#if ( RBA_ETHTCP_ISN_CLOCK_DRIVEN_ENABLED == STD_ON || RBA_ETHTCP_ISN_CLOCK_HASH_BASED_ENABLED == STD_ON )
extern void rba_EthTcp_InitIsnGenerator                       ( void );

extern void rba_EthTcp_IncrIsnGenerator                       ( void );
#endif

extern uint32 rba_EthTcp_GenerateIsn                          ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

/* RxData */

void rba_EthTcp_RxData                                        ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint8 *            DataPtr_pu8,
                                                                uint16             DataLen_u16 );

/* RxDataAcknowledgment */

extern void rba_EthTcp_RxDataAcknowledgment                   ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint16             ConfirmedLen_u16 );

/* SndData */

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
extern void rba_EthTcp_SndData                                ( TcpIp_SocketIdType TcpDynSockTblIdx_uo);
#endif

/* SndFlag */

extern void rba_EthTcp_SndFlag                                ( TcpIp_SocketIdType TcpDynSockTblIdx_uo);

/* SocketTable */

extern Std_ReturnType rba_EthTcp_GetNewSock                   ( const rba_EthTcp_NewSockInfo_tst * NewSockInfo_cpst,
                                                                TcpIp_SocketIdType *               TcpDynSockTblIdx_puo );

extern Std_ReturnType rba_EthTcp_ReopenSock                   ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern Std_ReturnType rba_EthTcp_LinkToListeningSock          ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern void rba_EthTcp_ResetSock                              ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                TcpIp_EventType    CloseType_en );

extern void rba_EthTcp_ResetTcpTableEntry                     ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );


extern void rba_EthTcp_SetSeqNum                              ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             SeqNum_u32 );

extern void rba_EthTcp_SetAckNum                              ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             AckNum_u32 );

extern Std_ReturnType rba_EthTcp_SetNextSendFrame             ( TcpIp_SocketIdType  TcpDynSockTblIdx_uo,
                                                                rba_EthTcp_Flag_ten TcpFlag_en );

extern void rba_EthTcp_SetRcvdWnd                             ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint32             SeqNum_u32,
                                                                uint32             AckNum_u32,
                                                                uint16             WinSize_u16 );

/* Tmr */

extern void rba_EthTcp_StartTimer                             ( TcpIp_SocketIdType   TcpDynSockTblIdx_uo,
                                                                rba_EthTcp_TmrSt_ten TmrSt_en );

extern void rba_EthTcp_RestartTimer                           ( TcpIp_SocketIdType   TcpDynSockTblIdx_uo,
                                                                rba_EthTcp_TmrSt_ten TmrSt_en );

extern void rba_EthTcp_StopTimer                              ( TcpIp_SocketIdType   TcpDynSockTblIdx_uo,
                                                                rba_EthTcp_TmrSt_ten TmrSt_en );

extern void rba_EthTcp_ExpireTimer                            ( TcpIp_SocketIdType   TcpDynSockTblIdx_uo,
                                                                rba_EthTcp_TmrSt_ten TmrSt_en );

extern void rba_EthTcp_IncrTimer                              ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

#if (RBA_ETHTCP_DYNAMIC_RETX_TIMER == STD_ON)
extern void rba_EthTcp_CalculateRto                           ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#endif

/* TxBuffer */

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
extern void rba_EthTcp_SetTxBufBeforeTx                       ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint16             Len_u16 );

extern void rba_EthTcp_SetTxBufBeforeReTx                     ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern void rba_EthTcp_SetTxBufAfterTx                        ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint16             Len_u16 );

extern void rba_EthTcp_SetTxBufAfterAcked                     ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                uint16             Len_u16 );

extern uint16 rba_EthTcp_GetTxBufFreeLen                      ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern uint16 rba_EthTcp_GetTxBufLnrFreeLen                   ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern uint16 rba_EthTcp_GetTxBufSndLen                       ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern uint16 rba_EthTcp_GetTxBufLnrSndLen                    ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

extern uint16 rba_EthTcp_GetTxBufUnackedLen                   ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#endif

/* Reorder handling APIs */
#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
extern void rba_EthTcp_ReorderBufferInit                      ( void );

extern void rba_EthTcp_ReleaseReorderBuffer                   ( rba_EthTcp_DynSockTblType_tst * DynSockTbl_pst );

extern Std_ReturnType rba_EthTcp_HoldOutOfOrderFrame          ( const uint8 *           RxData_pcu8,
                                                                TcpIp_SocketIdType      TcpDynSockTblIdx_uo,
                                                                uint16                  LenByte_u16 );

extern rba_EthTcp_FetchOutOfOrderFrame_ten  rba_EthTcp_FetchOutOfOrderFrame ( TcpIp_SocketIdType    TcpDynSockTblIdx_uo,
                                                                              uint8                 **RxData_ppu8,
                                                                              uint16                *LenByte_pu16 );
#endif

#if( TCPIP_TLS_ENABLED == STD_ON )
extern Std_ReturnType rba_EthTcp_CheckSecureServerConnection (const rba_EthTcp_DynSockTblType_tst * TcpDynSockTbl_pst,
                                                              const TcpIp_IPAddrParamType_tun     * IpAdrr_pcun,
                                                              TcpIp_TlsConnectionIdType             TlsConnId_uo);

extern Std_ReturnType rba_EthTcp_CheckSecureClientConnection (const rba_EthTcp_DynSockTblType_tst * TcpDynSockTbl_pst,
                                                              const rba_EthTcp_IPAddr_tun         * IpAdrr_pcun,
                                                              TcpIp_TlsConnectionIdType             TlsConnId_uo);

extern Std_ReturnType rba_EthTcp_OpenSecureConnection( TcpIp_SocketIdType   TcpDynSockTblIdx_uo);

extern Std_ReturnType rba_EthTcp_OpenDynamicSecureConnection ( TcpIp_SocketIdType  TcpDynSockTblIdx_uo);

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
extern void  rba_EthTcp_ReserveFreeQSlotForTlsTx ( TcpIp_SocketIdType TcpDynSockTblIdx_uo);
#endif/* RBA_ETHTCP_TXWNDSIZE > 0 */

#endif/* TCPIP_TLS_ENABLED == STD_ON */
#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


/*
 ***************************************************************************************************
 * Macros
 ***************************************************************************************************
 */

/* Macro for reading at given address */

/* Read 32 bits at position */
#define RBA_ETHTCP_READ_32BITS_AT_POS( BaseAddress, BytePos )                                   \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) ) ) << 24U )      |                   \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) + 1 ) ) << 16U )  |                   \
        (uint32)( (uint32)( * ( (BaseAddress) + (BytePos) + 2 ) ) << 8U )   |                   \
        (uint32)( * ( (BaseAddress) + (BytePos) + 3U ) )                                        \

/* Read 16 bits at position */
#define RBA_ETHTCP_READ_16BITS_AT_POS( BaseAddress, BytePos )                                   \
        (uint16)( (uint16)( * ( (BaseAddress) + (BytePos) ) ) << 8U )       |                   \
        (uint16)( * ( (BaseAddress) + (BytePos) + 1U ) )                                        \

/* Read 8 bits at position */
#define RBA_ETHTCP_READ_8BITS_AT_POS( BaseAddress, BytePos )                                    \
        (uint8)( * ( (BaseAddress) + (BytePos) ) )                                              \

/* Read 1 bit at position */
#define RBA_ETHTCP_READ_BIT_AT_POS( BaseAddress, BytePos, BitPos )                              \
        ( (uint8)( * ( (BaseAddress) + (BytePos) ) ) & ( 0x01U << (BitPos) ) ) >> (BitPos)      \


/* Macro for writing at given address */

/* Write 32 bits at position */
#define RBA_ETHTCP_WRITE_32BITS_AT_POS( BaseAddress, BytePos, Value )                           \
        ( * ( (BaseAddress) + (BytePos) ) ) = ( (uint8)( (Value) >> 24U ) );                    \
        ( * ( (BaseAddress) + (BytePos) + 1U ) ) = ( (uint8)( (Value) >> 16U ) );               \
        ( * ( (BaseAddress) + (BytePos) + 2U ) ) = ( (uint8)( (Value) >> 8U ) );                \
        ( * ( (BaseAddress) + (BytePos) + 3U ) ) = ( (uint8)( (Value) ) );                      \

/* Write 16 bits at position */
#define RBA_ETHTCP_WRITE_16BITS_AT_POS( BaseAddress, BytePos, Value )                           \
        ( * ( (BaseAddress) + (BytePos) ) ) = ( (uint8)( (Value) >> (uint8)8U ) );                      \
        ( * ( (BaseAddress) + (BytePos) + 1U ) ) = ( (uint8)( (Value) ) );                      \

/* Write 8 bits at position */
#define RBA_ETHTCP_WRITE_8BITS_AT_POS( BaseAddress, BytePos, Value )                            \
        ( * ( (BaseAddress) + (BytePos) ) ) = ( (uint8)( (Value) ) );                           \

/* Write 1 bit at position */
#define RBA_ETHTCP_WRITE_BIT_AT_POS( BaseAddress, BytePos, BitPos, Value )                      \
        ( * ( (BaseAddress) + (BytePos) ) ) |= ( (uint8)( ( (Value) & 0x01U ) << (BitPos) ) );  \
        ( * ( (BaseAddress) + (BytePos) ) ) &= ( (uint8)( ( (Value) | 0xFEU ) << (BitPos) ) );  \


/* Macros for ISN generation (MD4 hash) */

/* F, G and H functions defined as per RFC1320 p4 */
#define RBA_ETHTCP_MD4_F(X, Y, Z)       (((X) & (Y)) | ((~(X)) & (Z)))
#define RBA_ETHTCP_MD4_G(X, Y, Z)       (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))
#define RBA_ETHTCP_MD4_H(X, Y, Z)       (((X) ^ (Y)) ^ (Z))

/* Rotate function */
#define RBA_ETHTCP_MD4_ROTATE(A, S)     ((uint32)( (A) << (S) ) | (uint32)( (A) >> (32U - (S)) ))

/* MD4 transformation functions for the 3 rounds */
#define RBA_ETHTCP_MD4_STEP_F(A, B, C, D, X, S)                                                 \
        (A) = (A) + RBA_ETHTCP_MD4_F( (B), (C), (D) ) + (X);                                    \
        (A) = RBA_ETHTCP_MD4_ROTATE( (A), (S) );                                                \

#define RBA_ETHTCP_MD4_STEP_G(A, B, C, D, X, S)                                                 \
        (A) = (A) + RBA_ETHTCP_MD4_G( (B), (C), (D) ) + (X);                                    \
        (A) = RBA_ETHTCP_MD4_ROTATE( (A), (S) );                                                \

#define RBA_ETHTCP_MD4_STEP_H(A, B, C, D, X, S)                                                 \
        (A) = (A) + RBA_ETHTCP_MD4_H( (B), (C), (D) ) + (X);                                    \
        (A) = RBA_ETHTCP_MD4_ROTATE( (A), (S) );                                                \


/*
 ***************************************************************************************************
 * DET Defines
 ***************************************************************************************************
 */

/* API IDs for DET Support */

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Init() */
#define RBA_ETHTCP_E_INIT                           (0x01U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_MainFunction() */
#define RBA_ETHTCP_E_MAINFUNCTION                   (0x02U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_SetSocketId() */
#define RBA_ETHTCP_E_GETSOCKET                      (0x03U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_ChangeParameter() */
#define RBA_ETHTCP_E_CHANGEPARAMETER                (0x04U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Connect() */
#define RBA_ETHTCP_E_CONNECT                        (0x05U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Listen() */
#define RBA_ETHTCP_E_LISTEN                         (0x06U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Transmit() */
#define RBA_ETHTCP_E_TRANSMIT                       (0x07U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_RxIndication() */
#define RBA_ETHTCP_E_RXINDICATION                   (0x08U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Received() */
#define RBA_ETHTCP_E_RECEIVED                       (0x09U)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Close() */
#define RBA_ETHTCP_E_CLOSE                          (0x0AU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_Bind()  */
#define RBA_ETHTCP_E_BIND                           (0x0BU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_FindTcpSockId()  */
#define RBA_ETHTCP_FINDTCPSOCKID                    (0x0CU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_AbortByLocalAddrId()  */
#define RBA_ETHTCP_ABORTBYLOCALADDRID               (0x0DU)

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_GetAndResetMeasurementData()  */
#define RBA_ETHTCP_E_GETANDRESETMEASUREMENTDATA     (0x0EU)
#endif

#if( TCPIP_TLS_ENABLED == STD_ON )
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_OpenDynamicSecureConnection()  */
#define RBA_ETHTCP_E_OPENDYNAMICSECURECONNECTION   (0xF2U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_OpenSecureConnection()  */
#define RBA_ETHTCP_E_OPENSECURECONNECTION          (0xF3U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_ProvideDataTlsToUl()  */
#define RBA_ETHTCP_E_TLSRXINDICATION               (0xF4U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTls_SecureTransmit()  */
#define RBA_ETHTCP_E_SECURETRANSMIT                (0xF5U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_GetMSS()  */
#define RBA_ETHTCP_E_GETMSS  				       (0xF6U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_GetAvailTxBuffSize()  */
#define RBA_ETHTCP_E_GETAVAILTXBUFFSIZE		       (0xF7U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_SecureClose()  */
#define RBA_ETHTCP_E_SECURECLOSE                   (0xF8U)
/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_TlsEvent()  */
#define RBA_ETHTCP_TLSEVENT                        (0xF9U)
#endif

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_RxDataAcknowledgment()  */
#define RBA_ETHTCP_E_RXDATAACKNOWLEDGEMENT         (0xFAU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_ResetSock()  */
#define RBA_ETHTCP_E_RESETSOCK                     (0xFBU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_ReopenSock()  */
#define RBA_ETHTCP_E_REOPENSOCK                     (0xFCU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_RxData()  */
#define RBA_ETHTCP_E_RXDATA                         (0xFDU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_RxIndicationSynRcvd_AckProcessing()  */
#define RBA_ETHTCP_E_RXINDICATIONSYNRCVD_ACKPROCESSING      (0xFEU)

/* Indicates that Api Id passed in the call of Det_ReportError corresponds to rba_EthTcp_UpdateSocketAfterSndAck()  */
#define RBA_ETHTCP_E_UPDATESOCKETAFTERSNDACK        (0xFFU)

/* Development Error Codes for DET Support */

/* API service used without rba_EthTcp_Init module initialization */
#define RBA_ETHTCP_E_NOTINIT                        TCPIP_E_NOTINIT

/* Invalid argument received */
#define RBA_ETHTCP_E_INV_ARG                        TCPIP_E_INV_ARG

/* API service called with NULL pointer as an argument */
#define RBA_ETHTCP_E_NULL_PTR                       TCPIP_E_NULL_PTR

/* Invalid Destination IP address */
#define RBA_ETHTCP_E_INV_SOCKADDR                   TCPIP_E_INV_SOCKADDR

/* Address already in use */
#define RBA_ETHTCP_E_ADDRINUSE                      TCPIP_E_ADDRINUSE

/* Can't assign requested address */
#define RBA_ETHTCP_E_ADDRNOTAVAIL                   TCPIP_E_ADDRNOTAVAIL


/*
 ***************************************************************************************************
 * DET Check
 ***************************************************************************************************
 */
#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#if ( TCPIP_DEV_ERROR_DETECT == STD_ON )
#define RBA_ETHTCP_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(RBA_ETHTCP_MODULE_ID, RBA_ETHTCP_INSTANCE_ID, API, ERROR);
#else
#define RBA_ETHTCP_DET_REPORTERROR(API, ERROR)
#endif

#define RBA_ETHTCP_CHECK_DETERROR_RETVAL( CONDITION, API, ERROR, RETVAL )\
    if( CONDITION )                                                      \
    {                                                                    \
        RBA_ETHTCP_DET_REPORTERROR(API, ERROR)                           \
        return (RETVAL);                                                 \
    }                                                                    \

#define RBA_ETHTCP_CHECK_DETERROR_VOID( CONDITION, API, ERROR )          \
    if( CONDITION )                                                      \
    {                                                                    \
        RBA_ETHTCP_DET_REPORTERROR(API, ERROR)                           \
        return;                                                          \
    }                                                                    \

#define RBA_ETHTCP_CHECK_DETERROR( CONDITION, API, ERROR)                \
    if( CONDITION )                                                      \
    {                                                                    \
        RBA_ETHTCP_DET_REPORTERROR(API, ERROR)                           \
    }                                                                    \


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
#endif /* #ifndef RBA_ETHTCP_PRIV_H */
