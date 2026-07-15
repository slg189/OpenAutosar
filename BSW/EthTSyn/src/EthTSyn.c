

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED

# if( ETHTSYN_ECUC_RB_RTE_IN_USE == STD_ON)
#include "SchM_EthTSyn.h"
# endif

# if ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) ) )
#include "Mfl.h"
# endif

#include "EthTSyn_Cbk.h"

#if (ETHTSYN_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != 4))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != 2))
#error "AUTOSAR minor version undefined or mismatched"
#endif
#endif /* (ETHTSYN_DEV_ERROR_DETECT == STD_ON) */

#include "EthIf.h"

#include "EthTSyn_Prv.h"

#include "EthTSyn_Cfg_SchM.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"

/* EthTSyn call status flag */
EthTSyn_InitStatus_ten EthTSyn_InitStatus_en = ETHTSYN_UNINIT_E;

/* EthTSyn configuration pointer */
const EthTSyn_ConfigType *EthTSyn_CfgPtr_pcst   = NULL_PTR;
const EthTSyn_TimeDomainCfg_tst *EthTSyn_TimeDomainCfg_pcst   = NULL_PTR;

#define ETHTSYN_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#define ETHTSYN_START_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"

# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
/* EthTSyn message header */
const EthTSyn_ConstMsgHdr_tst EthTSyn_HdrCfg_cast[ETHTSYN_FIVE] =
{
        { ETHTSYN_FLAGS, ETHTSYN_SYNC_TYPE,                  ETHTSYN_TRANSPORT_SPECIFIC, ETHTSYN_VERSION_PTP, ETHTSYN_SYNC_CONTROL,     0 },
        { 0,             ETHTSYN_PDELAY_REQ_TYPE,            ETHTSYN_TRANSPORT_SPECIFIC, ETHTSYN_VERSION_PTP, ETHTYSN_PDELAY_CONTROL,   0 },
        { ETHTSYN_FLAGS, ETHTSYN_PDELAY_RESP_TYPE,           ETHTSYN_TRANSPORT_SPECIFIC, ETHTSYN_VERSION_PTP, ETHTYSN_PDELAY_CONTROL,   0 },
        { 0,             ETHTSYN_FOLLOW_UP_TYPE,             ETHTSYN_TRANSPORT_SPECIFIC, ETHTSYN_VERSION_PTP, ETHTSYN_FOLLOWUP_CONTROL, 0 },
        { 0,             ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE, ETHTSYN_TRANSPORT_SPECIFIC, ETHTSYN_VERSION_PTP, ETHTYSN_PDELAY_CONTROL,   0 }
};
#  endif
# endif

#define ETHTSYN_STOP_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#define ETHTSYN_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "EthTSyn_MemMap.h"

/* Table that stores EthTSyn_RamDataTyp_tst values */
EthTSyn_RamDataTyp_tst EthTSyn_RamDataTyp_ast[ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED];

# if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
/* Table that stores variables related to Slave */
EthTSyn_Slave_tst EthTSyn_Slave_ast[ETHTSYN_NUMBER_SLAVE_PORTS];
# endif

#  if (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON)
/* Table that stores variables related to Master */
EthTSyn_Master_tst EthTSyn_Master_ast[ETHTSYN_NUMBER_MASTER_PORTS];
#  endif

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )

#  if (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON)
/* Table that stores variables related to Pdelay Initiator */
EthTSyn_PdelayInitiator_tst EthTSyn_PdelayInitiator_st;
#  endif

#  if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
/* Table that stores variables related to Pdelay Responder */
EthTSyn_PdelayResponder_tst EthTSyn_PdelayResponder_st;
#  endif
# endif

/* Variable that stores Calculated Pdelay Value */
# if ( (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) )
Eth_TimeIntDiffType EthTSyn_Pdelay_ast[ETHTSYN_NUMBER_PORTS];
# endif

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
/* Global structure that stores variables related to Time Bridge */
EthTSyn_TiBrdg_tst EthTSyn_TimBrdg_st;
# endif

# ifdef ETHTSYN_SWITCH
#  if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
/* Variable that stores management object for master */
EthTSyn_MstMgmtObject_tst  EthTSyn_MstMgmtObject_ast[ETHTSYN_NUMBER_MASTER_PORTS];
#  endif

#  if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
/* Variable that stores management object for slave */
EthTSyn_SlaveMgmtObject_tst EthTSyn_SlaveMgmtObject_ast[ETHTSYN_NUMBER_SLAVE_PORTS];
#  endif
# endif

#define ETHTSYN_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#ifdef ETHTSYN_DEBUG_AND_TEST
#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
Eth_TimeStampType EthTSyn_StbMTimArry_ast[ETHTSYN_DEBUG_ARRAY_SIZE] = {0};
uint8 index = 0U;
#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
#endif


/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_Init()
 *
 * \Function Description
 * EthTSyn_00035: Initialises the Time Synchronization over Ethernet
 * \par Service ID 0x01, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * \param configPtr - Pointer to selected configuration structure
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */

void EthTSyn_Init( const EthTSyn_ConfigType *configPtr )
{
#if 0
    /*OFS not released*/
# if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
    uint8 lOFSnum_u8;
# endif
#endif

    /* Declare local variables */
    const EthTSyn_PortCfg_tst       *lPortCfg_pcst;
    uint8                            lIdxPort_u8;
    uint8                            lIdxDomain_u8;
    uint8                            lnrPortConfigs_u8;
    const EthTSyn_TimeDomainCfg_tst *lEthTSyn_TimeDomainCfg_pst;
    EthTSyn_RamDataTyp_tst          *lEthTSyn_RamDataTyp_pst;
# if( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
#  if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    EthTSyn_SubTLV_tst              *lSubTLV_pst;
#  endif
    uint8                            lIdxSlv_u8;
# endif
# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    float32                          lSync_LogMsgIntl_f32;
#  endif
    uint8                            lIdxMst_u8;
# endif
# if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
    float32                          lPdelay_Req_LogMsgIntl_f32;
#  endif
# endif
# if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) )
    const EthTSyn_PdelayCfg_tst     *lPdelayCfg_pcst;
# endif
# if ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) ) )
    float32                          llog2_f32;
# endif

    /* Initialise local variables */
    lIdxPort_u8        = ETHTSYN_ZERO;
    lIdxDomain_u8      = ETHTSYN_ZERO;
# if( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
    lIdxSlv_u8         = ETHTSYN_ZERO;
# endif
#  if (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON)
    lIdxMst_u8         = ETHTSYN_ZERO;
#  endif

    (void) configPtr;

    /* TRACE[SWS_EthTSyn_00006]: Initialize all internal variables */
    /* Initialize global variables */
    EthTSyn_CfgPtr_pcst        = &EthTSyn_ConfigSet;
    EthTSyn_TimeDomainCfg_pcst = EthTSyn_CfgPtr_pcst->TimeDomainCfg_pcst;

# if ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) ) )
    llog2_f32 = Mfl_Log_f32( ETHTSYN_LOG_BASE_FOR_LOGMSGINTL );
# endif

    for( lIdxDomain_u8 = 0; lIdxDomain_u8 < ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED; lIdxDomain_u8++ )
    {
        /* Initialize local pointers */
        lEthTSyn_RamDataTyp_pst    = &EthTSyn_RamDataTyp_ast[lIdxDomain_u8];
        lEthTSyn_TimeDomainCfg_pst = &EthTSyn_TimeDomainCfg_pcst[lIdxDomain_u8];
        lnrPortConfigs_u8          = lEthTSyn_TimeDomainCfg_pst->nrPortConfigs_u8;

        for( lIdxPort_u8 = ETHTSYN_ZERO; lIdxPort_u8 < lnrPortConfigs_u8; lIdxPort_u8++ )
        {
            /* Initialize local port config pointers */
            lPortCfg_pcst   = &lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst[lIdxPort_u8];

# if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) )
            /* Initialize local pdelay config pointers */
            lPdelayCfg_pcst = lPortCfg_pcst->PdelayCfg_pcst;
# endif
# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )

            /* Initialise variables related to Master */
            if(  NULL_PTR != lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst )
            {
#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )

                /* Initialise the Sync buffer Index variable to an invalid value */
                EthTSyn_Master_ast[lIdxMst_u8].SyncBufIdx_u8     = ETHTSYN_FFU;
                /* Initialise the Master State machine variable to INIT */
                EthTSyn_Master_ast[lIdxMst_u8].Sync_Mst_en       = ETHTSYN_SYNC_INIT_E;
#   if ( ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON )
                /* Obtain the initial value of time base update counter */
                EthTSyn_Master_ast[lIdxMst_u8].TimeBasUpdCntr_u8 = StbM_GetTimeBaseUpdateCounter( lEthTSyn_TimeDomainCfg_pst->StbM_SyncdTimeBasId_u16 );
#   endif

                /* Initialise Log Message Interval for Sync and Follow-Up Message */
                if ( ETHTSYN_ZERO != lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_u32 )
                {
                    lSync_LogMsgIntl_f32 = Mfl_Log_f32( lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_f32 );
                    lEthTSyn_RamDataTyp_pst->VarHeader_st.LogMsgIntl_au8[0] = (uint8)(Mfl_FloatToIntCvrt_f32_s8( lSync_LogMsgIntl_f32 / llog2_f32 ));
                }
                else
                {
                    lEthTSyn_RamDataTyp_pst->VarHeader_st.LogMsgIntl_au8[0] = ETHTSYN_ZERO;
                }

#  else
#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                /* Update the address of Management object structure related to master port to null pointer */
                EthTSyn_MstMgmtObject_ast[lIdxMst_u8].MstEthSwt_MgmtObject_pst = NULL_PTR;
#   endif
#  endif
                /* Map the index of Master Config structure to a variable in Master RAM datastructure */
                EthTSyn_Master_ast[lIdxMst_u8].IdxMstCfg_u8      = lIdxPort_u8;

                /* Increment the Index for Master RAM datastructure */
                lIdxMst_u8++;

            }
            else
# endif
# if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
            if( NULL_PTR != lPortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst )
            {
                /* Initialise variables related to Slave */
                /* Initialise the Slave State machine variable to INIT */
                EthTSyn_Slave_ast[lIdxSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_INIT_E;

                /*Set type of sub TLVs to zero */
#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )

                lSubTLV_pst = &EthTSyn_Slave_ast[lIdxSlv_u8].RxFollowup_st.SubTLV_st;

                /* Set the type of Time sub TLV to zero */
                lSubTLV_pst->TimeSubTLV_st.Type_u8      = ETHTSYN_ZERO;

                /* Set the type of Status sub TLV to zero */
                lSubTLV_pst->StatusSubTLV_st.Type_u8    = ETHTSYN_ZERO;

                /* Set the type of UserData sub TLV to zero */
                lSubTLV_pst->UserDataSubTLV_st.Type_u8  = ETHTSYN_ZERO;
#   if 0
                /*Set type and nrOFSSubTLV parameters to zero */
                /*OFS not released*/
                EthTSyn_RxFollowup_st.SubTLV_st.nrOFSSubTLV_u32 = ETHTSYN_ZERO;

                /*OFS not released*/
                for( lOFSnum_u8 = ETHTSYN_ZERO; lOFSnum_u8< ETHTSYN_SIXTEEN; lOFSnum_u8++ )
                {
                    EthTSyn_RxFollowup_st.SubTLV_st.OFSSubTLV_st[lOFSnum_u8].Type_u8  = ETHTSYN_ZERO;
                }
#   endif
#  endif
#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
#   ifndef ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME
                /* Update the address of Management object structure related to slave port to null pointer */
                EthTSyn_SlaveMgmtObject_ast[lIdxSlv_u8].SlaveEthSwt_MgmtObject_pst = NULL_PTR;
#   endif
#  endif
                /* Map the index of Slave Config structure to a variable in Slave RAM datastructure */
                EthTSyn_Slave_ast[lIdxSlv_u8].IdxSlaveCfg_u8 = lIdxPort_u8;

                /* Increment the Index for Master RAM datastructure */
                lIdxSlv_u8++;
            }
            else
# endif
            {
                /* Do Nothing */
            }

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON)
            if( ETHTSYN_ZERO != lPdelayCfg_pcst->PdelayReqTxPerd_u32 )
            {
                /* Initialise variables related to Pdelay Initiator */
                /* Initialise Pdelay Initiator state machine to INIT */
                EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en  = ETHTSYN_TX_PDELAY_REQ_INIT_E;
                /* Initialise Pdelay Request Buffer Index to an invalid value */
                EthTSyn_PdelayInitiator_st.PdelayReqBufIdx_u8   = ETHTSYN_FFU;
                /* Initialise Log Message Interval for Pdelay Request */
                lPdelay_Req_LogMsgIntl_f32 = Mfl_Log_f32( lPortCfg_pcst->PdelayCfg_pcst->PdelayReqTxPerd_f32 );
                lEthTSyn_RamDataTyp_pst->VarHeader_st.LogMsgIntl_au8[1] = (uint8)(Mfl_FloatToIntCvrt_f32_s8( lPdelay_Req_LogMsgIntl_f32 / llog2_f32 ));
            }
            else
            {
                lEthTSyn_RamDataTyp_pst->VarHeader_st.LogMsgIntl_au8[1] = ETHTSYN_ZERO;
            }
#  endif
#  if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
            if( FALSE != lPortCfg_pcst->GlbTimePdelayRespEna_b )
            {
                /* Initialise variables related to Pdelay Responder */
                /* Initialise Pdelay Responder state machine to INIT */
                EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_INIT_E;
                /* Initialise Pdelay Response Buffer Index to an invalid value */
                EthTSyn_PdelayResponder_st.PdelayRespBufIdx_u8  = ETHTSYN_FFU;
                /* Initialise Log Message Interval for Pdelay_Response and Pdelay_Response_Follow_Up Message */
                lEthTSyn_RamDataTyp_pst->VarHeader_st.LogMsgIntl_au8[2] = ETHTSYN_PDELAY_RESP_LOGMSGINTL;
            }
#  endif
# endif

# if ( (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON) )
            /* Ingress and Egress latency */
            /* TRACE[SWS_EthTSyn_00142]: EthTSynGlobalTimePropagationDelay shall be used as default value for the propagation delay, until first valid propagation delay has been measured */
            EthTSyn_Pdelay_ast[lIdxPort_u8].diff.secondsHi    = lPdelayCfg_pcst->PdelayGlbTimePrpgtnDly_st.secondsHi;
            EthTSyn_Pdelay_ast[lIdxPort_u8].diff.seconds      = lPdelayCfg_pcst->PdelayGlbTimePrpgtnDly_st.seconds;
            EthTSyn_Pdelay_ast[lIdxPort_u8].diff.nanoseconds  = lPdelayCfg_pcst->PdelayGlbTimePrpgtnDly_st.nanoseconds;
            EthTSyn_Pdelay_ast[lIdxPort_u8].sign              = TRUE;
# endif
            if( EthTSyn_InitStatus_en == ETHTSYN_UNINIT_E )
            {
                /* If the module is initialised for the first time, initialise Link State with FALSE */
                lEthTSyn_RamDataTyp_pst->LinkState_b   = FALSE;

#if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
                /* If the module is initialised for the first time, initialise Sequence Id at Slave to zero */
                EthTSyn_Slave_ast[lIdxPort_u8].RxSyncSeqId_u16      = ETHTSYN_ZERO;
#endif
            }
        } /* End of For loop for number of ports configured */

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
        /* Initialise variables related to Time Bridge */
        EthTSyn_TimBrdg_st.SyncMsgRxd_u8    =   ETHTSYN_MSG_CLR;
        EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 =   ETHTSYN_MSG_CLR;
        EthTSyn_TimBrdg_st.SyncBufIdx_u8    =   ETHTSYN_FFU;
        EthTSyn_TimBrdg_st.SyncFupLen_u16   =   ETHTSYN_ZERO;
        EthTSyn_TimBrdg_st.TiSubTLVLen_u16  =   ETHTSYN_ZERO;
# endif

        /* Initialise Transmission Mode to ON */
        lEthTSyn_RamDataTyp_pst->TxnMode_en                                              = ETHTSYN_TX_ON;
        /* Initialise main function counter for Master as zero */
        lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]             = ETHTSYN_ZERO;
        /* Initialise SourcePortIdentity for header to zero */
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.PortNumber_u16       = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[0] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[1] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[2] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[3] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[4] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[5] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[6] = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[7] = ETHTSYN_ZERO;
        /* Initialise sequence Id of Sync, Pdelay Request and Pdelay Response to maximum value */
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SequenceId_au16[0]                         = ETHTSYN_SEQID_INITIAL_VALUE;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SequenceId_au16[1]                         = ETHTSYN_SEQID_INITIAL_VALUE;
        lEthTSyn_RamDataTyp_pst->VarHeader_st.SequenceId_au16[2]                         = ETHTSYN_SEQID_INITIAL_VALUE;
        /* Initialise correction field to zero */
        lEthTSyn_RamDataTyp_pst->VarHeader_st.CorrectionField_u64                        = ETHTSYN_ZERO;
# ifdef ETHTSYN_SWITCH
        /* Stores the number of successful transmissions of FollowUp message */
        lEthTSyn_RamDataTyp_pst->nrTrsm_u8                                               = ETHTSYN_ZERO;
        lEthTSyn_RamDataTyp_pst->TrsmIdx_u8                                              = ETHTSYN_ZERO;
# endif

# if (ETHTSYN_TIME_BRIDGE_SM == STD_OFF)
#  if (ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON)
        /* Initialise the debounce counter to zero */
        lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX] = ETHTSYN_ZERO;
