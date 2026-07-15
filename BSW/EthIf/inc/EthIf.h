

#ifndef ETHIF_H
#define ETHIF_H

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#include "EthIf_Types.h"
#include "EthIf_PbCfg.h"

#ifdef ETHIF_CONFIGURED

/*
***************************************************************************************************
* Defines
***************************************************************************************************
*/

/* Vendor Id */
#define ETHIF_VENDOR_ID                     6U

/* EthIf module Id */
#define ETHIF_MODULE_ID                     65U

/* EthIf instance Id */
#define ETHIF_INSTANCE_ID                   0x0U

/* Software Version Info */
#define ETHIF_SW_MAJOR_VERSION              10U
#define ETHIF_SW_MINOR_VERSION              0U
#define ETHIF_SW_PATCH_VERSION              0U

/* AUTOSAR Version Info */
#define ETHIF_AR_RELEASE_MAJOR_VERSION      4
#define ETHIF_AR_RELEASE_MINOR_VERSION      2
#define ETHIF_AR_RELEASE_REVISION_VERSION   2

/*  Frame types */
#define ETHIF_FRAMETYP_ARP                  0x0806U
#define ETHIF_FRAMETYP_IPv4                 0x0800U
#define ETHIF_FRAMETYP_IPv6                 0x86DDU
#define ETHIF_FRAMETYP_VLAN                 0x8100U
#define ETHIF_FRAMETYP_AVB                  0x88B5U     /* Audio Video Bridge Protocol type */
#define ETHIF_FRAMETYP_PTP                  0x88F7U     /* Precision Time Protocol type */

/* Measurement data indexes. */
#define ETHIF_MEAS_DROP_CRTLIDX             0x01U         /* Measurement index of dropped datagrams caused by invalid CtrlIdx/VLAN.  */
#define ETHIF_MEAS_DROP_ETHERTYPE           0x80U         /* Vendor specific range : Measurement index of dropped datagrams caused by invalid EtherType. */
#define ETHIF_MEAS_ALL                      0xFFU         /* represents all measurement indexes. */

#ifndef ETHIF_HWADDR_LEN
#define ETHIF_HWADDR_LEN                    6U
#endif


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"
/* ETHIF024: Initializes the Ethernet Interface */
void EthIf_Init                          ( const EthIf_ConfigType * CfgPtr );

/* ETHIF034: Enables / disables the indexed controller */
Std_ReturnType EthIf_SetControllerMode   ( uint8 CtrlIdx,
                                           Eth_ModeType CtrlMode );

#if( ETHIF_RMU_SUPPORT == STD_ON )
/* RB SPECIFIC: Initialize the indexed Eth controller (EthIfPhysController) */
Std_ReturnType EthIf_PhysCtrlInit        ( uint8 PhysCtrlIdx );

/* RB SPECIFIC: Enables / disables the indexed Eth controller (EthIfPhysController) */
Std_ReturnType EthIf_SetPhysCtrlMode     ( uint8 PhysCtrlIdx,
                                           Eth_ModeType CtrlMode );
#endif

/* ETHIF039: Obtains the state of the indexed controller */
Std_ReturnType EthIf_GetControllerMode   ( uint8 CtrlIdx,
                                           Eth_ModeType * CtrlModePtr );

/* ETHIF061: Obtains the physical source address used by the indexed controller */
void EthIf_GetPhysAddr                   ( uint8 CtrlIdx,
                                           uint8 * PhysAddrPtr );

/* ETHIF00132: Sets the physical source address used by the indexed controller */
Std_ReturnType EthIf_SetPhysAddr         ( uint8 CtrlIdx,
                                           const uint8* PhysAddrPtr );

/* ETHIF067: Provides access to a transmit buffer of the specified Ethernet controller */
BufReq_ReturnType EthIf_ProvideTxBuffer  ( uint8 CtrlIdx,
                                           Eth_FrameType FrameType,
                                           uint8 Priority,
                                           uint8 * BufIdxPtr,
                                           uint8 ** BufPtr,
                                           uint16 * LenBytePtr );

/* ETHIF075: Triggers transmission of a previously filled transmit buffer */
Std_ReturnType EthIf_Transmit            ( uint8 CtrlIdx,
                                           uint8 BufIdx,
                                           Eth_FrameType FrameType,
                                           boolean TxConfirmation,
                                           uint16 LenByte,
                                           uint8 * PhysAddrPtr );

