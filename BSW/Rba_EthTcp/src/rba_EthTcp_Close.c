

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"
static Std_ReturnType rba_EthTcp_GracefulClose        ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void           rba_EthTcp_AbortClose           ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void           rba_EthTcp_AbortRelatedChannels ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_Close()  - By this API, the rba_EthTcp module is requested to close socket and release all related resources     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo        - Tcp Socket id                                                                                         **/
/** Abort_b             - This parameter specifies if the connection should be terminated immediately by sending a RST          **/
/**                       segment or be terminated after performing a regular connection termination handshake                  **/
/**                                                                                                                             **/
/** Parameters (inout):   None                                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):     None                                                                                                  **/
/**                                                                                                                             **/
/** Return value:       - Std_ReturnType                                                                                        **/
/**                       E_OK: The request has been accepted                                                                   **/
/**                       E_NOT_OK: The request has not been accepted                                                           **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_Close( TcpIp_SocketIdType TcpSockId_uo,
                                 boolean            Abort_b )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_CLOSE , RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if TcpSockId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_CLOSE, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* Process only when the socket is not CLOSED and not RESERVED */
    /* (UL does not have access to a socket which is CLOSED or RESERVED) */
    /* (see RFC793 p60, 62 : If the user does not have access to such a connection, return "error:  connection illegal for this process") */
    if( (lTcpDynSockTbl_pst->SockState_en != RBA_ETHTCP_SOCK_CLOSED_E) && (lTcpDynSockTbl_pst->SockState_en != RBA_ETHTCP_SOCK_RESERVED_E) )
    {
        /* ------------------------------------------ */
        /* Close related channels for LISTEN socket   */
        /* ------------------------------------------ */
        if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LISTEN_E )
        {
            /* Abort all the channel sockets which are not yet established or which are established but connected or accepted callback has not been given yet to UL*/
            rba_EthTcp_AbortRelatedChannels( TcpSockId_uo );
        }

        /* ------------------------------------------ */
        /* Graceful closing                           */
        /* ------------------------------------------ */
        if( Abort_b == FALSE )
        {
#if( TCPIP_TLS_ENABLED == STD_ON )
            /* If Tls connection Id is valid and it is not listen socket*/
            if((lTcpDynSockTbl_pst->TlsConnectionId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE ) && ( lTcpDynSockTbl_pst->State_en != RBA_ETHTCP_CONN_LISTEN_E ))
            {
                /* Initiate TLS connection close */
                lFunctionRetVal_en = rba_EthTls_CloseConnection(lTcpDynSockTbl_pst->TlsConnectionId_uo);
            }
            else
#endif/*( TCPIP_TLS_ENABLED == STD_ON )*/
            {
                /* Abort argument is set to FALSE so execute the Graceful closing */
                lFunctionRetVal_en = rba_EthTcp_GracefulClose( TcpSockId_uo );
            }
        }

        /* ------------------------------------------ */
        /* Abort closing                              */
        /* ------------------------------------------ */
        else
        {
            /* Abort argument is set to TRUE so execute the Abort closing */
            rba_EthTcp_AbortClose( TcpSockId_uo );

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
    }

    return lFunctionRetVal_en;
}

#if( TCPIP_TLS_ENABLED == STD_ON )
/*********************************************************************************************************************************/
/** rba_EthTcp_SecureClose()  - By this API, the rba_EthTcp module is requested by TLS module to close socket and           **/
/*                                  release all related resources after tls connection is closed                                **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo        - Tcp Socket id                                                                                         **/
/** Abort_b             - This parameter specifies if the connection should be terminated immediately by sending a RST          **/
/**                       segment or be terminated after performing a regular connection termination handshake                  **/
/**                                                                                                                             **/
/** Parameters (inout):   None                                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):     None                                                                                                  **/
/**                                                                                                                             **/
/** Return value:       - Std_ReturnType                                                                                        **/
/**                       E_OK: The request has been accepted                                                                   **/
/**                       E_NOT_OK: The request has not been accepted                                                           **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_SecureClose( TcpIp_SocketIdType TcpSockId_uo,\
                                        boolean            Abort_b )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpSockId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_SECURECLOSE, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* Report DET if tls connection is not valid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (lTcpDynSockTbl_pst->TlsConnectionId_uo == RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE ), \
                                                            RBA_ETHTCP_E_SECURECLOSE , RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* ------------------------------------------ */
    /* Graceful closing                           */
    /* ------------------------------------------ */
    if( Abort_b == FALSE )
    {
        /* Abort argument is set to FALSE so execute the Graceful closing */
        lFunctionRetVal_en = rba_EthTcp_GracefulClose( TcpSockId_uo );
    }

    /* ------------------------------------------ */
    /* Abort closing                              */
    /* ------------------------------------------ */
    else
    {
        /* Abort argument is set to TRUE so execute the Abort closing */
        rba_EthTcp_AbortClose( TcpSockId_uo );

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}
#endif

