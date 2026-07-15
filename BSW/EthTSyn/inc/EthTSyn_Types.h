
#ifndef ETHTSYN_TYPES_H
#define ETHTSYN_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "ComStack_Types.h"
#include "Eth_GeneralTypes.h"
#include "EthTSyn_Cfg.h"

#ifdef ETHTSYN_SWITCH
#include "EthSwt.h"
#endif
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/

#define ETHTSYN_CLK_ID_LEN                  8U
#define ETHTSYN_SYNC_MSG_LEN                44U
#define ETHTSYN_MAX_SYNC_FUP_MSG_LEN        102U

# if 0
/*OFS not released*/
#define ETHTSYN_OFS_SIZE            16U
# endif


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/

/*
 *   Description:    Handles the enabling and disabling of the transmission mode
 *   [SWS_EthTSyn_00033]
 */
typedef enum {
    ETHTSYN_TX_OFF                                  = 0x00,                                 /* Transmission Disabled */
    ETHTSYN_TX_ON                                   = 0x01                                  /* Transmission Enabled */
} EthTSyn_TransmissionModeType;


/*
 *   Description:    Information regarding the Sync state
 *   [SWS_EthTSyn_00034]
 */
typedef enum {
    ETHTSYN_SYNC                                    = 0x00,                                 /* Ethernet time synchronous*/
    ETHTSYN_UNSYNC                                  = 0x01,                                 /* Ethernet not time synchronous */
    ETHTSYN_UNCERTAIN                               = 0x02,                                 /* Ethernet Sync state uncertain */
    ETHTSYN_NEVERSYNC                               = 0x03                                  /* No Sync message received between EthTSyn_Init() and current requested state */
} EthTSyn_SyncStateType;


/* Description:    Defines the state machines of Pdelay Initiator */
typedef enum {
    ETHTSYN_TX_PDELAY_REQ_INIT_E                      = 0x00,                                 /* Init state for Pdelay Initiator state Machine */
    ETHTSYN_TX_PDELAY_REQ_E                           = 0x01,                                 /* Pdelay Request to be sent */
    ETHTSYN_TX_PDELAY_REQ_SENT_E                      = 0x02,                                 /* Pdelay Request Sent */
    ETHTSYN_RX_PDELAY_RESP_E                          = 0x03,                                 /* Pdelay Response Received */
    ETHTSYN_RX_PDELAY_RESP_FOLLOW_UP_E                = 0x04                                  /* Pdelay Response Follow_Up Received  */
} EthTSyn_Pdelay_ReqStMach_ten;


/* Description:    Defines the state machines of Pdelay Responder */
typedef enum {
    ETHTSYN_TX_PDELAY_RESP_INIT_E                     = 0x00,                                 /* Init state for Pdelay Responder state Machine */
    ETHTSYN_TX_PDELAY_RESP_E                          = 0x01,                                 /* Pdelay Response to be sent */
    ETHTSYN_TX_PDELAY_RESP_SENT_E                     = 0x02,                                 /* Pdelay Response Sent */
    ETHTSYN_TX_PDELAY_RESP_FOLLOW_UP_E                = 0x03                                  /* Pdelay Response Follow_Up Sent */
} EthTSyn_Pdelay_RespStMach_ten;

/* Description:    Defines the state machines of Sync Message */
typedef enum {
    ETHTSYN_SYNC_INIT_E                               = 0x01,                                 /* Init state for Pdelay Sync state Machine */
    ETHTSYN_SYNC_PREP_E                               = 0x02,                                 /* Pdelay Sync to be sent */
    ETHTSYN_SYNC_SENT_E                               = 0x03,                                 /* Pdelay Sync Sent */
    ETHTSYN_FOLLOW_UP_E                               = 0x04                                  /* Pdelay Sync Follow_Up Sent */
} EthTSyn_SyncStMach_ten;

