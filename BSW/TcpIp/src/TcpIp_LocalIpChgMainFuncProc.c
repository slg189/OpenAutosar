

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#include "EthIf.h"

#include "EthSM_Cbk.h"

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp.h"
#endif

#if ( TCPIP_AUTOIP_ENABLED == STD_ON )
#include "rba_EthAutoIp.h"
#endif

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCP.h"
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCPv6.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#if ( TCPIP_NDP_ENABLED == STD_ON )
#include "rba_EthNdp.h"
#endif

#include "TcpIp_Prv.h"


/*
***************************************************************************************************
* Static function declaration
***************************************************************************************************
*/

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

static void TcpIp_IpAddr_StateAssigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                        const TcpIp_LocalIpChgAssignedType_tst * LocalIpChgAssigned_pcst );

static void TcpIp_IpAddr_StateUnassigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                          const TcpIp_DomainType IpDomainType_cu32);

static void TcpIp_IpAddr_StopCurrAssigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                           TcpIp_AssignmentPrio_tu8 IPAsgnPrioNew_u8 );

#if (TCPIP_IPV6_ENABLED == STD_ON)
static Std_ReturnType TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                                                           const TcpIp_IPAddrParamType_tun *  IPAddrParam_pcun,
                                                                           Eth_FilterActionType FilterActionType_en );
#endif

static void TcpIp_UpLocalIpAddrAssignmentChgIndication(TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                        TcpIp_IpAddrStateType IpAddrState);

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/****************************************************************************************************************************/
/* TcpIp_LocalIpChgMainFuncProc() - This API is called from TcpIp_MainFunction().                                           */
/*                                  The notification of IP assignment change through API TcpIp_LocalIpAddrAssignmentChg(),  */
/*                                  is processed here.                                                                      */
/*                                                                                                                          */
/* Sync/Async               - Synchronous                                                                                   */
/* Reentrancy               - Non Reentrant                                                                                 */
/*                                                                                                                          */
/* Input Parameters :                                                                                                       */
/*      LocalAddrId_u8 : Idx of the LocalAddress for which the Local IP address change will be processed.                   */
/* Return           : void                                                                                                  */
/*                                                                                                                          */
/****************************************************************************************************************************/
void TcpIp_LocalIpChgMainFuncProc( TcpIp_LocalAddrIdType LocalAddrId_u8 )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    TcpIp_LocalIpChgAssignedType_tst    lLocalIpChgAssigned_st;
    uint8                               lAddrAssgntMethIdx_u8;
#if (TCPIP_IPV6_ENABLED == STD_ON)
    Std_ReturnType                      lStdRetType_en;
