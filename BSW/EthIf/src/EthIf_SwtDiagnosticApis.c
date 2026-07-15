

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"

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
 * ETHIF91059: Retrieves the signal quality of the link of the given Ethernet switch port
 * \par Service ID 0x1A, Synchronous, Reentrant for different Ethernet switch indexes and Ethernet Switch port indexes.
 * Non reentrant for the same SwitchPortIdx.
 *
 * Parameter In:
 * \param SwitchIdx     Index of the Ethernet switch within the context of the Ethernet Interface
 * \param SwitchPortIdx Index of the Ethernet switch port within the context of the Ethernet Interface Parameters
 *
 * Parameter Out:
 * \param ResultPtr   Pointer to the memory where the signal quality shall be stored.
 *
 * \return  Std_ReturnType {E_OK: The signal quality retrieved successfully; E_NOT_OK: The signal quality not retrieved successfully}
 *
 ***************************************************************************************************
 */
#if ( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: ResultPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for ResultPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetSwitchPortSignalQuality( uint8 SwitchIdx,
                                                 uint8 SwitchPortIdx,
                                                 EthIf_SignalQualityResultType * ResultPtr )
{
#if(ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON)

    /* Declare local variables */
    EthIf_SignalQualityMeasurement_tst   *lSignalQualityMeasurement_pst;
    uint8                                   lDynamicSwtPortTableIdx_u8;
    Std_ReturnType                          lStdRetVal_en = E_NOT_OK;


    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GET_SWITCHPORT_SIGNALQUALITY, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GET_SWITCHPORT_SIGNALQUALITY, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                                   ETHIF_SID_ETHIF_GET_SWITCHPORT_SIGNALQUALITY, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ResultPtr == NULL_PTR), ETHIF_SID_ETHIF_GET_SWITCHPORT_SIGNALQUALITY, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Calculate the index of EthIf_DynamicSwtPortTable for the requested SwitchIdx-SwitchPortIdx pair */
    lDynamicSwtPortTableIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8 + SwitchPortIdx;

    lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lDynamicSwtPortTableIdx_u8].SignalQualityMeasurement_st);

    /* Enter critical section: A critical section is required here as EthIf_Prv_SignalQualityProcessing()/EthIf_ClearSwitchPortSignalQuality() can interrupt this function, */
    /* and leads to inconsistent data. */
    SchM_Enter_EthIf_SwtPortSignalQuality();

    /* If the signal quality is successfully polled for the requested SwitchIdx-SwitchPortIdx, update the out parameter. */
    if( lSignalQualityMeasurement_pst->SignalQualityValid_b != FALSE )
    {
        ResultPtr->HighestSignalQuality = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality;
        ResultPtr->LowestSignalQuality  = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality;
        ResultPtr->ActualSignalQuality  = lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality;

        lStdRetVal_en = E_OK;
    }

    /* Exit critical section */
    SchM_Exit_EthIf_SwtPortSignalQuality();

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)ResultPtr;
    return E_NOT_OK;
#endif
}
#endif



/**
 ***************************************************************************************************
 * \module description
 * ETHIF91060: Clear the stored signal quality of the link of the given Ethernet switch port
 * \par Service ID 0x1B, Synchronous, Reentrant for different Ethernet switch indexes and Ethernet Switch port indexes.
 * Non reentrant for the same SwitchPortIdx.
 *
 * Parameter In:
 * \param SwitchIdx     Index of the Ethernet switch within the context of the Ethernet Interface
 * \param SwitchPortIdx Index of the Ethernet switch port within the context of the Ethernet Interface Parameters
 *
 * \return  Std_ReturnType {E_OK: The signal quality cleared successfully; E_NOT_OK: The signal quality cleared not successfully}
 *
 ***************************************************************************************************
 */
