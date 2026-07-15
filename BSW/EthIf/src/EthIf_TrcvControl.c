

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
#include "EthIf_Cfg_SchM.h"
#include "EthTrcv.h"
#endif

#include "EthIf_Prv.h"
/* BSW-12205 */
/* BSW-10868 */
/* BSW-486 */
#include "EthIf_Integration.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static Std_ReturnType EthIf_Prv_SetTrcvDown   ( uint8 EthIfTrcvIdx_u8 );

static Std_ReturnType EthIf_Prv_SetTrcvActive ( uint8 EthIfTrcvIdx_u8 );
#endif

/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \module description
 * Initialises the indexed transceiver
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * \return  Std_ReturnType {E_OK: success;E_NOT_OK: transceiver could not be initialized}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
Std_ReturnType EthIf_Prv_TransceiverInit( uint8 EthIfCtrlIdx_u8 )

{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;
    EthIf_DynamicTrcv_tst*      lDynamicTrcv_pst;

    /* If requested EthTrcv is no more in state ETHIF_TRCV_STATE_UNINIT no need to call transceiver API. Return with E_OK */
    lStdRetVal_en = E_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Check if EthTrcv initialization is not already called */
    if( lDynamicTrcv_pst->TrcvState_en == ETHIF_TRCV_STATE_UNINIT )
    {
        lStdRetVal_en = EthTrcv_TransceiverInit( lDynamicTrcv_pst->EthTrcvIdx_cu8,
                                                 0U );

        /* If driver API returns E_OK set EthCtrl state in EthIf to ETHIF_CTRL_STATE_INIT */
        if( lStdRetVal_en == E_OK )
        {
            lDynamicTrcv_pst->TrcvState_en = ETHIF_TRCV_STATE_DOWN;
        }
    }

    return lStdRetVal_en;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Enables / disables the indexed transceiver
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8               Index of the EthIf Controller whose referenced Trcv mode has to be changed
 * \param TrcvReqMode_en                {ETHTRCV_MODE_DOWN: disable the transceiver, ETHTRCV_MODE_ACTIVE: enable the transceiver}
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: transceiver mode could not be changed}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
Std_ReturnType EthIf_Prv_SetTransceiverMode( uint8 EthIfCtrlIdx_u8,
                                             EthTrcv_ModeType TrcvReqMode_en )
{
    /* Declare local variables. */
    Std_ReturnType                  lStdRetVal_en;
    EthIf_DynamicTrcv_tst*          lDynamicTrcv_pst;

    /* Initialise local variables. */
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index. */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* -------------------------------------- */
    /* Request DOWN                           */
    /* -------------------------------------- */

    /* If EthIf_Prv_SetTransceiverMode() is called with ETHTRCV_MODE_DOWN */
    if( TrcvReqMode_en == ETHTRCV_MODE_DOWN )
    {
        /* Check the state EthTrcv in context of EthIf */
        switch( lDynamicTrcv_pst->TrcvState_en )
        {
            /* Trcv mode in the context of EthIf is DOWN which means the transition already done */
            /* This case is possible only when an unexpected DOWN indication was received (Trcv state is DOWN and there are EthIfCtrl which were still active) */
            case ETHIF_TRCV_STATE_DOWN:
            {
                /* To prevent self-healing of the Trcv, SetTransceiverMode(DOWN) has to be called. EthIf State Machine will be restarted. */
                lStdRetVal_en = EthIf_Prv_SetTrcvDown( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8 );

                if( lStdRetVal_en == E_OK )
                {
                    /* Decrement the ActiveCntr */
                    lDynamicTrcv_pst->ActiveCntr_u8 = (lDynamicTrcv_pst->ActiveCntr_u8 > 0U) ?
                            (lDynamicTrcv_pst->ActiveCntr_u8 - 1U) : (lDynamicTrcv_pst->ActiveCntr_u8);

                    /* E_OK will be returned */
                }
            }
            break;

            /* Trcv mode in the context of EthIf is UNINIT, in that case only decrement ActiveCntr */
            /* Trcv mode in the context of EthIf is WAIT_DOWN which means the transition is ongoing */
            case ETHIF_TRCV_STATE_UNINIT:
            case ETHIF_TRCV_STATE_WAIT_DOWN:
            {
                /* Decrement the ActiveCntr */
                lDynamicTrcv_pst->ActiveCntr_u8 = (lDynamicTrcv_pst->ActiveCntr_u8 > 0U) ?
                        (lDynamicTrcv_pst->ActiveCntr_u8 - 1U) : (lDynamicTrcv_pst->ActiveCntr_u8);

                /* Return with E_OK */
                lStdRetVal_en = E_OK;
            }
            break;

            /* Trcv mode in the context of EthIf is ACTIVE */
            case ETHIF_TRCV_STATE_ACTIVE:
            {
                /* Check the ActiveCntr of the Trcv to know if it is used by any other EthIfCtrl */
                if( lDynamicTrcv_pst->ActiveCntr_u8 == 1U )
                {
                    /* We are in presence of the last EthIfCtrl using the Trcv or unexpected DOWN indication received*/

                    /* Call EthTrcv API */
                    lStdRetVal_en = EthIf_Prv_SetTrcvDown( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8 );

                    if( lStdRetVal_en == E_OK )
                    {
                        /* Enter critical section: Trcv mode is being changed/updated. No parallel write shall occur ( for example in API EthIf_TrcvModeIndication()). */
                        /* The critical section is needed as unexpected Trcv mode DOWN indication can occur at point of time. (EthTrcv execution context can be different from EthIf_MainFunctionState() execution context) */
                        SchM_Enter_EthIf_SetTrcvState();

                        /* If the current state of the Trcv is ACTIVE, go to WAIT_DOWN */
                        lDynamicTrcv_pst->TrcvState_en = (lDynamicTrcv_pst->TrcvState_en == ETHIF_TRCV_STATE_ACTIVE) ?
                                (ETHIF_TRCV_STATE_WAIT_DOWN) : (lDynamicTrcv_pst->TrcvState_en);

                        /* Exit critical section. */
                        SchM_Exit_EthIf_SetTrcvState();

                        /* Decrement the ActiveCntr */
                        lDynamicTrcv_pst->ActiveCntr_u8--;

                        /* E_OK will be returned */
                    }
                }
                else
                {
                    /* Other EthIfCtrl are using the Trcv */

                    /* Decrement the ActiveCntr */
                    lDynamicTrcv_pst->ActiveCntr_u8 = (lDynamicTrcv_pst->ActiveCntr_u8 > 0U) ?
                            (lDynamicTrcv_pst->ActiveCntr_u8 - 1U) : (lDynamicTrcv_pst->ActiveCntr_u8);

                    /* Return with E_OK */
                    lStdRetVal_en = E_OK;
                }
            }
            break;

            /* Else case when EthTrcv mode in context of EthIf is WAIT_ACTIVE. E_NOT_OK has to be returned. */
            /* Mode cannot be changed during ongoing transition */
            default:
            {
                /* E_NOT_OK will be returned */
            }
            break;
        }
    }

    /* -------------------------------------- */
    /* Request ACTIVE                         */
    /* -------------------------------------- */

    /* If EthIf_Prv_SetTransceiverMode() is called with ETHTRCV_MODE_ACTIVE */
    else
    {
        /* Check the state EthTrcv in context of EthIf */
        switch( lDynamicTrcv_pst->TrcvState_en )
        {
            /* Trcv mode in the context of EthIf is WAIT_ACTIVE or ACTIVE */
            /* which means the transition is ongoing or already done */
            case ETHIF_TRCV_STATE_WAIT_ACTIVE:
            case ETHIF_TRCV_STATE_ACTIVE:
            {
                /* Increment the ActiveCntr */
                lDynamicTrcv_pst->ActiveCntr_u8++;

                /* Return with E_OK */
                lStdRetVal_en = E_OK;
            }
            break;

            /* Trcv mode in the context of EthIf is DOWN */
            case ETHIF_TRCV_STATE_DOWN:
            {
                /* Call EthTrcv API */
                lStdRetVal_en = EthIf_Prv_SetTrcvActive( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8 );

                if( lStdRetVal_en == E_OK )
                {
                    /* Update the Trcv state to WAIT_ACTIVE and increment ActiveCntr */
                    lDynamicTrcv_pst->TrcvState_en = ETHIF_TRCV_STATE_WAIT_ACTIVE;

                    /* Increment the ActiveCntr */
                    lDynamicTrcv_pst->ActiveCntr_u8++;

                    /* E_OK will be returned */
                }
            }
            break;

            /* Else case if when EthTrcv mode in context of EthIf is WAIT_DOWN. E_NOT_OK has to be returned */
            /* Mode cannot be changed during ongoing transition */
            /* Else case when EthTrcv mode in context of EthIf is UNINIT. E_NOT_OK has to be returned. */
            default:
            {
                /* E_NOT_OK will be returned */
            }
            break;
        }
    }

    return lStdRetVal_en;
}
#endif


