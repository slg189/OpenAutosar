
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"
#include "rba_EthUdp_Cfg_SchM.h"
#include "TcpIp_Prv.h"
#include "TcpIp_Lib.h"
#include "rba_EthUdp_Prv.h"

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/*
***************************************************************************************************
* Static function declaration
***************************************************************************************************
*/

static void rba_EthUdp_GetPortAny( uint16 * Port_pu16);

static boolean rba_EthUdp_CheckLocalAddrInUse( TcpIp_SocketIdType SocketId_u16,
                                               TcpIp_LocalAddrIdType LocalAddrId_u8,
                                               boolean WildCardPort_b,
                                               uint16 * Port_pu16 );


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************/
/* rba_EthUdp_GetSocket() -  By this API service the rba_EthUdp is requested to allocate a new socket.               */
/*                                                                                                                   */
/* Synchronous, Reentrant (from [SWS_TcpIp_00018])                                                                   */
/*                                                                                                                   */
/* Input Parameters :                                                                                                */
/* Domain_u32           - IP address family.                                                                         */
/* SocketOwner_u8       - Specifies the upper layer which has requested to allocate a new socket.                    */
/*                                                                                                                   */
/* Output Parameters :                                                                                               */
/* SocketIdPtr_pu16     - Pointer to socket identifier representing the requested socket.                            */
/*                                                                                                                   */
/* Std_ReturnType       - E_OK or E_NOT_OK - Result of operation                                                     */
/*********************************************************************************************************************/
Std_ReturnType rba_EthUdp_GetSocket( TcpIp_DomainType Domain_u32,
                                     TcpIp_SocketIdType * SocketIdPtr_pu16,
                                     uint8 SocketOwner_u8 )
{
    /* Local variable declaration */
    TcpIp_SocketIdType         lSocIdx_u16;
    Std_ReturnType             lRetValStdType_en;

    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Report DET if rba_EthUdp module is uninitialized */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_GET_SOCKET_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Report DET if requested domain type is not TCPIP_AF_INET (IPv4) or TCPIP_AF_INET6 (IPv6).*/
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ((TCPIP_AF_INET != Domain_u32) && (TCPIP_AF_INET6 != Domain_u32)) , RBA_ETHUDP_E_GET_SOCKET_API_ID, RBA_ETHUDP_E_AFNOSUPPORT, E_NOT_OK )

    /* Report DET if SocketIdPtr is a Null Pointer */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == SocketIdPtr_pu16 ), RBA_ETHUDP_E_GET_SOCKET_API_ID, RBA_ETHUDP_E_NULL_PTR, E_NOT_OK )

    /* Check if any Udp socket is available */
    if(rba_EthUdp_UsedSocCnt_u16 < TCPIP_UDPSOCKETMAX)
    {
        /* Loop through all sockets to find free socket */
        for(lSocIdx_u16 = 0; lSocIdx_u16 < TCPIP_UDPSOCKETMAX; lSocIdx_u16++)
        {
            /* Enter critical section (to avoid race condition when rba_EthUdp_GetSocket is reentered) */
            /* (Two socket owners shall not receive the same allocated socket id in case GetSocket is reentered) */
            SchM_Enter_rba_EthUdp_ExclusiveAreaSockState();

            /* If current socket is CLOSED */
            if( rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].SockState_en == RBA_ETHUDP_SOCK_CLOSED )
            {
                /* Update socket properties - Domain and Socket owner */
                rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].DomainType_u32 = Domain_u32;
                rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].UL_SockOwnerIdx_u8 = SocketOwner_u8;

                /* Enter critical section */
                /* (Incrementation is not an atomic operation and need to be done in a critical section) */
                SchM_Enter_rba_EthUdp_ExclusiveAreaUsedSocCnt();

                /* Increment used socket count */
                /* This instruction need to be executed before the socket is set to OPEN state in order to prevent out of boundary value for rba_EthUdp_UsedSocCnt_u16 in case all sockets are closed concurently */
                rba_EthUdp_UsedSocCnt_u16++;

                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaUsedSocCnt();

                /* Socket is now OPEN */
                rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].SockState_en  = RBA_ETHUDP_SOCK_OPEN;

                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaSockState();

                /* Set out argument and return E_OK */
                *SocketIdPtr_pu16 = lSocIdx_u16;
                lRetValStdType_en = E_OK;
                break;
            }
            else
            {
                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaSockState();
            }
        }
    }

    return lRetValStdType_en;
}


