

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if(ETHIF_ETHIFTRCV_EXIST == STD_ON)
#include "EthTrcv.h"
#endif

#if(ETHIF_ETHIFSWITCH_EXIST == STD_ON)
#include "EthSwt.h"
#endif

#include "EthSM.h"
#include "EthSM_Cbk.h"

#if ( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "BswM_EthIf.h"
#endif

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_CLEARED_8
#include "EthIf_MemMap.h"

/* Link State Reload counter */
/* This is a global static variable because it is not desirable to allow direct access to it outside the file */
static uint8 EthIf_LinkStateReloadCntr_u8 = 0U;

#define ETHIF_STOP_SEC_VAR_CLEARED_8
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

static void EthIf_Prv_EthIfCtrl_LinkTransitions         ( uint8 LinkStatusCheckValidity_u8 );

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_LinkTransitions              ( uint8 EthIfCtrlIdx_u8  );
#endif

#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPG_LinkTransitions             ( uint8 EthIfCtrlIdx_u8  );
#endif

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_LinkTransitions               ( uint8 EthIfCtrlIdx_u8  );
#endif

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_LinkTransitions   ( void );
#endif

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_GetLinkState                      ( const uint8 * HostUplinkPortTable_pcau8,
                                                          const uint8 * StdPortTable_pcau8,
                                                          uint8 SizeHostUplinkPortTable_u8,
                                                          uint8 SizeStdPortTable_u8,
                                                          EthIf_EthIfLinkState_ten * LinkState_pen );
#endif

#if ( (ETHIF_ETHIFTRCV_EXIST == STD_ON) || ( (ETHIF_ETHIFSWITCH_EXIST == STD_ON) && (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_OFF) ) )
static void EthIf_Prv_ReadHWLinkState                   (void);
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
 ***************************************************************************************************
 * \module description
 * Called by EthIf_MainFunctionState(), to check link status of transceiver/Switch/EthIf SwitchPortGroups
 *
 * \return  None
 *
 ***************************************************************************************************
 */
