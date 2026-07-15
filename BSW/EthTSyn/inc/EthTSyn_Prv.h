

#ifndef ETHTSYN_PRV_H
#define ETHTSYN_PRV_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/
//#define ETHTSYN_DEBUG_AND_TEST

#ifdef ETHTSYN_DEBUG_AND_TEST
#define ETHTSYN_DEBUG_ARRAY_SIZE    100U
#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
extern Eth_TimeStampType EthTSyn_StbMTimArry_ast[ETHTSYN_DEBUG_ARRAY_SIZE];
extern uint8 index;
#define ETHTSYN_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
# endif

/* Error IDs for DET */
#define ETHTSYN_E_INV_TIMESTAMP             0x3U    /* Invalid timestamp received */
#define ETHTSYN_E_TXN_FAILED                0x5U    /* Transmission failed */
#define ETHTSYN_E_PHYADDFILTER_NOTSET       0x6U    /* Physical address filter not set */
#define ETHTSYN_E_NOT_INITIALIZED           0x20U   /* EthTSyn module was not initialized */
#define ETHTSYN_E_CTRL_IDX                  0x22U   /* Invalid value for Ethernet controller Index within the context of the Ethernet Interface */
#define ETHTSYN_E_PARAM_POINTER             0x23U   /* API called with invalid pointer */
#define ETHTSYN_E_PARAM                     0x24U   /* API called with invalid parameter */
#define ETHTSYN_E_INV_RETURNVALUE           0xFFU   /* Invalid return value */

#define ETHTSYN_E_TMCONFLICT                0x01U   /* Runtime error when master receives a sync from another master */
#define ETHTSYN_E_TSCONFLICT                0x02U   /* Runtime error when slave is configured */

/* Service IDs for DET */
#define ETHTSYN_SID_INIT                    0x1U    /* Service ID for EthTSyn_Init */
#define ETHTSYN_SID_GETVERSINFO             0x2U    /* Service ID for EthTSyn_GetVersionInfo */
#define ETHTSYN_SID_SETTXNMODE              0x5U    /* Service ID for EthTSyn_SetTransmissionMode */
#define ETHTSYN_SID_RXINDICATION            0x6U    /* Service ID for EthTSyn_RxIndication */
#define ETHTSYN_SID_TXCONFIRMATION          0x7U    /* Service ID for EthTSyn_TxConfirmation */
#define ETHTSYN_SID_TRCVLINKSTATECHG        0x8U    /* Service ID for EthTSyn_TrcvLinkStateChg */
#define ETHTSYN_SID_MAINFUNCTION            0x9U    /* Service ID for EthTSyn_MainFunction */
#define ETHTSYN_SID_TRANSMIT                0xFFU   /* Service ID for EthTSyn_Transmit */
#define ETHTSYN_SID_GETINGRESSTIMESTAMP     0xFEU   /* Service ID for EthTSyn_GetIngressTimeStamp */
#define ETHTSYN_SID_GETEGRESSTIMESTAMP      0xFDU   /* Service ID for EthTSyn_GetEgressTimeStamp */
#define ETHTSYN_SID_GETHWTIME               0xFCU   /* Service ID for EthTSyn_GetHwTime */
#define ETHTSYN_SID_GETCURRENTVIRTUALTIME   0xFBU   /* Service ID for EthTSyn_GetCurrentVirtualLocalTime */
#define ETHTSYN_SID_RECEIVESYNC             0xFAU   /* Service ID for EthTSyn_ReceiveSync */
#define ETHTSYN_SID_RUNSYNCMSTSM            0xF9U   /* Service ID for EthTSyn_RunSyncMstSM */
#define ETHTSYN_SID_RUNSYNCSLAVESM          0xF8U   /* Service ID for EthTSyn_RunSyncSlaveSM */

#define ETHTSYN_NS_UPPRLIM                  0x3B9AC9FFU /* Max Value of Nano Second */

/* EthTSyn Frame Constant Parameters */
#define ETHTSYN_SYNC_TYPE                   0x00U
#define ETHTSYN_FOLLOW_UP_TYPE              0x08U
#define ETHTSYN_PDELAY_REQ_TYPE             0x02U
#define ETHTSYN_PDELAY_RESP_TYPE            0x03U
#define ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE  0x0AU
#define ETHTSYN_TRANSPORT_SPECIFIC          1U
#define ETHTSYN_VERSION_PTP                 2U
#define ETHTSYN_SYNC_FUP_MSG_LEN            76U /* ( 44 + 32 )U 32Bytes of TLV */
#define ETHTSYN_PDELAY_MSG_LEN              54U
#define ETHTSYN_FLAGS                       0x0200U
#define ETHTSYN_SYNC_CONTROL                0x00U
#define ETHTSYN_FOLLOWUP_CONTROL            0x02U
#define ETHTYSN_PDELAY_CONTROL              0x05U
#define ETHTSYN_PDELAY_RESP_LOGMSGINTL      0x7FU
#define ETHTSYN_PDELAY_RESP_RECEIPT_TIMEOUT 100000000UL /* 100ms Unit : ns */
#define ETHTSYN_LOG_BASE_FOR_LOGMSGINTL     2.0F