/* Description:    Defines the initialization state of EthTSyn */
typedef enum {
    ETHTSYN_UNINIT_E                                  = 0x00,                                 /* EthTSyn module uninitialized */
    ETHTSYN_INIT_E                                    = 0x01                                  /* EthTSyn module initialized */
} EthTSyn_InitStatus_ten;

/* Description:    Defines the CRC states for Slave */
typedef enum {
    ETHTSYN_CRC_IGNORED                               = 0x00,                                 /* CRC is ignored */
    ETHTSYN_CRC_NOT_VALIDATED                         = 0x01,                                 /* CRC is not validated  */
    ETHTSYN_CRC_OPTIONAL                              = 0x02,                                 /* CRC is optional */
    ETHTSYN_CRC_VALIDATED                             = 0x03                                  /* CRC is validated */
} EthTSyn_RxCrcValidated_ten;

/* Description:    Defines the CRC states for Master */
typedef enum {
    ETHTSYN_CRC_SUPPORTED                             = 0x00,                                 /* CRC is supported */
    ETHTSYN_CRC_NOT_SUPPORTED                         = 0x01                                  /* CRC is not supported  */
} EthTSyn_TxCrcSecured_ten;

/* Description:    Defines the source port identity */
typedef struct {
    uint16 PortNumber_u16;                            /* Field:2 :Port number of the source */
    uint16 Reserved_u16;                              /* Reserved to make 32bit aligned*/
    uint8  ClockIdentity_au8[ETHTSYN_CLK_ID_LEN];     /* Field:1 64 bit Clock Identity */
} EthTSyn_SrcPortId_tst;

/* Description:    Defines the common message header defined in decreasing order of memory */
typedef struct {
    uint64                CorrectionField_u64;     /* Field:9 Correction field */
    EthTSyn_SrcPortId_tst SourcePortIdentity_st;   /* Field:11 Identity of source port */
    uint16                SequenceId_au16[4];      /* Field:12 Sequence Id Array of 4 to store Id for SYNC and pDelay_req and pdelay_Resp */
    uint16                MessageLength_u16;       /* Field:5 Length of the message */
    uint8                 LogMsgIntl_au8[3];       /* Field:14 Log Message interval */
    uint8                 DomainNumber_u8;         /* Field:6 Domain number of the message */
} EthTSyn_VarMsgHdr_tst;

typedef struct {
    EthTSyn_VarMsgHdr_tst        VarHeader_st;          /* Message Header variable part  */
    EthTSyn_TransmissionModeType TxnMode_en;            /* Transmission Mode  */
    uint32                       MainFunTimer_au32[4];  /* Timers for Sync, Pdelay_Init Machine, Debounce counter and Cyclic message resume counter */
# ifdef ETHTSYN_SWITCH
    uint8                        TrsmIdx_u8;            /* Stores the Index of successful transmissions of FollowUp message */
    uint8                        nrTrsm_u8;             /* Stores the number of successful transmissions of FollowUp message */
# endif
    boolean                      LinkState_b;           /* Variable to store the LinkState */
}EthTSyn_RamDataTyp_tst;

/* Description:    Defines the common message header defined in decreasing order of memory */
typedef struct {
    uint16 Flags_u16;                             /* Field:8 Flags for control */
    uint8  MessageType_u8;                        /* Field:1 Type of message */
    uint8  TransportSpecific_u8;                  /* Field:2 Transport specific data */
    uint8  VersionPTP_u8;                         /* Field:3 PTP version */
    uint8  ControlField_u8;                       /* Field:13 Control field */
    uint8  Reserved_u8;                           /* Reserved to make 32bit aligned*/
} EthTSyn_ConstMsgHdr_tst;

