

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
/** rba_EthTcp_GetSocket()  - By this API service the TCP stack is requested a new Tcp socket                                   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** Domain_u32              - Internet domain (address family AF_INET or AF_INET6)                                              **/
/**                                                                                                                             **/
/** SocketOwner_u8          - Socket Owner                                                                                      **/
/**                                                                                                                             **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** SocketIdPtr_puo         - Pointer to the allocated Tcp socket                                                               **/
/**                                                                                                                             **/
/** Return value:           - Std_ReturnType                                                                                    **/
/**                           E_OK: The request has been accepted                                                               **/
/**                           E_NOT_OK: The request has not been accepted                                                       **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_GetSocket( TcpIp_DomainType     Domain_u32,
                                     TcpIp_SocketIdType * SocketIdPtr_puo,
                                     uint8                SocketOwner_u8 )
{
    /* Declare local variables */
    TcpIp_SocketIdType lIdx_uo;
    Std_ReturnType     lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_GETSOCKET , RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if SocketIdPtr_puo is a NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (SocketIdPtr_puo == NULL_PTR), RBA_ETHTCP_E_GETSOCKET , RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* If there are remaining free Tcp sockets */
    if( rba_EthTcp_SockConnCntr_u16 < TCPIP_TCPSOCKETMAX )
    {
        /* Check if Socket is available */
       for( lIdx_uo = 0; (lIdx_uo < TCPIP_TCPSOCKETMAX); lIdx_uo++ )
       {
           /* State should either be RBA_ETHTCP_SOCK_CLOSED_E  */
           if( rba_EthTcp_DynSockTbl_ast[lIdx_uo].SockState_en == RBA_ETHTCP_SOCK_CLOSED_E )
           {
               /* Now socket is allocated */
               rba_EthTcp_DynSockTbl_ast[lIdx_uo].SockState_en = RBA_ETHTCP_SOCK_ALLOCATED_E;

               /* Set socket table with Domain */
               rba_EthTcp_DynSockTbl_ast[lIdx_uo].DomainType_u32 = Domain_u32;

               /* Store the socket owner in dynamic socket table.It specifies the upper layer to which the socket belongs to. */
               rba_EthTcp_DynSockTbl_ast[lIdx_uo].UL_SockOwnerIdx_u8 = SocketOwner_u8;

               /* Set the socket number as return value */
               ( * SocketIdPtr_puo ) = lIdx_uo;

               /* Increment the total number of used Tcp sockets */
               rba_EthTcp_SockConnCntr_u16++;

               /* Set return value to E_OK */
               lFunctionRetVal_en = E_OK;

               break;
           }
        }
    }

    return lFunctionRetVal_en;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_FindTcpSockId()  - By this API service the Tcp stack is requested a a index of the socket                        **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** LocalAddr_cpst              - Pointer to Local address information                                                          **/
/** RemoteAddr_cpst             - pointer to Remote address information                                                         **/
/**                                                                                                                             **/
/** Parameters (inout):           None                                                                                          **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** SocketIdPtr_puo             - Pointer to the found Tcp socket                                                               **/
/**                                                                                                                             **/
/** Return value                - Std_ReturnType                                                                                **/
/**                               E_OK: Socket found with corresponding remote/local address                                    **/
/**                               E_NOT_OK: Socket didnot found with corresponding remote/local address                         **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_FindTcpSockId ( const TcpIp_SockAddrType * LocalAddr_cpst,
                                          const TcpIp_SockAddrType * RemoteAddr_cpst,
                                          TcpIp_SocketIdType *       SocketIdPtr_puo )

{
    /* Declare local variables */
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;
    rba_EthTcp_DynSockTblType_tst *  lTcpDynSockTbl_pst;
    TcpIp_IPAddrParamType_tun *      lIPAddrParamType_pun;
    TcpIp_SocketIdType               lIdx_uo;
    Std_ReturnType                   lFunctionRetVal_en;
    Std_ReturnType                   lRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en  = E_NOT_OK;
    lRetVal_en          = E_NOT_OK;

    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_FINDTCPSOCKID , RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if lLocalAddr_st is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (LocalAddr_cpst == NULL_PTR), RBA_ETHTCP_FINDTCPSOCKID , RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );
    /* Report DET if lLocalAddr_st is invalid socket address structure */
    /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( ( ( LocalAddr_cpst != NULL_PTR ) && RBA_ETHTCP_SOCKADDR_HAS_INVALID_IP(LocalAddr_cpst) ),
                                          RBA_ETHTCP_FINDTCPSOCKID , RBA_ETHTCP_E_INV_SOCKADDR, E_NOT_OK );
    /* Report DET if lRemoteAddr_st is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (RemoteAddr_cpst == NULL_PTR), RBA_ETHTCP_FINDTCPSOCKID , RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );
    /* Report DET if SocketIdPtr_puo is a NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (SocketIdPtr_puo == NULL_PTR), RBA_ETHTCP_FINDTCPSOCKID , RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* Loop through all Tcp sockets */
    for ( lIdx_uo = 0U; lIdx_uo < TCPIP_TCPSOCKETMAX; lIdx_uo++ )
    {
        /* Retrieve the local address IP */
        lTcpDynSockTbl_pst  = &(rba_EthTcp_DynSockTbl_ast[lIdx_uo]);
        lRetVal_en          = TcpIp_GetLocalAddrIdProperties( lTcpDynSockTbl_pst->LocalAddrId_u8, &lLocalAddrConfig_pcst );

        if( lRetVal_en == E_OK)
        {
            lIPAddrParamType_pun    = &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un);

           /* Check if Domain, IP and Port values match */
            if ( (lTcpDynSockTbl_pst->DomainType_u32 == (uint16)LocalAddr_cpst->domain)             &&
                 /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                 RBA_ETHTCP_CHECK_SOCKADDR_LOCAL_PORT_EQUAL( lTcpDynSockTbl_pst, LocalAddr_cpst )   &&
                 /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                 RBA_ETHTCP_CHECK_SOCKADDR_REMOTE_PORT_EQUAL( lTcpDynSockTbl_pst, RemoteAddr_cpst ) &&
                 /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                 RBA_ETHTCP_CHECK_SOCKADDR_LOCAL_IP_EQUAL( lIPAddrParamType_pun, LocalAddr_cpst )   &&
                 /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                 RBA_ETHTCP_CHECK_SOCKADDR_REMOTE_IP_EQUAL( &(lTcpDynSockTbl_pst->RemoteIPAddr_un), RemoteAddr_cpst ) )
            {
                ( * SocketIdPtr_puo ) = lIdx_uo;
                lFunctionRetVal_en = E_OK;
                break;
            }
        }
    }

    return lFunctionRetVal_en;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
