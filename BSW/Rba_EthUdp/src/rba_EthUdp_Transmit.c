
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"
#include "rba_BswSrv.h"
#include "rba_EthUdp_Prv.h"

/*
***************************************************************************************************
* Static function declaration
***************************************************************************************************
*/

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

static Std_ReturnType rba_EthUdp_ConstructTxFrame( TcpIp_SocketIdType SocketId_u16,
                                                   const TcpIp_SockAddrType * RemoteAddr_pst,
                                                   const uint8 *  DataPtr_pcu8,
                                                   uint16 DataLength_u16,
                                                   uint8  *TxBuffer_pu8,
                                                   const void * MetaDataPtr_pcv );

static Std_ReturnType  rba_EthUdp_GetUdpCksum_IPv4( TcpIp_SocketIdType SocketId_u16,
                                                    uint32 RemoteIpAddr_u32,
                                                    const uint8 * TxBuffer_pcu8,
                                                    uint16 TxBufferLength_u16,
                                                    uint16 * UdpChecksum_pu16 );
#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/********************************************************************************************************************/
/* rba_EthUdp_ProvideTxBuffer() This function is called from TcpIp or DHCP COMPs to UDP layer  to provide buffer    */
/*                              to copy payload and then transmit this data by calling rba_EthUdp_Transmit(). Add   */
/*                              UDP header length before requesting for buffer.                                     */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId (from [SWS_TCPIP_00025])     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16                Socket id on which data is to be transmitted                                         */
/* DestIPv4Addr_u32            IP address of the destination node to which this node wants to transmit data         */
/*                                                                                                                  */
/* Parameter IN-OUT: none                                                                                           */
/*                                                                                                                  */
/* Parameter OUT:                                                                                                   */
/* Buffer_ppu8                 Pointer to the granted buffer. A double pointer shall be passed and the address of   */
/*                             the granted buffer shall be updated into this.                                       */
/* BufIdx_pu8                  Index to the granted buffer resource. This shall be used during transmit request     */
/* GrantLength_pcu16           Requested buffer length. The lower layer can give a smaller buffer than requested.   */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* BufReq_ReturnType           BUFREQ_OK        : Buffer successfully provided                                      */
/*                             BUFREQ_E_NOT_OK  : Development error detected                                        */
/*                             BUFREQ_E_BUSY    : All buffers in use                                                */
/*                                                                                                                  */
/* Notes :                                                                                                          */
/*  DD: This function adds the UDP header length to the incoming requested buffer length in order to allow for UDP  */
/*  protocol data.                                                                                                  */
/*  DD: Return error if the socket is not bound.                                                                    */
/*  DD: In case the lower layer returns "buffer overflow", return a "buffer not ok" error to upper layer.           */
/*  Reason:  The use case of re-trying an allocation in case of err code OVerflow is not used in case of UDP.       */
/*           It is also not re-entrant.                                                                             */
/*             + it complicates the code in some corner cases.                                                      */
/********************************************************************************************************************/
BufReq_ReturnType rba_EthUdp_ProvideTxBuffer( TcpIp_SocketIdType SocketId_u16,
                                              uint32 DestIPv4Addr_u32,
                                              uint8 * BufIdx_pu8,
                                              uint8 * *Buffer_ppu8,
                                              const uint16 * GrantLength_pcu16 )
{
    /* Local variable declaration */
    uint16                                      lGrantLength_u16;
    uint8                                       lBufferIdx_u8;
    uint8 *                                     lBuffer_pu8;
    BufReq_ReturnType                           lProvideTxBuffRetVal_en;
    Std_ReturnType                              lOperationRetVal_en;

    /* Checks: Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((NULL_PTR == rba_EthUdp_Config_pco), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_NOTINIT, BUFREQ_E_NOT_OK)

    /* Report DET if SocketId_u16 is not plausible*/
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((TCPIP_UDPSOCKETMAX <= SocketId_u16), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_INV_ARG, BUFREQ_E_NOT_OK)

    /* Report DET if GrantLength_pcu16 is NULL*/
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((NULL_PTR == GrantLength_pcu16), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_NULL_PTR, BUFREQ_E_NOT_OK)

    /* Report DET if BufIdx_pu8 is NULL*/
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((NULL_PTR == BufIdx_pu8), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_NULL_PTR, BUFREQ_E_NOT_OK)

    /* Report DET if Buffer_ppu8 is NULL*/
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((NULL_PTR == Buffer_ppu8), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_NULL_PTR, BUFREQ_E_NOT_OK)

    /* Report DET if GrantLength_pcu16 is greater than the maximum allowed Udp payload length */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE(((*GrantLength_pcu16) > (uint16)(TCPIP_MAXUINT16 - RBA_ETHUDP_HDRLENGTH)), RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_INV_ARG, BUFREQ_E_NOT_OK)

    /* Local Variable initialization */
    lProvideTxBuffRetVal_en = BUFREQ_E_NOT_OK;

    /* Check if ProvideTxBuffer operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the ProvideTxBuffer operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* DD: return no buffer available if the socket is not bound. */
        /* Check whether the requested socket state is RBA_ETHUDP_SOCK_BOUND */
        if( RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en )
        {
            /*DD: Add UDP header length to the buffer length requested by upper layer to allow space for the UDP protocol overhead*/
            lGrantLength_u16 = (*GrantLength_pcu16) + RBA_ETHUDP_HDRLENGTH;

            /* Initialize provided buffer pointer and buffer index */
            lBufferIdx_u8 = 0U;
            lBuffer_pu8 = NULL_PTR;

            switch (rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32)
            {
#if (TCPIP_IPV4_ENABLED == STD_ON)
                case TCPIP_AF_INET:
                {
                    /* If destination IPv4 address is not ANY */
                    if( TCPIP_IPADDR_ANY != DestIPv4Addr_u32 )
                    {
                        lProvideTxBuffRetVal_en = rba_EthIPv4_ProvideTxBuffer(rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                                                        DestIPv4Addr_u32,
                                                                        rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8,
                                                                        &lBufferIdx_u8,
                                                                        &lBuffer_pu8,
                                                                        &lGrantLength_u16);
                    }
                    else
                    {
                        /* Report DET if DestIPv4Addr_u32 is 0.0.0.0 and return BUFREQ_E_NOT_OK at the end of the function */
                        RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_INV_ARG )
                        /* If DET is disabled, default statement will be empty. Nothing to do here. */
                    }
                }
                break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
                case TCPIP_AF_INET6:
                {
                    (void)DestIPv4Addr_u32;
                    lProvideTxBuffRetVal_en = rba_EthIPv6_ProvideTxBuffer(rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                                                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8,
                                                                    &lBufferIdx_u8,
                                                                    &lBuffer_pu8,
                                                                    &lGrantLength_u16);
                }
                break;
#endif

                default:
                {
                    /* Unknown domain. Throw DET report and break execution. */
                    RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHUDP_E_NOPROTOOPT)
                    /* If DET is disabled, default statement will be empty. Nothing to do here. */
                }
                break;
            }

            if (BUFREQ_OK == lProvideTxBuffRetVal_en)
            {
                /* Buffer size is OK, move the pointer to application payload start; substract the UDP header size */
                *Buffer_ppu8 = lBuffer_pu8 + RBA_ETHUDP_HDRLENGTH;
                *BufIdx_pu8 = lBufferIdx_u8;
                /* Grant length does not need to be assigned, because it is all ok, as requested by the caller. No change needed. */
            }
            else if(BUFREQ_E_OVFL == lProvideTxBuffRetVal_en)
            {
                /* DD: the use case of re-trying an allocation in case of error code Overflow is not used. It is also not re-entrant.
                   Moreover, it complicates the code -> hence we do not support it. */
                lProvideTxBuffRetVal_en = BUFREQ_E_NOT_OK;
            }
            else
            {
                /* Do nothing. Propagate the error code. */
            }
        }
        else
        {
            /* Do nothing. Socket not bound. No buffer. */
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }
    else
    {
        /* ProvideTxBuffer operation is not allowed for execution, BUFREQ_E_NOT_OK will be returned */
    }

    return lProvideTxBuffRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthUdp_Transmit() This function is called from TcpIp or DHCP COMPs to transmit data over UDP. In this API    */
