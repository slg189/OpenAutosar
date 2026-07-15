

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

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */

#include "rba_BswSrv.h"

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
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
static Std_ReturnType rba_EthTcp_CopyDataValidPointer     ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                            const uint8 *      TxData_cpu8,
                                                            uint32             AvlLen_u32,
                                                            uint16 *           CopiedLength_pu16 );

static Std_ReturnType rba_EthTcp_CopyDataNullPointer      ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                            uint32             AvlLen_u32,
                                                            boolean            ForceRetrieve_b,
                                                            uint16 *           CopiedLength_pu16 );

static void rba_EthTcp_UpdateSocketAfterCopy              ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                            uint16             CopiedLen_u16 );

#if( TCPIP_TLS_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_TransmitTlsData ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                   const uint8 *      TxData_cpu8,
                                                   uint32             AvlLen_u32);
#endif
#endif
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
/** rba_EthTcp_Transmit()   - This service requests transmission of TCP data to a remote node. The transmission of the data is  **/
/**                         decoupled. The parameter AvailableLength indicates the length of data to be transmitted. Tcp shall  **/
/**                         retrieve data from SoAd via one or more subsequent Up_CopyTxData() calls within the context of      **/
/**                         this transmit function call. Tcp shall retrieve exactly AvailableLength bytes from the SoAd if the  **/
/**                         parameter ForceRetrieve_b is TRUE, otherwise the Tcp may retrieve less data. Note: The Tcp          **/
/**                         segment(s) are sent dependent on runtime factors e.g. receive window) and configuration parameter   **/
/**                         (e.g. Nagle algorithm) . If the socket has not been bound to a local resource via a previous call   **/
/**                         to TcpIp_Bind() the local IP address and port used for transmission is selected by TcpIp            **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo            - Tcp socket id                                                                                     **/
/** TxData_cpu8             - Pointer to a linear buffer containing the data to be transmitted                                  **/
/** AvlLen_u32              - Available data for transmission in bytes                                                          **/
/** ForceRetrieve_b         - This parameter is only valid if TxData_pu8 is a NULL_PTR                                          **/
/**                           Indicates how the TCP/IP stack retrieves data from SoAd if TxData_pu8 is a NULL_PTR               **/
/**                                                                                                                             **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):         None                                                                                              **/
/**                                                                                                                             **/
/** Return value:             Std_ReturnType                                                                                    **/
/**                           E_OK: The request has been accepted                                                               **/
/**                           E_NOT_OK: The request has not been accepted                                                       **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_Transmit( TcpIp_SocketIdType TcpSockId_uo,
                                    const uint8 *      TxData_cpu8,
                                    uint32             AvlLen_u32,
                                    boolean            ForceRetrieve_b )
{
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lCopiedLength_u16;
    Std_ReturnType                  lFunctionRetVal_en;
    Std_ReturnType                  lRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;
    lRetVal_en         = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( ( rba_EthTcp_InitFlag_b == FALSE ), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if AvlLen_u32 is equal to 0 */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (AvlLen_u32 == 0U), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if Up_TxConfirmation_pfct is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (NULL_PTR == TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthTcp_DynSockTbl_ast[TcpSockId_uo].UL_SockOwnerIdx_u8].Up_TxConfirmation_pfct),\
                                       RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

#if( TCPIP_TLS_ENABLED == STD_ON )
    /* Report DET if AvlLen_u32 is equal to 0 */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_DynSockTbl_ast[TcpSockId_uo].TlsRequestedConnId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
