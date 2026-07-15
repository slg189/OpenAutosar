

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "TcpIp_Prv.h"

#include "SoAd.h"
#include "SoAd_Cbk.h" /* Required for SoAd call-back functions */

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"
#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTcp_Cbk.h"
#include "rba_EthTls_Cbk.h"
#include "rba_EthTls.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */

#if (!defined(SOAD_AR_RELEASE_MAJOR_VERSION) || (SOAD_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - TcpIp and SoAd"
#endif

#if (!defined(SOAD_AR_RELEASE_MINOR_VERSION) || (SOAD_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - TcpIp and SoAd"
#endif


/*
 ***************************************************************************************************
 * Local functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_RxData()                - Execute the processing of newly received data                                          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                - Tcp dynamic socket table index                                                         **/
/** DataPtr_pu8                        - Pointer to the received data                                                           **/
/** DataLen_u16                        - Length of received data                                                                **/
/**                                                                                                                             **/
/** Parameters (inout):                  None                                                                                   **/
/**                                                                                                                             **/
/** Parameters (out):                    None                                                                                   **/
/**                                                                                                                             **/
/** Return value:                        None                                                                                   **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_RxData( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                        uint8 *            DataPtr_pu8,
                        uint16             DataLen_u16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    TcpIp_SockAddrType *            lRemoteAddr_pst;

    Std_ReturnType                  lRetVal_en;

#if (TCPIP_IPV4_ENABLED == STD_ON)
    TcpIp_SockAddrInetType          lRemoteAddrInet_st;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    uint8 lLoopIdx_u8;
    TcpIp_SockAddrInet6Type         lRemoteAddrInet6_st;
#endif

    /* Init local variables */
    lRetVal_en = E_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* ------------------------------------- */
    /* Update Snd Ack number                 */
    /* ------------------------------------- */

    /* Update send acknowledgment number of the Tcp socket */
    rba_EthTcp_SetAckNum( TcpDynSockTblIdx_uo, (lTcpDynSockTbl_pst->SndAckNum_u32 + DataLen_u16) );

    /* Update the total received data length (since last sending of ACK) */
    lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 += DataLen_u16;

    /* ------------------------------------- */
    /* Decrement local window size           */
    /* ------------------------------------- */

    /* Decrement window by the length in argument */
    /* (as per TcpIp Autosar) */
    lTcpDynSockTbl_pst->SndWndSize_u16 -= DataLen_u16;

    /* ------------------------------------- */
    /* If Window is closed                   */
    /* ------------------------------------- */

    /* If the local window is now equal to 0, means the window is now closed */
    if( lTcpDynSockTbl_pst->SndWndSize_u16 == 0U )
    {
        /* Send ACK frame in the next MainFunction */
        /* (this ACK is useful to inform the remote that our window is now closed and the remote cannot send data anymore - see RFC1122 p92) */
        (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_ACK_E );
    }

    /* ------------------------------------- */
    /* If Window is still opened             */
    /* ------------------------------------- */
    else
    {
#if ( RBA_ETHTCP_DLYACK_ENABLED == STD_ON )
        /* ------------------------------------- */
        /* Delayed Ack enabled                   */
        /* ------------------------------------- */

        /* If Total Received Data >= 2*MSS (see RFC 2581 p8) */
#if ( RBA_ETHTCP_MSS_ENABLED == STD_ON )
        if( lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 >= (uint16)(lTcpDynSockTbl_pst->LocalMss_u16 << 1U) )  /* Compare to our MSS in case we have sent MSS during connection establishment (MSS feature enabled) */
#else
        if( lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 >= (uint16)(RBA_ETHTCP_DFL_MSS << 1U) )                /* Compare to default MSS in case we have not sent MSS during connection establishment (MSS feature disabled) */
#endif
        {
            /* Send an ACK in the next MainFunction */
            (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_ACK_E );
        }

        /* If Total Received Data < 2*MSS */
        else
        {
            /* Start the Delayed ack timer if it is not already running */
            rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_DLYACK_E );
        }
#else
        /* ------------------------------------- */
        /* Delayed Ack disabled                  */
        /* ------------------------------------- */

        /* Send an ACK in the next MainFunction */
        (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_ACK_E );