#endif

    /* Address of the requested LocalAddress configuration structure. */
    lLocalAddrConfig_pcst =  ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* If there is a valid local IP assigned / unassigned notification */
    if( (lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqUnassignType_u8 != 0U ) ||
        (lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignType_en != TCPIP_IPADDR_ASSIGNMENT_INVALID) )
    {
        /* Enter critical section (to avoid race condition if terminating and binding of sockets is executed concurrently) */
        SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

        /* Set the ClosingSockInProgress_b to TRUE. */
        lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = TRUE;

        /* If there is no binding of sockets and packet transmission in progress, continue further processing. */
        /* During new Local IP assignment or IP unassignment, all the current opened sockets needs to be closed. Therefore no socket operations are allowed in the meantime. */
        if(( lLocalAddrConfig_pcst->SockOptStatus_pst->BindingSockCount_u16 == 0U ) &&
           ( lLocalAddrConfig_pcst->SockOptStatus_pst->SendingDataCount_u16 == 0U ))
        {
            /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
            SchM_Exit_TcpIp_ExclusiveAreaSockOperations();

            /* Enter critical section:  local IP change parameters can be modified in a different execution context via API TcpIp_LocalIPAddrAssignmentChg() API. */
            /* Hence the access to the parameters shall be protected under a critical section. */
            SchM_Enter_TcpIp_ExclusiveAreaLocalIPChgParamAccess();

            /* Store the local IP address assignment change parameters in local structure. */
            lLocalIpChgAssigned_st = *(lLocalAddrConfig_pcst->LocalIpChgAssigned_pst);

            /* Set the assignment change notification to initial values, after storing the parameters in local variables. */
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqUnassignType_u8 = 0U;
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignType_en = TCPIP_IPADDR_ASSIGNMENT_INVALID;
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignPrio_u8 = 0xFFU;

            /* Exit critical section. */
            SchM_Exit_TcpIp_ExclusiveAreaLocalIPChgParamAccess();

            /* ------------------------------------------ */
            /* LOCAL IP ADDRESS UNASSIGNED                */
            /* ------------------------------------------ */

            /* Loop through all AddressAssignments configured for the requested LocalAddress. */
            for( lAddrAssgntMethIdx_u8 = 0U; ((lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8) && (lLocalIpChgAssigned_st.ReqUnassignType_u8 != 0U)); lAddrAssgntMethIdx_u8++ )
            {
                /* Execute unassignement if TcpIp_LocalIpAddrAssignmentChg() has been received for the methods and method is the currently assigned */
                if( (0U != ( lLocalIpChgAssigned_st.ReqUnassignType_u8 & ( (uint8)(1u << lAddrAssgntMethIdx_u8 )))) &&
                    (lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en) )
                {
#if (TCPIP_IPV6_ENABLED == STD_ON)
                    /* Intialize return value to E_OK. */
                    lStdRetType_en = E_OK;

                    if(( lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET6 ) &&
                       ( lLocalAddrConfig_pcst->IpAddressType_en == TCPIP_UNICAST ))
                    {
                        /* Update the physical filter for the SNMA address of the unicast IPv6 address. */
                        lStdRetType_en = TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter( lLocalAddrConfig_pcst,
                                                                                              &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un),
                                                                                              REMOVE_FROM_FILTER );

                        /* If there was a failure in updating the SNMA MAC filter, then trigger TcpIp_LocalIpAddrAssignmentChg() again with unassigned notification for the current assigned method. */
                        /* This is necessary so that the unassignment of the IP address can be retried in the next mainfunction. */
                        if( E_NOT_OK == lStdRetType_en )
                        {
                            TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_u8,
                                                            TCPIP_IPADDR_STATE_UNASSIGNED,
                                                            NULL_PTR,
                                                            lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en,
                                                            TCPIP_AF_INET6 );
                        }
                    }

                    if( E_OK == lStdRetType_en )
#endif
                    {
                        /* If there is a valid local IP unassigned notification, call the corresponding subfunction to process the assignment change. */
                        TcpIp_IpAddr_StateUnassigned( LocalAddrId_u8,
                                                      lLocalAddrConfig_pcst->IpDomainType_u32 );
                    }
                    break;
                }
            }

            /* ------------------------------------------ */
            /* LOCAL IP ADDRESS ASSIGNED                  */
            /* ------------------------------------------ */

            /* Check also whether the new assignment priority is higher than current assigned priority (Lower the value higher the priority). */
            /* In case a lower priority assignment got stored in TcpIp_RequestIpAddrAssignment() API while an higher priority IP assignment was completed parallely, */
            /* this priority check will discard any notified lower priority IP assignments. */
            /* Current state shall not be in SHUTDOWN or OFFLINE as it is an extra security check to avoid any sub module giving a wrong assigned indication during these TcpIp states. */
            /* Requested state of TcpIpCtrl is not checked because the requested state can be changed in a different execution context via API TcpIp_RequestComMode() */
            /* and therebly the IP assigned notification will not be blocked if any change in requeste state occurs. */
            if(( lLocalIpChgAssigned_st.ReqAssignType_en != TCPIP_IPADDR_ASSIGNMENT_INVALID ) &&
               ( lLocalIpChgAssigned_st.ReqAssignPrio_u8 <= ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrPrio_u8 )) &&
               (( TCPIP_STATE_ONLINE == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ]) ||
                ( TCPIP_STATE_ONHOLD == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ]) ||
                ( TCPIP_STATE_STARTUP == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] )))
            {
#if (TCPIP_IPV6_ENABLED == STD_ON)
                /* Intialize return value to E_OK. */
                lStdRetType_en = E_OK;

                /* The solicited node multicast MAC filter shall be added for every unicast IPv6 address assigned as specified in RFC. */
                /* RFC4291 section 2.8: A host is required to recognize the following addresses as identifying itself: */
                /* The Solicited-Node multicast address for each of its unicast and anycast addresses. */
                if(( lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET6 ) &&
                   ( lLocalAddrConfig_pcst->IpAddressType_en == TCPIP_UNICAST ))
                {
                    /* If a new IP address of higher priority got assigned, then remove the SNMA MAC address filter for the current assigned IP address. */
                    if( TCPIP_IPADDR_ASSIGNMENT_INVALID != lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
                    {
                        /* Remove the physical filter for the SNMA address of the current assigned unicast IPv6 address. */
                        lStdRetType_en = TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter( lLocalAddrConfig_pcst,
                                                                                              &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un),
                                                                                              REMOVE_FROM_FILTER );

                    }

                    if( E_OK == lStdRetType_en )
                    {
                        /* Add the physical filter for the SNMA address of the newly assigned unicast IPv6 address. */
                        lStdRetType_en = TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter( lLocalAddrConfig_pcst,
                                                                                              &(lLocalIpChgAssigned_st.ReqAssignAddressParam_un),
                                                                                              ADD_TO_FILTER );

                    }

                    /* If there was a failure in updating the SNMA MAC filter, then trigger TcpIp_LocalIpAddrAssignmentChg() again with assigned notification for the newly assigned method.  */
                    /* This is necessary so that the assignment of the IP address can be retried in the next mainfunction. */
                    if( E_NOT_OK == lStdRetType_en )
                    {
                        TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_u8,
                                                        TCPIP_IPADDR_STATE_ASSIGNED,
                                                        &(lLocalIpChgAssigned_st.ReqAssignAddressParam_un),
                                                        lLocalIpChgAssigned_st.ReqAssignType_en,
                                                        TCPIP_AF_INET6 );
                    }
                }

                if( E_OK == lStdRetType_en )
