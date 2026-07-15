

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "EthSwt.h"
#endif

#include "EthIf_Prv.h"


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
 * Initialises the indexed switch
 *
 * Parameter In:
 * \param CtrlIdx  Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: switch could not be initialized}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
Std_ReturnType EthIf_Prv_SwitchInit( uint8 EthIfCtrlIdx_u8 )

{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    const EthIf_StaticEthIfCtrl_tst*        lStaticEthIfCtrl_pcst;
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;
    uint8                                   lEthIfSwitchIndex_u8;
    uint8                                   lLoopIdx_u8;
#endif

    /* Initialize return value of the function to E_OK */
    lStdRetVal_en = E_OK;

    /* Get pointer to Static EthIfCtrl identified by argument CtrlIdx */
    lStaticEthIfCtrl_pcst = &EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8];

    /* -------------------------------------- */
    /* EthIfCtrl references a Switch PG       */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch Port Group */
    if( (lStaticEthIfCtrl_pcst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) ||
        (lStaticEthIfCtrl_pcst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL) )
    {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )

        /* Fetch the Static Switch PG structure */
        lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lStaticEthIfCtrl_pcst->RefTrcvOrSwtOrPGTableIdx_cu8];

        /* Loop through all host and uplink ports of the PortGroup */
        /* Reason: HostUplinkPort list contains at least one port (either host or uplink) from master and slave switches. Same statement is not true for StandardPort list. */
        for(lLoopIdx_u8 = 0; lLoopIdx_u8 < lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8; lLoopIdx_u8++)
        {
            /* Get the EthIfSwitch index of the host/uplink ports of the PG  */
            lEthIfSwitchIndex_u8 = EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[lLoopIdx_u8] ].RefSwitchTableIdx_cu8;

            /* Switch mode in the context of EthIf is UNINIT */
            if( EthIf_CfgPtr_pco->EthIf_DynamicSwitchTable_cpst[lEthIfSwitchIndex_u8].SwtState_en == ETHIF_SWT_STATE_UNINIT )
            {
                /* Set the switch mode to INIT */
                EthIf_CfgPtr_pco->EthIf_DynamicSwitchTable_cpst[lEthIfSwitchIndex_u8].SwtState_en = ETHIF_SWT_STATE_INIT;
            }
        }
#endif
    }

    /* -------------------------------------- */
    /* EthIfCtrl references a Switch          */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch */
    else if( lStaticEthIfCtrl_pcst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH )
    {
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
        /* Switch mode in the context of EthIf is UNINIT */
        if( EthIf_CfgPtr_pco->EthIf_DynamicSwitchTable_cpst[lStaticEthIfCtrl_pcst->RefTrcvOrSwtOrPGTableIdx_cu8].SwtState_en == ETHIF_SWT_STATE_UNINIT )
        {
            /* Set the switch mode to INIT */
            EthIf_CfgPtr_pco->EthIf_DynamicSwitchTable_cpst[lStaticEthIfCtrl_pcst->RefTrcvOrSwtOrPGTableIdx_cu8].SwtState_en = ETHIF_SWT_STATE_INIT;
        }
#endif
    }

    /* In case EthIfCtrl does not reference a Switch or PG */
    else
    {
        /* Return with E_NOT_OK */
        lStdRetVal_en = E_NOT_OK;
    }

    return lStdRetVal_en;
}
#endif


/**
 *****************************************************************************************************************
 * \module description
 * Checks whether at least one port ( host or uplink or standard port) is in a particular state.
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 * \param PortState_en                 Port state against which all the ports shall be checked
 *
 * \return  boolean TRUE:  At least one port is in state PortState_en
 *                  FALSE: No ports are in state PortState_en
 *
 *****************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
boolean EthIf_Prv_CheckSwitchOrPGPortInState( const uint8 * HostUplinkPortTable_pcau8,
                                              const uint8 * StdPortTable_pcau8,
                                              uint8 SizeHostUplinkPortTable_u8,
                                              uint8 SizeStdPortTable_u8,
                                              EthIf_SwtPortState_ten PortState_en )
{
    /* Declare local variables */
    uint8                                   lLoopIdx_u8;
    boolean                                 lPortStateMatch_b;

    /* Local variables initialization. */
    lPortStateMatch_b = FALSE;

    /* Loop through all host and uplink ports*/
    for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
    {
        /* Terminate the loop if any of the host or uplink port is in state PortState_en */
        if( PortState_en == EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[HostUplinkPortTable_pcau8[lLoopIdx_u8]].PortState_en )
        {
            lPortStateMatch_b = TRUE;
            break;
        }
    }

    /* If none of the host or uplink port is in state PortState_en */
    if(FALSE == lPortStateMatch_b)
    {
        /* Loop through all standard ports*/
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
        {
            /* Terminate the loop if any of the host or uplink port is in state PortState_en */
            if( PortState_en == EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[StdPortTable_pcau8[lLoopIdx_u8]].PortState_en )
            {
                lPortStateMatch_b = TRUE;
                break;
            }
        }
    }

    return lPortStateMatch_b;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Checks whether a EthIfSwitch or EthIfSwitchPortGroup has been successfully activated.
 * Criteria: Host port, all uplink ports and at least one standard port are activated.
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 *
 * \return  boolean {TRUE: activated; FALSE: not activated}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
