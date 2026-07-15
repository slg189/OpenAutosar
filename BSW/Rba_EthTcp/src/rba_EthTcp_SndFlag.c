

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "SoAd.h"
#include "SoAd_Cbk.h"       /* Required for SoAd call-back functions */

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif/*  TCPIP_TLS_ENABLED == STD_ON  */

#if (!defined(SOAD_AR_RELEASE_MAJOR_VERSION) || (SOAD_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - TcpIp and SoAd"
#endif

#if (!defined(SOAD_AR_RELEASE_MINOR_VERSION) || (SOAD_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - TcpIp and SoAd"
#endif


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"
static Std_ReturnType rba_EthTcp_SndFlagSegment ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void rba_EthTcp_UpdateSocketAfterSndSyn  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void rba_EthTcp_UpdateSocketAfterSndAck  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void rba_EthTcp_UpdateSocketAfterSndFin  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );

static void rba_EthTcp_UpdateSocketAfterSndRst  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


/*
 ***************************************************************************************************
 * Local functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_SndFlag()    - Send a Tcp frame without payload. Only the Tcp header will be contained in the frame              **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo     - Tcp dynamic socket table index                                                                    **/
/**                                                                                                                             **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):         None                                                                                              **/
/**                                                                                                                             **/
/** Return value:             None                                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_SndFlag(   TcpIp_SocketIdType TcpDynSockTblIdx_uo )

{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lRetVal_en;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* ---------------------------------------------- */
    /* Decrement sequence number (RETX or KEEP_ALIVE) */
    /* ---------------------------------------------- */
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
    /* If a retransmission of Flag or the sending of a Keep Alive need to be done */
    if( (lTcpDynSockTbl_pst->RetransmissionFlg_b != FALSE)                   ||
        (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_KEEP_ALIVE_E) )
#else
    /* If the sending of a Keep Alive need to be done */
    if( lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_KEEP_ALIVE_E )
#endif
    {
        /* For flag retransmission, we should decrement the send sequence number */
        rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                                (lTcpDynSockTbl_pst->SndSeqNum_u32 - 1U) );
    }

    /* -------------------------------------- */
    /* Send flag segment                      */
    /* -------------------------------------- */

    /* Execute Tcp flag sending */
    lRetVal_en = rba_EthTcp_SndFlagSegment( TcpDynSockTblIdx_uo );

    /* -------------------------------------------------------- */
    /* Increment back back sequence number (RETX or KEEP_ALIVE) */
    /* -------------------------------------------------------- */
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
    /* If a retransmission of Flag or the sending of a Keep Alive was needed */
    if( (lTcpDynSockTbl_pst->RetransmissionFlg_b != FALSE)                   ||
        (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_KEEP_ALIVE_E) )
#else
    /* If the sending of a Keep Alive was needed */
    if( lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_KEEP_ALIVE_E )
