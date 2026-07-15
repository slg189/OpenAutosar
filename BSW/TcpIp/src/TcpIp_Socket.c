

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/*********************************************************************************************************************/
/*                                                                                                                   */
/* TcpIp_Prv_GetSocket() - By this API service the TCP/IP stack is requested to allocate a new socket.               */
/*                         Note: Each accepted incoming TCP connection also allocates a socket resource.             */
/*                                                                                                                   */
/* Service ID           - 0x03                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant                                                                                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                                */
/* Domain              - IP address family.                                                                          */
/* Protocol            - Socket protocol as sub-family of parameter type.                                            */
/* SocketOwner         - Specifies which upper layer has requested to allocate a new socket.                         */
/*                                                                                                                   */
/* Output Parameters :                                                                                               */
/* SocketIdPtr      - Pointer to socket identifier representing the requested socket. This socket identifier must be */
/*                    provided for all further API calls which requires a SocketId. Note: SocketIdPtr is only valid  */
/*                    if return value is E_OK.                                                                       */
/*                                                                                                                   */
/* Std_ReturnType     - E_OK or E_NOT_OK - Result of operation                                                       */
/*                                                                                                                   */
/*********************************************************************************************************************/
Std_ReturnType TcpIp_Prv_GetSocket( TcpIp_DomainType Domain,
                                    TcpIp_ProtocolType Protocol,
                                    TcpIp_SocketIdType * SocketIdPtr,
                                    uint8 SocketOwner )
{
    /* Local variable declaration */
    Std_ReturnType              lRetVal_en;

    /* Initialise local variable */
    lRetVal_en      = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_PRV_GET_SOCKET_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    if( TCPIP_IPPROTO_TCP == Protocol )
    {
#if ( TCPIP_TCP_ENABLED == STD_ON )
        lRetVal_en = rba_EthTcp_GetSocket(Domain, SocketIdPtr, SocketOwner);
#else
        /* Report DET if Protocol is not enabled. */
        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_PRV_GET_SOCKET_API_ID, TCPIP_E_NOPROTOOPT)
#endif
    }
    else if( TCPIP_IPPROTO_UDP == Protocol )
    {
#if ( TCPIP_UDP_ENABLED == STD_ON )
        lRetVal_en = rba_EthUdp_GetSocket(Domain, SocketIdPtr, SocketOwner);

        if(E_OK == lRetVal_en)
        {
            /* Offset shall be added to Udp socket index to give the feeling of single socket table being used for Tcp and Udp */
            (*SocketIdPtr) += TCPIP_UDP_SOCKET_OFFSET;
        }
#else
        /* Report DET if Protocol is not enabled. */
        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_PRV_GET_SOCKET_API_ID, TCPIP_E_NOPROTOOPT)
#endif
    }
    else
    {
        /* Report DET if  Protocol is invalid. Only TCPIP_IPPROTO_TCP or TCPIP_IPPROTO_UDP are valid  */
        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_PRV_GET_SOCKET_API_ID, TCPIP_E_PROTOTYPE)
    }

    return lRetVal_en;
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_Bind()         - By this API service the TCP/IP stack is requested to bind a UDP or TCP socket to a local  */
/**                       resource                                                                                  */
/*                                                                                                                  */
/* Service ID           - 0x05                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                   */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId             - Socket identifier of the related local socket resource.                                   */
/* LocalAddrId          - IP address identifier representing the local IP address and EthIf controller to bind the  */
/*                        socket to.                                                                                */
/* InOut Parameters :                                                                                               */
/* PortPtr             - Local port to which the socket shall be bound.                                             */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* Std_ReturnType   - E_OK or E_NOT_OK - Result of operation                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_Bind( TcpIp_SocketIdType SocketId,
                           TcpIp_LocalAddrIdType LocalAddrId,
                           uint16 * PortPtr )
{
    /* Local variable declaration */
    Std_ReturnType                      lRetValStdType_en;

    /* Initialise local variables */
    lRetValStdType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_BIND_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if SocketId is not valid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( TCPIP_SOCKETMAX <= SocketId ), TCPIP_E_BIND_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET if LocalAddrId is not valid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( (TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 <= LocalAddrId), TCPIP_E_BIND_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Enter critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
    SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

    /* If socket closing is not executed in parallel (from the execution context of TcpIp_MainFunction) */
    if( ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].SockOptStatus_pst->BindingSockCount_u16 < TCPIP_MAXUINT16 ) &&
            ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].SockOptStatus_pst->ClosingSockInProgress_b ==  FALSE ) )
    {
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].SockOptStatus_pst->BindingSockCount_u16++;
        lRetValStdType_en = E_OK;
    }

    /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
    SchM_Exit_TcpIp_ExclusiveAreaSockOperations();

    if( lRetValStdType_en == E_OK )
    {
#if ( TCPIP_TCP_ENABLED == STD_ON )
        /* Check whether socket index is in Tcp socket range                                                                                          */
        /* (Tcp socket index range is 0 to TCPIP_UDP_SOCKET_OFFSET - 1 and Udp socket index range is TCPIP_UDP_SOCKET_OFFSET to TCPIP_SOCKETMAX - 1 ) */
        if(  SocketId < TCPIP_TCPSOCKETMAX )
        {
            lRetValStdType_en = rba_EthTcp_Bind( SocketId,
                                                 LocalAddrId,
                                                 PortPtr,
                                                 TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].TcpIpCtrlFramePrio_u8 );

        }
        else
