

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
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \moduledescription
 * Returns a time value out of the HW registers according to the capability of the HW.
 * Is the HW resolution is lower than the Eth_TimeStampType resolution resp. range,
 * than an the remaining bits will be filled with 0.
 * \par Service ID 0x22, Synchronous, Non-reentrant for the same CtrlIdx, reentrant for different CtrlIdx
 *
 * Parameter In:
 * \param CtrlIdx           Index of EthIf controller.
 *
 * Parameters Out:
 * \param timeQualPtr       Quality of HW time stamp, e.g. based on current drift.
 * \param timeStampPtr      Current time stamp
 *
 * \return                  Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the current time}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
Std_ReturnType EthIf_GetCurrentTime( uint8 CtrlIdx,
                                     Eth_TimeStampQualType * timeQualPtr,
                                     Eth_TimeStampType * timeStampPtr )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    Std_ReturnType                      lStdRetVal_en;

    /* DET Checks */
    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( ( EthIf_InitStatus_en != ETHCTRL_STATE_INIT ), ETHIF_SID_ETHIF_GETCURRENTTIME, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8), ETHIF_SID_ETHIF_GETCURRENTTIME,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (timeQualPtr == NULL_PTR), ETHIF_SID_ETHIF_GETCURRENTTIME, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (timeStampPtr == NULL_PTR), ETHIF_SID_ETHIF_GETCURRENTTIME, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialize the local variable*/
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetCurrentTime_pfct != NULL_PTR )
    {
        /* Call Eth_<vi>_<ai>_GetCurrentTime() API */
        lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetCurrentTime_pfct(
                                            lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                            timeQualPtr,
                                            timeStampPtr);
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_GETCURRENTTIME, ETHIF_E_INV_CTRL_IDX )
    }

    return lStdRetVal_en;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Activates egress time stamping on a dedicated message object. Some HW does store once the egress
 * time stamp marker and some HW needs it always before transmission. There will be no "disable"
 * functionality, due to the fact, that the message type is always "time stamped" by network design.
 * \par Service ID 0x23, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of EthIf controller.
 * \param BufIdx        Index of the message buffer, where Application expects egress time stamping
 *
 * \return              None
 *
 ***************************************************************************************************
 */