#  endif
        /* Initialize Main Function Pdelay_req timer with Zero if Pdelay measurement is enabled */
        lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX] = ETHTSYN_ZERO;
# endif
    } /* End of For loop for number of domains configured */

    /* Set to initialized state*/
    EthTSyn_InitStatus_en   = ETHTSYN_INIT_E;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_GetVersionInfo()
 *
 * \Function Description
 * EthTSyn_00036: Returns the version information of this module
 * \par Service ID 0x02, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * None
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * \param versioninfo - Pointer to where to store the version information of this module
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
#if (ETHTSYN_VERSIONINFO == STD_ON)

void EthTSyn_GetVersionInfo( Std_VersionInfoType *versioninfo )
{
    /* Condition checks: If there is no DET error when DET compiler macro is switched On. */
    if( NULL_PTR != versioninfo )
    {
        /* Assign the version information to the pointer */
        versioninfo->vendorID            = ETHTSYN_VENDOR_ID;
        versioninfo->moduleID            = ETHTSYN_MODULE_ID;
        versioninfo->sw_major_version    = ETHTSYN_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version    = ETHTSYN_SW_MINOR_VERSION;
        versioninfo->sw_patch_version    = ETHTSYN_SW_PATCH_VERSION;
    }
    else
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* TRACE[SWS_EthTSyn_00007]: Report DET for Invalid Configuration Pointer */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_GETVERSINFO, ETHTSYN_E_PARAM_POINTER );
# endif
    }
}
#endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_SetTransmissionMode()
 *
 * \Function Description
 * EthTSyn_00039: This API is used to turn on and off the TX capabilities of the EthTSyn
 * \par Service ID 0x05, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * \param CtrlIdx - Index of the Ethernet controller
 * \param Mode    - ETHTSYN_TX_OFF or ETHTSYN_TX_ON
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_SetTransmissionMode( uint8                         CtrlIdx,
                                  EthTSyn_TransmissionModeType  Mode   )
{
    /* Local Variable Declaration */
    Std_ReturnType  lRetVal_en;
    uint8           lIdxDom_u8;

    /* Local variable initialisation */
    lIdxDom_u8    = ETHTSYN_ZERO;

    /* Function call to do the DET check */
    lRetVal_en = EthTSyn_SetTransmissionModeDetChk( CtrlIdx, Mode, &lIdxDom_u8 );

    /* Condition checks: If there is no DET error when DET compiler macro is switched On. */
    if( E_OK == lRetVal_en  )
    {
        /* Update the Transmission Mode global variable */
        EthTSyn_RamDataTyp_ast[lIdxDom_u8].TxnMode_en = Mode;
    }
}

# if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
#  if ( ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
/**
 **********************************************************************************************************************
 * \Function Name : EthTSyn_GetIngressTimeStamp()
 *
 * \Function Description
 * Calls EthIf_GetIngressTimeStamp and resets the attributes for the relevant message if Time Quality is not ETH_VALID
 *
 * Parameters (in) :
 * \param CtrlrIdx_u8      - Index of the Ethernet controller within the context of the Ethernet Interface
 * \param DataPtr_pu8      - Pointer to payload of the received Ethernet frame (i.e. Ethernet header is not provided)
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8     - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgType_pu8      - Pointer to Message Type
 *
 * Parameters (out):
 * \param TimeStampPtr_pst - Pointer to Eth_TimeStampType structure
 *
 * Return Value: None
 * Return Type : void
 **********************************************************************************************************************
 */
static void EthTSyn_GetIngressTimeStamp( uint8              CtrlrIdx_u8,
                                         uint8             *DataPtr_pu8,
                                         Eth_TimeStampType *TimeStampPtr_pst,
                                         uint8             *MsgType_pu8,
                                         uint8              IdxDomain_u8,
                                         uint8              IdxGlbTim_u8 )
{
    /* Local variable declaration */
# if( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) )
    const EthTSyn_PortCfg_tst  *lPortCfg_pcst;
# endif
    Eth_TimeStampQualType       lTimeQual_en;
# if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
    uint8                       lIdxSlaveCfg_u8;
# else
    (void) IdxGlbTim_u8;
# endif

    /* Local variable initialisation */
# if( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) )
    lPortCfg_pcst   = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst;
# endif
    lTimeQual_en    = ETH_UNCERTAIN;

    /* TRACE[SWS_EthTSyn_000127] : On invocation of EthTSyn_TxConfirmation() the egress time stamp shall be retrieved for Sync via EthIf_GetEgressTimeStamp()
     *  used for transmission delay compensation of the Sync message, if EthTSynHardwareTimestampSupport is set to TRUE.*/
    EthIf_GetIngressTimeStamp( CtrlrIdx_u8, DataPtr_pu8, &lTimeQual_en, TimeStampPtr_pst );

    /* Reset the state to INIT if Time Quality is not Eth_Valid and assign an invalid number to MessageType so
     * default case of switch statement will be executed in EthTSyn_TxConfirmation.
     * Reset the respective counters as well */
    if( ETH_VALID != lTimeQual_en )
    {
#   if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
        if( ETHTSYN_SYNC_TYPE == ( *MsgType_pu8 ) )
        {
            /* Reset Slave state machine to INIT if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_Slave_ast[IdxGlbTim_u8].Sync_Slave_en                                    = ETHTSYN_SYNC_INIT_E;

            /* Reset counter for Sync-SyncFollowUp timeout counter to configured value */
            lIdxSlaveCfg_u8 = EthTSyn_Slave_ast[IdxGlbTim_u8].IdxSlaveCfg_u8;
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst->Sync_SyncFolwUpTout_u32;
        }
        else
#   endif
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#    if (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON)
        if( ETHTSYN_PDELAY_RESP_TYPE == ( *MsgType_pu8 ) )
        {
            /* Reset Pdelay Initiator State machine to INIT if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                                               = ETHTSYN_TX_PDELAY_REQ_INIT_E;

            /* Reset counter for Pdelay Request Transmission Period if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX]            = lPortCfg_pcst->PdelayCfg_pcst->PdelayReqTxPerd_u32;

            /* Reset counter for Pdelay Response and Response FollowUp timeout if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = lPortCfg_pcst->PdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16;
        }
        else
#    endif
#    if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
        if( ETHTSYN_PDELAY_REQ_TYPE == ( *MsgType_pu8 ) )
        {
            /* Reset Pdelay Responder state machine to INIT if time quality returned by lower layer is not ETH_VALID */
            EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en  = ETHTSYN_TX_PDELAY_RESP_INIT_E;
        }
        else
#    endif
#   endif
        {
            /* Do Nothing */
        }
        ( *MsgType_pu8 ) = ETHTSYN_FFU;

#   if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: invalid timestamp received*/
        /* DET will hit after reseting counters and state machines */
        (void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_GETINGRESSTIMESTAMP, ETHTSYN_E_INV_TIMESTAMP );
#   endif
#   if ( (ETHTSYN_GLOBAL_TIME_SLAVE == STD_OFF) && ( ( ETHTSYN_TIME_BRIDGE_SM == STD_ON ) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_OFF) ) )
        (void) IdxDomain_u8;
#   endif
    }
}
#  endif

#  if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) \
        || ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) )|| defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
/**
 *********************************************************************************************************************
 * \Function Name : EthTSyn_GetEgressTimeStamp()
 *
 * \Function Description
 * Calls EthIf_GetEgressTimeStamp and resets the attributes for the relevant message if Time Quality is not ETH_VALID
 *
 * Parameters (in) :
 * \param CtrlrIdx_u8      - Index of the Ethernet controller within the context of the Ethernet Interface
 * \param BufIdx_u8        - Buffer pointer
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8     - Index of the Master Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgType_pu8      - Pointer to Message Type
 *
 * Parameters (out):
 * \param TimeStampPtr_pst - Pointer to Eth_TimeStampType structure
 *
 * Return Value: None
 * Return Type : void
 *********************************************************************************************************************
 */
static void EthTSyn_GetEgressTimeStamp( uint8               CtrlrIdx_u8,
                                        uint8               BufIdx_u8,
                                        Eth_TimeStampType  *TimeStampPtr_pst,
                                        uint8              *MsgType_pu8,
                                        uint8               IdxDomain_u8,
                                        uint8               IdxGlbTim_u8 )
{
    /* Local variable declaration */
    Eth_TimeStampQualType      lTimeQual_en;
#   if ( ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) ) && ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) )
    const EthTSyn_PortCfg_tst *lPortCfg_pcst;

    /* Local variable initialisation */
    lPortCfg_pcst = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst;
#   endif

    /* TRACE[SWS_EthTSyn_000127] : On invocation of EthTSyn_TxConfirmation() the egress time stamp shall be retrieved for Sync via EthIf_GetEgressTimeStamp()
     *  used for transmission delay compensation of the Sync message, if EthTSynHardwareTimestampSupport is set to TRUE.*/
    EthIf_GetEgressTimeStamp( CtrlrIdx_u8, BufIdx_u8, &lTimeQual_en, TimeStampPtr_pst);

    /* Reset the state to INIT if Time Quality is not Eth_Valid and assign an invalid number to MessageType so
     * default case of switch statement will be executed in EthTSyn_TxConfirmation.
     * Reset the respective counters as well */
    if( ETH_VALID != lTimeQual_en )
    {
#   if (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON)
        if( ETHTSYN_SYNC_TYPE == ( *MsgType_pu8 ) )
        {
#    if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
            /* Reset Master state machine to INIT if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_Master_ast[IdxGlbTim_u8].Sync_Mst_en                                     = ETHTSYN_SYNC_INIT_E;

            /* Reset counter for Sync transmission counter to configured value */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_u32;
#    else
            /* Set the state of sync message received to Invalid state */
            EthTSyn_TimBrdg_st.SyncMsgRxd_u8 = ETHTSYN_MSG_INV;

            (void) IdxGlbTim_u8;
#    endif
        }
        else
#   else
            (void) IdxGlbTim_u8;
#   endif
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#    if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
        if( ETHTSYN_PDELAY_REQ_TYPE == ( *MsgType_pu8 ) )
        {
            /* Reset Pdelay Initiator State machine to INIT if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                                               = ETHTSYN_TX_PDELAY_REQ_INIT_E;

            /* Reset counter for Pdelay Request Transmission Period if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX]            = lPortCfg_pcst->PdelayCfg_pcst->PdelayReqTxPerd_u32;

            /* Reset counter for Pdelay Response and Response FollowUp timeout if TimeQuality returned by lower layer is not ETH_VALID */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = lPortCfg_pcst->PdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16;
        }
        else
#    endif
#    if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
        if( ETHTSYN_PDELAY_RESP_TYPE == ( *MsgType_pu8 ) )
        {
            /* Reset Pdelay Responder state machine to INIT if time quality returned by lower layer is not ETH_VALID */
            EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_INIT_E;
        }
        else
#    endif
#   endif
        {
            /* Do Nothing */
        }
        ( *MsgType_pu8 ) = ETHTSYN_FFU;

#   if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: invalid timestamp received */
        /* DET will hit after reseting counters and state machines */
        (void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_GETEGRESSTIMESTAMP, ETHTSYN_E_INV_TIMESTAMP );
#   endif
    }

#   if ( ( ( ETHTSYN_GLOBAL_TIME_MASTER != STD_ON ) && ( ETHTSYN_PDELAY_INIT_SM_ENABLED != STD_ON ) ) || ( ETHTSYN_TIME_BRIDGE_SM == STD_ON ) )
    (void) IdxDomain_u8;
#   endif
}
#  endif

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) )
/**
 ******************************************************************************************************************
 * \Function Name : EthTSyn_GetHwTime()
 *
 * \Function Description
 * This API is used to obtain Current Time and Egress Time from Hardware
 *
 * Parameters (in) :
 * \param CtrlrIdx_u8             - Index of the Ethernet controller within the context of the Ethernet Interface
 * \param BufIdx_u8               - Buffer pointer
 * \param IdxDomain_u8            - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8            - Index of the Master Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgType_pu8             - Pointer to Message Type
 *
 * Parameters (out):
 * \param TimeStampPtrCurrent_pst - Pointer to Eth_TimeStampType structure for precision time calculation
 *
 * Return Value: None
 * Return Type : void
 ******************************************************************************************************************
 */
static void EthTSyn_GetHwTime( uint8              CtrlrIdx_u8,
                               uint8              BufIdx_u8,
                               uint8             *MsgType_pu8,
                               Eth_TimeStampType *TimeStampPtrCurrent_pst,
                               uint8              IdxDomain_u8,
                               uint8              IdxGlbTim_u8 )
{
    /* Local variable declaration */
#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    Eth_TimeIntDiffType        lSyncTxVirtualTime_st;
    Eth_TimeIntDiffType        lPreciseTime_st;
    Eth_TimeStampType          lTxTime_st;
    Eth_TimeStampType          lTimeStamp_st;
    StbM_VirtualLocalTimeType  lLocalTime_st;
    StbM_TimeStampType        *lSyncGlobalTime_pst;
    uint64                     lSyncTxTimeStamp_u64;
    uint64                     lActualTimeStamp_u64;
    Eth_TimeIntDiffType        lSyncCurrentTimeStamp_st;
    Eth_TimeIntDiffType        lSyncTxCurrentTime_st;
    uint64                     lVirtualLocalTime_u64;
    uint64                     lCurrentTimeStamp_u64;
    uint64                     lCurrentVirtualLocalTime_u64;
    uint64                     lTxDelay_u64;
    Std_ReturnType             lRetVal_en;

    /* Local variable initialisation */
    lTimeStamp_st.nanoseconds = ETHTSYN_ZERO;
    lTimeStamp_st.seconds     = ETHTSYN_ZERO;
    lTimeStamp_st.secondsHi   = ETHTSYN_ZERO;
    lRetVal_en                = E_OK;
#  endif

    /* This API is used to call EthIf_GetEgressTimeStamp */
    EthTSyn_GetEgressTimeStamp( CtrlrIdx_u8, BufIdx_u8, TimeStampPtrCurrent_pst, MsgType_pu8, IdxDomain_u8, IdxGlbTim_u8 );

#   if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    if( ETHTSYN_SYNC_TYPE == ( *MsgType_pu8 ) )
    {
        /* Store the received Eth PTP clock time */
        lSyncCurrentTimeStamp_st.diff = *TimeStampPtrCurrent_pst;
        lSyncCurrentTimeStamp_st.sign = TRUE;

        /* Convert the received Eth PTP clock time, which is in Eth_TimeStampType structure into a 64 bit variable, that stores time T2vlt */
        EthTSyn_CnvEthTitoHex( lSyncCurrentTimeStamp_st, ETHTSYN_ZERO, &lVirtualLocalTime_u64 );

        /* Assign time obtained from StbM_BusGetCurrentTime to an Eth_TimeStampType structure ( T0 ) */
        lSyncGlobalTime_pst     = &EthTSyn_Master_ast[IdxGlbTim_u8].SyncGlobalTime_st;
        lTxTime_st.nanoseconds = lSyncGlobalTime_pst->nanoseconds;
        lTxTime_st.seconds     = lSyncGlobalTime_pst->seconds;
        lTxTime_st.secondsHi   = lSyncGlobalTime_pst->secondsHi;

        if ( ETHTSYN == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].StbM_LocalTimeHardware_en )
        {
            /* Subtract the virtual local time stored (T2vlt) from the time stored in the global datastructure (T0vlt)
             * that is., perform ( T0vlt - T2vlt ) */
            lCurrentTimeStamp_u64 = EthTSyn_Master_ast[IdxGlbTim_u8].SyncVirtualLocalTime_u64 - lVirtualLocalTime_u64;

            /* Convert and store the current time in Eth_TimeIntDiffType structure.
             * This time value will be then used for preciseOriginTimestamp calculation.*/
            EthTSyn_CnvToEthTi(lCurrentTimeStamp_u64, ETHTSYN_ZERO, &lSyncTxVirtualTime_st );

            /* Calculate precise origin timestamp by subtracting from the time obtained from StbM_BusGetCurrentTime
             * that is., perform T0 - (T0VLT - T2VLT) */
            EthTSyn_SubTi( &lTxTime_st, &lSyncTxVirtualTime_st.diff, &lPreciseTime_st );

            /* Store calculated value of precise origin TimeStamp */
            TimeStampPtrCurrent_pst->nanoseconds = lPreciseTime_st.diff.nanoseconds;
            TimeStampPtrCurrent_pst->seconds     = lPreciseTime_st.diff.seconds;
            TimeStampPtrCurrent_pst->secondsHi   = lPreciseTime_st.diff.secondsHi;
        }
        else
        {
            /* Call StbM_GetCurrentVirtualLocalTime to obtain the current time */
            lRetVal_en = EthTSyn_GetCurrentVirtualLocalTime( MsgType_pu8, &lLocalTime_st, &lTimeStamp_st, ETHTSYN_TRANSMISSION, IdxDomain_u8 );

            if( E_OK == lRetVal_en )
            {
                /* Store the received PTP clock time */
                lSyncTxCurrentTime_st.diff = lTimeStamp_st;
                lSyncTxCurrentTime_st.sign = TRUE;

                /* Convert the received Eth PTP clock time, which is in Eth_TimeStampType structure into a 64 bit variable, that stores time T3vlt */
                EthTSyn_CnvEthTitoHex( lSyncTxCurrentTime_st, ETHTSYN_ZERO, &lCurrentVirtualLocalTime_u64 );

                /* Subtract the received virtual local time (T3vlt) and the time obtained from EthIf_GetEgressTimeStamp (T2vlt)
                 * that is., perform ( T3vlt - T2vlt ) */
                lTxDelay_u64 = lCurrentVirtualLocalTime_u64 - lVirtualLocalTime_u64;

                /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value ( T4vlt ) */
                lSyncTxTimeStamp_u64 = ( uint64 )( ( ( ( uint64 ) lLocalTime_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lLocalTime_st.nanosecondsLo );

                /* Subtract the virtual local time obtained from StbM_GetCurrentVirtualLocalTime ( T4vlt ) and the time stored in the global datastructure (T0vlt)
                 * that is., perform ( T4vlt - T0vlt ) */
                lActualTimeStamp_u64 = lSyncTxTimeStamp_u64 - EthTSyn_Master_ast[IdxGlbTim_u8].SyncVirtualLocalTime_u64;

                /* Add the time ( T3vlt - T2vlt ) + ( T4vlt - T0vlt ) which will be used for PreciseOriginTimeStamp calculation. */
                lCurrentTimeStamp_u64 = lTxDelay_u64 + lActualTimeStamp_u64;

                /* Convert and store the current time in Eth_TimeIntDiffType structure.
                 * This time value will be then used for preciseOriginTimestamp calculation.*/
                EthTSyn_CnvToEthTi( lCurrentTimeStamp_u64, ETHTSYN_ZERO, &lSyncTxVirtualTime_st );

                /* Calculate precise origin timestamp by subtracting from the time obtained from StbM_BusGetCurrentTime
                 * that is., perform T0 - (T3VLT - T2VLT) + (T4VLT - T0VLT) */
                EthTSyn_SubTi( &lTxTime_st, &lSyncTxVirtualTime_st.diff, &lPreciseTime_st );

                /* Store calculated value of precise origin TimeStamp */
                TimeStampPtrCurrent_pst->nanoseconds = lPreciseTime_st.diff.nanoseconds;
                TimeStampPtrCurrent_pst->seconds     = lPreciseTime_st.diff.seconds;
                TimeStampPtrCurrent_pst->secondsHi   = lPreciseTime_st.diff.secondsHi;
            } /* If return value from EthTSyn_GetCurrentVirtualLocalTime is E_OK */
        } /* If StbM is not referring to ETH free running timer */
    } /* If message transmitted is a Sync message */
#   endif
} /* Function End */
#   endif
#  endif
# endif

/**
 *******************************************************************************************************************
 * \Function Name : EthTSyn_GetCurrentVirtualLocalTime()
 *
 * \Function Description
 * Calls StbM_GetCurrentVirtualLocalTime and resets the attributes for the relevant message if E_NOT_OK is returned
 *
 * Parameters (in) :
 * \param Dir_u8               - Direction of traffic. i.e, TRANSMISSION or RECEPTION
 * \param IdxDomain_u8         - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * \param MsgType_pu8          - Pointer to Message Type
 *
 * Parameters (out):
 * \param VirtualLocalTime_pst - StbM_VirtualLocalTimeType structure to store Virtual Local Time Value
 * \param TimeStamp_pst        - Eth_TimeStampType structure to store Time returned from EthIf_GetCurrentTime
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 *******************************************************************************************************************
 */
Std_ReturnType EthTSyn_GetCurrentVirtualLocalTime( uint8                     *MsgType_pu8,
                                                   StbM_VirtualLocalTimeType *VirtualLocalTime_pst,
                                                   Eth_TimeStampType         *TimeStamp_pst,
                                                   uint8                      Dir_u8,
                                                   uint8                      IdxDomain_u8 )
{
    /* Local variable declaration */
    Std_ReturnType             lRetVal_en;
#   if ( ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON ) ) || ( ETHTSYN_DEV_ERROR_DETECT == STD_ON ) )
    Std_ReturnType             lResult_en;
    Eth_TimeStampQualType      lTimeQual_en;
#   endif
#   if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    uint8                      lIdxGlbTim_u8;
#   endif
#   if ( (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON) || (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) || ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) ) )
    const EthTSyn_PortCfg_tst *lPortCfg_pcst;

    /* Local variable initialisation */
    lPortCfg_pcst = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst;
#   endif

    /* Local variable initialization*/
    lRetVal_en   = E_NOT_OK;
#   if ( ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON ) ) || ( ETHTSYN_DEV_ERROR_DETECT == STD_ON ) )
    lResult_en   = E_OK;
    lTimeQual_en = ETH_VALID;
#   endif

    /* Critical section as current virtual time is obtained */
    SchM_Enter_EthTSyn();

#   if ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON ) )
    /* The access to controller Index has to be updated with Multi-Domain as with TimeBridge same controller Index is used */
    /* When Hardware Timestamping is enabled and irrespective of StbM referring to ETHTSYN or not, EthIf_GetCurrentTime is called to obtain the current time value ( T2 )  */
    lResult_en = EthIf_GetCurrentTime( EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[ETHTSYN_ZERO].EthIfCtrlrIdx_u8, &lTimeQual_en, TimeStamp_pst );
#   else
    TimeStamp_pst->nanoseconds = ETHTSYN_ZERO;
    TimeStamp_pst->seconds     = ETHTSYN_ZERO;
    TimeStamp_pst->secondsHi   = ETHTSYN_ZERO;
#   endif

    /* On invocation of EthTSyn_TxConfirmation() the egress time stamp shall be retrieved for t1 (Pdelay_Req) from the
     * StbM via StbM_GetCurrentVirtualLocalTime() */
    /* On invocation of EthTSyn_TxConfirmation() the responseOriginTimestamp t4 valid for Pdelay_Resp_Follow_Up shall be retrieved
     * from the StbM via StbM_GetCurrentVirtualLocalTime() on egress of Pdelay_Resp */
    lRetVal_en = StbM_GetCurrentVirtualLocalTime( EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].StbM_SyncdTimeBasId_u16, VirtualLocalTime_pst );

    /* Critical section ends */
    SchM_Exit_EthTSyn();

    /* Reset the state to INIT if Return Value is E_NOT_OK and assign an invalid number to MessageType so
     * default case of switch statement will be executed in EthTSyn_TxConfirmation.
     * Reset the counters as well */
    if ( ( E_OK != lRetVal_en )
#   if( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON ) )
        || ( E_OK != lResult_en ) || ( ETH_VALID != lTimeQual_en )
#   endif
       )
    {
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#    if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
        /* Reset Pdelay Initiator State machine and its counters if E_NOT_OK is returned by StbM_GetCurrentVirtualLocalTime
         * for Pdelay Request message for TRANSMISSION or for Pdelay Response message for RECEPTION */
        if( ( ( ETHTSYN_TRANSMISSION == Dir_u8 ) && ( ETHTSYN_PDELAY_REQ_TYPE  == ( *MsgType_pu8 ) ) )
            ||  ( ( ETHTSYN_RECEPTION    == Dir_u8 ) && ( ETHTSYN_PDELAY_RESP_TYPE == ( *MsgType_pu8 ) ) ) )
        {
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                                               = ETHTSYN_TX_PDELAY_REQ_INIT_E;
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX]            = lPortCfg_pcst->PdelayCfg_pcst->PdelayReqTxPerd_u32;
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = lPortCfg_pcst->PdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16;
        }
        else
#    endif
#    if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
        /* Reset Pdelay Responder State machine if E_NOT_OK is returned by StbM_GetCurrentVirtualLocalTime for Pdelay
         * Response message for TRANSMISSION or for Pdelay Request message for RECEPTION */
        if( ( ( ETHTSYN_TRANSMISSION == Dir_u8 ) && ( ETHTSYN_PDELAY_RESP_TYPE == ( *MsgType_pu8 ) ) )
            ||  ( ( ETHTSYN_RECEPTION    == Dir_u8 ) && ( ETHTSYN_PDELAY_REQ_TYPE  == ( *MsgType_pu8 ) ) ) )
        {
            EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_INIT_E;
        }
        else
#    endif
#   endif
        if( ETHTSYN_SYNC_TYPE == ( *MsgType_pu8 ) )
        {
#   if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#    if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
            if ( NULL_PTR != EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst )
            {
                lIdxGlbTim_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->GlbTimMstIdx_u8;
                /* Reset Master state machine and its counter if E_NOT_OK is returned by StbM_GetCurrentVirtualLocalTime for RECEPTION
                 * of Sync message */
                EthTSyn_Master_ast[lIdxGlbTim_u8].Sync_Mst_en                                    = ETHTSYN_SYNC_INIT_E;
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_u32;
            }
            else
#    endif
#    if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
            if ( NULL_PTR != EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst )
            {
                lIdxGlbTim_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;
                /* Reset Slave state machine and its counter if E_NOT_OK is returned by StbM_GetCurrentVirtualLocalTime for RECEPTION
                 * of Sync message */
                EthTSyn_Slave_ast[lIdxGlbTim_u8].Sync_Slave_en                                   = ETHTSYN_SYNC_INIT_E;
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = lPortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst->Sync_SyncFolwUpTout_u32;
            }
            else
#    endif
            {
                /* Do Nothing */
            }
#   else
            /* Reset Slave state machine and its counter if E_NOT_OK is returned by StbM_GetCurrentVirtualLocalTime for RECEPTION
             * of Sync message */
            EthTSyn_Slave_ast[0].Sync_Slave_en                                               = ETHTSYN_SYNC_INIT_E;
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = lPortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst->Sync_SyncFolwUpTout_u32;
#   endif
        }
        else
        {
            /* Do Nothing */
        }

        /* Reset the message type */
        ( *MsgType_pu8 )         = ETHTSYN_FFU;

        /* Set the return value as E_NOT_OK */
        lRetVal_en               = E_NOT_OK;

#   if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Function call to do the DET check */
        EthTSyn_GetCurrentVirtualLocalTimeDetChk( lResult_en, lTimeQual_en );
#   endif

#   if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_OFF ) && ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_OFF ) )
        /* Dir_u8 is not used if Pdelay Initiator and Responder are not enabled */
        (void) Dir_u8;
#   endif
    } /* If return value from EthTSyn_GetCurrentVirtualLocalTime and EthIf_GetCurrentTime is E_NOT_OK and Time quality value is ETH_INVALID */
    return lRetVal_en;
} /* Function End */

