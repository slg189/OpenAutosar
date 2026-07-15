

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
/** rba_EthTcp_Listen() - By this API, the rba_EthTcp module is requested to listen on the TCP socket specified by the          **/
/**                       Tcp socket id                                                                                         **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpSockId_uo        - Tcp socket id                                                                                         **/
/** MaxChannels_u16     - Maximum number of new parallel connections established on this listen connection                      **/
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
Std_ReturnType rba_EthTcp_Listen( TcpIp_SocketIdType TcpSockId_uo,
                                  uint16             MaxChannels_u16 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_LISTEN, RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if TcpSockId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_LISTEN, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if MaxChannels_u16 is not in the right range */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( ((MaxChannels_u16 == 0U) || (MaxChannels_u16 >= TCPIP_TCPSOCKETMAX)) , RBA_ETHTCP_E_LISTEN, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    /* Report DET if Up_TcpAccepted_pfct is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (NULL_PTR == TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthTcp_DynSockTbl_ast[TcpSockId_uo].UL_SockOwnerIdx_u8].Up_TcpAccepted_pfct), \
                                       RBA_ETHTCP_E_LISTEN, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* Check if the Tcp socket is bound */
    if ( lTcpDynSockTbl_pst->SockState_en == RBA_ETHTCP_SOCK_BOUND_E )
    {
        /* -------------------------------------------- */
        /* Listen for Tcp socket in CLOSED state        */
        /* -------------------------------------------- */
        if ( lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSED_E )
        {
            /* Check if requested number of client connections (MaxChannels_u16) is available */
            if ( MaxChannels_u16 <= (TCPIP_TCPSOCKETMAX - rba_EthTcp_SockConnCntr_u16) )
            {
                /* Set the connection state of the Tcp socket to LISTEN */
                rba_EthTcp_SetConnectionState( TcpSockId_uo, RBA_ETHTCP_CONN_LISTEN_E );

                /* Now socket is ready to accept incoming connection requests */
                lTcpDynSockTbl_pst->SockState_en = RBA_ETHTCP_SOCK_OPENED_E;

                /* Set the number of remaining channels in the Tcp dynamic table entry of the listening socket */
                lTcpDynSockTbl_pst->RemainChannels_u16 = MaxChannels_u16;

                /* Set the return value of the function to E_OK */
                lFunctionRetVal_en = E_OK;
            }
        }
    }

    return lFunctionRetVal_en;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