/*********************************************************************************************************************************/
/** rba_EthTcp_AbortByLocalAddrId() - By this API, Tcp is requested to terminate the connection using local address id          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** LocalAddrId_u8                  - Local address Id                                                                          **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                 None                                                                                      **/
/**                                                                                                                             **/
/** Return value:                     None                                                                                      **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_AbortByLocalAddrId ( TcpIp_LocalAddrIdType LocalAddrId_u8 )
{
    /* Local Variable declaration */
    TcpIp_SocketIdType lSocIdx_uo;

    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_VOID( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_ABORTBYLOCALADDRID, RBA_ETHTCP_E_NOTINIT );

    /* Loop through Tcp socket table and close all the sockets mapped to the requested local address */
    for(lSocIdx_uo = 0; lSocIdx_uo < TCPIP_TCPSOCKETMAX; lSocIdx_uo++)
    {
        /* Process only when local address id match and the socket is not CLOSED */
        if( ((LocalAddrId_u8 == rba_EthTcp_DynSockTbl_ast[lSocIdx_uo].LocalAddrId_u8) || (LocalAddrId_u8 == (uint8)TCPIP_LOCALADDRID_INVALID))  &&
            (rba_EthTcp_DynSockTbl_ast[lSocIdx_uo].SockState_en != RBA_ETHTCP_SOCK_CLOSED_E) )
        {
            /* ------------------------------------------ */
            /* Close related channels for LISTEN socket   */
            /* ------------------------------------------ */
            if( rba_EthTcp_DynSockTbl_ast[lSocIdx_uo].State_en == RBA_ETHTCP_CONN_LISTEN_E )
            {
                /* Abort all the channel sockets which are not yet established or which are established but connected or accepted callback has not been given yet to UL*/
                rba_EthTcp_AbortRelatedChannels( lSocIdx_uo );
            }

            /* ------------------------------------------ */
            /* Abort closing                              */
            /* ------------------------------------------ */
            /* Check If tcp socket is not closed previously*/
            if( rba_EthTcp_DynSockTbl_ast[lSocIdx_uo].State_en != RBA_ETHTCP_CONN_CLOSED_E )
            {
                /* Execute Abort closing of the socket */
                rba_EthTcp_AbortClose( lSocIdx_uo );
            }

        }
    }

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************************/
/** rba_EthTcp_GracefulClose()    - This function execute the Graceful closing as described in RFC793 p60, 61                   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo           - Tcp dynamic socket table index                                                              **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                 - Std_ReturnType                                                                              **/
/**                                 E_OK: The Graceful closing has been executed                                                **/
/**                                 E_NOT_OK: The Graceful closing has not been executed                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_GracefulClose( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;
    Std_ReturnType                  lRetVal_en;
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    uint16                          lRemainDataLen_u16;
    uint16                          lUnackedDataLen_u16;
#endif

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    switch ( lTcpDynSockTbl_pst->State_en )
    {
        /* --------------------------------------------------- */
        /* Graceful close for CLOSED, LISTEN or SYN_SENT state */
        /* --------------------------------------------------- */
        case RBA_ETHTCP_CONN_CLOSED_E:
        case RBA_ETHTCP_CONN_LISTEN_E:
        case RBA_ETHTCP_CONN_SYN_SENT_E:
        {
            /* Reset the socket */
            rba_EthTcp_ResetSock( TcpDynSockTblIdx_uo, TCPIP_TCP_CLOSED );

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------------ */
        /* Graceful close in SYN_RCVD state           */
        /* ------------------------------------------ */
        case RBA_ETHTCP_CONN_SYN_RCVD_E:
        {
            /* Postponed our FIN ACK */
            /* (The FIN ACK will be sent after the ESTABLISHED state will be reached) */
            lTcpDynSockTbl_pst->PostponeFin_b = TRUE;

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------------------- */
        /* Graceful close in ESTABLISHED or CLOSE_WAIT state */
        /* ------------------------------------------------- */
        case RBA_ETHTCP_CONN_ESTABLISHED_E:
        case RBA_ETHTCP_CONN_CLOSE_WAIT_E:
        {
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
            /* Get how much data in Tcp Tx buffer is ready to be sent */
            lRemainDataLen_u16 = rba_EthTcp_GetTxBufSndLen( TcpDynSockTblIdx_uo );

            /* Get the length of unacked data in the Tcp Tx buffer */
            lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );

            /* Check if the FIN ACK can be sent in the next MainFunction or need to be postponed after data */
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
            if ( (lRemainDataLen_u16 > 0U) || (lUnackedDataLen_u16 > 0U) )
#else
            if ( (lRemainDataLen_u16 > 0U) )
#endif
            {
                /* Postponed our FIN ACK after data */
                /* (we have not finished to send our data or some of them are not acknowledged yet) */
                lTcpDynSockTbl_pst->PostponeFin_b = TRUE;
            }
            else
#endif
            {
                /* Send a FIN ACK in the next MainFunction */
                lRetVal_en = rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_FIN_ACK_E );

                /* If a frame with higher priority needs to be sent before */
                if( lRetVal_en == E_NOT_OK )
                {
                    /* Postpone our FIN ACK */
                    lTcpDynSockTbl_pst->PostponeFin_b = TRUE;
                }
            }

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------------ */
        /* Graceful close in other state              */
        /* ------------------------------------------ */
        default:
        {
            /* Tcp state doesn't allow a graceful closing so return E_NOT_OK */
            /* (see RFC793 p60,61 : FIN-WAIT-1, FIN-WAIT-2, CLOSING, LAST-ACK, TIME-WAIT STATE, respond with "error:  connection closing") */
        }
        break;
    }

    return lFunctionRetVal_en;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_AbortClose()       - This function execute the Abort closing as described in RFC793 p62                          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo           - Tcp dynamic socket table index                                                              **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                   None                                                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_AbortClose( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    switch ( lTcpDynSockTbl_pst->State_en )
    {
        /* --------------------------------------------------------------------------------- */
        /* Abort close for SYN_RCVD, ESTABLISHED, FIN_WAIT_1, FIN_WAIT_2 or CLOSE_WAIT state */
        /* --------------------------------------------------------------------------------- */
        case RBA_ETHTCP_CONN_SYN_RCVD_E:
        case RBA_ETHTCP_CONN_ESTABLISHED_E:
        case RBA_ETHTCP_CONN_FIN_WAIT_1_E:
        case RBA_ETHTCP_CONN_FIN_WAIT_2_E:
        case RBA_ETHTCP_CONN_CLOSE_WAIT_E:
        {
            /* We are in a state where a reset need to be sent to abort the connection */

            /* Send a RST ACK */
            /* (We have to send RST ACK and not RST in order to be sure that the remote will be able to process the reset if its state is SYN_SENT state, see RFC793 p36, 37) */
            (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_RST_ACK_E );

            /* During an abort closing, we have to send directly the RST-ACK frame (we do not wait for the next MainFunction) */
            /* (For example, if Tcp_Close is called when the network goes offline (ETHSM_STATE_OFFLINE), the RST have to be sent immediately) */
            rba_EthTcp_SndFlag( TcpDynSockTblIdx_uo );

            /* If reset frame is not succesfully sent, forcefully close the socket */
            if( (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_RST_E)       ||
                (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_RST_ACK_E) )
            {
                /* Reset the socket */
                rba_EthTcp_ResetSock( TcpDynSockTblIdx_uo, TCPIP_TCP_RESET );
            }
        }
        break;

        /* ------------------------------------------ */
        /* Abort close for other states               */
        /* ------------------------------------------ */
        default:
        {
            /* Reset the socket */
            rba_EthTcp_ResetSock( TcpDynSockTblIdx_uo, TCPIP_TCP_RESET );
        }
        break;
    }

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_AbortRelatedChannels()   - This function abort all the channel sockets which are currently in opening            **/
/**                                       phase (ESTABLISHED state has not been reached yet or accepted or connected callback   **/
/**                                       has not been given to UL yet)                                                         **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                 - Tcp dynamic socket table index of the listening socket whose channels need to         **/
/**                                       be aborted                                                                            **/
/**                                                                                                                             **/
/** Parameters (inout):                   None                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):                     None                                                                                  **/
/**                                                                                                                             **/
/** Return value:                         None                                                                                  **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_AbortRelatedChannels( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    TcpIp_SocketIdType lIdxTcpSock_uo;

    /* -------------------------------------------------------- */
    /* Abort channels in LISTEN or SYN_RCVD or ESTABLISHED state*/
    /* -------------------------------------------------------- */

    /* Reset all the channel sockets in LISTEN or SYN_RCVD state which are related to the listening socket */
    for( lIdxTcpSock_uo=0U; lIdxTcpSock_uo < TCPIP_TCPSOCKETMAX; lIdxTcpSock_uo++ )
    {
        /* If the current Tcp socket is a channel of the listening Tcp socket and its state is LISTEN or SYN_RCVD or established */
        if( (rba_EthTcp_DynSockTbl_ast[lIdxTcpSock_uo].ListenTcpSockIdx_uo == TcpDynSockTblIdx_uo)  &&
            ((rba_EthTcp_DynSockTbl_ast[lIdxTcpSock_uo].State_en == RBA_ETHTCP_CONN_LISTEN_E)       ||
            (rba_EthTcp_DynSockTbl_ast[lIdxTcpSock_uo].State_en == RBA_ETHTCP_CONN_SYN_RCVD_E)      ||
            (rba_EthTcp_DynSockTbl_ast[lIdxTcpSock_uo].State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)) )
        {
            /* Execute an Abort close of the current channel */
            rba_EthTcp_AbortClose( lIdxTcpSock_uo );
        }
    }

    return;
}



#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
