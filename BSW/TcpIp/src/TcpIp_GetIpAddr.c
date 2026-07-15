

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

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************************/
/* TcpIp_GetIpAddr()    - Obtains the local IP address, netmask and default router actually used by LocalAddrId.                */
/*                                                                                                                              */
/* Service ID           - 0x10                                                                                                  */
/* Sync/Async           - Synchronous                                                                                           */
/* Reentrancy           - Reentrant                                                                                             */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/*  LocalAddrId         - Local address identifier referring to the local IP address which shall be obtained                    */
/*                                                                                                                              */
/* Output Parameters :                                                                                                          */
/*  IpAddrPtr           - Pointer to a structure where the IP address is stored. Structure members not related to the IP address*/
/*                        are of arbitrary value and shall not be used.                                                         */
/*  NetmaskPtr          - Pointer to memory where Network mask of IPv4 address or address prefix of IPv6 address in CIDR        */
/*                        Notation is stored                                                                                    */
/*  DefaultRouterPtr    - Pointer to struct where the IP address of the default router (gateway) is stored                      */
/*                        (struct member 'port' is not used and of arbitrary value)                                             */
/*                                                                                                                              */
/* Return  :                                                                                                                    */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                                  */
/*                        E_NOT_OK: The request has not been accepted.                                                          */
/*                                                                                                                              */
/********************************************************************************************************************************/
/* MR12 RULE 8.13 VIOLATION: IpAddrPtr and DefaultRouterPtr are out arguments and the addressed objects are modified inside the function, pointer to variable is defined in AUTOSAR for IpAddrPtr and DefaultRouterPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType TcpIp_GetIpAddr( TcpIp_LocalAddrIdType LocalAddrId,
                                TcpIp_SockAddrType * IpAddrPtr,
                                uint8 * NetmaskPtr,
                                TcpIp_SockAddrType * DefaultRouterPtr )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *            lLocalAddrConfig_pcst;
    Std_ReturnType                              lRetType_en;

    lRetType_en = E_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                       TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET if IpAddrPtr is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == IpAddrPtr ), TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if NetmaskPtr is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == NetmaskPtr ), TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if DefaultRouterPtr is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == DefaultRouterPtr ), TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    lLocalAddrConfig_pcst         = ( &( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId] ) );

    if( ( lLocalAddrConfig_pcst->IpDomainType_u32 != IpAddrPtr->domain ) || ( lLocalAddrConfig_pcst->IpDomainType_u32 != DefaultRouterPtr->domain ) )
    {
        /* Report DET if local address ID is invalid */
        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_INV_ARG )

        lRetType_en = E_NOT_OK; /* Set return value to E_NOT_OK; stop further processing. */
    }
    else
    {
        switch(lLocalAddrConfig_pcst->IpDomainType_u32)
        {
#if (TCPIP_IPV4_ENABLED == STD_ON)
            case TCPIP_AF_INET:
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                TcpIp_SockAddrInetType * lIPv4IPAddr_pst = ( (TcpIp_SockAddrInetType *) IpAddrPtr);
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                TcpIp_SockAddrInetType * lIPv4DefaultRouter_pst = ( (TcpIp_SockAddrInetType *) DefaultRouterPtr);

                /* Enter critical section to avoid race condition if TcpIp_IpAddr_StateUnassigned is executed concurently */
                SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

                ( lIPv4IPAddr_pst->addr[0] )        = ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32 );
                ( lIPv4DefaultRouter_pst->addr[0] ) = ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32 );

                /* Get the netmask in CIDR notation */
                (*NetmaskPtr) = 0U;
                while( ( lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.NetMask_u32 & (0x80000000uL >> (*NetmaskPtr) )) != 0U )
                {
                    ( *NetmaskPtr )++;
                }

                /* Exit critical section to avoid race condition if TcpIp_IpAddr_StateUnassigned is executed concurently */
                SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();
            }
            break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
            case TCPIP_AF_INET6:
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInet6Type to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above. */
                TcpIp_SockAddrInet6Type * lIPv6IPAddr_pst = ( (TcpIp_SockAddrInet6Type *) IpAddrPtr);
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInet6Type to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above. */
                TcpIp_SockAddrInet6Type * lIPv6DefaultRouter_pst = ( (TcpIp_SockAddrInet6Type *) DefaultRouterPtr);

                /* Enter critical section to avoid race condition if TcpIp_IpAddr_StateUnassigned is executed concurently */
                SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

                /* Copy current local IPv6 address and domain */
                rba_EthIPv6_CopyIPv6Addr(&(lIPv6IPAddr_pst->addr[0]), &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0]));

                /* Copy current default router IPv6 address and domain */
                rba_EthIPv6_CopyIPv6Addr(&(lIPv6DefaultRouter_pst->addr[0]), &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[0]));

                /* Copy the 8-bit netmask prefix, which for IPv6 structure is already stored in CIDR notation. */
                (*NetmaskPtr) = lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.NetMask_u8;

                /* Exit critical section to avoid race condition if TcpIp_IpAddr_StateUnassigned is executed concurently */
                SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();
            }
            break;
#endif

            default:
            {
                /* Address family is not supported. Report to DET and return E_NOT_OK. */
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_GET_IPADDR_API_ID, TCPIP_E_NOPROTOOPT )
                lRetType_en = E_NOT_OK;
            }
            break;
        }
    }

    return lRetType_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
