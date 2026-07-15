

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"
#include "EthIf_Cbk.h"
#include "EthSM_Cbk.h"
#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
/* Flag to indicate a link state change for at least one switch port */
boolean EthIf_SwitchPortLinkStateChanged_b = FALSE;
#endif

#define ETHIF_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

static Std_ReturnType EthIf_Prv_GetEthIfCtrlIdx( uint16 VlanId_u16,
                                                 const EthIf_DynamicPhysCtrl_tst * CbkDynamicPhysCtrl_pcst,
                                                 uint8 * EthIfCtrlIdx_pu8 );

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

LOCAL_INLINE void EthIf_Prv_IncMeasurementData( uint32 * MeasurementData_pu32 );

#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \module description
 * ETHIF085: Handles a received frame received by the indexed controller
 * \par Service ID 0x10, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param FrameType     Frame type of received Ethernet frame
 * \param IsBroadcast   parameter to indicate a broadcast frame
 * \param PhysAddrPtr   Pointer to Physical source address (MAC address in network byte order) of received Ethernet frame
 * \param DataPtr       Payload of the received frame
 * \param LenByte       Length of the received frame bytes
 *
 * \return              None
 *
 ***************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: Pointer to variable is defined in AUTOSAR for PhysAddrPtr. Deviation is not critical as it has no functional impact. */
