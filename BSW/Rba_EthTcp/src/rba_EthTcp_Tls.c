

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Cbk.h"
#include "rba_EthTcp_Prv.h"

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"



/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */



/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/**
 ***************************************************************************************************
 * \Function Name : rba_EthTcp_TlsEvent()
 *
 * \Function description
 *  rba_EthTcp_TlsEvent() gets called by TLS module after tls handshake is succeeded.
 *
 *  Parameters (in):
 * \param   TcpIp_SocketIdType
 * \arg     TcpIpSocId_uo - Socket identifier of the related local socket resource.
 * \param   TcpIp_EventType
 * \arg     Event_en - This parameter contains a Handshake status
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 *
 ***************************************************************************************************
 */
void rba_EthTcp_TlsEvent( TcpIp_SocketIdType TcpIpSocId_uo , \
                          TcpIp_EventType    Event_en )
{
    /* Variable to hold the address of owner config for the given index one by one */
   const TcpIp_SockOwnerConfig_tst *      lSockOwnerConfig_pcst;

    /* Variable to hold the address of dynamic table config for the given index one by one */
    rba_EthTcp_DynSockTblType_tst *       lTcpDynSockTbl_pst;

    /* Variable to hold the address of remote address which shall be passed to UL of Tcp for server use case */
    const TcpIp_SockAddrType *            lRemoteAddr_pst;

#if ( TCPIP_IPV4_ENABLED == STD_ON )
    /* Variable to hold the remote address for IPV4 */
    TcpIp_SockAddrInetType  			  lRemoteAddrInet_st;
#endif /* TCPIP_IPV4_ENABLED == STD_ON */

#if ( TCPIP_IPV6_ENABLED == STD_ON )
    /* Variable to hold the remote address for IPV6 */
   TcpIp_SockAddrInet6Type                lRemoteAddrInet6_st;

    /* Variable to hold the loop index to loop for remote address of IPV6 */
    uint8                                 lLoopIdx_u8;
#endif /* TCPIP_IPV6_ENABLED == STD_ON */

    /* Variable to hold the return value for internal function */
    Std_ReturnType                        lRetVal_en;

    /* Initialise the lRetVal_en with E_OK */
    lRetVal_en = E_OK;

	/* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( FALSE == rba_EthTcp_InitFlag_b ), \
                                      RBA_ETHTCP_TLSEVENT,\
                                      RBA_ETHTCP_E_NOTINIT );

    /* Report DET if TcpIpSocId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( TcpIpSocId_uo >= TCPIP_TCPSOCKETMAX ), \
                                      RBA_ETHTCP_TLSEVENT, \
                                      RBA_ETHTCP_E_INV_ARG );

    /* Assign the address of dynamic scoket table to local pointer for the given index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpIpSocId_uo] ) );

    /* Assign the address of sockety owner config to local pointer for the given index */
    lSockOwnerConfig_pcst = &(TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8]);

    /* Report DET if Event_en is event is other than TCPIP_TLS_HANDSHAKE_SUCCEEDED */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( TCPIP_TLS_HANDSHAKE_SUCCEEDED != Event_en ), \
                                      RBA_ETHTCP_TLSEVENT, \
                                      RBA_ETHTCP_E_INV_ARG );

    /* Report DET if Up_TcpIpEvent_pfct is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( NULL_PTR == lSockOwnerConfig_pcst->Up_TcpIpEvent_pfct),\
                                      RBA_ETHTCP_TLSEVENT, RBA_ETHTCP_E_NULL_PTR);

    /* Set TLS handshake completed flag */
    lTcpDynSockTbl_pst->TlsHandshakeCompleted_b = TRUE;

    /* If static tls secure connection is requested */
    if( TRUE == lTcpDynSockTbl_pst->TlsStaticSecureConnection_b )
    {
        /* Reset the IsTlsStaticSecureConnection flag */
        lTcpDynSockTbl_pst->TlsStaticSecureConnection_b = FALSE;

        /* If connection is requested for server socket */
        if( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo )
        {
            /* Create remote address structure */
            switch( lTcpDynSockTbl_pst->DomainType_u32 )
            {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
                case TCPIP_AF_INET:
                {
                    lRemoteAddrInet_st.domain   = TCPIP_AF_INET;
                    lRemoteAddrInet_st.port     = lTcpDynSockTbl_pst->RemotePort_u16;
                    lRemoteAddrInet_st.addr[0]  = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv4Addr_u32;
                    /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling
                       function. */
                    lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet_st );
                }
                break;
#endif /* TCPIP_IPV4_ENABLED == STD_ON */

#if ( TCPIP_IPV6_ENABLED == STD_ON )
                case TCPIP_AF_INET6:
                {
                    lRemoteAddrInet6_st.domain  = TCPIP_AF_INET6;
                    lRemoteAddrInet6_st.port    = lTcpDynSockTbl_pst->RemotePort_u16;

                    for(lLoopIdx_u8 = 0; lLoopIdx_u8 < 4u; lLoopIdx_u8++)
                    {
                        lRemoteAddrInet6_st.addr[lLoopIdx_u8] = \
                                                lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv6Addr_au32[lLoopIdx_u8];
                    }

                    /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling
                       function. */
                    lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet6_st );
                }
                break;
#endif /* TCPIP_IPV6_ENABLED == STD_ON */

                default:
                {
                    /* Set lRetVal_en to E_NOT_OK  */
                    lRetVal_en = E_NOT_OK;
                    lRemoteAddr_pst = NULL_PTR;
                }
                break;
            }

            if(lRetVal_en == E_OK)
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_TCPIP_00310] ***************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** For static TLS connection assignment at the TCP server side the interface <Up_TcpAccepted> shall not       **/
                /** be called after the ACK has been received.                                                                 **/
                /** Instead, this function shall be called after the TLS handshake has been finished successfully.             **/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                /* Call the UL API to indicate that a new client is accepted */
                /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type */
                lRetVal_en = lSockOwnerConfig_pcst->Up_TcpAccepted_pfct( lTcpDynSockTbl_pst->ListenTcpSockIdx_uo,
                                                                         TcpIpSocId_uo,
                                                                         lRemoteAddr_pst );
            }


        }/* End of if, <If connection is requested for server socket> */
        else
        {
            /* Call UL API to indicate that the connection is now established */
            lSockOwnerConfig_pcst->Up_TcpConnected_pfct( TcpIpSocId_uo );

        }/* End of else, <If connection is requested for server socket> */

    }/* End of If static tls secure connection is requested */

    /* If UL callback function fails */
    if( E_NOT_OK == lRetVal_en )
    {
        /* Send a RST-ACK in the next MainFunction */
        (void)rba_EthTcp_SetNextSendFrame( TcpIpSocId_uo, RBA_ETHTCP_SEND_RST_ACK_E );

    }
    else
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_TCPIP_00345] ***************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** For both dynamic and static TLS connection assignment, the socket owner shall be informed with             **/
        /** <Up_TcpIpEvent> and the event type TCPIP_TLS_HANDSHAKE_SUCCEEDED if an event callback is defined for       **/
        /** a socket owner and the TLS handshake has been finished successfully. For static TLS connection assignment  **/
        /** the call to <Up_TcpIpEvent> and the event type TCPIP_TLS_HANDSHAKE_SUCCEEDED shall take place after the    **/
        /** call to <Up_TcpAccepted>/<Up_TcpConnected>.                                                                **/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        /* Send event to UL to indicate that the TLS handshake is successfull */
        lSockOwnerConfig_pcst->Up_TcpIpEvent_pfct(TcpIpSocId_uo, TCPIP_TLS_HANDSHAKE_SUCCEEDED);
    }


}/* End of function, <rba_EthTcp_TlsEvent> */

