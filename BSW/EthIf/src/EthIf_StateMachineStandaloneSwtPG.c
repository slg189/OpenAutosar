

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "EthSwt.h"

#include "BswM_EthIf.h"
#endif

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Down           ( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );

static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvActive ( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );

static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Startup        ( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );

static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Active         ( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );

static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitDown       ( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );

static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvDown   ( uint8 PortGrpIdx_u8,
                                                                      const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                      EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst );
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
 *****************************************************************************************************************************
 * \module description
 * Called by EthIf_MainFunctionState(), to trigger all possible state transitions for EthIf SwitchPortGroups with no semantics
 *
 * \return  None
 *
 ******************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
void EthIf_Prv_StandaloneSwtPG_StateTransitions( void )
{
    /* Declare local variables */
    uint8                                       lPortGrpIdx_u8;
    const EthIf_StaticSwtPortGroup_tst*         lStaticSwitchPG_pcst;
    EthIf_DynamicStandaloneSwtPortGroup_tst*    lDynamicStandaloneSwtPG_pst;
    uint8                                       lNbTransitions_u8;
    EthIf_StandaloneSwtPortGroupMode_ten        lPreviousStandaloneSwtPGMode_en;

    /* Loop through all configured EthIfSwitchPortGroups */
    for( lPortGrpIdx_u8 = 0U; lPortGrpIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPortGroup_cu8; lPortGrpIdx_u8++ )
    {
        /* Fetch the EthIf Portgroup structure */
        lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lPortGrpIdx_u8];

        /* If the PortGroup is standalone */
        if( lStaticSwitchPG_pcst->Semantic_cen == ETHIF_SWTPORTGROUP_UNREFERENCED )
        {
            /* Fetch the Standalone PG requested mode structure */
            lDynamicStandaloneSwtPG_pst = lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst;

            lNbTransitions_u8 = 0U;

            /* Try to execute transition until it is not possible anymore */
            do
            {
                lPreviousStandaloneSwtPGMode_en = lDynamicStandaloneSwtPG_pst->Mode_en;

                /* Fetch the current state of the standalone EthIfSwitchPortGroup */
                switch( lDynamicStandaloneSwtPG_pst->Mode_en )
                {
                    case ETHIF_STANDALONE_PG_MODE_DOWN:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_DOWN */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_Down( lStaticSwitchPG_pcst,
                                                                       lDynamicStandaloneSwtPG_pst);
                    }
                    break;

                    case ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvActive( lStaticSwitchPG_pcst,
                                                                                 lDynamicStandaloneSwtPG_pst );
                    }
                    break;

                    case ETHIF_STANDALONE_PG_MODE_STARTUP:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_STARTUP */
                        /* The state is reached if the portgroup was triggered by EthIf_StartAllPorts() API */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_Startup( lStaticSwitchPG_pcst,
                                                                          lDynamicStandaloneSwtPG_pst );
                    }
                    break;

                    case ETHIF_STANDALONE_PG_MODE_ACTIVE:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_ACTIVE */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_Active( lStaticSwitchPG_pcst,
                                                                         lDynamicStandaloneSwtPG_pst );
                    }

                    break;

                    case ETHIF_STANDALONE_PG_MODE_WAITDOWN:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_WAITDOWN */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitDown( lStaticSwitchPG_pcst,
                                                                           lDynamicStandaloneSwtPG_pst );
                    }
                    break;

                    case ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN:
                    {
                        /* Transition from ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN */
                        EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvDown( lPortGrpIdx_u8,
                                                                               lStaticSwitchPG_pcst,
                                                                               lDynamicStandaloneSwtPG_pst );
                    }
                    break;

                    default:
                        /* Do Nothing */
                    break;
                }

                /* Increment the number of executed transitions */
                lNbTransitions_u8++;

            }while( (lDynamicStandaloneSwtPG_pst->Mode_en != lPreviousStandaloneSwtPGMode_en) &&
                    (lNbTransitions_u8 < 4U) ); /* The total number of consecutive transitions is limited to 4 as it is not possible to move more than 4 iteration without a waiting phase from HW */
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
 ***************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_DOWN
 *
 * Parameter In:
 * \param StaticSwitchPG_pcst           Static SwitchPortGroup whose ports have to be switched on/off
 * \param DynamicStandaloneSwtPG_pst    Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Down( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                           EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )
{
    /* Declare local variables */
    uint8                   lEthIfCtrlIdx_u8;
    uint8                   lRefTrcvOrSwtOrPGTableIdx_u8;
    Std_ReturnType          lStdRetVal_en;

    /* Check if the requested state of the Standalone EthIfSwitchPortGroup is ETHTRCV_MODE_ACTIVE or the Port Active Timer is started. */
    if( (DynamicStandaloneSwtPG_pst->ReqMode_en == ETHTRCV_MODE_ACTIVE) ||
        (EthIf_PortActiveTime_u32 > 0U) )
    {
        /* Check if at least one link-info PG is active before executing standalone PG state machine */
        /* hint: link-info PG is responsible for initializing EthCtrl, initializing EthSwt and setting EthCtrl mode to ACTIVE. These steps need to be done before EthSwt_SetSwitchPortMode() can be called using standalone PG */
        /* Also it must always be ensure that at least one link-info PG is active while standalone PG are used in order to fulfill requirement [SWS_EthIf_00256] */
        for( lEthIfCtrlIdx_u8 = 0U; lEthIfCtrlIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lEthIfCtrlIdx_u8++ )
        {
            /* Fetch the switchPG index referenced by EthIfCtrl */
            lRefTrcvOrSwtOrPGTableIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8;

            /* Check if there is a link-info PG connected to 'same EthCtrl' and for this EthIfCtrlState is WAIT_TRCVMODE_ACTIVE or ACTIVE. */
            /* Check for 'Same EthCtrl' is required as multiple EthCtrl are supported. */

            /* Important note: Multiple switchPG connected to same EthCtrl (same cascading branch) have same index in EthIf_DynamicSwtPort for the hostport. */
            /* Check for 'Same EthCtrl' is indirectly verified by checking the index of EthIf_DynamicSwtPort for the hostport. */
            /* Index of EthIf_DynamicSwtPort for the hostport is always stored at zeroth index of RefHostUplinkPortsTablePtr_cpacu8. */

            if( (EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlIdx_u8].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) &&
                (StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[0] == EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lRefTrcvOrSwtOrPGTableIdx_u8].RefHostUplinkPortsTablePtr_cpacu8[0] ) &&
                ((EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE) ||
                 (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE)) )
            {
                break;
            }
        }

        /* If atleast one link-info PG is found to be in state WAIT_TRCVMODE_ACTIVE or ACTIVE, proceed with the standalone PG state machine execution. */
        if( lEthIfCtrlIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8 )
        {
            /* Set the EthIfSwitchPortGroup to active. */
            lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeActive( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                               StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                               StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                               StaticSwitchPG_pcst->NbStdPorts_cu8 );

            /* If request to set the ports of the PG to active is accepted. */
            if( lStdRetVal_en == E_OK )
            {
                /* Set the state of the PG to ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE. */
                DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE;
            }
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
 ***************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE
 *
 * Parameter In:
 * \param StaticSwitchPG_pcst           Static SwitchPortGroup whose ports have to be switched on/off
 * \param DynamicStandaloneSwtPG_pst    Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * Return   None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvActive( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                     EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )
{
    /* Declare local variables */
    Std_ReturnType       lStdRetVal_en;
    boolean              lSwtPGActiveStatus_b;

    /* If the requested PG mode is DOWN and Port Active timer is not started, */
    /* then request the PG to DOWN and change the PG mode to WAIT_TRCVMODE_DOWN. */
    if(( DynamicStandaloneSwtPG_pst->ReqMode_en == ETHTRCV_MODE_DOWN ) &&
       ( EthIf_PortActiveTime_u32 ==  0U ))
    {
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeDown( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                         StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                         StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                         StaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If the request was successful. */
        if( lStdRetVal_en == E_OK )
        {
            DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN;
        }
    }
    else
    {
        /* Check host port, all uplink ports and one standard port are successfully activated. */
        lSwtPGActiveStatus_b =  EthIf_Prv_CheckSwitchOrPGActiveStatus( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                                       StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                                       StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                                       StaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If PG activation was successful. */
        if( lSwtPGActiveStatus_b != FALSE )
        {
            /* If the requested PG mode is ACTIVE, then change the mode of the PG to ACTIVE. */
            if( DynamicStandaloneSwtPG_pst->ReqMode_en == ETHTRCV_MODE_ACTIVE )
            {
                DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_ACTIVE;

    #if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
                /* Set EthIf_SwitchPortLinkStateChanged_b to TRUE if the PG is activated, so that the link state accumulation state machine is executed. */
                EthIf_SwitchPortLinkStateChanged_b = TRUE;
    #endif
            }
            /* If the requested PG mode is DOWN, then it means the port active timer was started. Change the state of the PG to Startup. */
            else
            {
                /* Change the state of the EthIfCtrl to ETHIF_STANDALONE_PG_MODE_STARTUP */
                DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_STARTUP;

    #if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
                /* Set EthIf_SwitchPortLinkStateChanged_b to TRUE if the PG is activated, so that the link state accumulation state machine is executed. */
                EthIf_SwitchPortLinkStateChanged_b = TRUE;
    #endif
            }
        }
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_STARTUP
 *
 * Parameter In:
 * \param StaticSwitchPG_pcst           Static SwitchPortGroup whose ports have to be switched on/off
 * \param DynamicStandaloneSwtPG_pst    Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Startup( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                              EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )
{
    /* Declare local variables */
    Std_ReturnType       lStdRetVal_en;

    /* Check if the requested state of the standalone EthIfSwitchPortGroup is ETHTRCV_MODE_ACTIVE */
    if( DynamicStandaloneSwtPG_pst->ReqMode_en == ETHTRCV_MODE_ACTIVE )
    {
        /* Change the state of the portgroup to ETHIF_STANDALONE_PG_MODE_ACTIVE */
        DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_ACTIVE;
    }
    /* Check if the Ports active time has expired */
    else if( EthIf_PortActiveTime_u32 == 0U )
    {
        /* If timer is expired, stop all the ports of EthIfSwitchPortGroup which are not requested for ACTIVE */
        lStdRetVal_en = EthIf_Prv_SetSwitchOrPGModeDown( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                         StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                         StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                         StaticSwitchPG_pcst->NbStdPorts_cu8 );

        /* If the request was successful. */
        if( lStdRetVal_en == E_OK )
        {
            /* Set the state of the EthIfSwitchPortGroup to ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN */
            DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN;
        }
    }
    else
    {
        /* Do nothing */
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_ACTIVE
 *
 * Parameter In:
 * \param PortGrpIdx_u8                 Index of the standalone EthIfSwitchPortGroup.
 * \param StaticSwitchPG_pcst           Static SwitchPortGroup whose ports have to be switched on/off
 * \param DynamicStandaloneSwtPG_pst    Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * Return   None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_Active( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                             EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )
{
    /* Declare local variables */
    uint8                                lLoopIdx_u8;
    EthIf_DynamicSwtPort_tst*            lDynamicSwtPort_past;

    /* If the requested port mode is DOWN */
    if( DynamicStandaloneSwtPG_pst->ReqMode_en == ETHTRCV_MODE_DOWN )
    {
        /* -------------------------------------- */
        /* Start PortOff delay for standard ports */
        /* -------------------------------------- */

        /* loop through all the standard ports of the PortGroup */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < StaticSwitchPG_pcst->NbStdPorts_cu8; lLoopIdx_u8++ )
        {
            /* Get pointer to the current standard port in EthIf Dynamic Swt Port table */
            lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8[lLoopIdx_u8]];

            /* Decrement the port active count */
            lDynamicSwtPort_past->ActiveCntr_u8 = (lDynamicSwtPort_past->ActiveCntr_u8 > 0U) ? (lDynamicSwtPort_past->ActiveCntr_u8 - 1U) : (0U);

            /* Check if the port state is WAIT_ACTIVE or ACTIVE */
            if( (lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_WAIT_ACTIVE) ||
                (lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE) )
            {
                /* If the port is not used by any other EthIfSwitchPortGroup, then the port is switched off after the Port off delay timer. */
                if( (lDynamicSwtPort_past->ActiveCntr_u8 == 0U ) &&
                    (lDynamicSwtPort_past->PortOffDelay_u32 == 0U) )
                {
                    /* Start Port off delay timer, by incrementing by one main function period. */
                    lDynamicSwtPort_past->PortOffDelay_u32 += ETHIF_MAINFUNCTION_STATE_PERIOD;
                }
            }
        }


        /* ----------------------------------------------- */
        /* Start PortOff delay for host and uplink ports   */
        /* ----------------------------------------------- */

        /* loop through host amd uplink ports of the PortGroup */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < StaticSwitchPG_pcst->NbHostUplinkPorts_cu8; lLoopIdx_u8++ )
        {
            /* Get pointer to the current host/uplink port in EthIf Dynamic Swt Port table */
            lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[lLoopIdx_u8] ];

            /* Decrement the port active count */
            lDynamicSwtPort_past->ActiveCntr_u8 = (lDynamicSwtPort_past->ActiveCntr_u8 > 0U) ? (lDynamicSwtPort_past->ActiveCntr_u8 - 1U) : (0U);

            /* Check if the port state is  WAIT_ACTIVE or ACTIVE */
            if( (lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_WAIT_ACTIVE) ||
                (lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE) )
            {
                /* If the port is not used by any other EthIfSwitchPortGroup, then the port is switched off after the Port off delay timer. */
                if( (lDynamicSwtPort_past->ActiveCntr_u8 == 0U ) &&
                    (lDynamicSwtPort_past->PortOffDelay_u32 == 0U) )
                {
                    /* Start Port off delay timer, by incrementing by one main function period. */
                    lDynamicSwtPort_past->PortOffDelay_u32 += ETHIF_MAINFUNCTION_STATE_PERIOD;
                }
            }
        }

        /* Set the PG state to wait down */
        DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_WAITDOWN;
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_WAITDOWN
 *
 * Parameter In:
 * \param StaticSwitchPG_pcst           Static SwitchPortGroup whose ports have to be switched on/off
 * \param DynamicStandaloneSwtPG_pst    Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * Return   None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitDown( const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                               EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )

{
    /* Declare local variables */
    Std_ReturnType                       lStdRetVal_en;
    uint8                                lLoopIdx_u8;
    EthIf_DynamicSwtPort_tst*            lDynamicSwtPort_past;
    boolean                              lPortModeChangeFailed_b;
    boolean                              lPortsStateMatch_b;
    boolean                              lPortDelayExpired_b;
    const EthIf_StaticSwitch_tst*        lStaticSwitch_pcst;

    /* Check if all the ports of the PG are in state other than ETHIF_SWTPORT_STATE_WAIT_ACTIVE. */
    lPortsStateMatch_b = EthIf_Prv_CheckSwitchOrPGPortInState( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                               StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                               StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                               StaticSwitchPG_pcst->NbStdPorts_cu8,
                                                               ETHIF_SWTPORT_STATE_WAIT_ACTIVE );

    /* If all the ports of the PG are in a different state than ETHIF_SWTPORT_STATE_WAIT_ACTIVE, */
    /* Request mode down to all the active ports which are used only by the current PG and port delay timer has expired. */
    if( FALSE == lPortsStateMatch_b )
    {
        /* Initialize port mode change and port delay expired flags */
        lPortDelayExpired_b = TRUE;
        lPortModeChangeFailed_b = FALSE;

        /* ------------------------------------------- */
        /* PortOff delay expiration for standard ports */
        /* ------------------------------------------- */

        /* loop through all the standard ports of the PortGroup */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < StaticSwitchPG_pcst->NbStdPorts_cu8; lLoopIdx_u8++ )
        {
            /* Get pointer to the current standard port in EthIf Dynamic Swt Port table */
            lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8[lLoopIdx_u8]];

            /* The port is in use in another PG, no timer expiration to check */
            if( lDynamicSwtPort_past->ActiveCntr_u8 > 0U )
            {
                continue;
            }

            /* Check whether the port off delay timer has been exceeded, and the port is still active */
            if( lDynamicSwtPort_past->PortOffDelay_u32 >= ETHIF_PORT_SWITCHOFF_DELAY )
            {
                /* When PortOffDelay_u32 is non-zero, PortState will be always ETHIF_SWTPORT_STATE_ACTIVE. */
                /* The additional check of PortState against ETHIF_SWTPORT_STATE_ACTIVE is done to avoid closing of any non-active ports. */
                if( lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE )
                {
                    /* Get the Static Switch related to current standard port */
                    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lDynamicSwtPort_past->RefSwitchTableIdx_cu8];

                    /* Set the PortGroup to DOWN */
                    lStdRetVal_en = EthSwt_SetSwitchPortMode ( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                               lDynamicSwtPort_past->EthSwtPortIdx_cu8,
                                                               ETHTRCV_MODE_DOWN );

                    /* If the request to switch off the port was successful */
                    if( lStdRetVal_en == E_OK )
                    {
                        /* Clear the Port delay timer */
                        lDynamicSwtPort_past->PortOffDelay_u32 = 0UL;

                        /* Set the Port state to ETHIF_SWTPORT_STATE_WAIT_DOWN */
                        lDynamicSwtPort_past->PortState_en  = ETHIF_SWTPORT_STATE_WAIT_DOWN;
                    }
                    else
                    {
                        /* Set flag a flag to TRUE to indicate setting of a port to DOWN failed */
                        lPortModeChangeFailed_b = TRUE;

                        /* If setting of any port to DOWN fails, terminate setting of remaining ports to DOWN */
                        break;
                    }
                }
            }

            /* Port is not in use in other PG but timer did not expire yet */
            else
            {
                lPortDelayExpired_b = FALSE;
            }
        }

        /* ------------------------------------------- */
        /* PortOff delay expiration for host and uplink ports */
        /* ------------------------------------------- */

        /* Check if setting DOWN of all standard ports were successful */
        if( (lPortDelayExpired_b != FALSE) && (lPortModeChangeFailed_b == FALSE) )
        {
            /* loop through all the host and uplink ports of the PortGroup */
            /* Port deactivation needs to begin with deactivation of uplink port of the slave, then uplink port of master and later host port */
            for( lLoopIdx_u8 = StaticSwitchPG_pcst->NbHostUplinkPorts_cu8; lLoopIdx_u8 > 0; lLoopIdx_u8-- )
            {
                /* Get pointer to the current standard port in EthIf Dynamic Swt Port table */
                lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[lLoopIdx_u8 - 1U] ];

                /* The port is in use in another PG, no timer expiration to check */
                if( lDynamicSwtPort_past->ActiveCntr_u8 > 0U )
                {
                    continue;
                }

                /* Check whether the port off delay timer has been exceeded, and the port is still active */
                if( lDynamicSwtPort_past->PortOffDelay_u32 >= ETHIF_PORT_SWITCHOFF_DELAY )
                {
                    /* When PortOffDelay_u32 is non-zero, PortState will be always ETHIF_SWTPORT_STATE_ACTIVE. */
                    /* The additional check of PortState against ETHIF_SWTPORT_STATE_ACTIVE is done to avoid closing of any non-active ports. */
                    if( lDynamicSwtPort_past->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE )
                    {
                        /* Get the Static Switch related to current standard port */
                        lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lDynamicSwtPort_past->RefSwitchTableIdx_cu8];

                        /* Set the PortGroup to DOWN */
                        lStdRetVal_en = EthSwt_SetSwitchPortMode ( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                                   lDynamicSwtPort_past->EthSwtPortIdx_cu8,
                                                                   ETHTRCV_MODE_DOWN );

                        /* If the request to switch off the port was successful */
                        if( lStdRetVal_en == E_OK )
                        {
                            /* Clear the Port delay timer */
                            lDynamicSwtPort_past->PortOffDelay_u32 = 0UL;

                            /* Set the Port state to ETHIF_SWTPORT_STATE_WAIT_DOWN */
                            lDynamicSwtPort_past->PortState_en  = ETHIF_SWTPORT_STATE_WAIT_DOWN;
                        }
                        else
                        {
                            /* Set flag a falg to TRUE to indicate setting of a port to DOWN failed */
                            lPortModeChangeFailed_b = TRUE;

                            /* If setting of any port to DOWN fails, terminate setting of remaining ports to DOWN */
                            break;
                        }
                    }
                }

                /* Port is not in use in other PG but timer did not expire yet */
                else
                {
                    lPortDelayExpired_b = FALSE;
                }
            }

            /* Check if setting DOWN of all host, uplink and standard ports were successful */
            if( (lPortDelayExpired_b != FALSE) && (lPortModeChangeFailed_b == FALSE) )
            {
                /* Set state to ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN. */
                DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN;
            }
        }
    }

    return;
}
#endif


