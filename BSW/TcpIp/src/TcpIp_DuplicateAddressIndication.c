

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#include "EthIf.h"
#include "TcpIp_Prv.h"
/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/**********************************************************************************************************************/
/* TcpIp_DuplicateAddressIndication() - This API is callback function called from Ipv6 modules when it has to         */
/*                                          inform TcpIp about the assigned IP address is Duplicate or Unique.        */
/*                                                                                                                    */
/* Sync/Async                             - Synchronous                                                               */
/* Reentrancy                             - Reentrant                                                                 */
/*                                                                                                                    */
/* Input Parameters :                                                                                                 */
/* LocalAddrId_u8           - IP address identifier representing the local IP address.                                */
/* DadState_en              - IP address is Duplicate or Unique.                                                      */
/* IPAddrParam_pcun         - Pointer to a memory where the conflicted IP address is stored.                          */
/* RemotePhysAddrPtr_pcu8   - Pointer to the memory where the remote physical address (MACaddress) related to the     */
/*                            specified IP address is stored in network byte order                                    */
/*                                                                                                                    */
/* Return :                   void                                                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/
void TcpIp_DuplicateAddressIndication( TcpIp_LocalAddrIdType                    LocalAddrId_u8,
                                       TcpIp_DadStateType_ten                   DadState_en,
                                       const TcpIp_IPAddrParamType_tun *        IPAddrParam_pcun,
                                       const uint8 *                            RemotePhysAddrPtr_pcu8)
{
#if ( TCPIP_IPV6_ENABLED == STD_ON )
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *         lLocalAddrConfig_pcst;
    uint8                                    lPhysAddr_au8[ETHIF_HWADDR_LEN];
    TcpIp_SockAddrInet6Type                  lIPv6IPAddr_pst;
    TcpIp_IpAddrAssignmentType               lAsgnedAddrMeth_en;
    boolean                                  lRetVal_b;
#endif

    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_DUPLICATEADDRESSINDICATION_API_ID, TCPIP_E_NOTINIT )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                        TCPIP_E_DUPLICATEADDRESSINDICATION_API_ID, TCPIP_E_INV_ARG )

    /* Report DET if IPAddrParam_pcun is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == IPAddrParam_pcun ), TCPIP_E_DUPLICATEADDRESSINDICATION_API_ID, TCPIP_E_NULL_PTR )

    /* For INET domain the IP address conflict handling is implemeted in Arp module.
     * In future handling for IPv4 can be carried out  in TcpIP Module */
#if ( TCPIP_IPV6_ENABLED == STD_ON )
    if(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].IpDomainType_u32 == TCPIP_AF_INET6 )
    {
        if(DadState_en == TCPIP_DAD_FAIL )
        {
            /* Update the pointer to LocalAddrId structure */
            lLocalAddrConfig_pcst   = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

            /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
            /* Hence the accesses to the current assigned address structure shall be protected in a critical section. */
            SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

            /* Update the current IP address assignment Method Type */
            lAsgnedAddrMeth_en = lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en;

            /*  Current assigned IPv6 address is equal to the IPv6 address in the IPAddrParam_pcun argument */
            /*  Call the comparison function only if the DadState_en is TCPIP_DAD_FAIL. */
            lRetVal_b = TcpIp_IPv6AddrCompareWithMask(&(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0]),
                                                          &(IPAddrParam_pcun->IPv6Par_st.IpAddr_st.Addr_au32[0]), TCPIP_IPV6_CIDR_MASK_128);

            /* Exit critical section. */
            SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

            /* Check whether localAddress has IP assigned */
            if(( TCPIP_IPADDR_ASSIGNMENT_INVALID != lAsgnedAddrMeth_en ) &&
                    (lRetVal_b != FALSE) )
            {
                /* Report DET if DadState_en is TCPIP_DAD_FAIL. */
                /* Autosar 4.5 says if DadState_en is TCPIP_DAD_FAIL Run time error shall be reported but as per CAP Coding guidelines
                 * Run time erros are not allowed in RTA-BSW development path for AR4.2 so a DET is added instead.*/
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_DUPLICATEADDRESSINDICATION_API_ID, TCPIP_E_DADCONFLICT )

                /* Check if upper layer callout API to inform DAD conflict */
                if( TcpIp_CurrConfig_pco->CalloutsConfig_pcst->Up_DADAddressConflict_pcft != NULL_PTR )
                {
                    /* Copy current local IPv6 address and set domain and port */
                    rba_EthIPv6_CopyIPv6Addr(&(lIPv6IPAddr_pst.addr[0]), &(IPAddrParam_pcun->IPv6Par_st.IpAddr_st.Addr_au32[0]));
                    lIPv6IPAddr_pst.domain = TCPIP_AF_INET6;
                    /* Port is not used and hence dummy value 0 is set */
                    lIPv6IPAddr_pst.port = 0;

                    /* Get controller MAC address */
                    EthIf_GetPhysAddr( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId_u8].EthIfCtrlRef_u8,
                            lPhysAddr_au8 );

                    /* Update the Upper Layer about the Ip Address Conflict using the callout function Up_DADAddressConflict */
                    /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrType to match the local parameter Type definition.Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above.*/
                    TcpIp_CurrConfig_pco->CalloutsConfig_pcst->Up_DADAddressConflict_pcft(LocalAddrId_u8,
                                                                       (TcpIp_SockAddrType *)(&lIPv6IPAddr_pst),
                                                                       lPhysAddr_au8,
                                                                       RemotePhysAddrPtr_pcu8);
                }

                /* For STATIC, LINKLOCAL, IPV6ROUTER, keep the address assigned (defend) and for DHCP, send Decline message to the server and unassign the address */
                if(lAsgnedAddrMeth_en == TCPIP_IPADDR_ASSIGNMENT_DHCP)
                {
                    /* Trigger TcpIp_LocalIpAddrAssignmentChg() with unassigned notification for the current assigned method. */
                    /* This is necessary so that the unassignment of the IP address will be executed in the next mainfunction. */
                    TcpIp_LocalIpAddrAssignmentChg (
                                    LocalAddrId_u8,
                                    TCPIP_IPADDR_STATE_UNASSIGNED,
                                    NULL_PTR,
                                    TCPIP_IPADDR_ASSIGNMENT_DHCP,
                                    TCPIP_AF_INET6 );
                }
            }
        }
        else
        {
            /* DAD_PASS is not yet supported because the Duplicate Address detection algorithm as per RFC4862 is not yet supported */
        }
    }
#endif
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