/*********************************************************************************************************************************/
/** rba_EthTcp_CheckSecureServerConnection - Function to check local ip is a secure server tls connection.                      **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTbl_pst                            - Pointer to Tcp dynamic socket table entry                                    **/
/** IpAdrr_pcun                                  - requested ip address and port                                                **/
/** TcpIp_TlsConnectionIdType                    - TlsConnection id                                                             **/
/**                                                                                                                             **/
/** Parameters (inout):                            None                                                                         **/
/**                                                                                                                             **/
/** Parameters (out):                              None                                                                         **/
/** Return type:                                   None                                                                         **/
/** Return value:                               - Std_ReturnType                                                                **/
/**                                                  E_OK: Secure tls connection check success                                  **/
/**                                                  E_NOT_OK:Secure tls connection check failed                                **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_CheckSecureServerConnection (const rba_EthTcp_DynSockTblType_tst * TcpDynSockTbl_pst,
                                                       const TcpIp_IPAddrParamType_tun     * IpAdrr_pcun,
                                                       TcpIp_TlsConnectionIdType             TlsConnId_uo)
{
    /* Variable to hold the return value for internal function */
    Std_ReturnType    lRetVal_en;

    lRetVal_en = E_NOT_OK;

    /****************************************************************************************************************/
    /****************************************** [SWS_TCPIP_00308] ***************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** For static TLS connection assignment a port and optionally an address is defined for at least one          **/
    /** TLS connection, TCP shall check during TCP SYN (either reception or transmission of SYN) if a port         **/
    /** assignment is available for any TLS connection and if this TLS connection is not in use. If so, the TCP    **/
    /** shall check the ports and automatically assign this TLS connection to the socket if a port matches.        **/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /****************************************************************************************************************/
    /****************************************** [SWS_TCPIP_00344] ***************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** For static TLS connection assignment the TCP server shall check its local port configuration when the SYN  **/
    /** frame is received. If the TLS port configuration matches it shall assign the corresponding TLS connection  **/
    /** to the socket.                                                                                             **/
    /****************************************************************************************************************/
    /****************************************************************************************************************/


    switch( TcpDynSockTbl_pst->DomainType_u32 )
    {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
        case TCPIP_AF_INET:
        {
            /* Check if requested IpV4 address and port are secure */
            lRetVal_en = rba_EthTls_CheckSecureAddrIpV4( TlsConnId_uo,\
                                                         IpAdrr_pcun->IPv4Par_st.IpAddr_u32, \
                                                         TcpDynSockTbl_pst->LocalPort_u16);
        }
        break;
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
        case TCPIP_AF_INET6:
        {
            /* Check if requested IpV6 address and port are secure */
            lRetVal_en = rba_EthTls_CheckSecureAddrIpV6( TlsConnId_uo,\
                                                         &(IpAdrr_pcun->IPv6Par_st.IpAddr_st.Addr_au32[0]), \
                                                         TcpDynSockTbl_pst->LocalPort_u16);
        }
        break;
#endif

        default:
        {
            /* do nothing */
        }
        break;
    }

    return lRetVal_en;

}

