

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
 * \Function Name : SoAd_TpCancelReceive()
 *
 * \Function description
 * Requests cancellation of the reception for a specific I-PDU
 *
 * Parameters (in):
 * \param   PduIdType
 * \arg     PduId - Identifiaction of the I-PDU to be cancelled.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):

 * Return value :Std_ReturnType
 * \retval  E_OK:       request accepted (but not yet performed).
 *          E_NOT_OK:   request not accepted (e.g. cancellation not possible).
 *
 ***************************************************************************************************
 */

Std_ReturnType SoAd_TpCancelReceive( PduIdType PduId)
{
#if ( (SOAD_TP_CANCEL_COM_ENABLED != STD_OFF) && (SOAD_TP_PRESENT != STD_OFF) )
    /* Variable to store the submode */
    SoAd_SoConModeType lSoConMode_en;
    /* Variable to store the return value */
    Std_ReturnType     lFunctionRetVal_u8;

    /* Variable to hold the index of the static soc table */
    SoAd_SoConIdType   lIdxstaticsocket_uo;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType   lIdxDynSocket_uo;

    boolean            ltpRxOngoing_b;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_TP_CANCEL_RECEIVE_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether PduId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noRxPdus_uo <= PduId ), SOAD_TP_CANCEL_RECEIVE_API_ID, SOAD_E_INV_PDUID, E_NOT_OK )

    /********************************* Start: DET Checking *************************************/

    SchM_Enter_SoAd_TpPduInUse();
    /* check if TpCancelRx for this PDU is in progress*/
    if( FALSE == SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[PduId].rxPduDynInfo_pst->isCancelTpRxCalled_b )
    {
        /* set a flag to indicate Tp cancelRx request processing is in progress*/
        SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[PduId].rxPduDynInfo_pst->isCancelTpRxCalled_b = TRUE;

        SchM_Exit_SoAd_TpPduInUse();
        /* Initialize the local variables which are declared above */
        /* get the static socket id from the RxPdu */
        lIdxstaticsocket_uo = SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[PduId].rxStaticSocketOrGroupIdx_uo;

        /* Get "the dynamic socket index value" from the static socket table  */
        lIdxDynSocket_uo =   (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxstaticsocket_uo].SoAd_Prv_idxDynSocTable_puo) ;

        /*Internal Check: lIdxDynSocket_uo shall be within the range*/
        if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo )
        {
            /* condition check: if TP Reception is going on */
             /*Cancel receive request is called for a particular PduId, so SoAd shall check whether the requested PduId
             * related reception is going on? or for example 4 RxPdus linked with same socket, TpReception is going on related
             * with the 1st RxPdu, and cancel Receive request comes for 3rd RxPdu, then SoAd should not cancel the Reception of
             * 1st RxPdu.*/

            lSoConMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en;

            ltpRxOngoing_b = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpReceptionOngoing_b;

            if( ( SOAD_SOCON_ONLINE == lSoConMode_en ) && ( TRUE == ltpRxOngoing_b ) )
            {
                /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                SchM_Enter_SoAd_OpenCloseReq();

                /* Check if there is no close request from UL or Tcp/Ip*/
                if((SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en  != SOAD_EVENT_OFFLINEREQ_UL) && \
                   (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en  != SOAD_EVENT_OFFLINEREQ_TCPIP))
                {
                    /* set a request to cancel it in next main function: as per SWS_SoAd_00581 */
                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en = SOAD_EVENT_OFFLINEREQ_SOAD ;


                }

                SchM_Exit_SoAd_OpenCloseReq();

                /* set lFunctionRetVal_u8 to E_OK */
                lFunctionRetVal_u8          = E_OK;

            }
        }

        /* Reset  flag to indicate Tp cancelRx request processing is completed */
        SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[PduId].rxPduDynInfo_pst->isCancelTpRxCalled_b = FALSE;
    }
    else
    {
        SchM_Exit_SoAd_TpPduInUse();
    }

    return(lFunctionRetVal_u8);
#else
    (void)PduId;

    return(E_NOT_OK);
#endif /* SOAD_TP_CANCEL_COM_ENABLED != STD_OFF && SOAD_TP_PRESENT != STD_OFF */
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