boolean EthIf_Prv_CheckSwitchOrPGActiveStatus( const uint8 * HostUplinkPortTable_pcau8,
                                               const uint8 * StdPortTable_pcau8,
                                               uint8 SizeHostUplinkPortTable_u8,
                                               uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    uint8                              lLoopIdx_u8;
    boolean                            lActiveFlag_b;

    /* Initialize local variable */
    lActiveFlag_b = TRUE;

    /* Loop through all host and uplink ports */
    for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
    {
        /* If any of the host or uplink port is not in state ACTIVE, terminate further check as the required condition for Switch or PG to be ACTIVE is not met.*/
        if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8] ].PortState_en != ETHIF_SWTPORT_STATE_ACTIVE )
        {
            lActiveFlag_b = FALSE;
            break;
        }
    }

    /* Check if all host and uplink ports has port state ACTIVE */
    if(lActiveFlag_b != FALSE)
    {
        /* Reinitialize lActiveFlag_b to FALSE */
        lActiveFlag_b = FALSE;

        /* Loop through all standard ports */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
        {
            /* If any of the standard port is in state ACTIVE, terminate further check as the required condition for Switch or PG to be ACTIVE is met. */
            if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ].PortState_en == ETHIF_SWTPORT_STATE_ACTIVE )
            {
                lActiveFlag_b = TRUE;
                break;
            }
        }
    }

    return lActiveFlag_b;
}
#endif


/**
 *************************************************************************************************************
 * \module description
 * Checks whether a EthIfSwitch or EthIfSwitchPortGroup has been successfully de-activated.
 * Criteria: All ports of switch or PG have active count greater than 0 or active count equals to 0 with state ETHIF_SWTPORT_STATE_DOWN.
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 *
 * \return  boolean {TRUE: De-activated; FALSE: De-activation failed. }
 *
 *************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
boolean EthIf_Prv_CheckSwitchOrPGDownStatus( const uint8 * HostUplinkPortTable_pcau8,
                                             const uint8 * StdPortTable_pcau8,
                                             uint8 SizeHostUplinkPortTable_u8,
                                             uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    uint8                              lLoopIdx_u8;
    boolean                            lDownFlag_b;

    /* Initialize local variable */
    lDownFlag_b = TRUE;

    /* If the active count is greater than zero that means that the port is still in use by other EthIfCtrl or other PG. */
    /* In that case the port can be assumed as deactivated while checking if a Switch or PG is DOWN. */

    /* Loop through all host and uplink ports */
    for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
    {
        /* If any of the host or uplink port is not in state DOWN and corresponding counter is zero, */
        /* terminate further check as it ensures Switch or PG is not completely deactivated  */
        if( ( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8] ].ActiveCntr_u8 == 0U ) &&
            ( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8] ].PortState_en != ETHIF_SWTPORT_STATE_DOWN ) )
        {
            lDownFlag_b = FALSE;
            break;
        }
    }

    /* If all host and uplink ports are in state DOWN */
    if(lDownFlag_b != FALSE)
    {
        /* Loop through all standard ports */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
        {
            /* If any of the standard port is not in state DOWN and corresponding counter is zero, */
            /* terminate further check as it ensures Switch or PG is not completely deactivated  */
            if( ( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ].ActiveCntr_u8 == 0U ) &&
                ( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ].PortState_en != ETHIF_SWTPORT_STATE_DOWN ) )
            {
                lDownFlag_b = FALSE;
                break;
            }
        }
    }

    return lDownFlag_b;
}
#endif