/*                        the UDP header is filled with source port, destination port, length and checksum.         */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Parameter IN:                                                                                                    */
/* SocketId_u16          Socket id on which data is to be transmitted                                               */
/* RemoteAddr_pst        Pointer to a structure containing remote IP address and remote port                        */
/* DataPtr_pcu8          Pointer to the upper layer data                                                            */
/* Buffer_pu8            Pointer to the buffer where data to be transmitted is copied.                              */
/* BufIdx_u8             Index to the buffer where the data is copied.                                              */
/* BufLength_u16         Length of data to be transmitted                                                           */
/* MetaDataPtr_pcv       MetaData provided by UpperLayer which will be given back within Up_CopyTxDataMetaData      */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : UDP datagram transmission successful                           */
/*                       TCPIP_E_NOT_OK            : UDP datagram transmission failed (general failure)             */
/*                       TCPIP_E_ARP_CACHE_MISS    : UDP datagram transmission failed because of ARP cache miss     */
/*                                                                                                                  */
/* Notes :                                                                                                          */
/*  DD: Checksum field will be calculated in the IP transmit function because the src and dest IPs must be          */
/*  calculated there anyway. (avoid two calls to resolve src IP)                                                    */
/*                                                                                                                  */
/*  Implementation:                                                                                                 */
/*  - fill the mandatory UDP header fields: src port, dest port, length, (cksum)*                                   */
/*  - switch(AF domain)                                                                                             */
/*      to decide what lower layer Transmit function to call.                                                       */
/********************************************************************************************************************/
TcpIp_ReturnType rba_EthUdp_Transmit( TcpIp_SocketIdType SocketId_u16,
                                      const TcpIp_SockAddrType * RemoteAddr_pst,
                                      const uint8 * DataPtr_pcu8,
                                      uint8 * Buffer_pu8,
                                      uint8 BufIdx_u8,
                                      uint16 BufLength_u16,
                                      const void * MetaDataPtr_pcv )
{
    /* Local variable declaration */
    TcpIp_IpHeader_tst                          lIpHeader_st;
    uint16                                      lTotUdpLength_u16;
    TcpIp_ReturnType                            lTcpIpRetVal_en;
    Std_ReturnType                              lRetVal_en;
    Std_ReturnType                              lOperationRetVal_en;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_NOTINIT, TCPIP_E_NOT_OK )

    /* Report DET if RemoteAddr_pst is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddr_pst ), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Check whether socket handle is a plausible value */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( SocketId_u16 >= TCPIP_UDPSOCKETMAX ), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if Buffer_pu8 is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == Buffer_pu8 ), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Local Variable initialization */
    lTcpIpRetVal_en = TCPIP_E_NOT_OK;

    /* Check if Transmit operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the Transmit operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Report DET: If rba_EthUdp_Transmit() is called on the socket which is not in state RBA_ETHUDP_SOCK_BOUND. */
        /* Socket should never be closed after rba_EthUdp_ProvideTxBuffer is called and before rba_EthUdp_Transmit is completed. */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( (RBA_ETHUDP_SOCK_BOUND != rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )

        /* Report DET: Local Domain not matching remote domain */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( (RemoteAddr_pst->domain != rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32), RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )

        /* If rba_EthUdp_Transmit() is called on the socket which is in state RBA_ETHUDP_SOCK_BOUND and local domain matching remote domain */
        if( (RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en) &&
            (RemoteAddr_pst->domain == rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32) )
        {
            /* Common code: provide protocol and TTL/Hop Limit information to rba_EthIP layer. */
            lIpHeader_st.ProtoType_en = TCPIP_IPPROTO_UDP;
            lIpHeader_st.Ttl_u8 = rba_EthUdp_Config_pco->Ttl_u8; /* The TTL field specific for IPv4 is actually Hop Limit in the IPv6 protocol. */
            lIpHeader_st.SockOwner_u8 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8;  /* Update the socket owner information for IPv4 layer */

            /* Update the DSCP value in the IPHeader_st for IPv6 layer (currently not used in IPv4 layer). */
            lIpHeader_st.DiffServiceCodepoint_u8 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockDiffServiceCodePoint_u8;
#if ( TCPIP_IPV6_ENABLED == STD_ON )
            lIpHeader_st.FlowLabel_u32 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFlowLabel_u32; /* Update the flow label information for IPv6 layer */
#else
            lIpHeader_st.FlowLabel_u32 = 0UL; /* Set to 0 as flow label is an IPv6 field and it is not used in IPv4 header. */
#endif

            /* AF domain specific code to verify the remote IP address validity */
            switch( rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 )
            {
#if (TCPIP_IPV4_ENABLED == STD_ON)
                case TCPIP_AF_INET:
                {
                    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 data. It is safe because of the switch before. */
                    const TcpIp_SockAddrInetType * lRemoteAdrIPv4_pst = (const TcpIp_SockAddrInetType*) RemoteAddr_pst;

                    /* MR12 RULE 10.4 VIOLATION: Type definition of wildcard IP address is according to AUTOSAR [SWS_TCPIP_00133]. */
                    if( TCPIP_IPADDR_ANY != lRemoteAdrIPv4_pst->addr[0] )
                    {
                        lTcpIpRetVal_en = TCPIP_OK;
                    }
                    else
                    {
                        /* Report DET: If remote IP is 0.0.0.0  */
                        RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )
                    }
                }
                break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
                case TCPIP_AF_INET6:
                {
                    lTcpIpRetVal_en = TCPIP_OK;
                }
                break;
#endif

                default:
                {
                    /* Unknown domain. Throw DET report and break execution. */
                    RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_NOPROTOOPT )
                    /* If DET is disabled, default statement will be empty. Nothing to do here. */
                }
                break;
            }

            /* If the the remote IP address has been successfully extracted */
            if( lTcpIpRetVal_en == TCPIP_OK )
            {
                /* Decrement the buffer pointer by UDP header length */
                Buffer_pu8 = Buffer_pu8 - RBA_ETHUDP_HDRLENGTH;

                /* Fill the UDP header, payload and handle the checksum calculation if IPv4 */
                lRetVal_en = rba_EthUdp_ConstructTxFrame( SocketId_u16,
                                                         RemoteAddr_pst,
                                                         DataPtr_pcu8,
                                                         BufLength_u16,
                                                         Buffer_pu8,
                                                         MetaDataPtr_pcv );

                /* In case of E_NOT_OK, Transmit the frame with a Udp length equal to 0 in purpose to free the buffer */
                lTotUdpLength_u16 = (lRetVal_en == E_NOT_OK) ? (0U) : (uint16)(BufLength_u16 + RBA_ETHUDP_HDRLENGTH);

                /*Domain can only be TCPIP_AF_INET or TCPIP_AF_INET6. Validation for Domain type has been handled in above switch case*/
#if (TCPIP_IPV4_ENABLED == STD_ON)
                if( TCPIP_AF_INET == rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32)
                {
                    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 data. It is safe because of the switch before. */
                    const TcpIp_SockAddrInetType * lRemoteAdrIPv4_pst = (const TcpIp_SockAddrInetType*) RemoteAddr_pst;

                    lTcpIpRetVal_en = rba_EthIPv4_Transmit( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8, lRemoteAdrIPv4_pst->addr[0], Buffer_pu8, BufIdx_u8, &lIpHeader_st, lTotUdpLength_u16 );
                }
                else
#endif
                {
#if (TCPIP_IPV6_ENABLED == STD_ON)
                    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv6 data. It is safe because of the switch before. */
                    const TcpIp_SockAddrInet6Type * lRemoteAdrIPv6_pst = (const TcpIp_SockAddrInet6Type*) RemoteAddr_pst;

                    lTcpIpRetVal_en = rba_EthIPv6_Transmit( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8, &(lRemoteAdrIPv6_pst->addr[0]), Buffer_pu8, BufIdx_u8, &lIpHeader_st, lTotUdpLength_u16 );
#endif
                }

                /* If Up_CopyTxData() failed or buffer is freed, return TCPIP_E_NOT_OK */
                lTcpIpRetVal_en = (lTotUdpLength_u16 == 0U) ? (TCPIP_E_NOT_OK) : (lTcpIpRetVal_en);
            }
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }
    else
    {
        /* Transmit operation is not allowed for execution, BUFREQ_E_NOT_OK will be returned */
    }

    return lTcpIpRetVal_en;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* rba_EthUdp_ConstructTxFrame() This is a helper function called by rba_EthUdp_Transmit and it performs the        */