# if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
/*Description:      Defines the fields associated with Time Sub TLV*/
typedef struct {
    EthTSyn_SrcPortId_tst SourcePortIdentity_st;   /* Parameter to store SourcePort Identity */
    uint16                Msglen_u16;              /* Parameter to store message lenth field */
    uint16                SequenceId_u16;          /* Parameter to store Sequence Id of followup message */
    uint8                 domnum_u8;               /* Parameter to store domain number */
    uint8                 Type_u8;                 /* Parameter for type field */
    uint8                 CRC_TimeFlags_u8;        /* Parameter for frame field CRC_Time_Flags */
    uint8                 CRC_Time0_u8;            /* Parameter for frame field CRC_Time_0 */
    uint8                 CRC_Time1_u8;            /* Parameter for frame field CRC_Time_1 */
    uint8                 Reserved_au8[3];         /* Reserved to make 32 bit aligned */
} EthTSyn_TimeSubTLV_tst;


/*Description:      Defines the fields in Status Sub TLV */
typedef struct {
    uint8   Type_u8;                               /* Parameter for type field */
    uint8   Status_u8;                             /* Parameter for frame field Status */
    uint8   CRC_Status_u8;                         /* Parameter for frame field CRC_Status */
    boolean isRead_b;                              /* Parameter to check if status parameter can be read */
} EthTSyn_StatusSubTLV_tst;


/*Description:      Defines the fields in UserData Sub TLV */
typedef struct {
    StbM_UserDataType UserData_st;                  /* Parameter for frame field OfsTimeSec and OfsTimeNSec*/
    uint8             Type_u8;                      /* Parameter for type field */
    uint8             CRC_UserData_u8;              /* Parameter for frame field CRC_UserData */
    boolean           isRead_b;                     /* Parameter to check if UserData can be read */
    uint8             Reserved_u8;                  /* Reserved to make 32 bit aligned */
} EthTSyn_UserDataSubTLV_tst;


/*Description:      Defines the fields in OFS Sub TLV */
typedef struct {
    StbM_UserDataType  UserData_st;                 /* Parameter for User length, UserData0, UserData1, UserData2 */
    StbM_TimeStampType OFS_Time_st;                 /* Parameter for frame field OfsTimeSec and OfsTimeNSec*/
    uint8              OFS_TimeDomain_u8;           /* Parameter for frame field OfsTimeDomain*/
    uint8              Type_u8;                     /* Parameter for type field */
    uint8              Status_u8;                   /* Parameter for frame field Status */
    uint8              CRC_OFS_u8;                  /* Parameter for frame field CRC_UserData */
    boolean            isRead_b;                    /* Parameter to check if frame fields can be read */
    uint8              Reserved_u8;                 /* Reserved to make 32 bit aligned */
} EthTSyn_OFSSubTLV_tst;


/*Description:      Datastructure containing all sub TLVs */
typedef struct {
    EthTSyn_TimeSubTLV_tst     TimeSubTLV_st;                  /* Pointer to Time Sub TLV structure */
    EthTSyn_StatusSubTLV_tst   StatusSubTLV_st;                /* Pointer to Status Sub TLV structure */
    EthTSyn_UserDataSubTLV_tst UserDataSubTLV_st;              /* Pointer to UserData Sub TLV structure */
# if 0
    EthTSyn_OFSSubTLV_tst      OFSSubTLV_st[ETHTSYN_OFS_SIZE]; /* Pointer to OFS Sub TLV structure */
    uint32                     nrOFSSubTLV_u32;                /* Number of OFS Sub TLVs received */
# endif
} EthTSyn_SubTLV_tst;
# endif


/* Description:    Defines the Follow_Up message */
typedef struct {
    Eth_TimeStampType  PreciseOriginTimeStamp_st;  /* Sync Origin timestamp */
    uint16             Reserved_u16;               /* Reserved to make 32bit aligned*/
    uint64             CorrectionField_u64;        /* Field:9 Correction field */
# if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    EthTSyn_SubTLV_tst SubTLV_st;                  /* Pointer to SubTLV datastructure */
    boolean            TLVHdr_b;                   /* Flag to check TLV header's presence */
    uint8              Reserved_au8[3];            /* Reserved to make 32 bit aligned */
# endif
} EthTSyn_FollowUpMsg_tst;


/* Description:    Defines the Pdelay_Resp message */
typedef struct {
    Eth_TimeStampType     ReceiveReceiptTimeStamp_st; /* Timestamp of received Pdelay_Req */
    uint16                Reserved_u16;               /* Reserved to make 32bit aligned*/
    EthTSyn_SrcPortId_tst RequestingPortIdentity_st;  /* PortId of the requesting port */
} EthTSyn_PdelayRespMsg_tst;


/* Description:    Defines the Pdelay_Resp_Follow_Up message */
typedef struct {
    Eth_TimeStampType ResponseOriginTimeStamp_st;     /* Timestamp of sent Pdelay_Resp */
    uint16            Reserved_u16;                   /* Reserved to make 32bit aligned*/
} EthTSyn_PdelayRespFollowUpMsg_tst;

# ifdef ETHTSYN_SWITCH
#  if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
/* Description :    Defines the variables related to Switch Management Object for Master SM */
typedef struct  {
    EthSwt_MgmtObjectType  *MstEthSwt_MgmtObject_pst;  /* Stores the addresses of EthSwt Management Object per port*/
} EthTSyn_MstMgmtObject_tst;
#  endif

#  if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
/* Description :    Defines the variables related to Switch Management Object for Slave SM */
typedef struct  {
#   ifndef ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME
    EthSwt_MgmtObjectType  *SlaveEthSwt_MgmtObject_pst;  /* Stores the addresses of EthSwt Management object for Slave Port*/
#   endif
    Eth_TimeIntDiffType     SlaveTiStamp_st;             /* Stores the Residence Time for slave port for Sync message*/
} EthTSyn_SlaveMgmtObject_tst;
#  endif
# endif

# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
/* Description :    Defines the variables related to Master SM */
typedef struct  {
#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    uint64                  SyncVirtualLocalTime_u64;  /* Stores time stamp T0vlt */
    EthTSyn_FollowUpMsg_tst TxFollowup_st;             /* Stores data related to FollowUp message */
    EthTSyn_SyncStMach_ten  Sync_Mst_en;               /* Stores state of Master State Machine */
    StbM_TimeStampType      SyncGlobalTime_st;         /* Stores time stamp T0 */
    uint8                   SyncBufIdx_u8;             /* Stores Buffer Index of a Sync Message */
#   if ( ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON )
    uint8                   TimeBasUpdCntr_u8;         /* Variable that stores the previous time base counter value */
#   endif
#  endif
    uint8                   IdxMstCfg_u8;              /* Stores Index to Master Port Config structure */
}EthTSyn_Master_tst;
# endif

# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
/* Description :    Defines the variables related to Slave SM */
typedef struct  {
    EthTSyn_FollowUpMsg_tst RxFollowup_st;               /* Stores data related to FollowUp message */
    EthTSyn_SrcPortId_tst   SyncSrcPortIdentity_st;      /* Stores the SourcePortIdentity received in first Sync message */
    EthTSyn_SyncStMach_ten  Sync_Slave_en;               /* Stores state of Slave State Machine */
    Eth_TimeStampType       SyncIngressTimeStamp_st;     /* Stores the Ingress Time Stamp of Sync message */
    uint16                  RxSyncSeqId_u16;             /* Stores the sequence Id of received Sync message */
    uint8                   IdxSlaveCfg_u8;              /* Stores Index to Slave Config structure */
}EthTSyn_Slave_tst;
# endif

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
/* Description :    Defines the variables related to Time Bridge */
typedef struct  {
    uint16              SyncFupLen_u16;                                   /* Stores the length of the received FollowUp message */
    uint8               SyncMsgRxd_u8;                                    /* Variable to check if Sync is received or not */
    uint8               SyncBufIdx_u8;                                    /* Stores the Buffer Index of the transmitted Sync message */
    uint8               TxSyncBuf_au8[ETHTSYN_SYNC_MSG_LEN];              /* Buffer to store received Sync message */
    uint8               TxFollowUpBuf_au8[ETHTSYN_MAX_SYNC_FUP_MSG_LEN];  /* Buffer to store received FollowUp message */
    uint16              TiSubTLVLen_u16;                                  /* Stores the length at where time sub TLV starts */
#  ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    Eth_TimeIntDiffType MstDrvOffsetTiStamp_st;                           /* Stores the time difference between Ingress and Egress Time Stamp of Sync message*/
#  endif
    uint8               SyncFupMsgRxd_u8;                                 /* Boolean to check if FollowUp is received or not */
}EthTSyn_TiBrdg_tst;
# endif