/* HIS METRIC PATH VIOLATION IN EthIf_RxIndication: The number of path is increased due to the consecutive if blocks which are required to extract VLAN ID, extract EtherType, identify FrameOwner, identify EthIfCtrl and call UL RxIndication. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
void EthIf_RxIndication( uint8 CtrlIdx,
                         Eth_FrameType FrameType,
                         boolean IsBroadcast,
                         uint8 * PhysAddrPtr,
                         Eth_DataType * DataPtr,
                         uint16 LenByte )
{
    /* Declare local variables */
    uint8*                          lBuf_pu8;
    uint16                          lLenByte_u16;
    uint16                          lEthType_u16;
    uint16                          lVlanId_u16;
    EthIf_DynamicPhysCtrl_tst*      lCbkDynamicPhysCtrl_pst;
    uint8                           lEthIfCtrlNr_u8;
    uint8                           lEthIfFrameOwner_u8;
    boolean                         lULRxIndicationAllowed_b;
    Std_ReturnType                  lRetVal_en;

    /* Initialize local variables. */
    lEthIfCtrlNr_u8               = 0xFF;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_CBK_RXINDICATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_CBK_RXINDICATION,
                                  ETHIF_E_INV_CTRL_IDX)

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( ((DataPtr == NULL_PTR) || (PhysAddrPtr == NULL_PTR) ), ETHIF_SID_ETHIF_CBK_RXINDICATION, ETHIF_E_PARAM_POINTER )

    /* Get the DynamicPhysCtrl table entry indexed by CtrlIdx */
    /* (CtrlIdx is the EthIfPhysControllerIdx which is a zero based index representing the EthCtrl among all the EthDriver) */
    /* (each EthCtrl is referenced uniquely by one EthIfPhysController) */
    lCbkDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx];

    /* -------------------------------------- */
    /* Extract VLAN Id and EtherType          */
    /* -------------------------------------- */

    /* Initialize buffer pointer and length */
    lBuf_pu8 = (uint8*) DataPtr;
    lLenByte_u16 = LenByte;

    /* Check Ethernet type to check is Frame is of VLAN Type */
    if( ETHIF_FRAMETYP_VLAN == FrameType )
    {
        /* Extract VLAN id */
        lVlanId_u16 = ( ( (uint16)(lBuf_pu8[ETHIF_VLANTAG_BYTE1] & 0x0FU) ) << 8U );
        lVlanId_u16 |=  ( lBuf_pu8[ETHIF_VLANTAG_BYTE2] );

        /* Extract EthernetType */
        lEthType_u16 = ( ( (uint16)(lBuf_pu8[ETHIF_ETHTYPE_IN_VLAN_BYTE1]) ) << 8U );
        lEthType_u16 |= ( lBuf_pu8[ETHIF_ETHTYPE_IN_VLAN_BYTE2] );

        /* Increment the buffer pointer by 4, so that buffer pointer points to IP header */
        lBuf_pu8 += ETHIF_VLANTAG_LENGTH;
        lLenByte_u16 -= ETHIF_VLANTAG_LENGTH;
    }
    else
    {
        lVlanId_u16  = 0xFFFFU;
        lEthType_u16 = FrameType;
    }

    /* If EthType is a AVB pass whole DataPtr */
    if( ETHIF_FRAMETYP_AVB == lEthType_u16 )
    {
        if( lVlanId_u16 == 0xFFFFU )
        {
            lBuf_pu8     -= ETHIF_OFFSET_PAYLOAD_NOVLAN;
            lLenByte_u16 += ETHIF_OFFSET_PAYLOAD_NOVLAN;
        }
        else
        {
            lBuf_pu8     -= ETHIF_OFFSET_PAYLOAD_VLAN;
            lLenByte_u16 += ETHIF_OFFSET_PAYLOAD_VLAN;
        }
    }

    /* -------------------------------------- */
    /* Identify Frame Owner                   */
    /* -------------------------------------- */

    /* Identify upper layer Rx indication to be called based on frame type field in the received frame */
    for( lEthIfFrameOwner_u8 = 0U; lEthIfFrameOwner_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrFrameOwners_cu8; lEthIfFrameOwner_u8++ )
    {
        /* If the FrameOwner config match with the EtherType of the frame */
        if( EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].FrameType_cu16 == lEthType_u16 )
        {
            break;
        }
    }

    /* If the FrameOwner is found */
    if( lEthIfFrameOwner_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrFrameOwners_cu8 )
    {
        /* Initialized UL RxIndication allowed flag to FALSE */
        lULRxIndicationAllowed_b = FALSE;

        /* Check if the PhysController state is ACTIVE */
        if( lCbkDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE )
        {
            /* If the FrameOwner has no RxIndication bypass configured, identify EthIfCtrl */
            if( EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].RxBypassEnabled_cb == FALSE )
            {
                /* -------------------------------------- */
                /* Identify EthIfCtrl                     */
                /* -------------------------------------- */
                lRetVal_en = EthIf_Prv_GetEthIfCtrlIdx( lVlanId_u16,
                                                        lCbkDynamicPhysCtrl_pst,
                                                        &lEthIfCtrlNr_u8 );

                /* Check whether a valid EthIfCtrlIdx was found */
                if( lRetVal_en == E_OK )
                {
                    /* If the EthIfCtrl state is ACTIVE */
                    if( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlNr_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE )
                    {
                        /* UL RxIndication is allowed */
                        lULRxIndicationAllowed_b = TRUE;
                    }
                }
            }
            /* If bypass is configured */
            else
            {
                /* UL RxIndication is allowed */
                lULRxIndicationAllowed_b = TRUE;
            }
        }

        /* -------------------------------------- */
        /* Call UL RxIndication                   */
        /* -------------------------------------- */

        /* If the frame is accepted to be gived to UL RxIndication */
        if( lULRxIndicationAllowed_b != FALSE )
        {
            EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].EthIfRxIndication_pfct(
                                                                    lEthIfCtrlNr_u8,    /* EthIf controller on which the frame is received */
                                                                    lEthType_u16,       /* Type of the Eth frame received */
                                                                    IsBroadcast,        /* Indicates whether the received frame is broadcast */
                                                                    PhysAddrPtr,        /* Offset to MAC address of the sending node */
                                                                    lBuf_pu8,           /* Upper layer payload start from here */
                                                                    lLenByte_u16 );     /* Length of upper layer payload */
        }
        /* Valid EthIfCtrlIdx was not found or it is in state DOWN */
        else
        {
#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
            /* Increment the counter which holds the number of dropped frames due to invalid CtrlIdx/VLAN. */
            EthIf_Prv_IncMeasurementData( &EthIf_InvalidCtrlIdxDropCnt_u32 );
#endif
        }
    }
    /* Check whether there is no match in the EtherType of the received frame or EtherType is not valid. */
    else
    {
#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
        /* Increment the counter which holds the number of dropped frames due to invalid EtherType. */
        EthIf_Prv_IncMeasurementData( &EthIf_InvalidEtherTypeDropCnt_u32 );
#endif
    }

    return;
}