/********************************************************************************************************************/
/* rba_EthUdp_Bind() - By this API rba_EthUdp is requested to bind a UDP socket to a local resource                 */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId (from [SWS_TcpIp_00015])     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16     - Socket identifier of the related local socket resource.                                       */
/* LocalAddrId_u8   - IP address identifier representing the local IP address to be bound to the socket             */
/* FramePrio_u8     - Frame priority to be set the socket                                                           */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/* Port_pu16           - Local port to which the socket shall be bound. In case the parameter Port is specified as  */
/*                        TCPIP_PORT_ANY, the TCP/IP stack shall choose the local port automatically from the range */
/*                        49152 to 65535 and shall update the parameter Port to the chosen value.                   */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* Std_ReturnType      - E_OK or E_NOT_OK - Result of operation                                                     */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_Bind( TcpIp_SocketIdType SocketId_u16,
                                TcpIp_LocalAddrIdType LocalAddrId_u8,
                                uint16 * Port_pu16,
                                uint8 FramePrio_u8)
{
    /* Local variable declaration */
    Std_ReturnType                              lRetValStdType_en;
    uint8                                       lEthIfCtrl_u8;
    boolean                                     lWildCardPort_b;
    boolean                                     lAddrInUse_b;
    Std_ReturnType                              lOperationRetVal_en;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_BIND_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Report DET if SocketId is not valid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (TCPIP_UDPSOCKETMAX <= SocketId_u16 ), RBA_ETHUDP_E_BIND_API_ID, RBA_ETHUDP_E_INV_ARG, E_NOT_OK )

    /* Report DET if Port_pu16 is a Null Pointer */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == Port_pu16 ), RBA_ETHUDP_E_BIND_API_ID, RBA_ETHUDP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) <= LocalAddrId_u8 ), RBA_ETHUDP_E_BIND_API_ID , RBA_ETHUDP_E_INV_ARG, E_NOT_OK )

    /* Local Variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Check if Bind operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the Bind operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Get EthIfCtrlIdx mapped to the localAddress */
        lEthIfCtrl_u8 = TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].EthIfCtrlRef_u8;

        /* Check whether the LocalAddress has IP assigned */
        lRetValStdType_en = TcpIp_CheckValidityOfLocalAddrId( LocalAddrId_u8 );

        /* Report DET if domain associated with Socket table and LocalAddrId is not same. This also ensures that socket state is either RBA_ETHUDP_SOCK_OPEN or RBA_ETHUDP_SOCK_BOUND */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( (rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 != TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].IpDomainType_u32 ),
                                                RBA_ETHUDP_E_BIND_API_ID , RBA_ETHUDP_E_INV_ARG )

        /* Raise DET if local address id is invalid. For Dhcp sockets binding is allowed even if IP address is unassigned */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( ( ( E_NOT_OK == lRetValStdType_en ) && ( TCPIP_DHCPCLIENT_PORT != *(Port_pu16) )),
                                                RBA_ETHUDP_E_BIND_API_ID , RBA_ETHUDP_E_ADDRNOTAVAIL )

        if( (RBA_ETHUDP_SOCK_OPEN == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en) &&                               /* Check whether the socket state OPEN */
            (rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 == TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].IpDomainType_u32) && /* Domains are matching */
            (((E_OK  == lRetValStdType_en) && (TCPIP_STATE_ONLINE == TcpIp_EthIfCtrlSt_aen[lEthIfCtrl_u8]))||               /* For Non-Dhcp sockets binding localAddress must have IP assigned and Ctrl state shall be TCPIP_STATE_ONLINE */
             ((TCPIP_DHCPCLIENT_PORT == *Port_pu16) && (TCPIP_STATE_STARTUP == TcpIp_EthIfCtrlSt_aen[lEthIfCtrl_u8]))) )    /* For Dhcp sockets binding is allowed even if the TcpIp Ctrl state is STARTUP */
        {
            /* In case the parameter is specified as TCPIP_PORT_ANY, the TCP/IP stack shall choose the local port automatically */
            /* from the range 49152(0xC000) to 65535(0xFFFF) and shall update the parameter to the chosen value                 */
            if( TCPIP_PORT_ANY == *Port_pu16 )
            {
                /* Function call to choose the local port automatically when the parameter is specified as TCPIP_PORT_ANY */
                rba_EthUdp_GetPortAny( Port_pu16 );

                /* Set flag indicating PORT_ANY to true */
                lWildCardPort_b = TRUE;
            }
            else
            {
                /* Set flag indicating PORT_ANY to false */
                lWildCardPort_b = FALSE;
            }

            /* Enter critical section (to avoid race condition if rba_EthUdp_Bind API is called in the same time for another socket id) */
            /* (It need to be ensured that the verification of the local address in use and the writing of the local address in the dynamic socket table are executed consistently) */
            SchM_Enter_rba_EthUdp_ExclusiveAreaLocalAddrInUse();

            /* Check if local address is in use */
            lAddrInUse_b = rba_EthUdp_CheckLocalAddrInUse( SocketId_u16, LocalAddrId_u8, lWildCardPort_b, Port_pu16 );

            if( lAddrInUse_b == FALSE )
            {
                /* Bind the socket with local address information and set the state to RBA_ETHUDP_SOCK_BOUND */
                rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8 = LocalAddrId_u8;
                rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16 = *Port_pu16;

                /* Now that the socket is bound, change the socket state */
                rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en = RBA_ETHUDP_SOCK_BOUND;

                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaLocalAddrInUse();

                /* Enter critical section */
                /* To ensure that socket frame priority is consistently updated when rba_EthUdp_ChangeParameter() is called in parallel */
                SchM_Enter_rba_EthUdp_ExclusiveAreaSockFramePrio();

                /* Update FramePrio value if not updated previously by rba_EthUdp_ChangeParameter() */
                if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8 == 0xFFu )
                {
                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8 = FramePrio_u8;
                }

                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaSockFramePrio();

                lRetValStdType_en = E_OK;
            }
            else
            {
                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaLocalAddrInUse();

                /* Report DET as local addrId and port combination is already in use */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_BIND_API_ID , RBA_ETHUDP_E_ADDRINUSE )
                lRetValStdType_en = E_NOT_OK;
            }
        }

        /* Check whether the bind is requested for the socket which is aready in state RBA_ETHUDP_SOCK_BOUND */
        else if( RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en )
        {
            /* If bind is requested for the socket which is aready in state RBA_ETHUDP_SOCK_BOUND and  */
            /* requested socket properties are same as current assigned properties return with E_OK */
            if( (LocalAddrId_u8 == rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8) &&
                (*Port_pu16 == rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16) &&
                (FramePrio_u8 == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8) )
            {
                lRetValStdType_en = E_OK;
            }

            /* If requested socket properties are different than current assigned rpoperties throw DET */
            else
            {
                /* Report DET if rba_EthUdp_Bind() is called for the socket which is already in state RBA_ETHUDP_SOCK_BOUND */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_BIND_API_ID , RBA_ETHUDP_E_INV_ARG )
                lRetValStdType_en = E_NOT_OK;
            }
        }

        /* Socket is not in correct state */
        else
        {
            lRetValStdType_en = E_NOT_OK;
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }
    else
    {
        /* Bind operation is not allowed for execution, E_NOT_OK will be returned */
    }

    return lRetValStdType_en;
}