#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
void EthIf_EnableEgressTimeStamp( uint8 CtrlIdx,
                                  uint8 BufIdx  )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*       lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_ENABLE_EGRESS_TIMESTAMP, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                 ETHIF_SID_ETHIF_ENABLE_EGRESS_TIMESTAMP,
                                 ETHIF_E_INV_CTRL_IDX )

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthEnableEgressTimeStamp_pfct != NULL_PTR )
    {
        /* Call Eth_<vi>_<ai>_EnableEgressTimeStamp() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthEnableEgressTimeStamp_pfct(
                                   lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                   BufIdx );
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_ENABLE_EGRESS_TIMESTAMP, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Reads back the egress time stamp on a dedicated message object.
 * EthIf_GetEgressTimeStamp() API must be called by the UL of EthIf within the UL_TxConfirmation() callback function
 * \par Service ID 0x24, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of EthIf controller.
 * \param BufIdx        Index of the message buffer, where Application expects egress time stamping
 *
 * Parameters Out:
 * \param timeQualPtr   Quality of HW time stamp, e.g. based on current drift
 * \param timeStampPtr  Current time stamp
 *
 * \return              None
 *
 ***************************************************************************************************
 */
#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
void EthIf_GetEgressTimeStamp( uint8 CtrlIdx,
                               uint8 BufIdx,
                               Eth_TimeStampQualType * timeQualPtr,
                               Eth_TimeStampType * timeStampPtr )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*       lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                   ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP,
                                   ETHIF_E_INV_CTRL_IDX )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (timeQualPtr == NULL_PTR), ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP, ETHIF_E_PARAM_POINTER )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (timeStampPtr == NULL_PTR), ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP, ETHIF_E_PARAM_POINTER )

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetEgressTimeStamp_pfct != NULL_PTR )
    {
        /* Call Eth_<vi>_<ai>_GetEgressTimeStamp() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetEgressTimeStamp_pfct(
                                lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                BufIdx,
                                timeQualPtr,
                                timeStampPtr );
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \moduledescription
 * Reads back the ingress time stamp on a dedicated message object.
 * It must be called within the RxIndication() function.
 * \par Service ID 0x25, Synchronous, Non-reentrant for the same CtrlIdx, reentrant for different CtrlIdx
 *
 * Parameter In:
 * \param CtrlIdx           Index of EthIf controller.
 * \param DataPtr           Pointer to the message buffer, where Application expects ingress time stamping
 *
 * Parameters Out:
 * \param timeQualPtr       Quality of HW time stamp, e.g. based on current drift.
 * \param timeStampPtr      Current time stamp
 *
 * \return                  None
 *
 ***************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: Pointer to variable is defined in AUTOSAR for DataPtr. Deviation is not critical as it has no functional impact. */
#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
void EthIf_GetIngressTimeStamp( uint8 CtrlIdx,
                                Eth_DataType * DataPtr,
                                Eth_TimeStampQualType * timeQualPtr,
                                Eth_TimeStampType * timeStampPtr )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*       lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8), ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_INV_CTRL_IDX )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (DataPtr == NULL_PTR), ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_PARAM_POINTER )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (timeQualPtr == NULL_PTR), ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_PARAM_POINTER )

    /* check if provided pointer is a NULL pointer*/
    ETHIF_REPORT_ERROR_RET_VOID( (timeStampPtr == NULL_PTR), ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_PARAM_POINTER )

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetIngressTimeStamp_pfct != NULL_PTR )
    {
        /* Call Eth_<vi>_<ai>_GetIngressTimeStamp() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetIngressTimeStamp_pfct(
                                 lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                 DataPtr,
                                 timeQualPtr,
                                 timeStampPtr);
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \moduledescription
 * Request the MgmtObject (in this context) for the corresponding switch port.
 * It must be called within the RxIndication() function.
 * \par Service ID 0x54, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx           Index of an Ethernet Interface controller
 * \param SwitchPortIdx     Index of the switch port for which Rx Management object has to be fetched
 *
 * Parameters Out:
 * \param MgmtObjectPtr     Pointer to the management object
 *
 * \return                  Std_ReturnType {E_OK: success; E_NOT_OK: management object could not be obtained}
 *
 ***************************************************************************************************
 */
#if( ETHIF_SWITCH_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
Std_ReturnType EthIf_GetRxMgmtObject( uint8 CtrlIdx,
                                      uint8 SwitchPortIdx,
                                      EthSwt_MgmtObjectType**   MgmtObjectPtr )
{
    /* Declare local variables */
    const EthIf_StaticEthIfCtrl_tst*        lStaticEthIfCtrl_pst;
    Std_ReturnType                          lStdRetVal_en;
    uint8                                   lEthSwtIdx_u8;
    const uint8*                            lRefHostUplinkPortsTablePtr_pcu8;
    uint8                                   lNbHostUplinkPorts_u8;
    const uint8*                            lRefStdPortsTablePtr_pcu8;
    uint8                                   lNbStdPorts_u8;
    boolean                                 lPortFound_b;
    uint8                                   lLoopIdx_u8;
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;
    uint8                                   lHostPortEthIfSwtIdx_u8;
#endif

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETRXMGMTOBJECT, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_GETRXMGMTOBJECT,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK  )

    /* Initialise local variables */
    lRefHostUplinkPortsTablePtr_pcu8 = NULL_PTR;
    lRefStdPortsTablePtr_pcu8        = NULL_PTR;
    lNbHostUplinkPorts_u8            = 0U;
    lNbStdPorts_u8                   = 0U;
    lEthSwtIdx_u8                    = 0U;
    lStdRetVal_en                    = E_NOT_OK;

    /* Get pointer to EthIfCtrl identified by argument CtrlIdx */
    lStaticEthIfCtrl_pst = &EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx];

    /* -------------------------------------- */
    /* EthIfCtrl references Switch PG         */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch Port Group (of type control or link-info) */
    if( (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL)       ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) )
    {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
        /* Fetch the Static Switch PG structure */
        lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8];

        /* Verify whether switchPortGrp contains cascaded switch by checking number of Host and Uplink ports. In case of non-cascaded switchPortGrp number of Host and Uplink ports will be one */
        /* Note: EthIf_GetRxMgmtObject() is not supported for cascaded switches, because with input parameter CtrlIdx alone it is not possible */
        /* to identify to which EthSwt the input parameter SwitchPortIdx refers to. */
        /* Report DET : Development Error: PortIdx is not valid */
        ETHIF_REPORT_ERROR_RET_VALUE( ( lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8 != 1U ), ETHIF_SID_ETHIF_GETRXMGMTOBJECT, ETHIF_E_INV_PARAM, E_NOT_OK )

        /* Get EthIfSwitchIdx for the host port. Index of EthIf_DynamicSwtPort for the hostport is always stored at zeroth index of RefHostUplinkPortsTablePtr_cpacu8. */
        lHostPortEthIfSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[0] ].RefSwitchTableIdx_cu8;

        /* Read out EthSwt index from configuration */
        lEthSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lHostPortEthIfSwtIdx_u8].EthSwtIdx_cu8;

        /* Fetch ports configuration for the switch PG */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8            = lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                   = lStaticSwitchPG_pcst->NbStdPorts_cu8;

#endif
    }

    /* -------------------------------------- */
    /* EthIfCtrl references Switch            */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch */
    else if( lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH )
    {
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
        /* Read out EthSwt index from configuration */
        lEthSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].EthSwtIdx_cu8;

        /* Fetch ports configuration for the complete switch */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8            = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                   = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].NbStdPorts_cu8;

#endif
    }

    /* EthIfCtrl does not have any reference to Switch or Switch Port Group and thus EthSwt index cannot be determined */
    else
    {
        /* E_NOT_OK will be returned */
    }

    /* -------------------------------------- */
    /* Forward call                           */
    /* -------------------------------------- */
    if( (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL)       ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH) )
    {
        /* Initialize flag variables to FALSE */
        lPortFound_b = FALSE;

        /* Check if the port given in argument match with host or uplink port of the Switch/PortGroup */
        for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < lNbHostUplinkPorts_u8; lLoopIdx_u8++)
        {
            if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lRefHostUplinkPortsTablePtr_pcu8[lLoopIdx_u8] ].EthSwtPortIdx_cu8 == SwitchPortIdx )
            {
                lPortFound_b = TRUE;
                break;
            }
        }


        /* Proceed with further check of matching SwitchPortIdx in the standard port list if requested PortIdx is not present in HostUplink port list. */
        if( lPortFound_b == FALSE )
        {
            /* Check if the port given in argument match with one standard port of the Switch/PortGroup */
            for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < lNbStdPorts_u8; lLoopIdx_u8++)
            {
                if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lRefStdPortsTablePtr_pcu8[lLoopIdx_u8] ].EthSwtPortIdx_cu8 == SwitchPortIdx )
                {
                    lPortFound_b = TRUE;
                    break;
                }
            }
        }

        /* Report DET : Development Error: PortIdx is not valid */
        ETHIF_REPORT_ERROR_RET_VALUE( ( lPortFound_b == FALSE ), ETHIF_SID_ETHIF_GETRXMGMTOBJECT, ETHIF_E_INV_PARAM, E_NOT_OK )

        /* Call EthSwt API */
        lStdRetVal_en = EthSwt_GetRxMgmtObject( lEthSwtIdx_u8,
                                                SwitchPortIdx,
                                                MgmtObjectPtr );
    }

    return lStdRetVal_en;
}
#endif


/**
 ***************************************************************************************************
 * \moduledescription
 * Request the MgmtObject (in this context) for the corresponding switch port.
 * It must be called within the TxConfirmation() function.
 * \par Service ID 0x55, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx           Index of an Ethernet Interface controller
 * \param SwitchPortIdx     Index of the switch port for which Tx Management object has to be fetched
 *
 * Parameters Out:
 * \param MgmtObjectPtr     Pointer to the management object
 *
 * \return                  Std_ReturnType {E_OK: success; E_NOT_OK: management object could not be obtained}
 *
 ***************************************************************************************************
 */
#if( ETHIF_SWITCH_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
Std_ReturnType EthIf_GetTxMgmtObject( uint8 CtrlIdx,
                                      uint8 SwitchPortIdx,
                                      EthSwt_MgmtObjectType**   MgmtObjectPtr )
{
    /* Declare local variables */
    const EthIf_StaticEthIfCtrl_tst*        lStaticEthIfCtrl_pst;
    Std_ReturnType                          lStdRetVal_en;
    uint8                                   lEthSwtIdx_u8;
    const uint8*                            lRefHostUplinkPortsTablePtr_pcu8;
    uint8                                   lNbHostUplinkPorts_u8;
    const uint8*                            lRefStdPortsTablePtr_pcu8;
    uint8                                   lNbStdPorts_u8;
    boolean                                 lPortFound_b;
    uint8                                   lLoopIdx_u8;
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const EthIf_StaticSwtPortGroup_tst*     lStaticSwitchPG_pcst;
    uint8                                   lHostPortEthIfSwtIdx_u8;
#endif

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETTXMGMTOBJECT, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_GETTXMGMTOBJECT,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK  )

    /* Initialise local variables */
    lRefHostUplinkPortsTablePtr_pcu8 = NULL_PTR;
    lRefStdPortsTablePtr_pcu8        = NULL_PTR;
    lNbHostUplinkPorts_u8            = 0U;
    lNbStdPorts_u8                   = 0U;
    lEthSwtIdx_u8                    = 0U;
    lStdRetVal_en                    = E_NOT_OK;

    /* Get pointer to EthIfCtrl identified by argument CtrlIdx */
    lStaticEthIfCtrl_pst = &EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx];

    /* -------------------------------------- */
    /* EthIfCtrl references Switch            */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch */
    if( lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH )
    {
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
        /* Read out EthSwt index from configuration */
        lEthSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].EthSwtIdx_cu8;

        /* Fetch ports configuration for the complete switch */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8            = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                   = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8].NbStdPorts_cu8;
#endif
    }

    /* -------------------------------------- */
    /* EthIfCtrl references Switch PG         */
    /* -------------------------------------- */

    /* If the EthIfCtrl reference a Switch Port Group (of type control or link-info) */
    else if( (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL)       ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) )
    {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
        /* Fetch the Static Switch PG structure */
        lStaticSwitchPG_pcst = &EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lStaticEthIfCtrl_pst->RefTrcvOrSwtOrPGTableIdx_cu8];

        /* Verify whether switchPortGrp contains cascaded switch by checking number of Host and Uplink ports. In case of non-cascaded switchPortGrp number of Host and Uplink ports will be one */
        /* Note: EthIf_GetTxMgmtObject() is not supported for cascaded switches, because with input parameter CtrlIdx alone it is not possible */
        /* to identify to which EthSwt the input parameter SwitchPortIdx refers to. */
        /* Report DET : Development Error: PortIdx is not valid */
        ETHIF_REPORT_ERROR_RET_VALUE( ( lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8 != 1U ), ETHIF_SID_ETHIF_GETTXMGMTOBJECT, ETHIF_E_INV_PARAM, E_NOT_OK )

        /* Get EthIfSwitchIdx for the host port. Index of EthIf_DynamicSwtPort for the hostport is always stored at zeroth index of RefHostUplinkPortsTablePtr_cpacu8. */
        lHostPortEthIfSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8[0] ].RefSwitchTableIdx_cu8;

        /* Read out EthSwt index from configuration */
        lEthSwtIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lHostPortEthIfSwtIdx_u8].EthSwtIdx_cu8;

        /* Fetch ports configuration for the switch PG */
        lRefHostUplinkPortsTablePtr_pcu8 = (const uint8 *)lStaticSwitchPG_pcst->RefHostUplinkPortsTablePtr_cpacu8;
        lNbHostUplinkPorts_u8           = lStaticSwitchPG_pcst->NbHostUplinkPorts_cu8;
        lRefStdPortsTablePtr_pcu8        = (const uint8 *)lStaticSwitchPG_pcst->RefStdPortsTablePtr_cpacu8;
        lNbStdPorts_u8                  = lStaticSwitchPG_pcst->NbStdPorts_cu8;

#endif
    }

    /* EthIfCtrl does not have any reference to Switch or Switch Port Group and thus EthSwt index cannot be determined */
    else
    {
        /* E_NOT_OK will be returned */
    }

    /* -------------------------------------- */
    /* Forward call                           */
    /* -------------------------------------- */
    if( (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL)       ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO) ||
        (lStaticEthIfCtrl_pst->ReferenceType_cen == ETHIF_ETHIFCTRL_REF_SWITCH) )
    {
        /* Initialize flag variables to FALSE */
        lPortFound_b = FALSE;

        /* Check if the port given in argument match with host or uplink port of the Switch/PortGroup */
        for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < lNbHostUplinkPorts_u8; lLoopIdx_u8++)
        {
            if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lRefHostUplinkPortsTablePtr_pcu8[lLoopIdx_u8] ].EthSwtPortIdx_cu8 == SwitchPortIdx )
            {
                lPortFound_b = TRUE;
                break;
            }
        }

        /* Proceed with further check of matching SwitchPortIdx in the standard port list if requested PortIdx is not present in HostUplink port list. */
        if( lPortFound_b == FALSE )
        {
            /* Check if the port given in argument match with one standard port of the Switch/PortGroup */
            for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < lNbStdPorts_u8; lLoopIdx_u8++)
            {
                if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[ lRefStdPortsTablePtr_pcu8[lLoopIdx_u8] ].EthSwtPortIdx_cu8 == SwitchPortIdx )
                {
                    lPortFound_b = TRUE;
                    break;
                }
            }
        }

        /* Report DET : Development Error: PortIdx is not valid */
        ETHIF_REPORT_ERROR_RET_VALUE( ( lPortFound_b == FALSE ), ETHIF_SID_ETHIF_GETTXMGMTOBJECT, ETHIF_E_INV_PARAM, E_NOT_OK )

        /* Call EthSwt API */
        lStdRetVal_en = EthSwt_GetTxMgmtObject( lEthSwtIdx_u8,
                                                SwitchPortIdx,
                                                MgmtObjectPtr );
    }

    return lStdRetVal_en;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
