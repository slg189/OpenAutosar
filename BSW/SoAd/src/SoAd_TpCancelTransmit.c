

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_DEBUG_AND_TEST
#include "TestSoAdApplication.h"
#endif
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
 * \Function Name : SoAd_TpCancelTransmit()
 *
 * \Function description
 * Requests cancellation of the transmission via TP for a specific I-PDU
 *
 * Parameters (in):
 * \param   PduIdType
 * \arg     PduId - Identifiaction of the I-PDU to be cancelled.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):

 * Return value :Std_ReturnType
 * \retval  E_OK :      The request was successful
 *          E_NOT_OK:   The request was not successful
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_TpCancelTransmit( PduIdType PduId)
{
#if ( (SOAD_TP_CANCEL_COM_ENABLED != STD_OFF) && (SOAD_TP_PRESENT != STD_OFF) )
    /* pointer to store the pduroutedest information */
    const PduRouteDestType_tst * lPduRRouteDest_cpst;

    /* Variable to store the submode */
    SoAd_SoConModeType           lSoConMode_en;

    /* Variable to store the return value */
    Std_ReturnType               lFunctionRetVal_u8;

    /* Variable to hold the index of PduRouteDests configured for this PduRoute*/
    PduIdType                    lIdxOfPduRouteDest_uo;

    /* Variable to hold the index of the static soc table */
    SoAd_SoConIdType             lIdxstaticsocket_uo;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /*Variable to store the number of pdu route destinations*/
    PduIdType                    lNumberofPduRouteDests_uo;

    boolean                      ltpTxOngoing_b;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_TP_CANCEL_TRANSMIT_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether PduId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noTxPdus_cuo <= PduId ), SOAD_TP_CANCEL_TRANSMIT_API_ID, SOAD_E_INV_PDUID, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    SchM_Enter_SoAd_TpPduInUse();
    /* check if TpCancelTx for this PDU is in progress*/
    if( FALSE == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[PduId].txPduDyninfo_pst->isCancelTpTxCalled_b)
    {
        /* set a flag to indicate Tp cancelTx request processing is in progress*/
        SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[PduId].txPduDyninfo_pst->isCancelTpTxCalled_b = TRUE;
        SchM_Exit_SoAd_TpPduInUse();

        /* Initialize the local variables which are declared above */
        /* Get the number of PduRouteDests configured with this PduRoute */
        lNumberofPduRouteDests_uo = SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[PduId].PduRouteDestNum_uo ;

        /* Extract pointer to first element of Pdu route destination structure */
        lPduRRouteDest_cpst = ( SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[PduId].PduRouteDestConf_cpst );

        /* Loop through all the route dests to transmit the Pdu */
        for( lIdxOfPduRouteDest_uo =0; lIdxOfPduRouteDest_uo < lNumberofPduRouteDests_uo; lIdxOfPduRouteDest_uo++)
        {
            /* Get the static socket index value for the associated TxPdu  */
            lIdxstaticsocket_uo = ( lPduRRouteDest_cpst[lIdxOfPduRouteDest_uo].txStaticSocketOrGroupIdx_uo );

            /* Get "the dynamic socket index value" from the static socket table  */
            lIdxDynSocket_uo =   (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxstaticsocket_uo].SoAd_Prv_idxDynSocTable_puo) ;
#ifdef SOAD_DEBUG_AND_TEST
        SoAd_DynSocket_tst = lIdxDynSocket_uo;
#endif

            /* Condition check: Dynamic socket shall be valid */
            if( lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo )
            {

                lSoConMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en;

                ltpTxOngoing_b = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTransmissionOnGoing_b;

                if((SOAD_SOCON_ONLINE == lSoConMode_en ) && ( TRUE == ltpTxOngoing_b ))
                {
                    /* Condition check: Dynamic socket shall be valid and the socket connection shall be ONLINE */
                    /*                      and Socket is not requested to close (not in a position which blocks Tx and Rx )
                     * Cancel transmit request is called for a particular PduId, so SoAd shall check whether the requested PduId
                     * related transmission is going on? or for example 4 TxPdus linked with same socket, TpTransmission is going on related
                     * with the 1st TxPdu, and cancel transmit request comes for 3rd TxPdu, then SoAd should not cancel the transmission of
                     * 1st TxPdu.*/

                    /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                    SchM_Enter_SoAd_OpenCloseReq();

                    /* Check if there is no close request from UL or Tcp/Ip*/
                    if((SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en  != SOAD_EVENT_OFFLINEREQ_UL) && \
                       (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en  != SOAD_EVENT_OFFLINEREQ_TCPIP))
                    {
                        /* set a request to cancel it in next main function*/
                        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en = SOAD_EVENT_OFFLINEREQ_SOAD ;

                    }

                    SchM_Exit_SoAd_OpenCloseReq();
                    /* set lFunctionRetVal_u8 to E_OK */
                    lFunctionRetVal_u8 = E_OK;
                }

            }

        }

        /* Reset  flag to indicate Tp cancelTx request processing is completed */
        SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[PduId].txPduDyninfo_pst->isCancelTpTxCalled_b = FALSE;
    }
    else
    {
        SchM_Exit_SoAd_TpPduInUse();
    }

        return(lFunctionRetVal_u8);
#else
    (void)PduId;

    return(E_NOT_OK);
#endif /* SOAD_TP_CANCEL_COM_ENABLED != STD_OFF &&  && SOAD_TP_PRESENT != STD_OFF*/
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
