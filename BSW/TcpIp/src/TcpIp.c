

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED

#include "EthIf.h"
#if (!defined(ETHIF_AR_RELEASE_MAJOR_VERSION) || (ETHIF_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched - EthIf and TcpIp"
#endif
#if (!defined(ETHIF_AR_RELEASE_MINOR_VERSION) || (ETHIF_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - EthIf and TcpIp"
#endif

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp.h"
#endif

#if ( TCPIP_NDP_ENABLED == STD_ON )
#include "rba_EthNdp.h"
#endif

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp.h"
#endif

#if ( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif

#if ( TCPIP_ICMP_ENABLED == STD_ON )
#include "rba_EthIcmp.h"
#endif

#if ( TCPIP_ICMPV6_ENABLED == STD_ON )
#include "rba_EthIcmpV6.h"
#endif

#if ( TCPIP_AUTOIP_ENABLED == STD_ON )
#include "rba_EthAutoIp.h"
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCP.h"
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
#include "rba_EthDHCPv6.h"
#endif

#include "TcpIp_Prv.h"

/* If RTE is enabled, include SchM_TcpIp.h header file */
#if( TCPIP_ECUC_RB_RTE_IN_USE == STD_ON )
#include "SchM_TcpIp.h"
#endif

#if ( TCPIP_DEV_ERROR_DETECT != STD_OFF )
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif
#endif /* ( TCPIP_DEV_ERROR_DETECT != STD_OFF ) */


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_VAR_INIT_8
#include "TcpIp_MemMap.h"
/* Flag to indicate whether TcpIp_Init has been executed or not since power ON */
boolean TcpIp_InitFlag_b = FALSE;
#define TCPIP_STOP_SEC_VAR_INIT_8
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_VAR_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"
const TcpIp_ConfigType * TcpIp_CurrConfig_pco = NULL_PTR;
#define TCPIP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_VAR_INIT_32
#include "TcpIp_MemMap.h"
uint32 TcpIp_RandNumSeed1_u32 = 100uL;
uint32 TcpIp_RandNumSeed2_u32 = 200uL;
#define TCPIP_STOP_SEC_VAR_INIT_32
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/*********************************************************************************************************************/
/*                                                                                                                   */
/* TcpIp_Init()     - This service initializes the TCP/IP Stack. TcpIp_Init may not block the start-up process for   */
/*                  an indefinite amount of time.                                                                    */
/*                                                                                                                   */
/* Service ID       - 0x01                                                                                           */
/* Sync/Async       - Synchronous                                                                                    */
/* Reentrancy       - Non Reentrant                                                                                  */
/*                                                                                                                   */
/* Input Parameters :                                                                                                */
/* ConfigPtr        - Pointer to the configuration data of the TcpIp module.                                         */
/*                                                                                                                   */
/* Output Parameters :  void                                                                                         */
/*                                                                                                                   */
/*********************************************************************************************************************/
void TcpIp_Init( const TcpIp_ConfigType * ConfigPtr )
{
    /* Local variable declaration */
    TcpIp_CurrAsgnedAddr_tst *      lCurrAsgnedAddr_pst;
    uint8                           lIdx1_u8;
#if ( TCPIP_CFG_CONFIGURATION_VARIANT != TCPIP_CFG_VARIANT_PRE_COMPILE )
    boolean                         lCfgValid_b;
#endif
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
    uint16                          lCurrentIdx_u16;
#endif

    /* Reset global variables. */
    TcpIp_InitFlag_b            = FALSE;
    TcpIp_CurrConfig_pco        = NULL_PTR;

#if ( TCPIP_CFG_CONFIGURATION_VARIANT != TCPIP_CFG_VARIANT_PRE_COMPILE )
    /* Initialize the config valid flag to FALSE */
    lCfgValid_b = FALSE;

    for( lIdx1_u8 = 0; lIdx1_u8 < TCPIP_NUM_CONFIG; lIdx1_u8++ )
    {
        if ( ( &TcpIp_ConfigSet[lIdx1_u8] ) == ConfigPtr ) /* TcpIp_Config is the post build dataset */
        {
            lCfgValid_b = TRUE;
            break;
        }
    }

    /* Report DET if ConfigPtr does not contain a valid configuration */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == lCfgValid_b ), TCPIP_E_INIT_API_ID, TCPIP_E_INIT_FAILED )

    /* Save the given configuration pointer */
    TcpIp_CurrConfig_pco = ConfigPtr;

