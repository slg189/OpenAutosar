

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"

#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
#include "EthTrcv.h"
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
 * \module description
 * Activates a given test mode
 * \par service ID 0x17, Synchronous, Reentrant for different TrcvIdx, Non reentrant for the same TrcvIdx
 *
 * Parameter In:
 * \param TrcvIdx   Index of the Ethernet transceiver within the context of the Ethernet Interface
 * \param Mode      Test mode to be activated
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: failed to activate the given test mode}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
Std_ReturnType EthIf_SetPhyTestMode ( uint8 TrcvIdx,
                                      EthTrcv_PhyTestModeType Mode )
{
#if (ETHIF_CFG_SET_PHYTESTMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPHYTESTMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_SETPHYTESTMODE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_SetPhyTestMode( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8,
                                            Mode );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)Mode;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Activates a given transmission mode
 * \par service ID 0x13, Synchronous, Reentrant for different TrcvIdx, Non reentrant for the same TrcvIdx
 *
 * Parameter In:
 * \param TrcvIdx   Index of the Ethernet transceiver within the context of the Ethernet Interface
 * \param Mode      Transmission mode to be activated
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: failed to activate the given transmission mode}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
Std_ReturnType EthIf_SetPhyTxMode ( uint8 TrcvIdx,
                                    EthTrcv_PhyTxModeType Mode )
{
#if(ETHIF_CFG_SET_PHYTXMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPHYTXMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_SETPHYTXMODE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_SetPhyTxMode(   EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8,
                                            Mode);

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)Mode;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Activates a given loopback mode.
 * \par service ID 0x12, Synchronous, Reentrant for different TrcvIdx, Non reentrant for the same TrcvIdx
 *
 * Parameter In:
 * \param TrcvIdx   Index of the Ethernet transceiver within the context of the Ethernet Interface
 * \param Mode      Loopback mode to be activated
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: failed to activate the given loopback mode}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
Std_ReturnType EthIf_SetPhyLoopbackMode ( uint8 TrcvIdx,
                                          EthTrcv_PhyLoopbackModeType Mode )
{
#if(ETHIF_CFG_SET_PHYLOOPBACKMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;


    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPHYLOOPBACKMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_SETPHYLOOPBACKMODE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_SetPhyLoopbackMode( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8,
                                                Mode );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)Mode;
    return E_NOT_OK;
#endif

}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the cable diagnostics result of a given transceiver.
 * \par service ID 0x14, Synchronous, Reentrant for different TrcvIdx, Non reentrant for the same TrcvIdx
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param ResultPtr     Pointer to the location where the cable diagnostics result shall be stored
 *
 * \return              Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the dianoctics result}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: ResultPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for ResultPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetCableDiagnosticsResult( uint8 TrcvIdx,
                                                EthTrcv_CableDiagResultType * ResultPtr )
{
#if(ETHIF_CFG_GET_CABLEDIAGNOSTICSRESULT_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Initialise local variables */
    lStdRetVal_en = E_NOT_OK;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETCABLEDIAGNOSTICSRESULT, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_GETCABLEDIAGNOSTICSRESULT, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ResultPtr == NULL_PTR), ETHIF_SID_ETHIF_GETCABLEDIAGNOSTICSRESULT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetCableDiagnosticsResult(  EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8 ,
                                                        ResultPtr );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)ResultPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the PHY identifier of the Ethernet Interface according to IEEE 802.3-2015 chapter 22.2.4.3.1 PHY Identifer
 * \par service ID 0x15, Synchronous, Reentrant for different TrcvIdx, Non reentrant for the same TrcvIdx
 *
 * Parameter In:
 * \param TrcvIdx           Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param OrgUniqueIdPtr    Pointer to the memory where the Organizationally Unique Identifier shall be stored
 * \param ModelNrPtr        Pointer to the memory where the Manufacturer's Model Number shall be stored
 * \param RevisionNrPtr     Pointer to the memory where the Revision Number shall be stored
 *
 * \return                  Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the PHY identifier}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: OrgUniqueIdPtr, ModelNrPtr and RevisionNrPtr are out arguments and the addressed objects are modified inside the function, pointer to variable is defined in AUTOSAR for OrgUniqueIdPtr, ModelNrPtr and RevisionNrPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetPhyIdentifier ( uint8 TrcvIdx,
                                        uint32 * OrgUniqueIdPtr,
                                        uint8 * ModelNrPtr,
                                        uint8 * RevisionNrPtr )
{

#if(ETHIF_CFG_GET_PHYIDENTIDFIER_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETPHYIDENTIFIER, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_GETPHYIDENTIFIER, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (OrgUniqueIdPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPHYIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ModelNrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPHYIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (RevisionNrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPHYIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetPhyIdentifier( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8,
                                              OrgUniqueIdPtr,
                                              ModelNrPtr,
                                              RevisionNrPtr );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)OrgUniqueIdPtr;
    (void)ModelNrPtr;
    (void)RevisionNrPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * ETHIF91056: Retrieves the signal quality of the link of the given Ethernet transceiver
 * \par Service ID 0x18, Synchronous, Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx.
 *
 * Parameter In:
 * \param TrcvIdx     Index of the transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param ResultPtr   Pointer to the memory where the signal quality shall be stored.
 *
 * \return  Std_ReturnType {E_OK: The signal quality retrieved successfully; E_NOT_OK: The signal quality not retrieved successfully}
 *
 ***************************************************************************************************
 */