#endif
                {
                    /* If there is a valid local IP assigned notification, call the corresponding subfunction to process the assignment change. */
                    TcpIp_IpAddr_StateAssigned( LocalAddrId_u8,
                                                &lLocalIpChgAssigned_st);
                }
            }

            /* Set ClosingSockInProgress to FALSE. */
            lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = FALSE;
        }
        else
        {
            /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurrently). */
            SchM_Exit_TcpIp_ExclusiveAreaSockOperations();
        }
    }
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* TcpIp_IpAddr_StateAssigned() - This API is called from  function TcpIp_LocalIpAddrAssignmentChg()                */
/*                                when the state of the IP address is  TCPIP_IPADDR_STATE_ASSIGNED                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8          - Local address index for which IP address was assigned.                                */
/*  LocalIpChgAssigned_pcst - Pointer to structure containing the assigned IP address method, assigned IP address,  */
/*                            netmask and default router                                                            */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*                      void                                                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
static void TcpIp_IpAddr_StateAssigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                        const TcpIp_LocalIpChgAssignedType_tst * LocalIpChgAssigned_pcst )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;

    /* Local variables initialization */
    /* Retrieve the local address configuration pointer*/
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* If another IP is already assigned to the LocalAddress, inform SoAd that current assigned IP is unassigned   */
    if( TCPIP_IPADDR_ASSIGNMENT_INVALID != lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
    {
        TcpIp_UpLocalIpAddrAssignmentChgIndication(LocalAddrId_u8,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED );
    }
    else
    {
        /* Increment the counter to keep record of number of LocalAddrIds referring to particular EthIfController  */
        TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8]++;
    }

    /* Stop current assigned IP addresses with lower priority */
    TcpIp_IpAddr_StopCurrAssigned( LocalAddrId_u8, LocalIpChgAssigned_pcst->ReqAssignPrio_u8 );

    /* Close Tcp and Udp sockets using this LocalAddrId, except Dhcp client  */