#endif
    {
        /* If the flag was not sent */
        if( lRetVal_en == E_NOT_OK )
        {
            /* Set back the next sequence number to be sent */
            rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                                    (lTcpDynSockTbl_pst->SndSeqNum_u32 + 1U) );
        }
    }

    /* -------------------------------------- */
    /* Transmission succeded                  */
    /* -------------------------------------- */

    /* If the segment has been successfully sent */
    if( lRetVal_en == E_OK )
    {
        /* -------------------------------------- */
        /* Update Tcp socket                      */
        /* -------------------------------------- */

        /* Execute action depending of the sent flag */
        switch ( lTcpDynSockTbl_pst->TcpFlag_en )
        {
            case RBA_ETHTCP_SEND_SYN_E:
            case RBA_ETHTCP_SEND_SYN_ACK_E:
            {
                /* Execute the post processing needed after the sending of a SYN based frame */
                rba_EthTcp_UpdateSocketAfterSndSyn( TcpDynSockTblIdx_uo );
            }
            break;

            case RBA_ETHTCP_SEND_ACK_FOR_SYN_ACK_E:
            case RBA_ETHTCP_SEND_ACK_E:
            case RBA_ETHTCP_SEND_KEEP_ALIVE_E:
            case RBA_ETHTCP_SEND_ACK_FOR_FIN_ACK_E:
            {
                /* Execute the post processing needed after the sending of a ACK based frame */
                rba_EthTcp_UpdateSocketAfterSndAck( TcpDynSockTblIdx_uo );
            }
            break;

            case RBA_ETHTCP_SEND_FIN_ACK_E:
            {
                /* Execute the post processing needed after the sending of a FIN based frame */
                rba_EthTcp_UpdateSocketAfterSndFin( TcpDynSockTblIdx_uo );
            }
            break;

            case RBA_ETHTCP_SEND_RST_E:
            case RBA_ETHTCP_SEND_RST_ACK_E:
            default:
            {
                /* Execute the post processing needed after the sending of a RST based frame */
                rba_EthTcp_UpdateSocketAfterSndRst( TcpDynSockTblIdx_uo );
            }
            break;
        }

        /* -------------------------------------- */
        /* Update next frame to be sent           */
        /* -------------------------------------- */

        /* Reset the Tcp flag to NOTHING */
        (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_NOTHING_E );
    }
    else if( (RBA_ETHTCP_SEND_RST_E == lTcpDynSockTbl_pst->TcpFlag_en) || (RBA_ETHTCP_SEND_RST_ACK_E == lTcpDynSockTbl_pst->TcpFlag_en))
    {
        /* Execute the post processing needed after the sending of a RST based frame */
        rba_EthTcp_UpdateSocketAfterSndRst( TcpDynSockTblIdx_uo );
    }
    else
    {
        /* do nothing*/
    }

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************************/
/** rba_EthTcp_SndFlagSegment() - Execute the sending of the Tcp flag segment                                                   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo         - Tcp dynamic socket table index                                                                **/
/**                                                                                                                             **/
/** Parameters (inout):           None                                                                                          **/
/**                                                                                                                             **/
/** Parameters (out):             None                                                                                          **/
/**                                                                                                                             **/
/** Return value:               - Std_ReturnType                                                                                **/
/**                               E_OK: The segment has been successfully transmitted                                           **/
/**                               E_NOT_OK: The segment has not been sent                                                       **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_SndFlagSegment ( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    BufReq_ReturnType               lRetBufReq_en;
    TcpIp_ReturnType                lIPv4TxRetType_en;
    uint8 *                         lIpProvidedBuf_pu8;
    TcpIp_IpHeader_tst              lIpHeader_st;
    uint16                          lTcpOptionLen_u16;
    uint16                          lTcpHdrLen_u16;
    uint8                           lIpProvidedBufIdx_u8;
    Std_ReturnType                  lFunctionRetVal_en;
    Std_ReturnType                  lRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Initialize the return value of called functions to E_OK */
    lRetVal_en = E_OK;

    /* Initialize the return value for the buffer request to E_NOT_OK */
    lRetBufReq_en = BUFREQ_E_NOT_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* -------------------------------------- */
    /* Configure the IP header                */
    /* -------------------------------------- */

    /* Set the ProtoType and the TTL of the IP header */
    lIpHeader_st.ProtoType_en = TCPIP_IPPROTO_TCP;
    lIpHeader_st.Ttl_u8       = rba_EthTcp_CurrConfig_cpo->TcpTtl_u8; /* Use Configured TTL */
    lIpHeader_st.SockOwner_u8 = lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8;/* To avoid unintentional setting of DHCP as upper layer,
                                                                        * socket owner is initialised in TCP instead of sending a garbage value. */
    RBA_ETHTCP_CALL_IP_LAYER_UPDATE_FLOW_LABEL(&lIpHeader_st, lTcpDynSockTbl_pst);

    RBA_ETHTCP_CALL_IP_LAYER_UPDATE_DSCP(&lIpHeader_st, lTcpDynSockTbl_pst);

    /* -------------------------------------- */
    /* Compute the Tcp header length          */
    /* -------------------------------------- */

    /* Get the length of the Tcp options */
    lTcpOptionLen_u16 = rba_EthTcp_GetOptionLength( TcpDynSockTblIdx_uo );

    /* Compute the length of the Tcp header */
    lTcpHdrLen_u16 = RBA_ETHTCP_DFL_HDRLENGTH + lTcpOptionLen_u16;

    /* -------------------------------------- */
    /* Request a buffer for the transmission  */
    /* -------------------------------------- */

    /* Ask the required buffer to the IP layer */
    lRetBufReq_en = RBA_ETHTCP_CALL_IP_LAYER_PROVIDE_TX_BUFFER_API( lTcpDynSockTbl_pst, &lIpProvidedBufIdx_u8, &lIpProvidedBuf_pu8, &lTcpHdrLen_u16 );

    /* Check if IP successfully provided the requested buffer */
    if ( lRetBufReq_en == BUFREQ_OK )
    {
        /* -------------------------------------- */
        /* Copy the Tcp frame into the buffer     */
        /* -------------------------------------- */

        /* Copy the Tcp header in the provided buffer */
        rba_EthTcp_SetHdrToFrame(   TcpDynSockTblIdx_uo,
                                    lIpProvidedBuf_pu8 );

        /* Copy the Tcp options in the provided buffer */
        rba_EthTcp_SetOptionToFrame(    TcpDynSockTblIdx_uo,
                                        lIpProvidedBuf_pu8 );

#if ( RBA_ETHTCP_SW_CHKSUM_TX == STD_ON )
        /* If Checksum offloading is disabled in the hw for the given local address, then checksum calculation
           needs to be done in sw */
        if(FALSE == rba_EthTcp_ChecksumOffloading_cab[lTcpDynSockTbl_pst->LocalAddrId_u8])
        {
            /* Update the Checksum field in the Tcp header (in EthDrv buffer) */
            lRetVal_en = rba_EthTcp_SetChecksumToFrame ( TcpDynSockTblIdx_uo,
                                                         lIpProvidedBuf_pu8,
                                                         lTcpHdrLen_u16 );
        }
#endif /* RBA_ETHTCP_SW_CHKSUM_TX == STD_ON */

        /* If checksum was successful, transmit the frame */
        if(lRetVal_en == E_OK)
        {
            /* -------------------------------------- */
            /* Transmission                           */
            /* -------------------------------------- */

            /* Ask the transmission of the frame */
            /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
            lIPv4TxRetType_en = RBA_ETHTCP_CALL_IP_LAYER_TRANSMIT_API( lTcpDynSockTbl_pst, lIpProvidedBuf_pu8, lIpProvidedBufIdx_u8, &lIpHeader_st, lTcpHdrLen_u16 );

            /* If the transmission of the frame is a success and all called functions have returned successfully */
            if( lIPv4TxRetType_en == TCPIP_OK )
            {
                /* Set the return value of the function to E_OK */
                lFunctionRetVal_en = E_OK;
            }
        }

        /* An error occured during copying of Tcp header fields */
        else
        {
            /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
            (void) RBA_ETHTCP_CALL_IP_LAYER_TRANSMIT_API( lTcpDynSockTbl_pst, lIpProvidedBuf_pu8, lIpProvidedBufIdx_u8, &lIpHeader_st, 0U );
        }
    }

    return lFunctionRetVal_en;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateSocketAfterSndSyn()    - Execute the post processing after SYN based frame is sent                         **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/**                                                                                                                             **/