#if ( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_ClearSwitchPortSignalQuality( uint8 SwitchIdx,
                                                   uint8 SwitchPortIdx )
{
#if(ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON)

    /* Declare local variables */
    EthIf_SignalQualityMeasurement_tst      *lSignalQualityMeasurement_pst;
    uint8                                   lDynamicSwtPortTableIdx_u8;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_CLEAR_SWITCHPORT_SIGNALQUALITY, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_CLEAR_SWITCHPORT_SIGNALQUALITY, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                                   ETHIF_SID_ETHIF_CLEAR_SWITCHPORT_SIGNALQUALITY, ETHIF_E_INV_PARAM, E_NOT_OK )

   /* Calculate the index of EthIf_DynamicSwtPortTable for the requested SwitchIdx-SwitchPortIdx pair */
   lDynamicSwtPortTableIdx_u8 = EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8 + SwitchPortIdx;

   lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lDynamicSwtPortTableIdx_u8].SignalQualityMeasurement_st);

   /* Enter critical section: A critical section is required here as EthIf_Prv_SignalQualityProcessing()/EthIf_GetSwitchPortSignalQuality() can interrupt this function, */
   /* and leads to inconsistent data. */
   SchM_Enter_EthIf_SwtPortSignalQuality();

    /* Clear the polled signal quality for the requested SwitchIdx-SwitchPortIdx */
    lSignalQualityMeasurement_pst->SignalQualityValid_b = FALSE;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality = 0U;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality  = ETHIF_UINT32_MAX;
    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality  = 0U;

    /* Exit critical section */
    SchM_Exit_EthIf_SwtPortSignalQuality();

    return E_OK;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the port over which this MAC-address can be reached
 * \par service ID 0x28, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param MacAddrPtr          MAC-address for which a switch port is searched over which the node
 *                            with this MAC-address can be reached.
 *
 * Parameter Out:
 * \param SwitchIdxPtr        Pointer to the switch index
 * \param PortIdxPtr          Pointer to the port index
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK:  switch port could not be initialized}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: MacAddrPtr, SwitchIdxPtr and PortIdxPtr are an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for MacAddrPtr, SwitchIdxPtr and PortIdxPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetPortMacAddr(const uint8* MacAddrPtr,
                                    uint8* SwitchIdxPtr,
                                    uint8* PortIdxPtr)
{
#if ( ETHIF_CFG_GET_PORTMACADDR_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lFunctionRetVal_en;
    Std_ReturnType              lStdRetVal_en;
    uint8                       lPortIdx_u8;
    uint8                       lLoopIdx_u8;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETPORTMACADDR, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MacAddrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPORTMACADDR, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdxPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPORTMACADDR, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortIdxPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPORTMACADDR, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialize lFunctionRetVal_en */
    lFunctionRetVal_en = E_NOT_OK;
    /* Loop through all configured EthIf switches */
    for(lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8; lLoopIdx_u8++)
    {
        /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
        lStdRetVal_en = EthSwt_GetPortMacAddr ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lLoopIdx_u8].EthSwtIdx_cu8,
                                                MacAddrPtr,
                                                &lPortIdx_u8  );
        /* If return value of EthSwt_GetPortMacAddr is equal to  E_OK*/
        if( lStdRetVal_en == E_OK )
        {
            /* Check if the function EthSwt_GetPortMacAddr already return E_OK in the previous iteration*/
            if( lFunctionRetVal_en == E_OK )
            {
                /* Set function return value to E_NOT_OK and break the loop*/
                lFunctionRetVal_en = E_NOT_OK;
                break;
            }
            else
            {
                lFunctionRetVal_en = E_OK;
                *SwitchIdxPtr = lLoopIdx_u8;
                *PortIdxPtr = lPortIdx_u8;

            }
        }
    }

    return lFunctionRetVal_en;
#else
    (void)MacAddrPtr;
    (void)SwitchIdxPtr;
    (void)PortIdxPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the address resolution table of a switch and copies the list into a user provided buffer.
 * The function will copy all or numberOfElements into the output list. If input value of
 * numberOfElements is 0 the function will not copy any data but only return the number of valid
 * entries in the cache. arlTableListPointer may be NULL_PTR in this case.
 * \par service ID 0x29, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx         Index of the switch within the context of the Ethernet Interface
 *
 * Parameter Inout:
 * numberOfElements         In: Maximum number of elements which can be written into the arlTable
*                           Out: Number of elements which are currently available in the EthSwitch module.
 *
 * Parameter Out:
 * \param arlTableListPointer   Returns a pointer to the memory where the ARL table of the switch consisting of a list
 *                              of structs with MAC-address, VLAN-ID and port shall be stored.
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: requested switchIdx is not valid or inactive}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: numberOfElements and arlTableListPointer are an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for numberOfElements and arlTableListPointer. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetArlTable( uint8 switchIdx,
                                  uint16* numberOfElements,
                                  Eth_MacVlanType* arlTableListPointer )
{
#if (ETHIF_CFG_GET_ARLTABLE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETARLTABLE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (switchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETARLTABLE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (numberOfElements == NULL_PTR), ETHIF_SID_ETHIF_GETARLTABLE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (arlTableListPointer == NULL_PTR), ETHIF_SID_ETHIF_GETARLTABLE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetArlTable ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[switchIdx].EthSwtIdx_cu8,
                                         numberOfElements,
                                         arlTableListPointer );

    return lStdRetVal_en;
