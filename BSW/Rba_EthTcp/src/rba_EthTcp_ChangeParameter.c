

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"
static Std_ReturnType rba_EthTcp_ChangeWindowSizeParameter ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             const uint8 *      ParamValue_cpu8 );

#if ( RBA_ETHTCP_NAGLE_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_ChangeNagleParameter      ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             const uint8 *      ParamValue_cpu8 );
#endif

#if ( RBA_ETHTCP_REORDER_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_ChangeOutOfOrderParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#endif

#if ( RBA_ETHTCP_KEEP_ALIVE_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_ChangeKeepAliveParameter  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             TcpIp_ParamIdType  ParamId_u8,
                                                             const uint8 *      ParamValue_cpu8 );
#endif

static Std_ReturnType rba_EthTcp_ChangeFramePrioParameter  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             const uint8 *      ParamValue_cpu8 );

#if(TCPIP_IPV6_ENABLED == STD_ON)
static Std_ReturnType rba_EthTcp_ChangeFlowLabelParameter  ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             const uint8 *      ParamValue_cpu8 );

static Std_ReturnType rba_EthTcp_ChangeDSCPParameter       ( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                             const uint8 *      ParamValue_cpu8 );
#endif

#if(TCPIP_TLS_ENABLED == STD_ON)
static Std_ReturnType rba_EthTcp_ChangeTlsConnectionAssignementParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                                          const uint8 *      ParamValue_cpu8 );

#endif/*  TCPIP_TLS_ENABLED == STD_ON */

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
/** rba_EthTcp_ChangeParameter()    - By this API, the rba_EthTcp module is requested to change a parameter of a socket.        **/
/**                                 E.g. the Nagle algorithm may be controlled by this API.                                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/**  TcpSockId_uo                  - Tcp Socket id                                                                              **/
/**  ParamId_u8                    - Identifier of the parameter to be changed                                                  **/
/**  ParamValue_cpu8               - Pointer to memory containing the new parameter value                                       **/
/**                                                                                                                             **/
/** Parameters (inout):              None                                                                                       **/
/**                                                                                                                             **/
/** Parameters (out):                None                                                                                       **/
/**                                                                                                                             **/
/** Return value:                  - Std_ReturnType                                                                             **/
/**                                  E_OK : The parameter has been changed successfully                                         **/
/**                                  E_NOT_OK : The parameter could not be changed.                                             **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_ChangeParameter( TcpIp_SocketIdType TcpSockId_uo,
                                           TcpIp_ParamIdType  ParamId_u8,
                                           const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_CHANGEPARAMETER, RBA_ETHTCP_E_NOTINIT, E_NOT_OK );
    /* Report DET if TcpSockId_uo is invalid */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (TcpSockId_uo >= TCPIP_TCPSOCKETMAX), RBA_ETHTCP_E_CHANGEPARAMETER, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );
    /* Report DET if ParamValue_cpu8 is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (ParamValue_cpu8 == NULL_PTR), RBA_ETHTCP_E_CHANGEPARAMETER, RBA_ETHTCP_E_NULL_PTR, E_NOT_OK );

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpSockId_uo] ) );

    /* Process only when the socket is not CLOSED and not RESERVED */
    /* (UL does not have access to a socket which is CLOSED or RESERVED) */
    if( (lTcpDynSockTbl_pst->SockState_en != RBA_ETHTCP_SOCK_CLOSED_E) && (lTcpDynSockTbl_pst->SockState_en != RBA_ETHTCP_SOCK_RESERVED_E) )
    {
        switch(ParamId_u8)
        {
            /* ------------------------------------ */
            /* Change the Window size parameter     */
            /* ------------------------------------ */
            case TCPIP_PARAMID_TCP_RXWND_MAX :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeWindowSizeParameter( TcpSockId_uo, ParamValue_cpu8 );
            }
            break;

            /* ------------------------------------ */
            /* Change the Nagle Algorithm parameter */
            /* ------------------------------------ */
            case TCPIP_PARAMID_TCP_NAGLE :
            {
#if ( RBA_ETHTCP_NAGLE_ENABLED == STD_ON )
                lFunctionRetVal_en = rba_EthTcp_ChangeNagleParameter( TcpSockId_uo, ParamValue_cpu8 );
#else
                /* If the ParamValue_cpu8 is STD_OFF  return E_OK for TCPIP_PARAMID_TCP_NAGLE to
                 * avoid reporting of DET when TcpIpTcpNagleEnabled is disabled in Tcp  */
                if(STD_OFF  == (*ParamValue_cpu8))
                {
                    lFunctionRetVal_en = E_OK;
                }
#endif
            }
            break;

            /* ------------------------------------ */
            /* Change the Keep Alive parameter      */
            /* ------------------------------------ */
            case TCPIP_PARAMID_TCP_KEEPALIVE :
            case TCPIP_PARAMID_TCP_KEEPALIVE_TIME :
            case TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX :
            case TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL :
            {
#if ( RBA_ETHTCP_KEEP_ALIVE_ENABLED == STD_ON )
                lFunctionRetVal_en = rba_EthTcp_ChangeKeepAliveParameter( TcpSockId_uo,ParamId_u8, ParamValue_cpu8 );
#endif
            }
            break;

            /* ------------------------------------ */
            /* Change the Frame priority            */
            /* ------------------------------------ */
            case TCPIP_PARAMID_FRAMEPRIO :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeFramePrioParameter( TcpSockId_uo, ParamValue_cpu8 );
            }
            break;