#if ( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: ResultPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for ResultPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetTrcvSignalQuality( uint8 TrcvIdx,
                                           EthIf_SignalQualityResultType * ResultPtr )
{
#if(ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON)

    /* Declare local variables */
    EthIf_SignalQualityMeasurement_tst      *lSignalQualityMeasurement_pst;
    Std_ReturnType                          lStdRetVal_en = E_NOT_OK;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GET_TRCV_SIGNALQUALITY, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_GET_TRCV_SIGNALQUALITY, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ResultPtr == NULL_PTR), ETHIF_SID_ETHIF_GET_TRCV_SIGNALQUALITY, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].SignalQualityMeasurement_st);

    /* Enter critical section: A critical section is required here as EthIf_Prv_SignalQualityProcessing()/EthIf_ClearTrcvSignalQuality() can interrupt this function, */
    /* and leads to inconsistent data. */
     SchM_Enter_EthIf_TrcvSignalQuality();

    /* If the signal quality is successfully polled for the requested TrcvIdx, update the out parameter. */
    if( lSignalQualityMeasurement_pst->SignalQualityValid_b != FALSE )
    {
        ResultPtr->HighestSignalQuality = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality;
        ResultPtr->LowestSignalQuality  = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality;
        ResultPtr->ActualSignalQuality  = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality;

        lStdRetVal_en = E_OK;
    }

    /* Exit critical section */
    SchM_Exit_EthIf_TrcvSignalQuality();

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)ResultPtr;
    return E_NOT_OK;
#endif
}
#endif


/**
 ***************************************************************************************************
 * \module description
 * ETHIF91059: Clear the stored signal quality of the link of the given Ethernet transceiver
 * \par Service ID 0x19, Synchronous, Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx.
 *
 * Parameter In:
 * \param TrcvIdx     Index of the transceiver within the context of the Ethernet Interface
 *
 * \return  Std_ReturnType {E_OK: The signal quality cleared successfully; E_NOT_OK: The signal quality cleared not successfully}
 *
 ***************************************************************************************************
 */
#if ( ETHIF_ETHIFTRCV_EXIST == STD_ON )
Std_ReturnType EthIf_ClearTrcvSignalQuality( uint8 TrcvIdx )

{
#if(ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON)

    /* Declare local variables */
    EthIf_SignalQualityMeasurement_tst   *lSignalQualityMeasurement_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_CLEAR_TRCV_SIGNALQUALITY, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_CLEAR_TRCV_SIGNALQUALITY, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].SignalQualityMeasurement_st);

    /* Enter critical section: A critical section is required here as EthIf_Prv_SignalQualityProcessing()/EthIf_GetTrcvSignalQuality() can interrupt this function, */
    /* and leads to inconsistent data. */
     SchM_Enter_EthIf_TrcvSignalQuality();

    /* Clear the polled signal quality for the requested TrcvIdx */
    lSignalQualityMeasurement_pst->SignalQualityValid_b = FALSE;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality = 0U;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality  = ETHIF_UINT32_MAX;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality  = 0U;

    /* Exit critical section */
    SchM_Exit_EthIf_TrcvSignalQuality();

    return E_OK;
#else
    (void)TrcvIdx;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the Mode of a given transceiver.
 * \par service ID 0x4a, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param TrcvModePtr   Pointer to the Mode of Ethernet transceiver
 *                      ETHTRCV_MODE_DOWN  : the transceiver is disabled
 *                      ETHTRCV_MODE_ACTIVE: the transceiver is enabled
 *
 * \return              Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the Ethernet transceiver Mode}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: TrcvModePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for TrcvModePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetTransceiverMode( uint8 TrcvIdx,
                                            EthTrcv_ModeType* TrcvModePtr )
{
#if(ETHIF_CFG_GET_TRANSCEIVERMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETTRANSCEIVERMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_GETTRANSCEIVERMODE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvModePtr == NULL_PTR), ETHIF_SID_ETHIF_GETTRANSCEIVERMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetTransceiverMode(  EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8 ,
                                                        TrcvModePtr );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)TrcvModePtr;
    return E_NOT_OK;