#  if ( ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) && ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF ) )
#   if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) )
/**
 ***************************************************************************************************************
 * \Function Name : EthTSyn_GetSwTime()
 *
 * \Function Description
 * This API is used to obtain Current Time when software time stamping is enabled
 *
 * Parameters (in) :
 * \param IdxDomain_u8            - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8            - Index of the Master Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgType_pu8             - Pointer to Message Type
 *
 * Parameters (out):
 * \param TimeStampPtrCurrent_pst - Pointer to Eth_TimeStampType structure for precision time calculation
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************************
 */
static void EthTSyn_GetSwTime( uint8             *MsgType_pu8,
                               Eth_TimeStampType *TimeStampPtrCurrent_pst,
                               uint8              IdxDomain_u8,
                               uint8              IdxGlbTim_u8 )
{
    /* Local variable declaration */
    uint64                     lSyncTxTimeStamp_u64;
    StbM_VirtualLocalTimeType  lLocalTime_st;
    Eth_TimeStampType          lTimeStamp_st;
    Eth_TimeIntDiffType        lPreciseTime_st;
    Std_ReturnType             lRetVal_en;
#    if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    StbM_TimeStampType        *lSyncGlobalTime_pst;
    Eth_TimeIntDiffType        lSyncTxVirtualTime_st;
    Eth_TimeIntDiffType        lCurrentTimestamp_st;
    Eth_TimeStampType          lTxTime_st;
    uint64                     lActualTimeStamp_u64;
#    endif

    /* Local variable initialization*/
    lTimeStamp_st.nanoseconds = ETHTSYN_ZERO;
    lTimeStamp_st.seconds     = ETHTSYN_ZERO;
    lTimeStamp_st.secondsHi   = ETHTSYN_ZERO;
    lRetVal_en                = E_OK;

    /* Call StbM_GetCurrentVirtualLocalTime to obtain the current time */
    lRetVal_en = EthTSyn_GetCurrentVirtualLocalTime( MsgType_pu8, &lLocalTime_st, &lTimeStamp_st, ETHTSYN_TRANSMISSION, IdxDomain_u8 );

    if( E_OK == lRetVal_en )
    {
        /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value ( T4vlt ) */
        lSyncTxTimeStamp_u64 = ( uint64 )( ( ( ( uint64 ) lLocalTime_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lLocalTime_st.nanosecondsLo );

#   if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
        if( ETHTSYN_SYNC_TYPE == ( *MsgType_pu8 ) )
        {
            /* Assign time obtained from StbM_BusGetCurrentTime to an Eth_TimeStampType structure ( T0 ) */
            lSyncGlobalTime_pst    = &EthTSyn_Master_ast[IdxGlbTim_u8].SyncGlobalTime_st;
            lTxTime_st.nanoseconds = lSyncGlobalTime_pst->nanoseconds;
            lTxTime_st.seconds     = lSyncGlobalTime_pst->seconds;
            lTxTime_st.secondsHi   = lSyncGlobalTime_pst->secondsHi;

            /* Subtract the virtual local time obtained from StbM_GetCurrentVirtualLocalTime ( T4vlt ) and the time stored in the global datastructure T0vlt
             * that is., perform ( T4vlt - T0vlt ) */
            lActualTimeStamp_u64 = lSyncTxTimeStamp_u64 - EthTSyn_Master_ast[IdxGlbTim_u8].SyncVirtualLocalTime_u64;

            /* Convert and store the current time in Eth_TimeIntDiffType structure */
            EthTSyn_CnvToEthTi( lActualTimeStamp_u64, ETHTSYN_ZERO, &lSyncTxVirtualTime_st );

            /* Store the converted time value which is of Eth_TimeIntDiffType */
            lCurrentTimestamp_st.diff = lTxTime_st;
            lCurrentTimestamp_st.sign = TRUE;

            /* Calculate precise origin timestamp as preciseOriginTimestamp = T0 + ( T4vlt - T0vlt )  */
            EthTSyn_AddTi( &lSyncTxVirtualTime_st, &lCurrentTimestamp_st, &lPreciseTime_st );

            /* Store calculated value of precise origin TimeStamp */
            TimeStampPtrCurrent_pst->nanoseconds = lPreciseTime_st.diff.nanoseconds;
            TimeStampPtrCurrent_pst->seconds     = lPreciseTime_st.diff.seconds;
            TimeStampPtrCurrent_pst->secondsHi   = lPreciseTime_st.diff.secondsHi;
        }
        else
#   else
            (void) IdxGlbTim_u8;
#   endif
#   if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
        if( ( ETHTSYN_PDELAY_REQ_TYPE == *MsgType_pu8 ) || ( ETHTSYN_PDELAY_RESP_TYPE == *MsgType_pu8 ) )
        {
            /* Convert and store the current time in Eth_TimeIntDiffType structure */
            EthTSyn_CnvToEthTi( lSyncTxTimeStamp_u64, ETHTSYN_ZERO, &lPreciseTime_st );

            /* Store calculated value of precise origin TimeStamp */
            TimeStampPtrCurrent_pst->nanoseconds = lPreciseTime_st.diff.nanoseconds;
            TimeStampPtrCurrent_pst->seconds     = lPreciseTime_st.diff.seconds;
            TimeStampPtrCurrent_pst->secondsHi   = lPreciseTime_st.diff.secondsHi;
        }
        else
#   endif
        {
            /* Do Nothing */
        }
    } /* If return value from EthTSyn_GetCurrentVirtualLocalTime is E_OK */
} /* Function End */
#   endif
#  endif

/**
 **************************************************************************************************************************
 * \Function Name : EthTSyn_RxIndication()
 *
 * \Function Description
 * EthTSyn_00040: By this API service the EthTSyn gets an indication and the data of a received frame.
 * \par Service ID 0x06, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * \param CtrlIdx      - Index of the Ethernet controller
 * \param FrameType    - frame type of received Ethernet frame
 * \param IsBroadcast  - parameter to indicate a broadcast frame
 * \param PhysAddrPtr  - pointer to Physical source address (MAC address in network byte order) of received Ethernet frame
 * \param DataPtr      - Pointer to payload of the received Ethernet frame (i.e. Ethernet header is not provided)
 * \param LenByte      - Length of received data
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 **************************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: According to 'Specification of Ethernet Interface AUTOSAR Release 4.1.1', in EthIf_RxIndication() parameter PhysAddrPtr is of type Pointer to unit8 */
void EthTSyn_RxIndication( uint8         CtrlIdx,
                           Eth_FrameType FrameType,
                           boolean       IsBroadcast,
                           uint8        *PhysAddrPtr,
                           uint8        *DataPtr,
                           uint16        LenByte )
{

    /* Local Variable declaration */
# if ( (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON) || (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON) )
    Eth_TimeStampType         lTimeStamp_st;
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    uint16                    lsntPortNumber_u16;
    uint16                    lrcvdPortNumber_u16;
#  endif
#  if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF )
    StbM_VirtualLocalTimeType lVirtualLocalTime_st;
    Std_ReturnType            lResult_en;
#  endif
# if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF )
    Eth_TimeIntDiffType       lSyncTxVirtualTime_st;
    uint64                    lSyncRxTimeStamp_u64;
# endif
    uint16                    lMsgLen_u16;
# endif
# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
#  if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
    uint8                     lIdxGlbTimSlv_u8;
    uint8                     lSwtMngtEthSwtPort_u8;
#   if( ( ETHTSYN_TIME_BRIDGE_SM == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
    uint8                     lMstPortNum_u8;
#   endif
#  endif
# endif
    uint8                     lMsgType_u8;
    uint8                     lCtrlIdx_u8;
    uint8                     lIdxDom_u8;
    uint8                     lIdxPort_u8;
    Std_ReturnType            lRetVal_en;

    /* Local Variable Initialization */
# if ( ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
    lTimeStamp_st.nanoseconds = ETHTSYN_ZERO;
    lTimeStamp_st.seconds     = ETHTSYN_ZERO;
    lTimeStamp_st.secondsHi   = ETHTSYN_ZERO;
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    lrcvdPortNumber_u16       = ETHTSYN_ZERO;
    lsntPortNumber_u16        = ETHTSYN_ZERO;
#  endif
    lMsgLen_u16               = ETHTSYN_ZERO;
# endif
    lMsgType_u8               = 0xFFU;
    lIdxDom_u8                = ETHTSYN_ZERO;
    lIdxPort_u8               = ETHTSYN_ZERO;
    lRetVal_en                = E_NOT_OK;

    (void) FrameType;
    (void) IsBroadcast;

    /* Function call to do the DET check */
    lRetVal_en = EthTSyn_RxIndicationDetChk( CtrlIdx, PhysAddrPtr, DataPtr, &lIdxDom_u8, &lIdxPort_u8 );

    /* Condition checks: If there is no DET error when DET compiler macro is switched On. */
    if( E_OK == lRetVal_en  )
    {
        /* This initialisation has to be updated with Multi-Domain as with TimeBridge same controller Index is used */
        lCtrlIdx_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].EthIfCtrlrIdx_u8;

        /* Extract Message Type only if received CtrlIdx matches to the configured EthIf Controller Index and length is atleast four bytes */
        if( ( lCtrlIdx_u8 == CtrlIdx ) && ( ETHTSYN_FOUR <= LenByte ) )
        {
            lMsgType_u8 = ( DataPtr[0] & 0x0FU );
# if ( ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
            lMsgLen_u16 = ( ( uint16 )( ( ( uint16 )DataPtr[2] ) << 8U ) | DataPtr[3] );
# endif
        }

# if( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) )
#  if ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON )
        /* If the received message is Sync by a Time Master, report DET error */
        /* Report DET: Development Error: Runtime error when master receives a sync from another master */
#   if ( ETHTSYN_DEV_ERROR_DETECT  == STD_ON )
        if ( ETHTSYN_SYNC_TYPE == lMsgType_u8 )
        {
            ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RXINDICATION, ETHTSYN_E_TMCONFLICT );
        }
#   endif
#  endif
        (void) lMsgType_u8;
# endif

# if ( ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
        /* Obtain the values of Source Port from SourcePortIdentity of Pdelay Request message and Requesting Port from
         * RequestingPortIdentity of Pdelay Response message so on the ingress of various Pdelay Response messages,
         * we process only the Pdelay Response meant for us */
        if( ETHTSYN_PDELAY_RESP_TYPE == lMsgType_u8 )
        {
            lrcvdPortNumber_u16 = ( ( uint16 )( ( ( uint16 ) DataPtr[ETHTSYN_FIFTYTWO] ) << ETHTSYN_EIGHT  ) | ( DataPtr[ETHTSYN_FIFTYTHREE] ) );
            lsntPortNumber_u16  = EthTSyn_RamDataTyp_ast[lIdxDom_u8].VarHeader_st.SourcePortIdentity_st.PortNumber_u16;
        }
#  endif

        /* If the received message is Sync or Pdelay_Req or Pdelay_Resp and if the HardwareTimeStamp support is true
        * then get the Ingress Timestamp of the received message. And check the Source port and Requesting port for Pdelay
        * Request and Pdelay Response respectively */
        if( ( ETHTSYN_SYNC_TYPE          == lMsgType_u8 )
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
        ||  ( ETHTSYN_PDELAY_REQ_TYPE    == lMsgType_u8 )
        ||  ( ( ETHTSYN_PDELAY_RESP_TYPE == lMsgType_u8 ) && ( lrcvdPortNumber_u16 == lsntPortNumber_u16 ) )
#   endif
          )
        {
#  if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
            /* This API is used to call EthIf_GetIngressTimeStamp to obtain PTP clock Time ( T1 ) */
            EthTSyn_GetIngressTimeStamp( CtrlIdx, DataPtr, &lTimeStamp_st, &lMsgType_u8, lIdxDom_u8, lIdxPort_u8 );
#  else
            /* On invocation of EthTSyn_RxIndication a reference time ( T1vlt ) shall be retrieved for Sync
             * via StbM_GetCurrentVirtualLocalTime() from StbM used for reception delay compensation of the time synchronization process */
            lResult_en = EthTSyn_GetCurrentVirtualLocalTime( &lMsgType_u8, &lVirtualLocalTime_st, &lTimeStamp_st, ETHTSYN_RECEPTION, lIdxDom_u8 );

            if( E_OK == lResult_en )
            {
                /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value  */
                lSyncRxTimeStamp_u64 = ( uint64 )( ( ( ( uint64 ) lVirtualLocalTime_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lVirtualLocalTime_st.nanosecondsLo );

                /* Convert and store the current time in Eth_TimeIntDiffType structure */
                EthTSyn_CnvToEthTi( lSyncRxTimeStamp_u64, ETHTSYN_ZERO, &lSyncTxVirtualTime_st );

                /* Assign SyncTxVirtualTime_st to Eth_TimeStampType structure that is further used for Pdelay calculations ( T1 ) */
                lTimeStamp_st = lSyncTxVirtualTime_st.diff;
            }
#  endif

#  if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
#   if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
            if( ETHTSYN_SYNC_TYPE == lMsgType_u8 )
            {
                if ( NULL_PTR == EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst )
                {
                    /* Return to the calling context if Time Slave is not configured */
                    return;
                }
                /* Index of the current global time slave */
                lIdxGlbTimSlv_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;
                lSwtMngtEthSwtPort_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].SwtMngtEthSwtPort_u8;

                /* Call EthIf_GetRxMgmtObject so we get the address of Management Object Structure from switch */
                /* This will have to be updated in future when Pdelay mechanism is updated with GTS as currently Pdelay mechanism is not supported with GTS */
                lRetVal_en = EthIf_GetRxMgmtObject( CtrlIdx, lSwtMngtEthSwtPort_u8, &( EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst ) );

#    if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
                /* Initialise the length of frame till time sub TLV to zero */
                EthTSyn_TimBrdg_st.TiSubTLVLen_u16  = ETHTSYN_ZERO;
#    endif

                if( E_OK != lRetVal_en )
                {
                    /* Reset the state of Slave SM */
                    EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en                                   = ETHTSYN_SYNC_INIT_E;

                    /* Reset the main function counter for timeout value between sync and FollowUp */
                    EthTSyn_RamDataTyp_ast[lIdxDom_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]      = ETHTSYN_ALLFFL;

#    if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )

                    /* Reset the Sync message received to invalid */
                    EthTSyn_TimBrdg_st.SyncMsgRxd_u8                                                    = ETHTSYN_MSG_INV;
#    endif

                    /* Reset the message type so switch default case is hit */
                    lMsgType_u8                                                                         = 0xFFU;

                    /* Set management object ownership to unused so this datastructure is free and switch can use it when EthIf_GetRxMgmtObject is called again */
                    EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;

#    if( ( ETHTSYN_TIME_BRIDGE_SM == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
                    for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
                    {
                        if( NULL_PTR != EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst )
                        {
                            /* Set management object ownership to unused so this datastructure is free and switch can use it when EthIf_GetRxMgmtObject is called again */
                            EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
                        }
                    }
#    endif
#    if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
                    /* Report DET: Development Error: invalid return value */
                    ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RXINDICATION, ETHTSYN_E_INV_RETURNVALUE );
#    endif
                }
            } /* If switch is defined and EthTSynGlobalTimeRxToUplinkSwitchResidenceTime is not configured */
#   endif
#  endif
            } /* If the received message is Sync or Pdelay_Req or Pdelay_Resp */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
            else if( ETHTSYN_PDELAY_RESP_TYPE == lMsgType_u8 )
            {
                lMsgType_u8 = 0xFFU;
            }
#  endif
            else
            {
                /* Do Nothing */
            }

        /* Check the message type */
        switch( lMsgType_u8 )
        {
    /* If the configured device is Time Slave, update the local datastructures with the received Ingress Timestamp */
#  if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
            case ETHTSYN_SYNC_TYPE:
            {
                /* Call the corresponding internal function */
                EthTSyn_ReceiveSync ( ( const uint8 * ) DataPtr, &lTimeStamp_st, lMsgLen_u16, LenByte, lIdxDom_u8, lIdxPort_u8 );
            }
            break;

            case ETHTSYN_FOLLOW_UP_TYPE:
            {
                /* Call the corresponding internal function */
                EthTSyn_ReceiveFollowUp ( ( const uint8 * ) DataPtr, lMsgLen_u16, LenByte, lIdxDom_u8, lIdxPort_u8 );
            }
            break;
#  endif

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
            case ETHTSYN_PDELAY_REQ_TYPE:
            {
                /* Call the corresponding internal function */
                EthTSyn_ReceivePdelayReq ( ( const uint8 * ) DataPtr, &lTimeStamp_st, lMsgLen_u16, LenByte, lIdxDom_u8 );
            }
            break;
#   endif

#   if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
            case ETHTSYN_PDELAY_RESP_TYPE:
            {
                /* Call the corresponding internal function */
                EthTSyn_ReceivePdelayResp ( ( const uint8 * ) DataPtr, &lTimeStamp_st, lMsgLen_u16, LenByte, lIdxDom_u8 );
            }
            break;

            case ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE:
            {
                /* Call the corresponding internal function */
                EthTSyn_ReceivePdelayRespFollowUp ( ( const uint8 * ) DataPtr, lMsgLen_u16, LenByte, lIdxDom_u8 );
            }
            break;
#   endif
#  endif

            default:
                /* Intentionally Empty */
            break;
        } /* Switch Statement for checking message type */
# endif
    }
} /* Function End */