#else

    /* Report DET if ConfigPtr is not NULL_PTR */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR != ConfigPtr ), TCPIP_E_INIT_API_ID, TCPIP_E_INIT_FAILED )

    /* If the variant is pre-compile (NULL_PTR is passed as input parameter), */
    /* intialize global configuration pointer with default configuration. */
    TcpIp_CurrConfig_pco = &TcpIp_ConfigSet[0];

    /* Ignore passed parameter. */
    (void)ConfigPtr;

#endif

    /* Copy configured IP address table to RAM. Because it should also be possible to change IP addresses during runtime */
    for( lIdx1_u8 = 0; ( lIdx1_u8 < ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ); lIdx1_u8++ )
    {
        /* Initialise the pointer */
        lCurrAsgnedAddr_pst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].CurrAsgnedAddr_pst );

        switch(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].IpDomainType_u32)
        {
#if (TCPIP_IPV4_ENABLED == STD_ON)
            case TCPIP_AF_INET:
            {
                ( lCurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32 )   = TCPIP_IPADDR_ZERO;
                ( lCurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32 )   = TCPIP_IPADDR_ZERO;
                ( lCurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.NetMask_u32 )  = TCPIP_IPV4_NETMASK_INVALID;

                TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv4Par_st.DftRtr_u32  = TCPIP_IPADDR_ZERO;
                TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv4Par_st.IpAddr_u32  = TCPIP_IPADDR_ZERO;
                TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv4Par_st.NetMask_u32 = TCPIP_IPV4_NETMASK_INVALID;

                /* Initialise ManualStaticIpAddr_pun to the default values if Assignment method is Static and Trigger is Manual */
                if( (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].AddrAsgnment_pcst->AddrAsgnmentMeth_en == TCPIP_IPADDR_ASSIGNMENT_STATIC ) &&
                        (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].AddrAsgnment_pcst->AddrAsgnmentTrig_en == TCPIP_MANUAL ) )
                {
                    TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.IpAddr_u32 = TCPIP_IPADDR_ZERO;
                    TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.DftRtr_u32 = TCPIP_IPADDR_ZERO;
                    TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.NetMask_u32 = TCPIP_IPV4_NETMASK_INVALID;
                }
            }
            break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
            case TCPIP_AF_INET6:
            {
                rba_EthIPv6_SetIPv6AddrTo0(&(lCurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0]));
                rba_EthIPv6_SetIPv6AddrTo0(&(lCurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[0]));
                lCurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.NetMask_u8 = 0;

                rba_EthIPv6_SetIPv6AddrTo0( &(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv6Par_st.IpAddr_st.Addr_au32[0]));
                rba_EthIPv6_SetIPv6AddrTo0( &(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv6Par_st.DftRtr_st.Addr_au32[0]));
                TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignAddressParam_un.IPv6Par_st.NetMask_u8 = 0;

                /* Initialise ManualStaticIpAddr_pun to the default values if Assignment method is Static and Trigger is Manual */
                if( (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].AddrAsgnment_pcst->AddrAsgnmentMeth_en == TCPIP_IPADDR_ASSIGNMENT_STATIC ) &&
                                        (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].AddrAsgnment_pcst->AddrAsgnmentTrig_en == TCPIP_MANUAL ) )
                {
                    rba_EthIPv6_SetIPv6AddrTo0(&(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.IpAddr_st.Addr_au32[0]));
                    rba_EthIPv6_SetIPv6AddrTo0(&(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.DftRtr_st.Addr_au32[0]));
                    TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.NetMask_u8 = 0;

                }
            }
            break;
#endif

            default:
            {
                /* Do nothing */
            }
            break;
        }

        ( lCurrAsgnedAddr_pst->CurrAsgnedAddrPrio_u8 )  = TCPIP_IPADDR_ASSIGNPRIO_INVALID;
        ( lCurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )  = TCPIP_IPADDR_ASSIGNMENT_INVALID;

        /* Initialize the local IP change address structure members to initial values. */
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignPrio_u8 = 0xFFU;
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqAssignType_en = TCPIP_IPADDR_ASSIGNMENT_INVALID;
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpChgAssigned_pst->ReqUnassignType_u8 = 0U;

        /* Initialize the request and release assignment bits to zero. */
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->RequestAddrAssgnMeth_u8 = 0x00;
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8 = 0x00;

        /* Initialize socket operation status */
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].SockOptStatus_pst->BindingSockCount_u16 = 0U;
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].SockOptStatus_pst->SendingDataCount_u16 = 0U;
        TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lIdx1_u8].SockOptStatus_pst->ClosingSockInProgress_b = FALSE;
    }

    /* Set all EthIf controller state configured in TcpIp to TCPIP_STATE_OFFLINE and also TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8*/
    for( lIdx1_u8 = 0; lIdx1_u8 < TCPIP_NUMETHIFCTRL; lIdx1_u8++ )
    {
        TcpIp_EthIfCtrlSt_aen[lIdx1_u8]     = TCPIP_STATE_OFFLINE;
        TcpIp_EthIfCtrlReqSt_aen[lIdx1_u8]  = TCPIP_STATE_OFFLINE;
        TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lIdx1_u8] = 0U;
    }