#endif
    }

    /* ----------------------------------- */
    /* Create remote address structure     */
    /* ----------------------------------- */
    lRemoteAddr_pst = NULL_PTR; /* Set to null pointer to ensure that no garbage values are forwarded to upper layer in case no IP protocol is enabled. */

    switch(lTcpDynSockTbl_pst->DomainType_u32)
    {
#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_AF_INET:
        {
            lRemoteAddrInet_st.domain   = TCPIP_AF_INET;
            lRemoteAddrInet_st.port     = lTcpDynSockTbl_pst->RemotePort_u16;
            lRemoteAddrInet_st.addr[0]  = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv4Addr_u32;
            /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling function. */
            lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet_st );
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_AF_INET6:
        {
            lRemoteAddrInet6_st.domain  = TCPIP_AF_INET6;
            lRemoteAddrInet6_st.port    = lTcpDynSockTbl_pst->RemotePort_u16;
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < 4u; lLoopIdx_u8++ )
            {
                lRemoteAddrInet6_st.addr[lLoopIdx_u8] = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv6Addr_au32[lLoopIdx_u8];
            }
            /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling function. */
            lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet6_st );
        }
        break;
#endif

        default:
        {
            /* If execution reaches here, then the IP support for the specified domain is not enabled; set local return value to E_NOT_OK. */
            lRetVal_en = E_NOT_OK;
        }
        break;
    }
    /* Condition check: Upper layer should provide a valid function for RxIndication */
    if( lRetVal_en == E_OK )
    {
#if( TCPIP_TLS_ENABLED == STD_ON )

        /* Check if connection is secure and Tls connection is already established  */
        if( lTcpDynSockTbl_pst-> TlsConnectionId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE )
        {
                /****************************************************************************************************************/
                /****************************************** [SWS_TCPIP_00328] ***************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** For static TLS connection the TCP server shall expect a TLS handshake after the ACK for the SYN has been   **/
                /** received. All incoming messages for this socket shall further be passed on to TLS.                         **/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

            /* Call TLS RxIndication with valid pointer */
            rba_EthTls_RxIndication( lTcpDynSockTbl_pst-> TlsConnectionId_uo, DataPtr_pu8, DataLen_u16);

        }
        else
        {
#if (RBA_ETHTCP_CFG_TLS_DYNAMIC_CONN == STD_ON)
            /* check if recieved data contains TLS handshake information */
            lRetVal_en = rba_EthTls_CheckIfTlsHandshakeData( DataPtr_pu8, DataLen_u16 );

            /* check if data corresponds to tls handshake data */
            if( lRetVal_en == E_OK )
            {
                /*Initiate TLS open connection */
                lRetVal_en = rba_EthTcp_OpenDynamicSecureConnection(TcpDynSockTblIdx_uo );

                /* check if open TLS connecteion failed*/
                if(lRetVal_en == E_NOT_OK)
                {
                    /* Send a RST-ACK in the next MainFunction */
                    (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_RST_ACK_E );
                }
                else
                {
                    /* Call TLS RxIndication with valid pointer */
                    rba_EthTls_RxIndication( lTcpDynSockTbl_pst-> TlsConnectionId_uo, DataPtr_pu8, DataLen_u16);
                }

            }
            else
#endif /* RBA_ETHTCP_CFG_TLS_DYNAMIC_CONN is STD_ON */
            {
                /* ----------------------------------- */
                /* Call UL RxIndication                */
                /* ----------------------------------- */
                /* Notify upper layer that new data have been received and are available for processing */
                /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type */
                TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_RxIndication_pfct(  TcpDynSockTblIdx_uo,
                                                                                                                           lRemoteAddr_pst,
                                                                                                                           DataPtr_pu8,
                                                                                                                           DataLen_u16 );
            }
        }
#else
        /* ----------------------------------- */
        /* Call UL RxIndication                */
        /* ----------------------------------- */
        /* Notify upper layer that new data have been received and are available for processing */
        /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type */
        TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_RxIndication_pfct(  TcpDynSockTblIdx_uo,
                                                                                                                   lRemoteAddr_pst,
                                                                                                                   DataPtr_pu8,
                                                                                                                   DataLen_u16 );
#endif/* TCPIP_TLS_ENABLED == STD_ON */

     }

}

