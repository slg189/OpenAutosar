

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
 * Private functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ******************************************************************************************************************************
 * \module description
 * Called to trigger all possible state transitions for EthIfCtrl having no reference
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered.
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl.
 *
 * \return  None
 *
 ******************************************************************************************************************************
 */
void EthIf_Prv_NoRef_StateTransitions( uint8 EthIfCtrlIdx_u8,
                                       Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType   lStdRetVal_en;

    /* Initialize local variable */
    lStdRetVal_en = E_NOT_OK;

    /* If requested control mode is active */
    if( (CtrlReqMode_en == ETH_MODE_ACTIVE) &&
        (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en != ETHIF_ETHIFCTRL_STATE_ACTIVE) )
    {
        /* Request physical controller for initialization */
        lStdRetVal_en = EthIf_Prv_ControllerInit( EthIfCtrlIdx_u8 );

        if( lStdRetVal_en == E_OK )
        {
            /* Request the controller mode to ACTIVE and increment ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_ACTIVE );
        }

        if( lStdRetVal_en == E_OK )
        {
            /* Indicate the controller mode to EthSM */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_ACTIVE );

            /* Indicate the link status as active to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8,
                                    ETHTRCV_LINK_STATE_ACTIVE );

            /* Set the EthIfCtrl state to ETHIF_ETHIFCTRL_STATE_ACTIVE. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_ACTIVE;

            /* Set the link state to ETHIF_LINKSTATE_ACTIVE. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_ACTIVE;

        }
    }

    /* If the requested controller mode is down */
    else if( (CtrlReqMode_en == ETH_MODE_DOWN) &&
             (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en != ETHIF_ETHIFCTRL_STATE_DOWN) )
    {
        /* Set the controller mode to DOWN and decrement ActiveCntr */
        lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                     ETH_MODE_DOWN );

        if( lStdRetVal_en == E_OK )
        {
            /* If setting of mode down is successful, indicate mode to EthSM */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_DOWN );

            /* Indicate the link state as down to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8,
                                    ETHTRCV_LINK_STATE_DOWN );

            /* Set the EthIfCtrl state to ETHIF_ETHIFCTRL_STATE_DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;

            /* Set the link state to ETHIF_LINKSTATE_DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
    }
    else
    {
        /* Do Nothing */
    }

    return;
}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