/**
 *********************************************************************************************************************************************************
 * \module description
 * Called by EthIf_Prv_StandaloneSwtPG_StateTransitions(), to trigger all possible transitions from state ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN
 *
 * Parameter In:
 * \param PortGrpIdx_u8                  Index of the standalone EthIfSwitchPortGroup.
 * \param lEthIf_Swt_PortGrp_pst         Static SwitchPortGroup whose ports have to be switched on/off
 * \param lEthIf_StandalonePG_Type_pst   Holds the state, link and requested mode for the Standalone SwitchPortGroup
 *
 * Return   None
 *
 *********************************************************************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_Transfrm_State_WaitTrcvDown( uint8 PortGrpIdx_u8,
                                                                   const EthIf_StaticSwtPortGroup_tst * StaticSwitchPG_pcst,
                                                                   EthIf_DynamicStandaloneSwtPortGroup_tst * DynamicStandaloneSwtPG_pst )
{
    /* Declare local variables. */
    boolean                               lSwtPGDownStatus_b;

    /* Check if all the ports which were requested to down state, are successfully switched off. */
    lSwtPGDownStatus_b = EthIf_Prv_CheckSwitchOrPGDownStatus( StaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                                              StaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                                              StaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                                              StaticSwitchPG_pcst->NbStdPorts_cu8 );

    /* If all the requested ports are down. */
    if( FALSE != lSwtPGDownStatus_b )
    {
        /* Set the state of the EthIf Ctrl to ETHIF_STANDALONE_PG_MODE_DOWN. */
        DynamicStandaloneSwtPG_pst->Mode_en = ETHIF_STANDALONE_PG_MODE_DOWN;

        /* If the portgroup is down and the link state of the PG is still active. */
        if( DynamicStandaloneSwtPG_pst->LinkState_en == ETHIF_LINKSTATE_ACTIVE )
        {
            /* Call BswM Link state info API */
            BswM_EthIf_PortGroupLinkStateChg( PortGrpIdx_u8, ETHTRCV_LINK_STATE_DOWN );

            /* Set link state to ETHIF_LINKSTATE_DOWN. */
            DynamicStandaloneSwtPG_pst->LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
    }

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
