

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

/* If RTE is enabled, include SchM_EthIf.h header file */
#if( ETHIF_ECUC_RB_RTE_IN_USE == STD_ON )
#include "SchM_EthIf.h"
#endif

#include "EthIf_Cfg_SchM.h"

/* EthTrcv version check */
#if ( ETHIF_ETHIFTRCV_EXIST == STD_ON )
#include "EthTrcv.h"
#if (!defined(ETHTRCV_AR_RELEASE_MAJOR_VERSION) || (ETHTRCV_AR_RELEASE_MAJOR_VERSION != ETHIF_AR_RELEASE_MAJOR_VERSION))
#error "EthTrcv AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(ETHTRCV_AR_RELEASE_MINOR_VERSION) || (ETHTRCV_AR_RELEASE_MINOR_VERSION != ETHIF_AR_RELEASE_MINOR_VERSION))
#error "EthTrcv AUTOSAR minor version undefined or mismatched"
#endif
#endif

/* EthSwt version check */
#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "EthSwt.h"
#if (!defined(ETHSWT_AR_RELEASE_MAJOR_VERSION) || (ETHSWT_AR_RELEASE_MAJOR_VERSION != ETHIF_AR_RELEASE_MAJOR_VERSION))
#error "EthSwt AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(ETHSWT_AR_RELEASE_MINOR_VERSION) || (ETHSWT_AR_RELEASE_MINOR_VERSION != ETHIF_AR_RELEASE_MINOR_VERSION))
#error "EthSwt AUTOSAR minor version undefined or mismatched"
#endif
#endif

/* BswM version check */
#if ( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "BswM.h"
#if (!defined(BSWM_AR_RELEASE_MAJOR_VERSION) || (BSWM_AR_RELEASE_MAJOR_VERSION != ETHIF_AR_RELEASE_MAJOR_VERSION))
#error "BswM AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(BSWM_AR_RELEASE_MINOR_VERSION) || (BSWM_AR_RELEASE_MINOR_VERSION != ETHIF_AR_RELEASE_MINOR_VERSION))
#error "BswM AUTOSAR minor version undefined or mismatched"
#endif
#endif

/* DET version check */
#if (ETHIF_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != ETHIF_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != ETHIF_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif
#endif

/* EthSM version check */
#include "EthSM.h"
#include "EthSM_Cbk.h"
#if (!defined(ETHSM_AR_RELEASE_MAJOR_VERSION) || (ETHSM_AR_RELEASE_MAJOR_VERSION != ETHIF_AR_RELEASE_MAJOR_VERSION))
#error "EthSM AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(ETHSM_AR_RELEASE_MINOR_VERSION) || (ETHSM_AR_RELEASE_MINOR_VERSION != ETHIF_AR_RELEASE_MINOR_VERSION))
#error "EthSM AUTOSAR minor version undefined or mismatched"
#endif

#if (ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON)
#include "EthIf_Cfg_Time.h"
#endif

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

/* EthIf Init API call status flag */
EthIf_StateType EthIf_InitStatus_en = ETHCTRL_STATE_UNINIT;

/* EthIf configuration pointer */
const EthIf_ConfigType * EthIf_CfgPtr_pco = NULL_PTR;

#define ETHIF_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_Timer( void );
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
 * ETHIF024: Initialises the Ethernet Interface
 * \par Service ID 0x01, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CfgPtr    Points to the implementation specific structure
 *
 * \return          None
 *
 ***************************************************************************************************
 */
void EthIf_Init( const EthIf_ConfigType * CfgPtr )
{
    /* Declare local variables */
    uint8       lLoopIdx_u8;
#if ( ETHIF_CFG_CONFIGURATION_VARIANT != ETHIF_CFG_VARIANT_PRE_COMPILE )
    boolean     lCfgValid_b;
#endif
    /* Initialize state to Uninit */
    EthIf_InitStatus_en  = ETHCTRL_STATE_UNINIT;

    /* Initialize global configuration pointer to null pointer */
    EthIf_CfgPtr_pco = NULL_PTR;

    /* -------------------------------------- */
    /* Verification of CfgPtr in argument     */
    /* -------------------------------------- */

#if ( ETHIF_CFG_CONFIGURATION_VARIANT != ETHIF_CFG_VARIANT_PRE_COMPILE )
    /* Initialize the config valid flag to FALSE */
    lCfgValid_b = FALSE;

    /* Loop through all EthIf config sets to check whether CfgPtr is a valid configuration */
    for( lLoopIdx_u8 = 0; lLoopIdx_u8 < ETHIF_NUM_CONFIGSETS; lLoopIdx_u8++ )
    {
        if ( (&EthIf_ConfigSet[lLoopIdx_u8]) == CfgPtr ) /* EthIf_Config_aco is the post build data set */
        {
            lCfgValid_b = TRUE;
            break;
        }
    }

    /* Report DET: Development Error: CfgPtr is an invalid configuration */
    ETHIF_REPORT_ERROR_RET_VOID( (lCfgValid_b == FALSE), ETHIF_SID_ETHCTRL_STATE_INIT, ETHIF_E_INIT_FAILED )

    /* Save the given configuration pointer */
    EthIf_CfgPtr_pco = CfgPtr;

#else

    /* Report DET if CfgPtr is not NULL_PTR */
    ETHIF_REPORT_ERROR_RET_VOID( (CfgPtr != NULL_PTR), ETHIF_SID_ETHIF_INIT, ETHIF_E_INIT_FAILED )

    /* If the variant is pre-compile (NULL_PTR is passed as input parameter), */
    /* intialize global configuration pointer with default configuration. */
    EthIf_CfgPtr_pco = &EthIf_ConfigSet[0];

    /* Ignore passed parameter. */
    (void)CfgPtr;

#endif

    /* -------------------------------------- */
    /* Initialization of global variables     */
    /* -------------------------------------- */

    /* Initialize Dynamic EthIfCtrl structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lLoopIdx_u8++ )
    {
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lLoopIdx_u8].ReqMode_en = ETH_MODE_DOWN;
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lLoopIdx_u8].State_en = ETHIF_ETHIFCTRL_STATE_DOWN;
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lLoopIdx_u8].LinkState_en = ETHIF_LINKSTATE_DOWN;
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lLoopIdx_u8].ErrorFlg_en = ETHIF_ETHIFCTRL_ERROR_NONE;
    }

    /* Initialize Dynamic EthCtrl structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8; lLoopIdx_u8++ )
    {
        EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[lLoopIdx_u8].CtrlState_en = ETHIF_CTRL_STATE_UNINIT;
        EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[lLoopIdx_u8].ActiveCntr_u8 = 0U;
    }

#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
    /* Initialize Dynamic Trcv structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8; lLoopIdx_u8++ )
    {
#if(ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON)
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.HighestSignalQuality = 0U;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.LowestSignalQuality  = ETHIF_UINT32_MAX;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.ActualSignalQuality  = 0U;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.SignalQualityValid_b = FALSE;
#endif
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvState_en             = ETHIF_TRCV_STATE_UNINIT;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvLinkState_en         = ETHIF_LINKSTATE_DOWN;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].ActiveCntr_u8            = 0U;
        EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvLinkStateActiveReq_b = FALSE;
    }
#endif

#if ( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
    /* Initialize Dynamic Switch structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwitches_cu8; lLoopIdx_u8++ )
    {
        EthIf_CfgPtr_pco->EthIf_DynamicSwitchTable_cpst[lLoopIdx_u8].SwtState_en = ETHIF_SWT_STATE_UNINIT;
    }

    /* Initialize Dynamic Switch Port structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPorts_cu8; lLoopIdx_u8++ )
    {
#if (ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON)
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.HighestSignalQuality = 0U;
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.LowestSignalQuality  = ETHIF_UINT32_MAX;
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.EthIfSignalQualityResultType_st.ActualSignalQuality  = 0U;
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st.SignalQualityValid_b = FALSE;
#endif
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortState_en     = ETHIF_SWTPORT_STATE_DOWN;
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortLinkState_en = ETHIF_LINKSTATE_DOWN;
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortOffDelay_u32 = 0U;
#endif
        EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].ActiveCntr_u8    = 0U;
    }

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Initialize Dynamic Standalone Switch PG structures */
    for( lLoopIdx_u8 = 0U; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPortGroup_cu8; lLoopIdx_u8++ )
    {
        /* If the current Port Group is a standalone PG */
        if( EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefStandaloneSwtPortGroupTableEntry_cpst != NULL_PTR )
        {
            EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefStandaloneSwtPortGroupTableEntry_cpst->Mode_en      = ETHIF_STANDALONE_PG_MODE_DOWN;
            EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefStandaloneSwtPortGroupTableEntry_cpst->ReqMode_en   = ETHTRCV_MODE_DOWN;
            EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[lLoopIdx_u8].RefStandaloneSwtPortGroupTableEntry_cpst->LinkState_en = ETHIF_LINKSTATE_DOWN;
        }
    }
#endif
#endif

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
    /* Initialize Phys address filters */
    EthIf_Prv_ResetPhysAddrFilterReferences();
#endif

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
    /* Initialize global variable related to link state change indication to default value */
    EthIf_SwitchPortLinkStateChanged_b = FALSE;
#endif

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
    /* Initialize error measurement counters to zero. */
    EthIf_InvalidCtrlIdxDropCnt_u32   = 0U;
    EthIf_InvalidEtherTypeDropCnt_u32 = 0U;
#endif

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Initialize switch port active timer to zero. */
    EthIf_PortActiveTime_u32 = 0U;
#endif

    /* EthIf is now initialized */
    EthIf_InitStatus_en = ETHCTRL_STATE_INIT;

    return;
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF082: Returns the version information of this module
 * \par Service ID 0x0b, Synchronous, Non Reentrant
 *
 * Parameter Out:
 * \param VersionInfoPtr    Version information of this module
 *
 * \return                  None
 *
 ***************************************************************************************************
 */
#if(ETHIF_VERSIONINFO_API == STD_ON)
void EthIf_GetVersionInfo( Std_VersionInfoType * VersionInfoPtr )
{
    /* Report DET : Development Error for Invalid Configuration Pointer */
    ETHIF_REPORT_ERROR_RET_VOID( ( VersionInfoPtr == NULL_PTR ), ETHIF_SID_ETHIF_GETVERSIONINFO, ETHIF_E_PARAM_POINTER)

    VersionInfoPtr->vendorID            = ETHIF_VENDOR_ID;
    VersionInfoPtr->moduleID            = ETHIF_MODULE_ID;
    VersionInfoPtr->sw_major_version    = ETHIF_SW_MAJOR_VERSION;
    VersionInfoPtr->sw_minor_version    = ETHIF_SW_MINOR_VERSION;
    VersionInfoPtr->sw_patch_version    = ETHIF_SW_PATCH_VERSION;

    return;
}
#endif


/*
 ***************************************************************************************************
 * Scheduled functions
 ***************************************************************************************************
 */

/**
 ***************************************************************************************************
 * \module description
 * ETHIF91051: The function checks for new received frames at the related Ethernet controller and reception queue
 * by calling Eth_Receive() with the respective FifoIdx.
 * \par Service ID 0x42, FIXED_CYCLIC
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if ( ETHIF_ENABLE_RX_POLLING == STD_ON )
void EthIf_Prv_MainFunctionRx_PriorityProcessing( uint8 EthIfPhysCtrlIdx_u8,
                                                  uint8 FifoIndex_u8,
                                                  uint16 MaxIterations_u16,
                                                  uint32 RuntimeLimit_u32 )
{
    /* Declare local variables */
    Eth_RxStatusType   lRxStatus_en;
    uint16             lRxIndicationIteration_u16;
#if (ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON )
    uint32             lInitialTime_u32;
    uint32             lCurrentTime_u32;
    uint32             lElapsedTime_u32;
#else
    (void)RuntimeLimit_u32;
#endif

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_MAINFUNCTIONRX_PRIO, ETHIF_E_NOT_INITIALIZED)

    /* Proceed only if Rx Interrupt is disabled for the EthCtrl */
    if( EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].RxInterruptEnabled_cb == FALSE )
    {
#if (ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON )
        /* Check if the passed runtime limit is not zero. */
        if( RuntimeLimit_u32 != 0UL )
        {
            /* If a runtime limit has been set, get the intial time when the function starts executing. */
            lInitialTime_u32 = ETHIF_GET_CURRTIME_IN_US();
        }
#endif

        /* No Critical section needed for protecting the access of CtrlState_en: It is allowed by Eth Driver to call Eth_Receive() after Eth_SetControllerMode(DOWN). DET is not reported in that case */
        /* Critical section is not desired here because lock time would be too long (Eth_Receive is calling RxIndication which can contain long processing) */

        /* Call Eth_Receive() only if EthCtrl is set to ETHIF_CTRL_STATE_ACTIVE */
        if ( EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].CtrlState_en == ETHIF_CTRL_STATE_ACTIVE)
        {
            lRxStatus_en = ETH_RECEIVED_MORE_DATA_AVAILABLE;

            /* Receive frames as per parameter MaxIterations or until there are no more frames to receive whichever is earliest */
            /* If parameter MaxIterations is equal to 0 that means infinity and thus loop shall continue until there are no more frames to receive */
            for( lRxIndicationIteration_u16 = 0;
                 (((MaxIterations_u16 == 0U) || (lRxIndicationIteration_u16 < MaxIterations_u16)) && (lRxStatus_en == ETH_RECEIVED_MORE_DATA_AVAILABLE));
                 lRxIndicationIteration_u16++ )
            {
                /* Call Eth_<vi>_<ai>_Receive() API */
                EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].RefEthDrvAPITablePtr_cpcst->EthReceive_pfct(
                             EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].EthCtrlIdx_cu8,
                             FifoIndex_u8,
                             &lRxStatus_en );