#if (TCPIP_IPV4_ENABLED == STD_ON)
    rba_EthIPv4_Init( TcpIp_CurrConfig_pco->EthIPv4Config_pco );
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    rba_EthIPv6_Init( TcpIp_CurrConfig_pco->EthIPv6Config_pco );
#endif

#if (TCPIP_ARP_ENABLED == STD_ON)
    rba_EthArp_Init( TcpIp_CurrConfig_pco->EthArpConfig_pco );
#endif

#if (TCPIP_UDP_ENABLED == STD_ON)
    rba_EthUdp_Init( TcpIp_CurrConfig_pco->EthUdpConfig_pco );
#endif

#if (TCPIP_TCP_ENABLED == STD_ON)
    rba_EthTcp_Init( TcpIp_CurrConfig_pco->EthTcpConfig_pco );
#endif

#if (TCPIP_TLS_ENABLED == STD_ON)
    rba_EthTls_Init( TcpIp_CurrConfig_pco->EthTlsConfig_pco );
#endif
#if (TCPIP_AUTOIP_ENABLED == STD_ON)
    rba_EthAutoIp_Init( TcpIp_CurrConfig_pco->EthAutoIpConfig_pco );
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    rba_EthDHCP_Init( TcpIp_CurrConfig_pco->EthDHCPConfig_pco );
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
    rba_EthDHCPv6_Init( TcpIp_CurrConfig_pco->EthDHCPv6Config_pco );
#endif

#if (TCPIP_ICMP_ENABLED == STD_ON)
    rba_EthIcmp_Init( TcpIp_CurrConfig_pco->EthIcmpConfig_pco );
#endif

#if (TCPIP_ICMPV6_ENABLED == STD_ON)
    rba_EthIcmpV6_Init( TcpIp_CurrConfig_pco->EthIcmpV6Config_pco );
#endif

#if (TCPIP_NDP_ENABLED == STD_ON)
    rba_EthNdp_Init( TcpIp_CurrConfig_pco->EthNdpConfig_pco );
#endif

    TcpIp_RandNumSeed1_u32 = 100uL;
    TcpIp_RandNumSeed2_u32 = 200uL;

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
    /* For all TcpIp sockets */
    for( lCurrentIdx_u16 = 0U; lCurrentIdx_u16 < TCPIP_SOCKETMAX; lCurrentIdx_u16++ )
    {
        TcpIp_DynSockAllocatedZeroCopyDesc_au16[lCurrentIdx_u16] = 0xFFFFU;
    }

    /* For all ZeroCopy descriptors */
    for( lCurrentIdx_u16 = 0U; lCurrentIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT; lCurrentIdx_u16++ )
    {
        TcpIp_ZeroCopyClearDesc( lCurrentIdx_u16 );
    }