/********************************************************************************************************************/
/* rba_EthUdp_ChangeParameter() By this API service the rba_EthUdp is requested to change a parameter of a socket   */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId (from [SWS_TcpIp_00016])     */
/*                                                                                                                  */
/* Parameter IN:                                                                                                    */
/*  SocketId_u16            - Socket identifier of the related local socket resource.                               */
/*  ParameterId_en          - Identifier of the parameter to be changed                                             */
/*  ParameterValue_u8       - Pointer to memory containing the new parameter value                                  */
/*                                                                                                                  */
/* Std_ReturnType       Result of operation                                                                         */
/*      E_OK                The parameter has been changed successfully                                             */
/*      E_NOT_OK            The parameter could not be changed.                                                     */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_ChangeParameter( TcpIp_SocketIdType SocketId_u16,
                                           TcpIp_ParamIdType ParameterId_en,
                                           const uint8 * ParameterValue_pcu8 )
{
    /* Local Variable declaration */
    Std_ReturnType                              lRetValStdType_en;
    Std_ReturnType                              lOperationRetVal_en;
#if ( TCPIP_IPV6_ENABLED == STD_ON )
    uint32                                      lFlowLabel_u32;
#endif

    /* Report DET if rba_EthUdp is not initialized */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_CHANGE_PARAMETER_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Local Variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Check if ChangeParameter operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the ChangeParameter operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Check the parameter to be changed  */
        switch ( ParameterId_en )
        {
            case TCPIP_PARAMID_FRAMEPRIO:
            {
                if( (*ParameterValue_pcu8) <= TCPIP_FRAMEPRIO_MAXVAL )
                {
                    /* Enter critical section */
                    /* To ensure that socket frame priority is consistently updated when rba_EthUdp_Bind() is called in parallel */
                    SchM_Enter_rba_EthUdp_ExclusiveAreaSockFramePrio();

                    /* Update frame priority value in the socket structure */
                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8 = *ParameterValue_pcu8;

                    /* Exit critical section */
                    SchM_Exit_rba_EthUdp_ExclusiveAreaSockFramePrio();

                    lRetValStdType_en = E_OK;
                }
            }
            break;
#if ( TCPIP_IPV6_ENABLED == STD_ON )
            case TCPIP_PARAMID_FLOWLABEL:
            {
                lFlowLabel_u32 = 0UL;

                /* Read the value of flow label */
                TcpIp_ReadU32(ParameterValue_pcu8, &lFlowLabel_u32);

                /* Check if the vlaue of the flow label is in the valid range */
                if( lFlowLabel_u32 <= TCPIP_FLOWLABEL_MAXVAL )
                {
                    /* Update Flow label value in the socket structure */
                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFlowLabel_u32 = lFlowLabel_u32;

                    lRetValStdType_en = E_OK;
                }
            }
            break;
#endif
            case TCPIP_PARAMID_DSCP:
            {
                /* Check if the value of the DSCP field is in the valid range. */
                if( *ParameterValue_pcu8 <= TCPIP_DSCP_MAXVAL )
                {
                    /* Update DSCP value in the socket structure */
                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockDiffServiceCodePoint_u8 = *ParameterValue_pcu8;

                    lRetValStdType_en = E_OK;
                }
            }
            break;
#if( TCPIP_IPV4_ENABLED == STD_ON )
            case TCPIP_PARAMID_UDP_CHECKSUM:
            {
                if( ((*ParameterValue_pcu8) == STD_ON) || ((*ParameterValue_pcu8) == STD_OFF) )
                {
                    /* Update the checksum enabling value in the socket structure */
                    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8 = *ParameterValue_pcu8;
                    lRetValStdType_en = E_OK;
                }
            }
            break;
#endif
            default:
            {
                lRetValStdType_en = E_NOT_OK;
            }
            break;
        }

        /* Report DET if the parameter ID is not accepted or if the parameter value is not within the valid range */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET ((lRetValStdType_en != E_OK), RBA_ETHUDP_E_CHANGE_PARAMETER_API_ID, RBA_ETHUDP_E_INV_ARG);

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }
    else
    {
        /* ChangeParameter operation is not allowed for execution, E_NOT_OK will be returned */
    }

    return lRetValStdType_en;
}