/*********************************************************************************************************************************/
/** rba_EthTcp_CheckSecureClientConnection - Function to check remote ip is a secure client tls connection.                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTbl_pst                            - Pointer to Tcp dynamic socket table entry                                    **/
/** IpAdrr_pcun                                  - requested ip address and port                                                **/
/** TcpIp_TlsConnectionIdType                    - TlsConnection id                                                             **/
/**                                                                                                                             **/
/** Parameters (inout):                            None                                                                         **/
/**                                                                                                                             **/
/** Parameters (out):                              None                                                                         **/
/** Return type:                                   Std_ReturnType                                                               **/
/** Return value:                                 -                                                                             **/
/**                                                  E_OK: Secure tls connection check success                                  **/
/**                                                  E_NOT_OK:Secure tls connection check failed                                **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_CheckSecureClientConnection (const rba_EthTcp_DynSockTblType_tst * TcpDynSockTbl_pst,
                                                       const rba_EthTcp_IPAddr_tun         * IpAdrr_pcun,
                                                       TcpIp_TlsConnectionIdType             TlsConnId_uo)
{
    /* Variable to hold the return value for internal function */
    Std_ReturnType    lRetVal_en;

    lRetVal_en = E_NOT_OK;

    /****************************************************************************************************************/
    /****************************************** [SWS_TCPIP_00343] ***************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** For static TLS connection assignment the TCP client shall check its remote port configuration when the SYN **/
    /** frame will be transmitted. If the TLS port configuration matches it shall assign the corresponding TLS     **/
    /** connection to the socket.                                                                                  **/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    switch( TcpDynSockTbl_pst->DomainType_u32 )
    {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
        case TCPIP_AF_INET:
        {
            /* Check if requested IpV4 address and port are secure */
            lRetVal_en = rba_EthTls_CheckSecureAddrIpV4( TlsConnId_uo,\
                                                         IpAdrr_pcun->IPv4Addr_u32, \
                                                         TcpDynSockTbl_pst->RemotePort_u16);
        }
        break;
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
        case TCPIP_AF_INET6:
        {
            /* Check if requested IpV6 address and port are secure */
            lRetVal_en = rba_EthTls_CheckSecureAddrIpV6( TlsConnId_uo,\
                                                         &(IpAdrr_pcun->IPv6Addr_au32[0]), \
                                                         TcpDynSockTbl_pst->RemotePort_u16);
        }
        break;
#endif

        default:
        {
            /* do nothing */
        }
        break;
    }

    return lRetVal_en;

}


