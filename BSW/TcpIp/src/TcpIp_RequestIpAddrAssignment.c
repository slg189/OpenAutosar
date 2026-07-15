

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

static TcpIp_AddrReturnType TcpIp_RequestStaticIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                                 const TcpIp_SockAddrType * LocalIpAddrPtr_pcst,
                                                                 uint8 Netmask_cu8,
                                                                 const TcpIp_SockAddrType * DefaultRouterPtr_pcst );

static TcpIp_AddrReturnType TcpIp_StoreIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                         TcpIp_IpAddrAssignmentType Type_en,
                                                         uint8 AddrAssignmentlIdx_u8,
                                                         uint8 * ReqAddrMeth_pu8,
                                                         uint8 * RelAddrMeth_pu8 );

#if (TCPIP_IPV4_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_RequestIpAddrAssignment_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                                                const uint8 Netmask_cu8,
                                                                const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst );

static TcpIp_AddrReturnType TcpIp_PerformMulticastIPAddrChecks_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                     const TcpIp_SockAddrInetType * IPv4IPAddr_pcst );

static TcpIp_AddrReturnType TcpIp_PerformUnicastIPAddrChecks_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                   const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                                                   const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst,
                                                                   const uint8 Netmask_cu8 );

static void TcpIp_StoreIpAddrAssignment_IPv4( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                              const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                              const uint8 Netmask_cu8,
                                              const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst );

static uint8 TcpIp_ConvertMaskingToCIDR( uint32 Netmask_u32 );
#endif /* TCPIP_IPV4_ENABLED */

#if (TCPIP_IPV6_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_RequestIpAddrAssignment_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                                                const uint8 Netmask_cu8,
                                                                const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst );

static TcpIp_AddrReturnType TcpIp_PerformMulticastIPAddrChecks_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                     const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst );

static TcpIp_AddrReturnType TcpIp_PerformUnicastIPAddrChecks_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                   const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                                                   const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst,
                                                                   const uint8 Netmask_cu8 );

static void TcpIp_StoreIpAddrAssignment_IPv6( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                              const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                              const uint8 Netmask_cu8,
                                              const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst );
#endif /* TCPIP_IPV6_ENABLED */

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/* TcpIp_RequestIpAddrAssignment() - This API stores the IP assignment requests for assignment methods with trigger MANUAL */
/*                                                                                                                  */
/*                                                                                                                  */
/* Sync/Async            - Asynchronous                                                                             */
/* Reentrancy            - Reentrant                                                                                */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      Type            - Type of IP address assignment which shall be initiated.                                   */
/*      LocalIpAddrPtr  - Pointer to structure containing the IP address which shall be assigned to EthIf controller*/
/*                        indirectly specified via LocalAddrId. Note: This parameter is only used in case the       */
/*                      parameters  Type is set to  TCPIP_IPADDR_ASSIGNMENT_STATIC, can be set to NULL_PTR otherwise*/
/*      Netmask        - Network mask of IPv4 address or address prefix of IPv6 address in CIDR Notation. Note: This*/
/*                       parameter is only used in case the parameter Type is set to TCPIP_IPADDR_ASSIGNMENT_STATIC.*/
/*      DefaultRouterPtr - Pointer to structure containing the IP address of the default router (gateway) which     */
/*                        shall be assigned. Note: This parameter is only used in case the parameter Type is set to */
/*                        TCPIP_IPADDR_ASSIGNMENT_STATIC, can be set to NULL_PTR otherwise.                         */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                      */
/*                        E_NOT_OK: The request has not been accepted.                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_RequestIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId,
                                              TcpIp_IpAddrAssignmentType Type,
                                              const TcpIp_SockAddrType * LocalIpAddrPtr,
                                              uint8 Netmask,
                                              const TcpIp_SockAddrType * DefaultRouterPtr )
{
    /* Local variables declaration */
    Std_ReturnType                              lStdRetType_en;
    TcpIp_AddrReturnType                        lTcpIp_AddrRetType_en;
    const TcpIp_LocalAddrConfig_to *            lLocalAddrConfig_pcst;
    uint8                                       lAddrAssignmentlIdx_u8; /* Stores the index of the assignment search */
    uint8                                       lReqAddrAssgnmeth_u8;
    uint8                                       lRelAddrAssgnmeth_u8;
    uint8                                       lCurrAsgnedPrio_u8;

    /* Local variable initialization */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;
    lReqAddrAssgnmeth_u8  = 0U;
    lRelAddrAssgnmeth_u8  = 0U;

    /* Report DET if TcpIp module is uninitialized */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ), TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId );

    /* Set lStdRetType_en to E_NOT_OK. */
    lStdRetType_en = E_NOT_OK;

    /* Check whether the requested IP assignment method is configured for the LocalAddress and is of trigger MANUAL. */
    /* If the Type is TCPIP_IPADDR_ASSIGNMENT_ALL check that atleast one of the configured assignment methods is MANUAL. */
    for( lAddrAssignmentlIdx_u8 = 0; (lAddrAssignmentlIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8) ; lAddrAssignmentlIdx_u8++ )
    {
        if(( TCPIP_MANUAL == lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentTrig_en ) &&
           (( Type == lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentMeth_en ) ||
            ( Type == TCPIP_IPADDR_ASSIGNMENT_ALL )))
        {
            /* Set lStdRetType_en to E_OK, if the search is successful and break from the loop. */
            lStdRetType_en = E_OK;
            break;
        }
    }

    /* Report DET if requested IP assignment method is not configured or IP assignment trigger is not MANUAL */
    TCPIP_DET_REPORT_ERROR_NO_RET( ( E_NOT_OK == lStdRetType_en ), TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID,  TCPIP_E_INV_ARG )

    /* If the state of the TcpIpCtrl is not TCPIP_STATE_SHUTDOWN, then proceed with IP assignment process. */
    if(( TCPIP_STATE_SHUTDOWN != TcpIp_EthIfCtrlSt_aen[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] ) &&
       ( E_OK == lStdRetType_en ))
    {
        /* Current assigned address structure can be modified in a different execution context. */
        /* Hence store the current assigned priority in a local variable for further processing. */
        lCurrAsgnedPrio_u8 = lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrPrio_u8;

        /* Check if the type requested is TCPIP_IPADDR_ASSIGNMENT_ALL. */
        if( Type == TCPIP_IPADDR_ASSIGNMENT_ALL )
        {
            /* Loop through all the configured IP assignment methods for the local AddrId from lowest to highest priority. */
            for( lAddrAssignmentlIdx_u8 = 0; ( lAddrAssignmentlIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8); lAddrAssignmentlIdx_u8++ )
            {
                /* Initialize lTcpIp_AddrRetType_en with E_OK, so that if current Type is not Static then the request will be directly stored. */
                /* If the address assignment with highest priority is already assigned, then E_OK will be returned. */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;

                /* check if the assignment trigger is MANUAL and the current assignment method has equal or higher priority than the current assigned assignment method. */
                if(( TCPIP_MANUAL == lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentTrig_en ) &&
                   (( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AsgnmentPrio_u8) <= lCurrAsgnedPrio_u8 ) )
                {
                    /* Check if the requested IP assignment is STATIC. */
                    if( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentMeth_en == TCPIP_IPADDR_ASSIGNMENT_STATIC )
                    {
                        /* Call the sub-function TcpIp_RequestStaticIpAddrAssignment(). The function will return TCPIP_ADDR_ASSIGNED if the IP address is already assigned. */
                        lTcpIp_AddrRetType_en = TcpIp_RequestStaticIpAddrAssignment( LocalAddrId,
                                                                                     LocalIpAddrPtr,
                                                                                     Netmask,
                                                                                     DefaultRouterPtr );
                    }

                    /* If the requested IP address is validated and not assigned, then store the request which will be processed in the next main function. */
                    if( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_OK )
                    {
                        /* Call sub-function TcpIp_StoreIpAddrAssignment() which will store the IP assignment request. */
                        lTcpIp_AddrRetType_en = TcpIp_StoreIpAddrAssignment( LocalAddrId,
                                                                             lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentMeth_en,
                                                                             lAddrAssignmentlIdx_u8,
                                                                             &lReqAddrAssgnmeth_u8,
                                                                             &lRelAddrAssgnmeth_u8 );
                    }

                    if( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_NOT_OK )
                    {
                        /* If Static IP address assignment or storing of IP assignment failed, stop further processing and return E_NOT_OK. */
                        /* Clear all previously stored IP address assignment methods in this function call also. */
                        lReqAddrAssgnmeth_u8 = 0U;
                        lRelAddrAssgnmeth_u8 = 0U;

                        /* break from the loop if there is a failure. */
                        break;
                    }
                    else
                    {
                        /* Do nothing. */
                    }
                }
            }
        }
        else
        {
            /* API shall reject the request, if requested IP assignment is of lesser priority than the current assigned IP. */
            if( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AsgnmentPrio_u8 <= lCurrAsgnedPrio_u8 )
            {
                /* Initialize lTcpIp_AddrRetType_en with E_OK, so that if Type is not Static then the request will be directly stored. */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;

                /* If the requested IP assignment method is STATIC, validate static IP, default router IP and netmask */
                if( TCPIP_IPADDR_ASSIGNMENT_STATIC == Type )
                {
                    /* Call the sub-function TcpIp_RequestStaticIpAddrAssignment(). The function will return TCPIP_ADDR_ASSIGNED if the IP address is already assigned. */
                    lTcpIp_AddrRetType_en = TcpIp_RequestStaticIpAddrAssignment( LocalAddrId,
                                                                                 LocalIpAddrPtr,
                                                                                 Netmask,
                                                                                 DefaultRouterPtr );
                }

                /* Check if the requested IP address is validated and not assigned already. */
                if( TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en )
                {
                    /* Call sub-function TcpIp_StoreIpAddrAssignment() which will store the IP assignment request. */
                    lTcpIp_AddrRetType_en = TcpIp_StoreIpAddrAssignment( LocalAddrId,
                                                                         Type,
                                                                         lAddrAssignmentlIdx_u8,
                                                                         &lReqAddrAssgnmeth_u8,
                                                                         &lRelAddrAssgnmeth_u8 );
                }
            }
        }

        /* Store the requested assignment methods into the global structure variable. */
        /* Enter critical section: The request assignment method variable can be updated in different execution context and should be protected in a critical section. */
        SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

        /* Store the request and release flags in global variable for further processing in next MainFunction */
        (lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8) |= lReqAddrAssgnmeth_u8;
        (lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8) |= lRelAddrAssgnmeth_u8;

        /* Exit critical section. */
        SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
    }

    /* If the IP address is either assigned already or the request is stored, then return E_OK. */
    lStdRetType_en = (lTcpIp_AddrRetType_en != TCPIP_ADDR_E_NOT_OK) ? (E_OK) : (E_NOT_OK);

    return lStdRetType_en;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*************************************************************************************************************************/