#endif

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* If the current state of the Tcp socket allow the Transmit call */
    /* In SYN_RCVD, ESTABLISHED and CLOSE_WAIT states, we also check that the Upper Layer has not closed the connection yet (there is no FIN ACK going to be sent and no FIN ACK postponed to be sent) */
    if ( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_SENT_E)       ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_RCVD_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE))                                                                     ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E) && (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_FIN_ACK_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE)) ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E) && (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_FIN_ACK_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE)) )
    {
        /* If no RESET is going to be sent by the Tcp socket and received MSS is greater than 0 */
        /* (once a reset is prepared to be sent it is not possible to call the Transmit API anymore) */
        /* (in case the received MSS is equal to 0 that means the the remote is not able to receive frames with data) */
        if( (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_RST_E)       &&
            (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_RST_ACK_E)   &&
            (lTcpDynSockTbl_pst->RcvdMss_u16 > 0U) )
        {
#if( TCPIP_TLS_ENABLED == STD_ON )
            /* check if Tcp connection is secure connection*/
            if(lTcpDynSockTbl_pst->TlsConnectionId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)
            {
                if(TRUE == lTcpDynSockTbl_pst->TlsHandshakeCompleted_b)
                {
                    /* call api to process Tx request via TLS*/
                    lFunctionRetVal_en = rba_EthTcp_TransmitTlsData(TcpSockId_uo, TxData_cpu8, AvlLen_u32 );
                }
            }
            else
#endif/* ( TCPIP_TLS_ENABLED == STD_ON ) */
            {
                /* ---------------------------- */
                /* Copy data (valid pointer)    */
                /* ---------------------------- */
                if ( TxData_cpu8 != NULL_PTR )
                {
                    /* Copy data to transmit from SoAd to Tcp with MemCopy */
                    lRetVal_en = rba_EthTcp_CopyDataValidPointer( TcpSockId_uo,
                                                                  TxData_cpu8,
                                                                  AvlLen_u32,
                                                                  &lCopiedLength_u16 );
                }

                /* ---------------------------- */
                /* Copy data (NULL pointer)     */
                /* ---------------------------- */
                else
                {
                    /* Copy data to transmit from SoAd to Tcp by calling Up_CopyTxData */
                    lRetVal_en = rba_EthTcp_CopyDataNullPointer( TcpSockId_uo,
                                                                 AvlLen_u32,
                                                                 ForceRetrieve_b,
                                                                 &lCopiedLength_u16 );
                }

                /* ---------------------------- */
                /* Update socket                */
                /* ---------------------------- */

                /* If data have been successfully copied to Tcp Tx Buffer */
                if ( E_OK == lRetVal_en )
                {

                    /* Update the variables, pointers and timers */
                    rba_EthTcp_UpdateSocketAfterCopy(   TcpSockId_uo,
                                                        lCopiedLength_u16 );


                    /* Set the return value of the function to E_OK */
                    lFunctionRetVal_en = E_OK;
                }

            }
        }
    }

    return lFunctionRetVal_en;
#else
    (void)TcpSockId_uo;
    (void)TxData_cpu8;
    (void)AvlLen_u32;
    (void)ForceRetrieve_b;

    return E_NOT_OK;
