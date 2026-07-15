

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED

#if (ETHTSYN_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != 4))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != 2))
#error "AUTOSAR minor version undefined or mismatched"
#endif
#endif /* (ETHTSYN_DEV_ERROR_DETECT == STD_ON) */

#include "EthTSyn_Prv.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */


/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_DomainNumGenn()
 *
 * \Function Description
 * This function is called for generating the Domain Index, port index and EthIfCtrl Index
 *
 * Parameters (in) :
 * \param CtrlIdx          - Index of the Ethernet Interface Controller
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * \param IdxDomPtr        - Index of the Domain configured
 * \param IdxPortPtr       - Index of the Port configured
 * \param EthIfCtrlrIdx_u8 - Index of the current EthIf Controller
 *
 * Return Value: None
 * Return Type : void
 *************************************************************************************************************
 */
void EthTSyn_DomainNumGenn( uint8  CtrlIdx,
                            uint8 *IdxDomPtr,
                            uint8 *IdxPortPtr,
                            uint8 *EthIfCtrlrIdx )
{
    /* Local Variable Declaration */
    uint8  lIdxDom_u8;
    uint8  lIdxPort_u8;
    uint8  lnrPortConfigs_u8;

    for( lIdxDom_u8 = 0; lIdxDom_u8 < ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED; lIdxDom_u8++ )
    {
        lnrPortConfigs_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].nrPortConfigs_u8;
        for( lIdxPort_u8 = 0; lIdxPort_u8 < lnrPortConfigs_u8; lIdxPort_u8++ )
        {
            if( CtrlIdx == EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].EthIfCtrlrIdx_u8 )
            {
                /* Store the index value */
                *IdxDomPtr     = lIdxDom_u8;
                *IdxPortPtr    = lIdxPort_u8;
                *EthIfCtrlrIdx = CtrlIdx;
                /* Break the Loop once index are initilized */
                /* MR12 RULE 14.2 VIOLATION: In EthTSyn_DomainNumGenn(), Loop control variables lIdxPort_u8 and lIdxDom_u8 are updated such that we could break the loop. */
                lIdxPort_u8  = lnrPortConfigs_u8;
                lIdxDom_u8   = ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED;
            } /* If check for controller index */
        } /* For loop to check through all ports present configured in the domain */
    } /* For loop to check through all Domains configured */
}/* End of function */

/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_SetTransmissionModeDetChk()
 *
 * \Function Description
 * This function is called from EthTSyn_SetTransmissionMode to handle the errors for function arguments.
 *
 * Parameters (in) :
 * \param CtrlIdx    - Index of the Ethernet controller
 * \param Mode       - ETHTSYN_TX_OFF or ETHTSYN_TX_ON
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * \param IdxDomPtr  - Index of the Domain configured
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 *************************************************************************************************************
 */
Std_ReturnType EthTSyn_SetTransmissionModeDetChk( uint8                         CtrlIdx,
                                                  EthTSyn_TransmissionModeType  Mode,
                                                  uint8                        *IdxDomPtr )
{
    /* Local Variable Declaration */
    uint8          lCtrlIdx_u8;
    Std_ReturnType lRetVal_en;
    uint8          lIdxPort_u8;

    /* Local Variable Initialisation */
    lIdxPort_u8 = ETHTSYN_ZERO;
    lCtrlIdx_u8 = ETHTSYN_FFU;

    /* Function call to obtaiin domin index and port index */
    EthTSyn_DomainNumGenn( CtrlIdx, IdxDomPtr, &lIdxPort_u8, &lCtrlIdx_u8 );

    /* DET error reporting */
    if (ETHTSYN_INIT_E != EthTSyn_InitStatus_en)
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: EthTSyn module was not initialized */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_SETTXNMODE, ETHTSYN_E_NOT_INITIALIZED );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( lCtrlIdx_u8 != CtrlIdx )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: Invalid value for Ethernet controller Index within the context of the Ethernet Interface */
        /* This DET has to be updated when postbuild is implemented */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_SETTXNMODE, ETHTSYN_E_CTRL_IDX );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( ( ETHTSYN_TX_OFF != Mode ) && ( ETHTSYN_TX_ON != Mode ) )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: invalid input parameter */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_SETTXNMODE, ETHTSYN_E_PARAM );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else
    {
        /* Set return value */
        lRetVal_en = E_OK;
    }
    return lRetVal_en;
}

# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_GetCurrentVirtualLocalTimeDetChk()
 *
 * \Function Description
 * This function is called from EthTSyn_GetCurrentVirtualLocalTime to handle the errors for function arguments.
 *
 * Parameters (in) :
 * \param Result_en   - Return value from EthIf_GetCurrentTime API
 * \param TimeQual_en - Time Quality parameter
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 *************************************************************************************************************
 */
void EthTSyn_GetCurrentVirtualLocalTimeDetChk( Std_ReturnType         Result_en,
                                               Eth_TimeStampQualType  TimeQual_en )
{
    /* DET error reporting */
#  if !( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON ) )
    (void) Result_en;
    (void) TimeQual_en;
#  else
    if ( ( E_OK != Result_en ) || ( ETH_VALID != TimeQual_en ) )
    {
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_GETCURRENTVIRTUALTIME, ETHTSYN_E_INV_TIMESTAMP );
    }
    else
#  endif
    {
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_GETCURRENTVIRTUALTIME, ETHTSYN_E_INV_RETURNVALUE );
    }
}
# endif

/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_RxIndicationDetChk()
 *
 * \Function Description
 * This function is called from EthTSyn_RxIndication to handle the errors for function arguments.
 *
 * Parameters (in) :
 * \param CtrlIdx     - Index of the Ethernet controller
 * \param PhysAddrPtr - pointer to Physical source address (MAC address in network byte order) of received Ethernet frame
 * \param DataPtr     - Pointer to payload of the received Ethernet frame (i.e. Ethernet header is not provided)
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * \param IdxDomPtr   - Index of the Domain configured
 * \param IdxPortPtr  - Index of the Port configured
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 *************************************************************************************************************
 */
Std_ReturnType EthTSyn_RxIndicationDetChk( uint8         CtrlIdx,
                                           const uint8  *PhysAddrPtr,
                                           const uint8  *DataPtr,
                                                 uint8  *IdxDomPtr,
                                                 uint8  *IdxPortPtr )
{
    /* Local Variable Declaration */
    uint8          lCtrlIdx_u8;
    Std_ReturnType lRetVal_en;

    /* Local Variable Initialisation */
    lCtrlIdx_u8 = ETHTSYN_FFU;

    /* Function call to obtain domin index and port index */
    EthTSyn_DomainNumGenn( CtrlIdx, IdxDomPtr, IdxPortPtr, &lCtrlIdx_u8 );

    /* DET error reporting */
    if (ETHTSYN_INIT_E != EthTSyn_InitStatus_en)
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: EthTSyn module was not initialized */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RXINDICATION, ETHTSYN_E_NOT_INITIALIZED );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( ( NULL_PTR == DataPtr ) || ( NULL_PTR == PhysAddrPtr ) )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: invalid pointer */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RXINDICATION, ETHTSYN_E_PARAM_POINTER );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( lCtrlIdx_u8 != CtrlIdx )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: Invalid value for Ethernet controller Index within the context of the Ethernet Interface */
        /* This DET has to be updated when postbuild is implemented */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RXINDICATION, ETHTSYN_E_CTRL_IDX );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else
    {
        /* Set return value */
        lRetVal_en = E_OK;
    }
    return lRetVal_en;
}

/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_TxConfirmationDetChk()
 *
 * \Function Description
 * This function is called from EthTSyn_TxConfirmation to handle the errors for function arguments.
 *
 * Parameters (in) :
 * \param CtrlIdx   - Index of the Ethernet controller
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * \param IdxDomPtr - Index of the Domain configured
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 *************************************************************************************************************
 */
Std_ReturnType EthTSyn_TxConfirmationDetChk( uint8   CtrlIdx,
                                             uint8  *IdxDomPtr )
{
    /* Local Variable Declaration */
    uint8          lCtrlIdx_u8;
    uint8          lIdxPort_u8;
    Std_ReturnType lRetVal_en;

    /* Local Variable Initialisation */
    lIdxPort_u8 = ETHTSYN_ZERO;
    lCtrlIdx_u8 = ETHTSYN_FFU;

    /* Function call to obtaiin domin index and port index */
    EthTSyn_DomainNumGenn( CtrlIdx, IdxDomPtr, &lIdxPort_u8, &lCtrlIdx_u8 );

    /* DET error reporting */
    if ( ETHTSYN_UNINIT_E == EthTSyn_InitStatus_en )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: EthTSyn module was not initialized */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TXCONFIRMATION, ETHTSYN_E_NOT_INITIALIZED );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( lCtrlIdx_u8 != CtrlIdx )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: Invalid value for Ethernet controller Index within the context of the Ethernet Interface */
        /* This DET has to be updated when postbuild is implemented */
        /* The access to controller Index has to be updated with Multi-Domain as with TimeBridge same controller Index is used */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TXCONFIRMATION, ETHTSYN_E_CTRL_IDX );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else
    {
        /* Set return value */
        lRetVal_en = E_OK;
    }
    return lRetVal_en;
}

#if ( ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) && ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON ) )
/**
 **************************************************************************************************************
 * \Function Name : EthTSyn_ReceiveSyncDetChk()
 *
 * \Function Description
 * This function is called from EthTSyn_ReceiveSync to handle the errors for function arguments.
 *
 * Parameters (in) :
 * \param DataPtr_u8       - Pointer to the received data
 * \param IdxGlbTimSlv_u8  - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 *************************************************************************************************************
 */
Std_ReturnType EthTSyn_ReceiveSyncDetChk( const uint8 *DataPtr_pu8,
                                                uint8  IdxGlbTimSlv_u8 )
{
    /* Local Variable declaration */
    EthTSyn_SrcPortId_tst *lSyncSrcPortIdentity_pst;
    EthTSyn_SrcPortId_tst  lEthTSyn_SyncSrcPortIdentity_st;
    Std_ReturnType         lRetVal_en;

    /* Local Pointer Initialization */
    lSyncSrcPortIdentity_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].SyncSrcPortIdentity_st;

    /* Store the Source Port Identity of the received Sync message */
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[0] = DataPtr_pu8[ETHTSYN_TWENTY];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[1] = DataPtr_pu8[ETHTSYN_TWENTYONE];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[2] = DataPtr_pu8[ETHTSYN_TWENTYTWO];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[3] = DataPtr_pu8[ETHTSYN_TWENTYTHREE];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[4] = DataPtr_pu8[ETHTSYN_TWENTYFOUR];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[5] = DataPtr_pu8[ETHTSYN_TWENTYFIVE];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[6] = DataPtr_pu8[ETHTSYN_TWENTYSIX];
    lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[7] = DataPtr_pu8[ETHTSYN_TWENTYSEVEN];
    lEthTSyn_SyncSrcPortIdentity_st.PortNumber_u16       = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[ETHTSYN_TWENTYEIGHT] ) << ETHTSYN_EIGHT ) | ( DataPtr_pu8[ETHTSYN_TWENTYNINE] ) );

    /* Check that source port identity of the received sync message and the source port identity of the first sync message stored in slave structure is same or not */
    if( ( ( ( uint16 )lSyncSrcPortIdentity_pst->ClockIdentity_au8[3] << ETHTSYN_EIGHT )
            | lSyncSrcPortIdentity_pst->ClockIdentity_au8[4] ) != ETHTSYN_MAC_EUI64_RESERVED_BYTE )
    {
        *lSyncSrcPortIdentity_pst = lEthTSyn_SyncSrcPortIdentity_st;
    }

    /* DET error reporting */
    if ( lSyncSrcPortIdentity_pst->PortNumber_u16 != lEthTSyn_SyncSrcPortIdentity_st.PortNumber_u16 )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: Runtime error when slave receives a sync message with different source pot identity */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RECEIVESYNC, ETHTSYN_E_TSCONFLICT );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else if ( (  ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[0] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[0] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[1] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[1] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[2] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[2] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[3] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[3] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[4] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[4] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[5] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[5] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[6] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[6] )
              || ( lSyncSrcPortIdentity_pst->ClockIdentity_au8[7] != lEthTSyn_SyncSrcPortIdentity_st.ClockIdentity_au8[7] ) ) )
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: Runtime error when slave receives a sync message with different source pot identity */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RECEIVESYNC, ETHTSYN_E_TSCONFLICT );
# endif
        lRetVal_en = E_NOT_OK;
    }
    else
    {
        /* Set return value */
        lRetVal_en = E_OK;
    }
    return lRetVal_en;
}
#endif

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_CONFIGURED */