#if( TCPIP_TLS_ENABLED == STD_ON )
            /****************************************************************************************************************/
            /****************************************** [SWS_TCPIP_00307] ***************************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /** In dynamic TLS connection assignment a TLS connection shall be assigned to a TCP socket through a function **/
            /** call to TcpIp_ChangeParameter() with the ParameterId TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT.              **/
            /** The ParameterValue the function provides a reference to a TLS connection for this socket.                  **/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /* ------------------------------------ */
            /* Change the RBA_ETHTLS connection assignment */
            /* ------------------------------------ */
            case TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeTlsConnectionAssignementParameter( TcpSockId_uo, ParamValue_cpu8 );

            }
            break;
#endif/* ( TCPIP_TLS_ENABLED == STD_ON) */

#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
            /* ------------------------------------ */
            /* Change the Out of order Parameter    */
            /* ------------------------------------ */
            case TCPIP_PARAMID_TCP_OUTOFORDER :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeOutOfOrderParameter( TcpSockId_uo );
            }
            break;
#endif

#if(TCPIP_IPV6_ENABLED == STD_ON)
            /* ------------------------------------ */
            /* Change the FLow label                */
            /* ------------------------------------ */
            case TCPIP_PARAMID_FLOWLABEL :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeFlowLabelParameter( TcpSockId_uo, ParamValue_cpu8 );
            }
            break;
            /* ------------------------------------ */
            /* Change the DSCP                      */
            /* ------------------------------------ */
            case TCPIP_PARAMID_DSCP :
            {
                lFunctionRetVal_en = rba_EthTcp_ChangeDSCPParameter( TcpSockId_uo, ParamValue_cpu8 );
            }
            break;
#endif
            /* ------------------------------------ */
            /* Invalid param id                     */
            /* ------------------------------------ */
            default :
            {
                /* nothing to do */
            }
            break;
        }
    }

    /* Report DET error in case the parameter has not been changed successfully */
    /* (this occurs when argument is invalid) */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (lFunctionRetVal_en == E_NOT_OK), RBA_ETHTCP_E_CHANGEPARAMETER, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    return lFunctionRetVal_en;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeWindowSizeParameter  - Execute the changing of the TCPIP_PARAMID_TCP_RXWND_MAX parameter                   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                   - Tcp dynamic socket table index                                                      **/