/**
 ******************************************************************************************************************
 * \module description
 * Activates an EthIfSwitch or an EthIfSwitchPortgroup of type control or standalone(unreferenced).
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: PortGroup mode could not be changed}
 *
 ******************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
Std_ReturnType EthIf_Prv_SetSwitchOrPGModeActive( const uint8 * HostUplinkPortTable_pcau8,
                                                  const uint8 * StdPortTable_pcau8,
                                                  uint8 SizeHostUplinkPortTable_u8,
                                                  uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    uint8                                       lLoopIdx_u8;
    Std_ReturnType                              lStdRetVal_en;
    boolean                                     lPortActivationSuccessful_b;
    const EthIf_StaticSwitch_tst*               lStaticSwitch_pcst;
    boolean                                     lPortsStateMatch_b;
    EthIf_DynamicSwtPort_tst*                   lDynamicSwtPort_pst;

    /* Initialize local variable */
    lStdRetVal_en               = E_NOT_OK;

    /* Check if any one port of the switch/PG is in state ETHIF_SWTPORT_STATE_WAIT_DOWN. */
    lPortsStateMatch_b = EthIf_Prv_CheckSwitchOrPGPortInState( HostUplinkPortTable_pcau8,
                                                               StdPortTable_pcau8,
                                                               SizeHostUplinkPortTable_u8,
                                                               SizeStdPortTable_u8,
                                                               ETHIF_SWTPORT_STATE_WAIT_DOWN );

    /* If no port is in state WAIT_DOWN, request ACTIVE to all the ports which are in state DOWN. */
    if( lPortsStateMatch_b == FALSE )
    {
        /* -------------------------------------- */
        /* Activate host and uplink ports         */
        /* -------------------------------------- */

        lPortActivationSuccessful_b = TRUE;

        /* Loop through host and uplink ports */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8 ; lLoopIdx_u8++ )
        {
            lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[HostUplinkPortTable_pcau8[lLoopIdx_u8]];

            /* Check if the port is not activated */
            if( lDynamicSwtPort_pst->PortState_en == ETHIF_SWTPORT_STATE_DOWN )
            {
                /* Fetch the Static Switch structure related to current standard port */
                lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lDynamicSwtPort_pst->RefSwitchTableIdx_cu8];

                /* Set the port to active */
                lStdRetVal_en = EthSwt_SetSwitchPortMode( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                          lDynamicSwtPort_pst->EthSwtPortIdx_cu8,
                                                          ETHTRCV_MODE_ACTIVE );

                /* If port activation request was accepted. */
                if( lStdRetVal_en == E_OK )
                {
                    /* Set the port state to ACTIVE */
                    lDynamicSwtPort_pst->PortState_en = ETHIF_SWTPORT_STATE_WAIT_ACTIVE;

                }
                else
                {
                    /* If at least one host/uplink port activation failed, abort further activation of ports */
                    lPortActivationSuccessful_b = FALSE;
                    break;
                }
            }

        }

        /* -------------------------------------- */
        /* Activate standard ports                */
        /* -------------------------------------- */
        /* If all host and uplink port are already in state ACTIVE/WAIT_ACTIVE or set to state WAIT_ACTIVE */
        if( lPortActivationSuccessful_b != FALSE )
        {
            /* Initialize the flag to FALSE */
            lPortActivationSuccessful_b = FALSE;

            /* Loop through all standard ports */
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
            {

                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[StdPortTable_pcau8[lLoopIdx_u8]];

                /* Check if the port is not activated */
                if( lDynamicSwtPort_pst->PortState_en == ETHIF_SWTPORT_STATE_DOWN )
                {
                    /* Fetch the Static Switch structure related to current standard port */
                    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lDynamicSwtPort_pst->RefSwitchTableIdx_cu8];

                    /* Set the port to active */
                    lStdRetVal_en = EthSwt_SetSwitchPortMode( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                              lDynamicSwtPort_pst->EthSwtPortIdx_cu8,
                                                              ETHTRCV_MODE_ACTIVE );

                    /* If port activation request was accepted. */
                    if( lStdRetVal_en == E_OK )
                    {
                        /* Set the port active status to true */
                        lDynamicSwtPort_pst->PortState_en = ETHIF_SWTPORT_STATE_WAIT_ACTIVE;

                        /* If the request was accepted for atleast one port, set the flag to TRUE */
                        lPortActivationSuccessful_b = TRUE;
                    }
                }

                /* If the port is already activated */
                else /* If PortState_en is WAIT_ACTIVE or ACTIVE*/
                {
                    /* Set flag to TRUE if at least one standard port is already in state WAIT_ACTIVE or ACTIVE */
                    lPortActivationSuccessful_b = TRUE;
                }
            }
        }

        /* If host port, all uplink ports (if present) and at least one standard port was successfully requested for active, set the return value to E_OK. */
        if( lPortActivationSuccessful_b != FALSE )
        {
            /* -------------------------------------- */
            /* ActiveCntr incrementation              */
            /* -------------------------------------- */

            /* Loop through host and uplink ports */
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
            {
                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[HostUplinkPortTable_pcau8[lLoopIdx_u8]];

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
                /* Clear the port switch off delay timer, if it was previously started */
                lDynamicSwtPort_pst->PortOffDelay_u32 = 0UL;
#endif

                /* Increment the port active count */
                lDynamicSwtPort_pst->ActiveCntr_u8++;
            }

            /* Loop through all standard ports */
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
            {
                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[StdPortTable_pcau8[lLoopIdx_u8]];

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
                /* Clear the port switch off delay timer, if it was previously started */
                lDynamicSwtPort_pst->PortOffDelay_u32 = 0UL;
#endif

                /* Increment the port active count */
                lDynamicSwtPort_pst->ActiveCntr_u8++;
            }

            lStdRetVal_en = E_OK;
        }
        else
        {
            /* Set return value to E_NOT_OK. */
            lStdRetVal_en = E_NOT_OK;
        }
    }

    return lStdRetVal_en;
}
#endif