#endif
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */
#if( TCPIP_TLS_ENABLED == STD_ON )
/*********************************************************************************************************************************/
/** rba_EthTcp_TransmitTlsData- This api called if Tx is requested over secure connection                                       **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo            - Tcp socket id                                                                                     **/
/** TxData_cpu8             - Pointer to a linear buffer containing the data to be transmitted                                  **/
/** AvlLen_u32              - Available data for transmission in bytes                                                          **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):         None                                                                                              **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from SoAd to Tcp                                 **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
static Std_ReturnType rba_EthTcp_TransmitTlsData ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                   const uint8 *      TxData_cpu8,
                                                   uint32             AvlLen_u32)
{
    /* Declare local variables */
    rba_EthTcp_TlsTxReqBuffInfo_tst  * lTlsTxReqBuff_pst;
    rba_EthTcp_DynSockTblType_tst *   lTcpDynSockTbl_pst;
    const TcpIp_SockOwnerConfig_tst * lSockOwnerConfig_pcst;
    uint8 *                           lPrevNextFreeDataPos_pu8;
    uint16                            lPrevOcccupiedSizeSnd_u16;
    TcpIp_TlsConnectionIdType         lTlsTxReqQIndex_uo;
    uint16                            lTcpFreeTxBuffLen_u16;
    uint16                            lTlsAvlLen_u16;
    Std_ReturnType                    lRetVal_en;
    Std_ReturnType                    lFunctionRetVal_en;
    BufReq_ReturnType                 lRetBufReq_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lTlsAvlLen_u16 = (uint16)AvlLen_u32;
    lRetVal_en = E_NOT_OK;
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Get Socket owner configuration details from socket index */
    lSockOwnerConfig_pcst = ( & ( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8] ) );

    /* Report DET if Up_CopyTxData_pfct is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( ((NULL_PTR == TxData_cpu8) && (NULL_PTR == lSockOwnerConfig_pcst->Up_CopyTxData_pfct )), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* If tcp connection is not mapped to TLS request queue then reserve a free queue */
    if(lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo == RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)
    {
        /*check if any free queue is available to store transmit request */
        rba_EthTcp_ReserveFreeQSlotForTlsTx(TcpDynSockTblIdx_uo);
    }

    /* check If tcp connection is mapped to TLS request queue */
    if(lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)
    {
        /* copy q index to local variable*/
        lTlsTxReqQIndex_uo = lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo;
        lTlsTxReqBuff_pst = &rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8];

        /*Get the available Tcp Tx buffer length */
        lTcpFreeTxBuffLen_u16 = rba_EthTcp_GetTxBufFreeLen(TcpDynSockTblIdx_uo);

        /*Get the available Tls Tx buffer length */
        lRetVal_en = rba_EthTls_GetSecureDataLength(lTcpDynSockTbl_pst->TlsConnectionId_uo, &lTlsAvlLen_u16);

        /* If all the data from UL can be copied in the Tcp Tx buffer and encrypted length would be grater than requested length */
        if((lRetVal_en == E_OK) && (lTlsAvlLen_u16 <= lTcpFreeTxBuffLen_u16) && (AvlLen_u32 < lTlsAvlLen_u16) && (lTlsTxReqBuff_pst->TlsTxReqBuffOccupied_b == FALSE))
        {
            /* Set the buffer as occupied*/
            lTlsTxReqBuff_pst->TlsTxReqBuffOccupied_b = TRUE;

            /* store the next free pointer of Tcp tx buffer in order to return back to same offset if any failure occurs to while adding data to tcp buffer */
            lPrevNextFreeDataPos_pu8 = lTcpDynSockTbl_pst->TxWndNxtFree_pu8;

            /* store the occupied of Tcp tx buffer in order to return back to same value if any failure occurs to while adding data to tcp buffer */
            lPrevOcccupiedSizeSnd_u16 = lTcpDynSockTbl_pst->TxOccupiedSizeSnd_u16;

            lRetVal_en = E_NOT_OK;

            /* check If data pointer is NULL*/
            if(TxData_cpu8 == NULL_PTR)
            {
                /* Copy UL data into the local buffer */
                lRetBufReq_en = lSockOwnerConfig_pcst->Up_CopyTxData_pfct( TcpDynSockTblIdx_uo, \
                                                                           &rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsDataReqBuff_pu8[0], \
                                                                           (uint16)AvlLen_u32 );

                /* If Up_CopyTxData_pfct returns BUFREQ_OK*/
                if(lRetBufReq_en == BUFREQ_OK)
                {
                    /* Request TLS to encrypt the application data*/
                    lRetVal_en = rba_EthTls_Transmit( lTcpDynSockTbl_pst->TlsConnectionId_uo, \
                                                      &rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsDataReqBuff_pu8[0], \
                                                      (uint16)AvlLen_u32);

                }/* If Up_CopyTxData_pfct returns BUFREQ_OK*/


            }
            else
            {
                /* Request TLS to encrypt the application data*/
                lRetVal_en = rba_EthTls_Transmit( lTcpDynSockTbl_pst->TlsConnectionId_uo, \
                                                  TxData_cpu8, \
                                                  (uint16)AvlLen_u32);
            }

            /* if Request TLS to encrypt the application data was successfull*/
            if((lRetVal_en == E_OK) && (lTlsTxReqBuff_pst->TlsEncryptedDataLen_u16 == lTlsAvlLen_u16))
            {
                /* Store the data length requested by UL*/
                lTlsTxReqBuff_pst->TlsAppDataLen_u16 = (uint16)AvlLen_u32;

                /* Incremet the count of occupied Tx req buffers*/
                rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].NoOfOccupiedTxReqBuff_u8++;

                /*shift the write index to next buffer in queue*/
                rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8++;

                /* check if write index reached RBA_ETHTCP_CFG_TLS_MAX_TX_REQ_PER_Q*/
                if(rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8 == RBA_ETHTCP_CFG_TLS_MAX_TX_REQ_PER_Q)
                {
                    /*shift the write index to start of queue*/
                    rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8 = 0;
                }

                /* If we are in ESTABLISHED or CLOSE WAIT state (states for which it is possible to execute the sending of data) */
                if ( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)        ||
                     (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E) )
                {
                    /* The window size of the remote is greater than the length of unacknowledged data */
                    if ( lTcpDynSockTbl_pst->RcvdWndSize_u16 > rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo ) )
                    {
                        /* Send DATA in the next MainFunction */
                        (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_DATA_E );
                    }