#if( ETHIF_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
/* ETHIF154: Returns a time value out of the HW registers according to the capability of the HW */
Std_ReturnType EthIf_GetCurrentTime      ( uint8 CtrlIdx,
                                           Eth_TimeStampQualType * timeQualPtr,
                                           Eth_TimeStampType * timeStampPtr );

/* ETHIF160: Activates egress time stamping on a dedicated message object */
void EthIf_EnableEgressTimeStamp         ( uint8 CtrlIdx,
                                           uint8 BufIdx  );

/* ETHIF166: Reads back the egress time stamp on a dedicated message object */
void EthIf_GetEgressTimeStamp            ( uint8 CtrlIdx,
                                           uint8 BufIdx,
                                           Eth_TimeStampQualType * timeQualPtr,
                                           Eth_TimeStampType * timeStampPtr );

/* ETHIF172: Reads back the ingress time stamp on a dedicated message object */
void EthIf_GetIngressTimeStamp           ( uint8 CtrlIdx,
                                           Eth_DataType * DataPtr,
                                           Eth_TimeStampQualType * timeQualPtr,
                                           Eth_TimeStampType * timeStampPtr );
#endif

#if( ETHIF_SWITCH_ENABLE_GLOBAL_TIME_SUPPORT == STD_ON )
/* AR4.4: Request the MgmtObject (in this context) for the corresponding switch port. */
Std_ReturnType EthIf_GetRxMgmtObject     ( uint8 CtrlIdx,
                                           uint8 SwitchPortIdx,
                                           EthSwt_MgmtObjectType** MgmtObjectPtr );

/* AR4.4: Request the MgmtObject (in this context) for the corresponding switch port */
Std_ReturnType EthIf_GetTxMgmtObject     ( uint8 CtrlIdx,
                                           uint8 SwitchPortIdx,
                                           EthSwt_MgmtObjectType** MgmtObjectPtr );
#endif

#if(ETHIF_VERSIONINFO_API == STD_ON)
/* ETHIF082: Returns the version information of this module */
void EthIf_GetVersionInfo                ( Std_VersionInfoType * VersionInfoPtr );
#endif

/* ETHIF139: Update the physical source address to/from the indexed controller filter */
Std_ReturnType EthIf_UpdatePhysAddrFilter          ( uint8 CtrlIdx,
                                                     const uint8 * PhysAddrPtr,
                                                     Eth_FilterActionType Action );

/* ETHIF91102: Request a mode for the EthIfSwtPortGroup */
Std_ReturnType EthIf_SwitchPortGroupRequestMode    ( EthIf_SwitchPortGroupIdxType PortGroupIdx,
                                                     EthTrcv_ModeType PortMode );

/* ETHIF91103: Request to start all configured ports */
Std_ReturnType EthIf_StartAllPorts                 ( void );

#if( ETHIF_ENABLE_ETHIF_GET_VLANID == STD_ON )
/* ETHIF91052: Returns the VLAN identifier of the requested Ethernet controller  */
Std_ReturnType EthIf_GetVlanId                     ( uint8 CtrlIdx,
                                                     uint16 * VlanIdPtr );
#endif

#if( ETHIF_ENABLE_OVERRIDE_SRCMAC_SUPPORT == STD_ON)
/* RB SPECIFIC: Overwrite the source MAC address of the frame to be transmitted */
Std_ReturnType EthIf_OverrideTxBufSrcAddr          ( uint8 CtrlIdx,
                                                     uint8 BufIdx,
                                                     const uint8 * PhysAddrPtr );
#endif

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/* ETHIF91011: Allows to read and reset detailed measurement data for diagnostic purposes. */
Std_ReturnType EthIf_GetAndResetMeasurementData    ( EthIf_MeasurementIdxType MeasurementIdx,
                                                     boolean MeasurementResetNeeded,
                                                     uint32 * MeasurementDataPtr );
#endif

#if( ETHIF_ETHIFTRCV_EXIST == STD_ON )
/* ETHIF91016: Set a test mode for the requested  Ethernet transceiver */
Std_ReturnType EthIf_SetPhyTestMode                ( uint8 TrcvIdx,
                                                     EthTrcv_PhyTestModeType Mode );

/* ETHIF91005: Set a transmission mode for the requested Ethernet transceiver */
Std_ReturnType EthIf_SetPhyTxMode                  ( uint8 TrcvIdx,
                                                     EthTrcv_PhyTxModeType Mode );