void EthIf_Prv_LinkStateProcessing( void )
{
    /* Increment counter for link state monitoring */
    EthIf_LinkStateReloadCntr_u8++;

    /* Check for the transceiver/Switch/Switch PortGroups link state is to be read or not (polling mode) */
    if ( EthIf_LinkStateReloadCntr_u8 == ETHIF_TRCV_LINKSTATECHGMAIN_RELOAD )
    {
        /* Only in case switch/switchPortGroup present and link state is provided by interrupt, there is no need for API call EthIf_Prv_ReadHWLinkState(). */
#if ( (ETHIF_ETHIFTRCV_EXIST == STD_ON) || ( (ETHIF_ETHIFSWITCH_EXIST == STD_ON) && (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_OFF) ) )
        /* Read link state for EthTrcv and EthSwt ports */
        EthIf_Prv_ReadHWLinkState();
#endif

        /* Call API to execute hardware link status check for all EthIfControllers */
        EthIf_Prv_EthIfCtrl_LinkTransitions( ((uint8)ETHIF_ETHIFCTRL_REF_TRCV |
                                              (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL |
                                              (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO |
                                              (uint8)ETHIF_ETHIFCTRL_REF_SWITCH) );

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
        /* Trigger API for standalone port groups link status check */
        EthIf_Prv_StandaloneSwtPG_LinkTransitions();
#endif

        /* Reset the counter for link state monitoring to 0 */
        EthIf_LinkStateReloadCntr_u8 = 0U;
    }

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
    /* Check for the Switch/Switch PortGroups link state is to be read or not (interrupt mode) */
    /* In link state interrupt mode, if EthIf_SwitchLinkUpIndication or EthIf_SwitchLinkDownIndication callback has been received since last EthIf_MainFunction state */
    /* then link state accumulation need to be executed */
    if( EthIf_SwitchPortLinkStateChanged_b != FALSE )
    {
        /* Call API to execute hardware link status check for EthIfControllers referencing PG or switch */
        EthIf_Prv_EthIfCtrl_LinkTransitions( ((uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL |
                                              (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO |
                                              (uint8)ETHIF_ETHIFCTRL_REF_SWITCH) );

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
        /* Trigger API for standalone port groups link status check */
        EthIf_Prv_StandaloneSwtPG_LinkTransitions();
#endif

        /* Reset the link state interrupt flag to FALSE */
        EthIf_SwitchPortLinkStateChanged_b = FALSE;
    }
#endif

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/**
 ***************************************************************************************************
 * \module description
 * To check link status of transceiver/Switch/EthIf SwitchPortGroups
 *
 * Parameter In:
 * \param LinkStatusCheckValidity_u8    Indicate for which kind of EthIfCtrl the link status check need to be executed
 *
 * \return                              None
 *
 ***************************************************************************************************
 */
static void EthIf_Prv_EthIfCtrl_LinkTransitions( uint8 LinkStatusCheckValidity_u8 )
{
    /* Declare local variables */
    uint8                                       lEthIfCtrlIdx_u8;

    /* Loop through all EthIf Ctrls */
   for( lEthIfCtrlIdx_u8 = 0; lEthIfCtrlIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lEthIfCtrlIdx_u8++ )
   {
       /* Based on the reference call the particular link transition APIs */
       switch( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlIdx_u8].ReferenceType_cen )
       {
           /* If the reference is a transceiver */
           case ETHIF_ETHIFCTRL_REF_TRCV:
           {
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
               /* If it is valid to execute link status check for EthIfCtrl referencing transceiver */
               if( (LinkStatusCheckValidity_u8 & (uint8)ETHIF_ETHIFCTRL_REF_TRCV) == (uint8)ETHIF_ETHIFCTRL_REF_TRCV )
               {
                   /* Call the transceiver link status check API */
                   EthIf_Prv_Trcv_LinkTransitions( lEthIfCtrlIdx_u8 );
               }
#endif
           }
           break;

           /* If the reference is a EthIfSwitchPortGroup of semantic Control or Link info */
           case ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL:
           case ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO:
           {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
               /* If it is valid to execute link status check for EthIfCtrl referencing port group */
               if( ((LinkStatusCheckValidity_u8 & (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL) == (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL)    ||
                   ((LinkStatusCheckValidity_u8 & (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) == (uint8)ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) )
               {
                   /* Call the port group link status check API */
                   EthIf_Prv_SwtPG_LinkTransitions( lEthIfCtrlIdx_u8 );
               }
#endif
           }
           break;

           /* If the reference is a switch */
           case ETHIF_ETHIFCTRL_REF_SWITCH:
           {
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
               /* If it is valid to execute link status check for EthIfCtrl referencing switch */
               if( (LinkStatusCheckValidity_u8 & (uint8)ETHIF_ETHIFCTRL_REF_SWITCH) == (uint8)ETHIF_ETHIFCTRL_REF_SWITCH )
               {
                   /* Call the switch link status check API */
                   EthIf_Prv_Swt_LinkTransitions( lEthIfCtrlIdx_u8 );
               }
#endif
           }
           break;

           default:
               /* Do Nothing */
           break;
       }
   }

   /* If no reference is configured for any EthIfCtrl, then void casting LinkStatusCheckValidity_u8 is necessary to avoid MISRA warning. */
   (void)LinkStatusCheckValidity_u8;

   return;
}


/**
 ***********************************************************************************************************************
 * \module description
 * To trigger all possible link transitions for EthIfCtrl referring to a transceiver
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return                  None
 *
 ***********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
static void EthIf_Prv_Trcv_LinkTransitions( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    EthIf_EthIfLinkState_ten                   lLinkState_en;

    /* Check the link status if the state of the EthIfCtrl is ETHIF_ETHIFCTRL_STATE_ACTIVE */
    if( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE )
    {
        /* Get already calculated transceiver link status */
        lLinkState_en = EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8].TrcvLinkState_en;

        /* If the current link state is DOWN and link state reported to EthSM is ACTIVE */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_ACTIVE) &&
            (lLinkState_en == ETHIF_LINKSTATE_DOWN) )
        {
            /* Indicate link state down to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_DOWN );

            /* Store the link status informed to EthSM */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
        /* If the current link state is ACTIVE and link state reported to EthSM is DOWN */
        else if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_DOWN) &&
                 (lLinkState_en == ETHIF_LINKSTATE_ACTIVE) )
        {
            /* Indicate link state active to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_ACTIVE );

            /* Store the link status informed to EthSM */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_ACTIVE;
        }
        else
        {
            /* Do Nothing */
        }
    }

    return;
}
#endif


/**
 ***********************************************************************************************************************
 * \module description
 * Trigger all possible link transitions for EthIfCtrl referring to a port group
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return                  None
 *
 ***********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_SwtPG_LinkTransitions( uint8 EthIfCtrlIdx_u8  )
{
    /* Declare local variable */
    EthIf_EthIfLinkState_ten                    lLinkState_en;
    const EthIf_StaticSwtPortGroup_tst*         lStaticSwitchPG_pcst;

    /* Fetch the Static Switch PG structure */
    lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Execute the link status check if the state of the EthIfCtrl is ETHIF_ETHIFCTRL_STATE_ACTIVE */
    if( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE )
    {
        /* Initialize local variable lLinkState_en (avoid compiler warning) */
        lLinkState_en = ETHIF_LINKSTATE_DOWN;

        /* Call the Portgroup link status API check */
        EthIf_Prv_GetLinkState( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                lStaticSwitchPG_pcst->NbStdPorts_cu8,
                                &lLinkState_en );

        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_ACTIVE) &&
            (lLinkState_en == ETHIF_LINKSTATE_DOWN) )
        {
            /* Indicate link state down to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_DOWN );

            /* Set the link status of the PortGroup to ETHIF_ETHIFCTRL_LINKSTATE_DOWN */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
        else if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_DOWN) &&
                 (lLinkState_en == ETHIF_LINKSTATE_ACTIVE) )
        {
            /* Indicate link state active to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_ACTIVE );

            /* The link status of the Port group is considered active if the host port and atleast one non-empty port link is active */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_ACTIVE;
        }
        else
        {
            /* Do Nothing */
        }
    }

    return;
}
#endif