#endif

    /* Set a flag, to indicate that the TcpIp_Init() has been called */
    TcpIp_InitFlag_b = TRUE;

    return;
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_MainFunction  - Schedules the TCP/IP stack. (Entry point for scheduling)                                   */
/*                                                                                                                  */
/* Service ID          - 0x15                                                                                       */
/* Sync/Async       - Synchronous                                                                                   */
/* Reentrancy       - Non Reentrant                                                                                 */
/*                                                                                                                  */
/* Scheduled function (from [SWS_TcpIp_00026])                                                                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
void TcpIp_MainFunction( void )
{
    TcpIp_StateType                 lEthIfCtrlStBeforeTrans_aen[TCPIP_NUMETHIFCTRL];
    uint8                           lLocalAddrPerEthIfCtrl_au8[TCPIP_NUMETHIFCTRL];
    uint8                           lEthIfCtrlIdx_u8;
    TcpIp_LocalAddrIdType           lLocalAddrId_u8;
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
    uint16                          lCurrentIdx_u16;
    Std_ReturnType                  lRetValStdType_en;
#endif

    /* Check whether TcpIp_Init() has been called */
    if( FALSE != TcpIp_InitFlag_b )
    {
        /* -------------------------------------- */
        /* TcpIp State Machine and Ip Req/Rel     */
        /* -------------------------------------- */

        /* Ctrl state will updated while looping though lLocalAddrId_u8(below), and required current state information will be lost */
        /* Hence it's necessary to have transition based on the locally stored initial controller state */
        for( lEthIfCtrlIdx_u8 = 0U; lEthIfCtrlIdx_u8 <TCPIP_NUMETHIFCTRL ; lEthIfCtrlIdx_u8++)
        {
            lEthIfCtrlStBeforeTrans_aen[lEthIfCtrlIdx_u8] = TcpIp_EthIfCtrlSt_aen[lEthIfCtrlIdx_u8];
            lLocalAddrPerEthIfCtrl_au8[lEthIfCtrlIdx_u8]  = TcpIp_AssgndLocalAddrCntPerEthIfCtrl_au8[lEthIfCtrlIdx_u8];
        }

        /* Loop through all configured LocalAddrIds. */
        for( lLocalAddrId_u8 = 0U; lLocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8; lLocalAddrId_u8++ )
        {
            /* Call the TcpIp state machine processing for each LocalAddrId. */
            TcpIp_StateMachine( lLocalAddrId_u8,
                                &lEthIfCtrlStBeforeTrans_aen[0],
                                &lLocalAddrPerEthIfCtrl_au8[0] );

            /* Call the TcpIp request/release IP assignment main function processing for each LocalAddrId. */
            TcpIp_IpReqRelMainFuncProc( lLocalAddrId_u8 );
        }


        /* -------------------------------------- */
        /* Sub-modules calls                      */
        /* -------------------------------------- */

#if ( TCPIP_TCP_ENABLED == STD_ON )
        rba_EthTcp_MainFunction();
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
        rba_EthUdp_MainFunction();
#endif

#if (TCPIP_TLS_ENABLED == STD_ON)
       rba_EthTls_MainFunction();
#endif

#if ( TCPIP_ARP_ENABLED == STD_ON )
        rba_EthArp_MainFunction();
#endif

#if ( TCPIP_AUTOIP_ENABLED == STD_ON )
        rba_EthAutoIp_MainFunction();
#endif

#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
        rba_EthDHCP_MainFunction();
#endif

#if ( TCPIP_DHCPV6_CLIENT_ENABLED == STD_ON )
        rba_EthDHCPv6_MainFunction();
#endif

#if ( defined(RBA_ETHIPV4_REASSEMBLY_ENABLED) && ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON ) )
        rba_EthIPv4_MainFunction();
#endif

#if ( TCPIP_NDP_ENABLED == STD_ON )
        rba_EthNdp_MainFunction();
#endif

        /* -------------------------------------- */
        /* Local Ip Change                        */
        /* -------------------------------------- */

        /* Loop through all configured LocalAddr and trigger Local IP change main function processing. */
        /* The API needs to triggered after all the sub-module main functions so that if there is any local IP change notified */
        /* by submodules is immediately processed in the same TcpIp_Mainfunction. */
        for( lLocalAddrId_u8 = 0U; lLocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8; lLocalAddrId_u8++ )
        {
            /* Call the TcpIp local IP address change main processing for each LocalAddrId. */
            TcpIp_LocalIpChgMainFuncProc( lLocalAddrId_u8 );
        }


        /* -------------------------------------- */
        /* ZeroCopy handling                      */
        /* -------------------------------------- */

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
        /* Try again to close the ZeroCopy descriptors still requested to be closed */
        for( lCurrentIdx_u16 = 0U; lCurrentIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT; lCurrentIdx_u16++ )
        {
            if( TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].DescState_en == TCPIP_ZEROCOPY_DESC_REQ_CLOSE )
            {
                /* Check if the reset of the descriptor is now allowed */
                lRetValStdType_en = TcpIp_ZeroCopyIsResetAllowed( lCurrentIdx_u16 );

                if( lRetValStdType_en == E_OK )
                {
                    /* Clear the ZeroCopy descriptor */
                    TcpIp_ZeroCopyClearDesc( lCurrentIdx_u16 );
                }
            }
        }
