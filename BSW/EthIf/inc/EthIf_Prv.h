

#ifndef ETHIF_PRV_H
#define ETHIF_PRV_H

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#if (ETHIF_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

#include "EthIf_Prv_Types.h"


/*
***************************************************************************************************
* Defines
***************************************************************************************************
*/

/* Location of the TPID (if VLAN is enabled) or EtherType (if VLAN is disabled) in the Ethernet frame */
#define ETHIF_ETHTYPE_BYTE1                     12U
#define ETHIF_ETHTYPE_BYTE2                     13U

/* Offset where the VLAN Id starts (if VLAN is enabled) or where the payload starts (if VLAN is disabled) */
#define ETHIF_ETHTHDR_OFFSET                    14U

/* Location of the VLAN Id */
#define ETHIF_VLANTAG_BYTE1                     0U
#define ETHIF_VLANTAG_BYTE2                     1U

/* Location of EtherType in VLAN tagged frame */
#define ETHIF_ETHTYPE_IN_VLAN_BYTE1             2U
#define ETHIF_ETHTYPE_IN_VLAN_BYTE2             3U

/* Length of EtherType in bytes */
#define ETHIF_ETHTYPEFIELD_LENGTH               2U

/* Length of VLAN tag in bytes */
#define ETHIF_VLANTAG_LENGTH                    4U

/* Length of Src and Dst MAC addresses in bytes */
#define ETHIF_MACHDR_LENGTH                     12U

/* Offset where the Src MAC address starts in the Ethernet frame */
#define ETHIF_OFFSET_SOURCEMACADDR              6U

/* Offset where the payload starts if VLAN is disabled */
#define ETHIF_OFFSET_PAYLOAD_NOVLAN             14U

/* Offset where the payload starts if VLAN is enabled */
#define ETHIF_OFFSET_PAYLOAD_VLAN               18U

/* FramePrio is a 3 bit field in Vlan header, it's maximun value is 7 */
#define ETHIF_FRAMEPRIO_MAXVAL                  7U

/* The maximum value a uint8, uint16 and uint32 variable can hold. */
#define ETHIF_UINT8_MAX                         0xFFU
#define ETHIF_UINT16_MAX                        0xFFFFU
#define ETHIF_UINT32_MAX                        0xFFFFFFFFUL

/* ZeroCopyTx - Size of the Ethernet header present in the Reuse header */
#define ETHIF_ZEROCOPY_ETHERNET_HDR_LEN         18U

/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

/* Variable declarations */
#define ETHIF_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

/* EthIf_Init API call status flag */
extern EthIf_StateType                EthIf_InitStatus_en;

/* EthIf configuration pointer */
extern const EthIf_ConfigType *       EthIf_CfgPtr_pco;

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
/* Flag to indicate a link state change for at least one switch port */
extern boolean                        EthIf_SwitchPortLinkStateChanged_b;
#endif

#define ETHIF_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

#define ETHIF_START_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )

/* Global variable for EthIf Switch Ports active timer */
extern uint32                         EthIf_PortActiveTime_u32;

#endif

/* Global variables to store measurement data related to EthIf. */
#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

/* Global variable to store the count of dropped frames due to invalid CtrlIdx/VlanId. */
extern uint32                         EthIf_InvalidCtrlIdxDropCnt_u32;

/* Global variable to store the count of dropped frames due to invalid EtherType. */
extern uint32                         EthIf_InvalidEtherTypeDropCnt_u32;

#endif

#define ETHIF_STOP_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * DET Defines
 ***************************************************************************************************
 */