/* construction of the UDP transmitted frame (Data+Header). the UDP header is filled with source port, destination  */
/* port, length  and checksum. Also, it handle the checksum calculation if the internet protocol is IPv4            */
/*                                                                                                                  */
/* Synchronous,Reentrant                                                                                            */
/*                                                                                                                  */
/* Parameter IN:                                                                                                    */
/* SocketId_u16          Socket id on which data is to be transmitted                                               */
/* RemoteAddr_pst        Pointer to a structure containing remote IP address and remote port                        */
/* DataPtr_pcu8          Pointer to the upper layer data.                                                           */
/* DataLength_u16        Length of data to be transmitted                                                           */
/* TxBuffer_pu8          Pointer to the transmission buffer where UDP Header and data to be transmitted is copied.  */
/* MetaDataPtr_pcv       MetaData provided by UpperLayer which will be given back within Up_CopyTxDataMetaData      */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : UDP datagram transmission successful                           */
/*                       TCPIP_E_NOT_OK            : UDP datagram transmission failed (general failure)             */
/*                       TCPIP_E_ARP_CACHE_MISS    : UDP datagram transmission failed because of ARP cache miss     */
/********************************************************************************************************************/
static Std_ReturnType rba_EthUdp_ConstructTxFrame( TcpIp_SocketIdType SocketId_u16,
                                                   const TcpIp_SockAddrType * RemoteAddr_pst,
                                                   const uint8 * DataPtr_pcu8,
                                                   uint16 DataLength_u16,
                                                   uint8  * TxBuffer_pu8,
                                                   const void * MetaDataPtr_pcv )
{
    /* Local variable declaration */
    uint16                      lTotUdpLength_u16;
    uint16                      lUDPChksum_u16;
    Std_ReturnType              lRetVal_en;
    BufReq_ReturnType           lConfCallbckRetVal_en;

    /* Local variable Initialization  */
    lRetVal_en = E_NOT_OK;
    lConfCallbckRetVal_en = BUFREQ_E_NOT_OK;

    /* Copy the upper layer data into the buffer provided by lower layer */
    if( NULL_PTR == DataPtr_pcu8 )
    {
        /* In case no MetaData is provided in argument (normal Autosar use case) */
        if( MetaDataPtr_pcv == NULL_PTR )
        {
            /* Check if upper layer callback API is configured */
            if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_CopyTxData_pfct != NULL_PTR )
            {
                /* Socket index shall be consecutive numbers of Tcp and udp socket index */
                /* Call configured Up_CopyTxData for the corresponding socket owner */
                lConfCallbckRetVal_en = TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_CopyTxData_pfct( SocketId_u16 + TCPIP_UDP_SOCKET_OFFSET,
                                                                                                                                                                    TxBuffer_pu8 + RBA_ETHUDP_HDRLENGTH,
                                                                                                                                                                    DataLength_u16 );
            }
        }
        /* In case MetaData is provided - use case */
        /* MetaData is useful when TcpIp_UdpTransmit is reentered (multicore or multitasking) */
        /* (UL can perform multiple Udp transmission in parallel for the same socket and requires extra information in CopyTxDataMetaData to know which Pdu triggered the transmission initially) */
        else
        {
            /* Check if upper layer callback API is configured */
            if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_CopyTxDataMetaData_pfct != NULL_PTR )
            {
                /* Socket index shall be consecutive numbers of Tcp and udp socket index */
                /* Call configured Up_CopyTxDataMetaData for the corresponding socket owner */
                lConfCallbckRetVal_en = TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_CopyTxDataMetaData_pfct( SocketId_u16 + TCPIP_UDP_SOCKET_OFFSET,
                                                                                                                                                                            TxBuffer_pu8 + RBA_ETHUDP_HDRLENGTH,
                                                                                                                                                                            DataLength_u16,
                                                                                                                                                                            MetaDataPtr_pcv );
            }
        }
    }
    else
    {
        /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. */
        (void)rba_BswSrv_MemCopy( TxBuffer_pu8 + RBA_ETHUDP_HDRLENGTH,
                                  DataPtr_pcu8,
                                  DataLength_u16 );

        /* Set lConfCallbckRetVal_en to  BUFREQ_OK to update the UDP header fields in the buffer TxBuffer_ppu8 */
        lConfCallbckRetVal_en = BUFREQ_OK;
    }

    /* Copy the UDP Header and process the checksum calculation */
    if( BUFREQ_OK == lConfCallbckRetVal_en )
    {
        /* Update the total UDP frame length */
        lTotUdpLength_u16 = (uint16)(DataLength_u16 + RBA_ETHUDP_HDRLENGTH);

        /* UDP header: fill source port */
        TcpIp_WriteU16( TxBuffer_pu8 + TCPIP_TP_SRCPORT_OFFSET, rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16 );

        /* Update the length of the UDP frame into the UDP header field */
        TcpIp_WriteU16( TxBuffer_pu8 + RBA_ETHUDP_UDPFRAMELENGTH_UB, lTotUdpLength_u16 );

#if (TCPIP_IPV4_ENABLED == STD_ON)
        if( TCPIP_AF_INET == rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 )
        {
            /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 data. It is safe because of the switch before. */
            const TcpIp_SockAddrInetType * lRemoteAdrIPv4_pst = (const TcpIp_SockAddrInetType*) RemoteAddr_pst;

            /* UDP header: fill destination port */
            TcpIp_WriteU16( TxBuffer_pu8 + TCPIP_TP_DESTPORT_OFFSET, lRemoteAdrIPv4_pst->port );

            /*Initialize the checksum output */
            lUDPChksum_u16 = 0U;

            /* Set checksum field to 0 to not influence the checksum calculation later in rba_EthUdp_GetUdpCksum_IPv4 function. */
            TcpIp_WriteU16( TxBuffer_pu8 + RBA_ETHUDP_UDPCHKSUM_UB, lUDPChksum_u16 );

            /* Get the checksum for the transmitted frame  */
            lRetVal_en = rba_EthUdp_GetUdpCksum_IPv4( SocketId_u16,
                                                      lRemoteAdrIPv4_pst->addr[0],
                                                      TxBuffer_pu8,
                                                      lTotUdpLength_u16,
                                                      &lUDPChksum_u16);

            /* Transmit the frame if the checksum was executed and computed correctly */
            if( E_OK == lRetVal_en )
            {
                /* Set checksum field to the checksum calculation result. */
                TcpIp_WriteU16( TxBuffer_pu8 + RBA_ETHUDP_UDPCHKSUM_UB, lUDPChksum_u16 );
            }
        }
        else
#endif
        {
#if (TCPIP_IPV6_ENABLED == STD_ON)
            /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv6 data. It is safe because of the switch before. */
            const TcpIp_SockAddrInet6Type * lRemoteAdrIPv6_pst = (const TcpIp_SockAddrInet6Type*) RemoteAddr_pst;

            /* UDP header: fill destination port */
            TcpIp_WriteU16( TxBuffer_pu8 + TCPIP_TP_DESTPORT_OFFSET, lRemoteAdrIPv6_pst->port );

            /* Set checksum field to 0 to not influence the checksum calculation later in IP transmit function,
             * as checksum for IPv6 Tx path is computed inside rba_EthIPv6 module. */
            TcpIp_WriteU16( TxBuffer_pu8 + RBA_ETHUDP_UDPCHKSUM_UB, 0U );
            lRetVal_en = E_OK;
#endif
        }
    }

    return lRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthUdp_GetUdpCksum_IPv4() This API is the one responsible of updating and computing the UDP SW checksum, it  */
