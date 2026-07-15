

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

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

static void TcpIp_StTransStartUpToOffline( TcpIp_LocalAddrIdType LocalAddrId_u8 );

static void TcpIp_StTransOnlineOnholdShutdownToOffline( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                uint8 * AssgnedLocalAddrPerEthIfCtrl_au8 );

#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
static Std_ReturnType TcpIp_CheckLinkLocalAddressOnly( uint8 EthIfCtrl_u8 );
static uint8 TcpIp_GetLinkLocalAddressCount( uint8 EthIfCtrl_u8 );
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

/****************************************************************************************************************************/
/* TcpIp_StateMachine() - This API is called from TcpIp_MainFunction(). The TcpIp_RequestComMode() stores the state         */
/*                        transition requests from EthSM and are processed in TcpIp_StateMachine()                          */
/*                                                                                                                          */
/* Sync/Async            - Synchronous                                                                                      */
/* Reentrancy            - Non Reentrant                                                                                    */
/*                                                                                                                          */
/* Input Parameters :                                                                                                       */
/* LocalAddrId_u8:                   - Idx of the LocalAddress for which the state machine will be processed.               */
/* EthIfCtrlStBeforeTrans_caen       - Current TcpIpCtrl states stored before the execution of the state machine processing.*/
/* AssgnedLocalAddrPerEthIfCtrl_au8  - Count of localAddrIds which have IP address assigned per EthIfCtrl.                  */
/*                                                                                                                          */
/* Return           : void                                                                                                  */
/*                                                                                                                          */
/****************************************************************************************************************************/
/* HIS METRIC LEVEL VIOLATION IN TcpIp_StateMachine: This API contains if-elseif-else-case for various combinations of TcpIp states and is not possible to avoid these HIS in-compliances. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
void TcpIp_StateMachine( TcpIp_LocalAddrIdType LocalAddrId_u8,
                         const TcpIp_StateType * EthIfCtrlStBeforeTrans_caen,
                         uint8 * AssgnedLocalAddrPerEthIfCtrl_au8 )
{
    /* Local variables declaration */
    TcpIp_StateType                     lEthIfCtrlCurrSt_en;
    TcpIp_StateType                     lEthIfCtrlReqSt_en;
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint8                               lAddrAssgntMethIdx_u8;
    uint8                               lloopIdx_u8;

    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );
    lEthIfCtrlCurrSt_en   = ( EthIfCtrlStBeforeTrans_caen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] );
    lEthIfCtrlReqSt_en    = ( TcpIp_EthIfCtrlReqSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8  ] );

    /************ Initiate IP assignment for the local Address ************/
    /*If the current state is TCPIP_STATE_OFFLINE, initiate IP assignment only if requested state is TCPIP_STATE_ONLINE. */
    if(( TCPIP_STATE_ONLINE == lEthIfCtrlReqSt_en) && ( TCPIP_STATE_OFFLINE == lEthIfCtrlCurrSt_en ))
    {
        /* Loop through all AddressAssignments configured for the requested LocalAddress */
        /* If the current state is TCPIP_STATE_OFFLINE */
        for( lAddrAssgntMethIdx_u8 = 0;( lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8 ); lAddrAssgntMethIdx_u8++ )
        {
            /* If the IP assignment trigger is AUTOMATIC set the corresponding bit. */
            /* Note: This bit is also set by TcpIp_RequestIpAddrAssignment() for MANUAL IP assignment trigger.    */
            if( TCPIP_AUTOMATIC == ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentTrig_en ))
            {
                /* Enter critical section: The request assignment method variable can be updated in different execution context and should be protected in a critical section. */
                SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

                TCPIP_UINT8_SET_BIT_WITH_SHIFT((lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8), (lAddrAssgntMethIdx_u8));

                /* Exit critical section. */
                SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
            }
        }

        /* Change the TcpIpCtrl state to startup. */
        TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] = TCPIP_STATE_STARTUP;
    }

    /******************************* Transition from TCPIP_STATE_STARTUP to TCPIP_STATE_OFFLINE ************************************/
    else if( ( TCPIP_STATE_STARTUP == lEthIfCtrlCurrSt_en ) && ( TCPIP_STATE_OFFLINE == lEthIfCtrlReqSt_en ) )
    {
        TcpIp_StTransStartUpToOffline( LocalAddrId_u8 );
    }

    /************************************ Transition from TCPIP_STATE_ONLINE to TCPIP_STATE_ONHOLD ************************************/
    else if( ( TCPIP_STATE_ONLINE == lEthIfCtrlCurrSt_en ) && ( TCPIP_STATE_ONHOLD == lEthIfCtrlReqSt_en ) )
    {
        /* Check whether this LocalAddress has IP assigned already */
        if(TCPIP_IPADDR_ASSIGNMENT_INVALID != lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en)
        {
            TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] = TCPIP_STATE_ONHOLD;

            #if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
            if((TCPIP_IPADDR_ASSIGNMENT_DHCP == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en) &&
               (TCPIP_AF_INET6 == lLocalAddrConfig_pcst->IpDomainType_u32))
            {
                (void)rba_EthDHCPv6_RunModeRequest(LocalAddrId_u8, RBA_ETHDHCPV6_RUNMODE_ONHOLD);
            }
            #endif

            /* Inform all upper layers that corresponding LocalAddress has enter ONHOLD state */
            for( lloopIdx_u8 = 0; lloopIdx_u8 < TcpIp_CurrConfig_pco->NumSockOwners_cu8; lloopIdx_u8++ )
            {
                if(( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lloopIdx_u8].Up_LocalIpAddrAssignmentChg_pfct != NULL_PTR ) &&
                   ( LocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrUserConfigured_u8 ))
                {
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lloopIdx_u8].Up_LocalIpAddrAssignmentChg_pfct( LocalAddrId_u8,
                                                                                                               TCPIP_IPADDR_STATE_ONHOLD );
                }
            }
        }
    }

    /************************************ Transition from TCPIP_STATE_ONHOLD to TCPIP_STATE_ONLINE ************************************/
    else if( ( TCPIP_STATE_ONHOLD == lEthIfCtrlCurrSt_en ) && ( TCPIP_STATE_ONLINE == lEthIfCtrlReqSt_en ) )
    {
        /* Check whether this LocalAddress has IP assigned already */
        if(TCPIP_IPADDR_ASSIGNMENT_INVALID != lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en)
        {
            TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] = TCPIP_STATE_ONLINE;

            /* Inform all upper layers that corresponding LocalAddress has entered ONLINE state  */
            for( lloopIdx_u8 = 0; lloopIdx_u8 < TcpIp_CurrConfig_pco->NumSockOwners_cu8; lloopIdx_u8++ )
            {
                if(( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lloopIdx_u8].Up_LocalIpAddrAssignmentChg_pfct != NULL_PTR ) &&
                  ( LocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrUserConfigured_u8 ))
                {
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[lloopIdx_u8].Up_LocalIpAddrAssignmentChg_pfct( LocalAddrId_u8,
                                                                                                               TCPIP_IPADDR_STATE_ASSIGNED );
                }
            }