/*********************************************************************************************************************************/
/** rba_EthTcp_OpenSecureConnection- Helper function called by rba_EthTcp_RxIndicationSynRcvd which processes the                 **/
/**                                                reception of a valid frame in SYN_RCVD state, only when the ACK bit is set.  **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                          - Tcp dynamic socket table index                                               **/
/**                                                                                                                             **/
/** Parameters (inout):                            None                                                                         **/
/**                                                                                                                             **/
/** Parameters (out):                              None                                                                         **/
/**                                                                                                                             **/
/** Return type:                                   Std_ReturnType                                                               **/
/** Return value:                                 -                                                                             **/
/**                                                  E_OK: open tls connection check success                                    **/
/**                                                  E_NOT_OK:open tls connection check failed                                  **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_OpenSecureConnection( TcpIp_SocketIdType   TcpDynSockTblIdx_uo)
{

    /* Variable to hold the address of dynamic table config for the given index one by one */
    rba_EthTcp_DynSockTblType_tst *  lTcpDynSockTbl_pst;

    rba_EthTls_ConnectionType_ten    lConnectionType_en;

    Std_ReturnType                   lFunctionRetVal_en;

    /* Assign the address of dynamic scoket table to local pointer for the given index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Call  TcpIp_TlsGetConnectionType to get connection type*/
    lFunctionRetVal_en = rba_EthTls_GetConnectionType(lTcpDynSockTbl_pst-> TlsConnectionId_uo, &lConnectionType_en);

    /* Report DET if Connection id is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL(  (E_NOT_OK == lFunctionRetVal_en),RBA_ETHTCP_E_OPENSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

    /* Report DET if Connection Type is server and listen socket id is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL(  (( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) \
                                          && ( RBA_ETHTLS_CLIENT== lConnectionType_en )),RBA_ETHTCP_E_OPENSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

    /* Report DET if Connection Type is server and listen socket id is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL(  ( ( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID == lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) && \
                                         ( RBA_ETHTLS_SERVER == lConnectionType_en ) ),RBA_ETHTCP_E_OPENSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

    /* Trigger the Tls handshake sequence by calling TcpIp_TlsOpenConnection API */
    lFunctionRetVal_en = rba_EthTls_OpenConnection( TcpDynSockTblIdx_uo,\
                                                    &lTcpDynSockTbl_pst-> TlsConnectionId_uo);

    return lFunctionRetVal_en;
}