/** Parameters (inout):                       None                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                         None                                                                              **/
/**                                                                                                                             **/
/** Return value:                             None                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_UpdateSocketAfterSndSyn ( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    switch ( lTcpDynSockTbl_pst->TcpFlag_en )
    {
        /* -------------------------------------- */
        /* Actions after SEND SYN                 */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_SYN_E:
        {
            /* Increment the send sequence number */
            /* (SYN bit takes one byte in the sequence number space) */
            rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                                    (lTcpDynSockTbl_pst->SndSeqNum_u32 + 1U) );

            /* -------------------------------------- */
            /* Actual state is CLOSED or LISTEN       */
            /* -------------------------------------- */
            if( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSED_E)      ||
                (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LISTEN_E) )
            {
                /* Set the unacked sequence number to ISS */
                lTcpDynSockTbl_pst->UnAckedSeqNum_u32 = lTcpDynSockTbl_pst->IniSndSeqNum_u32;

                /* Change the connection state of the socket to SYN_SENT */
                rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_SYN_SENT_E );

                /* Dynamic RETX management */
#if (RBA_ETHTCP_DYNAMIC_RETX_TIMER == STD_ON)
                /* We just sent a SYN so start the RTT measurement */
                lTcpDynSockTbl_pst->RttInProgress_b = TRUE;
