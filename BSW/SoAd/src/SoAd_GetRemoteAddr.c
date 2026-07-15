

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
 *************************************************************************************************************************
 * \Function Name : SoAd_GetRemoteAddr()
 *
 * \Function description
 *  Retrieves the remote address (IP address and port) actually used for the SoAd socket connection specified by SoConId.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - Socket connection index representing the SoAd socket connection for which the actually specified
 *                    remote address shall be obtained.
 *
 *  Parameters (inout): None
 *
 *  Parameters (out):
 * \param   TcpIp_SockAddrType*
 * \arg     IpAddrPtr - Pointer to a struct where the retrieved remote address (IP address and port) is stored.
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request was successful
 *          E_NOT_OK: The request was not successful
 *
 *
 ************************************************************************************************************************
 */
/* This function shall be called from upper layer of SoAd, if it receives a pdu via SoAd_Rxindication() and it is
 * interested to know from which remote node the message is received. Then upper layer shall call the below function */

Std_ReturnType SoAd_GetRemoteAddr( SoAd_SoConIdType     SoConId ,\
                                   TcpIp_SockAddrType * IpAddrPtr )
{
    /* Local pointer to store the Dynamic array for given index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the return value */
    Std_ReturnType               lFunctionRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_GET_REMOTE_ADDR_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether IpAddrPtr pointer is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == IpAddrPtr ), SOAD_GET_REMOTE_ADDR_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ) || \
                                 ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo].domain_en != IpAddrPtr->domain) ), \
                                 SOAD_GET_REMOTE_ADDR_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Initialize the return value with default E_NOT_OK and other local variables */
    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].SoAd_Prv_idxDynSocTable_puo);

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00666] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** SoAd_GetRemoteAddr() shall immediately return E_NOT_OK if the remote address of the socket connection *******/
    /**                 specified by parameter SoConId is not set.                                               ****/
    /****************************************************************************************************************/

    /* If dynamic socket is valid means , we can assume that remote address has been updated for this SoAd either
     * with configed Remote address or from appropriate UL */
    if( lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00664] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /* At SoAd_GetRemoteAddr() SoAd shall retrieve the remote address (IP address and port) actually used for *******/
        /*************************the socket connection specified by parameter SoConId. *********************************/
        /****************************************************************************************************************/

        /* copy the global array index address to local pointer for further processing */
        lSoAdDyncSocConfig_pst      =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
        SchM_Enter_SoAd_RemoteAddrUpdate();

        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_CopyRemoteAddrFromDyncSocToSockAddr(lSoAdDyncSocConfig_pst, IpAddrPtr);

        SchM_Exit_SoAd_RemoteAddrUpdate();

        /* Return value shall be set to E_OK */
        lFunctionRetVal_u8  = E_OK;
    }

    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