/*********************************************************************************************************************************/
/** rba_EthTcp_OpenDynamicSecureConnection - Function to check remote ip is a secure client tls connection.                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                - Tcp dynamic socket table index                                                         **/
/**                                                                                                                             **/
/** Parameters (inout):                  None                                                                                   **/
/**                                                                                                                             **/
/** Parameters (out):                    None                                                                                   **/
/**                                                                                                                             **/
/** Return type:                        Std_ReturnType                                                                          **/
/** Return value:                                 -                                                                             **/
/**                                     E_OK: open tls connection  success                                                      **/
/**                                     E_NOT_OK:open tls connection  failed                                                    **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_OpenDynamicSecureConnection ( TcpIp_SocketIdType  TcpDynSockTblIdx_uo)
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;

    const TcpIp_IPAddrParamType_tun  * lIPAddrParamType_cpun;

    rba_EthTls_ConnectionType_ten    lConnectionType_en;

    TcpIp_TlsConnectionIdType        lTlsConnId_uo;

    Std_ReturnType                   lFunctionRetVal_en;

    Std_ReturnType                   lRetVal_en;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Init local variables */
    lTlsConnId_uo = RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE;

    lFunctionRetVal_en = E_NOT_OK;
    lRetVal_en = E_NOT_OK;

    /* check Connection is IPv4 or IPv6 */
    switch( lTcpDynSockTbl_pst->DomainType_u32 )
    {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
        case TCPIP_AF_INET:
        {
            /* check if it is server connection*/
            if( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo )
            {
                /* fetch the local address*/
                lRetVal_en = TcpIp_GetLocalAddrIdProperties( lTcpDynSockTbl_pst->LocalAddrId_u8, &lLocalAddrConfig_pcst );

                if( lRetVal_en == E_OK )
                {
                    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                    lIPAddrParamType_cpun        = &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un);

                    /* Check if requested local IpV4 address and port are secure and retrieve Tls connection Id */
                    lRetVal_en = rba_EthTlsGetTlsConnIdxforSecureAddrIpV4( lIPAddrParamType_cpun->IPv4Par_st.IpAddr_u32, \
                                                                           lTcpDynSockTbl_pst->LocalPort_u16, \
                                                                           &lTlsConnId_uo);
                }
            }
            else
            {
                /* Check if requested local IpV4 address and port are secure and retrieve Tls connection Id*/
                lRetVal_en = rba_EthTlsGetTlsConnIdxforSecureAddrIpV4( lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv4Addr_u32, \
                                                                       lTcpDynSockTbl_pst->RemotePort_u16, \
                                                                       &lTlsConnId_uo);
            }
        }
        break;
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
        case TCPIP_AF_INET6:
        {
            /* check if it is server connection*/
            if( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo )
            {
                /* fetch the local address*/
                lRetVal_en = TcpIp_GetLocalAddrIdProperties( lTcpDynSockTbl_pst->LocalAddrId_u8, &lLocalAddrConfig_pcst );

                if( lRetVal_en == E_OK )
                {
                    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                    lIPAddrParamType_cpun        = &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un);

                    /* Check if requested local IpV4 address and port are secure and retrieve Tls connection Id */
                    lRetVal_en = rba_EthTlsGetTlsConnIdxforSecureAddrIpV6( &(lIPAddrParamType_cpun->IPv6Par_st.IpAddr_st.Addr_au32[0]), \
                                                                           lTcpDynSockTbl_pst->LocalPort_u16, \
                                                                           &lTlsConnId_uo);
                }
            }
            else
            {
                /* Check if requested local IpV4 address and port are secure and retrieve Tls connection Id */
                lRetVal_en = rba_EthTlsGetTlsConnIdxforSecureAddrIpV6( &(lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv6Addr_au32[0]), \
                                                                       lTcpDynSockTbl_pst->RemotePort_u16, \
                                                                       &lTlsConnId_uo);
            }

        }
        break;