#endif
            }

            /* -------------------------------------- */
            /* Actual state is SYN_SENT               */
            /* (case of SYN retransmission)           */
            /* -------------------------------------- */
            else
            {
                /* We do not execute the measurement of the RTT here */
                /* (Karn's algorithm :  RTT shall not be measured in case of retransmission) */
            }
        }
        break;

        /* -------------------------------------- */
        /* Actions after SEND SYN ACK             */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_SYN_ACK_E:
        default:
        {
            /* Set the total received data length since the last sending of ACK to 0 (the sent data frame contained the acknowledgement number) */
            ( lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 ) = 0U;

            /* Increment the send sequence number */
            /* (SYN bit takes one byte in the sequence number space) */
            rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                                    (lTcpDynSockTbl_pst->SndSeqNum_u32 + 1U) );

            /* ------------------------------------------------------- */
            /* Actual state is LISTEN, SYN SENT, TIME WAIT or LAST ACK */
            /* ------------------------------------------------------- */
            if( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LISTEN_E)      ||
                (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_SENT_E)    ||
                (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_TIME_WAIT_E)   ||
                (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LAST_ACK_E) )
            {
                /* Set the unacked sequence number to ISS */
                lTcpDynSockTbl_pst->UnAckedSeqNum_u32 = lTcpDynSockTbl_pst->IniSndSeqNum_u32;

                /* Change the connection state of the socket to SYN_RCVD */
                rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_SYN_RCVD_E );

                /* Dynamic RETX management */
#if (RBA_ETHTCP_DYNAMIC_RETX_TIMER == STD_ON)
                /* We just sent a SYN ACK so start the RTT measurement */
                lTcpDynSockTbl_pst->RttInProgress_b = TRUE;
#endif
            }

            /* -------------------------------------- */
            /* Actual state is SYN_RCVD               */
            /* (case of SYN-ACK retransmission)       */
            /* -------------------------------------- */
            else
            {
                /* We do not execute the measurement of the RTT here */
                /* (Karn's algorithm :  RTT shall not be measured in case of retransmission) */
            }
        }
        break;
    }

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateSocketAfterSndAck()    - Execute the post processing after ACK based frame is sent                         **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/**                                                                                                                             **/
/** Parameters (inout):                       None                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                         None                                                                              **/
/**                                                                                                                             **/
/** Return value:                             None                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_UpdateSocketAfterSndAck ( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

#if( TCPIP_TLS_ENABLED == STD_ON )

    /* Variable to hold the return value for internal function */
    Std_ReturnType                  lRetVal_en;

	/* Initialize the return value of called functions to E_OK */
	lRetVal_en = E_OK;