# if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
/* Description :    Defines the variables related to Pdelay Initatiator SM */
typedef struct  {
    Eth_TimeStampType            RxReceiptTimeStamp_st;          /* Stores the Receipt Time Stamp */
    uint16                       Reserved1_u16;                  /* Reserved to make 32 bit aligned */
    Eth_TimeStampType            RxResponseOriginTimeStamp_st;   /* Stores the Response Origin Time Stamp */
    uint16                       Reserved2_u16;                  /* Reserved to make 32 bit aligned */
    Eth_TimeStampType            PdelayReqEgressTimeStamp_st;    /* Stores the Egress Time Stamp of Pdelay Request message */
    uint16                       Reserved3_u16;                  /* Reserved to make 32 bit aligned */
    Eth_TimeStampType            PdelayRespIngressTimeStamp_st;  /* Stores the Ingress Time Stamp of Pdelay Response message */
    uint8                        PdelayReqBufIdx_u8;             /* Stores Buffer Index of a Pdelay Request Message */
    EthTSyn_Pdelay_ReqStMach_ten Pdelay_ReqStMach_en;            /* Stores the state of Pdelay Initiator SM */
}EthTSyn_PdelayInitiator_tst;
# endif

# if ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON )
/* Description :    Defines the variables related to Pdelay Responder SM */
typedef struct  {
    EthTSyn_PdelayRespMsg_tst         TxPdelayresp_st;           /* Stores the Pdelay Response structure */
    EthTSyn_PdelayRespFollowUpMsg_tst TxPdelayrespfollowup_st;   /* Stores the Pdelay Response FollowUp structure */
    EthTSyn_Pdelay_RespStMach_ten     Pdelay_RespStMach_en;      /* Stores the state of Pdelay Responder SM */
    uint8                             PdelayRespBufIdx_u8;       /* Stores Buffer Index of a Pdelay Response Message */
}EthTSyn_PdelayResponder_tst;
# endif

/* Config Structures*/

# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
/* Description:    Defines the config parameters for Time Master */
typedef struct {
    const EthTSyn_TxCrcSecured_ten TxCrcValidated_en;       /* Status of CRC at Master side*/
    const uint32                   SyncTxPerd_u32;          /* Tx period for Sync messages in terms of main function cycles. Type:Integer */
    const float32                  SyncTxPerd_f32;          /* Tx period for Sync messages. Type:Float*/
# if ( ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON )
    const uint32                   CyclicMsgResuCntr_u32;   /* Cyclic message resume counter for Sync messages */
    const boolean                  ImdtTimeSync_b;          /* Boolean field to check if Immediate Time Sync feature is enabled */
# endif
# if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    const uint8                    TLVFolwUpSubTLV_u8 ;     /* Bit field for sub TLVs */
    const uint8                    CrcFlagsTxValidated_u8;  /* Bit field for Follow-Up message elements that is included in TimeCRC validation */
# endif
    const uint8                    GlbTimMstIdx_u8;         /* Index of the current global time master */
} EthTSyn_GlbTimeMst_tst;
# endif


# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
/* Description:    Defines the config parameters for Time Slave */
typedef struct {
    const EthTSyn_RxCrcValidated_ten RxCrcValidated_en;       /* Status of CRC at slave side*/
    const uint32                     Sync_SyncFolwUpTout_u32; /* Follow-Up TimeOut value. Unit:Seconds */
# if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
    const uint8                      CrcFlagsRxValidated_u8;  /* Bit field for Follow-Up message elements that is included in CRC validation */
# endif
    const uint8                      GlbTimSLvIdx_u8;         /* Index of the current global time slave */
} EthTSyn_GlbTimeSlave_tst;
# endif


/* Description:    Defines the pointers to Time Master or Time Slave datastructure */
typedef struct {
# if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
    const EthTSyn_GlbTimeMst_tst    *GlbTimeMst_pcst;     /* Pointer to Time Master config structure  */
# endif
# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )
    const EthTSyn_GlbTimeSlave_tst  *GlbTimeSlave_pcst;   /* Pointer to Time Slave config structure   */
# endif
} EthTSyn_PortRole_tst;


/* Description:    Defines the config parameters for Pdelay */
typedef struct {
    const Eth_TimeStampType PdelayGlbTimePrpgtnDly_st;        /* Default value of propogation delay. */
    const uint16            PdelayResp_RespFolwUpTout_u16;    /* Timeout value between request and response, response and response_followup. Unit:Seconds */
    const uint32            PdelayReqTxPerd_u32;              /* Tx period for Pdelay_Req messages in terms of main function cycles. Type:Integer*/
    const float32           PdelayReqTxPerd_f32;              /* Tx period for Pdelay_Req messages. Type:Float*/
    const uint32            PdelayLtcyThd_u32;                /* Threshold for calculated Pdelay */
} EthTSyn_PdelayCfg_tst;


/* Description:    Defines the config parameters for EthTSyn-ports*/
typedef struct {
    const EthTSyn_PdelayCfg_tst *PdelayCfg_pcst;            /* Pointer to Pdelay config structure  */
    const EthTSyn_PortRole_tst  *PortRole_pcst;             /* Pointer to PortRole config structure  */
# if ( ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON )
    const uint32                 DebCntr_u32;               /* Debounce time between a two Timesync PDUs */
# endif
    const uint8                  FrmPrio_u8;                /* Indicates the frame priority of EthTSyn messages */
    const uint8                  EthIfCtrlrIdx_u8;          /* Index of the Ethernet Communication Controllers */
    const uint8                  SwtMngtEthSwtPort_u8;      /* Index of the Ethernet Switch Port Controllers */
    const boolean                GlbTimePdelayRespEna_b;    /* Boolean to check if Pdelay Response messages are sent or not. */
} EthTSyn_PortCfg_tst;


/*
 *   Description:    Defines the config parameters for Global Time Domains
 */
typedef struct {
# if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    const uint8                     *FolwUpDataIDList_pcst;        /* Pointer to DataIDList config structure */
# endif
    const EthTSyn_PortCfg_tst       *PortCfg_pcst;                 /* Pointer to Port config structure  */
    const StbMLocalTimeHardware_ten  StbM_LocalTimeHardware_en;    /* StbM PTP clock reference */
    const uint16                     StbM_SyncdTimeBasId_u16;      /* StbM TimeBase Id */
    const uint8                      GlbTimeDomId_u8;              /* Global time domain id*/
    const uint8                      nrPortConfigs_u8;             /* Number of Ports configured for a Global Time Domain */
} EthTSyn_TimeDomainCfg_tst;

/*
 *   Description:    Base type for the configuration of the Global Time Synchronization over Ethernet.
 *   [SWS_EthTSyn_00032]
 */
typedef struct {
    const EthTSyn_TimeDomainCfg_tst *TimeDomainCfg_pcst;        /* Pointer to Global Time Domain config structure */
} EthTSyn_ConfigType;

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/



#endif /* ETHTSYN_TYPES_H */