#endif

        default:
        {
          /* do nothing */
        }
        break;
    }

    /* check if it is secure connection */
    if(lRetVal_en == E_OK)
    {
        /* Call  TcpIp_TlsGetConnectionType to get connection type*/
        lRetVal_en = rba_EthTls_GetConnectionType(lTlsConnId_uo, &lConnectionType_en);

        /* Report DET if Connection id is invalid */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL(  (E_NOT_OK == lRetVal_en),RBA_ETHTCP_E_OPENDYNAMICSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

        /* Report DET if Connection Type is server and listen socket id is invalid */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL(  (( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) \
                                              && ( RBA_ETHTLS_CLIENT== lConnectionType_en )),RBA_ETHTCP_E_OPENDYNAMICSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

        /* Report DET if Connection Type is client and listen socket id is valid */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL(  ( ( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID == lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) && \
                                             ( RBA_ETHTLS_SERVER == lConnectionType_en ) ),RBA_ETHTCP_E_OPENDYNAMICSECURECONNECTION, TCPIP_E_INV_ARG, E_NOT_OK);

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
            /* Reset Tcp Tx buffer pointers */
            /* Initially, all pointers will be pointing to the start of the Tcp Tx buffer */

            lTcpDynSockTbl_pst->TxWndUnAckedData_pu8            = lTcpDynSockTbl_pst->TxWndStrt_pu8;
            lTcpDynSockTbl_pst->TxWndNxtSnd_pu8                 = lTcpDynSockTbl_pst->TxWndStrt_pu8;
            lTcpDynSockTbl_pst->TxWndNxtFree_pu8                = lTcpDynSockTbl_pst->TxWndStrt_pu8;
            lTcpDynSockTbl_pst->TxOccupiedSizeSnd_u16           = 0x00U;
            lTcpDynSockTbl_pst->TxOccupiedSizeUnacked_u16       = 0x00U;
#endif

        /* Report DET if Up_TcpIpEvent_pfct is uninitialized */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL( (NULL_PTR == TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct), \
                                           RBA_ETHTCP_E_OPENDYNAMICSECURECONNECTION, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

        /* Callback to UpperLayer */
        TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct( TcpDynSockTblIdx_uo, TCPIP_TLS_HANDSHAKE_INITIATED );

         /* If there is not data in Tcp Tx buffer trigger the rba_EthTls handshake sequence by calling TcpIp_TlsOpenConnection API */
        lRetVal_en = rba_EthTls_OpenConnection(TcpDynSockTblIdx_uo, &lTlsConnId_uo);

         /* If rba_EthTls open connection is successful */
         if(lRetVal_en == E_OK )
         {
             /* assign the rba_Ethtls connection id in dynamic table for future access */
             lTcpDynSockTbl_pst->TlsConnectionId_uo =  lTlsConnId_uo;

             lFunctionRetVal_en = E_OK;

         }
    }

    return lFunctionRetVal_en;

}

/*********************************************************************************************************************************/
/** rba_EthTcp_GetAvailTxBuffSize- The api is called to retrieve free tcp tx buffer length  									**/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** TcpFreeTxBuffSize_pu16           - pointer to free tcp tx buffer length                                                      **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from SoAd to Tcp                                 **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType  rba_EthTcp_GetAvailTxBuffSize( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                               uint16 * TcpFreeTxBuffSize_pu16)
{
    Std_ReturnType   lFunctionRetVal_en;

    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpDynSockTblIdx_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_GETAVAILTXBUFFSIZE, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    *TcpFreeTxBuffSize_pu16 = rba_EthTcp_GetTxBufFreeLen(TcpDynSockTblIdx_uo);

    lFunctionRetVal_en = E_OK;

#else
    (void)TcpFreeTxBuffSize_pu16;

    lFunctionRetVal_en = E_NOT_OK;
#endif

    return lFunctionRetVal_en;
}

/*********************************************************************************************************************************/
/** rba_EthTcp_GetMSS- The api is called to retrieve Tcp MSS length  														**/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** TcpMssValue_pu16           - pointer to Tcp MSS length                                                                      **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from SoAd to Tcp                                 **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType  rba_EthTcp_GetMSS ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,\
                                    uint16 * TcpMssValue_pu16)
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpDynSockTblIdx_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_GETMSS, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst  = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    *TcpMssValue_pu16 = ( (lTcpDynSockTbl_pst->RcvdMss_u16 < lTcpDynSockTbl_pst->LocalMss_u16) ? lTcpDynSockTbl_pst->RcvdMss_u16 : \
                                                                                                 lTcpDynSockTbl_pst->LocalMss_u16);

    return E_OK;

}

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
/*********************************************************************************************************************************/
/** rba_EthTcp_ReserveFreeQSlotForTlsTx- The api is called to reserve free queue to store tx requets on a socket                    **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/**                                                                                                                             **/
/** Return value:                   - None                                                                                      **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void  rba_EthTcp_ReserveFreeQSlotForTlsTx ( TcpIp_SocketIdType TcpDynSockTblIdx_uo)
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    TcpIp_TlsConnectionIdType       lLoopIndex;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst  = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* loop through maximum configured tls connections */
    for(lLoopIndex = 0; lLoopIndex < RBA_ETHTCP_CFG_MAX_NO_OF_Q_FOR_TLS_CONN; lLoopIndex++)
    {
        /* check if queue is available to store tx requests for a socket */
        if(rba_EthTcp_TlsTxReqQInfo_ast[lLoopIndex].IsQueueAllocated_b == FALSE)
        {
            /* store the queue index allocated to tcp socket */
            lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo = lLoopIndex;

            /* set flag to true */
            rba_EthTcp_TlsTxReqQInfo_ast[lLoopIndex].IsQueueAllocated_b = TRUE;

            break;
        }
    }

}
#endif

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif
#endif /* TCPIP_TCP_ENABLED == STD_ON */

