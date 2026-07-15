

#ifndef ETHIF_TYPES_H
#define ETHIF_TYPES_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "Eth_GeneralTypes.h"
#include "EthIf_Cfg.h"

#if ( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
#include "EthSwt.h"
#endif


/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */

/* -------------------------------------------- */
/* Typedef                                      */
/* -------------------------------------------- */

/* [SWS_EthIf_91101] Data Type that represents the Ethernet interface switch port group index. The index is zero based and unique for every configured switch port group */
typedef uint8 EthIf_SwitchPortGroupIdxType;

/* [SWS_EthIf_91010] Index to select specific measurement data. */
typedef uint8 EthIf_MeasurementIdxType;

/* -------------------------------------------- */
/* Enumerations                                 */
/* -------------------------------------------- */

/* Data type to define the possible reference type of an EthIfCtrl */
typedef enum
{
    ETHIF_ETHIFCTRL_REF_NOREF                       = 0x00U,    /* No reference is configured for the EthIf controller */
    ETHIF_ETHIFCTRL_REF_TRCV                        = 0x01U,    /* The reference is to a physical transceiver */
    ETHIF_ETHIFCTRL_REF_SWITCH                      = 0x02U,    /* The reference is to an ethernet switch */
    ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO          = 0x04U,    /* The reference is to a switch port group. The semantic of the reference is 'link-information'. */
    ETHIF_ETHIFCTRL_REF_PORTGROUP_CTRL              = 0x08U     /* The reference is to a switch port group. The semantic of the reference is 'control'. */
} EthIf_EthIfCtrlRef_ten;

/* Data type to define the state of an EthIfCtrl. This is used in state machine processing */
typedef enum
{
    ETHIF_ETHIFCTRL_STATE_DOWN                      = 0x00U,    /* EthIfCtrl is in state DOWN */
    ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_ACTIVE      = 0x01U,    /* EthIfCtrl is requested for ACTIVE, but Trcv Mode indication / Swt port mode indication is not yet given */
    ETHIF_ETHIFCTRL_STATE_ACTIVE                    = 0x02U,    /* EthIfCtrl is in state ACTIVE. Mode of HW configured for this EthIfCtrl (EthCtrl and optionally EthTrcv or EthSwt or PG) is active */
    ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN        = 0x03U,    /* EthIfCtrl is requested for DOWN, but Trcv Mode indication / Swt port mode indication is not yet given */
    ETHIF_ETHIFCTRL_STATE_WAIT_CTRLMODE_ACTIVE      = 0x04U     /* EthIfCtrl is requested for ACTIVE, but activation of switch port is not yet accepted */
} EthIf_EthIfCtrlState_ten;

/* Data type to define the list of errors of an EthIfCtrl */
typedef enum
{
    ETHIF_ETHIFCTRL_ERROR_NONE                      = 0x00U,    /* EthIfCtrl has no error */
    ETHIF_ETHIFCTRL_ERROR_TRCV_WAIT_ACTIVE          = 0x01U,    /* EthIfCtrl is in error mode due to unexpected Trcv mode indication to DOWN while EthIfCtrl is in WAIT_ACTIVE */
    ETHIF_ETHIFCTRL_ERROR_TRCV_ACTIVE               = 0x02U,    /* EthIfCtrl is in error mode due to unexpected Trcv mode indication to DOWN while EthIfCtrl is in ACTIVE */
    ETHIF_ETHIFCTRL_ERROR_SWT_HOST_OR_ALLSTD_PORT   = 0x03U,    /* EthIfCtrl is in error mode due to unexpected Switch Port mode indication to DOWN for the host port or all standard ports of switch or PG */
    ETHIF_ETHIFCTRL_ERROR_SWT_ALL_PORT              = 0x04U     /* EthIfCtrl is in error mode due to unexpected Switch Port mode indication to DOWN for all ports of switch or PG */
} EthIf_EthIfCtrlError_ten;

/* Data type to define the Link State */
typedef enum
{
    ETHIF_LINKSTATE_DOWN                            = 0x00U,    /* Link State is DOWN */
    ETHIF_LINKSTATE_ACTIVE                          = 0x01U     /* Link State is ACTIVE */
} EthIf_EthIfLinkState_ten;