/* Magic Number Macros */
#define ETHTSYN_ZERO                        0U
#define ETHTSYN_ZERO_FLOAT                  0.0F
#define ETHTSYN_ONE                         1U
#define ETHTSYN_TWO                         2U
#define ETHTSYN_THREE                       3U
#define ETHTSYN_FOUR                        4U
#define ETHTSYN_FIVE                        5U
#define ETHTSYN_SIX                         6U
#define ETHTSYN_SEVEN                       7U
#define ETHTSYN_EIGHT                       8U
#define ETHTSYN_NINE                        9U
#define ETHTSYN_TEN                         10U
#define ETHTSYN_ELEVEN                      11U
#define ETHTSYN_TWELVE                      12U
#define ETHTSYN_THIRTEEN                    13U
#define ETHTSYN_FOURTEEN                    14U
#define ETHTSYN_FIFTEEN                     15U
#define ETHTSYN_SIXTEEN                     16U
#define ETHTSYN_SEVENTEEN                   17U
#define ETHTSYN_EIGHTEEN                    18U
#define ETHTSYN_TWENTY                      20U
#define ETHTSYN_TWENTYONE                   21U
#define ETHTSYN_TWENTYTWO                   22U
#define ETHTSYN_TWENTYTHREE                 23U
#define ETHTSYN_TWENTYFOUR                  24U
#define ETHTSYN_TWENTYFIVE                  25U
#define ETHTSYN_TWENTYSIX                   26U
#define ETHTSYN_TWENTYSEVEN                 27U
#define ETHTSYN_TWENTYEIGHT                 28U
#define ETHTSYN_TWENTYNINE                  29U
#define ETHTSYN_THIRTY                      30U
#define ETHTSYN_THIRTYONE                   31U
#define ETHTSYN_THIRTYTWO                   32U
#define ETHTSYN_FORTY                       40U
#define ETHTSYN_FORTYEIGHT                  48U
#define ETHTSYN_FIFTYTWO                    52U
#define ETHTSYN_FIFTYTHREE                  53U
#define ETHTSYN_FIFTYSIX                    56U
#define ETHTSYN_SEVENTYSIX                  76U
#define ETHTSYN_SEVENTYSEVEN                77U
#define ETHTSYN_SEVENTYEIGHT                78U
#define ETHTSYN_SEVENTYNINE                 79U
#define ETHTSYN_EIGHTY                      80U
#define ETHTSYN_EIGHTYSIX                   86U
#define ETHTSYN_FFU                         255U
#define ETHTSYN_MAC_EUI64_RESERVED_BYTE     0xFFFEU
#define ETHTSYN_SEQID_INITIAL_VALUE         0xFFFFU
#define ETHTSYN_ALLFFL                      0xFFFFFFFFUL
#define ETHTSYN_NANOSEC                     1000000000UL
#define ETHTSYN_MAX_NANOSECOND              0x3B9AC9FFUL
#define ETHTSYN_CORRN_FIE_PORT_MASK         0xFFFFFFFFFFFF0000UL

/* Index to access data structure */
#define ETHTSYN_SYNC_HRD_IDX                0U
#define ETHTSYN_PDREQ_HRD_IDX               1U
#define ETHTSYN_PDRESP_HRD_IDX              2U
#define ETHTSYN_SYNC_FUP_HRD_IDX            3U
#define ETHTSYN_PDRESP_FUP_HRD_IDX          4U

#define ETHTSYN_SYNC_MAINFUN_IDX                0U
#define ETHTSYN_PDREQ_MAINFUN_IDX               1U
#define ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX    2U
#define ETHTSYN_DEB_IDX                         3U

/* Macros for states of Time Bridge messages */
#define ETHTSYN_MSG_RXD                   0x01U
#define ETHTSYN_MSG_TXT                   0x02U
#define ETHTSYN_MSG_CLR                   0x03U
#define ETHTSYN_MSG_INV                   0x04U

/* Sync Follow TLV */
#define ETHTYSN_FOLLOWUP_TLV_TYPE           0x0003U
#define ETHTYSN_FOLLOWUP_TLV_LENGTH         0x001CU
#define ETHTYSN_FOLLOWUP_TLV_ORGAID         0x0080C2U
#define ETHTYSN_FOLLOWUP_TLV_ORGASUBTYPE    0x00001U
#define ETHTYSN_FOLLOWUP_TLV_RATEOFFSET     0x00000000U
#define ETHTYSN_FOLLOWUP_TLV_TIMEBASEIND    0x0000U
#define ETHTYSN_FOLLOWUP_TLV_PHASECHG       0x00U           /* 12 Bytes */
#define ETHTYSN_FOLLOWUP_TLV_FREQCHG        0x000000U

/* Length till TLV header */
#define ETHTYSN_LENGTH_TILL_TLV_HEADER      86U

/* Autosar Compliant Sync FollowUp TLV header */
#define ETHTYSN_AR_FOLLOWUP_TLV_ORGAID          0x1A75FBU
#define ETHTYSN_AR_FOLLOWUP_TLV_ORGASUBTYPE     0x605676U

/* Time Sub TLV */
#define ETHTSYN_TIME_WITH_CRC                   0x28U
#define ETHTSYN_TIME_LENGTH                     5U

/* Status Sub TLV */
#define ETHTSYN_STATUS_WITH_CRC                 0x50U
#define ETHTSYN_STATUS_WITHOUT_CRC              0x51U
#define ETHTSYN_STATUS_LENGTH                   4U

/* UserData Sub TLV */
#define ETHTSYN_USERDATA_WITH_CRC               0x60U
#define ETHTSYN_USERDATA_WITHOUT_CRC            0x61U
#define ETHTSYN_USERDATA_LENGTH                 7U

# if 0
/*OFS not released*/
/* OFS Sub TLV */
#define ETHTSYN_OFS_WITH_CRC                    0x44U
#define ETHTSYN_OFS_WITHOUT_CRC                 0x34U
#define ETHTSYN_OFS_LENGTH                      19U
# endif

/* CRC parameter */
#define ETHTSYN_CRC_START_VALUE                 0xFFU

/* Bit masks for Sub TLVs */
#define ETHTSYN_MASTER_TIME_SUB_TLV             0x01U
#define ETHTSYN_MASTER_STATUS_SUB_TLV           0x02U
#define ETHTSYN_MASTER_USERDATA_SUB_TLV         0x04U

/* Bit masks for time CRC */
#define ETHTSYN_SUBTLV_MESSAGE_LENGTH                  0x01U
#define ETHTSYN_SUBTLV_DOMAIN_NUMBER                   0x02U
#define ETHTSYN_SUBTLV_CORRECTION_FIELD                0x04U
#define ETHTSYN_SUBTLV_SOURCEPORT_IDENTITY             0x08U
#define ETHTSYN_SUBTLV_SEQUENCE_ID                     0x10U
#define ETHTSYN_SUBTLV_PRECISE_ORIGIN_TIMESTAMP        0x20U

/*Masks*/
#define ETHTSYN_STATUS_MASK                  0x01U
#define ETHTSYN_SGW_TO_STATUS_MASK           0x04U
#define ETHTSYN_GLOBAL_TIME_BASE_BIT_MASK    0x08U

#define ETHTSYN_FIRST_BYTE_MASK              0x00FFU
#define ETHTSYN_SECOND_BYTE_MASK             0xFF00U

#define ETHTSYN_WORD_FIRST_BYTE_MASK              0x000000FFU
#define ETHTSYN_WORD_SECOND_BYTE_MASK             0x0000FF00U
#define ETHTSYN_WORD_THIRD_BYTE_MASK              0x00FF0000U
#define ETHTSYN_WORD_FOURTH_BYTE_MASK             0xFF000000U

#define ETHTSYN_DOUBLE_WORD_FIRST_BYTE_MASK       0x00000000000000FFUL
#define ETHTSYN_DOUBLE_WORD_SECOND_BYTE_MASK      0x000000000000FF00UL
#define ETHTSYN_DOUBLE_WORD_THIRD_BYTE_MASK       0x0000000000FF0000UL
#define ETHTSYN_DOUBLE_WORD_FOURTH_BYTE_MASK      0x00000000FF000000UL
#define ETHTSYN_DOUBLE_WORD_FIFTH_BYTE_MASK       0x000000FF00000000UL
#define ETHTSYN_DOUBLE_WORD_SIXTH_BYTE_MASK       0x0000FF0000000000UL
#define ETHTSYN_DOUBLE_WORD_SEVENTH_BYTE_MASK     0x00FF000000000000UL
#define ETHTSYN_DOUBLE_WORD_EIGHTH_BYTE_MASK      0xFF00000000000000UL

#define ETHTSYN_LOWER_DOUBLE_WORD_MASK            0x00000000FFFFFFFFUL
#define ETHTSYN_HIGHER_DOUBLE_WORD_MASK           0xFFFFFFFF00000000UL

/* Macros for Traffic Direction */
#define ETHTSYN_TRANSMISSION        0x01U
#define ETHTSYN_RECEPTION           0x02U

/* Macro for Mac address length */
#define ETHTSYN_MAC_ADDR_LENGTH     0x06U

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
*/

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

extern void EthTSyn_AddTi         (  const Eth_TimeIntDiffType *Time1,
                                     const Eth_TimeIntDiffType *Time2,
                                           Eth_TimeIntDiffType *TimeAdd );

extern void EthTSyn_SubTi         (  const Eth_TimeStampType   *Time1,
                                     const Eth_TimeStampType   *Time2,
                                           Eth_TimeIntDiffType *TimeDiff );

extern void EthTSyn_AvgTi         (  const Eth_TimeIntDiffType *Time1,
                                     const Eth_TimeIntDiffType *Time2,
                                           Eth_TimeIntDiffType *TimeAvg );

extern void EthTSyn_CnvToEthTi    (  uint64               Value_u64,
                                     uint8                Factor_u8,
                                     Eth_TimeIntDiffType *Time_pst );

extern void EthTSyn_CnvEthTitoHex (  Eth_TimeIntDiffType  Time_st,
                                     uint8                Factor_u8,
                                     uint64              *Value_pu64 );

extern Std_ReturnType EthTSyn_GetCurrentVirtualLocalTime( uint8                     *MsgType_pu8,
                                                          StbM_VirtualLocalTimeType *VirtualLocalTime_pst,
                                                          Eth_TimeStampType         *TimeStamp_pst,
                                                          uint8                      Dir_u8,
                                                          uint8                      IdxDomain_u8 );

# if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) || ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) || ( ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON ) )
extern void EthTSyn_Transmit( uint8 MsgTyp_u8,
                              uint8 IdxDomain_u8,
                              uint8 IdxGlbTim_u8 );
# endif

#if ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON )
extern void EthTSyn_ComposeSyncMsg( uint8 *BufPtr_pu8 );

extern void EthTSyn_ComposeFollowUpMsg( uint8 *BufPtr_pu8,
                                        uint8  IdxDomain_u8,
                                        uint8  IdxGlbTimMst_u8 );

extern void EthTSyn_RunSyncMstSM( uint8 IdxDomain_u8 );
# endif

# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if ( ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON )
extern void EthTSyn_ComposePdelayReqMsg( uint8 *BufPtr_pu8 );

extern void EthTSyn_ReceivePdelayResp( const uint8             *DataPtr_pu8,
                                       const Eth_TimeStampType *TimeStamp_pst,
                                             uint16             MsgLen_u16,
                                             uint16             LenByte_u16,
                                             uint8              IdxDomain_u8 );

extern void EthTSyn_ReceivePdelayRespFollowUp( const uint8  *DataPtr_pu8,
                                                     uint16  MsgLen_u16,
                                                     uint16  LenByte_u16,
                                                     uint8   IdxDomain_u8 );

extern void EthTSyn_RunPdelayInitiatorSM( uint8 IdxDomain_u8 );
#  endif

#  if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
extern void EthTSyn_ComposePdelayRespMsg( uint8 *BufPtr_pu8 );

extern void EthTSyn_ComposePdelayRespFollowUpMsg( uint8 *BufPtr_pu8 );

extern void EthTSyn_ReceivePdelayReq( const uint8             *DataPtr_pu8,
                                      const Eth_TimeStampType *TimeStamp_pst,
                                            uint16             MsgLen_u16,
                                            uint16             LenByte_u16,
                                            uint8              IdxDomain_u8 );

extern void EthTSyn_RunPdelayResponderSM( uint8 IdxDomain_u8 );
#  endif
# endif

#if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
extern void EthTSyn_ReceiveSync( const uint8             *DataPtr_pu8,
                                 const Eth_TimeStampType *TimeStamp_pst,
                                       uint16             MsgLen_u16,
                                       uint16             LenByte_u16,
                                       uint8              IdxDomain_u8,
                                       uint8              IdxPort_u8 );

extern void EthTSyn_ReceiveFollowUp( const uint8  *DataPtr_pu8,
                                           uint16  MsgLen_u16,
                                           uint16  LenByte_u16,
                                           uint8   IdxDomain_u8,
                                           uint8   IdxPort_u8 );