#if ( RBA_ETHTCP_ZWP_ENABLED == STD_ON )
                    if(lTcpDynSockTbl_pst->RcvdWndSize_u16 == 0U)
                    {
                        /* Start Zero Window Probe Timer  */
                        rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_ZWP_E );
                    }
#endif
                }

                /* update function return value*/
                lFunctionRetVal_en = E_OK;

            }/* if Request TLS to encrypt the application data was successfull*/
            else
            {
                /* restore the pointer to previous value before adding data to tcp buffer */
                lTcpDynSockTbl_pst->TxWndNxtFree_pu8 = lPrevNextFreeDataPos_pu8;

                /* restore the original tp occupied buffer size */
                lTcpDynSockTbl_pst->TxOccupiedSizeSnd_u16 =  lPrevOcccupiedSizeSnd_u16;

                /* Set the tls req buffer as free*/
                lTlsTxReqBuff_pst->TlsTxReqBuffOccupied_b = FALSE;

            }

        }/* If all the data from UL can be copied in the Tcp Tx buffer */

    } /* check If tcp connection is mapped to TLS request queue */

    return lFunctionRetVal_en;
}
#endif

/*********************************************************************************************************************************/
/** rba_EthTls_SecureTransmit()   - This service requests transmission of TCP TLS encrypted data to a remote node.    **/
/**                                         The transmission of the data is decoupled. The parameter TlsEncryptedDataLength_u32 **/
/**                                         indicates the length of data to be transmitted.                                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo            -     Tcp socket id                                                                                 **/
/** TlsEncryptedData_cpu8   -     Pointer to a linear buffer containing the encrypted TLS data to be transmitted                **/
/** TlsEncryptedDataLength_u32  - Available encrypted data for transmission in bytes                                            **/
/** IsAllEncyptionDataAvailable_b - This parameter is only valid if TxData_pu8 is a NULL_PTR                                    **/
/**                                                                                                                             **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):         None                                                                                              **/
/** TcpCopiedLength_pu16 *    - Pointer to length copied in Tcp buffer                                                          **/
/** Return value:             Std_ReturnType                                                                                    **/
/**                           E_OK: The request has been accepted                                                               **/
/**                           E_NOT_OK: The request has not been accepted                                                       **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/

Std_ReturnType rba_EthTcp_SecureTransmit( TcpIp_SocketIdType TcpSockId_uo,
                                          const uint8 *      TlsEncryptedData_cpu8,
                                          uint16             TlsEncryptedDataLength_u16)
{
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lTcpCopiedLength_u16;
    TcpIp_TlsConnectionIdType       lTlsTxReqQIndex_uo;
    Std_ReturnType                  lFunctionRetVal_en;
    Std_ReturnType                  lRetVal_en;

    lTcpCopiedLength_u16 = 0;
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */

    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_SECURETRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if AvlLen_u32 is equal to 0 */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TlsEncryptedDataLength_u16 == 0U), RBA_ETHTCP_E_SECURETRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if TcpSockId_u16 is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TlsEncryptedData_cpu8 == NULL_PTR), RBA_ETHTCP_E_SECURETRANSMIT, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (lTcpDynSockTbl_pst->TlsConnectionId_uo == RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE), RBA_ETHTCP_E_SECURETRANSMIT, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* If the current state of the Tcp socket allow the Transmit call */
    /* In SYN_RCVD, ESTABLISHED and CLOSE_WAIT states, we also check that the Upper Layer has not closed the connection yet (there is no FIN ACK going to be sent and no FIN ACK postponed to be sent) */
    if ( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_SENT_E)       ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_SYN_RCVD_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE))                                                                     ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E) && (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_FIN_ACK_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE)) ||
         ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E) && (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_FIN_ACK_E) && (lTcpDynSockTbl_pst->PostponeFin_b == FALSE)) )
    {
        /* If no RESET is going to be sent by the Tcp socket and received MSS is greater than 0 */
        /* (once a reset is prepared to be sent it is not possible to call the Transmit API anymore) */
        /* (in case the received MSS is equal to 0 that means the the remote is not able to receive frames with data) */
        if( (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_RST_E)       &&
            (lTcpDynSockTbl_pst->TcpFlag_en != RBA_ETHTCP_SEND_RST_ACK_E)   &&
            (lTcpDynSockTbl_pst->RcvdMss_u16 > 0U) )
        {

            /* ---------------------------- */
            /* Copy data (valid pointer)    */
            /* ---------------------------- */
            /* Copy data to transmit from SoAd to Tcp with MemCopy */
            lRetVal_en = rba_EthTcp_CopyDataValidPointer( TcpSockId_uo, \
                                                          TlsEncryptedData_cpu8, \
                                                          (uint32)TlsEncryptedDataLength_u16,
                                                          &lTcpCopiedLength_u16);

            /*check If all data is copied in Tcp buffer */
            if((lRetVal_en == E_OK) && (lTcpCopiedLength_u16 == TlsEncryptedDataLength_u16))
            {
                /* copy q index to local variable*/
                lTlsTxReqQIndex_uo = lTcpDynSockTbl_pst->TlsTxReqQueueIndex_uo;

                /*check if data belongs to tls or tls application*/
                if((lTlsTxReqQIndex_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE) && \
                   (rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].\
                         TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8].TlsTxReqBuffOccupied_b == TRUE) && \
                   (rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].NoOfOccupiedTxReqBuff_u8 < RBA_ETHTCP_CFG_TLS_MAX_TX_REQ_PER_Q))
                {
                    /* Advance the TxWndNxtFree pointer by the length of the copied data */
                    rba_EthTcp_SetTxBufBeforeTx( TcpSockId_uo, TlsEncryptedDataLength_u16 );

                    /* Store the encrypted data length given by TLS*/
                    rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].TlsTxReqBuff_pst[rba_EthTcp_TlsTxReqQInfo_ast[lTlsTxReqQIndex_uo].WrIndex_u8].\
                                                                TlsEncryptedDataLen_u16 += TlsEncryptedDataLength_u16;
                }
                else
                {
                    /* Update the variables, pointers and timers */
                    rba_EthTcp_UpdateSocketAfterCopy( TcpSockId_uo, TlsEncryptedDataLength_u16 );
                }

                /* update function return value*/
                lFunctionRetVal_en = E_OK;
            }

        }
    }

    return lFunctionRetVal_en;