/**
 ***************************************************************************************************
 * \module description
 * ETHIF091: Confirms frame transmission by the indexed controller
 * \par Service ID 0x11, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param BufIdx        Index of the transmitted buffer
 *
 * \return              None
 *
 ***************************************************************************************************
 */
 /* BSW-866 */
 /* BSW-875 */
void EthIf_TxConfirmation_Internal( uint8 CtrlIdx,
                           uint8 BufIdx,
                           const Eth_DataType * DataPtr,
                           uint16 LenByte )
/* END BSW-875 */
/* END BSW-866 */
{
    /* Declare local variables */
    const uint8*                    lBuf_pcu8;
    uint16                          lEthType_u16;
    uint16                          lVlanId_u16;
    EthIf_DynamicPhysCtrl_tst*      lCbkDynamicPhysCtrl_pst;
    uint8                           lEthIfCtrlNr_u8;
    uint8                           lEthIfFrameOwner_u8;
    Std_ReturnType                  lRetVal_en;

    /* Initialize local variable. */
    lEthIfCtrlNr_u8 = 0xFF;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_CBK_TXCONFIRMATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_CBK_TXCONFIRMATION,
                                  ETHIF_E_INV_CTRL_IDX)

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (DataPtr == NULL_PTR), ETHIF_SID_ETHIF_CBK_TXCONFIRMATION, ETHIF_E_PARAM_POINTER )

    /* Get the DynamicPhysCtrl table entry indexed by CtrlIdx */
    /* (CtrlIdx is the EthIfPhysControllerIdx which is a zero based index representing the EthCtrl among all the EthDriver) */
    /* (each EthCtrl is referenced uniquely by one EthIfPhysController) */
    lCbkDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx];

    /* -------------------------------------- */
    /* Extract VLAN Id and EtherType          */
    /* -------------------------------------- */

    /* Initialize buffer pointer and length */
    lBuf_pcu8 = (const uint8*)DataPtr;
    (void)LenByte;

    /* Extract EthernetType out of the frame */
    lEthType_u16    =( ( (uint16)lBuf_pcu8[ETHIF_ETHTYPE_BYTE1] ) << 8U ) ;
    lEthType_u16    |= ( lBuf_pcu8[ETHIF_ETHTYPE_BYTE2] );

    /* Check Ethernet type to check is Frame is of VLAN Type */
    if ( lEthType_u16 == ETHIF_FRAMETYP_VLAN )
    {
        /* Extract VLAN id out of VLAN tag */
        lVlanId_u16     =( ( (uint16)(lBuf_pcu8[ETHIF_ETHTHDR_OFFSET + ETHIF_VLANTAG_BYTE1] & 0x0FU) ) << 8U );
        lVlanId_u16     |= ( lBuf_pcu8[ETHIF_ETHTHDR_OFFSET + ETHIF_VLANTAG_BYTE2] );

        /* Extract EthernetType out of the VLAN-frame */
        lEthType_u16    = ( ( (uint16)(lBuf_pcu8[ETHIF_ETHTHDR_OFFSET + ETHIF_ETHTYPE_IN_VLAN_BYTE1]) ) << 8U );
        lEthType_u16    |= ( lBuf_pcu8[ETHIF_ETHTHDR_OFFSET + ETHIF_ETHTYPE_IN_VLAN_BYTE2] );
    }
    else
    {
        /* Extract VLAN id out of the frame */
        lVlanId_u16 = 0xFFFFU;
    }

    /* -------------------------------------- */
    /* Identify EthIfCtrl                     */
    /* -------------------------------------- */

    /* Identify EthIfCtrl based on VLAN id and EthCtrl index */
    lRetVal_en = EthIf_Prv_GetEthIfCtrlIdx( lVlanId_u16,
                                            lCbkDynamicPhysCtrl_pst,
                                            &lEthIfCtrlNr_u8 );

    /* Check whether a valid EthIfCtrlIdx was found. */
    if( lRetVal_en == E_OK )
    {
        /* If the EthIfCtrl state is not DOWN and mapped EthCtrl is ACTIVE */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlNr_u8].State_en != ETHIF_ETHIFCTRL_STATE_DOWN) &&
            (lCbkDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
        {
            /* -------------------------------------- */
            /* Call UL TxConfirmation                 */
            /* -------------------------------------- */

            /* For the transmitted Ethernet frame, check to which upper layer protocol Tx confirmation shall be given */
            /* Based on the transmitted frame's EthType, loop through the configuration to check the owner of this frame */
            for( lEthIfFrameOwner_u8 = 0; lEthIfFrameOwner_u8 < ( EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrFrameOwners_cu8 ); lEthIfFrameOwner_u8++ )
            {
                /* If the FrameOwner config match with the EtherType of the frame and if TxConfirmation callback is not NULL pointer */
                if( (EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].FrameType_cu16 == lEthType_u16) &&
                    (NULL_PTR != EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].EthIfTxConfirmation_pfct) )
                {
                    /* Call configured TxConfirmationFunction */
                    EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].EthIfTxConfirmation_pfct( lEthIfCtrlNr_u8,
                                                                                                          BufIdx );
                    break;
                }
            }
        }
    }
    else
    {
        /* Report DET : Development Error: lEthIfCtrlIdx_u8 is not valid */
        ETHIF_REPORT_ERROR_TRUE_NO_RET(  ETHIF_SID_ETHIF_CBK_TXCONFIRMATION, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}

/**
 ***************************************************************************************************
 * \module description
 * Called asynchronously when mode has been read out. Triggered by previous Eth_SetTransceiverMode call.
 * Can directly be called within the trigger functions.
 * \par Service ID 0x0f, Synchronous, Non Reentrant for the same CtrlIdx, reentrant for different
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface.
 * \param TrcvMode      Notified Ethernet transceiver mode
 *
 * \return              None
 *
 ***************************************************************************************************
 */
void EthIf_TrcvModeIndication( uint8 TrcvIdx,
                               EthTrcv_ModeType TrcvMode )
{
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
    /* Declare local variables */
    EthIf_DynamicTrcv_tst*          lCbkDynamicTrcv_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_TRCVMODEINDICATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8),
                                  ETHIF_SID_ETHIF_TRCVMODEINDICATION,
                                  ETHIF_E_INV_TRCV_IDX )

    /* Report DET : If EthIf has not requested EthTrcv for ACTIVE state and EthTrcv is giving call back EthIf_TrcvModeIndication(ETHTRCV_MODE_ACTIVE) */
    ETHIF_REPORT_ERROR_RET_VOID( ((TrcvMode == ETHTRCV_MODE_ACTIVE) && (EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].TrcvState_en != ETHIF_TRCV_STATE_WAIT_ACTIVE)),
                                 ETHIF_SID_ETHIF_TRCVMODEINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* Get the DynamicTrcv table entry indexed by TrcvIdx */
    /* (TrcvIdx is the index of the EthTrcv in context of EthTrcv and it also corresponds to EthIfTransceiver index in the context of EthIf) */
    /* (each EthTrcv is referenced uniquely by one EthIfTransceiver) */
    lCbkDynamicTrcv_pst = &EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx];

    /* Enter critical section: Trcv state is being changed/updated. No parallel write shall occur ( for example in API EthIf_Prv_SetTransceiverMode()). */
    SchM_Enter_EthIf_SetTrcvState();

    /* If the indicated Trcv mode is ETHTRCV_MODE_DOWN */
    if( TrcvMode == ETHTRCV_MODE_DOWN )
    {
        /* Store the transceiver mode and link state to DOWN. The mode will be processed in the next main function */
        lCbkDynamicTrcv_pst->TrcvState_en     = ETHIF_TRCV_STATE_DOWN;
        lCbkDynamicTrcv_pst->TrcvLinkState_en = ETHIF_LINKSTATE_DOWN;
    }
    /* If the indicated Trcv mode is ETHTRCV_MODE_ACTIVE */
    else if( TrcvMode == ETHTRCV_MODE_ACTIVE )
    {
        /* Store the transceiver mode ACTIVE. The mode will be processed in the next main function */
        lCbkDynamicTrcv_pst->TrcvState_en = ETHIF_TRCV_STATE_ACTIVE;
    }
    else
    {
        /* Do Nothing */
    }

    /* Exit critical section. */
    SchM_Exit_EthIf_SetTrcvState();