#endif

}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the  link state of the indexed transceiver.
 * \par service ID 0x4c, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param LinkStatePtr  Pointer to the link state of Ethernet transceiver
 *                      ETHTRCV_LINK_STATE_DOWN: transceiver is disconnected
 *                      ETHTRCV_LINK_STATE_ACTIVE: transceiver is connected
 *
 * \return              Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the Ethernet transceiver link state }
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: LinkStatePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for LinkStatePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_TransceiverGetLinkState( uint8 TrcvIdx,
                                                EthTrcv_LinkStateType* LinkStatePtr )
{
#if(ETHIF_CFG_GET_TRANSCEIVERLINKSTATE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_TRANSCEIVERGETLINKSTATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_TRANSCEIVERGETLINKSTATE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (LinkStatePtr == NULL_PTR), ETHIF_SID_ETHIF_TRANSCEIVERGETLINKSTATE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetLinkState(  EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8 ,
                                                    LinkStatePtr );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)LinkStatePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the baud rate of the indexed transceiver
 * \par service ID 0x4e, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param BaudRatePtr   Pointer to the baud rate of Ethernet transceiver
 *                      ETHTRCV_BAUD_RATE_10MBIT: 10MBit connection
 *                      ETHTRCV_BAUD_RATE_100MBIT: 100MBit connection
 *                      ETHTRCV_BAUD_RATE_1000MBIT: 1000MBit connection
 *                      ETHTRCV_BAUD_RATE_2500MBIT: 2500MBit connection
 *
 * \return              Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the Ethernet transceiver baud rate}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: BaudRatePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for BaudRatePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_TransceiverGetBaudRate( uint8 TrcvIdx,
                                                EthTrcv_BaudRateType* BaudRatePtr )
{
#if( ETHIF_CFG_GET_TRANSCEIVERBAUDRATE_API == STD_ON )
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_TRANSCEIVERGETBAUDRATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_TRANSCEIVERGETBAUDRATE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (BaudRatePtr == NULL_PTR), ETHIF_SID_ETHIF_TRANSCEIVERGETBAUDRATE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetBaudRate(  EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8 ,
                                                BaudRatePtr );

    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)BaudRatePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the duplex mode of the indexed transceiver
 * \par service ID 0x50, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param DuplexModePtr Pointer to the duplex mode of Ethernet transceiver
 *                      ETHTRCV_DUPLEX_MODE_HALF: half duplex connections
 *                      ETHTRCV_DUPLEX_MODE_FULL: full duplex connection
 *
 * \return              Std_ReturnType {E_OK: success; E_NOT_OK: failed to get the Ethernet transceiver duplex mode}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: DuplexModePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for DuplexModePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_TransceiverGetDuplexMode( uint8 TrcvIdx,
                                                EthTrcv_DuplexModeType* DuplexModePtr )
{
#if(ETHIF_CFG_GET_TRANSCEIVERDUPLEXMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_TRANSCEIVERGETDUPLEXMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_TRANSCEIVERGETDUPLEXMODE, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (DuplexModePtr == NULL_PTR), ETHIF_SID_ETHIF_TRANSCEIVERGETDUPLEXMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
    lStdRetVal_en = EthTrcv_GetDuplexMode(  EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8 ,
                                            DuplexModePtr );
    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    (void)DuplexModePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Trigger the cable diagnostics for the given Ethernet transceiver
 * \par service ID 0x62, Asynchronous,  Reentrant for different TrcvIdx. Non reentrant for the same TrcvIdx.
 *
 * Parameter In:
 * \param TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
 *
 * \return              Std_ReturnType {E_OK: The trigger has been accepted; E_NOT_OK: The trigger has not been accepted}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
Std_ReturnType EthIf_RunCableDiagnostic( uint8 TrcvIdx )
{
#if ( ETHIF_CFG_RUN_CABLEDIAGNOSTIC_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Initialise local variables */
    lStdRetVal_en = E_NOT_OK;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_RUNCABLEDIAGNOSTIC, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: TrcvIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TrcvIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8), ETHIF_SID_ETHIF_RUNCABLEDIAGNOSTIC, ETHIF_E_INV_TRCV_IDX, E_NOT_OK )

    /* The check for the transceiver is more appropriate and meaningful than EthIfController state, TrcvState_en is ETHIF_TRCV_STATE_ACTIVE it can trigger cable diagnostics */
    if( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].TrcvState_en == ETHIF_TRCV_STATE_ACTIVE )
    {
        /* Fetch Ethtrcv index in ethernet transceiver context and Forward function call to transceiver driver*/
        lStdRetVal_en = EthTrcv_RunCableDiagnostic(EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[TrcvIdx].EthTrcvIdx_cu8);
    }
    return lStdRetVal_en;
#else
    (void)TrcvIdx;
    return E_NOT_OK;
#endif
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
