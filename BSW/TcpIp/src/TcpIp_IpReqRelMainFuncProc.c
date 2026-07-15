

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#include "EthIf.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
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

static Std_ReturnType TcpIp_StartIPAssign( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                           TcpIp_IpAddrAssignmentType AddrAssignment_Meth_en,
                                           TcpIp_AssignmentTrigger_ten AddrAssignment_Trigger_en);

static Std_ReturnType TcpIp_Prv_ReleaseIPAddrAsgnmentMeth( const TcpIp_LocalAddrIdType LocalAddrId_cu8,
                                                           const TcpIp_IpAddrAssignmentType AddrAsgnmentMeth_cen );

#if (TCPIP_IPV6_ENABLED == STD_ON)
static void TcpIp_Prv_GetIPv6linkLocalIpAddr( uint8 EthIfCtrlIdx_u8,
                                              TcpIp_IPAddrParamType_tun * IPAddrParam_pun );
#endif

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/*******************************************************************************************************************/
/* TcpIp_IpReqRelMainFuncProc() - This API is called from TcpIp_MainFunction(). The  request to assign or release  */
/*                                an IP address is processed in this function.                                     */
/*                                The request for assignment is stored via  API TcpIp_RequestIpAddrAssignment()    */
/*                                or a TCPIP_STATE_ONLINE request from EthSM.                                      */
/*                                The release IP request is stored via API TcpIp_ReleaseIpAddrAssignment() API or  */
/*                                a TCPIP_STATE_OFFLINE request from EthSM.                                        */
/*                                                                                                                 */
/* Sync/Async            - Synchronous                                                                             */
/* Reentrancy            - Non Reentrant                                                                           */
/*                                                                                                                 */
/* Input Parameters :                                                                                              */
/*      LocalAddrId_u8 : Idx of the LocalAddress for which the IP address request/release will be processed.       */
/*                                                                                                                 */
/* Return           : void                                                                                         */
/*                                                                                                                 */
/*******************************************************************************************************************/
void TcpIp_IpReqRelMainFuncProc( TcpIp_LocalAddrIdType LocalAddrId_u8 )
{
    /* Declare local variables. */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    Std_ReturnType                      lStdRetType_en;
    uint8                               lAddrAssgntMethIdx_u8;
    boolean                             lDhcpAlreadyAssigned_b;
    uint8                               lRelAddrAssgntMethIdx_u8;
    uint8                               lReqAddrAssgntMethIdx_u8;

    /* Initialize local variables. */
    lStdRetType_en            = E_NOT_OK;
    lDhcpAlreadyAssigned_b    = FALSE;

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Store the global Request and release assignment variables into local variables. */
    /* Enter critical section: The request and release address assignment method variable can be updated in */
    /*                         different execution context and should be protected in a critical section. */
    SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

    lRelAddrAssgntMethIdx_u8 = lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8;
    lReqAddrAssgntMethIdx_u8 = lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8;
    lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8 = 0U;
    lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8 = 0U;

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();

    /* ------------------------------------------ */
    /* IP ADDRESS RELEASE PROCESSING              */
    /* ------------------------------------------ */

    /* If the release address assignment bits are set, then process the IP address release requests. */
    if( 0x00u != lRelAddrAssgntMethIdx_u8 )
    {
        /* Enter critical section (to avoid race condition if terminating and binding of sockets is executed concurrently) */
        SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

        /* Set ClosingSockInProgress_b to TRUE so that further bind and transmissions are blocked when IP address unassignment is ongoing. */
        lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = TRUE;

        /* If the currently binding sockets and transmission count is zero, proceed with releasing the IP addresses. */
        if(( lLocalAddrConfig_pcst->SockOptStatus_pst->BindingSockCount_u16 == 0U ) &&
           ( lLocalAddrConfig_pcst->SockOptStatus_pst->SendingDataCount_u16 == 0U ))
        {
            /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
            SchM_Exit_TcpIp_ExclusiveAreaSockOperations();

            /* Loop through all AddressAssignments configured for the requested LocalAddress */
            for( lAddrAssgntMethIdx_u8 = 0U; (lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8); lAddrAssgntMethIdx_u8++ )
            {
                if( 0U != ( lRelAddrAssgntMethIdx_u8 & ( (uint8)(1U << lAddrAssgntMethIdx_u8)) ) )
                {
                    /* If the release address bit is set for the current address assigment method, trigger the release of the IP address. */
                    lStdRetType_en = TcpIp_Prv_ReleaseIPAddrAsgnmentMeth ( LocalAddrId_u8,
                                                                           lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en );

                    /* If the release IP address function call was successful, clear the release assignment bit. */
                    if( E_OK == lStdRetType_en )
                    {
                        TCPIP_UINT8_CLEAR_BIT_WITH_SHIFT((lRelAddrAssgntMethIdx_u8), (lAddrAssgntMethIdx_u8));
                    }
                }
            }

            /* Set the closing sock in Progress flag to FALSE. */
            lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b  = FALSE;

        }
        else
        {
            /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurently) */
            SchM_Exit_TcpIp_ExclusiveAreaSockOperations();
        }
    }

    /* ------------------------------------------ */
    /* IP ADDRESS REQUEST PROCESSING              */
    /* ------------------------------------------ */

    /* If the request address assignment bits are set and the TcpIpCtrl requested state is ONLINE, */
    /*  then process the IP address assignment requests. */
    if(( 0x00U != lReqAddrAssgntMethIdx_u8 ) &&
       ( TCPIP_STATE_ONLINE == TcpIp_EthIfCtrlReqSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] ) &&
       (( TCPIP_STATE_ONLINE == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ]) ||
        ( TCPIP_STATE_ONHOLD == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ]) ||
        ( TCPIP_STATE_STARTUP == TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] )))
    {
        /* If Dhcp IP is already assigned, release the IP before assigning new one. */
        /* This is required as Dhcp release frame should have assigned Dhcp IP in the source IP field of IP header. */
        if( TCPIP_IPADDR_ASSIGNMENT_DHCP ==  lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
        {
            if( TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32 )
            {
#if (TCPIP_DHCP_CLIENT_ENABLED == STD_ON)
                (void)rba_EthDHCP_Stop(LocalAddrId_u8);
                lDhcpAlreadyAssigned_b = TRUE;
#endif
            }
            else
            {
#if (TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON)
                (void)rba_EthDHCPv6_RunModeRequest(LocalAddrId_u8, RBA_ETHDHCPV6_RUNMODE_OFFLINE);
                lDhcpAlreadyAssigned_b = TRUE;
#endif
            }
        }

        /* Loop through all AddressAssignments configured for the requested LocalAddress. */
        for( lAddrAssgntMethIdx_u8 = 0U; ((lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8) && (lDhcpAlreadyAssigned_b == FALSE)); lAddrAssgntMethIdx_u8++ )
        {
            /* Initiate IP assignment for all requested assignment methods */
            if( (0U != ( lReqAddrAssgntMethIdx_u8 & ( (uint8)(1u << lAddrAssgntMethIdx_u8 )))) &&
                    ( 0U == ( lRelAddrAssgntMethIdx_u8 & ( (uint8)(1u << lAddrAssgntMethIdx_u8 )))))
            {
                lStdRetType_en= TcpIp_StartIPAssign( LocalAddrId_u8,
                                                     lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en,
                                                     lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentTrig_en);

                /* If the IP assignmnet process is initiated successfully clear the bit */
                if( E_OK == lStdRetType_en )
                {
                    TCPIP_UINT8_CLEAR_BIT_WITH_SHIFT((lReqAddrAssgntMethIdx_u8), (lAddrAssgntMethIdx_u8));
                }
            }
        }
    }

    /* Enter critical section: The release address assignment method variable can be updated in different execution context and should be protected in a critical section. */
    SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

    /* Store back the req and rel assignements which have not been processed */
    lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8 |= lRelAddrAssgntMethIdx_u8;
    lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8 |= lReqAddrAssgntMethIdx_u8;

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* TcpIp_StartIPAssign() - This API is called from TcpIp_InitiateIpAssgn(). It starts the IP address assignment of  */
/*                          IP address assignment method AddrAssignment_Meth_en                                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           - Idx of the LocalAddress for which IP assignment process needs to be started          */
/* AddrAssignment_Meth_en    - IP address assignment method                                                         */
/* AddrAssignment_Trigger_en - IP address assignment trigger type                                                   */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/* Std_ReturnType       -   Result of operation                                                                     */
/*                          E_OK - Request processed successfully                                                   */
/*                          E_NOT_OK - Request NOT processed successfully                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType TcpIp_StartIPAssign( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                           TcpIp_IpAddrAssignmentType AddrAssignment_Meth_en,
                                           TcpIp_AssignmentTrigger_ten AddrAssignment_Trigger_en)
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *        lLocalAddrConfig_pcst;
    Std_ReturnType                          lStdRetType_en;
    const TcpIp_IPAddrParamType_tun *       lIpAddrParams_pcun;
    TcpIp_IPAddrParamType_tun               lIPAddrParam_un;

    lStdRetType_en = E_NOT_OK;

    /* Initialize the pointer to the local address configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    switch(AddrAssignment_Meth_en)
    {
        /* Initiation of AUTOIP assignment if the LocalAddress is configured so */
        case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL:
        case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP:
        {
            /* Check the domain of the localAddrId. */
            if(TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32)
            {
                /* If the domain of the localAddrId is TCPIP_AF_INET, trigger AutoIP address assignment. */
#if (TCPIP_AUTOIP_ENABLED == STD_ON)
                lStdRetType_en = rba_EthAutoIp_Start( LocalAddrId_u8 );
#endif
            }

            else
            {/* If the domain is TCPIP_AF_INET6, assign link local IP address to the localAddrId. */
#if (TCPIP_IPV6_ENABLED == STD_ON)
                /* Fetch the link local IP address. */
                TcpIp_Prv_GetIPv6linkLocalIpAddr( lLocalAddrConfig_pcst->EthIfCtrlRef_u8,
                                                  &lIPAddrParam_un );

                /*  RFC2464 : An IPv6 address prefix used for stateless autoconfiguration [ACONF] of an Ethernet interface must have a length of 64 bits. */
                /* Set the netmask to fixed value 64 as all IPv6 linklocal IP addresses have prefix length 64. */
                lIPAddrParam_un.IPv6Par_st.NetMask_u8 = 64u;

                /* Set default router value to 0. */
                rba_EthIPv6_SetIPv6AddrTo0(&(lIPAddrParam_un.IPv6Par_st.DftRtr_st.Addr_au32[0]));

                /* Store the IP address parameters and trigger TcpIpLocalIpAddrAssignment Change. */
                lIpAddrParams_pcun = (const TcpIp_IPAddrParamType_tun*) &lIPAddrParam_un;

                TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_u8,
                                                TCPIP_IPADDR_STATE_ASSIGNED,
                                                lIpAddrParams_pcun,
                                                TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL,
                                                TCPIP_AF_INET6 );

                /* Set return value to E_OK. */
                lStdRetType_en = E_OK;
#endif
            }

        }
        break;

        /* Initiation of DHCP assignment if the LocalAddress is configured so */
        case TCPIP_IPADDR_ASSIGNMENT_DHCP:
        {

            if(TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32)
            {
#if (TCPIP_DHCP_CLIENT_ENABLED == STD_ON)
                lStdRetType_en = rba_EthDHCP_Start( LocalAddrId_u8 );
#endif
            }
            else
            {
#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
                lStdRetType_en = rba_EthDHCPv6_RunModeRequest(LocalAddrId_u8, RBA_ETHDHCPV6_RUNMODE_ONLINE);
#endif
            }
        }
        break;

        /* Initiation of IPV6 ROUTER assignment if the LocalAddress is configured so */
        case TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER:
        {
            /*Start Router Discovery */
#if(TCPIP_NDP_PRD_PREFIXROUTERDISC_ENABLED == STD_ON)
            lStdRetType_en = rba_EthNdp_Start(LocalAddrId_u8);
#endif
        }
        break;

        /* Assignment of STATIC if the LocalAddress is configured so */
        case TCPIP_IPADDR_ASSIGNMENT_STATIC:
        {
            /* Initialize lStdRetType_en to E_OK */
            lStdRetType_en = E_OK;

            /* Check if the Trigger is Automatic. If yes pass the structure with preconfigured values */
            if( TCPIP_AUTOMATIC == AddrAssignment_Trigger_en )
            {
                lIpAddrParams_pcun = (const TcpIp_IPAddrParamType_tun*) (lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun);
            }
            else
            {
                /* Enter critical section: Manual static IP address structure can be updated in a different execution context, for example in TcpIp_RequestIPAddrAssignment() API. */
                /* Hence the manual static address shall be protected under critical section when it is accessed. */
                SchM_Enter_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();

                /* Store local copy of the manual static IP address structure if the assignment trigger is Manual and the requested assignment is static. */
                lIPAddrParam_un = *(lLocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun);

                /* Exit critical section. */
                SchM_Exit_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();

                lIpAddrParams_pcun = (const TcpIp_IPAddrParamType_tun*) &lIPAddrParam_un;
            }

            /* Check whether the LocalAddress is multicast. If yes, a MAC filter has to be added all the way down to the driver level */
            if( TCPIP_MULTICAST == lLocalAddrConfig_pcst->IpAddressType_en )
            {
                /* In the case of adding a physical address filter, the MAC address is generated from the Static configured address structure depending on the trigger type */
                lStdRetType_en = TcpIp_UpdateMulticastPhysFilter(lLocalAddrConfig_pcst, ADD_TO_FILTER, lIpAddrParams_pcun);
            }

            /* Assign IP to the localAddress */
            if( E_OK == lStdRetType_en )
            {
                TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_u8,
                                                TCPIP_IPADDR_STATE_ASSIGNED,
                                                lIpAddrParams_pcun,
                                                TCPIP_IPADDR_ASSIGNMENT_STATIC,
                                                lLocalAddrConfig_pcst->IpDomainType_u32 );
            }
        }
        break;

        default:
        {
            /* Do nothing */
        }
        break;
    }

    return lStdRetType_en;
}


/********************************************************************************************************************/
/* TcpIp_Prv_ReleaseIPAddrAsgnmentMeth()- By this API the IP address of a specific address assignment method        */
/*                                        for a localAddrId will be released.                                       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId_cu8       - IP address index specifying the IP address which shall be released.                 */
/*      AddrAsgnmentMeth_cen  - IP address assignment method.                                                       */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/* Std_ReturnType       -   Result of operation                                                                     */
/*                          E_OK - Request processed successfully                                                   */
/*                          E_NOT_OK - Request NOT processed successfully                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType TcpIp_Prv_ReleaseIPAddrAsgnmentMeth( const TcpIp_LocalAddrIdType LocalAddrId_cu8,
                                                           const TcpIp_IpAddrAssignmentType AddrAsgnmentMeth_cen )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *   lLocalAddrConfig_pcst;
    Std_ReturnType                     lStdRetType_en;

    /* Local variable initialization. */
    lStdRetType_en        = E_NOT_OK;
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_cu8 );

    /* Release the already bound sockets if the assignement method to release is the same as the currently assigned method */
    /* (otherwise, no socket can be bound to the local address id) */
    if ( AddrAsgnmentMeth_cen == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
    {
        /*Tcp RST frames need be sent before the DHCP releasing process takes place in case of DHCP Local Address*/
#if (TCPIP_TCP_ENABLED == STD_ON)
        rba_EthTcp_AbortByLocalAddrId(LocalAddrId_cu8);
#endif

        /* Need to close all the UDP sockets mapped to LocalAddrId_cu8 except DHCP Client socket */
#if (TCPIP_UDP_ENABLED == STD_ON)
        rba_EthUdp_TerminateSocUsingLocalAddr(LocalAddrId_cu8);
#endif
    }

    /* Call the respective Release IP address API for the passed address assignment method. */
    switch( AddrAsgnmentMeth_cen )
    {
        case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL:
        case TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP:
        {
            /* Check the domain of the localAddrId. */
            if( TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32 )
            { /* if domain is TCPIP_AF_INET stop AutoIP address assignment. */
#if (TCPIP_AUTOIP_ENABLED == STD_ON)
                /* rba_EthAutoIp_Stop need to be called independant of the currently assigned method because if probing sequence is ongoing, assignement in AutoIP need to be stopped */
                lStdRetType_en = rba_EthAutoIp_Stop( LocalAddrId_cu8 );
#endif
            }
            else
            {
                /* Notify that the link local IPv6 address is unassigned. */
#if (TCPIP_IPV6_ENABLED == STD_ON)
                TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_cu8,
                                                TCPIP_IPADDR_STATE_UNASSIGNED,
                                                NULL_PTR,
                                                TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL,
                                                TCPIP_AF_INET6 );

                /* Set return value to E_OK. */
                lStdRetType_en = E_OK;
#endif
            }

        }
        break;

        case TCPIP_IPADDR_ASSIGNMENT_DHCP:
        {
            if(lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET)
            {
#if (TCPIP_DHCP_CLIENT_ENABLED == STD_ON)
                /* rba_EthDHCP_Stop need to be called independant of the currently assigned method because if probing sequence is ongoing, assignement in Dhcp need to be stopped */
                lStdRetType_en = rba_EthDHCP_Stop( LocalAddrId_cu8 );
#endif
            }
            else
            {
#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
                lStdRetType_en = rba_EthDHCPv6_RunModeRequest(LocalAddrId_cu8, RBA_ETHDHCPV6_RUNMODE_OFFLINE);
#endif
            }
        }
        break;

        case TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER:
        {
            /*Stop Router Discovery process */
#if (TCPIP_NDP_PRD_PREFIXROUTERDISC_ENABLED == STD_ON)
            lStdRetType_en = rba_EthNdp_Stop( LocalAddrId_cu8 );
#endif
        }
        break;

        case TCPIP_IPADDR_ASSIGNMENT_STATIC:
        {
            /* Set return value to E_OK. */
            /* If the assignment method is pointing to STATIC and STATIC IP is not assigned API shall not return E_NOT_OK   */
            lStdRetType_en = E_OK;

            if ( TCPIP_IPADDR_ASSIGNMENT_STATIC == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
            {
                /* Check whether the LocalAddress is multicast */
                if( TCPIP_MULTICAST == lLocalAddrConfig_pcst->IpAddressType_en )
                {
                    /* In the case of removing a physical address filter, the MAC address is generated from the Current allocated address */
                    lStdRetType_en = TcpIp_UpdateMulticastPhysFilter( lLocalAddrConfig_pcst, REMOVE_FROM_FILTER,
                                                                    ( const TcpIp_IPAddrParamType_tun *) &lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un );
                }

                /* Reset the structure members, inform SoAd and EthSM about release of IP */
                if( E_OK == lStdRetType_en )
                {
                    TcpIp_LocalIpAddrAssignmentChg( LocalAddrId_cu8,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED,
                                                    NULL_PTR,                          /* Parameter of type TcpIp_IPvAddrParamType_tst is irrelevant for this use case */
                                                    TCPIP_IPADDR_ASSIGNMENT_STATIC,
                                                    lLocalAddrConfig_pcst->IpDomainType_u32);
                }
            }
        }
        break;

        default:
        {
            /* Nothing to do. */
        }
        break;
    }

    return lStdRetType_en;
}


/************************************************************************************************************************************/
/* TcpIp_UpdateMulticastPhysFilter() - This is a helper function which performs the actual physical address update                  */
/*                                      in the Eth filter list. The purpose of his function is to separate the processing depending */
/*                                      on the address domain family (IPv4/IPv6). This function must be called  only for addresses  */
/*                                      of Multicast type.                                                                          */
/*                                                                                                                                  */
/* Input Parameters :                                                                                                               */
/*    LocalAddrConfig_pcst      - Pointer to the local IP address configuration                                                     */
/*    FilterActionType_en       - The action desired to be applied to the specified MAC address                                     */
/*                                 Allowed values: ADD_TO_FILTER/ETH_ADD_TO_FILTER or REMOVE_FROM_FILTER/ETH_REMOVE_FROM_FILTER     */
/*    IPAddrParam_pcun          - Pointer to the IP address that is used to generate Multicast MAC address and update               */
/*                                Ethernet receive filter is updated                                                                */
/* Return  :                                                                                                                        */
/*    Std_ReturnType      E_OK: The specified multicast address has been successfully updated in the filter list.                   */
/*                        E_NOT_OK: An error occured during physical filter update for the specified multicast address.             */
/*                                                                                                                                  */
/* Note :                                                                                                                           */
/*     1. When the selected action is ADD_TO_FILTER / ETH_ADD_TO_FILTER, the MAC address is generated from the Static               */
/*     configured IP address, because this step is a part of the Multicast IP Address Assignment process.                           */
/*     2. When the selected action is REMOVE_FROM_FILTER / ETH_REMOVE_FROM_FILTER, the MAC address is generated from the            */
/*     Current configured IP address, because this step is a part of the Multicast IP Address Release process.                      */
/************************************************************************************************************************************/
Std_ReturnType TcpIp_UpdateMulticastPhysFilter( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                                Eth_FilterActionType FilterActionType_en,
                                                const TcpIp_IPAddrParamType_tun * IPAddrParam_pcun )
{
    /* Local variables declaration */
    uint8                                lMulticastMACAddr_au8[6];
    Std_ReturnType                       lStdRetType_en = E_OK;        /* Optimistic initialization */

    switch (LocalAddrConfig_pcst->IpDomainType_u32)
    {
#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_AF_INET:
        {
            /* Generate the multicast MAC address */
            rba_EthIPv4_GetIPMulticastDestEthAddr( (IPAddrParam_pcun->IPv4Par_st.IpAddr_u32), &lMulticastMACAddr_au8[0] );

            /* Call the lower layer API to update the physical address filter */
            lStdRetType_en = EthIf_UpdatePhysAddrFilter( LocalAddrConfig_pcst->EthIfCtrlRef_u8, &lMulticastMACAddr_au8[0], FilterActionType_en );
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_AF_INET6:
        {
            /* Generate the multicast MAC address */
            rba_EthIPv6_GetIPMulticastDestEthAddr( &(IPAddrParam_pcun->IPv6Par_st.IpAddr_st), &lMulticastMACAddr_au8[0] );

            /* Call the lower layer API to update the physical address filter */
            lStdRetType_en = EthIf_UpdatePhysAddrFilter( LocalAddrConfig_pcst->EthIfCtrlRef_u8, &lMulticastMACAddr_au8[0], FilterActionType_en );
        }
        break;
#endif

        default:
        {
            lStdRetType_en = E_NOT_OK;
        }
        break;
    }

    return lStdRetType_en;
}

/************************************************************************************************************************************/
/* TcpIp_Prv_GetIPv6linkLocalIpAddr() - This is a helper function which gets the link local IP address derived from the EUI-64.     */
/*                                                                                                                                  */
/* Input Parameters :                                                                                                               */
/*    EthIfCtrlIdx_u8 - The EthIfCtrlIdx for which the link local IP address is being assigned.                                     */
/* Output Parameters :                                                                                                              */
/*    IPAddrParam_pst - The constructed link local address is stored in this parameter.                                             */
/*                                                                                                                                  */
/* Return  :                                                                                                                        */
/*    void                                                                                                                          */
/*                                                                                                                                  */
/************************************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static void TcpIp_Prv_GetIPv6linkLocalIpAddr( uint8 EthIfCtrlIdx_u8,
                                              TcpIp_IPAddrParamType_tun * IPAddrParam_pun )
{
    /* Local variables */
    uint8 lPhysAddr[ETHIF_HWADDR_LEN] = {0};

    /* The IPv6 link-local address for an Ethernet interface is formed by appending the Interface Identifier to the prefix FE80::/64 */
    IPAddrParam_pun->IPv6Par_st.IpAddr_st.Addr_au32[0] = 0xFE800000uL;
    IPAddrParam_pun->IPv6Par_st.IpAddr_st.Addr_au32[1] = 0x00000000uL;

    /* Fetch the MAC address for the EthCtrl mapped to the EthIfCtrl. */
    EthIf_GetPhysAddr( EthIfCtrlIdx_u8,
                       &lPhysAddr[0] );

    /* Construct the interface identifier from EUI-64 as specified in RFC 2464. */

   /* The OUI of the Ethernet address (the first three octets) becomes the company_id of the EUI-64 (the first three octets).  The fourth and */
   /* fifth octets of the EUI are set to the fixed value FFFE hexadecimal. The last three octets of the Ethernet address become the last three octets of the EUI-64. */

   /* The Interface Identifier is then formed from the EUI-64 by complementing the "Universal/Local" (U/L) bit, which is the next-to- */
   /* lowest order bit of the first octet of the EUI-64.  */

    /* Complement the U/L bit of the first octet of the MAC address fetched,.by using XOR bitwise operation with 0x02. */
    lPhysAddr[0] = lPhysAddr[0]^0x02U;

    IPAddrParam_pun->IPv6Par_st.IpAddr_st.Addr_au32[2] = (uint32)((((uint32) lPhysAddr[0]) << 24) | (((uint32) lPhysAddr[1]) << 16) |
                                                                  (((uint32) lPhysAddr[2]) << 8) | 0xFFU );

    IPAddrParam_pun->IPv6Par_st.IpAddr_st.Addr_au32[3] =  (uint32)( 0xFE000000uL | (((uint32) lPhysAddr[3]) << 16) | (((uint32) lPhysAddr[4]) << 8) | lPhysAddr[5] );
}

#endif

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