/* is called by rba_EthUdp_Transmit()                                                                               */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds                                                                   */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16          Socket id on which data is to be transmitted                                               */
/* RemoteIpAddr_u32      Destination IP adress.                                                                     */
/* TxBuffer_pcu8         Pointer to the buffer where Udp header and application data to be transmitted is copied.   */
/* TxBufferLength_u16    Length of Udp header and application data to be transmitted                                */
/* UdpChecksum_pu16      Checksum computing result.                                                                 */
/*                                                                                                                  */
/* Return type :            Std_ReturnType                                                                          */
/*                          - E_OK, if checksum compution is done.                                                  */
/*                          - E_NOT_OK,  if checksum compution is failed.                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static Std_ReturnType  rba_EthUdp_GetUdpCksum_IPv4(TcpIp_SocketIdType SocketId_u16,
                                                   uint32 RemoteIpAddr_u32,
                                                   const uint8 * TxBuffer_pcu8,
                                                   uint16 TxBufferLength_u16,
                                                   uint16 * UdpChecksum_pu16)
{
    /* Local variable declaration */
    Std_ReturnType                              lRetValStdType_en;
    uint8                                       lNumOfIPFrags_u8;
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
    uint16                                      lChksumCalc_u16;
    TcpIp_PseudoHdr_tst                         lPseudoHdr_st;