#if (TCPIP_TCP_ENABLED == STD_ON)
    rba_EthTcp_AbortByLocalAddrId(LocalAddrId_u8);
#endif

#if (TCPIP_UDP_ENABLED == STD_ON)
    rba_EthUdp_TerminateSocUsingLocalAddr(LocalAddrId_u8);
#endif

    /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
    /* Hence the modifications of the current assigned address structure shall be protected in a critical section. */
    SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

    /* Update the IP address into the dynamic field. */
    ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )    = LocalIpChgAssigned_pcst->ReqAssignType_en;
    ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrPrio_u8 )    = LocalIpChgAssigned_pcst->ReqAssignPrio_u8;
    lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un              = LocalIpChgAssigned_pcst->ReqAssignAddressParam_un;

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

    /* Clear the ClosingSockInProgress_b flag to FALSE. */
    lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = FALSE;

    /* If the Ctrl is in STARTUP set the Ctrl to ONLINE and inform EthSM. */
    if( TCPIP_STATE_STARTUP == TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] )
    {
       ( TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] ) = TCPIP_STATE_ONLINE ;
       (void)EthSM_TcpIpModeIndication( lLocalAddrConfig_pcst->EthIfCtrlRef_u8, TCPIP_STATE_ONLINE );
    }

    /* If current EthIfCtrl state is not ONHOLD */
    /* Call back shall not be provided if the Ctrl state is ONHOLD. Call back will be provided to SoAd once Ctrl reaches to ONLINE  */
    if (TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] != TCPIP_STATE_ONHOLD )
    {
        TcpIp_UpLocalIpAddrAssignmentChgIndication(LocalAddrId_u8,
                                                    TCPIP_IPADDR_STATE_ASSIGNED );
    }
}


/********************************************************************************************************************/
/* TcpIp_IpAddr_StateUnassigned() -   This API is called from  function TcpIp_LocalIpAddrAssignmentChg()            */
/*                                    when the state of the IP address is  TCPIP_IPADDR_STATE_UNASSIGNED            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8        -  Local address index for which IP address was unassigned.                               */
/*  IpDomainType_cu32     - IP address domain type - TCPIP_AF_INET/ TCPIP_AF_INET6                                  */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*                      void                                                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
/* HIS METRIC PATH VIOLATION IN TcpIp_IpAddr_StateUnassigned: The number of path is increased due to the consecutive if blocks which are required for the processing of various IP assignment methods for domains TCPIP_AF_INET and TCPIP_AF_INET6. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
static void TcpIp_IpAddr_StateUnassigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                          const TcpIp_DomainType IpDomainType_cu32 )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint16                              lIdx_u16;
    TcpIp_LocalAddrIdType               lTotalActiveLocalAddr_u8;
    uint8                               lAddrAssgntMethIdx_u8;
    TcpIp_StateType                     lTcpIp_EthIfCtrlReqSt_en;

    /* Local variables initialization */
    lTotalActiveLocalAddr_u8 = 0U;

    /* Retrieve the local address configuration pointer*/
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Store local value of TcpIp_EthIfCtrlReqSt_aen and use this value for further processing, */
    /* because the requested state can be updated in API TcpIp_RequestComMode() via a different execution context. */
    lTcpIp_EthIfCtrlReqSt_en = TcpIp_EthIfCtrlReqSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8];

    /* Count total number of localAddress which has IP assigned */
    for( lIdx_u16 = 0; lIdx_u16 < ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ); lIdx_u16++ )
    {
        if( TCPIP_IPADDR_ASSIGNMENT_INVALID != ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx_u16].CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en ) )
        {
            lTotalActiveLocalAddr_u8++;
        }
    }

    TcpIp_UpLocalIpAddrAssignmentChgIndication( LocalAddrId_u8,
                                                TCPIP_IPADDR_STATE_UNASSIGNED );

    /* If no other localAddress has Ipassigned close all the socket. If LocalAddrId is set to TCPIP_LOCALADDRID_INVALID */
    /* rba_EthUdp_TerminateSocUsingLocalAddr() and rba_EthTcp_AbortByLocalAddrId() will close all the sockets */
    if(1u == lTotalActiveLocalAddr_u8)
    {
        /* Close all sockets, except Dhcp client  */
#if (TCPIP_TCP_ENABLED == STD_ON)
        rba_EthTcp_AbortByLocalAddrId(TCPIP_LOCALADDRID_INVALID);
#endif

#if (TCPIP_UDP_ENABLED == STD_ON)
        rba_EthUdp_TerminateSocUsingLocalAddr(TCPIP_LOCALADDRID_INVALID);
#endif
    }
    else
    {
        /* Close all sockets using this localAddrId, except Dhcp client */
#if (TCPIP_TCP_ENABLED == STD_ON)
        rba_EthTcp_AbortByLocalAddrId(LocalAddrId_u8);
#endif

#if (TCPIP_UDP_ENABLED == STD_ON)
        rba_EthUdp_TerminateSocUsingLocalAddr(LocalAddrId_u8);
#endif
    }

    /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
    /* Hence the modifications of the current assigned address structure shall be protected in a critical section. */
    SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

    /* Release IP address by setting the IP address to ZERO. */
    ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )    = TCPIP_IPADDR_ASSIGNMENT_INVALID;
    ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrPrio_u8 )    = TCPIP_IPADDR_ASSIGNPRIO_INVALID;

    switch(IpDomainType_cu32)
    {
#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_AF_INET:
        {
            ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32 )     = TCPIP_IPADDR_ZERO;
            ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32 )     = TCPIP_IPADDR_ZERO;
            ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.NetMask_u32 )    = TCPIP_IPV4_NETMASK_INVALID;
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_AF_INET6:
        {
            rba_EthIPv6_SetIPv6AddrTo0(&(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0]));
            rba_EthIPv6_SetIPv6AddrTo0(&(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[0]));
            ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.NetMask_u8 )    = 0U;
        }
        break;

#endif

        default:
        {
            /* not reacheable because the param is validated at function start */
        }
        break;
    }

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

    /* Clear the ClosingSockInProgress_b flag to FALSE. */
    lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = FALSE;

    /* If there is no LocalAddrId mapped to the EthIfCtrl that has IP address assigned, then change the state of that EthIfCtrl to OFFLINE */
    if(1U == TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8])
    {
        if( lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET ) /* ARP APIs cannot be called at all for IPv6 addresses */
        {
#if (TCPIP_ARP_ENABLED == STD_ON)
            /* Clean the ARP table corresponding to the current EthIf controller */
            rba_EthArp_CleanArpTable( lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ); /* Applicable only for IPv4 specific addresses */
#endif
        }
        else
        {
#if( TCPIP_NDP_ENABLED == STD_ON )
            /* Clean NDP table corresponding to the current EthIf controller. */
            rba_EthNdp_CleanNdpTable( lLocalAddrConfig_pcst->EthIfCtrlRef_u8) ; /* Applicable only for IPv6 specific addresses */
#endif
        }

        /* Set the Ctrl state to STARTUP or OFFLINE based on requested EthIfCtrl state */
        /* If the requested state is TCPIP_STATE_OFFLINE set current state to OFFLINE, otherwise set to STARTUP so that re-assignment of IP addresses are triggered. */
        TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] = (TCPIP_STATE_OFFLINE != lTcpIp_EthIfCtrlReqSt_en) ? TCPIP_STATE_STARTUP: TCPIP_STATE_OFFLINE;

        /* Inform EthSM about change in Ctrl state to OFFLINE */
        (void)EthSM_TcpIpModeIndication( lLocalAddrConfig_pcst->EthIfCtrlRef_u8,
                                         TCPIP_STATE_OFFLINE );
    }

    /* If the local Address has an assignment method with trigger AUTOMATIC try to retrigger IP assignment */
    if( TCPIP_STATE_OFFLINE != lTcpIp_EthIfCtrlReqSt_en ) /* If the TcpIpCtrl requested state is ONLINE or ONHOLD  */
    {
        /* Loop through all AddressAssignments configured for the requested LocalAddress */
        for( lAddrAssgntMethIdx_u8 = 0; (lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8); lAddrAssgntMethIdx_u8++ )
        {
            /* If the IP assignment trigger is AUTOMATIC set the corresponding bit. */
            if(TCPIP_AUTOMATIC == (lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentTrig_en) )
            {
                /* Enter critical section: The request assignment method variable can be updated in different execution context and should be protected in a critical section. */
                SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

                TCPIP_UINT8_SET_BIT_WITH_SHIFT((lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8), (lAddrAssgntMethIdx_u8));

                /* Exit critical section. */
                SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
            }
        }
    }

    /* Decrement the counter to keep record of number of LocalAddrIds referring to particular EthIfController  */
    TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] = ( (TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] > 0U) ?
                                                                                         (TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8]-1U) :
                                                                                         (0U) );
}

/********************************************************************************************************************/
/* TcpIp_IpAddr_StopCurrAssigned() -   This API is called from function TcpIp_IpAddr_StateAssigned()                */
/*                                    to stop all lower priority current assigned IP addresses.                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index for which IP address assignment shall be stopped.                     */
/*  IPAsgnPrioNew_u8    - new assignment priority (Lower the value higher the priority).                            */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*                      void                                                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
static void TcpIp_IpAddr_StopCurrAssigned( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                           TcpIp_AssignmentPrio_tu8 IPAsgnPrioNew_u8  )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint16                              lIdx_u16;

    /* Retrieve the local address configuration pointer*/
    lLocalAddrConfig_pcst   = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Loop through all the current assigned IP addresses */
    for( lIdx_u16 = 0; lIdx_u16 < ( lLocalAddrConfig_pcst->NumAddrAsgnment_u8 ); lIdx_u16++ )
    {
        /* If any other address assignment process (lower priority) is triggered but not yet completed, then stop that process */
        if( IPAsgnPrioNew_u8 < ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lIdx_u16].AsgnmentPrio_u8 ) )
        {
            /* Enter critical section: The request assignment method variable can be updated in different execution context and should be protected in a critical section. */
            SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

            /* Clear the request assignment bit for all the lower priority addresses which have not yet been processed. */
            (lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8) &= (uint8)( ~(uint8)(1U << lIdx_u16));

            /* Exit critical section. */
            SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();

            switch(lLocalAddrConfig_pcst->AddrAsgnment_pcst[lIdx_u16].AddrAsgnmentMeth_en)
            {
                case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL:
                case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP:
                {
                    /* Check whether domain is TCPIP_AF_INET. */
#if ( TCPIP_AUTOIP_ENABLED == STD_ON )
                    if( TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32 )
                    {
                        /* Stop AutoIp process */
                        (void)rba_EthAutoIp_Stop( LocalAddrId_u8 );
                    }
#endif
                }
                break;

                case TCPIP_IPADDR_ASSIGNMENT_DHCP:
                {
                    /* Stop Dhcp process */
                    if(lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET)
                    {
                        /* Stop Dhcp process */
#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
                        (void)rba_EthDHCP_Stop( LocalAddrId_u8 );
#endif
                    }
                    else
                    {
#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
                        (void)rba_EthDHCPv6_RunModeRequest(LocalAddrId_u8, RBA_ETHDHCPV6_RUNMODE_OFFLINE);
#endif
                    }
                }
                break;

                case TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER:
                {
                    /*Stop Router Discovery process */
#if (TCPIP_NDP_PRD_PREFIXROUTERDISC_ENABLED == STD_ON)
                    (void)rba_EthNdp_Stop( LocalAddrId_u8 );
#endif
                }
                break;

                default:
                {
                    /* Nothing to do */
                }
                break;
            }
        }
    }
}

/************************************************************************************************************************************/
/* TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter() - This is a helper function which updates the                              */
/*                                                   solicited node multicast address MAC filter for the passed static IPv6 address.*/
/*                                                                                                                                  */
/* Input Parameters :                                                                                                               */
/*    LocalAddrConfig_pcst - LocalAddrId config structure.                                                                          */
/*    IPAddrParam_pcun - The IPv6 address parameter for which the solicitated multicast address is constructed.                     */
/*    FilterActionType_en - Filter action to Add/remove SNMA multicast address filter.                                              */
/*                                                                                                                                  */
/* Return  :                                                                                                                        */
/*    Std_ReturnType  - E_OK Update successful.                                                                                     */
/*                      E_NOT_OK Update unsuccessful.                                                                               */
/*                                                                                                                                  */
/************************************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static Std_ReturnType TcpIp_Prv_UpdateIPv6SolicitedNodeMulticastMACfilter( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                                                           const TcpIp_IPAddrParamType_tun *  IPAddrParam_pcun,
                                                                           Eth_FilterActionType FilterActionType_en )
{
    /* Local variable declaration. */
    TcpIp_IPAddrParamType_tun           lSolMulticastAddrParams_un;
    Std_ReturnType                      lStdRetType_en;

    /*  The solicited-node multicast address is formed by appending the last-order 24 bits of an address (unicast or anycast)        */
    /*  to the prefix ff02::1:ff00:0/104. Therefore, the format for this address is ff02:0:0:0:0:1:ffxx:xxxx, where xx:xxxx is       */
    /*  the last-order 24-bit portion of the target address. For example, the solicited-node multicast address for the               */
    /*  link-local address fe80::240b:2ac8:47c8:8e50 is ff02::1:ffc8:8e50.                                                           */

    lSolMulticastAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0] = 0xFF020000uL;
    lSolMulticastAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[1] = 0uL;
    lSolMulticastAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[2] = 1uL;
    lSolMulticastAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[3] = ( (0xFF000000uL) | IPAddrParam_pcun->IPv6Par_st.IpAddr_st.Addr_au32[3] );

    /* Update the physical filter for the SNMA address generated. */
    lStdRetType_en = TcpIp_UpdateMulticastPhysFilter( LocalAddrConfig_pcst, FilterActionType_en, &(lSolMulticastAddrParams_un) );

    return lStdRetType_en;
}
#endif

/********************************************************************************************************************/
/* TcpIp_UpLocalIpAddrAssignmentChgIndication() - This API is called to inform all upper layers about the State of  */
/*                                                  IP address for the localAddrId is assigned/unassigned           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index for which IP address assignment shall be stopped.                     */
/*  IpAddrState         - TCPIP_IPADDR_STATE_ASSIGNED/TCPIP_IPADDR_STATE_UNASSIGNED                                 */
/*                                                                                                                  */
/* Return  :            - void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
static void TcpIp_UpLocalIpAddrAssignmentChgIndication(TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                        TcpIp_IpAddrStateType IpAddrState)
{
    /* Local variable declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint16                              lIdx_u16;

    /* Retrieve the local address configuration pointer*/
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Loop through all upper layers and inform that the IP address for the localAddrId is assigned/unassigned */
    for( lIdx_u16 = 0; lIdx_u16 < TcpIp_CurrConfig_pco->NumSockOwners_cu8; lIdx_u16++ )
    {
        if( (TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lIdx_u16].Up_LocalIpAddrAssignmentChg_pfct != NULL_PTR) &&
            (LocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrUserConfigured_u8) ) /* Callback need to be given to UL only if the local address was configured by the user (not autogenerated by TcpIp) */
        {
            TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lIdx_u16].Up_LocalIpAddrAssignmentChg_pfct( LocalAddrId_u8, IpAddrState );
        }
    }

#if (TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON)
    /* Inform separately Dhcpv6 about IP assignement change */
    /* This call is required so that Dhcpv6 know that IP is assigned or unassigned and address conflict handling in Dhcpv6 can be executed */
    /* For example: When rba_EthDHCPv6_RunModeRequest(ONLINE) was called and Dhcpv6 receives the call of rba_EthDHCPv6_LocalIpAddrAssignmentChg(UNASSIGNED), */
    /* this means that IP was unassigned by TcpIp because of IPv6 conflict on the network */
    if( (lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET6) &&
        (lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en == TCPIP_IPADDR_ASSIGNMENT_DHCP) )
    {
        rba_EthDHCPv6_LocalIpAddrAssignmentChg( LocalAddrId_u8, IpAddrState );
    }
#endif
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