#else
    (void)TcpSockId_uo;
    (void)TlsEncryptedData_cpu8;
    (void)TlsEncryptedDataLength_u16;

    return E_NOT_OK;
#endif
}
#endif

/*********************************************************************************************************************************/
/** rba_EthTcp_CopyDataValidPointer - Execute the copying of the data to transmit from SoAd to Tcp when valid pointer is given  **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/** TxData_cpu8                     - Pointer to a linear buffer containing the data to be copied                               **/
/** AvlLen_u32                      - Available data to be copied in bytes                                                      **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** CopiedLength_pu16               - Length of data copied in the Tcp Tx Buffer                                                **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from SoAd to Tcp                                 **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
static Std_ReturnType rba_EthTcp_CopyDataValidPointer ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                        const uint8 *      TxData_cpu8,
                                                        uint32             AvlLen_u32,
                                                        uint16 *           CopiedLength_pu16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lFreeLen_u16;
    uint16                          lLnrFreeLen_u16;
    uint16                          lLenToCopy_u16;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* -------------------------------------------------- */
    /* Check if there is free space in the Tcp Tx buffer  */
    /* -------------------------------------------------- */

    /* Get the total free location in the Tcp Tx buffer */
    lFreeLen_u16 = rba_EthTcp_GetTxBufFreeLen( TcpDynSockTblIdx_uo );

    /* If the free location is greater than 0 */
    if ( lFreeLen_u16 > 0U )
    {
        /* ---------------------------------------- */
        /* Compute the size of data to copy         */
        /* ---------------------------------------- */

        /* If all the data from SoAd can be copied in the Tcp Tx buffer */
        if ( AvlLen_u32 <= lFreeLen_u16 )
        {
            lLenToCopy_u16 = (uint16)AvlLen_u32;
        }

        /* There is not enough free space in the Tcp Tx buffer for all the data */
        else
        {
            lLenToCopy_u16 = 0U;
        }

        /* ------------------------------ */
        /* Execute the copying            */
        /* ------------------------------ */

        /* If there are some data to be copied */
        if ( lLenToCopy_u16 > 0U )
        {
            /* Get the size of the next linear free location in the Tcp Tx buffer */
            lLnrFreeLen_u16 = rba_EthTcp_GetTxBufLnrFreeLen( TcpDynSockTblIdx_uo );

            /* If data should be stored into two blocks in the Tcp Tx buffer */
            if( lLnrFreeLen_u16 < lLenToCopy_u16 )
            {
                /* Copy first part of SoAd data at the end of the Tcp Tx buffer */
                /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. */
                (void)rba_BswSrv_MemCopy( lTcpDynSockTbl_pst->TxWndNxtFree_pu8,
                                          TxData_cpu8,
                                          lLnrFreeLen_u16 );

                /* The rest data is copied at the beginning of the Tcp Tx buffer */
                /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. */
                (void)rba_BswSrv_MemCopy( lTcpDynSockTbl_pst->TxWndStrt_pu8,
                                          TxData_cpu8 + lLnrFreeLen_u16,
                                          (lLenToCopy_u16 - lLnrFreeLen_u16) );
            }

            /* If data should be stored in a linear block in the Tcp Tx buffer */
            else
            {
                /* Copy SoAd data into the Tcp Tx buffer */
                /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. */
                (void)rba_BswSrv_MemCopy( lTcpDynSockTbl_pst->TxWndNxtFree_pu8,
                                          TxData_cpu8,
                                          lLenToCopy_u16 );
            }

            /* Set the length of copied data in out argument of the function */
            ( * CopiedLength_pu16 ) = lLenToCopy_u16;

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
    }

    return lFunctionRetVal_en;
}
#endif