extern void EthTSyn_RunSyncSlaveSM( uint8 IdxDomain_u8 );

# if (ETHTSYN_TIME_BRIDGE_SM == STD_ON)
extern void EthTSyn_RunTimBrdgSM( uint8 IdxDomain_u8 );
# endif
#endif
extern void EthTSyn_DomainNumGenn                      ( uint8  CtrlIdx,
                                                         uint8 *IdxDomPtr,
                                                         uint8 *IdxPortPtr,
                                                         uint8 *EthIfCtrlrIdx );

extern Std_ReturnType EthTSyn_SetTransmissionModeDetChk( uint8                         CtrlIdx,
                                                         EthTSyn_TransmissionModeType  Mode,
                                                         uint8                        *IdxDomPtr  );

# if ( STD_OFF != ETHTSYN_DEV_ERROR_DETECT )
extern void EthTSyn_GetCurrentVirtualLocalTimeDetChk   ( Std_ReturnType             Result_en,
                                                         Eth_TimeStampQualType      TimeQual_en );
# endif

extern Std_ReturnType EthTSyn_RxIndicationDetChk       ( uint8         CtrlIdx,
                                                         const uint8  *PhysAddrPtr,
                                                         const uint8  *DataPtr,
                                                               uint8  *IdxDomPtr,
                                                               uint8  *IdxPortPtr );

extern Std_ReturnType EthTSyn_TxConfirmationDetChk     ( uint8   CtrlIdx,
                                                         uint8  *IdxDomPtr );

# if ( ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON ) && (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) )
extern Std_ReturnType EthTSyn_ReceiveSyncDetChk        ( const uint8 *DataPtr_pu8,
                                                               uint8  IdxGlbTimSlv_u8 );
# endif

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"

/* EthTSyn call status flag */
extern EthTSyn_InitStatus_ten EthTSyn_InitStatus_en;

/* EthTSyn configuration pointer */
extern const EthTSyn_ConfigType *EthTSyn_CfgPtr_pcst;
extern const EthTSyn_TimeDomainCfg_tst *EthTSyn_TimeDomainCfg_pcst;

#define ETHTSYN_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#define ETHTSYN_START_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"

# if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if ( (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON) || (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON) )
/* EthTSyn message header */
extern const EthTSyn_ConstMsgHdr_tst EthTSyn_HdrCfg_cast[5];
#  endif
# endif

#define ETHTSYN_STOP_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"


#define ETHTSYN_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "EthTSyn_MemMap.h"

/* Table that stores EthTSyn_RamDataTyp_tst values */
extern EthTSyn_RamDataTyp_tst EthTSyn_RamDataTyp_ast[ETHTSYN_NUMBER_TIME_DOMAIN_CONFIGURED];

# if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
/* Table that stores variables related to Slave */
extern EthTSyn_Slave_tst EthTSyn_Slave_ast[ETHTSYN_NUMBER_SLAVE_PORTS];
# endif


#  if (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON)
/* Table that stores variables related to Master */
extern EthTSyn_Master_tst EthTSyn_Master_ast[ETHTSYN_NUMBER_MASTER_PORTS];
#  endif

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#  if (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON)
/* Table that stores variables related to Pdelay Initiator */
extern EthTSyn_PdelayInitiator_tst EthTSyn_PdelayInitiator_st;
#  endif

#  if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
/* Table that stores variables related to Pdelay Responder */
extern EthTSyn_PdelayResponder_tst EthTSyn_PdelayResponder_st;
#  endif

# endif

/* Variable that stores Calculated Pdelay Value */
# if ( (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON) || (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON ) )
extern Eth_TimeIntDiffType EthTSyn_Pdelay_ast[ETHTSYN_NUMBER_PORTS];
# endif

# if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
/* Global structure that stores variables related to Time Bridge */
extern EthTSyn_TiBrdg_tst EthTSyn_TimBrdg_st;
# endif

# ifdef ETHTSYN_SWITCH
#  if ( ( ETHTSYN_GLOBAL_TIME_MASTER == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
/* Variable that stores management object for master */
extern EthTSyn_MstMgmtObject_tst  EthTSyn_MstMgmtObject_ast[ETHTSYN_NUMBER_MASTER_PORTS];
#  endif

#  if (ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON)
/* Variable that stores management object for slave */
extern EthTSyn_SlaveMgmtObject_tst EthTSyn_SlaveMgmtObject_ast[ETHTSYN_NUMBER_SLAVE_PORTS];
#  endif
# endif

#define ETHTSYN_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_PRV_H */

/**
 ***************************************************************************************************
 *
 ***************************************************************************************************
 */