#endif
    }

    return;
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_AllocBaseIpAddr()  - This API provides the interface between SoAd and the rba_EthAutoIp module,            */
/*                            to provide the baseIp address stored in the NVM.                                      */
/*                                                                                                                  */
/* Service ID               - 0x18                                                                                  */
/* Sync/Async               - Synchronous                                                                           */
/* Reentrancy               - Non Reentrant                                                                         */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8       - Local address id reference to IPv4 address config and EthIf controller index              */
/* IpAddrAsgnMethType_en - Address assignment method (Static/DHCP/AutoIp) used by this LocalAddrId                  */
/* LocalIpAddr_pst          - Pointer holding the BaseIp address to be used by the AutoIp                           */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  NIL                                                                                                             */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/* Std_ReturnType       -   Result of operation                                                                     */
/*                          E_OK - Request processed successfully                                                   */
/*                          E_NOT_OK - Request NOT processed successfully                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_AllocBaseIpAddr( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                      TcpIp_IpAddrAssignmentType IpAddrAsgnMethType_en,
                                      const TcpIp_SockAddrType * LocalIpAddr_pst )
{
#if ( TCPIP_AUTOIP_ENABLED == STD_ON )
    /* Local variable declaration */
    Std_ReturnType                  lRetVal_en;
    const TcpIp_SockAddrInetType *  lIPv4IPAddrPort_pcst;

    /* Initialize local variables */
    lRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_ALLOC_BASE_IPADDR_API_ID,
                                      TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                      TCPIP_E_ALLOC_BASE_IPADDR_API_ID , TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET if LocalIpAddr_pst is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == LocalIpAddr_pst ), TCPIP_E_ALLOC_BASE_IPADDR_API_ID,
                                      TCPIP_E_NULL_PTR, E_NOT_OK )

    if( ((TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL == IpAddrAsgnMethType_en) ||
         (TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP == IpAddrAsgnMethType_en)) &&
        (LocalIpAddr_pst->domain == TCPIP_AF_INET) )
    {
        /* MR12 RULE 11.3 VIOLATION: typecasted to TcpIp_SockAddrInetType to match the local parameter Type definition */
        lIPv4IPAddrPort_pcst = ( (const TcpIp_SockAddrInetType * ) LocalIpAddr_pst );

        /* Check whether the IP address of the remote node is in the AutoIp address range */
        if( ( TCPIP_AUTOIP_RANGE_START          <= ( lIPv4IPAddrPort_pcst->addr[0] ) ) &&
            ( ( lIPv4IPAddrPort_pcst->addr[0] ) <= TCPIP_AUTOIP_RANGE_END ) )
        {
            lRetVal_en = rba_EthAutoIp_AllocBaseIpAddr( LocalAddrId_u8, lIPv4IPAddrPort_pcst );
        }
    }

    return lRetVal_en;
#else
    (void)LocalAddrId_u8;
    (void)IpAddrAsgnMethType_en;
    (void)LocalIpAddr_pst;
    return E_NOT_OK;
#endif
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_GetCtrlIdx()  - TcpIp_GetCtrlIdx returns the index of the controller related to LocalAddrId.               */
/*                                                                                                                  */
/* Service ID          - 0x17                                                                                       */
/* Sync/Async          - Synchronous                                                                                */
/* Reentrancy          - Reentrant                                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId       - Local address identifier implicitly specifying the EthIf controller that shall be returned.  */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* CtrlIdxPtr        - Pointer to the memory where the index of the controller related to LocalAddrId is stored     */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/* Std_ReturnType       -   Result of operation                                                                     */
/*                          E_OK - Request processed successfully                                                   */
/*                          E_NOT_OK - Request NOT processed successfully                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_GetCtrlIdx( TcpIp_LocalAddrIdType LocalAddrId,
                                 uint8 * CtrlIdxPtr )
{
    /* Local variable declaration */
    Std_ReturnType                lRetVal_en;

    /* Initialize local variables */
    lRetVal_en = E_NOT_OK;

    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_GET_CTRLIDX_API_ID,
                                      TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                        TCPIP_E_GET_CTRLIDX_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET if CtrlIdxPtr is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == CtrlIdxPtr ), TCPIP_E_GET_CTRLIDX_API_ID,
                                      TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Fetch the EthIf CtrlIdx corresponding to the LocalAddrId */
    *CtrlIdxPtr = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].EthIfCtrlRef_u8 );

    /* return E_OK */
    lRetVal_en = E_OK;

    return lRetVal_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
