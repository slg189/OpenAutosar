

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************************/
/* TcpIp_UdpTransmit()  - This service transmits data via UDP to a remote node. The transmission of the data is immediately     */
/*                        performed with this function call by forwarding it to EthIf.                                          */
/*                                                                                                                              */
/* Service ID           - 0x12                                                                                                  */
/* Sync/Async           - Synchronous                                                                                          */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                               */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId             - Socket identifier of the related local socket resource.                                               */
/* DataPtr              - Pointer to a linear buffer of TotalLength bytes containing the data to be transmitted. In case DataPtr*/
/*                        is a NULL_PTR, TcpIp shall retrieve data from upper layer via callback <Up>_CopyTxData().             */
/* RemoteAddrPtr        - IP address and port of the remote host to transmit to.                                                */
/* TotalLength          - Indicates the payload size of the UDP datagram.                                                       */
/*                                                                                                                              */
/* InOut Parameters :                                                                                                           */
/*                                                                                                                              */
/* Return value:                                                                                                                */
/* TcpIp_ReturnType    - TCPIP_OK: UDP message has been forwarded to EthIf for transmission.                                    */
/*                       TCPIP_E_NOT_OK: UDP message could not be sent because of a permanent error, e.g. message is too long.  */
/*                       TCPIP_E_ARP_CACHE_MISS: UDP message could not be sent because of an ARP cache miss, ARP request has    */
/*                       been sent and upper layer may retry transmission by calling this function later again.                 */
/********************************************************************************************************************************/
TcpIp_ReturnType TcpIp_UdpTransmit( TcpIp_SocketIdType SocketId,
                                    const uint8 * DataPtr,
                                    const TcpIp_SockAddrType * RemoteAddrPtr,
                                    uint16 TotalLength )
{
    /* Local variable declaration */
    TcpIp_ReturnType        lTcpIpRetVal_en;

    /* Directly forward to TcpIp_UdpTransmitMetaData() with no metadata given in argument */
    lTcpIpRetVal_en = TcpIp_UdpTransmitMetaData( SocketId,
                               DataPtr,
                               RemoteAddrPtr,
                               TotalLength,
                               NULL_PTR );

    return lTcpIpRetVal_en;
}


/*************************************************************************************************************************************/
/* TcpIp_UdpTransmitMetaData()  - This service transmits data via UDP to a remote node with the possibility to provide metadata      */
/*                                Metadata contains information which helps in doing parallel transmission on the same socket.       */
/*                                                                                                                                   */
/* Service ID                - 0x84                                                                                                  */
/* Sync/Async                - Synchronous                                                                                           */
/* Reentrancy                - Reentrant                                                                                             */
/*                                                                                                                                   */
/* Input Parameters :                                                                                                                */
/* SocketId_u16              - Socket identifier of the related local socket resource.                                               */
/* DataPtr_pcu8              - Pointer to a linear buffer of TotalLength bytes containing the data to be transmitted. In case DataPtr*/
/*                             is a NULL_PTR, TcpIp shall retrieve data from corresponding upper layer callback CopyTxData           */
/*                             ( <Up>_CopyTxData() if MetaDataPtr_pcv in argument is a NULL_PTR or <Up>_CopyTxDataMetaData() if     */
/*                             MetaDataPtr_pcv in argument is a valid Pointer. .                                                     */
/* RemoteAddrPtr_pcst        - IP address and port of the remote host to transmit to.                                                */
/* TotalLength_u16           - Indicates the payload size of the UDP datagram.                                                       */
/* MetaDataPtr_pcv           - MetaData provided by UpperLayer which will be given back within Up_CopyTxDataMetaData                 */
/*                                                                                                                                   */
/* InOut Parameters :                                                                                                                */
/*                                                                                                                                   */
/* Return value:                                                                                                                     */
/* TcpIp_ReturnType    - TCPIP_OK: UDP message has been forwarded to EthIf for transmission.                                         */
/*                       TCPIP_E_NOT_OK: UDP message could not be sent because of a permanent error, e.g. message is too long.       */
/*                       TCPIP_E_ARP_CACHE_MISS: UDP message could not be sent because of an ARP cache miss, ARP request has         */
/*                       been sent and upper layer may retry transmission by calling this function later again.                      */
/*************************************************************************************************************************************/
TcpIp_ReturnType TcpIp_UdpTransmitMetaData( TcpIp_SocketIdType SocketId_u16,
                                            const uint8 * DataPtr_pcu8,
                                            const TcpIp_SockAddrType * RemoteAddrPtr_pcst,
                                            uint16 TotalLength_u16,
                                            const void * MetaDataPtr_pcv )
{
    /* Local variable declaration */
    uint8 *                 lBuffer_pu8;
    BufReq_ReturnType       lProvideTxBuffRetVal_en;
    TcpIp_ReturnType        lTcpIpRetVal_en;
    TcpIp_SocketIdType      lUdpSocIdx_u16;
    uint8                   lBufIdx_u8;
    uint16                  lPort_u16;
    uint32                  lIPv4AddrHolder_u32;
    Std_ReturnType          lRetVal_en;
    TcpIp_LocalAddrIdType   lLocalAddrId_u8;
    uint16                  lLocalPort_u16;

    /* Local variable initialisation */
    lTcpIpRetVal_en             = TCPIP_E_NOT_OK;
    lPort_u16                   = 0U;
    lIPv4AddrHolder_u32         = 0UL;
    lUdpSocIdx_u16              = SocketId_u16 - TCPIP_UDP_SOCKET_OFFSET;       /* Udp API needs socket index to be zero based index */

    /* Check for DET errors */
    /* Check whether TcpIp_Init() has been called or not */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_UDP_TRANSMIT_METADATA_API_ID, TCPIP_E_NOTINIT, TCPIP_E_NOT_OK )

    /* Check whether socket Id is valid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( (TCPIP_UDP_SOCKET_OFFSET > SocketId_u16) || (TCPIP_SOCKETMAX <= SocketId_u16) ), TCPIP_E_UDP_TRANSMIT_METADATA_API_ID, TCPIP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Check whether the RemoteAddrPtr is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR ==  RemoteAddrPtr_pcst ),TCPIP_E_UDP_TRANSMIT_METADATA_API_ID, TCPIP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Get the local addressId and Local port that is bound to SocketId coming from argument to read or write the variables dictating socket operations */
    lRetVal_en = rba_EthUdp_GetSocketInfo( lUdpSocIdx_u16, &lLocalAddrId_u8, &lLocalPort_u16 );

    /* Enter critical section (to avoid race condition if terminating of sockets and sending data is executed concurently) */
    SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

    if( lRetVal_en == E_OK )
    {
        /* If socket closing is not executed in parallel (from the execution context of TcpIp_MainFunction) */
        if( (lLocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) &&
            (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].SockOptStatus_pst->SendingDataCount_u16 < TCPIP_MAXUINT16) &&
            (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].SockOptStatus_pst->ClosingSockInProgress_b ==  FALSE) )
        {
            TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].SockOptStatus_pst->SendingDataCount_u16++;
            lTcpIpRetVal_en = TCPIP_OK;
        }
    }

    /* Exit critical section (to avoid race condition if terminating of sockets and sending data is executed concurently) */
    SchM_Exit_TcpIp_ExclusiveAreaSockOperations();

    if( lTcpIpRetVal_en == TCPIP_OK )
    {
        /* Retrieve the port number based on domain */
        switch(RemoteAddrPtr_pcst->domain)
        {
#if (TCPIP_IPV4_ENABLED == STD_ON)
            case TCPIP_AF_INET:
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType* to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                lPort_u16           = ((const TcpIp_SockAddrInetType*) RemoteAddrPtr_pcst)->port;
                /* lIPv4AddrHolder will hold the IPv4 address, as it is used in the rba_EthUdp_ProvideTxBuffer() function. */
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType* to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                lIPv4AddrHolder_u32    = ((const TcpIp_SockAddrInetType*) RemoteAddrPtr_pcst)->addr[0];
            }
            break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
            case TCPIP_AF_INET6:
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInet6Type* to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above. */
                lPort_u16           = ((const TcpIp_SockAddrInet6Type*) RemoteAddrPtr_pcst)->port;
                /* lIPv4AddrHolder will be set to 0, as it is NOT used in IPv6 specific implementation (to keep the compatibility with rba_EthUdp_ProvideTxBuffer). */
                lIPv4AddrHolder_u32 = 0UL;
            }
            break;