#endif/* ( TCPIP_TLS_ENABLED == STD_ON ) */

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    switch ( lTcpDynSockTbl_pst->TcpFlag_en )
    {
        /* -------------------------------------- */
        /* Actions after SEND ACK FOR SYN ACK     */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_ACK_FOR_SYN_ACK_E:
        {
            /* Set the total received data length since the last sending of ACK to 0 (the sent data frame contained the acknowledgement number) */
            lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 = 0U;

            /* -------------------------------------- */
            /* Actual state is SYN_SENT               */
            /* -------------------------------------- */
            if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_SENT_E )
            {
#if( TCPIP_TLS_ENABLED == STD_ON )

                /* Call the UL callback only if it is not a secure TLS connection.Check if UL has not request for opening of
                 * secure connection and also the Ip and port are not secure ip and secure port. */
                if( lTcpDynSockTbl_pst-> TlsConnectionId_uo == RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE )
#endif/* ( TCPIP_TLS_ENABLED == STD_ON ) */
                {
                    /* Call UL API to indicate that the connection is now established */
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TcpConnected_pfct( TcpDynSockTblIdx_uo );

                }
#if( STD_ON == TCPIP_TLS_ENABLED)
                else
                {
	                /* check if connection refers to secure tls connection */
					lRetVal_en = rba_EthTcp_CheckSecureClientConnection( lTcpDynSockTbl_pst, &lTcpDynSockTbl_pst->RemoteIPAddr_un, lTcpDynSockTbl_pst-> TlsConnectionId_uo );

		            /* If Tls conn id requested by UL matches with Tls connection id returned by Tls module */
		            if(lRetVal_en == E_OK)
                    {
		                /* set flag to true  to indicate it is static secure connection*/
                        lRetVal_en = rba_EthTcp_OpenSecureConnection(TcpDynSockTblIdx_uo);

                        /* If Tls open connection request was  successful*/
                        if( lRetVal_en == E_OK )
                        {
                            /* set flag to true  if TLS connection Id is valid */
                            lTcpDynSockTbl_pst->TlsStaticSecureConnection_b = TRUE;

                        }
                    }

                }/*End of Call the UL callback only if it is not a secure TLS connection */

                /* If Tls open connection request was not successful*/
                if( lRetVal_en == E_NOT_OK )
                {
                    /* Send a RST-ACK in the next MainFunction */
                    (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_RST_ACK_E );
                }
                else
#endif/* ( TCPIP_TLS_ENABLED == STD_ON ) */
                {
                    /* Change the connection state of the socket to ESTABLISHED */
                    rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_ESTABLISHED_E );

                }
            }
        }
        break;

        /* -------------------------------------- */
        /* Actions after SEND ACK                 */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_ACK_E:
        {
#if ( RBA_ETHTCP_DLYACK_ENABLED == STD_ON )
            /* Stop the Delayed ack timer if it is running */
            /* (as the acknowledgement number was sent, we can stop the DelayedAck timer) */
            rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_DLYACK_E );
#endif

            /* Set the total received data length since the last sending of ACK to 0 (the sent data frame contained the acknowledgement number) */
            lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 = 0U;
        }
        break;

        /* -------------------------------------- */
        /* Actions after SEND_KEEP_ALIVE          */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_KEEP_ALIVE_E:
        {
#if ( RBA_ETHTCP_KEEP_ALIVE_ENABLED == STD_ON )
            /* We need to increment back the send sequence number */
            rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                                    (lTcpDynSockTbl_pst->SndSeqNum_u32 + 1U) );
#endif
        }
        break;

        /* -------------------------------------- */
        /* Actions after SEND ACK FOR FIN ACK     */
        /* -------------------------------------- */
        case RBA_ETHTCP_SEND_ACK_FOR_FIN_ACK_E:
        default:
        {
#if ( RBA_ETHTCP_DLYACK_ENABLED == STD_ON )
            /* Stop the Delayed ack timer if it is running */
            /* (as the acknowledgement number was sent, we can stop the DelayedAck timer) */
            rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_DLYACK_E );
#endif

            /* Set the total received data length since the last sending of ACK to 0 (the sent data frame contained the acknowledgement number) */
            lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 = 0U;

            /* -------------------------------------- */
            /* Actual state is ESTABLISHED            */
            /* -------------------------------------- */
            if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E )
            {
                /* Change the connection state of the socket to CLOSE_WAIT */
                rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_CLOSE_WAIT_E );

                /* Report DET if Up_TcpIpEvent_pfct is uninitialized */
                RBA_ETHTCP_CHECK_DETERROR_VOID( (NULL_PTR == TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct),\
                                                 RBA_ETHTCP_E_UPDATESOCKETAFTERSNDACK, RBA_ETHTCP_E_NULL_PTR );

                /* Send event to SoAd to indicate that the connection is closing */
                /* (now that we are in CLOSE_WAIT state we should wait for Close API call from UL) */
                TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct( TcpDynSockTblIdx_uo, TCPIP_TCP_FIN_RECEIVED );

            }

            /* -------------------------------------- */
            /* Actual state is FIN_WAIT_1             */
            /* -------------------------------------- */
            else if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_1_E )
            {
                /* Change the connection state of the socket to CLOSING */
                rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_CLOSING_E );
            }

            /* -------------------------------------- */
            /* Actual state is FIN_WAIT_2             */
            /* -------------------------------------- */
            else if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_2_E )
            {
                /* Change the connection state of the socket to TIME_WAIT */
                rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_TIME_WAIT_E );
            }

            /* ---------------------------------------- */
            /* Other actual state                       */
            /* ---------------------------------------- */
            else
            {
                /* Nothing to do */
            }
        }
        break;
    }

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateSocketAfterSndFin()    - Execute the post processing after FIN based flag is sent                          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/**                                                                                                                             **/
/** Parameters (inout):                       None                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                         None                                                                              **/
/**                                                                                                                             **/
/** Return value:                             None                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_UpdateSocketAfterSndFin ( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* -------------------------------------- */
    /* Actions after SEND FIN ACK             */
    /* -------------------------------------- */