#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
            if((TCPIP_IPADDR_ASSIGNMENT_DHCP == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en) &&
               (TCPIP_AF_INET6 == lLocalAddrConfig_pcst->IpDomainType_u32))
            {
                (void)rba_EthDHCPv6_RunModeRequest(LocalAddrId_u8, RBA_ETHDHCPV6_RUNMODE_ONLINE);
            }
#endif
        }
    }

    /****************************** Transition from TCPIP_STATE_ONLINE/TCPIP_STATE_ONHOLD to TCPIP_STATE_OFFLINE ******************************/
    else if( ( (TCPIP_STATE_ONLINE == lEthIfCtrlCurrSt_en) || (TCPIP_STATE_ONHOLD == lEthIfCtrlCurrSt_en) ) &&
             ( TCPIP_STATE_OFFLINE == lEthIfCtrlReqSt_en ) )
    {
#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
        if( E_NOT_OK == TcpIp_IsIPv6LinkLocalAddress(LocalAddrId_u8))
#endif
        {
            TcpIp_StTransOnlineOnholdShutdownToOffline( LocalAddrId_u8,
                                            AssgnedLocalAddrPerEthIfCtrl_au8 );
        }
    }
    /****************************** Transition from TCPIP_STATE_SHUTDOWN to TCPIP_STATE_OFFLINE ******************************/
    else if ((TCPIP_STATE_SHUTDOWN == lEthIfCtrlCurrSt_en) && (TCPIP_STATE_OFFLINE == lEthIfCtrlReqSt_en))
    {
#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
        if(TcpIp_CheckLinkLocalAddressOnly(lLocalAddrConfig_pcst->EthIfCtrlRef_u8) == E_OK)
        {
            // Release address the last Link Local address
            TcpIp_StTransOnlineOnholdShutdownToOffline( LocalAddrId_u8,
                                                        AssgnedLocalAddrPerEthIfCtrl_au8 );
        }
#endif
    }
    else
    {
        /* Do nothing */
    }

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* TcpIp_StTransStartUpToOffline() - This API is called from TcpIp_StateMachine(). It handles the transition        */
/*                      requests from STARTUP to OFFLINE.                                                           */
/*                                                                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8     - Idx of the LocalAddress for which IP assignment process needs to be stopped                */
/*                                                                                                                  */
/* Return  :            void                                                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
static void TcpIp_StTransStartUpToOffline( TcpIp_LocalAddrIdType LocalAddrId_u8 )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *        lLocalAddrConfig_pcst;
    uint8                                   lAddrAssgntMethIdx_u8;

    /* Initialize the pointer to the local address configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Loop through all AddresAssignments configured for the requested LocalAddress */
    for( lAddrAssgntMethIdx_u8 = 0U; ( lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8 ); lAddrAssgntMethIdx_u8++ )
    {
        /* If AutoIp is configured for the LocalAddress, stop AutoIp process */
        if( (TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL == ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en )) ||
          (TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP == ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en )))
        {
#if (TCPIP_AUTOIP_ENABLED == STD_ON)
            /* Check whether domain is TCPIP_AF_INET. */
            if(TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32)
            {
                /* Stop AutoIp process. */
                (void)rba_EthAutoIp_Stop( LocalAddrId_u8 );
            }
#endif
        }
        /* If Dhcp is configured for the LocalAddress, stop Dhcp process */
        else if( TCPIP_IPADDR_ASSIGNMENT_DHCP == ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en ) )
        {
            if(TCPIP_AF_INET == lLocalAddrConfig_pcst->IpDomainType_u32)
            {
#if (TCPIP_DHCP_CLIENT_ENABLED == STD_ON)
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
        else if(TCPIP_IPADDR_ASSIGNMENT_IPV6_ROUTER ==  ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentMeth_en )  )
        {
            /*Stop Ndp process */
#if (TCPIP_NDP_PRD_PREFIXROUTERDISC_ENABLED == STD_ON)
            (void)rba_EthNdp_Stop( LocalAddrId_u8 );
#endif
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Enter critical section: The request and release assignment method variable can be updated in different execution context and should be protected in a critical section. */
    SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

    lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8 = 0x00U;
    lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8 = 0x00U;

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();

    /* Set TcpIpCtrl state to OFFLINE */
    TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] = TCPIP_STATE_OFFLINE;

    if( lLocalAddrConfig_pcst->IpDomainType_u32 == TCPIP_AF_INET )
    {
#if (TCPIP_ARP_ENABLED == STD_ON)
        /* Clean the ARP table corresponding to the EthIf controller */
        rba_EthArp_CleanArpTable( lLocalAddrConfig_pcst->EthIfCtrlRef_u8 );    /* Applicable only for IPv4 specific addresses */
#endif
    }
    else
    {
#if( TCPIP_NDP_ENABLED == STD_ON )
        /* Clean the NDP table corresponding to the EthIf controller. */
        rba_EthNdp_CleanNdpTable(lLocalAddrConfig_pcst->EthIfCtrlRef_u8);                       /* Applicable only for IPv6 specific addresses */
#endif
    }

    return;
}


/********************************************************************************************************************/
/* TcpIp_StTransOnlineOnholdShutdownToOffline() - This API is called from TcpIp_StateMachine(). It handles          */
/*                                       the transition requests from ONLINE, ONHOLD or SHUTDOWN to OFFLINE.        */
/*                                                                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8                    - Idx of the LocalAddress for which IP assignment process needs to be stopped. */
/* AssgnedLocalAddrPerEthIfCtrl_au8  - Count of localAddrIds which have IP address assigned per EthIfCtrl.          */
/*                                                                                                                  */
/* Return  :            void                                                                                        */
/*                                                                                                                  */
/********************************************************************************************************************/
static void TcpIp_StTransOnlineOnholdShutdownToOffline( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                uint8 * AssgnedLocalAddrPerEthIfCtrl_au8 )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint8                               lAddrAssgntMethIdx_u8;

    /* Initialize the pointer to the local address configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* -------------------------------------- */
    /* Set release flag                       */
    /* -------------------------------------- */

    /* Release the address assignement */
    /* (in case of CurrAsgnedAddrMeth_en is INVALID, the releasing of addresses is also done) */
    /* (it is required to stop the assignments even if assignement is INVALID because during probing sequence, the assignement is not yet completed but it still need to be stopped) */
    /* Loop through all address assignment methods configured for the LocalAddrId and stop the assignment methods */
    for( lAddrAssgntMethIdx_u8 = 0; lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8; lAddrAssgntMethIdx_u8++ )
    {
        /* Enter critical section: The release address assignment method variable can be updated in different execution context and should be protected in a critical section. */
        SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

        /* Set the bit for releasing IP address assignment method. */
        lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8 |= ((uint8)(1u << lAddrAssgntMethIdx_u8)) ;

        /* Clear IP assignment pending request bit */
        lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8 &= (uint8)( ~(uint8)(1U << lAddrAssgntMethIdx_u8) );

        /* Exit critical section. */
        SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
    }

    /* -------------------------------------- */
    /* Close sockets                          */
    /* -------------------------------------- */

    /* Enter critical section (to avoid race condition if terminating and binding of sockets is executed concurrently) */
    SchM_Enter_TcpIp_ExclusiveAreaSockOperations();

    /* Set the ClosingSockInProgress_b to TRUE. */
    /* The flag will be reset to FALSE when release IP address assignment is executed. (when ReleaseAddrAssgnMeth_u8 is set,         */
    /* then the unassignment will be processed in TcpIp_IpReqRelMainFuncProc() and flag will be reset to FALSE during this process). */
    lLocalAddrConfig_pcst->SockOptStatus_pst->ClosingSockInProgress_b = TRUE;

    /* If there is no binding of sockets and packet transmission in progress, continue further processing. */
    if(( lLocalAddrConfig_pcst->SockOptStatus_pst->BindingSockCount_u16 == 0U ) &&
       ( lLocalAddrConfig_pcst->SockOptStatus_pst->SendingDataCount_u16 == 0U ))
    {
        /* Exit critical section (to avoid race condition if terminating and binding of sockets is executed concurrently) */
        SchM_Exit_TcpIp_ExclusiveAreaSockOperations();

        /* Release the already bound sockets. The sockets are bound to the currently assigned IP address method.    */
        /* Tcp RST frames need be sent before the DHCP releasing process takes place in case of DHCP Local Address. */
#if (TCPIP_TCP_ENABLED == STD_ON)
        rba_EthTcp_AbortByLocalAddrId(LocalAddrId_u8);
#endif

        /* Need to close all the UDP sockets mapped to LocalAddrId_cu8 except DHCP Client socket */
#if (TCPIP_UDP_ENABLED == STD_ON)
        rba_EthUdp_TerminateSocUsingLocalAddr(LocalAddrId_u8);
#endif

        /* -------------------------------------- */
        /* Enter SHUTDOWN state                   */
        /* -------------------------------------- */

        /* Check if the current assignment method is valid. */
        if(( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en != TCPIP_IPADDR_ASSIGNMENT_INVALID ) &&
           ( AssgnedLocalAddrPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] > 0U) )
        {
            /* If there is a valid currently assigned method, decrement the locally saved counter to keep record of number of LocalAddrIds referring to particular EthIfController */
            AssgnedLocalAddrPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8]--;
        }

        /* AssgnedLocalAddrPerEthIfCtrl_au8 holds count of active LocalAddrId mapped to each EthIfController, count 0 indicates no localAddrId has ref to that particular EthIfController. */
        /* SHUTDOWN state needs to be entered only when all the assigned addresses have been processed so that TCP RST can be sent for all the assigned addresses. */
        /* In case of Dhcpv6, the SHUTDOWN state need to be entered when all addresses except Link Local have been processed (Dhcpv6 uses the link local address to send release frame). */
        /* When SHUTDOWN is entered, no transmission or reception is possible anymore (except for Dhcpv4 adress or link local address in case of Dhcpv6). */
        if (
#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
            (AssgnedLocalAddrPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] == TcpIp_GetLinkLocalAddressCount(lLocalAddrConfig_pcst->EthIfCtrlRef_u8)) ||
#endif
            (AssgnedLocalAddrPerEthIfCtrl_au8[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] == 0U) )
        {
            /* Set the TcpIpCtrl state to SHUTDOWN */
            ( TcpIp_EthIfCtrlSt_aen[ lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ] ) = TCPIP_STATE_SHUTDOWN ;
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


/********************************************************************************************************************/
/* TcpIp_CheckLinkLocalAddressOnly() - This API check if only the Link Local address is remaining for the EthIfCtrl */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* EthIfCtrl_u8                    - Idx of the EthIfCtrl.                                                          */
/*                                                                                                                  */
/* Return  :                       Std_ReturnType                                                                   */
/*                                                                                                                  */
/********************************************************************************************************************/
#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
static Std_ReturnType TcpIp_CheckLinkLocalAddressOnly( uint8 EthIfCtrl_u8 )
{
    /* Local variable declaration */
    Std_ReturnType                      lRetVal_en;
    Std_ReturnType                      lRetValLinkLocalAddr_en;
    uint8                               lIdx_u8;

    /* Initialize local variable */
    lRetVal_en = E_OK;

    /* Check if there is a LL address on the same controller that is still assigned */
    for( lIdx_u8 = 0U; lIdx_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8; lIdx_u8++ )
    {
        /* Check if the current local address is a valid assigned IPv6 Link Local address */
        lRetValLinkLocalAddr_en = TcpIp_IsIPv6LinkLocalAddress( lIdx_u8 );

        /* If the address belongs to the LocalAddrId_u8 controller */
        /* And the address is not an IPv6 Link Local address */
        /* And address is assigned */
        if( (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx_u8].EthIfCtrlRef_u8 == EthIfCtrl_u8) &&
            (lRetValLinkLocalAddr_en == E_NOT_OK) &&
            (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx_u8].CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en != TCPIP_IPADDR_ASSIGNMENT_INVALID) )
        {
            lRetVal_en = E_NOT_OK;
        }
    }

    return lRetVal_en;
}
#endif

/********************************************************************************************************************/
/* TcpIp_GetLinkLocalAddressCount() - This API counts the number of LinkLocal addresses for the EthIfCtrl           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* EthIfCtrl_u8                    - Idx of the EthIfCtrl.                                                          */
/*                                                                                                                  */
/* Return  :                       uint8                                                                            */
/*                                                                                                                  */
/********************************************************************************************************************/
#if( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
static uint8 TcpIp_GetLinkLocalAddressCount( uint8 EthIfCtrl_u8 )
{
    /* Local variable declaration */
    uint8                               lLinkLocalCount_u8;
    Std_ReturnType                      lRetValLinkLocalAddr_en;
    uint8                               lIdx_u8;

    /* Initialize local variable */
    lLinkLocalCount_u8 = 0U;

    /* Check if there is a LL address on the same controller that is still assigned */
    for( lIdx_u8 = 0U; lIdx_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8; lIdx_u8++ )
    {
        /* Check if the current local address is a valid assigned IPv6 Link Local address */
        lRetValLinkLocalAddr_en = TcpIp_IsIPv6LinkLocalAddress( lIdx_u8 );

        /* If the address belongs to the LocalAddrId_u8 controller */
        /* And the address is an IPv6 Link Local address */
        if( (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx_u8].EthIfCtrlRef_u8 == EthIfCtrl_u8) &&
            (lRetValLinkLocalAddr_en == E_OK) )
        {
            lLinkLocalCount_u8++;
        }
    }

    return lLinkLocalCount_u8;
}
#endif


#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