#else
    (void)TrcvIdx;
    (void)TrcvMode;
#endif

    return;
}


/**
 ***************************************************************************************************
 * \module description
 * Is called by EthSwt, if link state goes up for a particular port of the switch
 * \par Service ID 0x52, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param SwitchIdx     Index of the switch within the context of the Ethernet Switch Driver
 * \param PortIdx       Index of the port at the addressed switch Parameters
 *
 * \return              None
 *
 ***************************************************************************************************
 */
#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
void EthIf_SwitchLinkUpIndication( uint8 SwitchIdx,
                                   uint8 PortIdx )
{
    /* Declare local variables */
    EthIf_DynamicSwtPort_tst*    lDynamicSwtPort_past;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHLINKUPINDICATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8),
                                 ETHIF_SID_ETHIF_SWITCHLINKUPINDICATION,
                                 ETHIF_E_INV_SWT_IDX )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( ((SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) &&
                                  (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8)),
                                 ETHIF_SID_ETHIF_SWITCHLINKUPINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* Fetch the EthIfSwtPort array of structures from SwitchIdx index */
    lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8];

    /* If the actual link state of the port is different from ACTIVE, that means that the port link state has changed */
    if( lDynamicSwtPort_past[PortIdx].PortLinkState_en != ETHIF_LINKSTATE_ACTIVE )
    {
        /* Store the port link state indicated. The link state accumulation will be processed in the next main function */
        lDynamicSwtPort_past[PortIdx].PortLinkState_en = ETHIF_LINKSTATE_ACTIVE;

        /* Set a flag to TRUE to remember that link state has changed for one port of the switch */
        EthIf_SwitchPortLinkStateChanged_b = TRUE;
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Is called by EthSwt, if link state goes down for a particular port of the switch
 * \par Service ID 0x53, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param SwitchIdx     Index of the switch within the context of the Ethernet Switch Driver
 * \param PortIdx       Index of the port at the addressed switch Parameters
 *
 * \return              None
 *
 ***************************************************************************************************
 */
#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
void EthIf_SwitchLinkDownIndication( uint8 SwitchIdx,
                                     uint8 PortIdx  )
{
    /* Declare local variables */
    EthIf_DynamicSwtPort_tst*    lDynamicSwtPort_past;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHLINKDOWNINDICATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8),
                                 ETHIF_SID_ETHIF_SWITCHLINKDOWNINDICATION,
                                 ETHIF_E_INV_SWT_IDX )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( ((SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) &&
                                  (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8)),
                                 ETHIF_SID_ETHIF_SWITCHLINKDOWNINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* Fetch the EthIfSwtPort array of structures from SwitchIdx index */
    lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8];

    /* If the actual link state of the port is different from DOWN, that means that the port link state has changed */
    if( lDynamicSwtPort_past[PortIdx].PortLinkState_en != ETHIF_LINKSTATE_DOWN )
    {
        /* Store the port link state indicated. The link state accumulation will be processed in the next main function */
        lDynamicSwtPort_past[PortIdx].PortLinkState_en = ETHIF_LINKSTATE_DOWN;

        /* Set a flag to TRUE to remember that link state has changed for one port of the switch */
        EthIf_SwitchPortLinkStateChanged_b = TRUE;
    }

    return;
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * Is called by EthSwt, if the port mode changes for a particular port of the switch
 * \par Service ID 0x46, Asynchronous, Non Reentrant
 *
 * Parameter In:
 * \param SwitchIdx     Index of the switch within the context of the Ethernet Switch Driver
 * \param PortIdx       Index of the port at the addressed switch Parameters
 * \param PortMode      Notified Ethernet Switch port mode
 *
 * \return              None
 *
 ***************************************************************************************************
 */
void EthIf_SwitchPortModeIndication( uint8 SwitchIdx,
                                     uint8 SwitchPortIdx,
                                     EthTrcv_ModeType PortMode )
{
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Declare local variables */
    EthIf_DynamicSwtPort_tst*    lDynamicSwtPort_past;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8),
                                 ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION,
                                 ETHIF_E_INV_SWT_IDX )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( ((SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) &&
                                  (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8)),
                                 ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* Fetch the EthIfSwtPort array of structures from SwitchIdx index */
    lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8];

    /* Check if the indicated mode is Active and the state of the port is not Wait_Active. */
    /* Report DET : Development Error: Indicated switch port mode is invalid. */
    ETHIF_REPORT_ERROR_RET_VOID( ((PortMode == ETHTRCV_MODE_ACTIVE) && (lDynamicSwtPort_past[SwitchPortIdx].PortState_en != ETHIF_SWTPORT_STATE_WAIT_ACTIVE)),
                                 ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* Check if the indicated mode is Down and the state of the port is not Wait_Active or Wait_Down. */
    /* Hint: DOWN indication can happen in Wait_Active when the switch is not able to activate the port (HW not accessible). In that case, EthSwt reports a */
    /* Dem error and it is in the responsability of the application to react on this error (restart EthSM network, restart ECU ...). EthIf does not take any action (treated as non recoverable error). */
    /* DOWN indication from state ACTIVE is not possible with currently supported switch HW */
    /* Retry mechanism and error transition in state machine will be implemented part of CR137125 in order to try to recoved Dem error */
    /* Report DET : Development Error: Indicated switch port mode is invalid. */
    ETHIF_REPORT_ERROR_RET_VOID( ((PortMode == ETHTRCV_MODE_DOWN) &&
                                 ((lDynamicSwtPort_past[SwitchPortIdx].PortState_en != ETHIF_SWTPORT_STATE_WAIT_DOWN) && (lDynamicSwtPort_past[SwitchPortIdx].PortState_en != ETHIF_SWTPORT_STATE_WAIT_ACTIVE)) ),
                                 ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION,
                                 ETHIF_E_INV_PARAM )

    /* If the indicated port mode is ETHTRCV_MODE_DOWN store the state of the port as ETHIF_SWTPORT_STATE_DOWN. */
    if( PortMode == ETHTRCV_MODE_DOWN )
    {
        /* Set the state of the port as ETHIF_SWTPORT_STATE_DOWN. */
        lDynamicSwtPort_past[SwitchPortIdx].PortState_en = ETHIF_SWTPORT_STATE_DOWN;
        lDynamicSwtPort_past[SwitchPortIdx].PortLinkState_en = ETHIF_LINKSTATE_DOWN;
    }
    /* Set the state of the port as ETHIF_SWTPORT_STATE_ACTIVE. */
    else if( PortMode == ETHTRCV_MODE_ACTIVE )
    {
        lDynamicSwtPort_past[SwitchPortIdx].PortState_en = ETHIF_SWTPORT_STATE_ACTIVE;
    }
    else
    {
        /* Do Nothing */
    }
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)PortMode;
#endif

    return;
}

/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/**
 **********************************************************************************************************************
 * \module description
 *  Fetches the EthIfCtrlIdx for the corresponding Vlan ID and EthCtrlIdx.
 *
 * Parameter In:
 * \param VlanId_u16                  Vlan ID of the received frame for which EthIfCtrlIdx needs to be fetched.
 * \param CbkDynamicPhysCtrl_pcst     Holds the EthPhysCtrl dynamic structure corresponding to the EthCtrlIdx.
 *
 *  * Parameter Out:
 * \param EthIfCtrlIdx_pu8            Stores the EthIfCtrlIdx after matching the VlanId and CbkDynamicPhysCtrl_pcst.
 *
* \return  Std_ReturnType {E_OK: Valid EthIfCtrlIdx was found; E_NOT_OK: Valid EthIfCtrlIdx was not found }
 *
 **********************************************************************************************************************
 */
static Std_ReturnType EthIf_Prv_GetEthIfCtrlIdx( uint16 VlanId_u16,
                                                 const EthIf_DynamicPhysCtrl_tst * CbkDynamicPhysCtrl_pcst,
                                                 uint8 * EthIfCtrlIdx_pu8 )
{
    uint8                           lEthIfCtrlNr_u8;
    EthIf_DynamicPhysCtrl_tst*      lLoopDynamicPhysCtrl_pst;
    Std_ReturnType                  lStdRetVal_en;

    /*Initialize local variable. */
    lStdRetVal_en = E_NOT_OK;

    /* Identify EthIfCtrl based on VLAN id and EthCtrl index */
    for( lEthIfCtrlNr_u8 = 0U; lEthIfCtrlNr_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lEthIfCtrlNr_u8++ )
    {
        /* Fetch the PhysCtrl structure from current EthIfCtrl index. */
        lLoopDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlNr_u8].RefPhysCtrlTableIdx_cu8];

        if ( ( VlanId_u16 == EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlNr_u8].VlanId_cu16) &&
             ( lLoopDynamicPhysCtrl_pst == CbkDynamicPhysCtrl_pcst) )
        {
            /*Store the EthIfCtrlIdx in the output parameter. */
            *EthIfCtrlIdx_pu8 = lEthIfCtrlNr_u8;

            /* Set return value to E_OK. */
            lStdRetVal_en = E_OK;

            break;
        }
    }

    return lStdRetVal_en;
}


#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/**
 **********************************************************************************************************************
 * \module description
 *  Increments the measurement data passed as inout parameter if it has not exceeded ETHIF_UINT32_MAX.
 *
 * Parameter In:
 * None
 *
 * Parameter Out:
 * None
 *
 *  Parameter Inout:
 * \param MeasurementData_pu32            Pointer to the measurement data which shall be incrmented.
 *
* \return  None
 *
 **********************************************************************************************************************
 */
LOCAL_INLINE void EthIf_Prv_IncMeasurementData( uint32 * MeasurementData_pu32 )
{
    /* Enter critical section: Measurement data is getting compared with maximum value and then incremented. The value shall not be modified in a different execution context. */
    /* The operation is made as a critical operation. */
    SchM_Enter_EthIf_UpdateMeasurementData();

    /* [SWS_EthIf_00313]: All measurement data which counts data shall not overrun. */
    if(( *MeasurementData_pu32 ) < ETHIF_UINT32_MAX )
    {
        /* Increment the counter. */
        (*MeasurementData_pu32)++;
    }

    /* Exit critical section. */
    SchM_Exit_EthIf_UpdateMeasurementData();
}

#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

/*
 ***************************************************************************************************
 * ZeroCopy Interface functions
 ***************************************************************************************************
 */
#define ETHIF_START_SEC_CODE_FAST
#include "EthIf_MemMap.h"
/**
 ***************************************************************************************************
 * \module description
 * Confirms ZeroCopy frame transmission by the indexed controller
 * \par Service ID 0x60, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param TxId_u16      Unique identifier in the context of EthDriver representing the confirmed ZeroCopy transmission
 * \param HeaderPtr_pu8 Pointer to the transmitted Ethernet header buffer related to the ZeroCopy transmission with TxId given in argument
 * \param HeaderLen_u16 Length of the Ethernet header buffer
 *
 * \return              None
 *
 ***************************************************************************************************
 */
#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON )
void EthIf_ZeroCopyTxConfirmation( uint8 CtrlIdx_u8,
                                   uint16 TxId_u16,
                                   const Eth_DataType * HeaderPtr_pcu8,
                                   uint16 HeaderLen_u16 )
{
    /* Declare local variables */
    const uint8*                    lBuf_pu8;
    uint16                          lEthType_u16;
    uint16                          lVlanId_u16;
    EthIf_DynamicPhysCtrl_tst*      lCbkDynamicPhysCtrl_pst;
    uint8                           lEthIfCtrlNr_u8;
    uint8                           lEthIfFrameOwner_u8;
    Std_ReturnType                  lRetVal_en;
    uint32                          lTxId_u32;

    /* Initialize local variable. */
    lEthIfCtrlNr_u8 = 0xFF;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_CBK_ZEROCOPYTXCONFIRMATION, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx_u8 >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_CBK_ZEROCOPYTXCONFIRMATION,
                                  ETHIF_E_INV_CTRL_IDX)

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (HeaderPtr_pcu8 == NULL_PTR), ETHIF_SID_ETHIF_CBK_ZEROCOPYTXCONFIRMATION, ETHIF_E_PARAM_POINTER )

    /* Get the DynamicPhysCtrl table entry indexed by CtrlIdx */
    /* (CtrlIdx is the index of the EthCtrl in context of EthCtrl and it also corresponds to EthIfPhysController index in the context of EthIf) */
    /* (each EthCtrl is referenced uniquely by one EthIfPhysController) */
    lCbkDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx_u8];

    /* Construct TxId_u32 based on TxId provided by Driver */
    lTxId_u32 = (uint32)( ((uint32)(CtrlIdx_u8) << 16U) | (uint32)(TxId_u16) );


    /* -------------------------------------- */
    /* Extract VLAN Id and EtherType          */
    /* -------------------------------------- */

    /* Initialize buffer pointer and length */
    lBuf_pu8 = (const uint8*)HeaderPtr_pcu8;
    (void)HeaderLen_u16;

    /* Extract EthernetType out of the frame */
    lEthType_u16    =( ( (uint16)lBuf_pu8[ETHIF_ETHTYPE_BYTE1] ) << 8U ) ;
    lEthType_u16    |= ( lBuf_pu8[ETHIF_ETHTYPE_BYTE2] );

    /* Check Ethernet type to check is Frame is of VLAN Type */
    if ( lEthType_u16 == ETHIF_FRAMETYP_VLAN )
    {
        /* Extract VLAN id out of VLAN tag */
        lVlanId_u16     =( ( (uint16)(lBuf_pu8[ETHIF_ETHTHDR_OFFSET + ETHIF_VLANTAG_BYTE1] & 0x0FU) ) << 8U );
        lVlanId_u16     |= ( lBuf_pu8[ETHIF_ETHTHDR_OFFSET + ETHIF_VLANTAG_BYTE2] );

        /* Extract EthernetType out of the VLAN-frame */
        lEthType_u16    = ( ( (uint16)(lBuf_pu8[ETHIF_ETHTHDR_OFFSET + ETHIF_ETHTYPE_IN_VLAN_BYTE1]) ) << 8U );
        lEthType_u16    |= ( lBuf_pu8[ETHIF_ETHTHDR_OFFSET + ETHIF_ETHTYPE_IN_VLAN_BYTE2] );
    }
    else
    {
        /* Extract VLAN id out of the frame */
        lVlanId_u16 = 0xFFFFU;
    }

    /* -------------------------------------- */
    /* Identify EthIfCtrl                     */
    /* -------------------------------------- */

    /* Identify EthIfCtrl based on VLAN id and EthCtrl index */
    lRetVal_en = EthIf_Prv_GetEthIfCtrlIdx( lVlanId_u16,
                                            lCbkDynamicPhysCtrl_pst,
                                            &lEthIfCtrlNr_u8 );

    /* Check whether a valid EthIfCtrlIdx was found. */
    if( lRetVal_en == E_OK )
    {
        /* If the EthIfCtrl state is not DOWN and mapped EthCtrl is ACTIVE */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlNr_u8].State_en != ETHIF_ETHIFCTRL_STATE_DOWN) &&
            (lCbkDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
        {
            /* -------------------------------------- */
            /* Call UL TxConfirmation                 */
            /* -------------------------------------- */

            /* For the transmitted Ethernet frame, check to which upper layer protocol Tx confirmation shall be given */
            /* Based on the transmitted frame's EthType, loop through the configuration to check the owner of this frame */
            for( lEthIfFrameOwner_u8 = 0; lEthIfFrameOwner_u8 < ( EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrFrameOwners_cu8 ); lEthIfFrameOwner_u8++ )
            {
                /* If the FrameOwner config match with the EtherType of the frame and if TxConfirmation callback is not NULL pointer */
                if( (EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].FrameType_cu16 == lEthType_u16) &&
                    (NULL_PTR != EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].EthIfZeroCopyTxConfirmation_pfct) )
                {
                    /* Call configured TxConfirmationFunction */
                    EthIf_CfgPtr_pco->EthIf_FrameOwnerConfigTable_cpcst[lEthIfFrameOwner_u8].EthIfZeroCopyTxConfirmation_pfct( lEthIfCtrlNr_u8,
                                                                                                                               lTxId_u32 );
                    break;
                }
            }
        }
    }
    else
    {
        /* Report DET : Development Error: lEthIfCtrlIdx_u8 is not valid */
        ETHIF_REPORT_ERROR_TRUE_NO_RET(  ETHIF_SID_ETHIF_CBK_ZEROCOPYTXCONFIRMATION, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}
#endif
#define ETHIF_STOP_SEC_CODE_FAST
#include "EthIf_MemMap.h"
#endif /* ETHIF_CONFIGURED */
