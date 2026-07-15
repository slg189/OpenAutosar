

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
 * \Function Name : SoAd_GetPhysAddr()
 *
 * \Function description
 *  Obtains the physical source address of the EthIf controller used by the SoAd socket connection specified by SoConIdx.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index representing the SoAd socket connection for which
 *                      the physical source address of the related EthIf controller shall be obtained.
 *
 *  Parameters (inout): None
 *
 *  Parameters (out):
 * \param   uint8*
 * \arg     PhysAddrPtr - Pointer to the memory where the physical source address (MAC address)
 *                              in network byte order is stored.
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request has been accepted
 *          E_NOT_OK: The request has not been accepted
 *
 *
 ************************************************************************************************************************
 */
Std_ReturnType SoAd_GetPhysAddr( SoAd_SoConIdType SoConId ,\
                                 uint8 *          PhysAddrPtr )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Variable to hold the return value */
    Std_ReturnType                            lFunctionRetVal_u8;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_GET_PHYS_ADDR_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ), SOAD_GET_PHYS_ADDR_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Initialize the local variables that are declared above */

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo]);

    /* call the tcpip layer to get the physical address */
    lFunctionRetVal_u8 = TcpIp_GetPhysAddr( lSoAdStaticSoConGrpConfig_cpst->soConGrpLocalAddressIdx_u8, PhysAddrPtr );

    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