#else
    (void)switchIdx;
    (void)numberOfElements;
    (void)arlTableListPointer;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the mode of the indexed switch port
 * \par service ID 0x49, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx          Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param PortModePtr   ETH_MODE_DOWN: The Ethernet switch port of the given Ethernet switch is disabled
 *                      ETH_MODE_ACTIVE: The Ethernet switch port of the given Ethernet switch is enabled
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: The mode of the indexed switch port could not be obtained,
 *                                  or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: PortModePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for PortModePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetSwitchPortMode ( uint8 SwitchIdx,
                                         uint8 SwitchPortIdx,
                                         EthTrcv_ModeType* PortModePtr )
{
#if (ETHIF_CFG_GET_SWITCHPORTMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETSWITCHPORTMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETSWITCHPORTMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                                ETHIF_SID_ETHIF_GETSWITCHPORTMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortModePtr == NULL_PTR), ETHIF_SID_ETHIF_GETSWITCHPORTMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetSwitchPortMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                               SwitchPortIdx,
                                               PortModePtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)PortModePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the mode of the indexed switch port
 * \par service ID 0x4b, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx          Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param LinkStatePtr   ETHTRCV_LINK_STATE_DOWN: Switch port is disconnected
 *                       ETHTRCV_LINK_STATE_ACTIVE: Switch port is connected
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: Link state of the indexed switch port could not be
 *                               obtained, or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: LinkStatePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for LinkStatePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetLinkState ( uint8 SwitchIdx,
                                              uint8 SwitchPortIdx,
                                              EthTrcv_LinkStateType* LinkStatePtr )
{
#if ( ETHIF_CFG_GET_LINKSTATE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETLINKSTATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETLINKSTATE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                           ETHIF_SID_ETHIF_SWITCHPORTGETLINKSTATE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (LinkStatePtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETLINKSTATE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetLinkState ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                    SwitchPortIdx,
                                                    LinkStatePtr);

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)LinkStatePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the baud rate of the indexed switch port
 * \par service ID 0x4d, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param BaudRatePtr   ETHTRCV_BAUD_RATE_10MBIT: 10MBit connection
 *                      ETHTRCV_BAUD_RATE_100MBIT: 100MBit connection
 *                      ETHTRCV_BAUD_RATE_1000MBIT: 1000MBit connection
 *                      ETHTRCV_BAUD_RATE_2500MBIT: 2500MBit connection
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: Baud rate of the indexed switch port could
 *                    not be obtained, or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: BaudRatePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for BaudRatePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetBaudRate ( uint8 SwitchIdx,
                                             uint8 SwitchPortIdx,
                                             EthTrcv_BaudRateType* BaudRatePtr )
{
#if (ETHIF_CFG_GET_BAUDRATE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETBAUDRATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETBAUDRATE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
            ETHIF_SID_ETHIF_SWITCHPORTGETBAUDRATE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (BaudRatePtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETBAUDRATE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetBaudRate ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                         SwitchPortIdx,
                                         BaudRatePtr);

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)BaudRatePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtains the duplex mode of the indexed switch port
 * \par service ID 0x4f, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param DuplexModePtr   ETHTRCV_DUPLEX_MODE_HALF: half duplex connections
 *                        ETHTRCV_DUPLEXMODE_FULL: full duplex connection
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: duplex mode of the indexed switch port could
 *                      not be obtained, or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: DuplexModePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for DuplexModePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetDuplexMode ( uint8 SwitchIdx,
                                         uint8 SwitchPortIdx,
                                         EthTrcv_DuplexModeType* DuplexModePtr )
{
#if (ETHIF_CFG_GET_DUPLEXMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETDUPLEXMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETDUPLEXMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                         ETHIF_SID_ETHIF_SWITCHPORTGETDUPLEXMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (DuplexModePtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETDUPLEXMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetDuplexMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                           SwitchPortIdx,
                                           DuplexModePtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)DuplexModePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Reads a list with drop counter values of the corresponding port of the switch. The meaning of
 * these values is described at Eth_CounterType.
 * \par service ID 0x51, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param CounterPtr   counter values according to IETF RFC 1757, RFC 1643 and RFC 2233.
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: counter values read failure.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: CounterPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for CounterPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetCounterValues ( uint8 SwitchIdx,
                                                  uint8 SwitchPortIdx,
                                                  Eth_CounterType* CounterPtr )
{
#if (ETHIF_CFG_GET_COUNTERVALUES_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETCOUNTERVALUES, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETCOUNTERVALUES, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                            ETHIF_SID_ETHIF_SWITCHPORTGETCOUNTERVALUES, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CounterPtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETCOUNTERVALUES, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetCounterValues ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                              SwitchPortIdx,
                                              CounterPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)CounterPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Returns a list of statistic counters defined with Eth_RxTatsType. The majority of these Counters
 * are derived from the IETF RFC2819.
 * \par service ID 0x52, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param RxStatsPtr   List of values according to IETF RFC 2819 (Remote Network Monitoring Management Information Base)
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: The mode of the indexed switch port could not be obtained,
 *                                  or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: RxStatsPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for RxStatsPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetRxStats  ( uint8 SwitchIdx,
                                             uint8 SwitchPortIdx,
                                             Eth_RxStatsType* RxStatsPtr )
{
#if (ETHIF_CFG_GET_RXSTATS_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETRXSTATS, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETRXSTATS, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                       ETHIF_SID_ETHIF_SWITCHPORTGETRXSTATS, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (RxStatsPtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETRXSTATS, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetRxStats ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                        SwitchPortIdx,
                                        RxStatsPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)RxStatsPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Returns the list of Transmission Statistics out of IETF RFC1213 defined with Eth_TxStatsType,
 * where the maximal possible value shall denote an invalid value, e.g. this counter is not available.
 * \par service ID 0x53, Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param TxStatsPtr   List of values to read statistic values for transmission.
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: Tx-statistics could not be obtained.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: TxStatsPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for TxStatsPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetTxStats ( uint8 SwitchIdx,
                                            uint8 SwitchPortIdx,
                                            Eth_TxStatsType* TxStatsPtr )
{
#if (ETHIF_CFG_GET_TXSTATS_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETTXSTATS, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETTXSTATS, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                            ETHIF_SID_ETHIF_SWITCHPORTGETTXSTATS, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TxStatsPtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETTXSTATS, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetTxStats ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                        SwitchPortIdx,
                                        TxStatsPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)TxStatsPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Returns the list of Transmission Error Counters out of IETF RFC1213 and RFC1643 defined with
 *  Eth_TxErrorCounterValuesType, where the maximal possible value shall denote an invalid value,
 *  e.g. this counter is not available.
 * \par service ID 0x54, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param TxStatsPtr    List of values to read statistic error counter values for transmission.
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: Tx-statistics could not be obtained.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: TxStatsPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for TxStatsPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetTxErrorCounterValues ( uint8 SwitchIdx,
                                                         uint8 SwitchPortIdx,
                                                         Eth_TxErrorCounterValuesType* TxStatsPtr  )
{
#if (ETHIF_CFG_GET_TXERRORCOUNTERVALUES_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETTXERRORCOUNTERVALUES, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETTXERRORCOUNTERVALUES, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                      ETHIF_SID_ETHIF_SWITCHPORTGETTXERRORCOUNTERVALUES, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (TxStatsPtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETTXERRORCOUNTERVALUES, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetTxErrorCounterValues ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                        SwitchPortIdx,
                                                        TxStatsPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)TxStatsPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Returns the MAC learning mode, i.e. 1.) HW learning enabled, 2.) Hardware learning disabled,
 * 3.) Software learning enabled. Note: This feature is hardware dependent,
 * i.e. the switch hardware needs to support the different learning modes
 * \par service ID 0x55, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param MacLearningModePtr   Defines whether MAC addresses shall be learned and if they shall be learned in
 *                             software or hardware.
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: configuration could be persistently reset.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: MacLearningModePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for MacLearningModePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetMacLearningMode ( uint8 SwitchIdx,
                                                     uint8 SwitchPortIdx,
                                                     EthSwt_MacLearningType* MacLearningModePtr )
{
#if ( ETHIF_CFG_GET_MACLEARNINGMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGETMACLEARNINGMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SWITCHPORTGETMACLEARNINGMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
            ETHIF_SID_ETHIF_SWITCHPORTGETMACLEARNINGMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MacLearningModePtr == NULL_PTR), ETHIF_SID_ETHIF_SWITCHPORTGETMACLEARNINGMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetMacLearningMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                SwitchPortIdx,
                                                MacLearningModePtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)MacLearningModePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * This function retrieves the OUI (24 bit) of the indexed Ethernet switch port.
 * \par service ID 0x56, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param OrgUniqueIdPtr   Pointer to the memory where the Organizationally Unique Identifier (OUI) shall be stored.
 * \param ModelNrPtr       Pointer to the memory where the Manufacturer's Model Number shall be stored.
 * \param RevisionNrPtr    Pointer to the memory where the Revision Number shall be stored.
 *
 * \return          Std_ReturnType {E_OK: organizationally unique identifier of the Ethernet transceiver could be read.;
 *                          E_NOT_OK: organizationally unique identifier of the Ethernet transceiver could not be
 *                          obtained (i.e. OUI is not available).}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: OrgUniqueIdPtr, ModelNrPtr and RevisionNrPtr are an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for OrgUniqueIdPtr, ModelNrPtr and RevisionNrPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetSwitchPortIdentifier  ( uint8 SwitchIdx,
                                                uint8 SwitchPortIdx,
                                                uint32* OrgUniqueIdPtr,
                                                uint8* ModelNrPtr,
                                                uint8* RevisionNrPtr )
{
#if ( ETHIF_CFG_GET_SWITCHPORTIDENTIFIER_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (SwitchPortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                    ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (OrgUniqueIdPtr == NULL_PTR), ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
        ETHIF_REPORT_ERROR_RET_VALUE( (ModelNrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
        ETHIF_REPORT_ERROR_RET_VALUE( (RevisionNrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetPortIdentifier ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                               SwitchPortIdx,
                                               OrgUniqueIdPtr,
                                               ModelNrPtr,
                                               RevisionNrPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)SwitchPortIdx;
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
 * Obtain the Organizationally Unique Identifier that is given by the IEEE of the indexed Ethernet switch.
 * This function shall provide the OUI of Ethernet switch. The OUI has a size of 24 bit. If a ethernet switch
 * can provide the OUI the 8 most significant bits of the OUI shall be set to 0x00xxxxxx. If a Ethernet switch
 * can not provide the OUI the 8 most significant bits of the OUI shall be set to 0xFFxxxxxx.
 * \par service ID 0x57, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param OrgUniqueIdPtr   Pointer to the memory where the Organizationally Unique Identifier shall be stored.
 *
 * \return          Std_ReturnType {E_OK: organizationally unique identifier of the Ethernet switch could be read.;
 *                                  E_NOT_OK: organizationally unique identifier of the Ethernet switch could not be
 *                                  read (i.e. no OUI is available for this Ethernet switch)}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: OrgUniqueIdPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for OrgUniqueIdPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetSwitchIdentifier  ( uint8 SwitchIdx,
                                            uint32* OrgUniqueIdPtr )
{
#if ( ETHIF_CFG_GET_SWITCHIDENTIFIER_API  == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETSWITCHIDENTIFIER, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETSWITCHIDENTIFIER, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (OrgUniqueIdPtr == NULL_PTR), ETHIF_SID_ETHIF_GETSWITCHIDENTIFIER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetSwitchIdentifier ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                 OrgUniqueIdPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)OrgUniqueIdPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Store the given port mirror configuration in a shadow buffer in the Ethernet switch driver for
 * the given MirroredSwitchIdx.
 * \par service ID 0x58, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param MirroredSwitchIdx    Index of the switch within the context of the Ethernet Interface, where the Ethernet
 *                             switch port is located, that has to be mirrored
 * \param PortMirrorConfigurationPtr    Pointer to the memory where the port configuration shall be stored.
 *
 * \return          Std_ReturnType {E_OK: the port mirror configuration for the indexed Ethernet switch port was written.
 * E_NOT_OK: the port mirror configuration for the indexed Ethernet switch port was not written. (i.e. indexed ethernet
 *      switch is not available) ETHSWT_PORT_MIRRORING_CONFIGURATION_NOT_SUPPORTED: port mirroring configuration is not
 *      supported by Ethernet switch driver or by the Ethernet switch hardware}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: PortMirrorConfigurationPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for PortMirrorConfigurationPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_WritePortMirrorConfiguration ( uint8 MirroredSwitchIdx,
                                                    const EthSwt_PortMirrorCfgType* PortMirrorConfigurationPtr )
{
#if (ETHIF_CFG_WRITE_PORTMIRRORCONFIGURATION_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_WRITEPORTMIRRORCONFIGURATION, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: MirroredSwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MirroredSwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_WRITEPORTMIRRORCONFIGURATION, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortMirrorConfigurationPtr == NULL_PTR), ETHIF_SID_ETHIF_WRITEPORTMIRRORCONFIGURATION, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_WritePortMirrorConfiguration ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[MirroredSwitchIdx].EthSwtIdx_cu8,
                                                          PortMirrorConfigurationPtr);

    return lStdRetVal_en;
#else
    (void)MirroredSwitchIdx;
    (void)PortMirrorConfigurationPtr ;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtain the port mirror configuration of the given Ethernet switch.
 * \par service ID 0x49, Synchronous/Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Switch Driver
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param PortModePtr   ETH_MODE_DOWN: The Ethernet switch port of the given Ethernet switch is disabled
 *                      ETH_MODE_ACTIVE: The Ethernet switch port of the given Ethernet switch is enabled
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: The mode of the indexed switch port could not be obtained,
 *                                  or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: PortMirrorConfigurationPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for PortMirrorConfigurationPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_ReadPortMirrorConfiguration ( uint8 MirroredSwitchIdx,
                                                   EthSwt_PortMirrorCfgType* PortMirrorConfigurationPtr  )
{
#if (ETHIF_CFG_READ_PORTMIRRORCONFIGURATION_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_READPORTMIRRORCONFIGURATION, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: MirroredSwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MirroredSwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_READPORTMIRRORCONFIGURATION, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortMirrorConfigurationPtr == NULL_PTR), ETHIF_SID_ETHIF_READPORTMIRRORCONFIGURATION, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_ReadPortMirrorConfiguration ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[MirroredSwitchIdx].EthSwtIdx_cu8,
                                                         PortMirrorConfigurationPtr );

    return lStdRetVal_en;
#else
    (void)MirroredSwitchIdx;
    (void)PortMirrorConfigurationPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Delete the stored port mirror configuration of the given MirroredSwitchIdx. If no port mirror configuration was
 * found for the given MirroredSwitchIdx, the return value shall be E_OK.
 * \par service ID 0x5a, Synchronous, Reentrant for different MirroredSwitchIdx. Non reentrant for the same SwitchIdx.
 *
 * Parameter In:
 * \param MirroredSwitchIdx    Index of the switch within the context of the Ethernet Interface, where the Ethernet
 *        switch port is located, that has to be mirrored
 *
 * \return          Std_ReturnType {E_OK: Port mirror configuration was deleted successfully;
 *                                  E_NOT_OK: Port mirror configuration was not deleted successfully. (e.g. the port
 *                                  mirroring is enabled}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_DeletePortMirrorConfiguration ( uint8 MirroredSwitchIdx )
{
#if (ETHIF_CFG_DELETE_PORTMIRRORCONFIGURATION_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_DELETEPORTMIRRORCONFIGURATION, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: MirroredSwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MirroredSwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_DELETEPORTMIRRORCONFIGURATION, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_DeletePortMirrorConfiguration ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[MirroredSwitchIdx].EthSwtIdx_cu8);

    return lStdRetVal_en;
#else
    (void)MirroredSwitchIdx;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Obtain the current status of the port mirroring for the indexed Ethernet switch port
 * \par service ID 0x5b, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param PortMirrorStatePtr   Pointer to the memory where the port mirroring state (either PORT_MIRRORING_ENABLED or
 *                                            PORT_MIRRORING_DISABLED)of the given Ethernet switch port shall be stored.
 *
 * \return          Std_ReturnType {E_OK: the port mirroring state for the indexed Ethernet switch port returned
 *                          successfully. ; E_NOT_OK: the port mirror configuration for the indexed Ethernet switch
 *                          returned not successfully. (i.e. indexed ethernet switch port is not available)}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: PortMirrorStatePtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for PortMirrorStatePtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetPortMirrorState ( uint8 SwitchIdx,
                                          EthSwt_PortMirrorStateType* PortMirrorStatePtr  )
{
#if (ETHIF_CFG_GET_PORTMIRRORSTATE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETPORTMIRRORSTATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETPORTMIRRORSTATE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortMirrorStatePtr == NULL_PTR), ETHIF_SID_ETHIF_GETPORTMIRRORSTATE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetPortMirrorState ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                PortMirrorStatePtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortMirrorStatePtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Request to set the given port mirroring state of the port mirror configuration for the given Ethernet switch.
 * \par service ID 0x5c, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param MirroredSwitchIdx    Index of the Ethernet switch within the context of the Ethernet Interface, where
 *                             the port mirroring configuration is located that has to be enabled and disabled, repectively.
 * \param PortMirrorState      Contain the requested port mirroring state either PORT_MIRRORING_ENABLED or
 *                             PORT_MIRRORING_DISABLED
 *
 * \return          Std_ReturnType {E_OK: the requested port mirroring state for the indexed Ethernet switch port was
 *                                        set successfully.
 *                                  E_NOT_OK:  the requested port mirroring state for the indexed Ethernet switch was
 *                                  not set successfully. (i.e. indexed Ethernet switch is not available, no port
 *                                  mirror configuration is available}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_SetPortMirrorState ( uint8 MirroredSwitchIdx,
                                          EthSwt_PortMirrorStateType PortMirrorState )
{
#if (ETHIF_CFG_SET_PORTMIRRORSTATE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPORTMIRRORSTATE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: MirroredSwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (MirroredSwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SETPORTMIRRORSTATE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_SetPortMirrorState ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[MirroredSwitchIdx].EthSwtIdx_cu8,
                                                PortMirrorState );

    return lStdRetVal_en;
#else
    (void)MirroredSwitchIdx;
    (void)PortMirrorState;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Activates a given test mode of the indexed Ethernet switch port.
 * \par service ID 0x5d, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param PortIdx      Index of the port at the addressed switch
 * \param Mode         Test mode to be activated
 *
 * \return          Std_ReturnType {E_OK: the port test mode for the indexed Ethernet switch port was set successfully.
 *                                  E_NOT_OK: the port test mode for the indexed Ethernet switch was not set
 *                                  successfully. (i.e. indexed Ethernet switch port is not available)}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_SetPortTestMode  ( uint8 SwitchIdx,
                                        uint8 PortIdx,
                                        EthTrcv_PhyTestModeType Mode )
{
#if (ETHIF_CFG_SET_PORTTESTMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPORTTESTMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: MirroredSwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SETPORTTESTMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: SwitchPortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                  ETHIF_SID_ETHIF_SETPORTTESTMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_SetPortTestMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                             PortIdx,
                                             Mode );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    (void)Mode;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Activates a given test loop-back mode of the indexed Ethernet switch port.
 * \par service ID 0x5e, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param PortIdx      Index of the port at the addressed switch
 * \param Mode         Loop-back mode to be activated
 *
 * \return          Std_ReturnType {E_OK: the port mirroring loop-back back mode for the indexed Ethernet switch port
 *                                        was activated successfully. ;
 *                                        E_NOT_OK: the port mirroring loop-back back mode for the indexed Ethernet
 *                                        switch port was not activated successfully. (i.e. indexed Ethernet switch
 *                                        port is not available}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType  EthIf_SetPortLoopbackMode ( uint8 SwitchIdx,
                                            uint8 PortIdx,
                                            EthTrcv_PhyLoopbackModeType Mode )
{
#if (ETHIF_CFG_SET_PORTLOOPBACKMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPORTLOOPBACKMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SETPORTLOOPBACKMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                                     ETHIF_SID_ETHIF_SETPORTLOOPBACKMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_SetPortLoopbackMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                 PortIdx,
                                                 Mode );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    (void)Mode;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Activates a given transmission mode of the indexed Ethernet switch port.
 * \par service ID 0x5f, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Switch Driver
 * \param SwitchPortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param PortModePtr   ETH_MODE_DOWN: The Ethernet switch port of the given Ethernet switch is disabled
 *                      ETH_MODE_ACTIVE: The Ethernet switch port of the given Ethernet switch is enabled
 *
 * \return          Std_ReturnType {E_OK: success; E_NOT_OK: The mode of the indexed switch port could not be obtained,
 *                                  or the function is called in state ETHSWT_STATE_UNINIT or ETHSWT_STATE_INIT.}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_SetPortTxMode ( uint8 SwitchIdx,
                                     uint8 PortIdx,
                                     EthTrcv_PhyTxModeType Mode )
{
#if (ETHIF_CFG_SET_PORTTXMODE_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPORTTXMODE, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_SETPORTTXMODE, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
                                    ETHIF_SID_ETHIF_SETPORTTXMODE, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_SetPortTxMode ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                           PortIdx,
                                           Mode );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    (void)Mode;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the cable diagnostics result of the indexed Ethernet switch port respectively the referenced Ethernet
 * Transceiver Driver.
 * \par service ID 0x60, Synchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param PortIdx      Index of the port at the addressed switch
 *
 * Parameter Out:
 * \param ResultPtr   Pointer to the location where the cable diagnostics result shall be stored
 *
 * \return          Std_ReturnType {E_OK: the port cable diagnostic result for the indexed Ethernet switch port was
 *                                        obtained successfully.
 *                                  E_NOT_OK: the port cable diagnostic result for the indexed Ethernet switch port was
 *                                  not obtained successfully. (i.e. indexed Ethernet switch port is not available)}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: ResultPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for ResultPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_GetPortCableDiagnosticsResult ( uint8 SwitchIdx,
                                                     uint8 PortIdx,
                                                     EthTrcv_CableDiagResultType* ResultPtr )
{
#if (ETHIF_CFG_GET_PORTCABLEDIAGNOSTICSRESULT_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETPORTCABLEDIAGNOSTICSRESULT, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETPORTCABLEDIAGNOSTICSRESULT, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
            ETHIF_SID_ETHIF_GETPORTCABLEDIAGNOSTICSRESULT, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: ResultPtr is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ResultPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPORTCABLEDIAGNOSTICSRESULT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetPortCableDiagnosticsResult ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                           PortIdx,
                                                           ResultPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    (void)ResultPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Trigger the cable diagnostics of the given Ethernet Switch port (PortIdx) by calling EthTrcv_RunCableDiagnostic of
 * the referenced Ethernet transceiver.
 * \par service ID 0x61, Asynchronous, Reentrant for different SwitchIdx and PortIdx. Non reentrant for the same
 *                                     SwitchIdx and PortIdx.
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param PortIdx      Index of the port at the addressed switch
 *
 * \return          Std_ReturnType {E_OK: The trigger to run the cable diagnostic has been accepted
 *                                  E_NOT_OK: The trigger to run the cable diagnostic has not been accepted}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
Std_ReturnType EthIf_RunPortCableDiagnostic ( uint8 SwitchIdx,
                                              uint8 PortIdx )
{
#if ( ETHIF_CFG_RUN_PORTCABLEDIAGNOSTIC_API  == STD_ON)
    /* Declare local variables */
    Std_ReturnType               lStdRetVal_en;
    EthIf_DynamicSwtPort_tst*    lDynamicSwtPort_past;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_RUNPORTCABLEDIAGNOSTIC, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_RUNPORTCABLEDIAGNOSTIC, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
            ETHIF_SID_ETHIF_RUNPORTCABLEDIAGNOSTIC, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Initialise local variables */
    lStdRetVal_en       = E_NOT_OK;

    /* Fetch the EthIfSwtPort array of structures from SwitchIdx index */
    lDynamicSwtPort_past = &EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].RefPortsTableOffset_cu8];

    /* The check for the port state is more appropriate and meaningful than EthIfController state, if switch port state is ETHIF_SWTPORT_STATE_ACTIVE it can trigger cable diagnostics */
    if( lDynamicSwtPort_past[PortIdx].PortState_en == ETHIF_SWTPORT_STATE_ACTIVE )
    {
        /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
        lStdRetVal_en = EthSwt_RunPortCableDiagnostic ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                        PortIdx );
    }

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the data in memory of the indexed Ethernet switch in variable length
 * \par service ID 0x63, Asynchronous, Non reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param Offset       Offset of the Ethernet switch memory from where the reading starts
 * \param Length       Length of data in bytes that shall be copied
 *
 * Parameter Out:
 * \param BufferPtr   Pointer to the location where the data shall be copied
 *
 * \return          Std_ReturnType {E_OK: the data read was triggered successfully
 *                                  E_NOT_OK:  the data read was not triggered successfully (i.e. indexed Ethernet
 *                                  switch is not available}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: BufferPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for BufferPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchGetCfgDataRaw ( uint8 SwitchIdx,
                                           uint32 Offset,
                                           uint16 Length,
                                           uint8* BufferPtr )
{
#if (ETHIF_CFG_GET_CFGDATARAW_API == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETCFGDATARAW, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETCFGDATARAW, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: BufferPtr is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (BufferPtr == NULL_PTR), ETHIF_SID_ETHIF_GETCFGDATARAW, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetCfgDataRaw ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                           Offset,
                                           Length,
                                           BufferPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)Offset;
    (void)Length;
    (void)BufferPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the cable diagnostics result of the indexed Ethernet switch port respectively the referenced Ethernet
 * Transceiver Driver.
 * \par service ID 0x64, Asynchronous, Reentrant
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param DataSizePtr   Pointer to the location where the total size of the configuration data shall be copied
 * \param DataAdressPtr Pointer to the location where the start address of the configuration registers shall be copied
 *
 * \return          Std_ReturnType {E_OK: the data was obtained successfully
 *                                  E_NOT_OK: the data was not obtained successfully. (i.e. indexed Ethernet switch is
 *                                            not available}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: DataSizePtr and DataAdressPtr are an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for DataSizePtr and DataAdressPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType  EthIf_SwitchGetCfgDataInfo ( uint8 SwitchIdx,
                                             uint32* DataSizePtr,
                                             uint32* DataAdressPtr )
{
#if ( ETHIF_CFG_GET_CFGDATAINFO_API  == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETCFGDATAINFO, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETCFGDATAINFO, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: DataSizePtr is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (DataSizePtr == NULL_PTR), ETHIF_SID_ETHIF_GETCFGDATAINFO, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: DataAdressPtr is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (DataAdressPtr == NULL_PTR), ETHIF_SID_ETHIF_GETCFGDATAINFO, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetCfgDataInfo ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                            DataSizePtr,
                                            DataAdressPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)DataSizePtr;
    (void)DataAdressPtr;
    return E_NOT_OK;
#endif
}
#endif

/**
 ***************************************************************************************************
 * \module description
 * Retrieves the cable diagnostics result of the indexed Ethernet switch port respectively the referenced Ethernet
 * Transceiver Driver.
 * \par service ID 0x65, Asynchronous, Reentrant for different SwitchIdx and PortIdx. Non reentrant for the same
 *                                     SwitchIdx and PortIdx.
 *
 * Parameter In:
 * \param SwitchIdx    Index of the switch within the context of the Ethernet Interface
 * \param PortIdx      Index of the port at the addressed switch
 * \param SwitchPortEgressFifoIdx   Index of the egress FIFO of the addressed Ethernet switch port
 *
 * Parameter Out:
 * \param SwitchPortEgressFifoBufferLevelPtr   Pointer to a memory location, where the maximum amount of allocated FIFO
 *                                             buffer (in bytes) since the last read out shall be stored
 *
 * \return          Std_ReturnType {E_OK: success
 *                                  E_NOT_OK: The maximal FIFO buffer level could not be obtained}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
/* MR12 RULE 8.13 VIOLATION: SwitchPortEgressFifoBufferLevelPtr is an out argument and the addressed object is modified inside the function, pointer to variable is defined in AUTOSAR for SwitchPortEgressFifoBufferLevelPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_SwitchPortGetMaxFIFOBufferFillLevel ( uint8 SwitchIdx,
                                                           uint8 PortIdx,
                                                           uint8 SwitchPortEgressFifoIdx,
                                                           uint32* SwitchPortEgressFifoBufferLevelPtr )
{
#if (ETHIF_CFG_GET_MAXFIFOBUFFERFILLLEVEL_API  == STD_ON)
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETMAXFIFOBUFFERFILLLEVEL, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: SwitchIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8), ETHIF_SID_ETHIF_GETMAXFIFOBUFFERFILLLEVEL, ETHIF_E_INV_SWT_IDX, E_NOT_OK )

    /* Report DET : Development Error: PortIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( (SwitchIdx < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8) && (PortIdx >= EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].NbPorts_cu8) ),
            ETHIF_SID_ETHIF_GETMAXFIFOBUFFERFILLLEVEL, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: ResultPtr is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (SwitchPortEgressFifoBufferLevelPtr == NULL_PTR), ETHIF_SID_ETHIF_GETMAXFIFOBUFFERFILLLEVEL, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch EthSwt index in ethernet switch context and Forward function call to switch driver*/
    lStdRetVal_en = EthSwt_GetMaxFIFOBufferFillLevel ( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[SwitchIdx].EthSwtIdx_cu8,
                                                       PortIdx,
                                                       SwitchPortEgressFifoIdx,
                                                       SwitchPortEgressFifoBufferLevelPtr );

    return lStdRetVal_en;
#else
    (void)SwitchIdx;
    (void)PortIdx;
    (void)SwitchPortEgressFifoIdx;
    (void)SwitchPortEgressFifoBufferLevelPtr;
    return E_NOT_OK;
#endif
}
#endif


#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
