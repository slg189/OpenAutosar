

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED
#include "TcpIp.h"

#include "SoAd_Prv.h"

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
 ******************************************************************************************************************************
 * \Function Name : SoAd_WriteDhcpHostNameOption()
 *
 * \Function description
 * By this API service an upper layer of the SoAd can set the hostname option in the DHCP submodule of the TCP/IP stack.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index specifying the socket connection for which the parameter shall be changed
 * \param   uint8
 * \arg     length - Length of parameter value to be set.
 * \param   uint8
 * \arg     data - Pointer to memory containing the new parameter value.
 *
 * Parameters (inout): None
 *
 * Parameters (out)  : None
 *
 * Return value      : E_OK: The request has been accepted
 *                     E_NOT_OK: The request has not been accepted
 *
 ******************************************************************************************************************************
 */
Std_ReturnType SoAd_WriteDhcpHostNameOption( SoAd_SoConIdType SoConId,
                                             uint8            length,
                                             const uint8 *    data )
{
#if (SOAD_DHCPHOSTNAME_ENABLED != STD_OFF)
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Variable to store the return value */
    Std_ReturnType                            lFunctionRetVal_u8;

    /* Variable to hold the DHCP Option to call TCPIP function */
    uint8                                     lDhcpOption_u8;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_WRITE_DHCP_HOST_NAME_OPTION_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ), SOAD_WRITE_DHCP_HOST_NAME_OPTION_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo]);

    switch( lSoAdStaticSoConGrpConfig_cpst->domain_en)
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00707] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** The service SoAd_WriteDhcpHostNameOption() shall forward the call to TcpIp_DhcpWriteOption() with         ***/
        /** the parameter Option set to the option code 81 according to IETF RFC 4702, if the socket connection       ***/
        /** identified by SoConId is related to a local address of the TcpIp_DomainType TCPIP_AF_INET                 ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        case TCPIP_AF_INET:
        {
            /* Set DHCP Host Name Option 81 - IETF RFC 4702 */
            lDhcpOption_u8           = 81;

            /* Call the TCPIP function */
            lFunctionRetVal_u8 = TcpIp_DhcpWriteOption( lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalAddressIdx_u8, lDhcpOption_u8, length, data );

            break;
        }

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00708] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** The service SoAd_WriteDhcpHostNameOption() shall forward the call to TcpIp_DhcpV6ReadOption() with the    ***/
        /** parameter Option set to the option code 39 according to IETF RFC 4704, if the socket connection           ***/
        /** identified by SoConId is related to a local address of the TcpIp_DomainType TCPIP_AF_INET6                ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        case TCPIP_AF_INET6:
        {
            /* Set DHCP Host Name Option 39 - IETF RFC 4704 */
            lDhcpOption_u8           = 39;

            /*TcpIp_DhcpV6WriteOption() api will be called once it presents*/
            break;
        }

        default:
        {
            /* Nothing to do */
            break;
        }
    }


#ifdef SOAD_DEBUG_AND_TEST
        lFunctionRetVal_u8 = E_OK;
#endif

    return (lFunctionRetVal_u8);
#else
    (void)SoConId;
    (void)length;
    (void)data;

    return(E_NOT_OK);
#endif /* SOAD_DHCPHOSTNAME_ENABLED != STD_OFF */

}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