#if (ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON )
                /* Check if the passed runtime limit is not zero. */
                if( RuntimeLimit_u32 != 0UL )
                {
                    /* If a runtime limit has been set, get the current runtime in us. */
                    lCurrentTime_u32 = ETHIF_GET_CURRTIME_IN_US();

                    /* Calculate the elapsed time since the start of the function execution. */
                    if( lCurrentTime_u32 > lInitialTime_u32 )
                    {
                        /* Calculation of elapsed time if there is no wrap around in the time value. */
                        lElapsedTime_u32 = ( lCurrentTime_u32 - lInitialTime_u32 );
                    }
                    else
                    {
                        /* Calcuation of elapsed time if there is a wrap around condition in the timer value. */
                        lElapsedTime_u32 = ( (0xFFFFFFFFUL - lInitialTime_u32) + lCurrentTime_u32 );
                    }

                    /* If the elapsed time is greater than or equal to the runtime limit, */
                    /* exit the function even if more data is available. */
                    if( lElapsedTime_u32 >= RuntimeLimit_u32 )
                    {
                        break;
                    }
                }
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
 * ETHIF113: The function issues transmission confirmations in polling mode. It checks also for transceiver state changes.
 * \par Service ID 0x21, FIXED_CYCLIC
 *
 * \return  None
 *
 ***************************************************************************************************
 */