/********************************************************************************************************************/
/* rba_EthUdp_GetSocketInfo()  - It returns the Local Addr Id and local port mapped to the socket Id                */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters:                                                                                                */
/* SocketId_u16                - Socket identifier of the related local socket resource.                            */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/* LocalAddrId_pu8             - Local Addr Id to which the socket is bound                                         */
/* LocalPort_pu16              - Local port of the socket.                                                          */
/*                                                                                                                  */
/* Return type :               - Std_ReturnType                                                                     */
/*                               E_OK       Local Address Id mapped to the socket Id                                */
/*                               E_NOT_OK   Local Address Id not mapped to the socket Id                            */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_GetSocketInfo( TcpIp_SocketIdType SocketId_u16,
                                         TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                         uint16 * LocalPort_pu16 )
{
    /* Local variable declaration */
    Std_ReturnType                              lRetValStdType_en;
    Std_ReturnType                              lOperationRetVal_en;

    /* Report DET if rba_EthUdp is not initialized */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_GET_SOCKET_INFO_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Report DET if SocketId is not valid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (TCPIP_UDPSOCKETMAX <= SocketId_u16 ), RBA_ETHUDP_E_GET_SOCKET_INFO_API_ID, RBA_ETHUDP_E_INV_ARG, E_NOT_OK )

    /* Report DET if LocalAddrId_pu8 is a Null Pointer */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == LocalAddrId_pu8 ), RBA_ETHUDP_E_GET_SOCKET_INFO_API_ID, RBA_ETHUDP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if LocalPort_pu16 is a Null Pointer */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == LocalPort_pu16 ), RBA_ETHUDP_E_GET_SOCKET_INFO_API_ID, RBA_ETHUDP_E_NULL_PTR, E_NOT_OK )

    /* Local Variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    if( lOperationRetVal_en == E_OK )
    {
        if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en == RBA_ETHUDP_SOCK_BOUND )
        {
            ( * LocalAddrId_pu8 ) = rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8;

            ( * LocalPort_pu16 )  = rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16;

            lRetValStdType_en = E_OK;
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }
    return lRetValStdType_en;
}

/********************************************************************************************************************/
/* rba_EthUdp_Close     -By this API service rba_EthUdp stack is requested to close the socket                      */
/*                                                                                                                  */
/* Asynchronous, Reentrant                                                                                          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16         - Socket identifier of the related local socket resource.                                   */
/*                                                                                                                  */
/* Return type :        void                                                                                        */
/********************************************************************************************************************/
void rba_EthUdp_Close( TcpIp_SocketIdType SocketId_u16 )
{
    /* Local variable declaration */
    Std_ReturnType             lOperationRetVal_en;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( (NULL_PTR == rba_EthUdp_Config_pco), RBA_ETHUDP_E_CLOSE_API_ID, RBA_ETHUDP_E_NOTINIT )

    /* Report DET if SocketId_u16 is invalid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( (TCPIP_UDPSOCKETMAX <= SocketId_u16 ), RBA_ETHUDP_E_CLOSE_API_ID, RBA_ETHUDP_E_INV_ARG )

    /* Enter CLOSING state and check if resources can be released now */
    /* No other operation on the socket should be done in the same execution context in order to close the socket now */
    lOperationRetVal_en = rba_EthUdp_IsCloseOperationAllowed( SocketId_u16 );

    /* Check whether the socket closing is allowed to be executed now */
    if( lOperationRetVal_en == E_OK )
    {
        /* Inform upper layer about closing of socket, if Up_TcpIpEvent() API is configured for the socket owner. */
        if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct != NULL_PTR )
        {
            TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct( TCPIP_UDP_SOCKET_OFFSET + SocketId_u16,
                                                                                                                                        TCPIP_UDP_CLOSED );
        }

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
        /* Call TcpIp callback to close the ZeroCopy descriptor */
        /* Required if the Udp socket was used by at least one ZeroCopy transmission to release the allocated TcpIp descriptor */
        TcpIp_ZeroCopyReset( (SocketId_u16 + TCPIP_UDP_SOCKET_OFFSET) );
#endif

        /* Enter critical section */
        /* (Decrementation is not an atomic operation and need to be done in a critical section) */
        SchM_Enter_rba_EthUdp_ExclusiveAreaUsedSocCnt();

        /* Decrement used socket counter by 1 */
        /* This instruction need to be executed before the socket is set to CLOSED state in order to out of boundary value for rba_EthUdp_UsedSocCnt_u16 in case get socket closing is executed concurently */
        rba_EthUdp_UsedSocCnt_u16--;

        /* Enter critical section */
        SchM_Exit_rba_EthUdp_ExclusiveAreaUsedSocCnt();

        /* Reset socket properties to initial value */
        rba_EthUdp_ResetSockProperties(SocketId_u16);
    }
}


