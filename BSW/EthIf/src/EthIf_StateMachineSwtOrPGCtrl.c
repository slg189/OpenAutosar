

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

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
                                                       Eth_ModeType CtrlReqMode_en  );

static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitCtrlActive( uint8 EthIfCtrlIdx_u8,
                                                                 Eth_ModeType CtrlReqMode_en,
                                                                 const uint8 * HostUplinkPortTable_pcau8,
                                                                 const uint8 * StdPortTable_pcau8,
                                                                 uint8 SizeHostUplinkPortTable_u8,
                                                                 uint8 SizeStdPortTable_u8 );

static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                 Eth_ModeType CtrlReqMode_en,
                                                                 const uint8 * HostUplinkPortTable_pcau8,
                                                                 const uint8 * StdPortTable_pcau8,
                                                                 uint8 SizeHostUplinkPortTable_u8,
                                                                 uint8 SizeStdPortTable_u8 );

static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                         Eth_ModeType CtrlReqMode_en,
                                                         const uint8 * HostUplinkPortTable_pcau8,
                                                         const uint8 * StdPortTable_pcau8,
                                                         uint8 SizeHostUplinkPortTable_u8,
                                                         uint8 SizeStdPortTable_u8 );

static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8,
                                                               const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8 );
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
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
void EthIf_Prv_SwtOrPGCtrl_StateTransitions( uint8 EthIfCtrlIdx_u8,
                                             Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    EthIf_EthIfCtrlState_ten            lPreviousEthIfCtrlState_en;
    uint8                               lNbTransitions_u8;
    const uint8*                        lRefHostUplinkPortsTablePtr_pcu8;
    uint8                               lNbHostUplinkPorts_u8;
    const uint8*                        lRefStdPortsTablePtr_pcu8;
    uint8                               lNbStdPorts_u8;

    /* Initialise local variables */
    lRefHostUplinkPortsTablePtr_pcu8 = NULL_PTR;
    lRefStdPortsTablePtr_pcu8        = NULL_PTR;
    lNbHostUplinkPorts_u8            = 0U;
    lNbStdPorts_u8                   = 0U;

    /* If the EthIfCtrl references a Switch */
    if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH )
    {
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
        /* Fetch the Static Switch structure from EthIfSwitchPG index */
        const EthIf_StaticSwitch_tst * lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

        /* Fetch ports configuration for the complete switch */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8            = lStaticSwitch_pcst->NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                   = lStaticSwitch_pcst->NbStdPorts_cu8;
#endif
    }
    /* If the EthIfCtrl references a Switch PG of type "control" */
    else if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL )
    {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
        /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
        const EthIf_StaticSwtPortGroup_tst * lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

        /* Fetch ports configuration for the switch PG */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8            = lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                   = lStaticSwitchPG_pcst->NbStdPorts_cu8;
#endif
    }
    else
    {
        /* Reference type is not valid for this state machine */
    }

    /* If the reference type is valid */
    if( (lRefHostUplinkPortsTablePtr_pcu8 != NULL_PTR)  &&
        (lRefStdPortsTablePtr_pcu8 != NULL_PTR) )
    {
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
                    EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Down( EthIfCtrlIdx_u8,
                                                               CtrlReqMode_en );
                }
                break;

                case ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE:
                {
                    /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE */
                    EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitCtrlActive( EthIfCtrlIdx_u8,
                                                                         CtrlReqMode_en,
                                                                         lRefHostUplinkPortsTablePtr_pcu8,
                                                                         lRefStdPortsTablePtr_pcu8,
                                                                         lNbHostUplinkPorts_u8,
                                                                         lNbStdPorts_u8 );
                }
                break;

                case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE:
                {
                    /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
                    EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvActive( EthIfCtrlIdx_u8,
                                                                         CtrlReqMode_en,
                                                                         lRefHostUplinkPortsTablePtr_pcu8,
                                                                         lRefStdPortsTablePtr_pcu8,
                                                                         lNbHostUplinkPorts_u8,
                                                                         lNbStdPorts_u8 );
                }
                break;

                case ETHIF_ETHIFCTRL_STATE_ACTIVE:
                {
                    /* Transition from ETHIF_ETHIFCTRL_STATE_ACTIVE */
                    EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Active( EthIfCtrlIdx_u8,
                                                                 CtrlReqMode_en,
                                                                 lRefHostUplinkPortsTablePtr_pcu8,
                                                                 lRefStdPortsTablePtr_pcu8,
                                                                 lNbHostUplinkPorts_u8,
                                                                 lNbStdPorts_u8 );
                }
                break;

                case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN:
                {
                    /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
                    EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvDown( EthIfCtrlIdx_u8,
                                                                       lRefHostUplinkPortsTablePtr_pcu8,
                                                                       lRefStdPortsTablePtr_pcu8,
                                                                       lNbHostUplinkPorts_u8,
                                                                       lNbStdPorts_u8 );
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
 * Called by EthIf_Prv_SwtOrPGCtrl_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
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
 * Called by EthIf_Prv_SwtOrPGCtrl_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitCtrlActive( uint8 EthIfCtrlIdx_u8,
                                                                 Eth_ModeType CtrlReqMode_en,
                                                                 const uint8 * HostUplinkPortTable_pcau8,
                                                                 const uint8 * StdPortTable_pcau8,
                                                                 uint8 SizeHostUplinkPortTable_u8,
                                                                 uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* If the EthIfCtrl is requested for ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the ports of the switch/PG to active */
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeActive( HostUplinkPortTable_pcau8,
                                                           StdPortTable_pcau8,
                                                           SizeHostUplinkPortTable_u8,
                                                           SizeStdPortTable_u8 );

        /* If request to set the ports of the switch/PG to active is accepted. */
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

        /* If request to set the ports of the switch/PG to down is accepted */
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
 * Called by EthIf_Prv_SwtOrPGCtrl_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                 Eth_ModeType CtrlReqMode_en,
                                                                 const uint8 * HostUplinkPortTable_pcau8,
                                                                 const uint8 * StdPortTable_pcau8,
                                                                 uint8 SizeHostUplinkPortTable_u8,
                                                                 uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    boolean                             lSwtActiveStatus_b;
    Std_ReturnType                      lStdRetVal_en;

    /* If the EthIfCtrl is requested for ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Check if atleast the host port and one standard port is successfully activated. */
        lSwtActiveStatus_b = EthIf_Prv_CheckSwitchOrPGActiveStatus( HostUplinkPortTable_pcau8,
                                                                    StdPortTable_pcau8,
                                                                    SizeHostUplinkPortTable_u8,
                                                                    SizeStdPortTable_u8 );

        /* If switch activation was successful */
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
    /* If mode down has been requested */
    else
    {
        /* Set the EthIfSwitch/ EthIfSwitchPG to down. */
        /* If a port has multiple active count, the API will decrement the active count of the port, without triggering EthSwt_SetSwitchPOrtMode(). */
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeDown( HostUplinkPortTable_pcau8,
                                                         StdPortTable_pcau8,
                                                         SizeHostUplinkPortTable_u8,
                                                         SizeStdPortTable_u8 );

        /* Check whether setting of EthIfSwitch to DOWN was successful. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtOrPGCtrl_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                         Eth_ModeType CtrlReqMode_en,
                                                         const uint8 * HostUplinkPortTable_pcau8,
                                                         const uint8 * StdPortTable_pcau8,
                                                         uint8 SizeHostUplinkPortTable_u8,
                                                         uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* If mode down has been requested */
    if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Set the EthIfSwitch/ EthIfSwitchPG to down. */
        /* If a port has multiple active count, the API will decrement the active count of the port, without triggering EthSwt_SetSwitchPOrtMode(). */
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeDown( HostUplinkPortTable_pcau8,
                                                         StdPortTable_pcau8,
                                                         SizeHostUplinkPortTable_u8,
                                                         SizeStdPortTable_u8 );

        /* Check whether setting of EthIfSwitch to DOWN was successful. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtOrPGCtrl_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtOrPGCtrl_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8,
                                                               const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables. */
    Std_ReturnType                      lStdRetVal_en;
    boolean                             lSwtDownStatus_b;

    /* Check if all the ports which were requested to down state, are successfully switched off. */
    lSwtDownStatus_b = EthIf_Prv_CheckSwitchOrPGDownStatus( HostUplinkPortTable_pcau8,
                                                            StdPortTable_pcau8,
                                                            SizeHostUplinkPortTable_u8,
                                                            SizeStdPortTable_u8 );

    /* If all the requested ports are DOWN */
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

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
