

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_Received()   - By this API, the reception of data is confirmed to the rba_EthTcp module                          **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo            - Tcp socket id                                                                                     **/
/** DataLen_u16             - Number of bytes consumed by the upper layer                                                       **/
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
Std_ReturnType rba_EthTcp_Received ( TcpIp_SocketIdType TcpSockId_uo,
                                     uint16             DataLen_u16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lPreviousWindowSize_u16;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_RECEIVED, RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if TcpSockId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_RECEIVED, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( &( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* Check if we are in a state where the Received call is accepted (ESTABLISHED, FIN WAIT 1 or FIN WAIT 2) */
    if ( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)    ||
         (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_1_E)     ||
         (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_2_E) )
    {
        /* Report DET if DataLen_u16 is not valid */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL( (DataLen_u16 > (lTcpDynSockTbl_pst->TcpSockOpt_pst->SORxWindowMax_u16 - lTcpDynSockTbl_pst->SndWndSize_u16)), RBA_ETHTCP_E_RECEIVED, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

        /* If the consumed data length by the Upper Layer is valid */
        /* ------------------------------------- */
        /* Increment local window size           */
        /* ------------------------------------- */

        /* Save the actual window size */
        lPreviousWindowSize_u16 = lTcpDynSockTbl_pst->SndWndSize_u16;

        /* Increment window by the length in argument */
        /* (as per TcpIp Autosar [SWS_TCPIP_00115]) */
        lTcpDynSockTbl_pst->SndWndSize_u16 += DataLen_u16;

        /* ------------------------------------- */
        /* If Window is reopened                 */
        /* ------------------------------------- */

        /* If the local window is now greater than 0, means the window is now opened */
        if( (lPreviousWindowSize_u16 == 0U) && (lTcpDynSockTbl_pst->SndWndSize_u16 > 0U) )
        {
            /* Send ACK frame in the next MainFunction */
            /* (this ACK is useful to inform the remote that our window is now reopened and the remote can send data again - see RFC1122 p92) */
            (void)rba_EthTcp_SetNextSendFrame( TcpSockId_uo, RBA_ETHTCP_SEND_ACK_E );
        }

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;

    }

    return lFunctionRetVal_en;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