/********************************************************************************************************************/
/* rba_EthUdp_TerminateSocUsingLocalAddr - This API closes all Udp sockets excpt Dhcp client if input parameter     */
/*                LocalAddrId_u8 is TCPIP_LOCALADDRID_INVALID else closes only sockets mapped to the LocalAddrId_u8 */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8      -  Local adress for whcih sockets needs to be closed                                         */
/*                                                                                                                  */
/* Return type :        void                                                                                        */
/********************************************************************************************************************/
void rba_EthUdp_TerminateSocUsingLocalAddr( TcpIp_LocalAddrIdType LocalAddrId_u8 )
{
    /* Local Variable declaration */
    TcpIp_SocketIdType                     lSocIdx_u16;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( (NULL_PTR == rba_EthUdp_Config_pco), RBA_ETHUDP_E_TERMINATE_SOC_USING_LOCALADDR_API_ID, RBA_ETHUDP_E_NOTINIT )

    /* Loop through Udp socket table and close all the sockets mapped to the requested localAddress */
    for(lSocIdx_u16 = 0; lSocIdx_u16 < TCPIP_UDPSOCKETMAX; lSocIdx_u16++)
    {
        if( (rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].SockState_en != RBA_ETHUDP_SOCK_CLOSED) &&
            ((LocalAddrId_u8 == rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].LocalAddrId_u8 ) || (LocalAddrId_u8 == TCPIP_LOCALADDRID_INVALID)) &&
            (TCPIP_DHCPCLIENT_PORT != rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].LocalPort_u16) )
        {
            rba_EthUdp_Close(lSocIdx_u16);
        }
    }
}


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* rba_EthUdp_ResetSockProperties - Reset all the fields of a UDP socket to default values                          */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16         - Socket identifier of the related local socket resource.                                   */
/*                                                                                                                  */
/* Return type :        void                                                                                        */
/********************************************************************************************************************/
void rba_EthUdp_ResetSockProperties( TcpIp_SocketIdType SocketId_u16 )
{
    /* Reset all fields of a socket */
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32      = TCPIP_AF_NONE;
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8      = (TcpIp_LocalAddrIdType) TCPIP_LOCALADDRID_INVALID;
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16       = TCPIP_PORT_ANY;
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8    = 0xFFu;
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8  = 0xFFu;
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8 = 0U;
    /* varible to optionally control whether a UDP checksum will be generated, but it is default to checksumming on*/
#if( TCPIP_IPV4_ENABLED == STD_ON )
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8    = STD_ON;
#endif
#if ( TCPIP_IPV6_ENABLED == STD_ON )
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFlowLabel_u32            = 0UL;
#endif
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockDiffServiceCodePoint_u8  = 0U;

    /* Now that all the fields are reset, the socket state can be changed to CLOSED */
    /* This statement need to be the last one so that the socket will be assumed as closed only when all the fields are reset */
    rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en = RBA_ETHUDP_SOCK_CLOSED;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* rba_EthUdp_GetPortAny - This API assigns a port if the parameter port is specified as TCPIP_PORT_ANY, the TCP/IP */
/*                         stack shall choose the local port automatically from the range 49152 to 65535 and shall  */
/*                         update the parameter Port to the chosen value                                            */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* Port_pu16             -  Local port to which the socket shall be bound.                                          */
/*                                                                                                                  */
/* Return type :         void                                                                                       */
/********************************************************************************************************************/
static void rba_EthUdp_GetPortAny( uint16 * Port_pu16 )
{
    /* Enter critical section */
    /* (Incrementation is not an atomic operation and need to be done in a critical section) */
    SchM_Enter_rba_EthUdp_ExclusiveAreaPortAny();

    /* If assigned port in PORT_ANY range is already in use, increment rba_EthUdp_PortAny_u16 and assign the new port number */
    *Port_pu16 = rba_EthUdp_PortAny_u16;

    /* If rba_EthUdp_PortAny_u16 >= 65535, then set rba_EthUdp_PortAny_u16 to 49152 (0xC000) */
    if ( rba_EthUdp_PortAny_u16 >= TCPIP_LOCAL_PORT_ANY_END )
    {
        rba_EthUdp_PortAny_u16 = TCPIP_LOCAL_PORT_ANY_START;
    }

    /* If rba_EthUdp_PortAny_u16 is lower than 65535, simply increment */
    else
    {
        rba_EthUdp_PortAny_u16++;
    }

    /* Enter critical section */
    SchM_Exit_rba_EthUdp_ExclusiveAreaPortAny();
}


/********************************************************************************************************************/
/* rba_EthUdp_CheckLocalAddrInUse - Check if the local address is currently in use by any other socket. If the      */
/*                       local address is already in use and if WildCardPort_b is TRUE, a new port will be selected */
/*                       and local address checking will be done again                                              */
/*                                                                                                                  */
/* Synchronous, Non-Reentrant                                                                                       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16         - Socket identifier of the related local socket resource.                                   */
/* LocalAddrId_u8       - Local adress of the socket which shall be checked.                                        */
/* WildCardPort_b       - Flag indicating if rba_EthUdp_Bind was called with TCPIP_PORT_ANY                         */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/* Port_pu16            - Local port to which the socket shall be bound. In case the parameter Port is specified as */
/*                        TCPIP_PORT_ANY, the TCP/IP stack shall choose the local port automatically from the range */
/*                        49152 to 65535 and shall update the parameter Port to the chosen value.                   */
/*                                                                                                                  */
/* Boolean              Result of operation                                                                         */
/*      TRUE                Local address is not already in use by another socket                                   */
/*      FALSE               Local address is already in use                                                         */
/********************************************************************************************************************/
static boolean rba_EthUdp_CheckLocalAddrInUse( TcpIp_SocketIdType SocketId_u16,
                                               TcpIp_LocalAddrIdType LocalAddrId_u8,
                                               boolean WildCardPort_b,
                                               uint16 * Port_pu16 )
{
    /* Local variable declaration */
    TcpIp_SocketIdType          lSocIdx_u16;
    boolean                     lAddrInUse_b;

    /* Local variable initialization */
    lSocIdx_u16 = 0U;
    lAddrInUse_b = FALSE;

    /* Check if the local addrId and port combination passed as input parameters, is already bound by another UDP socket */
    while( lSocIdx_u16 < TCPIP_UDPSOCKETMAX )
    {
        /* The check is valid for unicast IP address and it should not be the same socket */
        if( (SocketId_u16 != lSocIdx_u16) &&
            (RBA_ETHUDP_SOCK_BOUND <= rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].SockState_en) &&
            (LocalAddrId_u8 == rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].LocalAddrId_u8) &&
            ((*Port_pu16) == rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].LocalPort_u16) &&
            (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].IpAddressType_en == TCPIP_UNICAST) )
        {
            if( FALSE == WildCardPort_b )
            {
                lAddrInUse_b = TRUE;
                break;
            }
            else
            {
                /* Function call to choose the local port automatically when the parameter is specified as TCPIP_PORT_ANY */
                rba_EthUdp_GetPortAny(Port_pu16);

                /* Check again for all sockets if the newly assigned port number is in use */
                lSocIdx_u16 = 0U;
            }
        }
        else
        {
            lSocIdx_u16++;
        }
    }

    return lAddrInUse_b;
}

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