/** ParamValue_cpu8                       - Pointer to memory containing the new parameter value                                **/
/**                                                                                                                             **/
/** Parameters (inout):                     None                                                                                **/
/**                                                                                                                             **/
/** Parameters (out):                       None                                                                                **/
/**                                                                                                                             **/
/** Return value:                         - Std_ReturnType                                                                      **/
/**                                         E_OK: Parameter is changed                                                          **/
/**                                         E_NOT_OK: Socket is not changed                                                     **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeWindowSizeParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                            const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Check if the Tcp socket is CLOSED or LISTEN state */
    /* (the changing of the default window size is allowed before socket is in communication) */
    /* (when the socket is in communication, it is not possible to change the default window size anymore) */
    if( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSED_E)     ||
        (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LISTEN_E) )
    {
        /* ------------------------------------ */
        /* Change default local window size     */
        /* ------------------------------------ */

        /* Overwrite statically configured default local window size with the value provided by SoAd */
        TcpIp_ReadU16( ParamValue_cpu8, &( lTcpDynSockTbl_pst->TcpSockOpt_pst->SORxWindowMax_u16 ) );

        /* ------------------------------------ */
        /* Change current local window size     */
        /* ------------------------------------ */

        /* Update the initial send window size in the Tcp socket */
        TcpIp_ReadU16( ParamValue_cpu8, &(lTcpDynSockTbl_pst->SndWndSize_u16) );

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeNagleParameter   - Execute the changing of the TCPIP_PARAMID_TCP_NAGLE parameter                           **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo               - Tcp dynamic socket table index                                                          **/
/** ParamValue_cpu8                   - Pointer to memory containing the new parameter value                                    **/
/**                                                                                                                             **/
/** Parameters (inout):                 None                                                                                    **/
/**                                                                                                                             **/
/** Parameters (out):                   None                                                                                    **/
/**                                                                                                                             **/
/** Return value:                     - Std_ReturnType                                                                          **/
/**                                     E_OK: Parameter is changed                                                              **/
/**                                     E_NOT_OK: Socket is not changed                                                         **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_NAGLE_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_ChangeNagleParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                       const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Check if the Param Value is valid */
    if( ((*ParamValue_cpu8) == STD_ON) || ((*ParamValue_cpu8) == STD_OFF) )
    {
        /* Enable/Disable Nagle Algorithm of the socket */
        lTcpDynSockTbl_pst->TcpSockOpt_pst->SONaglesEnad_u8 = (*ParamValue_cpu8);

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}
#endif


/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeKeepAliveParameter - Execute the changing of the TCPIP_PARAMID_TCP_KEEPALIVE parameter                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                 - Tcp dynamic socket table index                                                        **/
/** ParamId_u8                          - Identifier of the parameter to be changed                                             **/
/** ParamValue_cpu8                     - Pointer to memory containing the new parameter value                                  **/
/**                                                                                                                             **/
/** Parameters (inout):                   None                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):                     None                                                                                  **/
/**                                                                                                                             **/
/** Return value:                       - Std_ReturnType                                                                        **/
/**                                       E_OK: Parameter is changed                                                            **/
/**                                       E_NOT_OK: Socket is not changed                                                       **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_KEEP_ALIVE_ENABLED == STD_ON )
static Std_ReturnType rba_EthTcp_ChangeKeepAliveParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                           TcpIp_ParamIdType  ParamId_u8,
                                                           const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    switch(ParamId_u8)
    {
        /* ------------------------------------ */
        /* Change the Keep Alive parameter      */
        /* ------------------------------------ */
        case TCPIP_PARAMID_TCP_KEEPALIVE :

        /* Check if the Param Value is valid */
        if( ((*ParamValue_cpu8) == STD_ON) || ((*ParamValue_cpu8) == STD_OFF) )
        {
            /* -------------------------------------------- */
            /* Enable or disabled Keep Alive for the socket */
            /* -------------------------------------------- */

            /* Enable/Disable Keep Alive feature of the socket */
            lTcpDynSockTbl_pst->TcpSockOpt_pst->SOKeepAlvEnad_u8 = (*ParamValue_cpu8);

            /* ------------------------------------ */
            /* Start stop Keep Alive timer          */
            /* ------------------------------------ */

            /* If Keep Alive is now enabled for this socket */
            if ( lTcpDynSockTbl_pst->TcpSockOpt_pst->SOKeepAlvEnad_u8 == STD_ON )
            {
                /* If we are in a state where the Keep Alive timer is able to run */
                if( (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)     ||
                    (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_1_E)      ||
                    (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_FIN_WAIT_2_E)      ||
                    (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSING_E)         ||
                    (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E)      ||
                    (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_LAST_ACK_E) )
                {
                    /* Start the Keep Alive timer */
                    rba_EthTcp_StartTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_KEEPALIVE_E );
                }
            }
            else
            {
                /* Stop the Keep Alive timer */
                rba_EthTcp_StopTimer( TcpDynSockTblIdx_uo, RBA_ETHTCP_TMR_KEEPALIVE_E );
            }

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------ */
        /* Change the keep alive time           */
        /* ------------------------------------ */
        case TCPIP_PARAMID_TCP_KEEPALIVE_TIME :
        {
           /* Overwrite statically configured maximum KeepAliveTime (KeepAlvTime_u32) with the value provided by SoAd */
            TcpIp_ReadU32( ParamValue_cpu8, &( lTcpDynSockTbl_pst->TcpSockOpt_pst->SOKeepAlvTime_u32 ) );

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------ */
        /* Change the keep alive probes allowed */
        /* ------------------------------------ */
        case TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX :
        {
            /* Overwrite statically configured maximum probes (KeepAlvProbesMax_u16) with the value provided by SoAd */
            TcpIp_ReadU16( ParamValue_cpu8, &( lTcpDynSockTbl_pst->TcpSockOpt_pst->SOKeepAlvProbesMax_u16 ) );

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;

        /* ------------------------------------ */
        /* Change the keep alive interval       */
        /* ------------------------------------ */
        case TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL :
        default :
        {
            /* Overwrite statically configured keepAliveInterVal(KeepAlvIntl_u32) with the value provided by SoAd */
            TcpIp_ReadU32( ParamValue_cpu8, &( lTcpDynSockTbl_pst->TcpSockOpt_pst->SOKeepAlvIntl_u32 ) );

            /* Set the return value of the function to E_OK */
            lFunctionRetVal_en = E_OK;
        }
        break;
    }

    return lFunctionRetVal_en;
}
#endif


/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeFramePrioParameter   - Execute the changing of the TCPIP_PARAMID_TCP_FRAMEPRIO parameter                   **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                   - Tcp dynamic socket table index                                                      **/
/** ParamValue_cpu8                       - Pointer to memory containing the new parameter value                                **/
/**                                                                                                                             **/
/** Parameters (inout):                     None                                                                                **/
/**                                                                                                                             **/
/** Parameters (out):                       None                                                                                **/
/**                                                                                                                             **/
/** Return value:                         - Std_ReturnType                                                                      **/
/**                                         E_OK: Parameter is changed                                                          **/
/**                                         E_NOT_OK: Socket is not changed                                                     **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeFramePrioParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                           const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* Check if the Param Value is valid */
    /* (FramePrio is a 3 bit field in Vlan header, it's maximum value is 7) */
    if( (*ParamValue_cpu8) <= TCPIP_FRAMEPRIO_MAXVAL )
    {
       /* Overwrite statically configured FramePrio (TcpIpIpFramePrioDefault) with the value provided by SoAd */
       lTcpDynSockTbl_pst->SockFramePrio_u8 =  (*ParamValue_cpu8);

       /* Set the return value of the function to E_OK */
       lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}

#if( TCPIP_TLS_ENABLED == STD_ON )
/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeTlsConnectionAssignementParameter    - Execute the changing of the TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT parameter **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/** ParamValue_cpu8                         - Pointer to memory containing the new parameter value                              **/
/**                                                                                                                             **/
/** Parameters (inout):                     None                                                                                **/
/**                                                                                                                             **/
/** Parameters (out):                       None                                                                                **/
/**                                                                                                                             **/
/** Return value:                           - Std_ReturnType                                                                    **/
/**                                             E_OK: Parameter is changed                                                      **/
/**                                             E_NOT_OK: parameter is not changed                                                 **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeTlsConnectionAssignementParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo, \
                                                                          const uint8 *  	 ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst  * lTcpDynSockTbl_pst;

    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;

    const TcpIp_IPAddrParamType_tun  * lIPAddrParamType_cpun;

    rba_EthTls_ConnectionType_ten    lConnectionType_en;

    TcpIp_TlsConnectionIdType        lTlsConnId_uo;

#if(TCPIP_SIZEOF_TLS_CONN_ID_TYPE == 2U)
    uint16                           lTlsConnId_u16;
#endif

    uint16                           lFreeLen_u16;

    Std_ReturnType                   lFunctionRetVal_en;

    Std_ReturnType                   lRetVal_en;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    lFunctionRetVal_en = E_NOT_OK;

    /* TcpIp_TlsConnectionIdType can be uint8 or uint16 .so check value of ParamValue_cpu8 is in range uint16 or uint8 */
#if(TCPIP_SIZEOF_TLS_CONN_ID_TYPE == 2U)

    /* assign the rba_Ethtls connection id in local variable */
    TcpIp_ReadU16( ParamValue_cpu8, &lTlsConnId_u16 );

    lTlsConnId_uo = lTlsConnId_u16;

#else

    /* assign the rba_Ethtls connection id in local variable */
    lTlsConnId_uo = ParamValue_cpu8[0];

#endif

    /* If change parameter for Tcp connection is already requested*/
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (lTcpDynSockTbl_pst->TlsConnectionId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE),RBA_ETHTCP_E_CHANGEPARAMETER, TCPIP_E_INV_ARG, E_NOT_OK );

    /* Check If Tcp handshake is completed but Open Tls connection is not requested*/
    if(lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E)
    {
        /* Call  rba_EthTls_GetConnectionType to get connection type*/
        lRetVal_en = rba_EthTls_GetConnectionType( lTlsConnId_uo, &lConnectionType_en );

        /* Report DET if Connection id is invalid */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL( ( E_NOT_OK == lRetVal_en ),RBA_ETHTCP_E_CHANGEPARAMETER, TCPIP_E_INV_ARG, E_NOT_OK );

        /* Report DET if lConnectionType_en is server when tcp socket is client(Listen socket id is invalid) */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL( \
                ( ( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID == lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) && ( RBA_ETHTLS_SERVER == lConnectionType_en ) ),\
                    RBA_ETHTCP_E_CHANGEPARAMETER, TCPIP_E_INV_ARG, E_NOT_OK );

        /* Report DET if lConnectionType_en is client when tcp socket is server(Listen socket id is valid) */
        RBA_ETHTCP_CHECK_DETERROR_RETVAL( \
                ( ( RBA_ETHTCP_DEAFULT_LISTEN_SOCKET_ID != lTcpDynSockTbl_pst->ListenTcpSockIdx_uo ) && ( RBA_ETHTLS_CLIENT == lConnectionType_en ) ),\
                    RBA_ETHTCP_E_CHANGEPARAMETER, TCPIP_E_INV_ARG, E_NOT_OK );

        if(lConnectionType_en == RBA_ETHTLS_SERVER)
        {
            /* fetch the local address*/
            lRetVal_en = TcpIp_GetLocalAddrIdProperties( lTcpDynSockTbl_pst->LocalAddrId_u8, &lLocalAddrConfig_pcst );

            if( lRetVal_en == E_OK )
            {
                /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 or IPv6 specific data. */
                lIPAddrParamType_cpun        = &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un);

                /* check if connection refers to secure tls connection */
                /* If connection is secure then valid Tls connection id will be updated in this function*/
                lRetVal_en = rba_EthTcp_CheckSecureServerConnection( lTcpDynSockTbl_pst, lIPAddrParamType_cpun, lTlsConnId_uo );
            }

        }
        else
        {
            /* check if connection refers to secure tls connection */
            lRetVal_en = rba_EthTcp_CheckSecureClientConnection( lTcpDynSockTbl_pst, &lTcpDynSockTbl_pst->RemoteIPAddr_un, lTlsConnId_uo );

        }

        /* check If requested Tls conn id matches with Tls conn id returned fron Tls module*/
        if(lRetVal_en == E_OK)
        {

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
            /* Get the tcp tx buffer free length*/
            lFreeLen_u16 = rba_EthTcp_GetTxBufFreeLen(TcpDynSockTblIdx_uo);

            /* check if Tcp Tx buffer has any data*/
            if(lFreeLen_u16 != RBA_ETHTCP_TXWNDSIZE)
            {
                /*store the Tls connection id.The stored id is used to trigger Tls handshke in main function once the Tcp Tx buffer is empty.
                 * This is required so that the remaining data in Tcp buffer is transmitted before TLS secure connection is established */
                lTcpDynSockTbl_pst->TlsRequestedConnId_uo = lTlsConnId_uo;

                lFunctionRetVal_en = E_OK;
            }
            else
#endif
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_TCPIP_00337] ***************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** For dynamic TLS connection assignment via TcpIp_ChangeParameter(), the call to TcpIp_ChangeParameter()     **/
                /** shall initiate the TLS handshake as follows:                                                               **/
                /**   1. a TLS Server shall wait for a ClientHello as the next message on this socket.                         **/
                /**   2. a TLS Client shall start sending a ClientHello message.                                               **/
                /**   3. after that TcpIp shall no longer pass on plain messages to upper or lower layer but pass it on to TLS.**/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                 /* If there is not data in Tcp Tx buffer trigger the rba_EthTls handshake sequence by calling TcpIp_TlsOpenConnection API */
                 lFunctionRetVal_en = rba_EthTls_OpenConnection(TcpDynSockTblIdx_uo, &lTlsConnId_uo);

                 /* If rba_EthTls open connection is successful */
                 if(E_OK == lFunctionRetVal_en )
                 {
                     /* assign the rba_Ethtls connection id in dynamic table for future access */
                     lTcpDynSockTbl_pst->TlsConnectionId_uo =  lTlsConnId_uo;

                 }
            }/* check if Tcp Tx buffer has any data*/
        }

    }
    /* If connection is in listen/Syn Rcvd/Syn Sent state */
    else if(lTcpDynSockTbl_pst->State_en < RBA_ETHTCP_CONN_ESTABLISHED_E)
    {
        /* assign the rba_Ethtls connection id in dynamic table for future access */
        lTcpDynSockTbl_pst->TlsConnectionId_uo =  lTlsConnId_uo;

        lFunctionRetVal_en = E_OK;

    }
    else
    {
        /* do nothing */
    }

    return lFunctionRetVal_en;

}
#endif/* ( TCPIP_TLS_ENABLED == STD_ON) */


