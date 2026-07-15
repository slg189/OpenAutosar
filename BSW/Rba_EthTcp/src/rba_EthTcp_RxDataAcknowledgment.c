

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
#include "rba_EthTls.h"
#include "rba_EthTls_Cbk.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */

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
static void rba_EthTcp_UpdateTimerAfterDataAcknowledgment( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                           boolean            NeedTimerRestart_b,
                                                           boolean            NeedTimerExpiration_b );
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
/** rba_EthTcp_RxDataAcknowledgment()       - Execute the processing of a received data acknowledgment                          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/** ConfirmedLen_u16                        - Length of confirmed data by the remote (acknowledged data)                        **/
/**                                                                                                                             **/
/** Parameters (inout):                       None                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                         None                                                                              **/
/**                                                                                                                             **/
/** Return value:                             None                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_RxDataAcknowledgment( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                      uint16             ConfirmedLen_u16 )
{
    /* Declare local variables */
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lUnackedDataLen_u16;

#if(TCPIP_TLS_ENABLED == STD_ON )
    rba_EthTcp_TlsTxReqBuffInfo_tst  * lTlsTxReqBuff_pst;
    TcpIp_TlsConnectionIdType       lTlsTxReqQIndex_uo;
    uint32                          lConfirmedAppLen_u32;
    uint32                          lRemainingAckLen_u32;
    uint16                          lAckedAppDataLen_u16;
#endif/* (TCPIP_TLS_ENABLED == STD_ON ) */

#endif /* RBA_ETHTCP_TXWNDSIZE > 0 */

    boolean                         lNeedTimerRestart_b;
    boolean                         lNeedTimerExpiration_b;

    /* Initialize the timer restart flag to FALSE */
    lNeedTimerRestart_b = FALSE;

    /* Initialize the timer expiration flag to FALSE */
    lNeedTimerExpiration_b = FALSE;

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

#if(TCPIP_TLS_ENABLED == STD_ON )
    lTlsTxReqQIndex_uo = lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo;
    lConfirmedAppLen_u32 = 0U;
    lAckedAppDataLen_u16 = 0U;
#endif/* (TCPIP_TLS_ENABLED == STD_ON ) */

    /* ------------------------- */
    /* Complete or Partial ACK   */
    /* ------------------------- */

    /* If the data length confirmed by the remote is greater than 0 (complete or partial ACK) */
    if( ConfirmedLen_u16 > 0U )
    {
        /* ------------------------- */
        /* Update variables          */
        /* ------------------------- */

        /* Reset the duplicate ACK counter */
        /* (we have received an ACK that acknowledge some data so we can reset the duplicate ACK counter) */
        lTcpDynSockTbl_pst->DuplicateAckCntr_u8 = 0U;

        /* Update the acknowledged data location in the Tcp Tx buffer */
        rba_EthTcp_SetTxBufAfterAcked( TcpDynSockTblIdx_uo, ConfirmedLen_u16 );

        /* Get the new length of unacked data in the Tcp Tx buffer */
        lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );

        /* -------------------------------------------- */
        /* Oldest unacked segment length handling       */
        /* -------------------------------------------- */

        /* If the oldest unacked segment is now acknowledged */
        if( ConfirmedLen_u16 >= lTcpDynSockTbl_pst->OldestUnackedSegmentLength_u16 )
        {
            /* Set the oldest unacked segment length to 0 */
            lTcpDynSockTbl_pst->OldestUnackedSegmentLength_u16 = 0U;

            /* If there are still unacked data */
            if( lUnackedDataLen_u16 > 0U )
            {
                /* Set the oldest unacked segment length to the length of unacked data */
                lTcpDynSockTbl_pst->OldestUnackedSegmentLength_u16 = lUnackedDataLen_u16;

#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON || RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON )
                /* Set a flag to indicate that the RETX and UTO timers need to be restarted */
                /* (we are going to begin a new retransmission procedure for the next unacknowledged data) */
                lNeedTimerRestart_b = TRUE;
#endif
            }
        }

        /* The oldest unacked segment is not fully acknowledged yet */
        else
        {
            /* Decrement the oldest unacked segment length by the length of acknowledged data */
            lTcpDynSockTbl_pst->OldestUnackedSegmentLength_u16 -= ConfirmedLen_u16;
        }

        /* ------------------------- */
        /* Partial Ack handling      */
        /* ------------------------- */

        /* If we have a partial ACK (all our data have not been fully acknowledged) */
        if( lUnackedDataLen_u16 > 0U )
        {
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
            /* When a partial ACK is received, we can do an intermediate retransmission (expire RETX timer) */
            /* (See congestion control algorithms - RFC3782 p8) */
            lNeedTimerExpiration_b = TRUE;
#endif
        }

        /* ------------------------- */
        /* Upper layer confirmation  */
        /* ------------------------- */

#if(( TCPIP_TLS_ENABLED == STD_ON ) && ( RBA_ETHTCP_TXWNDSIZE > 0 ))

        /* Check if it is secure connection*/
        if(lTcpDynSockTbl_pst->TlsConnectionId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)
        {
            /* check if connection is secure connection and TLS handshake is completed.ie the ack recieved is for application data and not TLS handshake data.*/
            if(TRUE == lTcpDynSockTbl_pst->TlsHandshakeCompleted_b)
            {
                /* Check if ack belongs to Only Tls data:(Ack for Alert message).In this case,the lTlsTxReqQIndex_uo is default value as there was no Tx request from application. */
                if(lTlsTxReqQIndex_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)
                {
                    /* the remaing ack length to be confirmed to upper layer is current recieved ack length and previously stored ack length.*/
                    lRemainingAckLen_u32 = ConfirmedLen_u16 + rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsPreviouslyAckedDataLen_u16;

                    /* execute while loop till complete Tx data length is confirmed to each Tx request from application*/
                    while((lRemainingAckLen_u32 >= \
                            (rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8].TlsEncryptedDataLen_u16)) && \
                            (rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8].TlsTxReqBuffOccupied_b == TRUE))
                    {
                        lTlsTxReqBuff_pst = &rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8];

                        /* decrement the remaining ack length to be confirmed to ul by the length of encrypted data of that tx request.*/
                        lRemainingAckLen_u32 -= lTlsTxReqBuff_pst->TlsEncryptedDataLen_u16;

                        /* store the total confirmed length to be given to application*/
                        lConfirmedAppLen_u32 += lTlsTxReqBuff_pst->TlsAppDataLen_u16;

                        /*reset the buffer after read operation */
                        lTlsTxReqBuff_pst->TlsEncryptedDataLen_u16 = 0U;

                        lTlsTxReqBuff_pst->TlsAppDataLen_u16 = 0U;

                        lTlsTxReqBuff_pst->TlsTxReqBuffOccupied_b = FALSE;

                        rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsPreviouslyAckedDataLen_u16 = 0;

                        /* Decrement the count of occupied Tx req buffers*/
                        rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].NoOfOccupiedTxReqBuff_u8--;

                        /*shift to next buffer to be read in queue */
                        rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8++;

                        /* if end of buffer is reached*/
                        if(rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8 == RBA_ETHTCP_CFG_TLS_MAX_TX_REQ_PER_Q)
                        {
                            /*shift to first buffer in queue */
                            rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8 = 0;
                        }
                    }

                    /* If all data has been acked and no pending request in any buffers */
                    if(rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8].TlsTxReqBuffOccupied_b == FALSE)
                    {
                        /* Release the Tls Queue */
                        rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].IsQueueAllocated_b = FALSE;

                        /* Reset the Tls to Tcp Connection queue mapping */
                        lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo = RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE;

                    }

                    /* If there is further data acknowledged apart from application data,this implies data belongs to tls(tls allot message use case)*/
                    if(lRemainingAckLen_u32 > 0U)
                    {
                        /* If remaining data acknowledged is application data*/
                        if(rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].RdIndex_u8].TlsTxReqBuffOccupied_b == TRUE)
                        {
                            /* store the remaining acked length to be used in next context of Tx confirmation .*/
                            rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsPreviouslyAckedDataLen_u16 = (uint16)lRemainingAckLen_u32;
                        }
                        else
                        {
                            /* Call Tls tx confirmation to indicate Tls handshake data sent is acknowledged */
                            rba_EthTls_TxConfirmation(lTcpDynSockTbl_pst-> TlsConnectionId_uo, (uint16)lRemainingAckLen_u32 );

                        }
                    }

                    if(NULL_PTR != TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TxConfirmation_pfct)
                    {
                        /* If any application Tx request is completely acked*/
                        while(lConfirmedAppLen_u32 > 0U)
                        {
                            /* provide Tx confirmation twice since it is possible only to confirm maximum of 0xFFFFU at once to UL*/
                            lAckedAppDataLen_u16 = (lConfirmedAppLen_u32 <=  0xFFFFU) ? (uint16)lConfirmedAppLen_u32 : (0xFFFFU);

                            /* Call the Upper layer API to indicate that the transmission of some data has succeeded (the acknowledgment has been received for these data) */
                            TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TxConfirmation_pfct( TcpDynSockTblIdx_uo,
                                                                                                                                        lAckedAppDataLen_u16 );

                            /*compute remaining length to be confirmed to UL */
                            lConfirmedAppLen_u32 -= lAckedAppDataLen_u16;

                        }
                    }
                }
                else
                {
                    /* Call Tls tx confirmation to indicate Tls  data sent is acknowledged */
                    rba_EthTls_TxConfirmation(lTcpDynSockTbl_pst-> TlsConnectionId_uo, ConfirmedLen_u16 );
                }

            }
            else
            {
                /* Call Tls tx confirmation to indicate Tls handshake data sent is acknowledged */
                rba_EthTls_TxConfirmation(lTcpDynSockTbl_pst-> TlsConnectionId_uo, ConfirmedLen_u16 );

            }
        }
        else