#endif
#if (RBA_ETHUDP_SW_CHKSUM == STD_ON)
    uint8                                       lEthIfCtrl_u8;
#endif

    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Check if the software UDP checksum flag is disabled or not */
    if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8 != STD_OFF )
    {
        /* Initialize the number of the fragments */
        lNumOfIPFrags_u8 = 1U;

        /* Check if there is a fragmentation in the frame to be transmitted */
        lRetValStdType_en = rba_EthIPv4_CheckFragmentation( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                                            TxBufferLength_u16,
                                                            &lNumOfIPFrags_u8 );

        if( lRetValStdType_en == E_OK )
        {
#if (RBA_ETHUDP_SW_CHKSUM == STD_ON)
            lEthIfCtrl_u8 = rba_EthUdp_LocalAddressToCtrlMap_cau8[rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8];
#endif
            /* Checksum shall be calculated by software if */
            /* 1. The software checksum calculation for dedicated lEthIfCtrl_u8 is not offloaded to the driver OR   */
            /* 2. The frame undergoes fragmentation */
            if(
#if (RBA_ETHUDP_SW_CHKSUM == STD_ON)
               (rba_EthUdp_ChecksumOffloading_cab[lEthIfCtrl_u8] == FALSE) ||
#endif
               (lNumOfIPFrags_u8 > 1U) )
            {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
                /* ---------------------- */
                /* PseudoHeader Creation  */
                /* ---------------------- */

                /* Create the IPv4 pseudo header (protocol, local IP, Remote IP and length) to calculate UDP checksum */
                lPseudoHdr_st.IpDomainType_t = TCPIP_AF_INET;
                lPseudoHdr_st.ProtoType_en = TCPIP_IPPROTO_UDP;

                /* Copy destination IPv4 address */
                lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32 = RemoteIpAddr_u32;

                /* Get the rest of the PseudoHeader parts by calling IPv4 module */
                lRetValStdType_en = rba_EthIPv4_GetPseudoHeader( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                                                 rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8,
                                                                 TxBufferLength_u16,
                                                                 &lPseudoHdr_st );

                if( lRetValStdType_en == E_OK )
                {
                    /* ------------------------------ */
                    /* Checksum computation           */
                    /* ------------------------------ */

                    /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
                    lRetValStdType_en = TcpIp_OnesComplChkSum( TxBuffer_pcu8, &lPseudoHdr_st, TxBufferLength_u16, &lChksumCalc_u16 );

                    /* Once checksum calculation is finished, copy the result in the specific field belongs to UDP checksum */
                    if( lRetValStdType_en == E_OK )
                    {
                        /* For UDP, if checksum evaluates to 0 it shall be changed to 0xffff as per RFC 768 */
                        lChksumCalc_u16 = (lChksumCalc_u16 == 0U) ? 0xFFFFU : lChksumCalc_u16;

                        /* Store the computed checksum in the out argument */
                        (*UdpChecksum_pu16) = lChksumCalc_u16;
                    }
                }
#else
                /* RemoteIpAddr_u32 and TxBuffer_pcu8 is unused if TCPIP_ONES_COMPL_CHKSUM_ENABLED is OFF. */
                (void)RemoteIpAddr_u32;
                (void)TxBuffer_pcu8;

                /* Report DET: If TcpIp_OnesComplChkSum() is not available */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_TRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )
                lRetValStdType_en = E_NOT_OK;
#endif
            }
            else
            {
                /* No need to calculate checksum.Set checksum field to 0 because the HW will do it */
                (*UdpChecksum_pu16) = 0U;
                lRetValStdType_en = E_OK;
            }
        }
    }
    else /* rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8) = FALSE  */
    {
        /* UDP SW checksum should not be executed since it was disabled optionnally. and in this */
        /* case, the frame shall be sent with the value 0 inside the field belong to the UDP checksum */
        (*UdpChecksum_pu16) = 0U;
        lRetValStdType_en = E_OK;
    }

    return lRetValStdType_en;
}
#endif

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* #if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