#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeOutOfOrderParameter     - Execute the changing of the TCPIP_PARAMID_TCP_OUTOFORDER parameter               **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                      - Tcp dynamic socket table index                                                   **/
/**                                                                                                                             **/
/** Parameters (inout):                        None                                                                             **/
/**                                                                                                                             **/
/** Parameters (out):                          None                                                                             **/
/**                                                                                                                             **/
/** Return value:                             - Std_ReturnType                                                                  **/
/**                                             E_OK: Parameter is changed                                                      **/
/**                                             E_NOT_OK: Parameter is not changed                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeOutOfOrderParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo)
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /*Set out of order value to be true for the particular socket */
    lTcpDynSockTbl_pst->OutOfOrder_b = TRUE;

    /*Set lFunctionRetVal_en to E_OK */
    lFunctionRetVal_en = E_OK;

  return lFunctionRetVal_en;
}
#endif

#if(TCPIP_IPV6_ENABLED == STD_ON)
/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeFlowLabelParameter     - Execute the changing of the TCPIP_PARAMID_FLOWLABEL parameter                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/** ParamValue_cpu8                         - Pointer to memory containing the new parameter value                              **/
/**                                                                                                                             **/
/** Parameters (inout):                     None                                                                                **/
/**                                                                                                                             **/
/** Parameters (out):                       None                                                                                **/
/**                                                                                                                             **/
/** Return value:                           - Std_ReturnType                                                                    **/
/**                                             E_OK: Parameter is changed                                                      **/
/**                                             E_NOT_OK: Socket is not changed                                                 **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeFlowLabelParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                           const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;
    uint32                          lFlowLabelVal_u32;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Read the value of flow label */
    TcpIp_ReadU32(ParamValue_cpu8, &lFlowLabelVal_u32);

    /* Check if the Param Value is valid */
    if(lFlowLabelVal_u32 <= TCPIP_FLOWLABEL_MAXVAL)
    {
        /* Get Tcp dynamic table entry at index lIdx_u8 */
        lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

        /* Overwrite statically configured Flow Label (TcpIpIpFlowLabelDefault) with the value provided by SoAd */
        lTcpDynSockTbl_pst->SockFlowLabel_u32 = lFlowLabelVal_u32;

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}