/* Data type to define the EthCtrl state in the context of EthIf */
typedef enum
{
    ETHIF_CTRL_STATE_UNINIT                         = 0x00U,    /* Referenced EthCtrl is not initialized */
    ETHIF_CTRL_STATE_DOWN                           = 0x01U,    /* Referenced EthCtrl is DOWN */
    ETHIF_CTRL_STATE_ACTIVE                         = 0x02U     /* Referenced EthCtrl is ACTIVE */
} EthIf_CtrlState_ten;

/* Data type to define the Transceiver state in the context of EthIf */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
typedef enum
{
    ETHIF_TRCV_STATE_UNINIT                         = 0x00U,    /* Referenced EthTrcv is not initialized */
    ETHIF_TRCV_STATE_DOWN                           = 0x01U,    /* Referenced EthTrcv is DOWN */
    ETHIF_TRCV_STATE_WAIT_ACTIVE                    = 0x02U,    /* Referenced EthTrcv is requested to be ACTIVE but Trcv mode indication is not yet given */
    ETHIF_TRCV_STATE_ACTIVE                         = 0x03U,    /* Referenced EthTrcv is ACTIVE */
    ETHIF_TRCV_STATE_WAIT_DOWN                      = 0x04U     /* Referenced EthTrcv is requested to be DOWN but Trcv mode indication is not yet given */
} EthIf_TrcvState_ten;
#endif

/* Data type to define the state of a Switch */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
typedef enum
{
    ETHIF_SWT_STATE_UNINIT                           = 0x00U,    /* Referenced EthSwt is not initialized */
    ETHIF_SWT_STATE_INIT                             = 0x01U     /* Referenced EthSwt is initialized */
} EthIf_SwtState_ten;
#endif

/* Data type to define the state of a Switch port */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
typedef enum
{
    ETHIF_SWTPORT_STATE_DOWN                         = 0x00U,    /* Port of the Switch is DOWN */
    ETHIF_SWTPORT_STATE_WAIT_ACTIVE                  = 0x01U,    /* Port of the Switch is requested to be ACTIVE but Switch port mode indication is not yet given */
    ETHIF_SWTPORT_STATE_ACTIVE                       = 0x02U,    /* Port of the Switch is ACTIVE */
    ETHIF_SWTPORT_STATE_WAIT_DOWN                    = 0x03U     /* Port of the Switch is requested to be DOWN but Switch port mode indication is not yet given */
} EthIf_SwtPortState_ten;
#endif

/* Data type to define the semantic of a Switch PG */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
typedef enum
{
    ETHIF_SWTPORTGROUP_SEMANTIC_CONTROL             = 0x00U,    /* Port Group is referenced by an EthIfCtrl and its semantic is "control" */
    ETHIF_SWTPORTGROUP_SEMANTIC_LINK_INFO           = 0x01U,    /* Port Group is referenced by an EthIfCtrl and its semantic is "link-info" */
    ETHIF_SWTPORTGROUP_UNREFERENCED                 = 0x02U     /* Port Group is not referenced by an EthIfCtrl */
} EthIf_SwtPortGroupSemantic_ten;
#endif

/* Data type to define the state of a standalone Switch PG. This is used in state machine processing */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
typedef enum
{
    ETHIF_STANDALONE_PG_MODE_DOWN                   = 0x00U,    /* Standalone Switch Port Group is in state DOWN */
    ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_ACTIVE   = 0x01U,    /* Standalone Switch Port Group is requested to be ACTIVE but Switch port mode indication is not yet given */
    ETHIF_STANDALONE_PG_MODE_STARTUP                = 0x02U,    /* Standalone Switch Port Group is made ACTIVE for configured time EthIfPortStartupActiveTime */
    ETHIF_STANDALONE_PG_MODE_ACTIVE                 = 0x03U,    /* Standalone Switch Port Group is made ACTIVE by BswM */
    ETHIF_STANDALONE_PG_MODE_WAITDOWN               = 0x04U,    /* Standalone Switch Port Group is requested for DOWN, and changes after configured time EthIfSwitchOffPortTimeDelay */
    ETHIF_STANDALONE_PG_MODE_WAIT_TRCVMODE_DOWN     = 0x05U     /* Standalone Switch Port Group is requested to be DOWN but Switch port mode indication is not yet given */
} EthIf_StandaloneSwtPortGroupMode_ten;
#endif

/* -------------------------------------------- */
/* Structures                                   */
/* -------------------------------------------- */