/**
 *********************************************************************************************************************
 * \module description
 * Calls API EthTrcv_TransceiverLinkStateRequest() if the previous call has failed
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of the EthIfController for which EthIf_SetControllerMode(ETH_MODE_ACTIVE) is triggered
 *
 * \return          None
 *
 *********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
void EthIf_Prv_RetryTransceiverLinkStateActive( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                              lStdRetVal_en;
    EthIf_DynamicTrcv_tst*                      lDynamicTrcv_pst;

    /* Fetch the PhysTrcv structure from EthIfCtrl index */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the transceiver is not yet requested for LinkState ACTIVE */
    if( lDynamicTrcv_pst->TrcvLinkStateActiveReq_b == FALSE )
    {
        /* Call EthTrcv_TransceiverLinkStateRequest(ACTIVE) */
        lStdRetVal_en = EthTrcv_TransceiverLinkStateRequest( lDynamicTrcv_pst->EthTrcvIdx_cu8,
                                                             ETHTRCV_LINK_STATE_ACTIVE );

        if( lStdRetVal_en == E_OK )
        {
            /* Transceiver Link State has been successfully requested to ACTIVE, no need to retry in next MainFunction */
            lDynamicTrcv_pst->TrcvLinkStateActiveReq_b = TRUE;
        }
    }

    return;
}
#endif


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/**
 *********************************************************************************************************************
 * \module description
 * Calls Trcv APIs to set TrcvLink ACTIVE and TrcvMode ACTIVE.
 *
 * Parameter In:
 * \param EthIfTrcvIdx_u8  Index of the EthIfTransceiver for which EthIf_SetControllerMode(ETH_MODE_ACTIVE) is triggered
 *
 * \return          None
 *
 *********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static Std_ReturnType EthIf_Prv_SetTrcvActive( uint8 EthIfTrcvIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                              lStdRetVal_en;

    /* Set the transceiver to mode ACTIVE */
    lStdRetVal_en = EthTrcv_SetTransceiverMode( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].EthTrcvIdx_cu8,
                                                ETHTRCV_MODE_ACTIVE );

    if( lStdRetVal_en == E_OK )
    {
        /* If the transceiver has been successfully requested to be active, call EthTrcv_TransceiverLinkStateRequest(ACTIVE) */
        lStdRetVal_en = EthTrcv_TransceiverLinkStateRequest( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].EthTrcvIdx_cu8,
                                                             ETHTRCV_LINK_STATE_ACTIVE );

        if( lStdRetVal_en == E_OK )
        {
            /* Transceiver Link State has been successfully requested to ACTIVE, no need to retry in next MainFunction */
            EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].TrcvLinkStateActiveReq_b = TRUE;
        }

        /* Forcefully set the return value to E_OK when SetTransceiverMode is a successful. */
        /* It will be retried in further EthIf_MainFunctionState() to bring LinkState to ACTIVE if it is not possible now */
        lStdRetVal_en = E_OK;
    }

    return lStdRetVal_en;
}
#endif