/*********************************************************************************************************************************/
/** rba_EthTcp_CopyDataNullPointer  - Execute the copying of the data to transmit from SoAd to Tcp when NULL pointer is given   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/** AvlLen_u32                      - Available data to be copied in bytes                                                      **/
/** ForceRetrieve_b                 - Indicates if the complete length need to be copied or partial copy is allowed             **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** CopiedLength_pu16               - Length of data copied in the Tcp Tx Buffer                                                **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: Data have been successfully copied from SoAd to Tcp                                 **/
/**                                   E_NOT_OK: No data have been copied                                                        **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
static Std_ReturnType rba_EthTcp_CopyDataNullPointer ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                       uint32             AvlLen_u32,
                                                       boolean            ForceRetrieve_b,
                                                       uint16 *           CopiedLength_pu16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst *   lTcpDynSockTbl_pst;
    const TcpIp_SockOwnerConfig_tst * lSockOwnerConfig_pcst;
    BufReq_ReturnType                 lRetBufReq_en;
    uint16                            lFreeLen_u16;
    uint16                            lLnrFreeLen_u16;
    uint16                            lLenToCopy_u16;
    Std_ReturnType                    lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Get Socket owner configuration details from socket index */
    lSockOwnerConfig_pcst = ( & ( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lTcpDynSockTbl_pst->UL_SockOwnerIdx_u8] ) );

    /* Check for DET errors */
    /* Report DET if Up_CopyTxData_pfct is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (NULL_PTR == lSockOwnerConfig_pcst->Up_CopyTxData_pfct ), RBA_ETHTCP_E_TRANSMIT, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* -------------------------------------------------- */
    /* Check if there is free space in the Tcp Tx buffer  */
    /* -------------------------------------------------- */

    /* Get the total free location in the Tcp Tx buffer */
    lFreeLen_u16 = rba_EthTcp_GetTxBufFreeLen( TcpDynSockTblIdx_uo );

    /* If the free location is greater than 0 */
    if ( lFreeLen_u16 > 0U )
    {
        /* ---------------------------------------- */
        /* Compute the size of data to copy         */
        /* ---------------------------------------- */

        /* If Force Retrieve is FALSE */
        /* (a part of the data to be transmitted can to be retrieved within this function call) */
        if ( ForceRetrieve_b == FALSE )
        {
            /* If all the data from SoAd can be copied in the Tcp Tx buffer */
            if ( AvlLen_u32 <= lFreeLen_u16 )
            {
                lLenToCopy_u16 = (uint16)AvlLen_u32;
            }

            /* There is not enough free space in the Tcp Tx buffer for all the data */
            else
            {
                lLenToCopy_u16 = lFreeLen_u16;
            }
        }

        /* If Force Retrieve is TRUE */
        /* (all data to be transmitted shall be retrieved within this function call) */
        else
        {
            /* If all the data from SoAd can be copied in the Tcp Tx buffer */
            if ( AvlLen_u32 <= lFreeLen_u16 )
            {
                lLenToCopy_u16 = (uint16)AvlLen_u32;
            }

            /* There is not enough free space in the Tcp Tx buffer for all the data */
            else
            {
                lLenToCopy_u16 = 0U;
            }

        }

        /* ------------------------------ */
        /* Execute the copying            */
        /* ------------------------------ */

        /* If there are some data to be copied */
        if ( lLenToCopy_u16 > 0U )
        {
            /* Get the size of the next linear free location in the Tcp Tx buffer */
            lLnrFreeLen_u16 = rba_EthTcp_GetTxBufLnrFreeLen( TcpDynSockTblIdx_uo );

            /* If data should be stored into two blocks in the Tcp Tx buffer */
                if( lLnrFreeLen_u16 < lLenToCopy_u16 )
                {
                    /* Copy first part of SoAd data at the end of the Tcp Tx buffer */
                    lRetBufReq_en = lSockOwnerConfig_pcst->Up_CopyTxData_pfct( TcpDynSockTblIdx_uo,
                                                                               lTcpDynSockTbl_pst->TxWndNxtFree_pu8,
                                                                               lLnrFreeLen_u16 );

                    if( lRetBufReq_en == BUFREQ_OK )
                    {
                        /* The rest data is copied at the beginning of the Tcp Tx buffer */
                        lRetBufReq_en = lSockOwnerConfig_pcst->Up_CopyTxData_pfct( TcpDynSockTblIdx_uo,
                                                                                   lTcpDynSockTbl_pst->TxWndStrt_pu8,
                                                                                   lLenToCopy_u16 - lLnrFreeLen_u16 );
                    }
                }

                /* If data should be stored in a linear block in the Tcp Tx buffer */
                else
                {
                        /* Copy SoAd data into the Tcp Tx buffer */
                        lRetBufReq_en = lSockOwnerConfig_pcst->Up_CopyTxData_pfct( TcpDynSockTblIdx_uo,
                                                                                   lTcpDynSockTbl_pst->TxWndNxtFree_pu8,
                                                                                   lLenToCopy_u16 );
                }

                /* If no error occured during the copying of the data using Up_CopyTxData */
                if ( lRetBufReq_en == BUFREQ_OK )
                {
                    /* Set the length of copied data in out argument of the function */
                    ( * CopiedLength_pu16 ) = lLenToCopy_u16;

                    /* Set the return value of the function to E_OK */
                    lFunctionRetVal_en = E_OK;
                }
        }
    }
    /* If there is no free space in the Tcp Tx buffer */
    else
    {
        /* If no free memory is available then inform SoAd to wait until Tx buffer memory is available */
        /* SoAd shall not terminate the session if Up_CopyTxData() is called with length of 0 */
         (void)lSockOwnerConfig_pcst->Up_CopyTxData_pfct( TcpDynSockTblIdx_uo,
                                                          lTcpDynSockTbl_pst->TxWndNxtFree_pu8,
                                                          0U );
    }

    return lFunctionRetVal_en;
}
#endif