/* [SWS_EthIf_91057] Structure to store polled EthTrcv/EthSwt port signal quality */
typedef struct
{
    uint32 HighestSignalQuality;              /* The highest link signal quality of a link since last clear */
    uint32 LowestSignalQuality;               /* The lowest link signal quality of a link since last clear  */
    uint32 ActualSignalQuality;               /* Actual link signal quality                                 */
} EthIf_SignalQualityResultType;

#if( ETHIF_ENABLE_SIGNALQUALITY  == STD_ON )
/* EthIf implementation specific data type to manage polled signal quality results */
typedef struct
{
    EthIf_SignalQualityResultType EthIfSignalQualityResultType_st;   /* Standard AUTOSAR data type to hold polled signal quality results */
    boolean SignalQualityValid_b;              /* Set to TRUE if the member EthIf_SignalQualityResultType is updated after successful polling of EthTrcv/EthSwt */
} EthIf_SignalQualityMeasurement_tst;
#endif

/* General table structure of EthIf */
typedef struct
{
    const uint8 nrEthIfCtrl_cu8;                    /* Total number of configured EthIfCtrls             */
    const uint8 nrFrameOwners_cu8;                  /* Total number of configured Frame Owners           */
    const uint8 nrEthIfPhysCtrl_cu8;                /* Total number of configured EthIfPhysCtrls         */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
    const uint8 nrEthIfTransceiver_cu8;             /* Total number of configured EthIfTransceivers      */
#endif
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
    const uint8 nrEthIfSwitches_cu8;                /* Total number of configured EthIfSwitches          */
    const uint8 nrEthIfSwtPorts_cu8;                /* Total number of configured Ports for all Switches */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const uint8 nrEthIfSwtPortGroup_cu8;            /* Total number of configured Switch Port Groups     */
#endif
#endif
} EthIf_General_tst;

/* FrameOwner table structure of EthIf which contains configured callback function references */
typedef struct
{
    const Eth_FrameType FrameType_cu16;              /* Selects the Ethernet frame type e.g. 0x800 = IPv4 */

    void (*EthIfRxIndication_pfct) ( uint8 CtrlIdx,            /* RxIndication function pointer */
                                     Eth_FrameType FrameType,
                                     boolean IsBroadcast,
                                     uint8 * PhysAddrPtr,
                                     uint8 * DataPtr,
                                     uint16 LenByte );

    void (*EthIfTxConfirmation_pfct) ( uint8 EthIfCtrlIdx_u8,  /* TxConfirmation function pointer */
                                       uint8 BufIdx_u8 );

#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON )
    void (*EthIfZeroCopyTxConfirmation_pfct) ( uint8 EthIfCtrlIdx_u8,    /* ZeroCopyTxConfirmation function pointer */
                                               uint32 TxId_u32 );
#endif
    const boolean RxBypassEnabled_cb;                /* Rx bypass flag (when enabled, EthIfCtrl identification in EthIf_RxIndication will not be executed) */
} EthIf_FrameOwnerConfig_tst;

/* Static EthIfCtrl table structure of EthIf which stores static configurations like VLAN id or MTU size */
typedef struct
{
    const EthIf_EthIfCtrlRef_ten ReferenceType_cen;   /* Reference type of EthIf controller (Trcv or Swt or Swt PG of type "control" or Swt PG of type"link-info" or no reference) */

    const uint16 VlanId_cu16;                         /* Vlan Id of the EthIfCtrl.
                                                       * If parameter EthIfVlanId is configured, this structure member will hold
                                                       * configured value.
                                                       * If parameter EthIfVlanId is not configured, this structure member will hold
                                                       * 0xFFFF.
                                                       * EthIfVlanId can be configured to 0 if priority tagged frames are required */

    const uint16 MtuSize_cu16;                        /* Specifies the maximum transmission unit (MTU) of the EthIfCtrl in bytes */

    const uint8 RefPhysCtrlTableIdx_cu8;              /* Index of the referenced PhysCtrl table in the context of EthIf on which this connection will be transmitted/received */

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const uint8 RefTrcvOrSwtOrPGTableIdx_cu8;         /* Index of the referenced Transceiver or Switch or Switch Port Group table in the context of EthIf.
                                                       * If the EthIfCtrl does not reference a Transceiver or Switch or Switch Port Group, this is set to 0xFF. */
#endif
} EthIf_StaticEthIfCtrl_tst;