/* TcpIp_RequestStaticIpAddrAssignment() - This is a helper function called by the TcpIp_RequestIpAddrAssignment() API   */
/*                                         which processes IPv4 and IPv6 static IP assignment requests.                  */
/*                                         The actual IP address assignment will be performed by the                     */
/*                                         TcpIp_StateMachine() API.                                                     */
/*                                                                                                                       */
/* Input Parameters :                                                                                                    */
/*      LocalAddrId_u8      - IP address index specifying the IP address for which an assignment shall be initiated.     */
/*      LocalIpAddrPtr_pcst - Pointer to structure containing the IP address which shall be assigned to EthIf controller */
/*                            indirectly specified via LocalAddrId.                                                      */
/*      Netmask_cu8     - Network mask.                                                                                  */
/*      DefaultRouterPtr_pcst - Pointer to structure containing the IP address of the default router (gateway) which     */
/*                        shall be assigned. Note: This parameter is only used in case the parameter Type is set to      */
/*                        TCPIP_IPADDR_ASSIGNMENT_STATIC, can be set to NULL_PTR otherwise.                              */
/*                                                                                                                       */
/* Return  :                                                                                                             */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the checks.                   */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the checks.                 */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId.      */
/*                                                                                                                       */
/*************************************************************************************************************************/
static TcpIp_AddrReturnType TcpIp_RequestStaticIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                                 const TcpIp_SockAddrType * LocalIpAddrPtr_pcst,
                                                                 uint8 Netmask_cu8,
                                                                 const TcpIp_SockAddrType * DefaultRouterPtr_pcst )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType                        lTcpIp_AddrRetType_en;
    uint8                                       lNetmask_u8;
    const TcpIp_LocalAddrConfig_to *            lLocalAddrConfig_pcst;

#if (TCPIP_IPV4_ENABLED == STD_ON)
    const TcpIp_SockAddrInetType *              lIPv4IPAddr_pcst;
    const TcpIp_SockAddrInetType *              lIPv4DefaultRouter_pcst;
    TcpIp_SockAddrInetType                      lIPv4IPAddr_st;
    TcpIp_SockAddrInetType                      lIPv4DefaultRouter_st;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    uint8                                       lLoopIdx_u8;
    const TcpIp_SockAddrInet6Type *             lIPv6IPAddr_pcst;
    const TcpIp_SockAddrInet6Type *             lIPv6DefaultRouter_pcst;
    TcpIp_SockAddrInet6Type                     lIPv6IPAddr_st;
    TcpIp_SockAddrInet6Type                     lIPv6DefaultRouter_st;
#endif

    /* Initialize local variables. */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;
    lNetmask_u8 = 0;

#if (TCPIP_IPV4_ENABLED == STD_ON)
    lIPv4IPAddr_pcst = NULL_PTR;
    lIPv4DefaultRouter_pcst = NULL_PTR;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
    lIPv6IPAddr_pcst = NULL_PTR;
    lIPv6DefaultRouter_pcst = NULL_PTR;
