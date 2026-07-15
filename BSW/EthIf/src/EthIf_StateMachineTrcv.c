

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthSM.h"
#include "EthSM_Cbk.h"

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_Transfrm_State_Down              ( uint8 EthIfCtrlIdx_u8,
                                                              Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_Trcv_Transfrm_State_WaitTrcvActive    ( uint8 EthIfCtrlIdx_u8 );

static void EthIf_Prv_Trcv_Transfrm_State_Active            ( uint8 EthIfCtrlIdx_u8,
                                                              Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_Trcv_Transfrm_State_WaitTrcvDown      ( uint8 EthIfCtrlIdx_u8 );
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 *********************************************************************************************************************************
 * \module description
 * Called to trigger all possible state transitions for EthIfCtrl refering a Transceiver
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en    Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 *********************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
void EthIf_Prv_Trcv_StateTransitions( uint8 EthIfCtrlIdx_u8,
                                      Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    EthIf_EthIfCtrlState_ten    lPreviousEthIfCtrlState_en;
    uint8                       lNbTransitions_u8;

    /* Initialize the previous EthIfCtrl state */
    lPreviousEthIfCtrlState_en = EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en;
    lNbTransitions_u8 = 0U;

    /* Try to execute transition until it is not possible anymore */
    do
    {
        /* Save the EthIfCtrl state as previous state */
        lPreviousEthIfCtrlState_en = EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en;

        switch( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en )
        {
            case ETHIF_ETHIFCTRL_STATE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_DOWN */
                EthIf_Prv_Trcv_Transfrm_State_Down( EthIfCtrlIdx_u8,
                                                    CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
                EthIf_Prv_Trcv_Transfrm_State_WaitTrcvActive( EthIfCtrlIdx_u8 );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_ACTIVE */
                EthIf_Prv_Trcv_Transfrm_State_Active( EthIfCtrlIdx_u8,
                                                      CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
                EthIf_Prv_Trcv_Transfrm_State_WaitTrcvDown( EthIfCtrlIdx_u8 );
            }
            break;

            default:
                /* Do Nothing */
            break;
        }

        /* Increment the number of executed transitions */
        lNbTransitions_u8++;

    }while( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en != lPreviousEthIfCtrlState_en) &&
            (lNbTransitions_u8 < 3U) ); /* The total number of consecutive transitions is limited to 3 as it is not possible to move more than 3 iteration without a waiting phase from HW */

    return;
}
#endif


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/**
 ***************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Trcv_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_DOWN.
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
                                                Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* If the requested controller mode is Active and transceiver error is not set */
    if( (CtrlReqMode_en == ETH_MODE_ACTIVE) &&
        (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].ErrorFlg_en == ETHIF_ETHIFCTRL_ERROR_NONE) )
    {
        /* Initialize the physical controller referred by the EthIf Ctrl */
        lStdRetVal_en = EthIf_Prv_ControllerInit( EthIfCtrlIdx_u8 );

        if( lStdRetVal_en == E_OK )
        {
            /* Initialize EthIfTrcv referred by the EthIf Ctrl. */
            lStdRetVal_en = EthIf_Prv_TransceiverInit( EthIfCtrlIdx_u8 );
        }

        /* After initialization is successful */
        if( lStdRetVal_en == E_OK )
        {
            /* Make the Trcv up */
            /* In case the actual state of the Trcv is already WAIT_ACTIVE, this call is used only to increment the ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetTransceiverMode( EthIfCtrlIdx_u8,
                                                          ETHTRCV_MODE_ACTIVE );

            /* Update EthIfCtrl state to ETHIF_TRCV_STATE_WAIT_TRCV_ACTIVE */
            if( lStdRetVal_en == E_OK )
            {
                /* Move to next state */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE;
            }
        }
    }

    return;
}
#endif


/**
 *********************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Trcv_StateTransitions(), to trigger all possible state transitions from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE.
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;
    EthIf_DynamicTrcv_tst*      lDynamicTrcv_pst;

    /* Fetch the Dynamic Transceiver structure from EthIfTrcv index */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the state of the transceiver is not WAIT_ACTIVE or ACTIVE, then an unexpected error occured */
    /* (EthIf_TrcvModeIndication(DOWN) was received while waiting for the activation of the transceiver) */
    if( (lDynamicTrcv_pst->TrcvState_en != ETHIF_TRCV_STATE_WAIT_ACTIVE) &&
        (lDynamicTrcv_pst->TrcvState_en != ETHIF_TRCV_STATE_ACTIVE) )
    {
        /* Make the Trcv down and decrement the ActiveCntr */
        /* In case other EthIfCtrl are using the Trcv, this call will only decrement the ActiveCntr */
        lStdRetVal_en = EthIf_Prv_SetTransceiverMode( EthIfCtrlIdx_u8,
                                                      ETHTRCV_MODE_DOWN );

        if( lStdRetVal_en == E_OK )
        {
            /*If yes, set the transceiver error flag to true for the corresponding EthIfCtrlIdx */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].ErrorFlg_en = ETHIF_ETHIFCTRL_ERROR_TRCV_WAIT_ACTIVE;

            /* Move to next state */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }

    /* State of the referenced EthTrcv in the context of EthIf is ACTIVE */
    /* Callback EthIf_TrcvModeIndication(ACTIVE) has been received indicating that the transceiver has changed state */
    else if( lDynamicTrcv_pst->TrcvState_en == ETHIF_TRCV_STATE_ACTIVE )
    {
        /* Set the controller mode to ACTIVE and increment ActiveCntr */
        lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                     ETH_MODE_ACTIVE );

        if( lStdRetVal_en == E_OK )
        {
            /* Indicate that the hardware mode is active to EthSM */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_ACTIVE );

            /* Update EthIf Ctrl state to ETHIF_ETHIFCTRL_STATE_ACTIVE */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_ACTIVE;
        }
    }

    /* In other cases */
    else
    {
        /* State of the referenced Trcv in not yet ACTIVE */
        /* Check LinkState ACTIVE request was successfully performed, if not try to do it again */
        EthIf_Prv_RetryTransceiverLinkStateActive( EthIfCtrlIdx_u8 );
    }

    return;
}
#endif


/**
 *********************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Trcv_StateTransitions(), to trigger all possible state transitions for EthIfCtrl referring a transceiver.
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                  Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;
    EthIf_DynamicTrcv_tst*      lDynamicTrcv_pst;

    /* Fetch the Dynamic Transceiver structure from EthIfTrcv index */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the state of the transceiver is not ACTIVE, then an unexpected error occured */
    /* (EthIf_TrcvModeIndication(DOWN) was received while transceiver was already active.) */
    if( lDynamicTrcv_pst->TrcvState_en != ETHIF_TRCV_STATE_ACTIVE )
    {
        /* Make the Trcv down and decrement the ActiveCntr */
        /* In case other EthIfCtrl are using the Trcv, this call will only decrement the ActiveCntr */
        lStdRetVal_en = EthIf_Prv_SetTransceiverMode( EthIfCtrlIdx_u8,
                                                      ETHTRCV_MODE_DOWN );

        if( lStdRetVal_en == E_OK )
        {
            /*If yes, set the transceiver error flag to true for the corresponding EthIfCtrlIdx */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].ErrorFlg_en = ETHIF_ETHIFCTRL_ERROR_TRCV_ACTIVE;

            /* Move to next state */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }
    /* If requested EthIf controller mode is down and there is no Trcv error */
    else if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Make the Trcv down and decrement the ActiveCntr */
        lStdRetVal_en = EthIf_Prv_SetTransceiverMode( EthIfCtrlIdx_u8,
                                                      ETHTRCV_MODE_DOWN );

        if( lStdRetVal_en == E_OK )
        {
            /* Move to next state */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }
    /* In other cases */
    else
    {
        /* State is ACTIVE and requested state is also ACTIVE */
        /* Check LinkState ACTIVE request was successfully performed, if not try to do it again */
        EthIf_Prv_RetryTransceiverLinkStateActive( EthIfCtrlIdx_u8 );
    }

    return;
}
#endif


/**
 *************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Trcv_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;
    EthIf_DynamicTrcv_tst*      lDynamicTrcv_pst;

    /* Fetch the Dynamic Transceiver structure from EthIfTrcv index */
    lDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* State of the referenced EthTrcv in the context of EthIf is DOWN or UNINIT */
    /* Or the transceiver is still in use by other EthIfCtrl */
    if( (lDynamicTrcv_pst->TrcvState_en <= ETHIF_TRCV_STATE_DOWN) || (lDynamicTrcv_pst->ActiveCntr_u8 > 0U) )
    {
        /* Initialize return value to E_OK */
        lStdRetVal_en = E_OK;

        /* If the state transition occured from ACTIVE to WAIT_TRCV_DOWN, then set the EthCtrl mode to DOWN. */
        /* Hint: In case of error transition from WAIT_ACTIVE to WAIT_DOWN, the EthCtrl was not activated, therefore it shall not be deactivated here. */
        if (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].ErrorFlg_en != ETHIF_ETHIFCTRL_ERROR_TRCV_WAIT_ACTIVE)
        {
            /* Set the controller mode to DOWN (if not in use by other EthIfCtrl) and decrement ActiveCntr. */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_DOWN );
        }

        if( lStdRetVal_en == E_OK )
        {
            /* Indicate that the hardware mode is down to EthSM */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_DOWN  );

            /* Update EthIf Ctrl state to ETHIF_ETHIFCTRL_STATE_DOWN */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;

            /* If the transceiver is now disabled */
            if( lDynamicTrcv_pst->TrcvState_en == ETHIF_TRCV_STATE_DOWN )
            {
                /* Reset transceiver initialization flag */
                /* Trcv will be reinitialized in next state machine cycle */
                lDynamicTrcv_pst->TrcvState_en = ETHIF_TRCV_STATE_UNINIT;
            }

            /* If the link state of the EthIfCtrl is still active */
            if( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_ACTIVE )
            {
                /* Give indication to EthSM that the virtual link state of the Trcv is also DOWN */
                EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8,
                                        ETHTRCV_LINK_STATE_DOWN );

                /* Update EthIfCtrl link state to ETHIF_LINKSTATE_DOWN */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
            }
        }
    }

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