#endif

            default:
            {
                /* Unknown domain. Throw DET report and set invalid domain flag. */
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_UDP_TRANSMIT_METADATA_API_ID, TCPIP_E_AFNOSUPPORT )

                lTcpIpRetVal_en = TCPIP_E_NOT_OK;
            }
            break;
        }

        /* If the domain is invalid, exit the function */
        if( lTcpIpRetVal_en == TCPIP_OK )
        {
            /* Check whether the destination port is Zero */
            TCPIP_DET_REPORT_ERROR_NO_RET( (lPort_u16 == 0U), TCPIP_E_UDP_TRANSMIT_METADATA_API_ID, TCPIP_E_INV_ARG )

            /* Discard the transmission if message length is zero */
            if( ( lPort_u16 != 0U ) && ( TotalLength_u16 > 0U ) )
            {
                /* Local variable initialisation */
                lBuffer_pu8 = NULL_PTR;
                lBufIdx_u8 = 0xFFU;

                /* Call lower layer API to allocate memory */
                lProvideTxBuffRetVal_en = rba_EthUdp_ProvideTxBuffer( lUdpSocIdx_u16,
                                                                      lIPv4AddrHolder_u32,
                                                                      &lBufIdx_u8,
                                                                      &lBuffer_pu8,
                                                                      &TotalLength_u16 );

                /* Check whether lower layer has provided valid buffer */
                if( BUFREQ_OK == lProvideTxBuffRetVal_en )
                {

                    /* Call lower layer transmit API */
                    lTcpIpRetVal_en = rba_EthUdp_Transmit( lUdpSocIdx_u16,
                                                           RemoteAddrPtr_pcst,
                                                           DataPtr_pcu8,
                                                           lBuffer_pu8,
                                                           lBufIdx_u8,
                                                           TotalLength_u16,
                                                           MetaDataPtr_pcv );
                }
                else
                {
                    lTcpIpRetVal_en = TCPIP_E_NOT_OK;
                }

            }
            else
            {
                lTcpIpRetVal_en = TCPIP_E_NOT_OK;
            }
        }
        else
        {
            /* nothing to do. return error due to invalid domain */
        }

        /* Enter critical section ( to avoid race condition if terminating of sockets and sending data is executed at same time ) */
        SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

        /* Binding is not in progress */
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].SockOptStatus_pst->SendingDataCount_u16--;

        /* Exit critical section ( to avoid race condition if terminating of sockets and sending data is executed at same time ) */
        SchM_Exit_TcpIp_ExclusiveAreaSockOperations();
    }

    return lTcpIpRetVal_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif  /* TCPIP_UDP_ENABLED */
#endif /* #ifdef TCPIP_CONFIGURED */