/* API IDs for DET Support (Autosar APIs)*/
#define ETHIF_SID_ETHIF_INIT                                0x1U
#define ETHIF_SID_ETHIF_SETCONTROLLERMODE                   0x3U
#define ETHIF_SID_ETHIF_GETCONTROLLERMODE                   0x4U
#define ETHIF_SID_ETHIF_MAINFUNCTIONSTATE                   0x5U
#define ETHIF_SID_ETHIF_SWITCHPORTGROUPREQUESTMODE          0x6U
#define ETHIF_SID_ETHIF_STARTALLPORTS                       0x7U
#define ETHIF_SID_ETHIF_GETPHYSADDR                         0x8U
#define ETHIF_SID_ETHIF_PROVIDETXBUFFER                     0x9U
#define ETHIF_SID_ETHIF_TRANSMIT                            0xAU
#define ETHIF_SID_ETHIF_GETVERSIONINFO                      0xBU
#define ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER               0xCU
#define ETHIF_SID_ETHIF_SETPHYSADDR                         0xDU
#define ETHIF_SID_ETHIF_TRCVMODEINDICATION                  0xFU
#define ETHIF_SID_ETHIF_CBK_RXINDICATION                    0x10U
#define ETHIF_SID_ETHIF_CBK_TXCONFIRMATION                  0x11U
#define ETHIF_SID_ETHIF_SETPHYLOOPBACKMODE                  0x12U
#define ETHIF_SID_ETHIF_SETPHYTXMODE                        0x13U
#define ETHIF_SID_ETHIF_GETCABLEDIAGNOSTICSRESULT           0x14U
#define ETHIF_SID_ETHIF_GETPHYIDENTIFIER                    0x15U
#define ETHIF_SID_ETHIF_SETPHYTESTMODE                      0x17U
#define ETHIF_SID_ETHIF_GET_TRCV_SIGNALQUALITY              0x18U
#define ETHIF_SID_ETHIF_CLEAR_TRCV_SIGNALQUALITY            0x19U
#define ETHIF_SID_ETHIF_GET_SWITCHPORT_SIGNALQUALITY        0x1AU
#define ETHIF_SID_ETHIF_CLEAR_SWITCHPORT_SIGNALQUALITY      0x1BU
#define ETHIF_SID_ETHIF_MAINFUNCTIONTX                      0x21U
#define ETHIF_SID_ETHIF_GETCURRENTTIME                      0x22U
#define ETHIF_SID_ETHIF_ENABLE_EGRESS_TIMESTAMP             0x23U
#define ETHIF_SID_ETHIF_GET_EGRESS_TIMESTAMP                0x24U
#define ETHIF_SID_ETHIF_GETINGRESSTIMESTAMP                 0x25U
#define ETHIF_SID_ETHIF_GETPORTMACADDR                      0x28U
#define ETHIF_SID_ETHIF_GETARLTABLE                         0x29U
#define ETHIF_SID_ETHIF_MAINFUNCTIONRX_PRIO                 0x42U
#define ETHIF_SID_ETHIF_GETVLANID                           0x43U
#define ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA               0x45U
#define ETHIF_SID_ETHIF_SWITCHPORTMODEINDICATION            0x46U
#define ETHIF_SID_ETHIF_GETRXMGMTOBJECT                     0x47U
#define ETHIF_SID_ETHIF_GETTXMGMTOBJECT                     0x48U
#define ETHIF_SID_ETHIF_GETSWITCHPORTMODE                   0x49U
#define ETHIF_SID_ETHIF_GETTRANSCEIVERMODE                  0x4AU
#define ETHIF_SID_ETHIF_SWITCHPORTGETLINKSTATE              0x4BU
#define ETHIF_SID_ETHIF_TRANSCEIVERGETLINKSTATE             0x4CU
#define ETHIF_SID_ETHIF_SWITCHPORTGETBAUDRATE               0x4DU
#define ETHIF_SID_ETHIF_TRANSCEIVERGETBAUDRATE              0x4EU
#define ETHIF_SID_ETHIF_SWITCHPORTGETDUPLEXMODE             0x4FU
#define ETHIF_SID_ETHIF_TRANSCEIVERGETDUPLEXMODE            0x50U
#define ETHIF_SID_ETHIF_SWITCHPORTGETCOUNTERVALUES          0x51U
#define ETHIF_SID_ETHIF_SWITCHPORTGETRXSTATS                0x52U
#define ETHIF_SID_ETHIF_SWITCHPORTGETTXSTATS                0x53U
#define ETHIF_SID_ETHIF_SWITCHPORTGETTXERRORCOUNTERVALUES   0x54U
#define ETHIF_SID_ETHIF_SWITCHPORTGETMACLEARNINGMODE        0x55U
#define ETHIF_SID_ETHIF_GETSWITCHPORTIDENTIFIER             0x56U
#define ETHIF_SID_ETHIF_GETSWITCHIDENTIFIER                 0x57U
#define ETHIF_SID_ETHIF_WRITEPORTMIRRORCONFIGURATION        0x58U
#define ETHIF_SID_ETHIF_READPORTMIRRORCONFIGURATION         0x59U
#define ETHIF_SID_ETHIF_DELETEPORTMIRRORCONFIGURATION       0x5AU
#define ETHIF_SID_ETHIF_GETPORTMIRRORSTATE                  0x5BU
#define ETHIF_SID_ETHIF_SETPORTMIRRORSTATE                  0x5CU
#define ETHIF_SID_ETHIF_SETPORTTESTMODE                     0x5DU
#define ETHIF_SID_ETHIF_SETPORTLOOPBACKMODE                 0x5EU
#define ETHIF_SID_ETHIF_SETPORTTXMODE                       0x5FU
#define ETHIF_SID_ETHIF_GETPORTCABLEDIAGNOSTICSRESULT       0x60U
#define ETHIF_SID_ETHIF_RUNPORTCABLEDIAGNOSTIC              0x61U
#define ETHIF_SID_ETHIF_RUNCABLEDIAGNOSTIC                  0x62U
#define ETHIF_SID_ETHIF_GETCFGDATARAW                       0x63U
#define ETHIF_SID_ETHIF_GETCFGDATAINFO                      0x64U
#define ETHIF_SID_ETHIF_GETMAXFIFOBUFFERFILLLEVEL           0x65U

/* API IDs for DET Support (Non-Autosar APIs) */
#define ETHIF_SID_ETHIF_SWITCHLINKUPINDICATION              0xA0U
#define ETHIF_SID_ETHIF_SWITCHLINKDOWNINDICATION            0xA1U
#define ETHIF_SID_ETHIF_OVERRIDEBUFTXSRCADDR                0xA2U
#define ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER                  0xA3U
#define ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT                    0xA4U
#define ETHIF_SID_ETHIF_ZEROCOPYMARKLASTTX                  0xA5U
#define ETHIF_SID_ETHIF_CBK_ZEROCOPYTXCONFIRMATION          0xA6U
#define ETHIF_SID_ETHIF_PHYSCTRLINIT                        0xA7U
#define ETHIF_SID_ETHIF_SETPHYSCTRLMODE                     0xA8U

/* Development Error Codes for DET Support */
#define ETHIF_E_INV_CTRL_IDX                                0x1U /* Invalid controller index */
#define ETHIF_E_INV_TRCV_IDX                                0x2U /* Invalid transceiver index */
#define ETHIF_E_INV_SWT_IDX                                 0x3U /* Invalid switch index */
#define ETHIF_E_INV_PORT_GROUP_IDX                          0x4U /* Invalid Port Group index */
#define ETHIF_E_NOT_INITIALIZED                             0x5U /* EthIf module was not initialized */
#define ETHIF_E_PARAM_POINTER                               0x6U /* Invalid pointer in parameter list */
#define ETHIF_E_INV_PARAM                                   0x7U /* Invalid parameter */
#define ETHIF_E_INIT_FAILED                                 0x8U /* Initialization failed */


/*
 ***************************************************************************************************
 * DET Check
 ***************************************************************************************************
 */

/* No reporting to DET module if Development Detection Error is OFF */
/* According to CAP Coding Guideline, Development Error detection shall not be controlled by configuration switch DevErrorDetect for safety reasons. */
/* This behavior is compliant to runtime-error behavior and a deviation from AUTOSAR */
#if (ETHIF_DEV_ERROR_DETECT == STD_ON)
#define ETHIF_CALL_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(ETHIF_MODULE_ID, ETHIF_INSTANCE_ID, API, ERROR);
#else
#define ETHIF_CALL_DET_REPORTERROR(API, ERROR)
#endif

/* DET For functions with return type as 'void' */
#define ETHIF_REPORT_ERROR_RET_VOID( CONDITION, API, ERROR )            \
    if( CONDITION )                                                     \
    {                                                                   \
        ETHIF_CALL_DET_REPORTERROR(API, ERROR)                          \
        return;                                                         \
    }                                                                   \

/* DET For functions with return type as 'Std_ReturnType' */
#define ETHIF_REPORT_ERROR_RET_VALUE( CONDITION, API, ERROR, RETVAL )   \
    if( CONDITION )                                                     \
    {                                                                   \
        ETHIF_CALL_DET_REPORTERROR(API, ERROR)                          \
        return (RETVAL);                                                \
    }                                                                   \

/* DET Macro for functions to report DET always, without a return statement.  */
#define ETHIF_REPORT_ERROR_TRUE_NO_RET( API, ERROR )                    \
    ETHIF_CALL_DET_REPORTERROR(API, ERROR)                              \


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

#if ( ETHIF_ENABLE_RX_POLLING == STD_ON )
/* MainFunctionRx for Priority Processing */
void EthIf_Prv_MainFunctionRx_PriorityProcessing             ( uint8 EthIfPhysCtrlIdx_u8,
                                                               uint8 FifoIndex_u8,
                                                               uint16 MaxIterations_u16,
                                                               uint32 RuntimeLimit_u32 );
#endif

/* Function called from EthIf_MainFunctionState to trigger the Link State processing */
void EthIf_Prv_LinkStateProcessing                           ( void );

/* Function called from EthIf_MainFunctionState to poll signal quality */
#if( ETHIF_ENABLE_SIGNALQUALITY == STD_ON )
void EthIf_Prv_SignalQualityProcessing                       ( void );
#endif

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
/* Trigger all possible state transitions for EthIfCtrl refering a Transceiver */
void EthIf_Prv_Trcv_StateTransitions                         ( uint8 EthIfCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );
#endif

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON )
/* To trigger all possible state transitions for EthIfCtrl refering to EthIfSwitch */
void EthIf_Prv_SwtOrPGCtrl_StateTransitions                  ( uint8 EthIfCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );
#endif

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
/* Trigger all possible state transitions for EthIfCtrl refering a Port Group of semantic "link-info". */
void EthIf_Prv_SwtPGLinkInfo_StateTransitions                ( uint8 EthIfCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );
#endif

/* To trigger all possible state transitions for EthIfCtrl having no reference to either Trcv/Switch/SwitchPortGroup */
void EthIf_Prv_NoRef_StateTransitions                        ( uint8 EthIfCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );

#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
/* Trigger all possible state transitions for Standalone Port Group (not referenced by any EthIfCtrl) */
void EthIf_Prv_StandaloneSwtPG_StateTransitions              ( void );
#endif

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
/* This function must be called by the EthIf_Prv_SetControllerMode function, before setting the eth controller to ETHCTRL_MODE_ACTIVE */
void EthIf_Prv_ResetPhysAddrFilterReferencesForEthHwCtrlIdx  ( uint8 PhysCtrlIdx_u8 );

/* This function must be called by the EthIf_Init. It clears the physical address filters for all configured physical controllers */
void EthIf_Prv_ResetPhysAddrFilterReferences                 ( void );
#endif

/* Initializes the indexed controller */
Std_ReturnType EthIf_Prv_ControllerInit                      ( uint8 EthIfCtrlIdx_u8 );

/* Initializes the indexed physical controller */
Std_ReturnType EthIf_Prv_PhysCtrlInit                        ( uint8 EthIfPhysCtrlIdx_u8 );

/* Enables / disables the indexed controller */
Std_ReturnType EthIf_Prv_SetControllerMode                   ( uint8 EthIfCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );
/* Enables the indexed physical controller */
Std_ReturnType EthIf_Prv_SetPhysCtrlMode                     ( uint8 EthIfPhysCtrlIdx_u8,
                                                               Eth_ModeType CtrlReqMode_en );

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
/* Initializes the indexed Transceiver */
Std_ReturnType EthIf_Prv_TransceiverInit                     ( uint8 EthIfCtrlIdx_u8 );

/* Enable / disable the indexed Transceiver */
Std_ReturnType EthIf_Prv_SetTransceiverMode                  ( uint8 EthIfCtrlIdx_u8,
                                                               EthTrcv_ModeType TrcvReqMode_en );

/*Calls API EthTrcv_TransceiverLinkStateRequest() if the previous call has failed  */
void EthIf_Prv_RetryTransceiverLinkStateActive               ( uint8 EthIfCtrlIdx_u8 );
#endif

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
/* Initializes the indexed Switch */
Std_ReturnType EthIf_Prv_SwitchInit                          ( uint8 EthIfCtrlIdx_u8 );

/* Checks whether at least one port (host or uplink or standard port) is in a particular state. */
boolean EthIf_Prv_CheckSwitchOrPGPortInState                 ( const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8,
                                                               EthIf_SwtPortState_ten PortState_en );

/* Check if a Switch or Switch Port Group is active (Host port, all uplink ports and one standard port are activated) */
boolean EthIf_Prv_CheckSwitchOrPGActiveStatus                ( const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8);

/* Check if a Switch or Switch Port Group has been successfully de-activated */
boolean EthIf_Prv_CheckSwitchOrPGDownStatus                  ( const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8 );

/* Request ports of a Switch or Switch Port Group to be active */
Std_ReturnType EthIf_Prv_SetSwitchOrPGModeActive             ( const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8 );

/* Request ports of a Switch or Switch Port Group to be down */
Std_ReturnType EthIf_Prv_SetSwitchOrPGModeDown               ( const uint8 * HostUplinkPortTable_pcau8,
                                                               const uint8 * StdPortTable_pcau8,
                                                               uint8 SizeHostUplinkPortTable_u8,
                                                               uint8 SizeStdPortTable_u8 );
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_PRV_H */