void EthIf_MainFunctionTx( void )
{
#if ( ETHIF_ENABLE_TX_POLLING  == STD_ON )
    /* Declare local variables */
    uint8  EthIfPhysCtrlIdx_u8;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_MAINFUNCTIONTX, ETHIF_E_NOT_INITIALIZED )

    /* Call TxConfirmation once for each EthCtrl */
    for( EthIfPhysCtrlIdx_u8 = 0; EthIfPhysCtrlIdx_u8 <  EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8; EthIfPhysCtrlIdx_u8++ )
    {
        /* Enter critical section: EthCtrl is being used based on his state, in the meantime no change of EthCtrl state shall be done */
        SchM_Enter_EthIf_EthCtrlState();

        /* Call Eth_TxConfirmation() only if EthCtrl is set to ETHIF_CTRL_STATE_ACTIVE and Tx Interrupt is disabled for the EthCtrl. */
        if ( (EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) &&
             (EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].TxInterruptEnabled_cb == FALSE) )
        {
            /* Call Eth_<vi>_<ai>_TxConfirmation() API */
            EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].RefEthDrvAPITablePtr_cpcst->EthTxConfirmation_pfct(
                    EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8].EthCtrlIdx_cu8 );
        }

        /* Exit critical section */
        SchM_Exit_EthIf_EthCtrlState();
    }
#endif

    return;
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF91104: The function is polling the link state of the used communication hardware (Ethernet transceiver, Ethernet switch ports).
 * \par Service ID 0x05, FIXED_CYCLIC
 *
 * \return  None
 *
 ***************************************************************************************************
 */