#endif

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Report DET if LocalIpAddrPtr_pcst is Null Pointer but DefaultRouterPtr_pcst is not Null Pointer. */
    TCPIP_DET_REPORT_ERROR_RET_VALUE((( NULL_PTR == LocalIpAddrPtr_pcst ) && (NULL_PTR != DefaultRouterPtr_pcst )),
                                       TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, TCPIP_ADDR_E_NOT_OK )

    /* Report DET if domain of LocalIpAddrPtr_pcst is different than domain of current local IP address configuration. */
    TCPIP_DET_REPORT_ERROR_RET_VALUE((( NULL_PTR != LocalIpAddrPtr_pcst ) && ( LocalIpAddrPtr_pcst->domain != lLocalAddrConfig_pcst->IpDomainType_u32 )),
                                       TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, TCPIP_ADDR_E_NOT_OK )

    /* Report DET if domain of DefaultRouterPtr_pcst is different than domain of current local IP address configuration */
    TCPIP_DET_REPORT_ERROR_RET_VALUE((( NULL_PTR != DefaultRouterPtr_pcst ) && ( DefaultRouterPtr_pcst->domain != lLocalAddrConfig_pcst->IpDomainType_u32 ) ),
                                       TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, TCPIP_ADDR_E_NOT_OK )

    /* Check domain here as address pointer is checked for null value */
    switch( lLocalAddrConfig_pcst->IpDomainType_u32 )
    {
#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_AF_INET:
        {
            /* Check if the LocalIpAddrPtr_pcst is not NULL_PTR. */
            if( LocalIpAddrPtr_pcst != NULL_PTR )
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                lIPv4IPAddr_pcst        = ( ( const TcpIp_SockAddrInetType * ) LocalIpAddrPtr_pcst );

                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
                lIPv4DefaultRouter_pcst = ( ( const TcpIp_SockAddrInetType * ) DefaultRouterPtr_pcst );      /* Get full default IPv4 router info */

                /* Store the passed netmask in the local variable. */
                lNetmask_u8 = Netmask_cu8;
            }
            else if( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun != NULL_PTR )
            {   /* If LocalIpAddrPtr_pcst is passed as NULL_PTR, then the parameters shall be fetched from the StaticIpAddressConfig container. */
                /* Store the domain as TCPIP_AF_INET. */
                lIPv4IPAddr_st.domain        = TCPIP_AF_INET;

                /* Store the Local IP address to be assigned. */
                lIPv4IPAddr_st.addr[0]       = lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv4Par_st.IpAddr_u32;

                lIPv4IPAddr_pcst              = &lIPv4IPAddr_st;

                /* If the default router IP address is zero in CfgStaticIpAddr_pcun structure, */
                /*  it means there is no request for assignment of default router IP address.  It shall be stored as NULL_PTR. */
                if( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv4Par_st.DftRtr_u32 == TCPIP_IPADDR_ZERO )
                {
                    lIPv4DefaultRouter_pcst = NULL_PTR;
                }
                else
                {   /* Store the domain as TCPIP_AF_INET. */
                    lIPv4DefaultRouter_st.domain = TCPIP_AF_INET;

                    /* Store the default router IP address to be assigned. */
                    lIPv4DefaultRouter_st.addr[0] = lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv4Par_st.DftRtr_u32;

                    lIPv4DefaultRouter_pcst = &lIPv4DefaultRouter_st;
                }
               /* Store the netmask to be assigned. */
               lNetmask_u8 = TcpIp_ConvertMaskingToCIDR( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv4Par_st.NetMask_u32 );
            }
            else
            {
                /* Set return value to TCPIP_ADDR_E_NOT_OK. */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

                /* Report DET if lLocalAddrConfig_pcst is a Null Pointer and ManualStaticIpAddr_pun is also NULL_PTR (TcpIpStaticIPAddrConfig is not configured.) */
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_NULL_PTR )
            }

            if( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_OK )
            {
                lTcpIp_AddrRetType_en     = TcpIp_RequestIpAddrAssignment_IPv4( LocalAddrId_u8, lIPv4IPAddr_pcst, lNetmask_u8, lIPv4DefaultRouter_pcst);

                /*If the passed parameters are invalid, then report DET error with TCPIP_E_INV_ARG. */
                TCPIP_DET_REPORT_ERROR_RET_VALUE(( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_NOT_OK), TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, TCPIP_ADDR_E_NOT_OK )
            }
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_AF_INET6:
        {
            /* Check if the LocalIpAddrPtr_pcst is not NULL_PTR. */
            if( LocalIpAddrPtr_pcst != NULL_PTR )
            {
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInet6Type to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above. */
                lIPv6IPAddr_pcst        = ( ( const TcpIp_SockAddrInet6Type * ) LocalIpAddrPtr_pcst );
                /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInet6Type to match the local parameter Type definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET6 above. */
                lIPv6DefaultRouter_pcst = ( ( const TcpIp_SockAddrInet6Type * ) DefaultRouterPtr_pcst );     /* Get full default IPv6 router info */

                /* Store the passed netmask in the local variable. */
                lNetmask_u8 = Netmask_cu8;
            }
            else if( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun != NULL_PTR )
            {   /* If LocalIpAddrPtr_pcst is passed as NULL_PTR, then the parameters shall be fetched from the StaticIpAddressConfig container. */
                /* Store the domain as TCPIP_AF_INET6. */
                lIPv6IPAddr_st.domain = TCPIP_AF_INET6;

                /* Store the local IP address to be assigned. */
                for( lLoopIdx_u8 = 0; lLoopIdx_u8 < 4U; lLoopIdx_u8++ )
                {
                    lIPv6IPAddr_st.addr[lLoopIdx_u8] = lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.IpAddr_st.Addr_au32[lLoopIdx_u8];
                }

                lIPv6IPAddr_pcst  = &lIPv6IPAddr_st;

                /* If the configured default router IP address is zero, */
                /*  it means there is no request for assignment of default router IP address. It shall be stored as NULL_PTR. */
                if( ( ( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.DftRtr_st.Addr_au32[0] == 0uL ) &&
                      ( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.DftRtr_st.Addr_au32[1] == 0uL ) &&
                      ( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.DftRtr_st.Addr_au32[2] == 0uL ) &&
                      ( lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.DftRtr_st.Addr_au32[3] == 0uL ) ) != FALSE )
                {
                    lIPv6DefaultRouter_pcst = NULL_PTR;
                }
                else
                {
                    /* Store the domain as TCPIP_AF_INET6. */
                    lIPv6DefaultRouter_st.domain = TCPIP_AF_INET6;

                    /* Store the default router IP address to be assigned. */
                    for( lLoopIdx_u8 = 0; lLoopIdx_u8 < 4U; lLoopIdx_u8++ )
                    {
                        lIPv6DefaultRouter_st.addr[lLoopIdx_u8] = lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.DftRtr_st.Addr_au32[lLoopIdx_u8];
                    }

                    lIPv6DefaultRouter_pcst = &lIPv6DefaultRouter_st;
                }

                /* Store the passed netmask in the local variable. */
                lNetmask_u8 = lLocalAddrConfig_pcst->CfgStaticIpAddr_pcun->IPv6Par_st.NetMask_u8;
            }
            else
            {
                /* Set return value to TCPIP_ADDR_E_NOT_OK. */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

                /* Report DET if lLocalAddrConfig_pcst is a Null Pointer and ManualStaticIpAddr_pun is also NULL_PTR (TcpIpStaticIPAddrConfig is not configured.) */
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_NULL_PTR )
            }

            if( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_OK )
            {
                lTcpIp_AddrRetType_en     = TcpIp_RequestIpAddrAssignment_IPv6( LocalAddrId_u8, lIPv6IPAddr_pcst, lNetmask_u8, lIPv6DefaultRouter_pcst);

                /*If the passed parameters are invalid, then report DET error with TCPIP_E_INV_ARG. */
                TCPIP_DET_REPORT_ERROR_RET_VALUE( (lTcpIp_AddrRetType_en == TCPIP_ADDR_E_NOT_OK), TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, TCPIP_ADDR_E_NOT_OK )
            }
        }
        break;
#endif

        default:
        {
            lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

            /* Domain type is invalid! Report error to DET */
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_AFNOSUPPORT )
        }
        break;
    }

    return lTcpIp_AddrRetType_en;
}