/*********************************************************************************************************************************/
/** rba_EthTcp_UpdateSocketAfterCopy    - Update the Tcp socket after the data have been copied in the Tcp Tx Buffer            **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                 - Tcp dynamic socket table index                                                        **/
/** CopiedLen_u16                       - Length of data that has been copied in the Tcp Tx Buffer                              **/
/**                                                                                                                             **/
/** Parameters (inout):                   None                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):                     None                                                                                  **/
/**                                                                                                                             **/
/** Return value:                         None                                                                                  **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
static void rba_EthTcp_UpdateSocketAfterCopy ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                               uint16             CopiedLen_u16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lUnackedDataLen_u16;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* ---------------------------------------- */
    /* Update Tx Buffer pointer                 */
    /* ---------------------------------------- */

    /* Advance the TxWndNxtFree pointer by the length of the copied data */
    rba_EthTcp_SetTxBufBeforeTx( TcpDynSockTblIdx_uo, CopiedLen_u16 );

    /* ---------------------------------------- */
    /* Update next frame and timer              */
    /* ---------------------------------------- */

    /* If we are in ESTABLISHED or CLOSE WAIT state (states for which it is possible to execute the sending of data) */
    if ( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)        ||
         (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E) )
    {
        /* Get the length of unacked data in the Tcp Tx buffer */
        lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );

        /* The window size of the remote is greater than the length of unacknowledged data */
        if ( lTcpDynSockTbl_pst->RcvdWndSize_u16 > lUnackedDataLen_u16 )
        {
            /* Send DATA in the next MainFunction */
            (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_DATA_E );
        }

#if ( RBA_ETHTCP_ZWP_ENABLED == STD_ON )
        if ( lTcpDynSockTbl_pst->RcvdWndSize_u16 == 0U )
        {
            /* Start Zero Window Probe Timer  */
            rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_ZWP_E );
        }
#endif
    }
}
#endif

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