#if ( RBA_ETHTCP_DLYACK_ENABLED == STD_ON )
    /* Stop the Delayed ack timer if it is running */
    /* (as the acknowledgement number was sent, we can stop the DelayedAck timer) */
    rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_DLYACK_E );
#endif

    /* Set the total received data length since the last sending of ACK to 0 (the sent data frame contained the acknowledgement number) */
    lTcpDynSockTbl_pst->TotalRcvdDataLen_u16 = 0U;

    /* Increment the send sequence number */
    /* (FIN bit takes one byte in the sequence number space) */
    rba_EthTcp_SetSeqNum(   TcpDynSockTblIdx_uo,
                            (lTcpDynSockTbl_pst->SndSeqNum_u32 + 1U) );

    /* --------------------------------------- */
    /* Actual state is ESTABLISHED             */
    /* --------------------------------------- */
    if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E )
    {
        /* Change the connection state of the socket to FIN_WAIT_1 */
        rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_FIN_WAIT_1_E );

        /* No RTT measurement to start because the FIN-ACK is the last frame that can be sent (except ACK) */
    }

    /* -------------------------------------- */
    /* Actual state is CLOSE_WAIT             */
    /* -------------------------------------- */
    else if( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E )
    {
        /* Change the connection state of the socket to LAST_ACK */
        rba_EthTcp_SetConnectionState( TcpDynSockTblIdx_uo, RBA_ETHTCP_CONN_LAST_ACK_E );

        /* No RTT measurement to start because the FIN-ACK is the last frame that can be sent (except ACK) */
    }

    /* ----------------------------------------------- */
    /* Actual state is FIN WAIT 1, CLOSING or LAST ACK */
    /* (case of FIN-ACK retransmission)                */
    /* ----------------------------------------------- */
    else
    {
        /* Nothing to do */
    }

    return;
}

/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateSocketAfterSndRst()    - Execute the post processing after RST based frame is sent                         **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/**                                                                                                                             **/
/** Parameters (inout):                       None                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                         None                                                                              **/
/**                                                                                                                             **/
/** Return value:                             None                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_UpdateSocketAfterSndRst ( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* -------------------------------------- */
    /* Actions after SEND RST or SEND RST ACK */
    /* -------------------------------------- */

    /* -------------------------------------- */
    /* Actual state is CLOSED                 */
    /* -------------------------------------- */
    if ( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSED_E )
    {
        /* If there is no SYN postponed to be sent */
        /* (If a SYN is postponed to be sent, we should not close the socket because the SYN still need to be sent) */
        if( lTcpDynSockTbl_pst->PostponeSyn_b == FALSE )
        {
            /* Reset the socket */
            rba_EthTcp_ResetSock( TcpDynSockTblIdx_uo, TCPIP_TCP_RESET );
        }
    }

    /* -------------------------------------- */
    /* Actual state is an other state         */
    /* -------------------------------------- */
    else
    {
        /* Reset the socket */
        rba_EthTcp_ResetSock( TcpDynSockTblIdx_uo, TCPIP_TCP_RESET );
    }

    return;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */

