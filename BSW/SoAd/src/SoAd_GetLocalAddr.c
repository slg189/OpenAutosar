

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv_Domain.h"


#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

/**
 ********************************************************************************************************************************************
 * \Function Name : SoAd_GetLocalAddr()
 *
 * \Function description
 *  Obtains the local address (IP address and port) actually used for the SoAd socket connection specified by SoConIdx,
 *   the netmask and default router.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index representing the SoAd socket connection for which the actual local IP address
 *                          shall be obtained.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):
 * \param   TcpIp_SockAddrType*
 * \arg     LocalAddrPtr - Pointer to struct where the IP address of the default router (gateway) is stored
 *                           (struct member port is not used and of arbitrary value).
 * \param   uint8*
 * \arg     NetmaskPtr - Pointer to memory where Network mask of IPv4 address or address prefix of IPv6 address in CIDR Notation is stored.
 * \param   TcpIp_SockAddrType*
 * \arg     DefaultRouterPtr - Pointer to a struct where the local address (IP address and port) is stored.
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request has been accepted
 *          E_NOT_OK: The request has not been accepted
 *
 *
 *************************************************************************************************************************************************
 */
Std_ReturnType SoAd_GetLocalAddr( SoAd_SoConIdType     SoConId ,\
                                  TcpIp_SockAddrType * LocalAddrPtr,\
                                  uint8 *              NetmaskPtr,\
                                  TcpIp_SockAddrType * DefaultRouterPtr)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Variable to hold the return value */
    Std_ReturnType   lFunctionRetVal_u8;

    /* Variable to store the return value of the internal functions */
    Std_ReturnType   lRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* Variable to store the Local port */
    uint16           lLocalPort_u16;

    /* LocalAddrPtr_cpst can store the local address. */
    boolean          lCanStoreLocalAddr_b;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_GET_LOCAL_ADDR_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ), SOAD_GET_LOCAL_ADDR_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == LocalAddrPtr ), SOAD_GET_LOCAL_ADDR_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Check whether LocalAddrPtr_cpst can store the local address */
    lCanStoreLocalAddr_b = SOAD_SOCK_ADDR_CAN_STORE_CON_ADDR(LocalAddrPtr, SoConId);

    /* if LocalAddrPtr_cpst is able to store the local address */
    if(TRUE == lCanStoreLocalAddr_b)
    {
        /* replacing of the Domain name */
        (LocalAddrPtr)->domain = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(SoConId)].soConGrpId_uo].domain_en;

        /* Initialize the local variables that are declared above */
        lLocalPort_u16 = SOAD_DEFAULT_PORT;

        /* Get static socket Group configuration pointers */
        lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo]);

        /* call tcp ip layer to get the ip address */
        lRetVal_u8 = TcpIp_GetIpAddr( lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalAddressIdx_u8, LocalAddrPtr, NetmaskPtr, DefaultRouterPtr);

        /* Condition check: The function call to TcpIp layer should be successful */
        if( E_OK == lRetVal_u8 )
        {
#if (SOAD_USE_LOCAL_PORT_FROM_CONFIG != STD_OFF)

            /* Condition to check if the local port is not configured to ANY */
            if(TCPIP_PORT_ANY != lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalPort_u16)
            {
                /* Update the local port from the Static Socket Configuration */
                lLocalPort_u16 = lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalPort_u16;
            }
            else
#endif /* (SOAD_USE_LOCAL_PORT_FROM_CONFIG != STD_OFF) */
            {
                /* Get the index of the dynamic socket table */
                lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].SoAd_Prv_idxDynSocTable_puo);

                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();

                /* Condition to check if the socket is in bound or higher state */
                if( ( lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo ) && \
                ( SOAD_SOCON_BOUND <= SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en ) )
                {
                    /* Update the Socket port from the dynamic socket configuration */
                    lLocalPort_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].soConDynLocalPort_u16;
                }

                SchM_Exit_SoAd_SocConModeProperties();
            }

            /* when either the Socket is in Bound state or when the local port is configured with ANY(wildcard),local port will be not equal to SOAD_DEFAULT_PORT */
            if( SOAD_DEFAULT_PORT != lLocalPort_u16 )
            {
                /* Load the port value from the configured value*/
                /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
                SoAd_SetPortToSockAddr(LocalAddrPtr, lLocalPort_u16);

                lFunctionRetVal_u8 = E_OK;
            }
        } /* end of if( E_OK == lRetVal_u8 ) */
    }/* end of if(TRUE == lCanStoreLocalAddr_b) */
    else
    {
        /* Log the DET */
        SOAD_DET_REPORT_ERROR( SOAD_GET_LOCAL_ADDR_API_ID, SOAD_E_INV_ARG )
    }

    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