#if( TCPIP_TLS_ENABLED == STD_ON )
/*********************************************************************************************************************************/
/** rba_EthTcp_TlsRxIndication()        - Execute the providing data to UL from Tls                                             **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                - Tcp dynamic socket table index                                                         **/
/** DecryptedData_pu8                   - Pointer to the received data                                                           **/
/** DecryptedDataLen_u16                - Length of received data                                                                **/
/**                                                                                                                             **/
/** Parameters (inout):                  None                                                                                   **/
/**                                                                                                                             **/
/** Parameters (out):                    None                                                                                   **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from Tcp  to UL                                  **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_TlsRxIndication( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                 uint8 *            DecryptedData_pu8,
                                 uint16             DecryptedDataLen_u16 )
{
    rba_EthTcp_DynSockTblType_tst *   lTcpDynSockTbl_pst;
    TcpIp_SockAddrType *              lRemoteAddr_pst;

#if (TCPIP_IPV4_ENABLED == STD_ON)
    TcpIp_SockAddrInetType            lRemoteAddrInet_st;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    uint8 lLoopIdx_u8;
    TcpIp_SockAddrInet6Type           lRemoteAddrInet6_st;
#endif
    Std_ReturnType                    lRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lRetVal_en         = E_OK;

    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_VOID( (TcpDynSockTblIdx_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_TLSRXINDICATION, RBA_ETHTCP_E_INV_ARG );

    /* Report DET if DecryptedData_pu8 is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( DecryptedData_pu8 == NULL_PTR ), RBA_ETHTCP_E_TLSRXINDICATION, RBA_ETHTCP_E_NULL_PTR );

    /* Report DET if DecryptedDataLen_u16 is zero */
    RBA_ETHTCP_CHECK_DETERROR_VOID( ( DecryptedDataLen_u16 == 0U ), RBA_ETHTCP_E_TLSRXINDICATION, RBA_ETHTCP_E_INV_ARG );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* ----------------------------------- */
    /* Create remote address structure     */
    /* ----------------------------------- */
    lRemoteAddr_pst = NULL_PTR; /* Set to null pointer to ensure that no garbage values are forwarded to upper layer in case no IP protocol is enabled. */

    switch(lTcpDynSockTbl_pst->DomainType_u32)
    {
#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_AF_INET:
        {
            lRemoteAddrInet_st.domain   = TCPIP_AF_INET;
            lRemoteAddrInet_st.port     = lTcpDynSockTbl_pst->RemotePort_u16;
            lRemoteAddrInet_st.addr[0]  = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv4Addr_u32;
            /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling function. */
            lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet_st );
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_AF_INET6:
        {
            lRemoteAddrInet6_st.domain  = TCPIP_AF_INET6;
            lRemoteAddrInet6_st.port    = lTcpDynSockTbl_pst->RemotePort_u16;
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < 4u; lLoopIdx_u8++ )
            {
                lRemoteAddrInet6_st.addr[lLoopIdx_u8] = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv6Addr_au32[lLoopIdx_u8];
            }
            /* MR12 RULE 11.3 VIOLATION: Cast is required to respect the type definition of the calling function. */
            lRemoteAddr_pst             = ( (TcpIp_SockAddrType*) &lRemoteAddrInet6_st );
        }
        break;
#endif

        default:
        {
            /* If execution reaches here, then the IP support for the specified domain is not enabled; set local return value to E_NOT_OK. */
            lRetVal_en = E_NOT_OK;
        }
        break;
    }

    if(lRetVal_en == E_OK)
    {
        /* Notify upper layer that new data have been received and are available for processing */
        /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type */
        TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_RxIndication_pfct(TcpDynSockTblIdx_uo,
                                                                                                                 lRemoteAddr_pst,
                                                                                                                 DecryptedData_pu8,
                                                                                                                 DecryptedDataLen_u16 );
    }

}

#endif/* ( TCPIP_TLS_ENABLED == STD_ON ) */

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