/*******************************************************************************************************************************/
/* TcpIp_StoreIpAddrAssignment() - This API stores the IP assignment requests for assignment methods with trigger MANUAL.      */
/*                                                                                                                             */
/* Input Parameters :                                                                                                          */
/* LocalAddrId_u8     - IP address index specifying the IP address for which an assignment shall be initiated.                 */
/* Type_en            - Type of IP address assignment which shall be initiated.                                                */
/* AddrAssignmentlIdx_u8  - The assignment request in bit form corresponding to index of configured assignment method array.   */
/*                                                                                                                             */
/* Output Parameters :                                                                                                         */
/* ReqAddrMeth_pu8   - Requested IP address assignment method stored in bit form.                                              */
/* RelAddrMeth_pu8   - IP address method to be released, stored in bit form.                                                    */
/*                                                                                                                             */
/* Return  :                                                                                                                   */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the checks.                         */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the checks.                       */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId.            */
/*                                                                                                                             */
/*******************************************************************************************************************************/
static TcpIp_AddrReturnType TcpIp_StoreIpAddrAssignment( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                         TcpIp_IpAddrAssignmentType Type_en,
                                                         uint8 AddrAssignmentlIdx_u8,
                                                         uint8 * ReqAddrMeth_pu8,
                                                         uint8 * RelAddrMeth_pu8 )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType                        lTcpIp_AddrRetType_en;
    const TcpIp_LocalAddrConfig_to *            lLocalAddrConfig_pcst;

    /* Initialize local variable. */
    /* set lTcpIp_AddrRetType_en to TCPIP_ADDR_E_OK. */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;

    /* Address of the requested LocalAddress configuration structure. */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Check if the assignment method is not static */
    if( Type_en != TCPIP_IPADDR_ASSIGNMENT_STATIC )
    {
        /* Check if the requested IP address assignment method is the same as the current assignment method. */
        if( Type_en == lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en )
        {
            /* If the current assigned type is AutoIp or DHCP, set return value to assigned. */
            /* No need to restart them because the same IP address will get re-assigned.     */
            lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
        }
    }
    else
    {
        /* If the assignment method is STATIC, then the IP address is always released and assigned irrespective of whether it is the current assigned address method or not. */
        /* If current assigned address is being assigned in parallel in MainFunction context, then setting the RelAddrMeth_pu8 flag might be missed. */
        /* Store the assignment method index in the out parameter */
        TCPIP_UINT8_SET_BIT_WITH_SHIFT((*RelAddrMeth_pu8), (AddrAssignmentlIdx_u8));
    }

    /* Store the assignment request in bit form corresponding to index of configured assignment method array. */
    if( TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en )
    {
        /* Store the assignment method index in the out parameter */
        TCPIP_UINT8_SET_BIT_WITH_SHIFT((*ReqAddrMeth_pu8), (AddrAssignmentlIdx_u8));
    }

    return lTcpIp_AddrRetType_en;
}


/********************************************************************************************************************/
/* TcpIp_RequestIpAddrAssignment_IPv4() - This is a helper function called by the TcpIp_RequestIpAddrAssignment() API*/
/*                        which performs all the IPv4 domain specific required validation checks and stores the     */
/*                        asignment request. The actual IP adress assignment will be performed by the                 */
/*                        TcpIp_StateMachine() API.                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      IPv4IPAddr_pcst - Pointer to structure containing the IP address which shall be assigned to EthIf controller */
/*                        indirectly specified via LocalAddrId.                                                      */
/*      Netmask_cu8     - Network mask of IPv4.                                                                      */
/*      IPv4DefaultRouter_pcst - Pointer to structure containing the IP address of the default router (gateway) which*/
/*                        shall be assigned. Note: This parameter is only used in case the parameter Type is set to  */
/*                        TCPIP_IPADDR_ASSIGNMENT_STATIC, can be set to NULL_PTR otherwise.                          */
/*                                                                                                                   */
/* Return  :                                                                                                         */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the checks.               */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the checks.             */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId.  */
/*                                                                                                                   */
/*********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_RequestIpAddrAssignment_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                                                const uint8 Netmask_cu8,
                                                                const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType                lTcpIp_AddrRetType_en;
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId );

    /* Discard the request, if the static IP to be assigned is                 */
    /* 1. 0x00000000                                                           */
    /* 2. Is loop back IP                                                      */
    /* 3. Reserved IP range 240.0.0.0/4 ( 0xffffffff  )                        */
    if ( (TCPIP_IPADDR_ZERO      != IPv4IPAddr_pcst->addr[0]) &&
         (TCPIP_IPV4_LOOPBACK    != (IPv4IPAddr_pcst->addr[0] & TCPIP_IPV4_LOOPBACK_MASK))  &&
         (TCPIP_IPV4_RESERVED_IP != (IPv4IPAddr_pcst->addr[0] & TCPIP_IPV4_RESERVED_IP)))
    {
        /* Case of Unicast */
        if( TCPIP_UNICAST == lLocalAddrConfig_pcst->IpAddressType_en )
        {
            lTcpIp_AddrRetType_en = TcpIp_PerformUnicastIPAddrChecks_IPv4( LocalAddrId, IPv4IPAddr_pcst, IPv4DefaultRouter_pcst, Netmask_cu8 );
        }

        /* Case of Multicast */
        else
        {
            lTcpIp_AddrRetType_en = TcpIp_PerformMulticastIPAddrChecks_IPv4( LocalAddrId, IPv4IPAddr_pcst );
        }

        /* If checks succeeded, store the address assignment */
        if( TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en )
        {
            TcpIp_StoreIpAddrAssignment_IPv4( lLocalAddrConfig_pcst, IPv4IPAddr_pcst, Netmask_cu8, IPv4DefaultRouter_pcst );
        }
    }

    return lTcpIp_AddrRetType_en;
}
#endif


/**********************************************************************************************************************/
/* TcpIp_PerformMulticastIPAddrChecks_IPv4() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv4 */
/*                                          and performs some basic checks on the IPv4 address to be assigned of      */
/*                                          MULTICAST type.                                                           */
/*                                                                                                                    */
/* Input Parameters :                                                                                                 */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.      */
/*      IPv4IPAddr_pcst      - Pointer to structure containing the IPv4 address which shall be assigned to EthIf      */
/*                        controller indirectly specified via LocalAddrId.                                            */
/*                                                                                                                    */
/* Return  :                                                                                                          */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the multicast checks.      */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the multicast checks.    */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId.   */
/*                                                                                                                    */
/**********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_PerformMulticastIPAddrChecks_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                     const TcpIp_SockAddrInetType * IPv4IPAddr_pcst )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType                lTcpIp_AddrRetType_en;
    uint8                               lLocalAddrId_u8;
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId );

    /* If LocalAddress is configured as multicast, Static IP shall be in multicast address range */
    if( (IPv4IPAddr_pcst->addr[0] >= RBA_ETHIPV4_MULTICASTADDR_MIN) &&
        (IPv4IPAddr_pcst->addr[0] <= RBA_ETHIPV4_MULTICASTADDR_MAX) )
    {
        lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;

        /* If the requested multicast IP to be assigned is already assigned to the EthIfCtrl return E_NOT_OK */
        for(lLocalAddrId_u8 = 0; lLocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ; lLocalAddrId_u8++)
        {
            /* Do not assign requested IPv4 multicast address if:
                1. Current assigned EthIfCtrl index is the same with the requested EthIfCtrl index
                2. Current assigned IPv4 address is equal to the requested IPv4 address to be assigned */
            if( ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].EthIfCtrlRef_u8 == lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ) &&
                ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32 == IPv4IPAddr_pcst->addr[0] ) )
            {
                /* If the requested IP address is already assigned, check if it was assigned for a different LocalAddrId mapped to the same EthIfCtrlIdx. */
                if( lLocalAddrId_u8 != LocalAddrId )
                {
                    /* If its requested for a different localAddrId, return TCPIP_ADDR_E_NOT_OK and throw DET */
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

                    TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_ADDRINUSE )
                }
                else
                {
                    /* If the requested IP address is assigned for the same LocalAddrId,  */
                    /* then return TCPIP_ADDR_ASSIGNED indicating that the IP address is already assigned. */
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                }

                break;
            }
        }
    }

    return lTcpIp_AddrRetType_en;
}
#endif


/********************************************************************************************************************/
/* TcpIp_PerformUnicastIPAddrChecks_IPv4() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv4().*/
/*                                 It performs some basic checks on the IP address to be assigned of UNICAST type.     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      IPv4IPAddr_pcst - Pointer to structure containing the IPv4 address which shall be assigned to EthIf         */
/*                      controller indirectly specified via LocalAddrId.                                             */
/*      IPv4DefaultRouter_pcst - Pointer to structure containing the IPv4 address of the default router (gateway)   */
/*                      which shall be assigned.                                                                    */
/*      Netmask_cu8     - Network mask of IPv4 address.                                                             */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the unicast checks.      */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the unicast checks.    */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId. */
/*                                                                                                                  */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_PerformUnicastIPAddrChecks_IPv4( TcpIp_LocalAddrIdType LocalAddrId,
                                                                   const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                                                   const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst,
                                                                   const uint8 Netmask_cu8 )
{
    /* Local variables declaration */
    uint32                    lNetmask_u32 = 0uL;
    TcpIp_AddrReturnType      lTcpIp_AddrRetType_en;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* -------------------------------------- */
    /* Check static IP                        */
    /* -------------------------------------- */

    /* -Netmask shall be in valid range (1 to 30)          */
    /* -Static IP shall not be in multicast address range  */
    if( ((Netmask_cu8 >= 1U) && (Netmask_cu8 <= 30U)) &&
        ((IPv4IPAddr_pcst->addr[0] < RBA_ETHIPV4_MULTICASTADDR_MIN) || (IPv4IPAddr_pcst->addr[0] > RBA_ETHIPV4_MULTICASTADDR_MAX)) )
    {
        /* Get netmask in 32 bit format */
        lNetmask_u32 = ( 0xFFFFFFFFuL << (TCPIP_NETMASK_MAX_VAL - Netmask_cu8) );

        /* Futher check of IP address validity */
        if( (((IPv4IPAddr_pcst->addr[0] & lNetmask_u32)    != IPv4IPAddr_pcst->addr[0]) &&     /* Static IP shall not be network address           */
             ((IPv4IPAddr_pcst->addr[0] | (~lNetmask_u32)) != IPv4IPAddr_pcst->addr[0])) )     /* Static IP shall not be network broadcast address */
        {
            /* Set return value to TCPIP_ADDR_E_OK to reinitiate the IP address assignment process. */
            lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;

            /* If the local IP address is in AutoIP range, then passed default router IP address must be NULL_PTR and netmask must be 16. */
            /* According to RFC 3927 p16 : The 169.254/16 address prefix MUST NOT be subnetted. */
            if(( TCPIP_AUTOIP_NET == ( IPv4IPAddr_pcst->addr[0] & TCPIP_AUTOIP_MASK )) &&
               (( NULL_PTR != IPv4DefaultRouter_pcst ) || ( Netmask_cu8 != 16U )))
            {
                /* If either IPv4DefaultRouter_pcst is not NULL_PTR or Netmask_cu8 is not 16 and the static IP address is in AutoIP range, */
                /* return TCPIP_ADDR_E_NOT_OK. */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;
            }
        }
    }

    /* If static IP is valid */
    if( lTcpIp_AddrRetType_en != TCPIP_ADDR_E_NOT_OK )
    {
        /* -------------------------------------- */
        /* Check already assigned                 */
        /* -------------------------------------- */

        /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
        /* Hence the accesses to the current assigned address structure shall be protected in a critical section. */
        SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

        /* If the requested IP address and netmask are already assigned, */
        /* then return TCPIP_ADDR_ASSIGNED indicating that the IP address is already assigned. */
        if(( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32 == IPv4IPAddr_pcst->addr[0] ) &&
           ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.NetMask_u32 == lNetmask_u32 ))
        {
            /* If default router is given in argument */
            if( NULL_PTR != IPv4DefaultRouter_pcst )
            {
                /* If the IP address and netmask are already assigned, check if the requested default router address is also the same. */
                if( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32 == IPv4DefaultRouter_pcst->addr[0] )
                {
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                }
            }
            else
            {
                /* If the requested IP address and netmask are already assigned and the requested default router IP address is NULL_PTR, */
                /* check if the current assigned default router IP address is TCPIP_IPADDR_ZERO. */
                if( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32 == TCPIP_IPADDR_ZERO )
                {
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                }
            }
        }

        /* Exit critical section. */
        SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

        /* -------------------------------------- */
        /* Check default router                   */
        /* -------------------------------------- */

        /* If IPv4DefaultRouter_pcst pointer is not NULL, implies that default router is required and shall be within the valid range */
        /* The domain for the default router is already verified in a DET check in the main API */
        if( (TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en) &&
            (NULL_PTR != IPv4DefaultRouter_pcst) )
        {
            /* Reset the return value of the function to TCPIP_ADDR_E_NOT_OK */
            lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

            if( ((IPv4IPAddr_pcst->addr[0] & lNetmask_u32) == (IPv4DefaultRouter_pcst->addr[0] & lNetmask_u32)) &&    /* Static IP and default router shall have same network address ( This implicitly covers - 0x0, loopback, reserved IP, AutoIp ) */
                ((IPv4DefaultRouter_pcst->addr[0] | (~lNetmask_u32)) != IPv4DefaultRouter_pcst->addr[0]) &&  /* Default router IP shall not be network broadcast address */
                (IPv4IPAddr_pcst->addr[0] != IPv4DefaultRouter_pcst->addr[0]) &&                     /* Static IP and default router IP shall not be same */
                ((IPv4DefaultRouter_pcst->addr[0] & lNetmask_u32) != IPv4DefaultRouter_pcst->addr[0]) &&   /* Default router IP shall not be network address */
                ((IPv4DefaultRouter_pcst->addr[0] < RBA_ETHIPV4_MULTICASTADDR_MIN) || (IPv4DefaultRouter_pcst->addr[0] > RBA_ETHIPV4_MULTICASTADDR_MAX)) )    /* Default router IP shall not be Multicast address */
            {
                /* Discard the request if default router IP is                             */
                /* 1. Is loop back IP                                                      */
                /* 2. Reserved IP range 240.0.0.0/4 ( 0xffffffff  )                        */
                /* 3. AutoIp address                                                       */
                if ( (TCPIP_IPV4_LOOPBACK    != (IPv4DefaultRouter_pcst->addr[0] & TCPIP_IPV4_LOOPBACK_MASK))  &&
                     (TCPIP_IPV4_RESERVED_IP != (IPv4DefaultRouter_pcst->addr[0] & TCPIP_IPV4_RESERVED_IP)) &&
                     (TCPIP_AUTOIP_NET       != (IPv4DefaultRouter_pcst->addr[0] & TCPIP_AUTOIP_MASK)) )
                {
                    /* Set return value to TCPIP_ADDR_E_OK TCPIP_ADDR_E_OK to reinitiate the IP address assignment process. */
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;
                }
            }
        }
    }

    return lTcpIp_AddrRetType_en;
}
#endif

/********************************************************************************************************************/
/* TcpIp_StoreIpAddrAssignment_IPv4() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv4() and*/
/*                        stores the provided local IPv4 address, default router address and netmask info.          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrConfig_pcst - Pointer to the local IPv4 address config structure.                                  */
/*      IPv4IPAddr_pcst - Pointer to structure containing the IP address which shall be assigned to EthIf controller*/
/*                        indirectly specified via LocalAddrId.                                                     */
/*      Netmask_cu8     - Network mask of IPv4.                                                                     */
/*      IPv4DefaultRouter_pcst - Pointer to structure containing the IP address of the default router (gateway) which*/
/*                        shall be assigned.                                                                        */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                      */
/*                        E_NOT_OK: The request has not been accepted.                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static void TcpIp_StoreIpAddrAssignment_IPv4( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                              const TcpIp_SockAddrInetType * IPv4IPAddr_pcst,
                                              const uint8 Netmask_cu8,
                                              const TcpIp_SockAddrInetType * IPv4DefaultRouter_pcst )
{
    /* Enter critical section: Manual static IP address structure can be accessed in a different execution context, by the TcpIp_MainFunction(). */
    /* Hence the manual static address shall be protected under critical section when it is modified. */
    SchM_Enter_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();

    LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.IpAddr_u32  = IPv4IPAddr_pcst->addr[0];
    LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.NetMask_u32 = TCPIP_IPV4_NETMASK_INVALID; /* Initializing with default value */
    LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.DftRtr_u32  = TCPIP_IPADDR_ZERO;          /* Initializing with default value */

    /* Netmask and default router are valid only for unicast IP */
    if(TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en)
    {
        LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.NetMask_u32 = ( 0xFFFFFFFFuL << (TCPIP_NETMASK_MAX_VAL - Netmask_cu8));

        /* If IPv4DefaultRouter_pcst is not NULL store default router IP */
        if(NULL_PTR != IPv4DefaultRouter_pcst)
        {
            LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv4Par_st.DftRtr_u32 = IPv4DefaultRouter_pcst->addr[0];
        }
    }

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();
}
#endif


/********************************************************************************************************************/
/* TcpIp_ConvertMaskingToCIDR() - This API converts the masking notation into the CIDR Notation                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      Netmask_u32     - Netmask into the Masking notation                                                         */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      uint8              lNetmask_u8: The CIDR Notation resulted from the masking notation.                       */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static uint8 TcpIp_ConvertMaskingToCIDR( uint32 Netmask_u32 )
{
    /* Get the netmask in CIDR notation */
    uint8 lNetmask_u8 = 0;
    while( ( Netmask_u32 & (0x80000000uL >> ( lNetmask_u8 ) )) != 0u )
    {
        lNetmask_u8++;
    }
    return lNetmask_u8;
}
#endif


/********************************************************************************************************************/
/* TcpIp_RequestIpAddrAssignment_IPv6() - This is a helper function called by the TcpIp_RequestIpAddrAssignment() API*/
/*                        which performs all the IPv6 domain specific required validation checks and stores the     */
/*                        asignment request. The actual IP adress assignment will be performed by the                 */
/*                        TcpIp_StateMachine() API.                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      IPv6IPAddr_pcst - Pointer to structure containing the IP address which shall be assigned to EthIf controller*/
/*                        indirectly specified via LocalAddrId.                                                     */
/*      Netmask_cu8     - Mask prefix of IPv6 address in CIDR Notation.                                             */
/*      IPv6DefaultRouter_pcst - Pointer to structure containing the IP address of the default router (gateway) which*/
/*                        shall be assigned. Note: This parameter is only used in case the parameter Type is set to */
/*                        TCPIP_IPADDR_ASSIGNMENT_STATIC, can be set to NULL_PTR otherwise.                         */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the checks.              */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the checks.            */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId. */
/*                                                                                                                  */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_RequestIpAddrAssignment_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                                                const uint8 Netmask_cu8,
                                                                const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType                   lTcpIp_AddrRetType_en;
    const TcpIp_LocalAddrConfig_to *       lLocalAddrConfig_pcst;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* Address of the requested LocalAddress configuration structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId );

    /* Discard the request, if the static IP to be assigned is: */
    /* 1. 0x00000000 */
    /* 2. Loop back IP  */
    if( (((IPv6IPAddr_pcst->addr[0] == 0uL) && (IPv6IPAddr_pcst->addr[1] == 0uL) &&
         (IPv6IPAddr_pcst->addr[2] == 0uL) && (IPv6IPAddr_pcst->addr[3] == 0uL)) == FALSE)
        &&
        (((IPv6IPAddr_pcst->addr[0] == 0uL ) && (IPv6IPAddr_pcst->addr[1] == 0uL) &&
         (IPv6IPAddr_pcst->addr[2] == 0uL) && (IPv6IPAddr_pcst->addr[3] == 1uL)) == FALSE) )
    {
        /* Case of Unicast */
        if( TCPIP_UNICAST == lLocalAddrConfig_pcst->IpAddressType_en )
        {
            lTcpIp_AddrRetType_en = TcpIp_PerformUnicastIPAddrChecks_IPv6( LocalAddrId, IPv6IPAddr_pcst, IPv6DefaultRouter_pcst, Netmask_cu8 );
        }

        /* Case of Multicast */
        else
        {
            lTcpIp_AddrRetType_en = TcpIp_PerformMulticastIPAddrChecks_IPv6( LocalAddrId, IPv6IPAddr_pcst );
        }

        /* If checks succeeded, store the address assignment */
        if( TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en )
        {
            TcpIp_StoreIpAddrAssignment_IPv6( lLocalAddrConfig_pcst, IPv6IPAddr_pcst, Netmask_cu8, IPv6DefaultRouter_pcst );
        }
    }

    return lTcpIp_AddrRetType_en;
}
#endif