/**
 ***********************************************************************************************************************
 * \module description
 * Trigger all possible link transitions for EthIfCtrl referring to a switch
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of EthIfCtrl for which state transitions needs to be triggered
 *
 * \return                  None
 *
 ***********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
static void EthIf_Prv_Swt_LinkTransitions( uint8 EthIfCtrlIdx_u8  )
{
    /* Declare local variable */
    EthIf_EthIfLinkState_ten                    lLinkState_en;
    const EthIf_StaticSwitch_tst*               lStaticSwitch_pcst;

    /* Fetch the Static Switch PG structure */
    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefTrcvOrSwtOrPGTableIdx_cu8];

    /* Execute the EthIfSwitch link status check if the state of the EthIfSwitch is ETHIF_ETHIFCTRL_STATE_ACTIVE */
    if( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE )
    {
        /* Initialize local variable lLinkState_en (avoid compiler warning) */
        lLinkState_en = ETHIF_LINKSTATE_DOWN;

        /* Call the Portgroup link status API check */
        EthIf_Prv_GetLinkState( lStaticSwitch_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                lStaticSwitch_pcst->RefStdPortsTablePtr_cpacu8,
                                lStaticSwitch_pcst->NbHostUplinkPorts_cu8,
                                lStaticSwitch_pcst->NbStdPorts_cu8,
                                &lLinkState_en );

        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_ACTIVE) &&
            (lLinkState_en == ETHIF_LINKSTATE_DOWN) )
        {
            /* Indicate link state down to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_DOWN );

            /* Set the link status of the EthIfCtrl to ETHIF_ETHIFCTRL_LINKSTATE_DOWN */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
        else if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en == ETHIF_LINKSTATE_DOWN) &&
                 (lLinkState_en == ETHIF_LINKSTATE_ACTIVE ) )
        {
            /* Indicate link state active to EthSM */
            EthSM_TrcvLinkStateChg( EthIfCtrlIdx_u8, ETHTRCV_LINK_STATE_ACTIVE );

            /* The link status of the Port group is considered active if the host port and atleast one non-empty port link is active */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[EthIfCtrlIdx_u8].LinkState_en = ETHIF_LINKSTATE_ACTIVE;
        }
        else
        {
            /* Do Nothing */
        }
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Called by EthIf_MainFunctionState(), to trigger all possible link transitions for EthIf SwitchPortGroups with no semantics
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_StandaloneSwtPG_LinkTransitions( void )
{
    /* Declare local variables */
    uint8                                       lPortGrpIdx_u8;
    EthIf_EthIfLinkState_ten                    lLinkState_en;
    const EthIf_StaticSwtPortGroup_tst*         lStaticSwitchPG_pcst;

    /* Loop through all configured EthIfSwitchPortGroups */
    for( lPortGrpIdx_u8 = 0U; lPortGrpIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPortGroup_cu8; lPortGrpIdx_u8++ )
    {
        /* Fetch the Static Switch PG structure */
        lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lPortGrpIdx_u8];

        /* If the PortGroup is standalone, execute the link status check if the state is Startup, active or waitdown */
        if( (lStaticSwitchPG_pcst->Semantic_cen == ETHIF_SWTPORTGROUP_UNREFERENCED) &&
            ((lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->Mode_en == ETHIF_STANDALONE_PG_MODE_STARTUP) ||
             (lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->Mode_en == ETHIF_STANDALONE_PG_MODE_ACTIVE)  ||
             (lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->Mode_en == ETHIF_STANDALONE_PG_MODE_WAITDOWN)) )
        {
            /* Initialize local variable lLinkState_en (avoid compiler warning) */
            lLinkState_en = ETHIF_LINKSTATE_DOWN;

            /* Call the Portgroup link status API check */
            EthIf_Prv_GetLinkState( lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8,
                                    lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8,
                                    lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8,
                                    lStaticSwitchPG_pcst->NbStdPorts_cu8,
                                    &lLinkState_en );

            if( (lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->LinkState_en == ETHIF_LINKSTATE_ACTIVE) &&
                (lLinkState_en == ETHIF_LINKSTATE_DOWN) )
            {
                /* Call BswM Link state info API */
                BswM_EthIf_PortGroupLinkStateChg( lPortGrpIdx_u8, ETHTRCV_LINK_STATE_DOWN );

                /* Set the state of the standalone PG to ETHIF_STANDALONE_PG_LINKSTATE_DOWN */
                lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->LinkState_en = ETHIF_LINKSTATE_DOWN;
            }
            else if( (lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->LinkState_en == ETHIF_LINKSTATE_DOWN) &&
                     (lLinkState_en == ETHIF_LINKSTATE_ACTIVE) )
            {
                /* Call BswM Link state info API */
                BswM_EthIf_PortGroupLinkStateChg( lPortGrpIdx_u8, ETHTRCV_LINK_STATE_ACTIVE );

                /* The link status of the Port group is considered active if the host port and atleast one non-empty port link is active */
                lStaticSwitchPG_pcst->RefStandaloneSwtPortGroupTableEntry_cpst->LinkState_en = ETHIF_LINKSTATE_ACTIVE;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    return;
}
#endif


/**
 ***********************************************************************************************************************
 * \module description
 * EthIf_Prv_GetLinkState: This API returns the link status of a EthIfSwitch/EthIfSwitchPortGroup as per link accumulation
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 *
 * Parameters Out:
 * \param linkstate                    Link status of the Switch/SwitchPortGroup as per Link accumulation.
 *
  * \return  None
 *
 ***********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_GetLinkState( const uint8 * HostUplinkPortTable_pcau8,
                                    const uint8 * StdPortTable_pcau8,
                                    uint8 SizeHostUplinkPortTable_u8,
                                    uint8 SizeStdPortTable_u8,
                                    EthIf_EthIfLinkState_ten * LinkState_pen )
{
    /* Declare local variables */
    uint8     lLoopIdx_u8;

    /* Initialize accumulated link state to ACTIVE. */
    *LinkState_pen = ETHIF_LINKSTATE_ACTIVE;

    /* Loop through all host and uplink ports*/
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
    {
        /* Criteria 1 for accumulated link state to be ACTIVE is, all host and uplink ports must have ACTIVE link status.  */
        if(EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8] ].PortLinkState_en != ETHIF_LINKSTATE_ACTIVE )
        {
            *LinkState_pen = ETHIF_LINKSTATE_DOWN;
            break;
        }
    }

    /* Check if all host and uplink ports has ACTIVE link status. */
    if(*LinkState_pen == ETHIF_LINKSTATE_ACTIVE)
    {
        /* Reinitialize accumulated link state to DOWN. */
        *LinkState_pen = ETHIF_LINKSTATE_DOWN;

        /* Loop through all standard ports*/
        for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
        {
            /* Criteria 2 for accumulated link state to be ACTIVE is, at least one standard port must have ACTIVE link status.  */
            if(EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ].PortLinkState_en == ETHIF_LINKSTATE_ACTIVE )
            {
                *LinkState_pen = ETHIF_LINKSTATE_ACTIVE;
                break;
            }
        }
    }

    return;
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * To read link status of transceiver and switch ports (only if link state acquired by polling).
 * Function is called by EthIf_Prv_LinkStateProcessing() when link reload timer expires.
 *
 * \return                              None
 *
 ***************************************************************************************************
 */

#if ( (ETHIF_ETHIFTRCV_EXIST == STD_ON) || ( (ETHIF_ETHIFSWITCH_EXIST == STD_ON) && (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_OFF) ) )
static void EthIf_Prv_ReadHWLinkState( void )
{
    /* Declaration of local variables */
    Std_ReturnType               lStdRetVal_en;
    EthTrcv_LinkStateType        lLinkState_en;
    uint8                        lLoopIdx_u8;
#if ( (ETHIF_ETHIFSWITCH_EXIST == STD_ON) && (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_OFF) )
    uint8                        lRefSwitchTableIdx_u8;
#endif

#if (ETHIF_ETHIFTRCV_EXIST == STD_ON)
    /* Loop through all EthIf transceivers configured */
    for(lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8; lLoopIdx_u8++)
    {
        /* Check if the Trcv state is ETHIF_TRCV_STATE_ACTIVE */
        if( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvState_en == ETHIF_TRCV_STATE_ACTIVE)
        {
            /* Initialize local variable lLinkState_en (avoid compiler warning) */
            lLinkState_en = ETHTRCV_LINK_STATE_DOWN;

            /* Get link state from the Trcv*/
            lStdRetVal_en = EthTrcv_GetLinkState( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].EthTrcvIdx_cu8,
                                                  &lLinkState_en );

            if(lStdRetVal_en == E_OK)
            {
                /* Convert to EthIf_EthIfLinkState_ten and store the result in Trcv dynamic structure. */
                EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvLinkState_en = (lLinkState_en == ETHTRCV_LINK_STATE_ACTIVE) ? (ETHIF_LINKSTATE_ACTIVE) : (ETHIF_LINKSTATE_DOWN);
            }
        }
    }