void EthIf_MainFunctionState( void )
{

    /* local variable declaration */
    uint8           lEthIfCtrlIdx_u8;
    Eth_ModeType    lCtrlReqMode_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_MAINFUNCTIONSTATE, ETHIF_E_NOT_INITIALIZED )

    /* -------------------------------------- */
    /* Timer handling                         */
    /* -------------------------------------- */

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Processing of PortOffDelay and PortActive timers */
    EthIf_Prv_Timer();
#endif

    /* -------------------------------------- */
    /* Standalone Swt PG State Machine        */
    /* -------------------------------------- */

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Execute state machine transitions */
    EthIf_Prv_StandaloneSwtPG_StateTransitions();
#endif

    /* -------------------------------------- */
    /* EthIfCtrl State Machines               */
    /* -------------------------------------- */

    /* Loop through all EthIf Ctrls */
    for( lEthIfCtrlIdx_u8 = 0U; lEthIfCtrlIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8; lEthIfCtrlIdx_u8++ )
    {
        /* Fetch the mode requested for the EthIfCtrl */
        lCtrlReqMode_en = EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[lEthIfCtrlIdx_u8].ReqMode_en;

        /* Depending on the particular reference type call the particular state transition APIs */
        switch( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[lEthIfCtrlIdx_u8].ReferenceType_cen )
        {
            case ETHIF_ETHIFCTRL_REF_TRCV:
            {
#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
                /* Reference type is a transceiver */
                EthIf_Prv_Trcv_StateTransitions( lEthIfCtrlIdx_u8, lCtrlReqMode_en );
#endif
            }
            break;

            case ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO:
            {
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
                /* Reference type is a EthIfSwitchPortGroup of semantic "link-info" */
                EthIf_Prv_SwtPGLinkInfo_StateTransitions( lEthIfCtrlIdx_u8, lCtrlReqMode_en );
#endif
            }
            break;

            case ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL:
            case ETHIF_ETHIFCTRL_REF_SWITCH:
            {
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON )
                EthIf_Prv_SwtOrPGCtrl_StateTransitions( lEthIfCtrlIdx_u8, lCtrlReqMode_en );
#endif
            }
            break;

            case ETHIF_ETHIFCTRL_REF_NOREF:
            {
                /* EthIfCtrl has no reference */
                EthIf_Prv_NoRef_StateTransitions( lEthIfCtrlIdx_u8, lCtrlReqMode_en );
            }
            break;

            default:
                /* Do Nothing */
            break;
        }
    }

    /* -------------------------------------- */
    /* Link State processing                  */
    /* -------------------------------------- */

    EthIf_Prv_LinkStateProcessing();

    /* -------------------------------------- */
    /* Signal quality processing              */
    /* -------------------------------------- */
#if( ETHIF_ENABLE_SIGNALQUALITY == STD_ON )
    EthIf_Prv_SignalQualityProcessing();
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
 * Execute timer incrementation per each MainFunctionState
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
static void EthIf_Prv_Timer( void )
{
    /* Declare local variables */
    uint8                            lloopIdx_u8;

    /* -------------------------------------- */
    /* PortOffDelay timer                     */
    /* -------------------------------------- */

    /* Loop through all the ports of all the Switches */
    for( lloopIdx_u8 = 0U; lloopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPorts_cu8; lloopIdx_u8++ )
    {
        /* Check whether the port off delay timer has been started */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lloopIdx_u8].ActiveCntr_u8 == 0U) &&
            (EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lloopIdx_u8].PortOffDelay_u32 > 0U) &&
            (EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lloopIdx_u8].PortOffDelay_u32 < ETHIF_PORT_SWITCHOFF_DELAY) )
        {
            /* If the timer has been started, but not timed out, increment the timer */
            EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lloopIdx_u8].PortOffDelay_u32 += ETHIF_MAINFUNCTION_STATE_PERIOD;
        }
    }

    /* -------------------------------------- */
    /* PortActive timer                       */
    /* -------------------------------------- */

    /* Check if Port active timer has been started */
    if( EthIf_PortActiveTime_u32 > 0U )
    {
        /* Decrement the PortActiveTime if the timer has been started and not reached to zero */
        EthIf_PortActiveTime_u32--;
    }

    return;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