/**
 *********************************************************************************************************************
 * \module description
 * Calls Trcv APIs to set TrcvLink DOWN and TrcvMode DOWN.
 *
 * Parameter In:
 * \param EthIfTrcvIdx_u8    Index of the EthIfTransceiver for which EthIf_SetControllerMode(ETH_MODE_DOWN) is triggered
 *
 * \return          None
 *
 *********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static Std_ReturnType EthIf_Prv_SetTrcvDown( uint8 EthIfTrcvIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                           lStdRetVal_en;

    /* Initialize return value to E_NOT_OK */
    lStdRetVal_en = E_NOT_OK;

    /* If the transceiver link state is still requested for ACTIVE */
    if( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].TrcvLinkStateActiveReq_b != FALSE )
    {
        /* Call EthTrcv_TransceiverLinkStateRequest(DOWN) first */
        lStdRetVal_en = EthTrcv_TransceiverLinkStateRequest( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].EthTrcvIdx_cu8,
                                                             ETHTRCV_LINK_STATE_DOWN );
    }
    else
    {
        /* Trcv link state has already been requested to DOWN previously */
        lStdRetVal_en = E_OK;
    }

    /* Link State requested to DOWN */
    if( lStdRetVal_en == E_OK )
    {
        /* Transceiver link State has been successfully requested to DOWN, no need to retry in next MainFunction */
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].TrcvLinkStateActiveReq_b = FALSE;

        /* Set the transceiver to mode DOWN */
        lStdRetVal_en = EthTrcv_SetTransceiverMode( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIfTrcvIdx_u8].EthTrcvIdx_cu8,
                                                    ETHTRCV_MODE_DOWN );
    }

    return lStdRetVal_en;
}
#endif


#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