/**
 *********************************************************************************************************************
 * \module description
 * Deactivates an EthIfSwitch or an EthIfSwitchPortgroup of type control or standalone(unreferenced).
 *
 * Parameter In:
 * \param HostUplinkPortTable_pcau8    Pointer to the host and uplink ports table of the Switch/SwitchPortGroup
 * \param StdPortTable_pcau8           Pointer to the standard ports table of the Switch/SwitchPortGroup
 * \param SizeHostUplinkPortTable_u8   Total number of host and uplink ports of the Switch/SwitchPortGroup
 * \param SizeStdPortTable_u8          Number of standard ports of the Switch/SwitchPortGroup
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: PortGroup mode could not be changed}
 *
 *********************************************************************************************************************
 */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
Std_ReturnType EthIf_Prv_SetSwitchOrPGModeDown( const uint8 * HostUplinkPortTable_pcau8,
                                                const uint8 * StdPortTable_pcau8,
                                                uint8 SizeHostUplinkPortTable_u8,
                                                uint8 SizeStdPortTable_u8 )
{
    /* Declare local variables */
    uint8                                       lLoopIdx_u8;
    Std_ReturnType                              lStdRetVal_en;
    boolean                                     lPortModeChangeFailed_b;
    const EthIf_StaticSwitch_tst*               lStaticSwitch_pcst;
    EthIf_DynamicSwtPort_tst*                   lDynamicSwtPort_pst;
    boolean                                     lPortsStateMatch_b;

    /* Initialize local variable */
    lStdRetVal_en = E_NOT_OK;

    /* Check if any one port of the switch/PG is in state ETHIF_SWTPORT_STATE_WAIT_ACTIVE. */
    lPortsStateMatch_b = EthIf_Prv_CheckSwitchOrPGPortInState( HostUplinkPortTable_pcau8,
                                                               StdPortTable_pcau8,
                                                               SizeHostUplinkPortTable_u8,
                                                               SizeStdPortTable_u8,
                                                               ETHIF_SWTPORT_STATE_WAIT_ACTIVE );

    /* If no port is in state WAIT_ACTIVE, request DOWN to all the ports which are in state ACTIVE. */
    if( lPortsStateMatch_b == FALSE )
    {
        /* Initialize Port Mode change error flag to FALSE */
        lPortModeChangeFailed_b = FALSE;

        /* -------------------------------------- */
        /* Deactivate standard ports              */
        /* -------------------------------------- */

        /* Loop through all standard ports */
        for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
        {
            lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ];

            /* If the port state is ACTIVE and there is no other switch or PG active for this port, switch off the port */
            if( ( lDynamicSwtPort_pst->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE ) &&
                ( lDynamicSwtPort_pst->ActiveCntr_u8 == 1U ) )
            {
                /* Fetch the Static Switch structure related to current standard port */
                lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[ lDynamicSwtPort_pst->RefSwitchTableIdx_cu8 ];

                /* Set the port to DOWN. */
                lStdRetVal_en = EthSwt_SetSwitchPortMode( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                          lDynamicSwtPort_pst->EthSwtPortIdx_cu8,
                                                          ETHTRCV_MODE_DOWN );

                if( lStdRetVal_en == E_OK )
                {
                    /* Set the port status to WAIT_DOWN. */
                    lDynamicSwtPort_pst->PortState_en = ETHIF_SWTPORT_STATE_WAIT_DOWN;
                }
                else
                {
                    /* If deactivation of any of the port fails, abort further deactivation. */
                    lPortModeChangeFailed_b = TRUE;
                    break;
                }
            }
        }

        /* -------------------------------------- */
        /* Deactivate host and uplink port        */
        /* -------------------------------------- */

        /* If all standard ports are requested to DOWN . */
        if( lPortModeChangeFailed_b == FALSE )
        {
            /* Loop through all host and uplink ports. Here ports deactivated in reverse order of the list to follow the exact reverse order of activation so that consistency is retained.*/
            /* The ports are arranged in the order - host port, uplink port of master switch and uplink port of the slave switch */
            for( lLoopIdx_u8 = SizeHostUplinkPortTable_u8; lLoopIdx_u8 > 0; lLoopIdx_u8-- )
            {
                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8 - 1U] ];

                /* If the port state is ACTIVE and there is no other switch or PG active for this port, switch off the port */
                if( ( lDynamicSwtPort_pst->PortState_en == ETHIF_SWTPORT_STATE_ACTIVE ) &&
                    ( lDynamicSwtPort_pst->ActiveCntr_u8 == 1U ) )
                {
                    /* Fetch the Static Switch structure related to current standard port */
                    lStaticSwitch_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[ lDynamicSwtPort_pst->RefSwitchTableIdx_cu8 ];

                    /* Set the port to DOWN. */
                    lStdRetVal_en = EthSwt_SetSwitchPortMode( lStaticSwitch_pcst->EthSwtIdx_cu8,
                                                              lDynamicSwtPort_pst->EthSwtPortIdx_cu8,
                                                              ETHTRCV_MODE_DOWN );

                    if( lStdRetVal_en == E_OK )
                    {
                        /* Set the port status to WAIT_DOWN. */
                        lDynamicSwtPort_pst->PortState_en = ETHIF_SWTPORT_STATE_WAIT_DOWN;
                    }
                    else
                    {
                        /* If deactivation of any of the port fails, abort further deactivation. */
                        lPortModeChangeFailed_b = TRUE;
                        break;
                    }
                }
            }
        }

        /* If all the ports were successfully requested for down, set the return value to E_OK. */
        if( lPortModeChangeFailed_b == FALSE )
        {
            /* Loop through all host and uplink ports and decrement the active counter value by 1 */
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeHostUplinkPortTable_u8; lLoopIdx_u8++ )
            {
                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ HostUplinkPortTable_pcau8[lLoopIdx_u8] ];
                lDynamicSwtPort_pst->ActiveCntr_u8 = (lDynamicSwtPort_pst->ActiveCntr_u8 > 0U) ? (lDynamicSwtPort_pst->ActiveCntr_u8 - 1U) : (0U);
            }

            /* Loop through all standard ports and decrement the active counter value by 1 */
            for( lLoopIdx_u8 = 0; lLoopIdx_u8 < SizeStdPortTable_u8; lLoopIdx_u8++ )
            {
                lDynamicSwtPort_pst = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ StdPortTable_pcau8[lLoopIdx_u8] ];
                lDynamicSwtPort_pst->ActiveCntr_u8 = (lDynamicSwtPort_pst->ActiveCntr_u8 > 0U) ? (lDynamicSwtPort_pst->ActiveCntr_u8 - 1U) : (0U);
            }

            lStdRetVal_en = E_OK;
        }
        else
        {
            /* Set return value to E_NOT_OK. */
            lStdRetVal_en = E_NOT_OK;
        }
    }

    return lStdRetVal_en;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