/* ETHIF91018: Set a loop back mode for the requested Ethernet transceiver  */
Std_ReturnType EthIf_SetPhyLoopbackMode            ( uint8 TrcvIdx,
                                                     EthTrcv_PhyLoopbackModeType Mode );

/* ETHIF91014: Obtain the diagnostic result for the requested Ethernet transceiver */
Std_ReturnType EthIf_GetCableDiagnosticsResult     ( uint8 TrcvIdx,
                                                     EthTrcv_CableDiagResultType * ResultPtr );

/* ETHIF91020: Obtain the physical idenifier for the requested Ethernet transceiver  */
Std_ReturnType EthIf_GetPhyIdentifier              ( uint8 TrcvIdx,
                                                     uint32 * OrgUniqueIdPtr,
                                                     uint8 * ModelNrPtr,
                                                     uint8 * RevisionNrPtr );

/* ETHIF91056: Retrieves the signal quality of the link of the given Ethernet transceiver */
Std_ReturnType EthIf_GetTrcvSignalQuality          ( uint8 TrcvIdx,
                                                     EthIf_SignalQualityResultType * ResultPtr );

/* ETHIF91059: Clear the stored signal quality of the link of the given Ethernet transceiver */
Std_ReturnType EthIf_ClearTrcvSignalQuality        ( uint8 TrcvIdx );

/* ETHIF91108: Retrieves the Mode of the indexed transceiver */
Std_ReturnType EthIf_GetTransceiverMode( uint8 TrcvIdx,
                                            EthTrcv_ModeType* TrcvModePtr );

/* ETHIF91110: Retrieves the Link State of the indexed transceiver */
Std_ReturnType EthIf_TransceiverGetLinkState( uint8 TrcvIdx,
                                                EthTrcv_LinkStateType* LinkStatePtr );

/* ETHIF91112: Retrieves the Baud Rate of the indexed transceiver */
Std_ReturnType EthIf_TransceiverGetBaudRate( uint8 TrcvIdx,
                                                EthTrcv_BaudRateType* BaudRatePtr );

/* ETHIF91114: Retrieves the duplex mode of the indexed transceiver */
Std_ReturnType EthIf_TransceiverGetDuplexMode( uint8 TrcvIdx,
                                                EthTrcv_DuplexModeType* DuplexModePtr );

/* ETHIF91132: Trigger the cable diagnostics for the given Ethernet transceiver */
Std_ReturnType EthIf_RunCableDiagnostic( uint8 TrcvIdx );
#endif

#if( ETHIF_ETHIFSWITCH_EXIST == STD_ON )

/* ETHIF91059: Retrieves the signal quality of the link of the given Ethernet switch port */
Std_ReturnType EthIf_GetSwitchPortSignalQuality         ( uint8 SwitchIdx,
                                                         uint8 SwitchPortIdx,
                                                         EthIf_SignalQualityResultType * ResultPtr );

/* ETHIF91060: Clear the stored signal quality of the link of the given Ethernet switch port */
Std_ReturnType EthIf_ClearSwitchPortSignalQuality       ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx );

/* ETHIF00190: Obtains the port over which this MAC-address can be reached */
Std_ReturnType EthIf_GetPortMacAddr                     ( const uint8* MacAddrPtr,
                                                          uint8* SwitchIdxPtr,
                                                          uint8* PortIdxPtr );

/* ETHIF00196: Obtains the address resolution table of a switch and copies the list into a user provided buffer.
 * The function will copy all or numberOfElements into the output list. If input value of numberOfElements is 0 the
 * function will not copy any data but only return the number of valid entries in the cache. arlTableListPointer may
 * be NULL_PTR in this case. */
Std_ReturnType EthIf_GetArlTable                        ( uint8 switchIdx,
                                                          uint16* numberOfElements,
                                                          Eth_MacVlanType* arlTableListPointer );

/* ETHIF91107: Obtains the mode of the indexed switch port */
Std_ReturnType EthIf_GetSwitchPortMode                  ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          EthTrcv_ModeType* PortModePtr );

/* ETHIF91109: Obtains the link state of the indexed switch port */
Std_ReturnType EthIf_SwitchPortGetLinkState             ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          EthTrcv_LinkStateType* LinkStatePtr );

/* ETHIF91111: Obtains the baud rate of the indexed switch port */
Std_ReturnType EthIf_SwitchPortGetBaudRate              ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          EthTrcv_BaudRateType* BaudRatePtr );

/* ETHIF91113: Obtains the duplex mode of the indexed switch port */
Std_ReturnType EthIf_SwitchPortGetDuplexMode            ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          EthTrcv_DuplexModeType* DuplexModePtr );

