

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

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_Down           ( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_Swt_Transfrm_State_WaitCtrlActive ( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_Swt_Transfrm_State_WaitTrcvActive ( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_Swt_Transfrm_State_Active         ( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_Swt_Transfrm_State_WaitTrcvDown   ( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_Swt_ReqDownHandling               ( uint8 EthIfCtrlIdx_u8 );
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
 * Called to trigger all possible state transitions for EthIfCtrl refering a Switch
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en    Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 *********************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
void EthIf_Prv_Swt_StateTransitions( uint8 EthIfCtrlIdx_u8,
                                     Eth_ModeType CtrlReqMode_en )
{
    switch( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en )
    {
        case ETHIF_ETHIFCTRL_STATE_DOWN:
        {
            /* Transition from ETHIF_ETHIFCTRL_STATE_DOWN */
            EthIf_Prv_Swt_Transfrm_State_Down( EthIfCtrlIdx_u8,
                                               CtrlReqMode_en );
        }
        break;

        case ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE:
        {
            /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE */
            EthIf_Prv_Swt_Transfrm_State_WaitCtrlActive( EthIfCtrlIdx_u8,
                                                         CtrlReqMode_en );
        }
        break;

        case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE:
        {
            /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
            EthIf_Prv_Swt_Transfrm_State_WaitTrcvActive( EthIfCtrlIdx_u8,
                                                         CtrlReqMode_en );
        }
        break;

        case ETHIF_ETHIFCTRL_STATE_ACTIVE:
        {
            /* Transition from ETHIF_ETHIFCTRL_STATE_ACTIVE */
            EthIf_Prv_Swt_Transfrm_State_Active( EthIfCtrlIdx_u8,
                                                 CtrlReqMode_en );
        }
        break;

        case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN:
        {
            /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
            EthIf_Prv_Swt_Transfrm_State_WaitTrcvDown( EthIfCtrlIdx_u8,
                                                       CtrlReqMode_en );
        }
        break;

        default:
            /* Do Nothing */
        break;
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
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Swt_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
                                               Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* If the EthIfCtrl is requested for ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Initialize the physical controller mapped to the EthIf Ctrl */
        lStdRetVal_en = EthIf_Prv_ControllerInit( EthIfCtrlIdx_u8 );

        /* If controller initialization was successful */
        if( lStdRetVal_en == E_OK )
        {
            /* Call Switch init API */
            /* Switch init not to be called for Leo Switch */
            lStdRetVal_en = EthIf_Prv_SwitchInit( EthIfCtrlIdx_u8 );
        }

        /* If controller and switch initialization was successful, set the controller mode to active */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the controller mode to ACTIVE */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_ACTIVE );

            /* If request to set the ports of the switch to active is accepted */
            if( lStdRetVal_en == E_OK )
            {
                /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE;
            }
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Swt_Transfrm_State_WaitCtrlActive(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_WaitCtrlActive( uint8 EthIfCtrlIdx_u8,
                                                         Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;
    const EthIf_StaticSwitch_tst*       lStaticSwitch_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the EthIfCtrl is requested for ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the ports of the switch to active */
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeActive( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                           lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                           lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                           lStaticSwitch_pcst->NbStdPorts_cu8 );

        /* If request to set the ports of the switch to active is accepted. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE;
        }
    }

    /* If the EthIfCtrl is requested for DOWN */
    else
    {
        /* Set the controller mode to DOWN */
        lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                     ETH_MODE_DOWN );

        /* If request to set the ports of the switch to down is accepted */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the EthIfCtrl state to DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Swt_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                         Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    boolean                             lSwtActiveStatus_b;
    const EthIf_StaticSwitch_tst*       lStaticSwitch_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the requested EthIfCtrl mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Check if atleast the host port and one standard port is successfully activated. */
        lSwtActiveStatus_b = EthIf_Prv_CheckSwitchOrPGActiveStatus( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                    lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                                    lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                                    lStaticSwitch_pcst->NbStdPorts_cu8 );

        /* If switch activation was successful. */
        if( lSwtActiveStatus_b != FALSE )
        {
            /* Change the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_ACTIVE. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_ACTIVE;

            /* Indicate the controller mode to EthSM */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_ACTIVE );

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
                /* Set EthIf_SwitchPortLinkStateChanged_b to TRUE, so that the linkstate accumulation state machine is executed, */
                /* even if there is no Linkstate interrupt notification from EthSwt. */
                EthIf_SwitchPortLinkStateChanged_b = TRUE;
#endif
        }
    }

    /* Requested controller mode is DOWN */
    else
    {
        /* Trigger API to handle state transitons when requested EthIfCtrl mode is DOWN. */
        EthIf_Prv_Swt_ReqDownHandling( EthIfCtrlIdx_u8 );

    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Swt_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                 Eth_ModeType CtrlReqMode_en  )
{
    /* If mode down has been requested */
    if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Trigger API to handle state transitons when requested EthIfCtrl mode is DOWN. */
        EthIf_Prv_Swt_ReqDownHandling( EthIfCtrlIdx_u8 );
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_Swt_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8,
                                                       Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables. */
    Std_ReturnType                      lStdRetVal_en;
    boolean                             lSwtDownStatus_b;
    const EthIf_StaticSwitch_tst*       lStaticSwitch_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the requested EthIfCtrl mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the EthIfSwitch to active. */
        lStdRetVal_en =  EthIf_Prv_SetSwitchOrPGModeActive( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                            lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                            lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                            lStaticSwitch_pcst->NbStdPorts_cu8 );

        /* If request to set the ports of the PG to active is accepted. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE;

            /* If the link state of the EthIfCtrl is still active */
            /* From WAIT_TRCVMODE_DOWN to WAIT_TRCVMODE_ACTIVE, the Link State has to be reset to DOWN otherwise the Link State stored internally in EthIf might remain ACTIVE */
            /* and EthSM state machine might be blocked in ETHSM_STATE_WAIT_TRCVLINK once EthIfCtrl state machine will be ACTIVE */
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

    /* If the requested EthIfCtrl mode is DOWN */
    else
    {   /* Check if all the ports which were requested to down state, are successfully switched off. */
        lSwtDownStatus_b = EthIf_Prv_CheckSwitchOrPGDownStatus( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                                lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                                lStaticSwitch_pcst->NbStdPorts_cu8 );

        /* If all the requested ports are down. */
        if( lSwtDownStatus_b != FALSE )
        {
            /* Set the controller mode to DOWN and decrement ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_DOWN );

            if( lStdRetVal_en == E_OK )
            {
                /* Indicate the EthIf controller mode to DOWN */
                EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                          ETH_MODE_DOWN );

                /* Set the state of the EthIf Ctrl to DOWN */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;

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
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Execute handling of state transitons when requested EthIfCtrl mode is DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_ReqDownHandling( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;
    boolean                             lMultipleActiveCnt_b;
    const EthIf_StaticSwitch_tst*       lStaticSwitch_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Check if all the ports of the switch have active count greater than one. */
    lMultipleActiveCnt_b = EthIf_Prv_CheckSwitchOrPGMultActiveCnt( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                   lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                                   lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                                   lStaticSwitch_pcst->NbStdPorts_cu8 );

    /* Set the EthIfSwitch to down. */
    /* If a port has multiple active count, the API will decrement the active count of the port, without triggering EthSwt_SetSwitchPOrtMode(). */
    lStdRetVal_en =  EthIf_Prv_SetSwitchOrPGModeDown( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                      lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                                      lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                                      lStaticSwitch_pcst->NbStdPorts_cu8 );

    /* Check whether setting of EthIfSwitch to DOWN was successful. */
    if( lStdRetVal_en == E_OK )
    {
        /* If all the ports have active count greater than one, reduce the active count by one. */
        if( lMultipleActiveCnt_b != FALSE )
        {
            /* Set the controller mode to DOWN and decrement ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_DOWN );

            if( lStdRetVal_en == E_OK )
            {
                /* Indicate the EthIf controller mode as DOWN */
                EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                          ETH_MODE_DOWN );

                /* Set the EthIfCtrl state to DOWN. */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;

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
        else /* Not all ports have active count greater than one, and hence some/all ports can be switched off. */
        {
            /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
