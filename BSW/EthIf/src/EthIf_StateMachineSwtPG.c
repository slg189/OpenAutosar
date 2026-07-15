

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

#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_Down          ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitCtrlActive( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGControl_Transfrm_State_Active        ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );


static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvDown  ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGControl_ReqDownHandling              ( uint8 EthIfCtrlIdx_u8 );
#endif

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Down         ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                   Eth_ModeType CtrlReqMode_en );

static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Active       ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );


static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvDown ( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en );

static boolean EthIf_Prv_SwtPGLinkInfo_ActiveStatus             ( uint8 EthIfCtrlIdx_u8 );
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
 * Called to trigger all possible state transitions for EthIfCtrl refering a Switch PG
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 *********************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
void EthIf_Prv_SwtPG_StateTransitions( uint8 EthIfCtrlIdx_u8,
                                       Eth_ModeType CtrlReqMode_en  )
{
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
    /* If the EthIfCtrl references a Switch PG of type "control" */
    if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL )
    {
        switch( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en )
        {
            case ETHIF_ETHIFCTRL_STATE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_DOWN */
                EthIf_Prv_SwtPGControl_Transfrm_State_Down( EthIfCtrlIdx_u8,
                                                            CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE */
                EthIf_Prv_SwtPGControl_Transfrm_State_WaitCtrlActive( EthIfCtrlIdx_u8,
                                                                      CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
                EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvActive( EthIfCtrlIdx_u8,
                                                                      CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_ACTIVE */
                EthIf_Prv_SwtPGControl_Transfrm_State_Active( EthIfCtrlIdx_u8,
                                                              CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
                EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvDown( EthIfCtrlIdx_u8,
                                                                    CtrlReqMode_en );
            }
            break;

            default:
                /* Do Nothing */
            break;
        }
    }
#endif

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* If the EthIfCtrl references a Switch PG of type "link-info" */
    if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO )
    {
        switch( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en )
        {
            case ETHIF_ETHIFCTRL_STATE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_DOWN */
                EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Down( EthIfCtrlIdx_u8,
                                                             CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
                EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvActive( EthIfCtrlIdx_u8,
                                                                       CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_ACTIVE:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_ACTIVE */
                EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Active( EthIfCtrlIdx_u8,
                                                               CtrlReqMode_en );
            }
            break;

            case ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN:
            {
                /* Transition from ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
                EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvDown( EthIfCtrlIdx_u8,
                                                                     CtrlReqMode_en );
            }
            break;

            default:
                /* Do Nothing */
            break;
        }
    }
#endif

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
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
                                                        Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;

    /* If requested EthIf controller mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
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
 * Called by EthIf_Prv_SwtPGControl_Transfrm_State_WaitCtrlActive(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitCtrlActive( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If requested EthIf controller mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the EthIfSwitchPortGroup to active */
        lStdRetVal_en =  EthIf_Prv_SetSwitchOrPGModeActive( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                            lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                            lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                            lStaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If request to set the ports of the PG to active is accepted. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
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
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                  Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    boolean                                 lSwtPGActiveStatus_b;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the requested EthIfCtrl mode is ACTIVE. */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Check if atleast the host port and one standard port is successfully activated. */
        lSwtPGActiveStatus_b =  EthIf_Prv_CheckSwitchOrPGActiveStatus( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                       lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                                       lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                                       lStaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If PG activation was successful. */
        if( lSwtPGActiveStatus_b != FALSE )
        {
            /* Change the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_ACTIVE. */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_ACTIVE;

            /* Indicate the controller mode to EthSM. */
            EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                      ETH_MODE_ACTIVE );

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
            /* Set EthIf_SwitchPortLinkStateChanged_b to TRUE, so that the linkstate accumulation state machine is executed, */
            /* even if there is no Linkstate change notification from EthSwt. */
            EthIf_SwitchPortLinkStateChanged_b = TRUE;
#endif
        }
    }

    /* Requested controller mode is DOWN */
    else
    {
        /* Trigger API to handle state transitons when requested EthIfCtrl mode is DOWN. */
        EthIf_Prv_SwtPGControl_ReqDownHandling( EthIfCtrlIdx_u8 );
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                          Eth_ModeType CtrlReqMode_en )
{
    /* If mode down has been requested */
    if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Trigger API to handle state transitons when requested EthIfCtrl mode is DOWN. */
        EthIf_Prv_SwtPGControl_ReqDownHandling( EthIfCtrlIdx_u8 );
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions from state ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8,
                                                                Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables. */
    Std_ReturnType                          lStdRetVal_en;
    boolean                                 lSwtPGDownStatus_b;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the requested EthIfCtrl mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the EthIfSwitchPortGroup to active */
        lStdRetVal_en =  EthIf_Prv_SetSwitchOrPGModeActive( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                            lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                            lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                            lStaticSwitchPG_pcst->NbStdPorts_cu8 );

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
    {
        /* Check if all the ports which were requested to down state, are successfully switched off. */
        lSwtPGDownStatus_b = EthIf_Prv_CheckSwitchOrPGDownStatus( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                  lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                                  lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                                  lStaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If all the requested ports are down. */
        if( lSwtPGDownStatus_b != FALSE )
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
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return  boolean {TRUE: activated; FALSE: not activated}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
static void EthIf_Prv_SwtPGControl_ReqDownHandling( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    boolean                                 lMultipleActiveCnt_b;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Checks whether all the ports of the EthIfSwitchPortGroup are having a active count greater than one */
    lMultipleActiveCnt_b = EthIf_Prv_CheckSwitchOrPGMultActiveCnt( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                   lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                                   lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                                   lStaticSwitchPG_pcst->NbStdPorts_cu8 );

    /* Decrement the active count of the host port, uplink ports and the standard ports of the PG. */
    lStdRetVal_en =  EthIf_Prv_SetSwitchOrPGModeDown( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                      lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                      lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                      lStaticSwitchPG_pcst->NbStdPorts_cu8 );

    /* Check whether setting of PG to DOWN was successful. */
    if( lStdRetVal_en == E_OK )
    {
        /* If all the ports have active count greater than one, reduce EthCtrl active count by one. */
        if( lMultipleActiveCnt_b != FALSE )
        {
            /* Decrement ActiveCntr for the EthIfCtrl */
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

        /*  If atleast one port has active count less than or equal to one. */
        else
        {
            /* Update the EthIfCtrl state to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions for ETHIF_ETHIFCTRL_STATE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8       Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en        Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Down( uint8 EthIfCtrlIdx_u8,
                                                         Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    Std_ReturnType       lStdRetVal_en;

    /* If the requested controller mode is ACTIVE */
    if( CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Initialize the physical controller */
        lStdRetVal_en = EthIf_Prv_ControllerInit( EthIfCtrlIdx_u8 );

        /* If controller initialization was successful */
        if( lStdRetVal_en == E_OK )
        {
            /* Call Switch init API */
            /* Switch init not to be called for Leo Switch */
            lStdRetVal_en = EthIf_Prv_SwitchInit( EthIfCtrlIdx_u8 );
        }

        /* If controller initialization was successful */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the controller mode to ACTIVE and increment ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_ACTIVE );

            if( lStdRetVal_en == E_OK )
            {
                /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE. */
                /* The state is changed to ETHIF_ETHIFCTRL_STATE_ACTIVE when ports of the EthIfSwitchPortGroup is set to active by BswM. */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE;
            }
        }
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions for ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvActive( uint8 EthIfCtrlIdx_u8,
                                                                   Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    boolean                                 lActiveFlag_b;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Check whether atleast host port and one non-host port is active. */
    lActiveFlag_b = EthIf_Prv_CheckSwitchOrPGActiveStatus( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                           lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                           lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                           lStaticSwitchPG_pcst->NbStdPorts_cu8 );

    /* If the required ports are activated. */
    if( lActiveFlag_b != FALSE )
    {
        /* Indicate the EthIf Ctrl mode as ACTIVE to EthSM */
        EthSM_CtrlModeIndication( EthIfCtrlIdx_u8,
                                  ETH_MODE_ACTIVE );

        /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_ACTIVE */
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_ACTIVE;

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
        /* Set EthIf_SwitchPortLinkStateChanged_b to TRUE, so that the linkstate accumulation state machine is executed, */
        /* even if there is no Linkstate change notification from EthSwt. */
        EthIf_SwitchPortLinkStateChanged_b = TRUE;
#endif
    }

    /* If the requested controller mode is DOWN */
    else if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Check whether an active EthIfCtrl referring a link-info PG is present, mapped to the same EthCtrl. */
        lActiveFlag_b = EthIf_Prv_SwtPGLinkInfo_ActiveStatus( EthIfCtrlIdx_u8 );

        /* If an active EthIfCtrl is present, then set the EthIfCtrl mode and state to down. */
        if( lActiveFlag_b != FALSE )
        {
            /* Set the controller to DOWN and decrement ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_DOWN );

            if( lStdRetVal_en == E_OK )
            {
                /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_DOWN */
                EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;
            }
        }
        else
        {
            /* Set the state of the EthIfCtrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
    }
    else
    {
        /* Do Nothing */
    }

    return;
}
#endif


/**
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions for ETHIF_ETHIFCTRL_STATE_ACTIVE
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_Active( uint8 EthIfCtrlIdx_u8,
                                                           Eth_ModeType CtrlReqMode_en  )
{
    /* Declare local variables */
    boolean                                 lActiveFlag_b;
    Std_ReturnType                          lStdRetVal_en;

    /* If the requested controller mode is DOWN */
    if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Check whether an active EthIfCtrl referring to a link info PG and mapped to the same EthCtrl is present. */
        lActiveFlag_b = EthIf_Prv_SwtPGLinkInfo_ActiveStatus( EthIfCtrlIdx_u8 );

        /* If there is no active EthCtrl present, then set the state to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN. */
        if( lActiveFlag_b == FALSE )
        {
            /* Set the SwitchPortGroup state to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN;
        }
        /* If there are other active EthIfCtrls present, then set the EthIfCtrl mode and state to DOWN. */
        else
        {
            /* Here only ActiveCntr gets decremented and EthCtrl will not be set to DOWN */
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
 ************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_SwtPG_StateTransitions(), to trigger all possible state transitions for ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8           Index of EthIfCtrl for which state transitions needs to be triggered
 * \param CtrlReqMode_en            Ctrl Mode requested by EthIf for the current EthIfCtrl
 *
 * \return  None
 *
 ***************************************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPGLinkInfo_Transfrm_State_WaitTrcvDown( uint8 EthIfCtrlIdx_u8,
                                                                 Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    boolean                                 lAnotherActLinkInfoCtrlExist_b;
    uint8                                   lEthIfCurrentPortIdx_u8;
    uint8                                   lEthIfSwitchIndex_u8;
    uint8                                   lLoopIdx_u8;
    boolean                                 lAllStandaloneSwtPortGrpDown_b;
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure from EthIfSwitchPG index */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* If the requested controller mode is ACTIVE */
    if(CtrlReqMode_en == ETH_MODE_ACTIVE )
    {
        /* Set the state of the EthIf Ctrl to ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE */
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

    /* If the requested controller mode is DOWN */
    else
    {
        /* Initialize flag to TRUE */
        lAllStandaloneSwtPortGrpDown_b = TRUE;

        /* Loop through all standalone switchPG and check if any one related to same EthCtrl is ACTIVE (part of same cascading branch) */
        /* It must always be ensured that at least one link-info PG is active while standalone PG are used in order to fulfill requirement [SWS_EthIf_00256] */
        for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPortGroup_cu8; lLoopIdx_u8++)
        {
            if( ( EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].Semantic_cen == ETHIF_SWTPORTGROUP_UNREFERENCED ) &&
                ( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[0] == EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefHostUplinkPortsTablePtr_cpacu8[0] ) &&
                ( EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefStandaloneSwtPortGroupTableEntry_cpst->Mode_en != ETHIF_STANDALONE_PG_MODE_DOWN ) )
            {
                lAllStandaloneSwtPortGrpDown_b = FALSE;
                break;
            }
        }

        /* Check if there is any active EthIfCtrl referring to a link-info PG. */
        lAnotherActLinkInfoCtrlExist_b = EthIf_Prv_SwtPGLinkInfo_ActiveStatus( EthIfCtrlIdx_u8 );

        /* Set the EthIf controller to down if */
        /* If all standalone switchPG are down or */
        /* If other active EthIfCtrl referring to a link info PG and the same EthCtrl is present. */
        if( (lAllStandaloneSwtPortGrpDown_b != FALSE) || (lAnotherActLinkInfoCtrlExist_b != FALSE) )
        {
            /* Set the controller mode to DOWN and decrement ActiveCntr */
            lStdRetVal_en = EthIf_Prv_SetControllerMode( EthIfCtrlIdx_u8,
                                                         ETH_MODE_DOWN  );

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
 *****************************************************************************************************************
 * \module description
 * API to check if any EthIfCtrl (referring a link-info PG) other than the input parameter EthIfCtrlIdx_u8 is active.
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * \return  Boolean {TRUE: Other EthIfCtrl/s are active; FALSE: All EthIfCtrls are down }
 *
 *****************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static boolean EthIf_Prv_SwtPGLinkInfo_ActiveStatus( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    uint8             lEthIfEthCtrlIdx_u8;
    uint8             lLoopEthIfCtrlIdx_u8;
    boolean           lResult_b;

    /* Initialize result to FALSE */
    lResult_b = FALSE;

    /* Fetch the EthIf EthCtrl index from EthIfCtrl index */
    lEthIfEthCtrlIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefPhysCtrlTableIdx_cu8;

    /* If any of the EthIfCtrl referencing a link info PG (mapped to same EthCtrl) except the requested one is active (is in state different from DOWN) */
    /* call shall not be forwarded to driver API */
    for( lLoopEthIfCtrlIdx_u8 = 0; lLoopEthIfCtrlIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lLoopEthIfCtrlIdx_u8++ )
    {
        if( (lEthIfEthCtrlIdx_u8 == EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lLoopEthIfCtrlIdx_u8].RefPhysCtrlTableIdx_cu8)  &&  /* If any of the EthIfCtrl (mapped to same EthCtrl) */
            (EthIfCtrlIdx_u8 != lLoopEthIfCtrlIdx_u8)                                                                                  &&  /* except the requested one */
            (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lLoopEthIfCtrlIdx_u8].State_en != ETHIF_ETHIFCTRL_STATE_DOWN)          &&  /* is not in DOWN state */
            (ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO == EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lLoopEthIfCtrlIdx_u8].ReferenceType_cen) )    /* and refers a link-info PG. */
        {
            lResult_b = TRUE;
            break;
        }
    }

    return lResult_b;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