/**
 *********************************************************************************************************
 * \Function Name : EthTSyn_TrcvLinkStateChg()
 *
 * \Function Description
 * EthTSyn_00041: This API confirms the transmission of an Ethernet frame
 * \par Service ID 0x07, Synchronous, Non Re-entrant
 *
 * Parameters (in) :
 * \param CtrlIdx        - Index of the Ethernet controller within the context of the Ethernet Interface
 * \param TrcvLinkState  - ETHTRCV_LINK_STATE_DOWN, ETHTRCV_LINK_STATE_ACTIVE
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: E_NOT_OK / E_OK
 * Return Type : void
 *********************************************************************************************************
 */
void EthTSyn_TrcvLinkStateChg( uint8                 CtrlIdx,
                               EthTrcv_LinkStateType TrcvLinkState )
{
    /* Local Variable Declaration */
    EthTSyn_RamDataTyp_tst *lEthTSyn_RamDataTyp_pst;
    uint8                  *lClockIdentity_pau8;
# if( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
#  if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    EthTSyn_SubTLV_tst     *lSubTLV_pst;
#  endif
    uint8                   lIdxGlbTimSlv_u8;
# endif
    Std_ReturnType          lResult_en;
    uint8                   lMacAddr_au8[6];
    uint8                   lIdxDom_u8;
    uint8                   lIdxPort_u8;
    uint8                   lCtrlIdx_u8;
# if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) )
    uint8                   lIdxGlbTimMst_u8;
# endif

    /* Local Variable Initialization */
# if( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
#  if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    lSubTLV_pst   = NULL_PTR;
#  endif
# endif
    lResult_en    =   E_NOT_OK;
    lCtrlIdx_u8   =   ETHTSYN_FFU;

    if ( ETHTSYN_INIT_E == EthTSyn_InitStatus_en )
    {
        /* Function call to obtaiin domin index and port index */
        EthTSyn_DomainNumGenn( CtrlIdx, &lIdxDom_u8, &lIdxPort_u8, &lCtrlIdx_u8 );

        if ( lCtrlIdx_u8 == CtrlIdx )
        {
            /* Initialize local pointer */
            lEthTSyn_RamDataTyp_pst = &EthTSyn_RamDataTyp_ast[lIdxDom_u8];

            if( ETHTRCV_LINK_STATE_ACTIVE == TrcvLinkState )
            {
                /* TRACE[SWS_EthTSyn_00020]: Restarts the transmission and reception of TSyn messages */
                /* Set the Link status to be TRUE */
                lEthTSyn_RamDataTyp_pst->LinkState_b = TRUE;

                /* Set the filter for PTP multicast MAC */
                lResult_en = EthIf_UpdatePhysAddrFilter( CtrlIdx, EthTSyn_Multicast_mac_au8, ETH_ADD_TO_FILTER );

                /* Get the MAC Address */
                EthIf_GetPhysAddr( CtrlIdx , lMacAddr_au8 );

                /* Update Clock Identity */
                lClockIdentity_pau8 = &lEthTSyn_RamDataTyp_pst->VarHeader_st.SourcePortIdentity_st.ClockIdentity_au8[0];
                lClockIdentity_pau8[0] = lMacAddr_au8[0];
                lClockIdentity_pau8[1] = lMacAddr_au8[1];
                lClockIdentity_pau8[2] = lMacAddr_au8[2];
                lClockIdentity_pau8[3] = 0xFFU;
                lClockIdentity_pau8[4] = 0xFEU;
                lClockIdentity_pau8[5] = lMacAddr_au8[3];
                lClockIdentity_pau8[6] = lMacAddr_au8[4];
                lClockIdentity_pau8[7] = lMacAddr_au8[5];
            }
            else
            {
                /* TRACE[SWS_EthTSyn_00019]: Reset the state machines */
                /* Re-Initialise the State machines when the link is not active */
# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
                /* Initialise Pdelay Initiator state to Init state */
                EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en  = ETHTSYN_TX_PDELAY_REQ_INIT_E;
#  endif

#  if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
                /* Initialise Pdelay Responder state to Init state */
                EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_INIT_E;
#  endif

#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
                if ( NULL_PTR != EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeMst_pcst )
                {
                    /* Index of the current global time master datastructure */
                    lIdxGlbTimMst_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeMst_pcst->GlbTimMstIdx_u8;
                    /* Initialise time master state to Init state */
                    EthTSyn_Master_ast[lIdxGlbTimMst_u8].Sync_Mst_en = ETHTSYN_SYNC_INIT_E;
                }
#  endif

                /* Re-Initialise the Main Function timers when the link is not active */
#  if ( ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON )
                /* Initialise the debounce counter to zero */
                lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX]            =   ETHTSYN_ZERO;
#  endif
                /* Initialize Main Function Pdelay_req timer with Zero if Pdelay measurement is enabled */
                lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX]  =   ETHTSYN_ZERO;
# endif
                /* Initialise main function counter for Master as zero */
                lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]   =   ETHTSYN_ZERO;

# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
                if ( NULL_PTR != EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst )
                {
                    /* Index of the current global time master datastructure */
                    lIdxGlbTimSlv_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDom_u8].PortCfg_pcst[lIdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;
                    /* Initialise time slave state to Init state */
                    EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_INIT_E;
#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
                    lSubTLV_pst = &EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st;
                    /* Set the type of Time sub TLV to zero */
                    lSubTLV_pst->TimeSubTLV_st.Type_u8      = ETHTSYN_ZERO;
                    /* Set the type of Status sub TLV to zero */
                    lSubTLV_pst->StatusSubTLV_st.Type_u8    = ETHTSYN_ZERO;
                    /* Set the type of User Data sub TLV to zero */
                    lSubTLV_pst->UserDataSubTLV_st.Type_u8  = ETHTSYN_ZERO;
#   if 0
                    /*Set type and nrOFSSubTLV parameters to zero */
                    /*OFS not released*/
                    EthTSyn_RxFollowup_st.SubTLV_st.nrOFSSubTLV_u32                       = ETHTSYN_ZERO;
                    for( lOFSnum_u8 = ETHTSYN_ZERO; lOFSnum_u8< ETHTSYN_SIXTEEN; lOFSnum_u8++ )
                    {
                        EthTSyn_RxFollowup_st.SubTLV_st.OFSSubTLV_st[lOFSnum_u8].Type_u8  = ETHTSYN_ZERO;
                    }
#   endif
#  endif
                }
# endif
                /* Set the Link status to be FALSE */
                lEthTSyn_RamDataTyp_pst->LinkState_b = FALSE;

                /* Set the filter for PTP multicast MAC */
                lResult_en = EthIf_UpdatePhysAddrFilter( CtrlIdx, EthTSyn_Multicast_mac_au8, ETH_REMOVE_FROM_FILTER );

# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
                if ( E_OK != lResult_en )
                {
                    /*  Report DET : Development error for Physical address filter not set  */
                    ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRCVLINKSTATECHG, ETHTSYN_E_PHYADDFILTER_NOTSET );
                }