/*********************************************************************************************************************************/
/** rba_EthTcp_ChangeDSCPParameter     - Execute the changing of the TCPIP_PARAMID_DSCP parameter                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo                     - Tcp dynamic socket table index                                                    **/
/** ParamValue_cpu8                         - Pointer to memory containing the new parameter value                              **/
/**                                                                                                                             **/
/** Parameters (inout):                     None                                                                                **/
/**                                                                                                                             **/
/** Parameters (out):                       None                                                                                **/
/**                                                                                                                             **/
/** Return value:                           - Std_ReturnType                                                                    **/
/**                                             E_OK: Parameter is changed                                                      **/
/**                                             E_NOT_OK: Socket is not changed                                                 **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_ChangeDSCPParameter( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                                      const uint8 *      ParamValue_cpu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    Std_ReturnType                  lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check if the Param Value is valid */
    if((*ParamValue_cpu8) <= TCPIP_DSCP_MAXVAL)
    {
        /* Get Tcp dynamic table entry at index lIdx_u8 */
        lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

        /* Overwrite statically configured DSCP  with the value provided by SoAd */
        lTcpDynSockTbl_pst->SockDscp_u8 = (*ParamValue_cpu8);

        /* Set the return value of the function to E_OK */
        lFunctionRetVal_en = E_OK;
    }

    return lFunctionRetVal_en;
}
#endif

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */

