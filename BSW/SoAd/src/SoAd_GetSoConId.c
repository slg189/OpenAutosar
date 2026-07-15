

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
 *************************************************************************************************************************************
 * \Function Name : SoAd_GetSoConId()
 *
 * \Function description
 * Returns socket connection index related to specified transmit PduId.
 * In case a fan-out is configured for TxPduId (i.e. multiple SoAdPduRouteDest specified) E_NOT_OK shall be returned
 *
 * Parameters (in):
 * \param   PduIdType
 * \arg     TxPduId - Transmit PduId specifying the SoAd socket connection for which the socket connection index shall be returned.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):
 * \param   SoAd_SoConIdType *
 * \arg     SoConIdPtr - Pointer to memory receiving the socket connection index asked for.
 *
 * Return value :
 * \retval  E_OK :      The request was successful
 *          E_NOT_OK:   The request was not successful
 *
 ***************************************************************************************************************************************
 */

Std_ReturnType SoAd_GetSoConId( PduIdType          TxPduId ,\
                                SoAd_SoConIdType * SoConIdPtr )

{
    /* Local pointer to store the Tx Pdu table for given index */
    const SoAd_TxPduConfigType_tst * lSoAdTxPduConfig_cpst ;

    /* Variable to store the return value */
    Std_ReturnType                   lFunctionRetVal_u8;

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
    /* Variable to store the SoconGroupId if BundleGroup is Refering to the Socket Connection Group */
    SoAd_SoConIdType                 lSoConGrpId;
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_GET_SOCON_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether TxPduId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noTxPdus_cuo <= TxPduId ), SOAD_GET_SOCON_API_ID, SOAD_E_INV_PDUID, E_NOT_OK )

    /* Check whether SoConIdPtr pointer is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == SoConIdPtr ), SOAD_GET_SOCON_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Get the Tx Pdu pointer for given index */
    lSoAdTxPduConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[TxPduId]);

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00724] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** In case SoAd_GetSoConId() is called with a TxPduId related to a SoAdPduRoute with a fan-out (i.e. multiple **/
    /** SoAdPduRouteDest specified), SoAd_GetSoConId() shall skip further processings and return E_NOT_OK          **/
    /****************************************************************************************************************/

    /* Enter only If fanout (multiple PduRouteDests)NOT configured)*/
    if( 1U == lSoAdTxPduConfig_cpst->PduRouteDestNum_uo )
    {
#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
        /*Check if the Bundle reference is mapped to the socket connection */
        if( SOAD_SOCKET_BUNDLE_REFERENCE == lSoAdTxPduConfig_cpst->PduRouteDestConf_cpst->identifyTxSocketConnOrGroupRef_b )
        {
            /* The local variable is updated with the Socket Connection Group Id  */
            lSoConGrpId = (lSoAdTxPduConfig_cpst->PduRouteDestConf_cpst->txStaticSocketOrGroupIdx_uo) ;

            /* For the Socket connnection Group find the number of socket connections ,Enter only if there is one socket connection*/
            if(1U == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoConGrpId].maxSoConChannel_uo  )
            {
                /*The socket Id associated with the given PduId shall be updated in the pointer given by the upper layer*/
                (* SoConIdPtr) = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoConGrpId].startSoConIdx_uo ;

                /* retVal is set to E_OK */
                lFunctionRetVal_u8 = E_OK;
            }
        }
        else
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
        {
            /* The socket Id associated with the given PduId shall be updated in the pointer given by the upper layer */
            (* SoConIdPtr) = lSoAdTxPduConfig_cpst->PduRouteDestConf_cpst->txStaticSocketOrGroupIdx_uo  ;

            /* retVal is set to E_OK */
            lFunctionRetVal_u8 = E_OK;
        }
    }

    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