/* Dynamic EthIfCtrl table structure of EthIf which maintains the state of each EthIfCtrl */
typedef struct
{
     Eth_ModeType ReqMode_en;                         /* Requested mode of the EthIfCtrl */
     EthIf_EthIfCtrlState_ten State_en;               /* Current state of the EthIfCtrl */
     EthIf_EthIfLinkState_ten LinkState_en;           /* Link state of the EthIfCtrl */
     EthIf_EthIfCtrlError_ten ErrorFlg_en;            /* Error flag of the EthIfCtrl. If unexpected indication is received from the
                                                       * underlying HW controller, the EthIfCtrl will be marked with error flag. EthSM
                                                       * will then execute the restarting of the EthIfCtrl. */
} EthIf_DynamicEthIfCtrl_tst;

/* EthDriver table structure of EthIf which contains configured Eth Driver APIs */
typedef struct
{
    Std_ReturnType (*EthControllerInit_pfct) ( uint8 CtrlIdx_u8,            /* ControllerInit function pointer */
                                               uint8 CfgIdx_u8 );

    Std_ReturnType (*EthSetControllerMode_pfct) ( uint8 CtrlIdx_u8,         /* SetControllerMode function pointer */
                                                  Eth_ModeType CtrlMode_en );

    void (*EthGetPhysAddr_pfct) ( uint8 CtrlIdx_u8,                         /* GetPhysAddr function pointer */
                                  uint8 * PhysAddrPtr_pu8 );


     void (*EthSetPhysAddr_pfct) ( uint8 CtrlIdx_u8,               /* SetPhysAddr function pointer */
                                   const uint8 * PhysAddrPtr_pu8 );

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
    Std_ReturnType (*EthUpdatePhysAddrFilter_pfct) ( uint8 CtrlIdx_u8,      /* UpdatePhysAddrFilter function pointer */
                                                     const uint8 * PhysAddrPtr_pcu8,
                                                     Eth_FilterActionType Action_en );
#endif

    BufReq_ReturnType (*EthProvideTxBuffer_pfct) ( uint8 CtrlIdx_u8,        /* ProvideTxBuffer function pointer */
                                                   uint8 * BufIdxPtr_pu8,
                                                   Eth_DataType ** BufPtr_ppo,
                                                   uint16 * LenBytePtr_pu16 );

    Std_ReturnType (*EthTransmit_pfct) ( uint8 CtrlIdx_u8,                  /* Transmit function pointer */
                                         uint8 BufIdx_u8,
                                         Eth_FrameType FrameType_o,
                                         boolean TxConfirmation_b,
                                         uint16 LenByte_u16,
                                         uint8 * PhysAddrPtr_pu8 );
#if( ETHIF_ENABLE_RX_POLLING == STD_ON )
    void (*EthReceive_pfct) ( uint8 CtrlIdx_u8,                             /* Receive function pointer */
                              uint8 FifoIdx_u8,
                              Eth_RxStatusType * RxStatusPtr );
#endif
#if( ETHIF_ENABLE_TX_POLLING == STD_ON )
    void (*EthTxConfirmation_pfct) ( uint8 CtrlIdx_u8 );                    /* TxConfirmation function pointer */
#endif
#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
    Std_ReturnType (*EthGetCurrentTime_pfct) ( uint8 CtrlIdx_u8,            /* GetCurrentTime function pointer */
                                               Eth_TimeStampQualType * timeQualPtr,
                                               Eth_TimeStampType * timeStampPtr );

    void (*EthEnableEgressTimeStamp_pfct) ( uint8 CtrlIdx_u8,               /* EnableEgressTimeStamp function pointer */
                                            uint8 BufIdx_u8 );

    void (*EthGetEgressTimeStamp_pfct) ( uint8 CtrlIdx_u8,                  /* GetEgressTimeStamp function pointer */
                                         uint8 BufIdx_u8,
                                         Eth_TimeStampQualType * timeQualPtr,
                                         Eth_TimeStampType * timeStampPtr );

    void (*EthGetIngressTimeStamp_pfct) ( uint8 CtrlIdx_u8,                 /* GetIngressTimeStamp function pointer */
                                          const Eth_DataType * DataPtr,
                                          Eth_TimeStampQualType * timeQualPtr,
                                          Eth_TimeStampType * timeStampPtr );
#endif
#if( ETHIF_ENABLE_OVERRIDE_SRCMAC_SUPPORT == STD_ON)
    Std_ReturnType (*EthOverrideTxBufSrcAddr_pfct) ( uint8 CtrlIdx_u8,      /* OverrideTxBufSrcAddr function pointer */
                                                     uint8 BufIdx_u8,
                                                     const uint8 * SrcMacAddrPtr_pu8 );
#endif
#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON)
    Std_ReturnType (*EthZeroCopyTransmit_pfct) ( uint8 CtrlIdx_u8,         /* ZeroCopyTransmit function pointer */
                                                 const Eth_ZeroCopyArg_tst * Eth_ZeroCopyArg_pcst,
                                                 uint16 * DescriptorIdPtr );

    void (*EthZeroCopyMarkLastTx_pfct) ( uint8 CtrlIdx_u8,                 /* ZeroCopyMarkLastTx function pointer */
                                         uint16 * DescriptorIdPtr );
#endif
} EthIf_EthDriverApiConfig_tst;