/* ETHIF91115: Reads a list with drop counter values of the corresponding port of the switch. The meaning of these
 * values is described at Eth_CounterType. */
Std_ReturnType EthIf_SwitchPortGetCounterValues         ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          Eth_CounterType* CounterPtr );

/* ETHIF91116: Returns a list of statistic counters defined with Eth_RxTatsType. The majority of these Counters are
 * derived from the IETF RFC2819. */
Std_ReturnType EthIf_SwitchPortGetRxStats               ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          Eth_RxStatsType* RxStatsPtr );

/* ETHIF91117: Returns the list of Transmission Statistics out of IETF RFC1213 defined with Eth_TxStatsType, where the
 * maximal possible value shall denote an invalid value, e.g. this counter is not available. */
Std_ReturnType EthIf_SwitchPortGetTxStats               ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          Eth_TxStatsType* TxStatsPtr );

/* ETHIF91118: Returns the list of Transmission Error Counters out of IETF RFC1213 and RFC1643 defined with
 * Eth_TxErrorCounterValuesType, where the maximal possible value shall denote an invalid value, e.g. this counter
 * is not available. */
Std_ReturnType EthIf_SwitchPortGetTxErrorCounterValues  ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          Eth_TxErrorCounterValuesType* TxStatsPtr );

/* ETHIF91119: Returns the MAC learning mode, i.e. 1.) HW learning enabled, 2.) Hardware learning disabled, 3.)
 * Software learning enabled. Note: This feature is hardware dependent, i.e. the switch hardware needs to support the
 * different learning modes */
Std_ReturnType EthIf_SwitchPortGetMacLearningMode       ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          EthSwt_MacLearningType* MacLearningModePtr );

/* ETHIF91120: This function retrieves the OUI (24 bit) of the indexed Ethernet switch port. */
Std_ReturnType EthIf_GetSwitchPortIdentifier            ( uint8 SwitchIdx,
                                                          uint8 SwitchPortIdx,
                                                          uint32* OrgUniqueIdPtr,
                                                          uint8* ModelNrPtr,
                                                          uint8* RevisionNrPtr );

/* ETHIF91121: Obtain the Organizationally Unique Identifier that is given by the IEEE of the indexed Ethernet switch.
 * This function shall provide the OUI of Ethernet switch. The OUI has a size of 24 bit. If a ethernet switch can
 * provide the OUI the 8 most significant bits of the OUI shall be set to 0x00xxxxxx. If a Ethernet switch can not
 * provide the OUI the 8 most significant bits of the OUI shall be set to 0xFFxxxxxx. */
Std_ReturnType EthIf_GetSwitchIdentifier                ( uint8 SwitchIdx,
                                                          uint32* OrgUniqueIdPtr );

/* ETHIF91122: Store the given port mirror configuration in a shadow buffer in the Ethernet switch driver for the given
 * MirroredSwitchIdx. */
Std_ReturnType EthIf_WritePortMirrorConfiguration       ( uint8 MirroredSwitchIdx,
                                                          const EthSwt_PortMirrorCfgType* PortMirrorConfigurationPtr );

/* ETHIF91123: Obtain the port mirror configuration of the given Ethernet switch. */
Std_ReturnType EthIf_ReadPortMirrorConfiguration        ( uint8 MirroredSwitchIdx,
                                                          EthSwt_PortMirrorCfgType* PortMirrorConfigurationPtr );

/* ETHIF91124: Delete the stored port mirror configuration of the given MirroredSwitchIdx. If no port mirror
 * configuration was found for the given MirroredSwitchIdx, the return value shall be E_OK. */
Std_ReturnType EthIf_DeletePortMirrorConfiguration      ( uint8 MirroredSwitchIdx );

/* ETHIF91125: Obtain the current status of the port mirroring for the indexed Ethernet switch port */
Std_ReturnType EthIf_GetPortMirrorState                 ( uint8 SwitchIdx,
                                                          EthSwt_PortMirrorStateType* PortMirrorStatePtr );

/* ETHIF91126: Request to set the given port mirroring state of the port mirror configuration for the given Ethernet
 * switch. */
Std_ReturnType EthIf_SetPortMirrorState                 ( uint8 MirroredSwitchIdx,
                                                          EthSwt_PortMirrorStateType PortMirrorState );