# endif
            }
            ( void )lResult_en;
        }
        else
        {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
            /* Report DET: Development Error: Invalid value for Ethernet controller Index within the context of the Ethernet Interface */
            /* This DET has to be updated when postbuild is implemented */
            ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRCVLINKSTATECHG, ETHTSYN_E_CTRL_IDX );
# endif
        }
    }
    else
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: EthTSyn module was not initialized */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRCVLINKSTATECHG, ETHTSYN_E_NOT_INITIALIZED );
# endif
    }
}

/**
 ***********************************************************************************************************************************
 * \Function Name : EthTSyn_TxConfirmation()
 *
 * \Function Description
 * EthTSyn_00042: Allows resetting state machine in case of unexpected Link loss to avoid inconsistent Sync and Follow_Up sequences
 * \par Service ID 0x08, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * \param CtrlIdx    - Index of the Ethernet controller within the context of the Ethernet Interface
 * \param BufIdx     - Index of the buffer resource
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***********************************************************************************************************************************
 */
void EthTSyn_TxConfirmation ( uint8 CtrlIdx,
                              uint8 BufIdx  )
{
# if ( ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) || ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) )
    /* Local Variable declaration */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    Eth_TimeStampType          lTimeStampRefSync_st;
    uint8                      lMsgType_u8;
#  else
#   ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    uint8                      lMsgType_u8;
#    if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF )
    StbM_VirtualLocalTimeType  lVirtualLocalTime_st;
    Eth_TimeIntDiffType        lSyncTxVirtualTime_st;
    Eth_TimeStampType          lTimeStamp_st;
    uint64                     lSyncTxTimeStamp_u64;
#    else
    Eth_TimeStampType          lTimeStampEgress_st;
#    endif
#   endif
#  endif
#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
#   if( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
    EthTSyn_MstMgmtObject_tst *lMstPort_pst;
    uint8                      lMstPortNum_u8;
    uint8                      lIdxMstCfg_u8;
    uint8                      lSwtMngtEthSwtPort_u8;
#   endif
#  endif
    uint8                      lIdxGlbTimMst_u8;
    uint8                      lIdxDomain_u8;
    Std_ReturnType             lRetVal_en;

    /* Local Variable Initialization */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   if (ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON)
    lTimeStampRefSync_st.nanoseconds = ETHTSYN_ZERO;
    lTimeStampRefSync_st.seconds     = ETHTSYN_ZERO;
    lTimeStampRefSync_st.secondsHi   = ETHTSYN_ZERO;
#   endif
    lMsgType_u8                      = 0xFFU;
#  else
#   ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
#    if( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF )
    lTimeStamp_st.nanoseconds        = ETHTSYN_ZERO;
    lTimeStamp_st.seconds            = ETHTSYN_ZERO;
    lTimeStamp_st.secondsHi          = ETHTSYN_ZERO;
    lMsgType_u8                      = ETHTSYN_SYNC_TYPE;
    lRetVal_en                       = E_OK;
#    endif
#   else
    lIdxMstCfg_u8                    = ETHTSYN_ZERO;
    lSwtMngtEthSwtPort_u8            = ETHTSYN_ZERO;
#   endif
#  endif
    lIdxGlbTimMst_u8                 = ETHTSYN_ZERO;
    lIdxDomain_u8                    = ETHTSYN_ZERO;
    lRetVal_en                       = E_NOT_OK;

    /* Function call to do the DET check */
    lRetVal_en = EthTSyn_TxConfirmationDetChk( CtrlIdx, &lIdxDomain_u8 );

#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    if ( NULL_PTR != EthTSyn_TimeDomainCfg_pcst[lIdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst )
    {
        /* Index of the current global time master */
        lIdxGlbTimMst_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->GlbTimMstIdx_u8;
    }
#  endif

    /* Condition checks: If there is no DET error when DET compiler macro is switched On. */
    if ( E_OK == lRetVal_en )
    {
        /* Check the type of message by comparing the received buffer index with the stored buffer indexes of Sync,
        * Pdelay_Req and Pdelay_Resp messages */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   if( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
        if( BufIdx == EthTSyn_Master_ast[lIdxGlbTimMst_u8].SyncBufIdx_u8 )
        {
            lMsgType_u8 = ETHTSYN_SYNC_TYPE;
        }
        else
#   endif
#   if( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
        if( BufIdx == EthTSyn_PdelayInitiator_st.PdelayReqBufIdx_u8 )
        {
            lMsgType_u8 = ETHTSYN_PDELAY_REQ_TYPE;
        }
        else
#   endif
#   if( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
        if( BufIdx == EthTSyn_PdelayResponder_st.PdelayRespBufIdx_u8 )
        {
            lMsgType_u8 = ETHTSYN_PDELAY_RESP_TYPE;
        }
        else
#   endif
        {
            /* Do Nothing */
        }

        /* If the received message is Sync or Pdelay_Req or Pdelay_Resp and if the HardwareTimeStamp support is true
        * then obtain the relevant timestamps */
        if( ( lMsgType_u8 == ETHTSYN_SYNC_TYPE ) || ( lMsgType_u8 == ETHTSYN_PDELAY_REQ_TYPE ) || ( lMsgType_u8 == ETHTSYN_PDELAY_RESP_TYPE ) )
        {
#  if(ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON)
            /* Call this API to obtain Hardware TimeStamps */
            EthTSyn_GetHwTime( CtrlIdx, BufIdx, &lMsgType_u8, &lTimeStampRefSync_st, lIdxDomain_u8, lIdxGlbTimMst_u8 );
#  else
            /* Call this API to obtain Software TimeStamps */
            EthTSyn_GetSwTime( &lMsgType_u8, &lTimeStampRefSync_st, lIdxDomain_u8, lIdxGlbTimMst_u8 );

            (void) CtrlIdx;
#  endif
        }

        /* Check the message type */
        switch( lMsgType_u8 )
        {
#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
            case ETHTSYN_SYNC_TYPE:
            {
                /* Assign the resultant time to global datastructure variable that will store PreciseOriginTimeStamp */
                EthTSyn_Master_ast[lIdxGlbTimMst_u8].TxFollowup_st.PreciseOriginTimeStamp_st  = lTimeStampRefSync_st;
                /* Update the state of Master state machine to indicate that Sync message is sent */
                EthTSyn_Master_ast[lIdxGlbTimMst_u8].Sync_Mst_en                              = ETHTSYN_SYNC_SENT_E;
                /* Reset the Sync buffer Index to an invalid value */
                EthTSyn_Master_ast[lIdxGlbTimMst_u8].SyncBufIdx_u8                            = ETHTSYN_FFU;
            }
            break;
#  endif /* if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) */
#  if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
            case ETHTSYN_PDELAY_REQ_TYPE:
            {
                /* Update the datastructure with the received Egress Timestamp */
                EthTSyn_PdelayInitiator_st.PdelayReqEgressTimeStamp_st                        = lTimeStampRefSync_st;
                /* Update the state of Pdelay Initiator state machine to indicate that Pdelay Request has been sent */
                EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                                = ETHTSYN_TX_PDELAY_REQ_SENT_E;
                /* Reset the Pdelay Request buffer Index to an invalid value */
                EthTSyn_PdelayInitiator_st.PdelayReqBufIdx_u8                                 = ETHTSYN_FFU;
            }
            break;
#  endif /* if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) */
#  if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
            case ETHTSYN_PDELAY_RESP_TYPE:
            {
                /* Update the datastructure with the received StbM Raw Timestamp */
                EthTSyn_PdelayResponder_st.TxPdelayrespfollowup_st.ResponseOriginTimeStamp_st = lTimeStampRefSync_st;
                /* Update the state of Pdelay Responder state machine to indicate that Pdelay Response FollowUp has to be sent next */
                EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en                               = ETHTSYN_TX_PDELAY_RESP_FOLLOW_UP_E;
                /* Reset the Pdelay Response buffer Index to an Invalid value */
                EthTSyn_PdelayResponder_st.PdelayRespBufIdx_u8                                = ETHTSYN_FFU;
            }
            break;
#  endif /* if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) */
            default:
                /* Intentionally Empty */
            break;
        }/* End of Switch case */
#  endif /* if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF ) */

#  ifdef ETHTSYN_SWITCH
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
        /* Check the type of message is sync before EthIf_GetTxMgmtObject is called */
        if( ETHTSYN_SYNC_TYPE == lMsgType_u8 )
        {
#   else
        if( BufIdx != EthTSyn_TimBrdg_st.SyncBufIdx_u8 )
        {
            EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_CLR;
        }
        else
        {
#   endif
            /* Reset the number of transmissions to zero */
            EthTSyn_RamDataTyp_ast[lIdxDomain_u8].nrTrsm_u8 = ETHTSYN_ZERO;

#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
#    if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
            for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
            {
                /* Initialise local variable */
                lMstPort_pst = &( EthTSyn_MstMgmtObject_ast[lMstPortNum_u8] );
                lIdxMstCfg_u8 = EthTSyn_Master_ast[lMstPortNum_u8].IdxMstCfg_u8;
                lSwtMngtEthSwtPort_u8 = EthTSyn_TimeDomainCfg_pcst[lIdxDomain_u8].PortCfg_pcst[lIdxMstCfg_u8].SwtMngtEthSwtPort_u8;
                /* Obtain the address of the EthSwt Management object for master port*/
                lRetVal_en = EthIf_GetTxMgmtObject( CtrlIdx, lSwtMngtEthSwtPort_u8, &(lMstPort_pst->MstEthSwt_MgmtObject_pst ) );

                if( E_OK != lRetVal_en )
                {
                    /* Set Management object to unused if E_NOT_OK is returned by lower layer */
                    lMstPort_pst->MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;

                    /* Set address of Master port management object as NULL_PTR*/
                    lMstPort_pst->MstEthSwt_MgmtObject_pst = NULL_PTR;

#     if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
                    /* Report DET: Development Error: invalid return value */
                    ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TXCONFIRMATION, ETHTSYN_E_INV_RETURNVALUE );
#     endif
                }
                else
                {
                    /*Increment the number of ports for which Follow Up will be sent */
                    EthTSyn_RamDataTyp_ast[lIdxDomain_u8].nrTrsm_u8++;
                }
            }
#    endif
#   endif

#   if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
#    ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
            /* Assign the message type to a local variable */
            lMsgType_u8 = ETHTSYN_SYNC_TYPE;

#     if( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
            /* Obtain the egress time stamp for sync message */
            EthTSyn_GetEgressTimeStamp( CtrlIdx, EthTSyn_TimBrdg_st.SyncBufIdx_u8, &lTimeStampEgress_st, &lMsgType_u8, lIdxDomain_u8, lIdxGlbTimMst_u8 );

            /* Obtain the offset between reception and transmission of Sync message */
            EthTSyn_SubTi( &lTimeStampEgress_st, &EthTSyn_Slave_ast[0].SyncIngressTimeStamp_st, &EthTSyn_TimBrdg_st.MstDrvOffsetTiStamp_st );
#     else
            /* Call StbM_GetCurrentVirtualLocalTime to obtain the current time */
            lRetVal_en = EthTSyn_GetCurrentVirtualLocalTime( &lMsgType_u8, &lVirtualLocalTime_st, &lTimeStamp_st, ETHTSYN_TRANSMISSION, lIdxDomain_u8 );

            /* Update MstDrvOffsetTiStamp_st with Time obtained from StbM */
            if( E_OK == lRetVal_en )
            {
                /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value ( T4vlt ) */
                lSyncTxTimeStamp_u64 = ( uint64 )( ( ( ( uint64 ) lVirtualLocalTime_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lVirtualLocalTime_st.nanosecondsLo );

                /* Convert and store the current time in Eth_TimeIntDiffType structure */
                EthTSyn_CnvToEthTi( lSyncTxTimeStamp_u64, ETHTSYN_ZERO, &lSyncTxVirtualTime_st );

                /* Obtain the offset between reception and transmission of Sync message and Update MstDrvOffsetTiStamp_st with Time obtained from StbM */
                EthTSyn_SubTi( &lSyncTxVirtualTime_st.diff, &EthTSyn_Slave_ast[0].SyncIngressTimeStamp_st, &EthTSyn_TimBrdg_st.MstDrvOffsetTiStamp_st );
            }
            else
            {
                /* Set the state of sync message received to Invalid state */
                EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;
            }
#     endif
#    endif
#     if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
            if( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[lIdxDomain_u8].nrTrsm_u8 )
            {
                /* Set the owner of EthSwt Management object back to unused */
                EthTSyn_SlaveMgmtObject_ast[0].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
            }
#     endif

            /* If TxConfirmation of Sync message for a Time Bridge configuration is received, reset the buffer Index to an Invalid value */
            EthTSyn_TimBrdg_st.SyncBufIdx_u8 = ETHTSYN_FFU;

            if( ETHTSYN_MSG_INV != EthTSyn_TimBrdg_st.SyncMsgRxd_u8 )
            {
                /* Set Sync message received parameter to clear as Sync message has been transmitted */
                EthTSyn_TimBrdg_st.SyncMsgRxd_u8 = ETHTSYN_MSG_CLR;
            }
        (void) CtrlIdx;
#   endif /* IF Timebridge is configured */
        }
#  endif /* If Switch is configured */
    } /* End of If check for EthTSyn_TxConfirmationDetChk */
        (void) lIdxGlbTimMst_u8;
# else
        (void) CtrlIdx;
        (void) BufIdx;
# endif
} /* Function End */

# if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_PrepareMessage()
 *
 * \Function Description
 * This API is used to update the local datastructures of the message
 *
 * Parameters (in) :
 * \param MsgTyp     - Type of the message
 * \param IdxDomain  - Index of the current configured Global Time Domain
 * \param IdxGlbTim  - Index of the Master/Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: Returns the length of the message
 * Return Type : uint16
 ***************************************************************************************************
 */
static uint16 EthTSyn_PrepareMessage ( uint8 MsgTyp,
                                       uint8 IdxDomain,
                                       uint8 IdxGlbTim )
{
    /* Local Variable declaration */
    uint16 lMsgLen_u16;
#   if( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF ) )
    uint8  lTLVFolwUpSubTLV_u8;
#   endif

    /* Local Variable Initialization */
    lMsgLen_u16  = ETHTSYN_ZERO;

    /* Check the type of message and update the corresponding datastructures */
    switch( MsgTyp )
    {
#   if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
        case ETHTSYN_SYNC_TYPE:
        {
            EthTSyn_RamDataTyp_ast[IdxDomain].VarHeader_st.SequenceId_au16[ETHTSYN_SYNC_HRD_IDX] += ETHTSYN_ONE;
            lMsgLen_u16                                                                           = ETHTSYN_SYNC_MSG_LEN;
        }
        break;

        case ETHTSYN_FOLLOW_UP_TYPE:
        {
            lMsgLen_u16 = ETHTSYN_SYNC_FUP_MSG_LEN;

#    if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
            lTLVFolwUpSubTLV_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TLVFolwUpSubTLV_u8;
            if( ETHTSYN_MASTER_STATUS_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_STATUS_SUB_TLV ) )
            {
                /* Update buffer length for status sub TLV */
                lMsgLen_u16 += ETHTSYN_STATUS_LENGTH;
            }
            if( ( ETHTSYN_MASTER_USERDATA_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_USERDATA_SUB_TLV ) )
            &&  ( ETHTSYN_ZERO != EthTSyn_Master_ast[IdxGlbTim].TxFollowup_st.SubTLV_st.UserDataSubTLV_st.UserData_st.userDataLength ) )
            {
                /* Update buffer length for UserData sub TLV */
                lMsgLen_u16 += ETHTSYN_USERDATA_LENGTH;
            }
            if( ETHTSYN_MASTER_TIME_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_TIME_SUB_TLV ) )
            {
                /* Update buffer length for Time sub TLV */
                lMsgLen_u16 += ETHTSYN_TIME_LENGTH;
            }
            if ( ETHTSYN_SYNC_FUP_MSG_LEN < lMsgLen_u16 )
            {
                /* Increment buffer length for TLV header */
                lMsgLen_u16 += ETHTSYN_TEN;
            }
#    endif
        }
        break;
#   endif

#   if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
        case ETHTSYN_PDELAY_REQ_TYPE:
        {
            EthTSyn_RamDataTyp_ast[IdxDomain].VarHeader_st.SequenceId_au16[ETHTSYN_PDREQ_HRD_IDX] += ETHTSYN_ONE;
            lMsgLen_u16                                                                            = ETHTSYN_PDELAY_MSG_LEN;
        }
        break;
#   endif

#   if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
        case ETHTSYN_PDELAY_RESP_TYPE:
        {
            lMsgLen_u16 = ETHTSYN_PDELAY_MSG_LEN;
        }
        break;

        case ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE:
        {
            lMsgLen_u16 = ETHTSYN_PDELAY_MSG_LEN;
        }
        break;
#   endif

        default:
            /* Intentionally Empty */
        break;
    }
#   if( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_OFF ) && ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_OFF ) )
    (void) IdxDomain;
#   endif

#   if( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_OFF ) || ( ETHTSYN_MESSAGE_COMPLIANCE == STD_ON ) )
    (void) IdxGlbTim;
#   endif

    return lMsgLen_u16;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeHdr()
 *
 * \Function Description
 * Composes the Header
 *
 * Parameters (in) :
 * \param BufPtr_pu8      - Buffer pointer
 * \param ConstHdrPtr_pst - pointer to constant part of header
 * \param VarHdr_pst      - pointer to variable part of header
 * \param HdrIdx_u8       - Index to sequence id
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
static void EthTSyn_ComposeHdr( uint8                         *BufPtr_pu8,
                                const EthTSyn_ConstMsgHdr_tst *ConstHdrPtr_pst,
                                const EthTSyn_VarMsgHdr_tst   *VarHdr_pst,
                                uint8                          HdrIdx_u8 )
{
    *( BufPtr_pu8 + 0 )  = (uint8)( ( uint8 )( ( uint8 )( ConstHdrPtr_pst->TransportSpecific_u8 & 0x0FU ) << 4U ) | ( ConstHdrPtr_pst->MessageType_u8 & 0x0FU ) );
    *( BufPtr_pu8 + 1 )  = (uint8)( ConstHdrPtr_pst->VersionPTP_u8 & 0x0FU );
    *( BufPtr_pu8 + 2 )  = (uint8)( ( VarHdr_pst->MessageLength_u16 & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 3 )  = (uint8)( VarHdr_pst->MessageLength_u16 & 0x00FFU );
    *( BufPtr_pu8 + 4 )  = (uint8)( VarHdr_pst->DomainNumber_u8 );
    *( BufPtr_pu8 + 5 )  = (uint8)( ETHTSYN_ZERO );
    *( BufPtr_pu8 + 6 )  = (uint8)( ( ConstHdrPtr_pst->Flags_u16 & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 7 )  = (uint8)(   ConstHdrPtr_pst->Flags_u16 & 0x00FFU );
    *( BufPtr_pu8 + 8 )  = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_EIGHTH_BYTE_MASK ) >> ETHTSYN_FIFTYSIX );
    *( BufPtr_pu8 + 9 )  = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SEVENTH_BYTE_MASK ) >> ETHTSYN_FORTYEIGHT );
    *( BufPtr_pu8 + 10 ) = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SIXTH_BYTE_MASK ) >> ETHTSYN_FORTY );
    *( BufPtr_pu8 + 11 ) = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIFTH_BYTE_MASK ) >> ETHTSYN_THIRTYTWO );
    *( BufPtr_pu8 + 12 ) = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 13 ) = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 14 ) = (uint8)( ( VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 15 ) = (uint8)(   VarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIRST_BYTE_MASK );
    *( BufPtr_pu8 + 16 ) = (uint8)( ETHTSYN_ZERO );
    *( BufPtr_pu8 + 17 ) = (uint8)( ETHTSYN_ZERO );
    *( BufPtr_pu8 + 18 ) = (uint8)( ETHTSYN_ZERO );
    *( BufPtr_pu8 + 19 ) = (uint8)( ETHTSYN_ZERO );
    *( BufPtr_pu8 + 20 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[0] );
    *( BufPtr_pu8 + 21 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[1] );
    *( BufPtr_pu8 + 22 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[2] );
    *( BufPtr_pu8 + 23 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[3] );
    *( BufPtr_pu8 + 24 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[4] );
    *( BufPtr_pu8 + 25 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[5] );
    *( BufPtr_pu8 + 26 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[6] );
    *( BufPtr_pu8 + 27 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[7] );
    *( BufPtr_pu8 + 28 ) = (uint8)( ( VarHdr_pst->SourcePortIdentity_st.PortNumber_u16 & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 29 ) = (uint8)( VarHdr_pst->SourcePortIdentity_st.PortNumber_u16 & 0x00FFU );
    *( BufPtr_pu8 + 30 ) = (uint8)( ( VarHdr_pst->SequenceId_au16[HdrIdx_u8] & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 31 ) = (uint8)( VarHdr_pst->SequenceId_au16[HdrIdx_u8] & 0x00FFU );
    *( BufPtr_pu8 + 32 ) = (uint8)( ConstHdrPtr_pst->ControlField_u8 );
    *( BufPtr_pu8 + 33 ) = (uint8)( VarHdr_pst->LogMsgIntl_au8[HdrIdx_u8] );
}
#  endif
# endif

# if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposePacket()
 *
 * \Function Description
 * Updates the Tx buffer
 *
 * Parameters (in) :
 * \param MsgTyp_u8     - Type of the message
 * \param BufIdx_u8     - Index of the buffer resource
 * \param BufPtr_pu8    - Pointer to the granted buffer
 * \param IdxDomain_u8  - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8  - Index of the Master/Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
static void EthTSyn_ComposePacket( uint8  MsgTyp_u8,
                                   uint8  BufIdx_u8,
                                   uint8 *BufPtr_pu8,
                                   uint8  IdxDomain_u8,
                                   uint8  IdxGlbTim_u8 )
{
# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    /* Local Variable declaration */
    const EthTSyn_ConstMsgHdr_tst *lConstHdr_pst;
    EthTSyn_VarMsgHdr_tst         *lVarHdr_pst;
    uint8                          lHrdIdx_u8;
#  ifdef ETHTSYN_SWITCH
#   if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    uint64                        *lCorrnField_pu64;
    uint8                          lTrsmIdx_u8;
    uint8                          lIdxMstCfg_u8;
    uint8                          lPortNr_u8;
    Eth_TimeIntDiffType            lResidenceTi_st;
#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    EthSwt_MgmtObjectType         *lMstMgmtObj_pst;
#    endif
#   endif
#  endif
    /* Local Variable Initialization */
    lConstHdr_pst    = NULL_PTR;
    lVarHdr_pst      = &( EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st );
    lHrdIdx_u8       = ETHTSYN_ZERO;
# endif

    /* Check for the message type and call appropriate internal function */
    switch( MsgTyp_u8 )
    {
# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
        case ETHTSYN_SYNC_TYPE:
        {
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
            EthTSyn_Master_ast[IdxGlbTim_u8].SyncBufIdx_u8   = BufIdx_u8;
            lConstHdr_pst                                    = &( EthTSyn_HdrCfg_cast[ETHTSYN_SYNC_HRD_IDX] );
            lHrdIdx_u8                                       = ETHTSYN_SYNC_HRD_IDX;
            lVarHdr_pst->MessageLength_u16                   = ETHTSYN_SYNC_MSG_LEN;
#  else
            /* Store the buffer Index */
            EthTSyn_TimBrdg_st.SyncBufIdx_u8   = BufIdx_u8;
#  endif
            EthTSyn_ComposeSyncMsg( BufPtr_pu8 );
        }
        break;

        case ETHTSYN_FOLLOW_UP_TYPE:
        {
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
            lConstHdr_pst                       = &( EthTSyn_HdrCfg_cast[ETHTSYN_SYNC_FUP_HRD_IDX] );
            lHrdIdx_u8                          = ETHTSYN_SYNC_HRD_IDX;
            lVarHdr_pst->MessageLength_u16      = ETHTSYN_SYNC_FUP_MSG_LEN;
#   ifdef ETHTSYN_SWITCH
            lTrsmIdx_u8     = EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8;
#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
            lMstMgmtObj_pst = EthTSyn_MstMgmtObject_ast[lTrsmIdx_u8].MstEthSwt_MgmtObject_pst;
            /* Obtain Residence Time using Egress and Ingress Times from Switch */
            EthTSyn_SubTi( &lMstMgmtObj_pst->EgressTimestamp, &lMstMgmtObj_pst->IngressTimestamp, &lResidenceTi_st );
#    else
            /* Convert the Switch Uplink to Tx Residence time to Eth_TimeIntDiffType format */
            EthTSyn_CnvToEthTi( ( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ), ETHTSYN_ZERO, &lResidenceTi_st );
#    endif
            lCorrnField_pu64 = &( EthTSyn_Master_ast[lTrsmIdx_u8].TxFollowup_st.CorrectionField_u64 );
            /* Convert the time obtained to 64 bit value */
            EthTSyn_CnvEthTitoHex( lResidenceTi_st, ETHTSYN_SIXTEEN, lCorrnField_pu64 );

            /* Clear lower 16 bits of the correction field for storing port number */
            ( *lCorrnField_pu64 ) = ( *lCorrnField_pu64 ) & ( ETHTSYN_CORRN_FIE_PORT_MASK );

            /* Store the port number in a local variable as well as in the Buffer */
            lIdxMstCfg_u8 = EthTSyn_Master_ast[lTrsmIdx_u8].IdxMstCfg_u8;
            lPortNr_u8    = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxMstCfg_u8].SwtMngtEthSwtPort_u8;

            /* Update last byte to have Port number */
            ( *lCorrnField_pu64 )   = ( *lCorrnField_pu64 ) | ( (uint64) lPortNr_u8 );

            /* Update correction field parameter in variable header */
            lVarHdr_pst->CorrectionField_u64 = *lCorrnField_pu64;
#   endif
#  endif
            EthTSyn_ComposeFollowUpMsg( BufPtr_pu8, IdxDomain_u8, IdxGlbTim_u8 );
        }
        break;
# endif

# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
        case ETHTSYN_PDELAY_REQ_TYPE:
        {
            /* Store the buffer Index */
            EthTSyn_PdelayInitiator_st.PdelayReqBufIdx_u8    = BufIdx_u8;
            lConstHdr_pst                                    = &( EthTSyn_HdrCfg_cast[ETHTSYN_PDREQ_HRD_IDX] );
            lHrdIdx_u8                                       = ETHTSYN_PDREQ_HRD_IDX;
            lVarHdr_pst->MessageLength_u16                   = ETHTSYN_PDELAY_MSG_LEN;
            EthTSyn_ComposePdelayReqMsg( BufPtr_pu8 );
        }
        break;
#  endif

#  if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
        case ETHTSYN_PDELAY_RESP_TYPE:
        {
            /* Store the buffer Index */
            EthTSyn_PdelayResponder_st.PdelayRespBufIdx_u8   = BufIdx_u8;
            lConstHdr_pst                                    = &( EthTSyn_HdrCfg_cast[ETHTSYN_PDRESP_HRD_IDX] );
            lHrdIdx_u8                                       = ETHTSYN_PDRESP_HRD_IDX;
            lVarHdr_pst->MessageLength_u16                   = ETHTSYN_PDELAY_MSG_LEN;
            EthTSyn_ComposePdelayRespMsg( BufPtr_pu8 );
        }
        break;

        case ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE:
        {
            lConstHdr_pst                                    = &( EthTSyn_HdrCfg_cast[ETHTSYN_PDRESP_FUP_HRD_IDX] );
            lHrdIdx_u8                                       = ETHTSYN_PDRESP_HRD_IDX;
            lVarHdr_pst->MessageLength_u16                   = ETHTSYN_PDELAY_MSG_LEN;
            EthTSyn_ComposePdelayRespFollowUpMsg( BufPtr_pu8 );
        }
        break;
#  endif
# endif
        default:
            /* Intentionally Empty */
        break;
    }

# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    /* Update the header */
    if( lConstHdr_pst != NULL_PTR )
    {
        EthTSyn_ComposeHdr( BufPtr_pu8, lConstHdr_pst, lVarHdr_pst, lHrdIdx_u8 );
    }
# endif
# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_OFF )
    (void) IdxGlbTim_u8;
# endif
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_Transmit()
 *
 * \Function Description
 * Transmits PTP message
 *
 * Parameters (in) :
 * \param MsgTyp_u8     - Type of the message
 * \param IdxDomain_u8  - Index of the current configured Global Time Domain
 * \param IdxGlbTim_u8  - Index of the Master/Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_Transmit( uint8 MsgTyp_u8,
                       uint8 IdxDomain_u8,
                       uint8 IdxGlbTim_u8 )
{
    /* Local Variable declaration */
    uint8                    *lBufPtr_pu8;
    BufReq_ReturnType         lbufReqStatus_en;
    uint16                    lMsgLen_u16;
    uint16                    lReqMsgLen_u16;
    uint8                     lEthTSyn_Multicast_mac_au8[ETHTSYN_MAC_ADDR_LENGTH];
    uint8                     lBufIdx_u8;
    uint8                     lFrmPrio_u8;
    uint8                     lIdx_u8;
    uint8                     lCtrlIdx_u8;
    Std_ReturnType            lResult_u8;
    boolean                   lTxConfirmation_b;
# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    StbM_TimeStampType        lTimeStampGlb_st;
    StbM_VirtualLocalTimeType lTimeStamplocalVirl_st;
    StbM_UserDataType         lUserData_st;
    EthTSyn_Master_tst       *lEthTSyn_Master_past;
#   if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    EthTSyn_SubTLV_tst       *lSubTLV_pst;
#   endif
#  endif
# endif

    /* Local Variable Initialization */
    lBufPtr_pu8                          = NULL_PTR;
    lbufReqStatus_en                     = BUFREQ_E_NOT_OK;
    lMsgLen_u16                          = ETHTSYN_ZERO;
    lBufIdx_u8                           = ETHTSYN_ZERO;
    lResult_u8                           = E_NOT_OK;
    lTxConfirmation_b                    = TRUE;
    lFrmPrio_u8                          = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->FrmPrio_u8;
    /* This initialisation has to be updated with Multi-Domain as with TimeBridge same controller Index is used */
    lCtrlIdx_u8                          = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->EthIfCtrlrIdx_u8;
# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    lTimeStampGlb_st.nanoseconds         = ETHTSYN_ZERO;
    lTimeStampGlb_st.seconds             = ETHTSYN_ZERO;
    lTimeStampGlb_st.secondsHi           = ETHTSYN_ZERO;
    lTimeStamplocalVirl_st.nanosecondsHi = ETHTSYN_ZERO;
    lTimeStamplocalVirl_st.nanosecondsLo = ETHTSYN_ZERO;
#  endif
# endif

    for( lIdx_u8 = ETHTSYN_ZERO; lIdx_u8<ETHTSYN_MAC_ADDR_LENGTH; lIdx_u8++)
    {
        lEthTSyn_Multicast_mac_au8[lIdx_u8]  = EthTSyn_Multicast_mac_au8[lIdx_u8];
    }

#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    /* Prepare the message by updating the local data structures */
    lMsgLen_u16 = EthTSyn_PrepareMessage( MsgTyp_u8, IdxDomain_u8, IdxGlbTim_u8 );
#  else
    if( ETHTSYN_MSG_RXD == EthTSyn_TimBrdg_st.SyncMsgRxd_u8 )
    {
        lMsgLen_u16 = ETHTSYN_SYNC_MSG_LEN;
    }
    else if( ETHTSYN_MSG_RXD == EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 )
    {
        lMsgLen_u16 = EthTSyn_TimBrdg_st.SyncFupLen_u16;
    }
    else
    {
        /* Do Nothing */
    }
#  endif

    /* Store the requested message length */
    lReqMsgLen_u16 = lMsgLen_u16;

    /* Get free buffer from EthIf */
    lbufReqStatus_en = EthIf_ProvideTxBuffer( lCtrlIdx_u8, ETHTSYN_ETHERTYPE, lFrmPrio_u8, &lBufIdx_u8, &lBufPtr_pu8, &lMsgLen_u16 );

    /* If free buffer is granted and if the requested message length is granted and
     * update the buffer with the local datastructure of the message to be transmitted */
    if( BUFREQ_OK == lbufReqStatus_en )
    {
        /* If the transmission mode is ON, then transmit the message  */
        if( ( EthTSyn_RamDataTyp_ast[IdxDomain_u8].TxnMode_en == ETHTSYN_TX_ON ) && ( lReqMsgLen_u16 == lMsgLen_u16 ) )
        {
            EthTSyn_ComposePacket( MsgTyp_u8, lBufIdx_u8, lBufPtr_pu8, IdxDomain_u8, IdxGlbTim_u8 );

#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   if( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
            if ( ETHTSYN_SYNC_TYPE == MsgTyp_u8 )
            {
                /* If the message type is Sync, then call StbM_BusGetCurrentTime that returns the current Time ( Time T0 ) and the virtual local
                 * time ( T0vlt ) of the Time Base.*/
                lResult_u8 = StbM_BusGetCurrentTime( EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].StbM_SyncdTimeBasId_u16, &lTimeStampGlb_st, &lTimeStamplocalVirl_st, &lUserData_st );

# ifdef ETHTSYN_DEBUG_AND_TEST
                lTimeStampGlb_st.nanoseconds = ETHTSYN_ZERO;
                lTimeStampGlb_st.seconds = ETHTSYN_ONE;
                lTimeStampGlb_st.secondsHi = ETHTSYN_ZERO;
# endif
                /* Store the time Current time ( T0 ) in global data structure */
                lEthTSyn_Master_past = &EthTSyn_Master_ast[IdxGlbTim_u8];
                lEthTSyn_Master_past->SyncGlobalTime_st.timeBaseStatus = lTimeStampGlb_st.timeBaseStatus;
                lEthTSyn_Master_past->SyncGlobalTime_st.nanoseconds    = lTimeStampGlb_st.nanoseconds;
                lEthTSyn_Master_past->SyncGlobalTime_st.seconds        = lTimeStampGlb_st.seconds;
                lEthTSyn_Master_past->SyncGlobalTime_st.secondsHi      = lTimeStampGlb_st.secondsHi;

                /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value ( T0vlt ) */
                lEthTSyn_Master_past->SyncVirtualLocalTime_u64 = ( uint64 )( ( ( ( uint64 ) lTimeStamplocalVirl_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lTimeStamplocalVirl_st.nanosecondsLo );

#    if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
                lSubTLV_pst = &lEthTSyn_Master_past->TxFollowup_st.SubTLV_st;
                /* TRACE[SWS_EthTSyn_00094]:The SGW value (Time Gateway synchronization status) shall be mapped to the Status element of the Status sub TLV */
                /* Shift the SGW bit in timebasestatus to LSB position */
                lSubTLV_pst->StatusSubTLV_st.Status_u8 = ( ( lTimeStampGlb_st.timeBaseStatus ) & ( ETHTSYN_SGW_TO_STATUS_MASK ) ) >> ETHTSYN_TWO;

                /* Assign UserData obtained from StbM to global datastructure */
                lSubTLV_pst->UserDataSubTLV_st.UserData_st = lUserData_st;
#    endif
                if ( E_OK != lResult_u8 )
                {
                    /* Reset the state to INIT if return value is E_NOT_OK and assign an invalid number to MessageType
                     * so default case of switch statement will be executed. Reset the counter as well as SyncBufIdx_u8 */
                    lEthTSyn_Master_past->SyncBufIdx_u8                                              = ETHTSYN_FFU;
                    lEthTSyn_Master_past->Sync_Mst_en                                                = ETHTSYN_SYNC_INIT_E;
                    EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_u32;
                    MsgTyp_u8                                                                        = ETHTSYN_FFU;

#    if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
                    /* Report DET: Development Error: invalid return value */
                    ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRANSMIT, ETHTSYN_E_INV_RETURNVALUE );
#    endif
                }/* If return value is E_NOT_OK */
            }/* If the message type is Sync */
#   endif
#   if ( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
            /* If the message is Sync or Pdelay_Req or Pdelay_Resp and if HardwareTimeStampSupport is true,
             * enable Egress Timestamp*/
            if( ( MsgTyp_u8 == ETHTSYN_SYNC_TYPE ) || ( MsgTyp_u8 == ETHTSYN_PDELAY_REQ_TYPE ) || ( MsgTyp_u8 == ETHTSYN_PDELAY_RESP_TYPE ) )
            {
                    EthIf_EnableEgressTimeStamp( lCtrlIdx_u8, lBufIdx_u8 );
            }
#   endif
#  endif
            if(ETHTSYN_FFU != MsgTyp_u8)
            {
                lResult_u8 = EthIf_Transmit( lCtrlIdx_u8, lBufIdx_u8, ETHTSYN_ETHERTYPE, lTxConfirmation_b, lMsgLen_u16, lEthTSyn_Multicast_mac_au8 );
# if ( ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON )
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_DEB_IDX] = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->DebCntr_u32;
# endif
# ifdef ETHTSYN_DEBUG_AND_TEST
#  if( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
                if( ( ETHTSYN_DEBUG_ARRAY_SIZE > index ) && ( ETHTSYN_FOLLOW_UP_TYPE == MsgTyp_u8 ) )
                {
                    EthTSyn_StbMTimArry_ast[index] = EthTSyn_Master_ast[IdxGlbTim_u8].TxFollowup_st.PreciseOriginTimeStamp_st;
                    index++;
                }
#  endif
# endif
            }
        }
        else
        {
            /* If the transmission mode is OFF, release the buffer by calling EthIf_Transmit with message length 0 */
            lResult_u8 = EthIf_Transmit( lCtrlIdx_u8, lBufIdx_u8, ETHTSYN_ETHERTYPE, lTxConfirmation_b, (uint16) 0, lEthTSyn_Multicast_mac_au8 );

# if( ETHTSYN_DEV_ERROR_DETECT  == STD_ON )
            /*  Report DET : Development error for Transmission mode off */
            ( void )Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRANSMIT, ETHTSYN_E_TXN_FAILED );
# endif
        }
    }
# if( ETHTSYN_DEV_ERROR_DETECT  == STD_ON )
    if ( E_OK != lResult_u8 )
    {
        /* Report DET : Development error for transmission failed */
        ( void )Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_TRANSMIT, ETHTSYN_E_TXN_FAILED );
    }