/* Dynamic PhysCtrl table structure of EthIf which maintains the state of each EthCtrl */
typedef struct
{
    const EthIf_EthDriverApiConfig_tst * const RefEthDrvAPITablePtr_cpcst;  /* Pointer to a structure storing all the related EthDriver APIs */
    EthIf_CtrlState_ten CtrlState_en;                                       /* EthCtrl state in the context of EthIf */
    const uint8 EthCtrlIdx_cu8;                                             /* EthCtrl index in the context of Ethernet Controller */
    uint8 ActiveCntr_u8;                                                    /* Active counter which reflect the number of EthIfCtrl requesting the EthCtrl to be active */
    const boolean RxInterruptEnabled_cb;                                    /* Specify whether Rx interrupt is enabled for each physical controller */
    const boolean TxInterruptEnabled_cb;                                    /* Specify whether Tx interrupt is enabled for each physical controller */
} EthIf_DynamicPhysCtrl_tst;

/* Dynamic Transceiver table structure of EthIf which maintains the state of each EthTrcv */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
typedef struct
{
#if( ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON )
    EthIf_SignalQualityMeasurement_tst SignalQualityMeasurement_st;      /* Stores the signal quality for the current transceiver */
#endif
    EthIf_TrcvState_ten TrcvState_en;                   /* Transceiver state in the context of EthIf */
    EthIf_EthIfLinkState_ten TrcvLinkState_en;          /* Link state of the transceiver  */
    const uint8 EthTrcvIdx_cu8;                         /* Trcv index in the context of Ethernet Transceiver */
    uint8 ActiveCntr_u8;                                /* Active counter which reflect the number of EthIfCtrl requesting the Transceiver to be active */
    boolean TrcvLinkStateActiveReq_b;                   /* Indicates whether API call EthTrcv_TransceiverLinkStateRequest(ACTIVE) was successful */
} EthIf_DynamicTrcv_tst;
#endif

/* Static Switch table structure of EthIf which stores static configuration of each Swt like the host port index or the list of its standard ports */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
typedef struct
{
    const uint8 * const RefHostUplinkPortsTablePtr_cpacu8;  /* Pointer to a table storing DynamicSwtPort structure indexes of the host and uplink ports */
    const uint8 * const RefStdPortsTablePtr_cpacu8;         /* Pointer to a table storing DynamicSwtPort structure indexes of the standard ports */
    const uint8 EthSwtIdx_cu8;                              /* EthSwt index in the context of Ethernet Switch */
    const uint8 RefPortsTableOffset_cu8;                    /* Offset in the DynamicSwtPort table structure where the list of ports of the switch begins */
    const uint8 NbHostUplinkPorts_cu8;                      /* Total number of host port and uplink ports in the Switch */
    const uint8 NbStdPorts_cu8;                             /* Number of standard ports of the Switch */
    const uint8 NbPorts_cu8;                                /* Total number of ports of the Switch (counting also the unused ports) */
} EthIf_StaticSwitch_tst;
#endif

/* Dynamic Switch table structure of EthIf which maintains the state of each EthSwt */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
typedef struct
{
    EthIf_SwtState_ten SwtState_en;                     /* EthSwt state in the context of EthIf */
} EthIf_DynamicSwitch_tst;
#endif