#endif
        {
            /* Call UDP Bind if UDP protocol is enabled. */
#if ( TCPIP_UDP_ENABLED == STD_ON )
            lRetValStdType_en = rba_EthUdp_Bind( (SocketId - TCPIP_UDP_SOCKET_OFFSET),          /* Udp API needs socket index to be zero based index */
                                                 LocalAddrId,
                                                 PortPtr,
                                                 TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].TcpIpCtrlFramePrio_u8 );
#endif
        }

        /* Enter critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
        SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

        /* Binding is not in progress */
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].SockOptStatus_pst->BindingSockCount_u16--;

        /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
        SchM_Exit_TcpIp_ExclusiveAreaSockOperations();
    }
    else
    {
        /* Do Nothing */
    }

    return lRetValStdType_en;
}


/********************************************************************************************************************/
/* TcpIp_Close          - By this API service the TCP/IP stack is requested to close the socket and release all     */
/*                       related resources                                                                          */
/*                                                                                                                  */
/* Service ID           - 0x04                                                                                      */
/* Sync/Async           - Asynchronous                                                                              */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                   */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId             - Socket identifier of the related local socket resource.                                   */
/* Abort                - TRUE: Connection will immediately be terminated by sending a RST-Segment and releasing    */
/*                        all related resources.                                                                    */
/*                        FALSE: Connection will be terminated after performing a regular connection termination    */
/*                        handshake and releasing all related resources.                                            */
/*                        socket to.                                                                                */
/* Output Parameters :                                                                                              */
/* Std_ReturnType        - E_OK / E_NOT_OK - Result of operation                                                    */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_Close( TcpIp_SocketIdType SocketId,
                            boolean Abort )
{
    /* Local variable declaration */
    Std_ReturnType            lRetVal_en;

    /* Initialise local variables */
    lRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_CLOSE_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if SocketId is not less than the total number of sockets */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( TCPIP_SOCKETMAX <= SocketId ), TCPIP_E_CLOSE_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Check whether socket index is in Tcp socket range */
    /* (Tcp socket index range is 0 to TCPIP_UDP_SOCKET_OFFSET - 1 and Udp socket index range is TCPIP_UDP_SOCKET_OFFSET to TCPIP_SOCKETMAX - 1 ) */
#if ( TCPIP_TCP_ENABLED == STD_ON )
    if(SocketId < TCPIP_TCPSOCKETMAX)
    {
        lRetVal_en = rba_EthTcp_Close( SocketId, Abort  );
    }
    else
#endif
    {
        /* Call UDP close if UDP protocol is enabled. */
#if ( TCPIP_UDP_ENABLED == STD_ON )
        rba_EthUdp_Close( SocketId - TCPIP_UDP_SOCKET_OFFSET);                   /* Udp API needs socket index to be zero based index */
        lRetVal_en = E_OK;
#endif
    }

    return lRetVal_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