#endif/* (( TCPIP_TLS_ENABLED == STD_ON ) && (( RBA_ETHTCP_TXWNDSIZE > 0 )))*/
        {
	        if(NULL_PTR != TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TxConfirmation_pfct)
	        {
	            /* Call the Upper layer API to indicate that the transmission of some data has succeeded (the acknowledgment has been received for these data) */
	            TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8].Up_TxConfirmation_pfct( TcpDynSockTblIdx_uo,
	                                                                                                                        ConfirmedLen_u16 );
	        }
		}

    }

    /* ------------------------- */
    /* Duplicate ACK             */
    /* ------------------------- */

    /* If the data length confirmed by the remote is equal to 0 (duplicate ACK) */
    else
    {
        /* Get the length of unacked data in the Tcp Tx buffer */
        lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );

        /* If there are unacked data */
        if( lUnackedDataLen_u16 > 0U )
        {
            /* Increment the duplicate ack counter */
            /* (duplicate ack counting should be done only in case we have some unacked data that might need retransmission) */
            lTcpDynSockTbl_pst->DuplicateAckCntr_u8++;

            /* If we have received 3 duplicate ACK */
            if( lTcpDynSockTbl_pst->DuplicateAckCntr_u8 >= 3U )
            {
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
                /* When 3 duplicate ACK are received, we can do an intermediate retransmission (expire RETX timer) */
                /* (See fast retransmit algorithm - RFC5681 p8) */
                lNeedTimerExpiration_b = TRUE;
#endif

                /* Reset the duplicate ACK counter */
                lTcpDynSockTbl_pst->DuplicateAckCntr_u8 = 0U;
            }
        }
    }
#endif

    /* ------------------------- */
    /* Timer management          */
    /* ------------------------- */

    /* Update the Tcp timers */
    rba_EthTcp_UpdateTimerAfterDataAcknowledgment(  TcpDynSockTblIdx_uo,
                                                    lNeedTimerRestart_b,
                                                    lNeedTimerExpiration_b );

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateTimerAfterDataAcknowledgment() - Execute the timers update after the reception of a data acknowledgment    **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                             - Tcp dynamic socket table index                                            **/
/** NeedTimerRestart_b                              - Flag indicating if the ReTx and UTO timers need to be restarted           **/
/** NeedTimerExpiration_b                           - Flag indicating if the ReTx timer need to be expired                      **/
/**                                                                                                                             **/
/** Parameters (inout):                               None                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                 None                                                                      **/
/**                                                                                                                             **/
/** Return value:                                     None                                                                      **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_UpdateTimerAfterDataAcknowledgment( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                           boolean            NeedTimerRestart_b,
                                                           boolean            NeedTimerExpiration_b )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
#if ( (RBA_ETHTCP_RETX_ENABLED == STD_ON) || (RBA_ETHTCP_ZWP_ENABLED == STD_ON) || (RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON) )
    uint16                          lUnackedDataLen_u16;
#endif
#if ( RBA_ETHTCP_ZWP_ENABLED == STD_ON )
    uint16                          lRemainDataLen_u16;
#endif

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

#if ( (RBA_ETHTCP_RETX_ENABLED == STD_ON) || (RBA_ETHTCP_ZWP_ENABLED == STD_ON) || (RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON) )
    /* Get the length of unacked data in the Tcp Tx buffer */
    lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );
#endif

#if ( RBA_ETHTCP_KEEP_ALIVE_ENABLED == STD_ON )
    /* ------------------------- */
    /* Keep Alive timer          */
    /* ------------------------- */

    /* Restart the Keep Alive timer (if it is running) */
    /* (the connection has to remain open if ACK is received) */
    rba_EthTcp_RestartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_KEEPALIVE_E );
#endif

#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
    /* ------------------------- */
    /* ReTx timer                */
    /* ------------------------- */

    /* Check if the starting / restarting / expiration of the RETX timer is needed */
    /* (there are unacked data and window size is greater than 0) */
    if ( (lUnackedDataLen_u16 > 0U)                         &&
         (lTcpDynSockTbl_pst->RcvdWndSize_u16 > 0U) )
    {
        /* Start the Retransmission timer (if it is not already running) */
        rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_RETX_E );

        /* Check if the restart flag is set */
        if( NeedTimerRestart_b != FALSE )
        {
            /* Restart the Retransmission timer */
            rba_EthTcp_RestartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_RETX_E );
        }

        /* Check if the expiration flag is set */
        if( NeedTimerExpiration_b != FALSE )
        {
            /* Expire the Retransmission timer */
            /* (In the next MainFunction a retransmission will occur) */
            rba_EthTcp_ExpireTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_RETX_E );
        }
    }

    /* Check if the stopping of the RETX timer is needed */
    /* (all the data are acknowledged or zero window is advertised by the remote) */
    else
    {
        /* Stop the Retransmission timer (if it is running) */
        rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_RETX_E );
    }
#else
    (void)NeedTimerExpiration_b;
    (void)NeedTimerRestart_b;
#endif

#if ( RBA_ETHTCP_ZWP_ENABLED == STD_ON )
    /* ------------------------- */
    /* ZWP timer                 */
    /* ------------------------- */

    /* Get how much data in Tcp Tx buffer is ready to be sent */
    lRemainDataLen_u16 = rba_EthTcp_GetTxBufSndLen( TcpDynSockTblIdx_uo );

    /* Check if the starting / restarting of ZWP timer is needed */
    /* (some data are unacked or pending to be sent and remote window size is equal to 0) */
    if( ((lUnackedDataLen_u16 > 0U) || (lRemainDataLen_u16 > 0U))   &&
        (lTcpDynSockTbl_pst->RcvdWndSize_u16 == 0U) )
    {
        /* Start the Zero Window Probe Timer (if it is not already running) */
        rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_ZWP_E );

        /* Restart the Zero Window Probe Timer */
        /* (the connection has to stay open if remote window size remain equal to 0 - RFC1122 p92) */
        rba_EthTcp_RestartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_ZWP_E );
    }

    /* Check if the stopping of ZWP timer is needed */
    /* (no more data are unacked or pending to be sent or remote window size is greater than 0) */
    else
    {
        /* Stop the Zero Window Probe Timer (if it is running) */
        rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_ZWP_E );
    }
#endif

#if ( RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON )
    /* ------------------------- */
    /* User Timeout timer        */
    /* ------------------------- */

    /* Check if the restarting of the UTO timer is needed */
    /* (there are unacked data) */
    if ( lUnackedDataLen_u16 > 0U )
    {
        /* Check if the restart flag is set */
        if( NeedTimerRestart_b != FALSE )
        {
            /* Restart the User Timeout timer (if it is running) */
            rba_EthTcp_RestartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_UTO_E );
        }
    }

    /* Check if the stopping of UTO timer is needed */
    /* (all the data are acknowledged) */
    else
    {
        /* Stop the User Timeout timer */
        rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_UTO_E );
    }
#endif

    return;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
