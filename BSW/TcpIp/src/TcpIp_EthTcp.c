

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************************/
/* TcpIp_TcpTransmit()  - By this API service the reception of socket data is confirmed to the TCP/IP stack.                    */
/*                                                                                                                              */
/* Service ID           - 0x13                                                                                                  */
/* Sync/Async           - Asynchronous                                                                                          */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                               */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId                 - TcpIp internal handle for sockets used in the internal dynamic TCP socket table                   */
/* DataPtr                  - Pointer to a linear buffer of AvailableLength bytes containing the data to be transmitted.        */
/* AvailableLength          - Available data for transmission in bytes.                                                         */
/* ForceRetrieve            - This parameter is only valid if DataPtr is a NULL_PTR.                                            */
/*                            Indicates how the TCP/IP stack retrieves data from SoAd if DataPtr is a NULL_PTR.                 */
/*                                                                                                                              */
/* InOut Parameters :                                                                                                           */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/*                                                                                                                              */
/* Return value:            Std_ReturnType                                                                                      */
/*                              E_OK: The request has been accepted                                                             */
/*                              E_NOT_OK: The request has not been accepted                                                     */
/*                                                                                                                              */
/********************************************************************************************************************************/
Std_ReturnType TcpIp_TcpTransmit( TcpIp_SocketIdType SocketId,
                                  const uint8 * DataPtr,
                                  uint32 AvailableLength,
                                  boolean ForceRetrieve )
{
    Std_ReturnType        lRetType_en;

    lRetType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_TCP_TRANSMIT_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    lRetType_en =  rba_EthTcp_Transmit( SocketId,
                                        DataPtr,
                                        AvailableLength,
                                        ForceRetrieve );

    return lRetType_en;
}


/********************************************************************************************************************************/
/* TcpIp_TcpReceived() - By this API service the reception of socket data is confirmed to the TCP/IP stack.                     */
/*                                                                                                                              */
/* Service ID           - 0x08                                                                                                  */
/* Sync/Async           - Asynchronous                                                                                          */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                               */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId             - TcpIp internal handle for sockets used in the internal dynamic TCP socket table                       */
/* Length               - Number of bytes finally consumed by the upper layer.                                                  */
/*                                                                                                                              */
/* InOut Parameters :                                                                                                           */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/*                                                                                                                              */
/* Return value:        Std_ReturnType                                                                                          */
/*                          E_OK: The request has been accepted                                                                 */
/*                          E_NOT_OK: The request has not been accepted                                                         */
/*                                                                                                                              */
/********************************************************************************************************************************/
Std_ReturnType TcpIp_TcpReceived( TcpIp_SocketIdType SocketId,
                                  uint16 Length )
{
    Std_ReturnType        lRetType_en;

    lRetType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_TCP_RECEIVED_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    lRetType_en = rba_EthTcp_Received( SocketId,
                                       Length );

    return lRetType_en;
}


/********************************************************************************************************************************/
/* TcpIp_TcpConnect()   - By this API service the TCP/IP stack is requested to establish a TCP connection to the configured peer*/
/*                                                                                                                              */
/* Service ID           - 0x06                                                                                                  */
/* Sync/Async           - Asynchronous                                                                                          */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                               */
/*                                                                                                                              */
/* Parameters (in):                                                                                                             */
/* SocketId             - TcpIp internal handle for sockets used in the internal dynamic TCP socket table                       */
/* RemoteAddrPtr        - IP address and port of the remote host to connect to.                                                 */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/*                                                                                                                              */
/* InOut Parameters :                                                                                                           */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/*                                                                                                                              */
/* Return value:        Std_ReturnType                                                                                          */
/*                          E_OK: The request has been accepted                                                                 */
/*                          E_NOT_OK: The request has not been accepted                                                         */
/*                                                                                                                              */
/********************************************************************************************************************************/
Std_ReturnType TcpIp_TcpConnect( TcpIp_SocketIdType SocketId,
                                 const TcpIp_SockAddrType * RemoteAddrPtr )
{
    Std_ReturnType        lRetType_en;

    lRetType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_TCP_CONNECT_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    lRetType_en = rba_EthTcp_Connect( SocketId,
                                      RemoteAddrPtr );

    return lRetType_en;
}


/********************************************************************************************************************************/
/* TcpIp_TcpListen()    - By this API service the TCP/IP stack is requested to listen on the TCP socket specified by the        */
/*                        socket identifier.                                                                                    */
/*                                                                                                                              */
/* Service ID           - 0x07                                                                                                  */
/* Sync/Async           - Asynchronous                                                                                          */
/* Reentrancy           - Reentrant for different SocketIds. Non reentrant for the same SocketId.                               */
/*                                                                                                                              */
/*  Parameters (in):                                                                                                            */
/* SocketId             - TcpIp internal handle for sockets used in the internal dynamic TCP socket table                       */
/* MaxChannels          - Maximum number of new parallel connections established on this listen connection.                     */
/*                                                                                                                              */
/* Parameters (inout):  None                                                                                                    */
/*                                                                                                                              */
/* Parameters (out):    None                                                                                                    */
/*                                                                                                                              */
/* Return value:        Std_ReturnType                                                                                          */
/*                          E_OK: The request has been accepted                                                                 */
/*                          E_NOT_OK: The request has not been accepted                                                         */
/*                                                                                                                              */
/********************************************************************************************************************************/
Std_ReturnType TcpIp_TcpListen( TcpIp_SocketIdType SocketId,
                                uint16 MaxChannels )
{
    Std_ReturnType        lRetType_en;

    lRetType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_TCP_LISTEN_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    lRetType_en = rba_EthTcp_Listen( SocketId,
                                     MaxChannels );

    return lRetType_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* ( TCPIP_TCP_ENABLED == STD_ON ) */
#endif /* #ifdef TCPIP_CONFIGURED */