/********************************************************************************************************************/
/* TcpIp_PerformMulticastIPAddrChecks_IPv6() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv6*/
/*                                         and performs some basic checks on the IPv6 address to be assigned of     */
/*                                      MULTICAST type.                                                             */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      IPv6IPAddr_pcst      - Pointer to structure containing the IPv6 address which shall be assigned to EthIf    */
/*                        controller indirectly specified via LocalAddrId.                                          */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the multicast checks.    */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the multicast checks.  */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId. */
/*                                                                                                                  */
/* Steps:                                                                                                           */
/*        1. Check that whatever the multicast address type (Interface-Local, Link-Local, Global scoped), it is not a */
/*         reserved IPv6 multicast address -> extract from RFC 4291, Section 2.7 (Multicast Addresses).              */
/*         2. Check that the requested IPv6 address is not already assigned for the local address index cfg.         */
/********************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_PerformMulticastIPAddrChecks_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                     const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType    lTcpIp_AddrRetType_en;
    uint32                  lfirstIpv6AddrByte_u32;
    uint8                   lLocalAddrId_u8;
    boolean                 lIpAddressEqual_b;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* Check whether the requested multicast IP address to be assigned is not a solicited node multicast address. */
    /* SNMA addresses are used internally only by NDP protocol, hence the assignment shall not be requested. */
    if((( IPv6IPAddr_pcst->addr[0] == 0xFF020000uL ) &&
       ( IPv6IPAddr_pcst->addr[1] == 0uL )           &&
       ( IPv6IPAddr_pcst->addr[2] == 1uL )          &&
       (( IPv6IPAddr_pcst->addr[3] & (0xFF000000uL)) == (0xFF000000uL))) == FALSE )
    {
        lfirstIpv6AddrByte_u32 = IPv6IPAddr_pcst->addr[0];

        /* 1. Check that whatever the multicast address type (Interface-Local, Link-Local, Global scoped), */
        /*    it is not a reserved IPv6 multicast address => */
        /*    begins with 0xFF and the 'scop' field does NOT have one of the following values: */
        /*    a) 0x0  reserved */
        /*    b) 0x3  reserved */
        /*    c) 0xF  reserved */
        if( ((lfirstIpv6AddrByte_u32 & TCPIP_IPV6_MULTICAST_MASK)           == TCPIP_IPV6_MULTICAST_MASK) &&            /* Must start with 0xFF (IPv6 multicast type) */
            ((lfirstIpv6AddrByte_u32 & TCPIP_IPV6_MULTICAST_SCOP_FLAG_MASK) != TCPIP_IPV6_MULTICAST_RESERVED_SCOP_0) && /* Must not start with 0xFF00 (reserved) */
            ((lfirstIpv6AddrByte_u32 & TCPIP_IPV6_MULTICAST_SCOP_FLAG_MASK) != TCPIP_IPV6_MULTICAST_RESERVED_SCOP_3) && /* Must not start with 0xFF03 (reserved) */
            ((lfirstIpv6AddrByte_u32 & TCPIP_IPV6_MULTICAST_SCOP_FLAG_MASK) != TCPIP_IPV6_MULTICAST_RESERVED_SCOP_F) )  /* Must not start with 0xFF0F (reserved) */
        {
            /* If the requested address is not SNMA and not in the reserved range, accept the request with TCPIP_ADDR_E_OK. */
            lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;
        }
    }

    /* If multicast IP is valid */
    if( lTcpIp_AddrRetType_en == TCPIP_ADDR_E_OK )
    {
        /* 2. If the requested multicast IP to be assigned is already assigned to the EthIfCtrl return E_NOT_OK */
        for(lLocalAddrId_u8 = 0; lLocalAddrId_u8 < TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ; lLocalAddrId_u8++)
        {
            /* Do not assign requested IPv6 multicast address if: */
            /*  1. Current assigned EthIfCtrl index is the same with the requested EthIfCtrl index */
            if( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].EthIfCtrlRef_u8 == TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].EthIfCtrlRef_u8 )
            {
                /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
                /* Hence the accesses to the current assigned address structure shall be protected in a critical section. */
                SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

                /*  Call the comparison function only if the first 2 conditions are fulfilled. */
                lIpAddressEqual_b = TcpIp_IPv6AddrCompareWithMask(&(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0]),
                                                              &(IPv6IPAddr_pcst->addr[0]), TCPIP_IPV6_CIDR_MASK_128);

                /* Exit critical section. */
                SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

                /* Do not assign requested IPv6 multicast address if: */
                /*  2. Current assigned IPv6 address is equal to the requested IPv6 address to be assigned */
                if( lIpAddressEqual_b != FALSE )
                {
                    /* If the requested IP address is already assigned, check if it was assigned for a different LocalAddrId mapped to the same EthIfCtrlIdx. */
                    if( lLocalAddrId_u8 != LocalAddrId )
                    {
                        /* If it is requested for a different localAddrId, return TCPIP_ADDR_E_NOT_OK and throw DET */
                        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_REQUEST_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_ADDRINUSE )

                        lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;
                    }
                    else
                    {
                        /* If the requested IP address is assigned for the same LocalAddrId,  */
                        /* then return TCPIP_ADDR_ASSIGNED indicating that the IP address is already assigned. */
                        lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                    }

                    break;
                }
            }
        }
    }
    else
    {
        /* Return TCPIP_ADDR_E_NOT_OK. */
        lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;
    }

    return lTcpIp_AddrRetType_en;
}
#endif