/* ETHIF91127: Activates a given test mode of the indexed Ethernet switch port. */
Std_ReturnType EthIf_SetPortTestMode                    ( uint8 SwitchIdx,
                                                          uint8 PortIdx,
                                                          EthTrcv_PhyTestModeType Mode );

/* ETHIF91128: Activates a given test loop-back mode of the indexed Ethernet switch port. */
Std_ReturnType EthIf_SetPortLoopbackMode                ( uint8 SwitchIdx,
                                                          uint8 PortIdx,
                                                          EthTrcv_PhyLoopbackModeType Mode );

/* ETHIF91129: Activates a given transmission mode of the indexed Ethernet switch port. */
Std_ReturnType EthIf_SetPortTxMode                      ( uint8 SwitchIdx,
                                                          uint8 PortIdx,
                                                          EthTrcv_PhyTxModeType Mode );

/* ETHIF91130: Retrieves the cable diagnostics result of the indexed Ethernet switch port respectively the referenced
 * Ethernet Transceiver Driver. */
Std_ReturnType EthIf_GetPortCableDiagnosticsResult      ( uint8 SwitchIdx,
                                                          uint8 PortIdx,
                                                          EthTrcv_CableDiagResultType* ResultPtr );

/* ETHIF91131: Trigger the cable diagnostics of the given Ethernet Switch port (PortIdx) by calling
 * EthTrcv_RunCableDiagnostic of the referenced Ethernet transceiver. */
Std_ReturnType EthIf_RunPortCableDiagnostic             ( uint8 SwitchIdx,
                                                          uint8 PortIdx );

/* ETHIF91133: Retrieves the data in memory of the indexed Ethernet switch in variable length. */
Std_ReturnType EthIf_SwitchGetCfgDataRaw                ( uint8 SwitchIdx,
                                                          uint32 Offset,
                                                          uint16 Length,
                                                          uint8* BufferPtr );

/* ETHIF91134: Retrieves the total size of data and the memory start address of the indexed Ethernet Switch. */
Std_ReturnType EthIf_SwitchGetCfgDataInfo               ( uint8 SwitchIdx,
                                                          uint32* DataSizePtr,
                                                          uint32* DataAdressPtr );

/* ETHIF91135: The function retrieves the maximum amount of allocated FIFO buffer of the indexed Ethernet switch egress
 * port. If the Ethernet switch hardware does not support Ethernet switch port based maximal FIFO buffer level, the
 * content of SwitchPortEgressFifoBufferLevelPtr shall be set to 0xFFFFFFFF. This API may be called by e.g. a CDD. */
Std_ReturnType EthIf_SwitchPortGetMaxFIFOBufferFillLevel ( uint8 SwitchIdx,
                                                           uint8 PortIdx,
                                                           uint8 SwitchPortEgressFifoIdx,
                                                           uint32* SwitchPortEgressFifoBufferLevelPtr );

#endif
#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"


#define ETHIF_START_SEC_CODE_FAST
#include "EthIf_MemMap.h"

#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON )
/* RB SPECIFIC: Prefill the ZeroCopy Reuse header with the Ethernet header fields */
Std_ReturnType EthIf_ZeroCopyFillHeader( uint8 CtrlIdx_u8,
                                         Eth_FrameType FrameType_u16,
                                         uint8 Priority_u8,
                                         const uint8 * PhysAddrPtr_pcu8,
                                         const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst );

/* RB SPECIFIC: Execute the ZeroCopy transmission */
Std_ReturnType EthIf_ZeroCopyTransmit( uint8 CtrlIdx_u8,
                                       Eth_ZeroCopyArg_tst * ZeroCopyArg_pst,
                                       uint32 * TxId_pu32 );

/* RB SPECIFIC: Mark the last executed ZeroCopy transmission with TxConf=TRUE */
void EthIf_ZeroCopyMarkLastTx( uint8 CtrlIdx_u8,
                               uint32 * TxId_pu32 );
#endif
#define ETHIF_STOP_SEC_CODE_FAST
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Scheduled
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/* If RTE is enabled, the main function declarations are present in SchM_EthIf.h. */
#if( ETHIF_ECUC_RB_RTE_IN_USE == STD_OFF )

/* ETHIF113: The function issues transmission confirmations in polling mode. It checks also for transceiver state changes */
void EthIf_MainFunctionTx                ( void );

/* ETHIF91104: The function is polling the link state of the used communication hardware (Ethernet transceiver, Ethernet switch ports) */
void EthIf_MainFunctionState             ( void );

#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */

#endif /* ETHIF_H */