#endif

#if ( (ETHIF_ETHIFSWITCH_EXIST == STD_ON) && (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_OFF) )

    /* Loop through all configured EthIf switch ports */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPorts_cu8; lLoopIdx_u8++ )
    {
        /* Check if the switch port state is ETHIF_SWTPORT_STATE_ACTIVE */
        if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortState_en == ETHIF_SWTPORT_STATE_ACTIVE )
        {
            /* Initialize local variable lLinkState_en (avoid compiler warning) */
            lLinkState_en = ETHTRCV_LINK_STATE_DOWN;

            /* Get the index of static switch table for the switch port */
            lRefSwitchTableIdx_u8 = EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].RefSwitchTableIdx_cu8;

            /* Get the link state from Eth switch */
            lStdRetVal_en = EthSwt_GetLinkState( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lRefSwitchTableIdx_u8].EthSwtIdx_cu8,
                                                 EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].EthSwtPortIdx_cu8,
                                                 &lLinkState_en );

            if( lStdRetVal_en == E_OK )
            {
                /* Convert to EthIf_EthIfLinkState_ten and store the result in switch dynamic structure. */
                EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortLinkState_en = (lLinkState_en == ETHTRCV_LINK_STATE_ACTIVE) ? (ETHIF_LINKSTATE_ACTIVE) : (ETHIF_LINKSTATE_DOWN);
            }
        }
    }
#endif

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