/********************************************************************************************************************/
/* TcpIp_PerformUnicastIPAddrChecks_IPv6() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv6().*/
/*                                 It performs some basic checks on the IP address to be assigned of UNICAST type.     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall be initiated.    */
/*      IPv6IPAddr_pcst - Pointer to structure containing the IPv6 address which shall be assigned to EthIf         */
/*                      controller indirectly specified via LocalAddrId.                                            */
/*      IPv6DefaultRouter_pcst - Pointer to structure containing the IPv6 address of the default router (gateway)   */
/*                      which shall be assigned.                                                                    */
/*      Netmask_cu8     - Mask prefix of IPv6 address in CIDR Notation.                                             */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      TcpIp_AddrReturnType  TCPIP_ADDR_E_OK:     The specified IP address has passed all the unicast checks.      */
/*                            TCPIP_ADDR_E_NOT_OK: The specified IP address did not pass all the unicast checks.    */
/*                            TCPIP_ADDR_ASSIGNED: The specified IP address is already assigned to the localAddrId. */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static TcpIp_AddrReturnType TcpIp_PerformUnicastIPAddrChecks_IPv6( TcpIp_LocalAddrIdType LocalAddrId,
                                                                   const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                                                   const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst,
                                                                   const uint8 Netmask_cu8 )
{
    /* Local variables declaration */
    TcpIp_AddrReturnType    lTcpIp_AddrRetType_en;
    uint32                  lfirstIpv6AddrByte_u32 = IPv6IPAddr_pcst->addr[0];
    boolean                 lIPAddrEqual_b;
    boolean                 lNetworkAddrEqual_b;
    boolean                 lDefaultRtrIsNetworkAddr_b;

    /* Initialize the return value of the function to TCPIP_ADDR_E_NOT_OK */
    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

    /* -------------------------------------- */
    /* Check static IP                        */
    /* -------------------------------------- */

    /* - Netmask must be in valid range: [1, 128] */
    /* - Static IP must not be of type Multicast (must not start with 0xFF) */
    if( ((Netmask_cu8 >= 1U) && (Netmask_cu8 <= TCPIP_IPV6_CIDR_MASK_128)) &&
         ((lfirstIpv6AddrByte_u32 & TCPIP_IPV6_MULTICAST_MASK) != TCPIP_IPV6_MULTICAST_MASK) )
    {
        /* Static IP shall not be network address */
        if( TcpIp_IsIPv6AddrEqualToNetworkAddr( &(IPv6IPAddr_pcst->addr[0]), Netmask_cu8 ) == FALSE )
        {
            /* Set return value to TCPIP_ADDR_E_OK to reinitiate the IP address assignment process. */
            lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;
        }
    }

    /* If static IP is valid */
    if( lTcpIp_AddrRetType_en != TCPIP_ADDR_E_NOT_OK )
    {
        /* -------------------------------------- */
        /* Check already assigned                 */
        /* -------------------------------------- */

        /* Enter critical section: Current assigned address structure can be modified in a different execution context. */
        /* Hence the accesses to the current assigned address structure shall be protected in a critical section. */
        SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

        /* If the requested IP address and netmask are already assigned, */
        /* then return TCPIP_ADDR_ASSIGNED indicating that the IP address is already assigned. */
        if(( TcpIp_IPv6AddrCompareWithMask(
                     &( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.IpAddr_st.Addr_au32[0] ),
                     &( IPv6IPAddr_pcst->addr[0] ), TCPIP_IPV6_CIDR_MASK_128 ) != FALSE )  &&
           (( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.NetMask_u8 ) == Netmask_cu8 ))
        {
            /* If default router is given in argument */
            if( NULL_PTR != IPv6DefaultRouter_pcst )
            {
                /* If the IP address and netmask are already assigned, check if the requested default router address is also the same. */
                if( TcpIp_IPv6AddrCompareWithMask(
                     &( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[0] ),
                     &( IPv6DefaultRouter_pcst->addr[0] ), TCPIP_IPV6_CIDR_MASK_128 ) != FALSE )
                {
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                }
            }
            else
            {
                /* If the requested IP address and netmask are already assigned and the requested default router IP address is NULL_PTR, */
                /* check if the current assigned default router IP address is configured (different than 0:0:0:0:0:0:0:0/128). */
                if( ((TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[0] == 0uL) &&
                     (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[1] == 0uL) &&
                     (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[2] == 0uL) &&
                     (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].CurrAsgnedAddr_pst->IpAddrParams_un.IPv6Par_st.DftRtr_st.Addr_au32[3] == 0uL)) != FALSE )
                {
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_ASSIGNED;
                }
            }
        }

        /* Exit critical section. */
        SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess();

        /* -------------------------------------- */
        /* Check default router                   */
        /* -------------------------------------- */

        /* If IPv6DefaultRouter_pcst pointer is not NULL, implies that default router is required and shall be within the valid range */
        /* The domain for the default router is already verified in a DET check in the main API */
        if( (TCPIP_ADDR_E_OK == lTcpIp_AddrRetType_en) &&
            (NULL_PTR != IPv6DefaultRouter_pcst) )
        {
            /* Check Default router IPv6 address only if it is configured! (different than 0:0:0:0:0:0:0:0/128) */
            /* The 0:0:0:0:0:0:0:0/128 (Unspecified) IPv6 address for Default router is acceptable */
            if( ( ( IPv6DefaultRouter_pcst->addr[0] == 0uL ) &&
                  ( IPv6DefaultRouter_pcst->addr[1] == 0uL ) &&
                  ( IPv6DefaultRouter_pcst->addr[2] == 0uL ) &&
                  ( IPv6DefaultRouter_pcst->addr[3] == 0uL ) ) == FALSE )
            {
                /* Reset the return value of the function to TCPIP_ADDR_E_NOT_OK */
                lTcpIp_AddrRetType_en = TCPIP_ADDR_E_NOT_OK;

                /* Compare static IP and default router */
                lIPAddrEqual_b = TcpIp_IPv6AddrCompareWithMask( &( IPv6IPAddr_pcst->addr[0] ), &( IPv6DefaultRouter_pcst->addr[0] ), TCPIP_IPV6_CIDR_MASK_128 );

                /* Compare network address of static IP and default router */
                lNetworkAddrEqual_b = TcpIp_IPv6AddrCompareWithMask( &( IPv6IPAddr_pcst->addr[0] ), &( IPv6DefaultRouter_pcst->addr[0] ), Netmask_cu8 );

                /* Check if default router is the network address */
                lDefaultRtrIsNetworkAddr_b = TcpIp_IsIPv6AddrEqualToNetworkAddr( &(IPv6DefaultRouter_pcst->addr[0]), Netmask_cu8 );

                /* Static IP and default router shall NOT have the same address */
                /* Static IP and default router shall have same network address */
                /* Discard the request if default router IP is: */
                /* 1. Loop back IP (::1/128) */
                /* 2. Multicast address */
                /* 3. Network address */
                if( (lIPAddrEqual_b == FALSE) &&
                    (lNetworkAddrEqual_b != FALSE) &&
                    (((IPv6DefaultRouter_pcst->addr[0] == 0uL) && (IPv6DefaultRouter_pcst->addr[1] == 0uL) &&               /* Default router shall not be Loop back IP (::1/128) */
                      (IPv6DefaultRouter_pcst->addr[2] == 0uL) && (IPv6DefaultRouter_pcst->addr[3] == 1uL)) == FALSE) &&
                    ((IPv6DefaultRouter_pcst->addr[0] & TCPIP_IPV6_MULTICAST_MASK) != TCPIP_IPV6_MULTICAST_MASK) &&         /* Default router shall not be Multicast address */
                    (lDefaultRtrIsNetworkAddr_b == FALSE) )                                                                 /* Default router shall not be Network address */
                {
                    /* Set return value to TCPIP_ADDR_E_OK TCPIP_ADDR_E_OK to reinitiate the IP address assignment process. */
                    lTcpIp_AddrRetType_en = TCPIP_ADDR_E_OK;
                }
            }
        }
    }

    return lTcpIp_AddrRetType_en;
}
#endif


/********************************************************************************************************************/
/* TcpIp_StoreIpAddrAssignment_IPv6() - This is a helper function called by TcpIp_RequestIpAddrAssignment_IPv6() and*/
/*                        stores the provided local IPv6 address, default router address and netmask info.          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrConfig_pcst - Pointer to the local IPv6 address config structure.                                  */
/*      IPv6IPAddr_pcst - Pointer to structure containing the IP address which shall be assigned to EthIf controller*/
/*                        indirectly specified via LocalAddrId.                                                     */
/*      Netmask_cu8     - Mask prefix of IPv6 address in CIDR Notation.                                             */
/*      IPv6DefaultRouter_pcst - Pointer to structure containing the IP address of the default router (gateway)     */
/*                        which shall be assigned.                                                                  */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                      */
/*                        E_NOT_OK: The request has not been accepted.                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV6_ENABLED == STD_ON)
static void TcpIp_StoreIpAddrAssignment_IPv6( const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                              const TcpIp_SockAddrInet6Type * IPv6IPAddr_pcst,
                                              const uint8 Netmask_cu8,
                                              const TcpIp_SockAddrInet6Type * IPv6DefaultRouter_pcst )
{
    /* Local variables declaration */
    uint8  lLoopIndex_u8;

    /* Enter critical section: Manual static IP address structure can be accessed in a different execution context, by the TcpIp_MainFunction(). */
    /* Hence the manual static address shall be protected under critical section when it is modified. */
    SchM_Enter_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();

    for(lLoopIndex_u8 = 0; lLoopIndex_u8 < 4u; lLoopIndex_u8++)
    {
        /* Copy IPv6 address param to static IPv6 address config */
        LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.IpAddr_st.Addr_au32[lLoopIndex_u8] = IPv6IPAddr_pcst->addr[lLoopIndex_u8];

        /* Initialize Default Router IPv6 address with default value */
        LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.DftRtr_st.Addr_au32[lLoopIndex_u8] = 0;
    }

   LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.NetMask_u8 = 0;   /* Initialize netmask prefix with default value */

    /* Netmask and default router are valid only for unicast IP */
    if(TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en)
    {
        /* For the IPv6 structure, the netmask is stored in CIDR notation (prefix). */
        LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.NetMask_u8 = Netmask_cu8;

        /* If IPv6DefaultRouter_pcst is not NULL, store default router IP from received param */
        if(NULL_PTR != IPv6DefaultRouter_pcst)
        {
            for(lLoopIndex_u8 = 0; lLoopIndex_u8 < 4u; lLoopIndex_u8++)
            {
               LocalAddrConfig_pcst->LocalIpReqRel_pst->ManualStaticIpAddr_pun->IPv6Par_st.DftRtr_st.Addr_au32[lLoopIndex_u8] = IPv6DefaultRouter_pcst->addr[lLoopIndex_u8];
            }
        }
    }

    /* Exit critical section. */
    SchM_Exit_TcpIp_ExclusiveAreaManualStaticIPAddrAccess();
}
#endif

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