# else
    (void) lResult_u8;
# endif
}
# endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_MainFunction()
 *
 * \Function Description
 * EthTSyn_00044: Main function for cyclic call / resp. Sync, Follow_Up and Pdelay_Req transmissions
 * \par Service ID 0x09, Synchronous, Non Reentrant
 *
 * Parameters (in) :
 * None
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_MainFunction( void )
{
    /* Local Variable declaration */
    EthTSyn_RamDataTyp_tst *lEthTSyn_RamDataTyp_pst;
    uint8                   lIdxDomain_u8;

    /* Local Variable Initialisation */
    lIdxDomain_u8           = ETHTSYN_ZERO;

    if ( ETHTSYN_INIT_E == EthTSyn_InitStatus_en )
    {
        /* Loop through all the Domains Configured for transmitting Sync and Follow-Up frames */
        /* In case of TimeBridge, only one domain to be considered for implementation */
# if ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED != 0x1U
        for( ; lIdxDomain_u8 < ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED; lIdxDomain_u8++ )
# endif
        {
            lEthTSyn_RamDataTyp_pst = &EthTSyn_RamDataTyp_ast[lIdxDomain_u8];
            /* State Machines will Run only if Link State is TRUE */
            if( TRUE == lEthTSyn_RamDataTyp_pst->LinkState_b )
            {
# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF)
                /* Master And pdelay state machines will run only if TX Mode is ON */
                if( ETHTSYN_TX_ON == lEthTSyn_RamDataTyp_pst->TxnMode_en )
                {

#  if ( ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON )
                    if ( ETHTSYN_ZERO != lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX] )
                    {
                        /*Decrement the counter every main cycle*/
                        lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX]--;
                    }
#  endif
                    /* Run Sync Master State Machine */
#  if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
                    EthTSyn_RunSyncMstSM( lIdxDomain_u8 );
#  endif

#  if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
                    /* Run Pdelay Initiator State Machine */
                    EthTSyn_RunPdelayInitiatorSM( lIdxDomain_u8 );
#  endif

#  if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
                    /* Run Pdelay Responder State Machine */
                    EthTSyn_RunPdelayResponderSM( lIdxDomain_u8 );
#  endif
                }
# endif
                /* Run Sync Slave State Machine */
# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
                EthTSyn_RunSyncSlaveSM( lIdxDomain_u8 );
# endif

# if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
                if( ETHTSYN_TX_ON == lEthTSyn_RamDataTyp_pst->TxnMode_en )
                {
                    /* Run Time Bridge state machine */
                    EthTSyn_RunTimBrdgSM( lIdxDomain_u8 );
                }
# endif
            }/* If check for link state */
        } /* For loop to check through all Domains configured */
    }
    else
    {
# if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: EthTSyn module was not initialized */
        ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_MAINFUNCTION, ETHTSYN_E_NOT_INITIALIZED );
# endif
    }
}

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_CONFIGURED */