/* Dynamic Switch Port table structure of EthIf which maintains the state of each port of each EthSwt */
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
typedef struct
{
#if (ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON)
    EthIf_SignalQualityMeasurement_tst SignalQualityMeasurement_st;     /* Stores the signal quality for the current switch port */
#endif
    EthIf_SwtPortState_ten PortState_en;                                /* State of the port in the context of EthIf */
    EthIf_EthIfLinkState_ten PortLinkState_en;                          /* Link state of the port */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    uint32 PortOffDelay_u32;                                            /* Timer to switch off a port after a request for DOWN is received */
#endif
    const uint8 RefSwitchTableIdx_cu8;                                  /* Index of the referenced Static Switch table */
    const uint8 EthSwtPortIdx_cu8;                                      /* EthSwt port index in the context of Ethernet Switch */
    uint8 ActiveCntr_u8;                                                /* Indicates in how many EthIf Switch Port Groups the port is active */
} EthIf_DynamicSwtPort_tst;
#endif

/* Dynamic Standalone Switch PG table structure of EthIf. Maintains stand-alone switch port group state, link and requested mode information. Port group (PG) is stand-alone, if it is not referenced by any EthIfCtrl. */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
typedef struct
{
    EthIf_StandaloneSwtPortGroupMode_ten Mode_en;                        /* Mode of the Standalone Port Group in the context of EthIf. This is used during state machine processing. */
    EthTrcv_ModeType ReqMode_en;                                         /* Requested mode of the Standalone Port Group */
    EthIf_EthIfLinkState_ten LinkState_en;                               /* Link State of the Standalone Port Group */
} EthIf_DynamicStandaloneSwtPortGroup_tst;
#endif

/* Static Switch PG table structure of EthIf which stores static configuration of each PG like the host port index or the list of its standard ports */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
typedef struct
{
    const uint8 * const RefHostUplinkPortsTablePtr_cpacu8;  /* Pointer to a table storing DynamicSwtPort structure indexes of the host and uplink ports. */
    const uint8 * const RefStdPortsTablePtr_cpacu8;         /* Pointer to a table storing DynamicSwtPort structure indexes of the standard ports. */
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    EthIf_DynamicStandaloneSwtPortGroup_tst * const RefStandaloneSwtPortGroupTableEntry_cpst;   /* If the semantic of the Switch Port Group is unreferenced, this is a pointer to
                                                                                                 * the related Dynamic Standalone Switch Port Group structure, otherwise NULL_PTR. */
#endif
    const EthIf_SwtPortGroupSemantic_ten Semantic_cen;      /* Semantic of the Switch Port Group. It is either link-information, control or unreferenced */
    const uint8 NbHostUplinkPorts_cu8;                      /* Number of host and uplink ports in the switch port group */
    const uint8 NbStdPorts_cu8;                             /* Number of standard ports which are part of the Switch Port Group */
} EthIf_StaticSwtPortGroup_tst;
#endif

/* [SWS_EthIf_00149] Main configuration structure of EthIf */
typedef struct
{
    const EthIf_General_tst * const EthIf_GeneralTable_cpcst;                         /* Pointer to the General table structure of EthIf */
    const EthIf_FrameOwnerConfig_tst * const EthIf_FrameOwnerConfigTable_cpcst;       /* Pointer to the FrameOwner table structure of EthIf */
    const EthIf_StaticEthIfCtrl_tst * const EthIf_StaticEthIfCtrlTable_cpcst;         /* Pointer to the Static EthIfCtrl table structure of EthIf */
    EthIf_DynamicEthIfCtrl_tst * const EthIf_DynamicEthIfCtrlTable_cpst;              /* Pointer to the Dynamic EthIfCtrl table structure of EthIf */
    EthIf_DynamicPhysCtrl_tst * const EthIf_DynamicPhysCtrlTable_cpst;                /* Pointer to the Dynamic PhysCtrl table structure of EthIf */
#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
    EthIf_DynamicTrcv_tst * const EthIf_DynamicTrcvTable_cpst;                        /* Pointer to the Dynamic Transceiver table structure of EthIf */
#endif
#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
    const EthIf_StaticSwitch_tst * const EthIf_StaticSwitchTable_cpcst;               /* Pointer to the Static Switch table structure of EthIf */
    EthIf_DynamicSwitch_tst * const EthIf_DynamicSwitchTable_cpst;                    /* Pointer to the Dynamic Switch table structure of EthIf */
    EthIf_DynamicSwtPort_tst * const EthIf_DynamicSwtPortTable_cpst;                  /* Pointer to the Dynamic Switch Port table structure of EthIf */
#if( ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    const EthIf_StaticSwtPortGroup_tst * const EthIf_StaticSwtPortGroupTable_cpcst;   /* Pointer to the Static Switch PG table structure of EthIf */
#endif
#endif
} EthIf_ConfigType;


#endif /* ETHIF_TYPES_H */
