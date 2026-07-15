

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
 ************************************************************************************************************************
 * \Function Name : SoAd_ReleaseIpAddrAssignment()
 *
 * \Function description
 *  By this API service the local IP address assignment which shall be released for the socket connection specified by
 *  SoConIdx.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index specifying the socket connection for which the IP address shall be released
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request has been accepted
 *          E_NOT_OK: The request has not been accepted
 *
 *
 ************************************************************************************************************************
 */
Std_ReturnType SoAd_ReleaseIpAddrAssignment( SoAd_SoConIdType SoConId )
{
#if (SOAD_IPADDR_ASSIGNMENT_ENABLED != STD_OFF)
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Variable to hold the return value */
    Std_ReturnType                            lFunctionRetVal_u8;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_RELEASE_IPADDR_ASSIGNMENT_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( (SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ), SOAD_RELEASE_IPADDR_ASSIGNMENT_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* Start: DET Checking *************************************/

    /* Initialize the local variables which are declared above */
    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo]);

    /* Call the TcpIp module function with local address index associated with the given socket Id */
    lFunctionRetVal_u8 = TcpIp_ReleaseIpAddrAssignment(lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalAddressIdx_u8 );

    return(lFunctionRetVal_u8);
#else
    (void)SoConId;

    return(E_NOT_OK);
#endif /* SOAD_IPADDR_ASSIGNMENT_ENABLED != STD_OFF */
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
