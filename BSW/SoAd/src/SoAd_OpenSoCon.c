

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "SoAd_Prv.h"

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
 ***************************************************************************************************
 * \Function Name : SoAd_OpenSoCon()
 *
 * \Function description
 * This API opens the socket connection specified by SoConId.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId -socket connection index specifying the socket connection which shall be opened
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK            The request was successful
 *          E_NOT_OK        The request was not successful.
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_OpenSoCon( SoAd_SoConIdType SoConId )
{
    /* Local pointer to store the Dynamic array for given index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to store the return value */
    Std_ReturnType               lFunctionRetVal_u8;

    /* Index to store the dynamic socket connection */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_OPEN_SOCON_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid & socket is configured as automatic or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ) || \
                                 ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo].soConGrpAutoConSetup_b ) == TRUE ) ), \
                                    SOAD_OPEN_SOCON_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

     /********************************* End: DET Checking *************************************/

    /* Initialize the local variables which are declared above */
    /* Get the index of the dynamic socket table which is linked with this static socket*/
    lIdxDynSocket_uo = ( *SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].SoAd_Prv_idxDynSocTable_puo) ;

    /*Condition check: If the dynamic socket is not linked or allocated to the static socket
     * There is a possibility that SoAd_OpenSoCon shall be called just after SoAd_SetRemoteAddress.
     * So, already a link to dynamic socket table is established at that time. So the same dync socket shall be used now*/
    if(SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo <= lIdxDynSocket_uo )
    {
        /* Get the available dynamic socket from the list by calling a function */
        SoAd_ReserveFreeDynSocket(SoConId, &lIdxDynSocket_uo);
    }

    /* If dynamic socket is valid
     * usecase: SoAd_SetRemoteAddr is called before SoAd_OpenSoCon is called */
    if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
    {
        /* copy the global array index address to local pointer for further processing */
        lSoAdDyncSocConfig_pst =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00588] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** SoAd shall store a request to open or close a socket connection when called with SoAd_OpenSoCon() and     ***/
        /** SoAd_CloseSoCon() respectively, but handle the request only in the SoAd_MainFunction() respecting the     ***/
        /**             connection setup and shutdown policy.                                                         ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
        SchM_Enter_SoAd_OpenCloseReq();

        /* Increment the socConOpenReqCnt_u16 variable by 1 and compare the socConOpenReqCnt_u16 with socConCloseReqCnt_u16 in next MainFunction*/
        /* if no of open req is greater than close req, then set the event to onlinereq, such that open req action shall be taken place in the same main function */
        lSoAdDyncSocConfig_pst->socConOpenReqCnt_u16++;

        SchM_Exit_SoAd_OpenCloseReq();

        lFunctionRetVal_u8 = E_OK;
    }

    /* Return the value  */
    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
