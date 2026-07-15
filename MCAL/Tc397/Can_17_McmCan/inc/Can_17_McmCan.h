/*******************************************************************************
**                                                                            **
**  Copyright (C) Infineon Technologies (2019)                                **
**                                                                            **
**  All rights reserved.                                                      **
**                                                                            **
**  This document contains proprietary information belonging to Infineon      **
**  Technologies. Passing on and copying of this document, and communication  **
**  of its contents is not permitted without prior written authorization.     **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME     : Can_17_McmCan.h                                            **
**                                                                            **
**  VERSION      : 1.40.0_9.0.0                                               **
**                                                                            **
**  DATE         : 2020-01-07                                                 **
**                                                                            **
**  VARIANT      : Variant PB                                                 **
**                                                                            **
**  PLATFORM     : Infineon AURIX2G                                           **
**                                                                            **
**  AUTHOR       : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**  VENDOR       : Infineon Technologies                                      **
**                                                                            **
**  TRACEABILITY : [cover parentID={BF45E37B-05DF-4a01-8B07-8FDEBA3DEE6E}]    **
**                 [cover parentID={DA6A0DC6-8157-42d9-8ABB-5D18C6B692F5}]    **
**                                                                            **
**  DESCRIPTION  : CAN Driver header definition file                          **
**                                                                            **
**  SPECIFICATION(S) : Specification of CAN Driver, AUTOSAR Release 4.2.2     **
**                                                                            **
**  MAY BE CHANGED BY USER : no                                               **
**                                                                            **
*******************************************************************************/

#ifndef CAN_17_MCMCAN_H
#define CAN_17_MCMCAN_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/* [cover parentID={6CE56ADD-37B9-4c96-BC08-609A206511DC}]
[/cover] */
/* [cover parentID={C0E673D5-B80C-40f5-9944-70221E106146}]
[/cover] */
/* Register definition file for Aurix2G target */
#include "IfxCan_reg.h"
/* Include Communication Stack Types */
#include "ComStack_Types.h"
/* Include the declarations of Can functions and types */
#include "Can_17_McmCan_Cfg.h"
/* [cover parentID={594CC1E2-D5F5-40fc-9916-76425DAA99DF}]
[/cover] */
/* Includes all types and constants that are shared among the
   AUTOSAR CAN modules Can, CanIf and CanTrcv */
#include "Can_GeneralTypes.h"
/* Inclusion of Mcal Specific Global Header File */
#include "McalLib.h"
#include "Mcal_Compiler.h"
#include "Can_17_McmCan_Externals.h"
/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/* Published parameters */

/* CAN Vendor ID - Vendor ID of the dedicated implementation of this
module according to the AUTOSAR vendor list */
#define CAN_17_MCMCAN_VENDOR_ID                  ((uint16)17)
/* CAN Module ID - Module ID of this module from Module List */
#define CAN_17_MCMCAN_MODULE_ID                  ((uint16)80)

#if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/* [cover parentID={06D1B973-8E2A-4923-970C-686093ABAFD2}]
[/cover] */
/* API Service called with wrong parameter: Config is a null pointer */
#define CAN_17_MCMCAN_E_PARAM_POINTER                      ((uint8)0x01)
/* [cover parentID={0BF785B4-8E12-43f7-8AF5-BCA49BFB8D87}]
[/cover] */
/* API called with wrong parameter: Handle parameter is not a
configured Hardware Transmit Handle */
#define CAN_17_MCMCAN_E_PARAM_HANDLE                       ((uint8)0x02)
/* [cover parentID={9ED8C975-CA94-41e9-9613-025FCBF17E96}]
[/cover] */
/* API called with wrong parameter : Parameter Data length is
more than 8 bytes */
#define CAN_17_MCMCAN_E_PARAM_DLC                          ((uint8)0x03)
/* [cover parentID={B81C2953-A515-4a48-9267-57D40F5FD116}]
[/cover] */
/* API called with wrong parameter : Parameter Controller is out of range */
#define CAN_17_MCMCAN_E_PARAM_CONTROLLER                   ((uint8)0x04)
/* [cover parentID={05598611-0879-4065-A5B4-4ED73DD973CE}]
[/cover] */
/* API Service used without initialization */
#define CAN_17_MCMCAN_E_UNINIT                             ((uint8)0x05)
/* [cover parentID={8D8DB9BD-EF6D-4906-BEF9-2043468C9472}]
[/cover] */
/* Invalid invalid transition requested OR Invalid CAN controller state for
API service call */
#define CAN_17_MCMCAN_E_TRANSITION                         ((uint8)0x06)
/* [cover parentID={4D86462A-BB81-4c29-B566-F1C306C59D08}]
[/cover] */
/* DET for Over write error in receive ISR */
#define CAN_17_MCMCAN_E_DATALOST                           ((uint8)0x07)
/* [cover parentID={68BF99F9-E0BD-4b2e-A430-ADE85607E95E}]
[/cover] */
/* API called with invalid / unsupported baudrate */
#define CAN_17_MCMCAN_E_PARAM_BAUDRATE                     ((uint8)0x08)
/* [cover parentID={AE8B3176-0F3B-4c32-9EF7-39AF041D75F5}]
[/cover] */
/* Invalid ICOM configuration Id */
#define CAN_17_MCMCAN_E_ICOM_CONFIG_INVALID                ((uint8)0x09)
/* [cover parentID={76F0A24E-BD8A-43f1-869F-3AD067317A48}]
[/cover] */
/* Invalid configuration set selection */
#define CAN_17_MCMCAN_E_INIT_FAILED                        ((uint8)0x0A)
/* [cover parentID={4B121357-CD4D-483b-B5D0-2655DF362A49}]
[/cover] */
/* API called with invalid value of Can Message Id */
#define CAN_17_MCMCAN_E_PARAM_MSGID                        ((uint8)0x0D)

#if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
/* Resource not configured to calling core */
#define CAN_17_MCMCAN_E_NOT_CONFIGURED                     ((uint8)0x64)
/* Core Initialization called when master initialization is not done */
#define CAN_17_MCMCAN_E_MASTER_CORE_UNINIT                 ((uint8)0x66)
/* [cover parentID={A28907C5-5A4A-4971-8442-1B9D7FE405D5}]
[/cover] */
/* Master De-initialization called before de-initialization of the slave cores*/
#define CAN_17_MCMCAN_E_SLAVE_CORE_INIT                    ((uint8)0x67)
#endif /* #if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */

#endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

/* Service IDs. Used while reporting development errors */
#define CAN_17_MCMCAN_SID_INIT                             ((uint8)0x00)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_SID_MAINFUNCTIONWRITE                ((uint8)0x01)
#define CAN_17_MCMCAN_SID_SETCONTROLLERMODE                ((uint8)0x03)
#define CAN_17_MCMCAN_SID_DISABLECONTROLLERINTERRUPTS      ((uint8)0x04)
#define CAN_17_MCMCAN_SID_ENABLECONTROLLERINTERRUPTS       ((uint8)0x05)
#define CAN_17_MCMCAN_SID_WRITE                            ((uint8)0x06)
#define CAN_17_MCMCAN_SID_GETVERSIONINFO                   ((uint8)0x07)
#define CAN_17_MCMCAN_SID_MAINFUNCTIONREAD                 ((uint8)0x08)
#define CAN_17_MCMCAN_SID_MAINFUNCTIONBUSOFF               ((uint8)0x09)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_SID_MAINFUNCTIONWAKEUP               ((uint8)0x0a)
#define CAN_17_MCMCAN_SID_CHECKBAUDRATE                    ((uint8)0x0e)
#define CAN_17_MCMCAN_SID_DEINIT                           ((uint8)0x65)
#define CAN_17_MCMCAN_SID_SETBAUDRATE                      ((uint8)0x0f)
#define CAN_17_MCMCAN_SID_SETICOMCONFIGURATION             ((uint8)0x10)
#define CAN_17_MCMCAN_SID_RECEIVEHANDLER                   ((uint8)0x12)
#define CAN_17_MCMCAN_SID_TRANSMITHANDLER                  ((uint8)0x66)

/* Diver used values */
#define CAN_17_MCMCAN_POLLING                               (uint8)0
#define CAN_17_MCMCAN_INTERRUPT                             (uint8)1

/* Identifier types of an L-PDU */
#define CAN_17_MCMCAN_ID_STANDARD                           (0x00)
#define CAN_17_MCMCAN_ID_MIXED                              (0x01)
#define CAN_17_MCMCAN_ID_EXTENDED                           (0x02)

/* Macros related to User mode implementation */
#define CAN_17_MCMCAN_MCAL_SUPERVISOR                 (0U)
#define CAN_17_MCMCAN_MCAL_USER1                      (1U)

/* Macros related to Kernels and Nodes for respective controller ID */

/* KERNEL0 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMKERNEL0_ID                             (0U)

/* KERNEL0 CONTROLLER 0 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER00_ID                        (0U)

/* KERNEL0 CONTROLLER 1 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER01_ID                        (1U)

/* KERNEL0 CONTROLLER 2 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER02_ID                        (2U)

/* KERNEL0 CONTROLLER 3 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER03_ID                        (3U)


/* KERNEL1 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMKERNEL1_ID                             (1U)

/* KERNEL1 CONTROLLER 0 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER10_ID                        (0U)

/* KERNEL1 CONTROLLER 1 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER11_ID                        (1U)

/* KERNEL1 CONTROLLER 2 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER12_ID                        (2U)

/* KERNEL1 CONTROLLER 3 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER13_ID                        (3U)


/* KERNEL2 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMKERNEL2_ID                             (2U)

/* KERNEL2 CONTROLLER 0 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER20_ID                        (0U)

/* KERNEL2 CONTROLLER 1 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER21_ID                        (1U)

/* KERNEL2 CONTROLLER 2 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER22_ID                        (2U)

/* KERNEL2 CONTROLLER 3 Configuration ID*/
#define CAN_17_MCMCAN_HWMCMCONTROLLER23_ID                        (3U)

/* Number of nodes per kernel */
#define CAN_17_MCMCAN_NOOF_NODES_PER_KERNEL                       (4U)

/* Number of RAM sections required for CAN messages configured */
#define CAN_17_MCMCAN_NOOF_RAM_SECTIONS_PER_CONTROLLER            (7U)

/* Number of memory Map details required */
#define CAN_17_MCMCAN_NOOF_MOMAP_PER_CONTROLLER                   (6U)

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* Size in bytes of the ICOM Signal */
#define CAN_17_MCMCAN_ICOM_DATA_SIGNAL_MATCH_SIZE                   (8U)
#endif
/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/
/* [cover parentID={6AFD7103-C809-4d90-9208-CE8AD32204F9}]
[/cover] */
#if (CAN_17_MCMCAN_LPDU_RX_CALLOUT == STD_ON)
typedef boolean (*Can_17_McmCan_LPduRxCalloutFnPtrType)(const Can_HwHandleType Hrh,
                                              const Can_IdType CanId,
                                              const uint8 CanDlc,
                                              const uint8 *CanSduPtr);
#endif
/* Data type used for Tx buffer Type */
/* [cover parentID={0ED85824-0D1F-4800-8D7F-86B8011FB519}]
[/cover] */
typedef enum
{
  CAN_17_MCMCAN_TX_DED_BUFFER = 0,
  CAN_17_MCMCAN_TX_QUEUE = 1
} Can_17_McmCan_TxBufferType;

/* Data type used for Rx buffer Type */
/* [cover parentID={53491133-A24D-4e10-AB69-3366EF45352A}]
[/cover] */
typedef enum
{
  CAN_17_MCMCAN_RX_DED_BUFFER = 0,
  CAN_17_MCMCAN_RX_FIFO0 = 1,
  CAN_17_MCMCAN_RX_FIFO1 = 2
} Can_17_McmCan_RxBufferType;

#if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* Data type used for Icom signal operator */
/* [cover parentID={D8E06510-0A6B-4cb4-A7E9-71C4BD331546}]
[/cover] */
typedef enum
{
  CAN_17_MCMCAN_ICOM_OPER_AND,
  CAN_17_MCMCAN_ICOM_OPER_EQUAL,
  CAN_17_MCMCAN_ICOM_OPER_GREATER,
  CAN_17_MCMCAN_ICOM_OPER_SMALLER,
  CAN_17_MCMCAN_ICOM_OPER_XOR
} Can_17_McmCan_IcomSignalOperType;
#endif

/* CAN Kernel Configuration data type */
/* [cover parentID={DD20A67C-F0E2-4934-9721-2D311834B16E}]
[/cover] */
typedef struct
{
  /* The Global Base address of Kernel module */
  Ifx_CAN* CanBaseAddress;
  /* The information of used controller configuration */
  boolean CanUsedHwCfgIndx [CAN_17_MCMCAN_NOOF_NODES_PER_KERNEL];
} Can_17_McmCan_McmModuleConfigType;

/* CAN Controller corresponding Message RAM Partition Configuration data type */
/* [cover parentID={904C450C-7C72-4eb5-9283-2E06BAEDB7AB}]
[/cover] */
typedef struct
{
  /* Start Address of Each Section with in the Message RAM */
  uint32 CanControllerMsgRAMMap [CAN_17_MCMCAN_NOOF_RAM_SECTIONS_PER_CONTROLLER];
  /* No of used Tx dedicated buffer */
  uint8 CanTxDedBuffCount;
  /* TxEvent FIFO Size */
  uint8 CanTxEvntFIFOSize;
  /* RXFIFO0 Size */
  uint8 CanRxFIFO0Size;
  /* RXFIFO0 Water Mark Level */
  uint8 CanRxFIFO0Threshold;
  /* RXFIFO1 Size */
  uint8 CanRxFIFO1Size;
  /* RXFIFO1 Water Mark Level */
  uint8 CanRxFIFO1Threshold;

  #if(CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  /* Tx Queue buffer Size */
  uint8 CanTxQueueSize;
  /* Tx Configuration support status */
  boolean CanTxQueueStatus;
  #endif
} Can_17_McmCan_ControllerMsgRAMConfigType;

/* CAN Controller Baudrate Configuration data type */
/* [cover parentID={786F3ACD-A455-433e-B1B0-CE400442AFF5}]
[/cover] */
typedef struct
{
  /* Nominal Baudrate settings value */
  uint32 CanControllerBaudrate;
  /* Baudrate value configured in Kbps */
  uint16 CanBaudrateCfg;
  #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* Supporting FD Baudrate configuration index value */
  uint16 CanFDIndex;
  /* Status of FD configuration support */
  boolean CanFdConfigEnabled;
  #endif
} Can_17_McmCan_ControllerBaudrateConfigType;

#if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
/* CAN Controller FDBaudrate Configuration data type */
/* [cover parentID={872F6083-3C47-4d09-A0DC-90217AAC5587}]
[/cover] */
typedef struct
{
  /* FD Baudrate settings value */
  uint32 CanControllerFDBaudrate;
  /* Transceiver Delay composition Register set value */
  uint32  CanTrcvDelyComp;
  /* BSR status value */
  boolean CanTxBRSEnable;
} Can_17_McmCan_ControllerFDBaudrateConfigType;
#endif

/* Data type for CAN Receive Hardware object configuration - Standard
Message filter. */
/* [cover parentID={1CF1C125-F8EC-46f8-BED2-4BCBBCAD0F18}]
[/cover] */
typedef struct
{
  /* Standard Filter mask value */
  uint32 CanSIDFiltEleS0;
  /* Hardware object Id */
  Can_HwHandleType CanSidHwObjId;
  /* configuration id of Controller to which this hardware object is assigned */
  uint8 HwControllerId;
  /* The filter settings for which type of Rx Buffer */
  Can_17_McmCan_RxBufferType CanSidBufferType;

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  /* The Normal message ID is supported for ICOM support or not */
  boolean CanSidPNSupport;
  #endif

} Can_17_McmCan_SIDFilterConfigType;

/* Data type for CAN Receive Hardware object configuration - Extended
Message filter. */
/* [cover parentID={B4C93844-14D8-4721-9581-7D2972983138}]
[/cover] */
typedef struct
{
  /* Extended Filter mask value0 */
  uint32 CanXIDFiltEleF0;
  /* Extended Filter mask value1 */
  uint32 CanXIDFiltEleF1;
  /* Hardware object Id */
  Can_HwHandleType CanXidHwObjId;
  /* configuration id of Controller to which this hardware object is assigned */
  uint8  HwControllerId;
  /* The filter settings for which type of Rx Buffer */
  Can_17_McmCan_RxBufferType CanXidBufferType;

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  /* The Normal message ID is supported for ICOM support or not */
  boolean CanXidPNSupport;
  #endif

} Can_17_McmCan_XIDFilterConfigType;

/* Data type for CAN Transmit Hardware object configuration */
/* [cover parentID={DAFD4CB9-E6F7-40f6-9318-C911A238160E}]
[/cover] */
typedef struct
{
  /* Hardware object Id */
  Can_HwHandleType CanTxHwObjId;
  /* Tx Buffer index number */
  uint8 CanTxBuffIndx;
  /* configuration id of Controller to which this hardware object is assigned */
  uint8 HwControllerId;

  #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* The CAN FD padding Value */
  uint8 CanFdPaddValue;
  #endif

  /* Hardware object CanIdType */
  uint8 CanTxHwObjIdType;
  /* The buffer types used by the Hardware handler */
  Can_17_McmCan_TxBufferType CanTxBufferType;
  #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
  /* CAN Trigger transmit status */
  boolean CanTrigTxStatus;
  #endif /* #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON) */
} Can_17_McmCan_TxHwObjectConfigType;

/* CAN controller Events configuration type */
/* [cover parentID={9AAFE49D-EAB0-4d78-80AC-78CE6B460D45}]
[/cover] */
typedef struct
{
  uint8 CanEventType[CAN_17_MCMCAN_NOOF_NODES_PER_KERNEL];
} Can_17_McmCan_EventHandlingType;

#if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)
/* Multiple periods Transmit Hw object configuration data type */
/* [cover parentID={F936548E-C002-4ad1-90DB-4583760DBFB7}]
[/cover] */
typedef struct
{
  /* Tx buffer mask value for Tx completion status check*/
  uint32 CanTxBufferMaskvalue;
  /* Hardware object Id */
  Can_HwHandleType CanPerHthHwObjId;
  /* configuration id of Controller to which this hardware object is assigned */
  uint8 HwControllerId;
} Can_17_McmCan_HthMaskObjectConfigType;

/* Multiple period Tx configuration index data type */
/* [cover parentID={6B35775E-DDC6-418a-ADCF-A21C0247C00D}]
[/cover] */
typedef struct
{
  /* Start Index of Hth associate with in the Period */
  Can_HwHandleType CanPerHthStartIndx;
  /* No of Hth associated with in the Period */
  Can_HwHandleType CanPerHthEndIndx;
} Can_17_McmCan_PeriodHthMaskConfigType;
#endif

#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)
/* Multiple periods Receive Hw object configuration data type */
/* [cover parentID={E2A8A79F-B873-4512-9C62-A36E98002A89}]
[/cover] */
typedef struct
{
  /* Rx buffer mask value for receive status check (0- 31)*/
  uint32 CanPerRxbufferMaskvalue0;
  /* Rx buffer mask value for receive status check(32-63)*/
  uint32 CanPerRxbufferMaskvalue1;
  /* Hardware object Id */
  Can_HwHandleType CanPerHrhHwObjId;
  /* configuration id of Controller to which this hardware object is assigned */
  uint8 HwControllerId;
  /* The Rx buffer Type used by Hrh */
  Can_17_McmCan_RxBufferType CanPerHrhBufferType;
} Can_17_McmCan_HrhMaskObjectConfigType;

/* Multiple period Tx configuration index data type */
/* [cover parentID={36257A28-9486-4628-8D7B-9A84E3B7FEC2}]
[/cover] */
typedef struct
{
  /* Start Index of Hrh associate with in the Period */
  Can_HwHandleType CanPerHrhStartIndx;
  /* No of Hrh associated with in the Period */
  Can_HwHandleType CanPerHrhEndIndx;
} Can_17_McmCan_PeriodHrhMaskConfigType;

#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* ICOM configuration data type */
/* [cover parentID={E4EF3F9B-B129-4fef-826E-9C2E55B3B076}]
[/cover] */
typedef struct
{
  /* ICOM Message configuration start index */
  uint16 CanIcomFirstMsgIndx;
  /* No of ICOM Message configuration */
  uint16 CanIcomNoOfMsgIndx;
  /* Bus off is configured as wakeup source */
  boolean CanIcomWakeOnBusOff;
} Can_17_McmCan_IcomConfigType;

/* ICOM message configuration data type */
/* [cover parentID={061E0C55-9E90-45d9-BA6F-7C4A483912F9}]
[/cover] */
typedef struct
{
  /* Icom Message ID */
  Can_IdType CanIcomMsgId;
  /* Icom Message Mask value */
  Can_IdType CanIcomMaskRef;
  /* Icom Message Counter value */
  uint16 CanIcomCntrVal;
  /* Icom Message DLC value */
  uint8  CanIcomDLC;
  /* Icom Message Signal configuration Index value */
  uint8  CanIcomFirstSignalIndx;
  /* No of Message Signal configuration */
  uint8  CanIcomNoOfSignalIndx;
  /* DLC error configured as wakeup source or not */
  boolean CanIcomLengthErr;
} Can_17_McmCan_IcomRxMsgConfigType;

/*ICOM message signal configuration data type */
/* [cover parentID={3F43B820-59ED-4cc0-A9FD-5B48323812B8}]
[/cover] */
typedef struct
{
  /* Combination of Signal Mask value */
  uint8 CanIcomSignalMask[CAN_17_MCMCAN_ICOM_DATA_SIGNAL_MATCH_SIZE];
  /* Combination of Signal value */
  uint8 CanIcomSignalValue[CAN_17_MCMCAN_ICOM_DATA_SIGNAL_MATCH_SIZE];
  /* Operation selected for the Signal validation */
  Can_17_McmCan_IcomSignalOperType CanIcomSignalOper;
} Can_17_McmCan_IcomRxMsgSignalConfigType;

#endif

/* Data type for CAN Controller configuration */
/* [cover parentID={5BB9EBE1-AEF3-47fa-ACE5-9B0E9E7AAD7A}]
[/cover] */
typedef struct
{
  /* Can controller Base Node address */
  Ifx_CAN_N* CanNodeAddress;
  /* combination of Loopback and receive input pin selection setting */
  uint32 CanNPCRValue;
  /* The controller Hw object configuration mapping information with
  start address and index - Hth, SID and XID */
  uint16 CanControllerMOMap[CAN_17_MCMCAN_NOOF_MOMAP_PER_CONTROLLER];
  /* Default baudrate configuration Index */
  uint16 CanDefaultBRCfgIndx;
  /* Start index value of Baudrate configuration */
  uint16 CanBaudrateCfgIndx;
  /* Total no of Baudrate configuration */
  uint16 CanNoOfBaudrateCfg;
  /* The controller Associated Kernel configuration Index */
  uint8  CanKernelHwId;
  /* The CAN controller Hw Index */
  uint8  CanControllerHwId;
  /* The CAN controller Logical Hw Index - Controller ID defined by user */
  uint8  CanControllerLogicalId;

  #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* FD support status of the controller */
  boolean  CanFDSupport;
  #endif

} Can_17_McmCan_ControllerConfigType;

/* Indexing with respect to Logical controoler ID */
/* [cover parentID={1CCC1E90-1CEF-4b30-89BE-79245FB342F9}]
[/cover] */
typedef uint8 Can_17_McmCan_ControllerIndexType;

/* Indexing with multiplexed periods to transmit periods */
/* [cover parentID={4F506C86-8FE2-4f15-8A1B-E19B1ABD3803}]
[/cover] */
typedef uint8 Can_17_McmCan_HthPeriodIndexType;

/* Indexing with multiplexed periods to recieve periods */
/* [cover parentID={EFE82982-6AFB-46cc-A10B-64026D7D1D89}]
[/cover] */
typedef uint8 Can_17_McmCan_HrhPeriodIndexType;

/* Structure Holding the mapping details of the Configured Hth to
  core specific Hth index, Logical controller ID and the Core it is assigned to.
  The index is not considering the Hrh in the hoh objects.   */
/* [cover parentID={6166196A-356A-40f8-87D0-F85ACDC795F5}]
[/cover] */
typedef struct
{
  /* Core that the controller is assigned to */
  uint8 CanHthCoreAssigned;
  /* Logical controller indexing */
  uint8 CanHthLogicContIndex;
  /* Core specific Hth Index */
  uint16 CanHthCoreSpecIndex;
} Can_17_McmCan_HthIndexType;

/* Structure Holding the mapping details of the logical controller index to
  core specific controller index and the physical controller index */
/* [cover parentID={9E01DBC8-F0F8-457c-997E-28AB12738B40}]
[/cover] */
typedef struct
{
  /* Core that the controller is assigned to */
  uint8 CanLCoreAssigned;
  /* Indexing with respect to core specific array */
  uint8 CanLCoreSpecContIndex;
  /* Node index (0-3) */
  uint8 CanLContPhyIndex;
  /* Kernel Index (0-2) */
  uint8 CanLKerPhyIndex;
} Can_17_McmCan_LogicalControllerIndexType;

/* Structure Holding the mapping details of the physical controller index to
  core specific controller index and the logical controller index */
/* Physical controllers not used shall be stubbed with the default value of
  255 */
/* [cover parentID={0ABF4167-D2A2-40e0-9C69-C7E3005A346C}]
[/cover] */
typedef struct
{
  /* Logical controller indexing */
  uint8 CanPLogicContIndex;
  /* Indexing with respect to core specific array */
  uint8 CanPCoreSpecContIndex;
  /* Core that the controller is assigned to */
  uint8 CanPCoreAssigned;
} Can_17_McmCan_PhyControllerIndexType;

/* Type of CAN external data structure containing the overall initialization
data for the CAN Driver and SFR settings affecting all controllers assigned
per core */
/* [cover parentID={BA524CBD-8D81-4326-B419-A75B9496697F}]
[/cover] */
typedef struct
{
  /* Number of controllers configured in the core */
  const uint8 CanCoreContCnt;
  /* Array of all the controllers configured in the core */
  const Can_17_McmCan_ControllerIndexType* CanControllerIndexingPtr;
  /* Pointer to CAN Controller configuration */
  const Can_17_McmCan_ControllerConfigType* CanControllerConfigPtr;
  /* Pointer to CAN controller corresponding Message RAM Mapping
  configuration */
  const Can_17_McmCan_ControllerMsgRAMConfigType* \
  CanControllerMsgRAMMapConfigPtr;
  /* Pointer to CAN Controller Handling of Events : Interrupt/Polling */
  const Can_17_McmCan_EventHandlingType* CanEventHandlingConfigPtr;
  /* Pointer to Controller Baudrate Configuration */
  const Can_17_McmCan_ControllerBaudrateConfigType* CanBaudrateConfigPtr;

  #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* Pointer to Controller FDBaudrate Configuration */
  const Can_17_McmCan_ControllerFDBaudrateConfigType* CanFDConfigParamPtr;
  #endif

  /* Pointer to Configuration of transmit hardware objects */
  const Can_17_McmCan_TxHwObjectConfigType* CanTxHwObjectConfigPtr;
  /* Pointer to Configuration of receive hardware objects ->Standard Messages */
  const Can_17_McmCan_SIDFilterConfigType* CanSIDFilterConfigPtr;
  /* Pointer to Configuration of receive hardware objects ->Extended Messages */
  const Can_17_McmCan_XIDFilterConfigType* CanXIDFilterConfigPtr;

  #if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)
  /* Transmit Period to core specific period mapping*/
  const Can_17_McmCan_HthPeriodIndexType* CanHthPeriodIndexPtr;
  /* Pointer to Configuration of Transmit hardware objects supported with
  multiple Periods */
  const Can_17_McmCan_HthMaskObjectConfigType* CanHthMaskObjectConfigPtr;
  /* Pointer to Configuration of Tx hardware object with multiple periods */
  const Can_17_McmCan_PeriodHthMaskConfigType* CanPeriodHthMaskConfigPtr;
  #endif
  #if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)
  /* Recieve Period to core specific period mapping*/
  const Can_17_McmCan_HrhPeriodIndexType* CanHrhPeriodIndexPtr;
  /* Pointer to Configuration of Receive hardware objects supported with
  multiple Periods */
  const Can_17_McmCan_HrhMaskObjectConfigType* CanHrhMaskObjectConfigPtr;
  /* Pointer to Configuration of Rx hardware object with multiple periods */
  const Can_17_McmCan_PeriodHrhMaskConfigType* CanPeriodHrhMaskConfigPtr;
  #endif

} Can_17_McmCan_CoreConfigType;

/* [cover parentID={17D20A33-9909-4557-83BF-5E15F6419166}]
   [cover parentID={1438CB3A-D1B9-4ded-80A1-C01EAA92025C}]
[/cover] */
/* Master configuration structure having the pointer index of the configurations
for all the cores in the micro-controller */
typedef struct
{
  /* Core specific configuration set */

  /* Pointer to the Core specific CAN configuration set */
  const Can_17_McmCan_CoreConfigType* CanCoreConfigPtr[MCAL_NO_OF_CORES];
  /* Global data shared amongst all cores */

  /* Number of Kernels configured */
  const uint8 CanNoOfKernel;
  /* Number of Hrh configured */
  const Can_HwHandleType CanNoOfHrh;
  /* Pointer to CAN Kernel configuration */
  const Can_17_McmCan_McmModuleConfigType* CanMCMModuleConfigPtr;
  /* Pointer holding physical controller index data */
  const Can_17_McmCan_PhyControllerIndexType* CanPhyControllerIndexPtr;
  /* Pointer holding logical controller index data */
  const Can_17_McmCan_LogicalControllerIndexType* CanLogicalControllerIndexPtr;
  /* Pointer holding configured Hth index data */
  const Can_17_McmCan_HthIndexType* CanHthIndexPtr;

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  /* Pointer to the ICOM configurations set */
  const Can_17_McmCan_IcomConfigType *CanIcomConfigPtr;
  /* Pointer to the ICOM Rx message configurations */
  const Can_17_McmCan_IcomRxMsgConfigType *CanIcomMsgConfigPtr;
  /* Pointer to the ICOM Rx message signal configurations */
  const Can_17_McmCan_IcomRxMsgSignalConfigType *CanIcomRxSignalConfigPtr;
  #endif
  #if (CAN_17_MCMCAN_LPDU_RX_CALLOUT == STD_ON)
  const Can_17_McmCan_LPduRxCalloutFnPtrType CanLPduRxCalloutFuncPtr;
  #endif
} Can_17_McmCan_ConfigType;

/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Functions                                      **
*******************************************************************************/
#define CAN_17_MCMCAN_START_SEC_CODE_QM_GLOBAL
/*[cover parentID={566ED99C-0D96-46ac-97BF-E97B04E2C700}]
[/cover] */
/*[cover parentID={4579FE20-92DA-4848-93DB-7AD4FD35DD50}]
[/cover] */
/*[cover parentID={5713A17A-3FA1-427f-A0B6-89125A17689A}]
[/cover] */
/*[cover parentID={E4047F9A-8865-4b2c-9D3C-385479854EAD}]
[/cover] */
/*[cover parentID={EA0715EE-3E3C-4aac-A42E-4B5CFC96CEED}]
[/cover] */
/*[cover parentID={A4F3B858-B167-4b5c-AB7F-390C5F5D2185}]
[/cover] */
/* MISRA2012_RULE_20_1_JUSTIFICATION: Memmap header usage as per Autosar
   guideline. */
/* MISRA2012_RULE_4_10_JUSTIFICATION: Memmap header is repeatedly included
    without safegaurd. It complies to Autosar guidelines. */
#include "Can_17_McmCan_MemMap.h"

/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_Init                                 **
**                    (                                                       **
**                      const Can_17_McmCan_ConfigType* const Config          **
**                    )                                                       **
**                                                                            **
** Description      : Driver Module Initialization function                   **
**                    * This function initializes:                            **
**                    * Static variables, including flags                     **
**                    * CAN HW Unit global hardware settings                  **
**                    * Controller specific settings for each CAN Controller  **
** All CAN Controllers will be in state CANIF_CS_STOPPED after initialization **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x00                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters(in)   : Config - Pointer to CAN driver configuration            **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_Init
(
  const Can_17_McmCan_ConfigType* const Config
);

#if (CAN_17_MCMCAN_DEINIT_API == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_DeInit                     **
**                    (                                                       **
**                     void                                                   **
**                    )                                                       **
**                                                                            **
** Description      : This function for De-initializes the CAN Driver.        **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x65                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_DeInit
(
  void
);
#else
/* MISRA2012_RULE_4_9_JUSTIFICATION: GetVersionInfo is declared as a function
  like macro as per AUTOSAR.*/
#define Can_17_McmCan_DeInit() \
             (ERROR_Can_17_McmCan_DeInit_NOT_SELECTED)

#endif /* (CAN_17_MCMCAN_DEINIT_API == STD_ON) */

#if (CAN_17_MCMCAN_VERSION_INFO_API == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_GetVersionInfo                       **
**                    (                                                       **
**                      Std_VersionInfoType* const versioninfo                **
**                    )                                                       **
**                                                                            **
** Description      : - This function returns the version information of this **
**                      module.The version information include : Module ID,   **
**                      Vendor ID,Vendor specific version numbers             **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x07                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (out) : versioninfo - Pointer to store the                      **
**                    version information of this module.                     **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_GetVersionInfo
(
  Std_VersionInfoType* const versioninfo
);
#else
/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro used to generate
  compile time error in case of incorrect configuration*/
#define Can_17_McmCan_GetVersionInfo(versioninfo)                              \
                               (ERROR_Can_17_McmCan_GetVersionInfo_NOT_SELECTED)

#endif /* #if( (CAN_17_MCMCAN_VERSION_INFO_API == STD_ON) */


#if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : Std_ReturnType Can_17_McmCan_CheckBaudrate              **
**                    (                                                       **
**                      const uint8 Controller,                               **
**                      const uint16 Baudrate                                 **
**                    )                                                       **
**                                                                            **
** Description      : Checks CAN Controller Baudrate                          **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x0e                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                    Baudrate -  Baudrate to be checked                      **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : E_OK/E_NOT_OK                                           **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Can_17_McmCan_CheckBaudrate
(
  const uint8 Controller, const uint16 Baudrate
);

/*******************************************************************************
**                                                                            **
** Syntax           : Std_ReturnType Can_17_McmCan_SetBaudrate                **
**                    (                                                       **
**                     const uint8 Controller,                                **
**                     const uint16 BaudRateConfigID                          **
**                    )                                                       **
**                                                                            **
** Description      : This service shall set the baud rate configuration of   **
**                    the CAN controller.                                     **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x0f                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                    BaudRateConfigID - References a baud rate configuration **
**                    by ID                                                   **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : E_OK/E_NOT_OK                                           **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Can_17_McmCan_SetBaudrate
(
  const uint8 Controller,
  const uint16 BaudRateConfigID
);
#else
/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro used to generate
 compile time error in case of incorrect configuration*/
#define Can_17_McmCan_CheckBaudrate(Controller,Baudrate) \
             (ERROR_Can_17_McmCan_CheckBaudrate_NOT_SELECTED)

/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro used to generate
 compile time error in case of incorrect configuration*/
#define Can_17_McmCan_SetBaudrate(Controller,BaudRateConfigID) \
             (ERROR_Can_17_McmCan_SetBaudrate_NOT_SELECTED)

#endif /* #if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON) */

/*******************************************************************************
**                                                                            **
** Syntax           : Can_ReturnType Can_17_McmCan_SetControllerMode          **
**                    (                                                       **
**                      const uint8 Controller,                               **
**                      const Can_StateTransitionType Transition              **
**                    )                                                       **
**                                                                            **
** Description      : Performs software triggered state transitions of the    **
**                    CAN Controller State machine                            **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x03                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                    Transition - Requested transition                       **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : CAN_OK/CAN_NOT_OK                                       **
**                                                                            **
*******************************************************************************/
extern Can_ReturnType Can_17_McmCan_SetControllerMode
(
  const uint8 Controller,
  const Can_StateTransitionType Transition
);

/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_DisableControllerInterrupts          **
**                    (                                                       **
**                      const uint8 Controller                                **
**                    )                                                       **
**                                                                            **
** Description      : Disables CAN Controller all Interrupts                  **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x04                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_DisableControllerInterrupts
(
  const uint8 Controller
);

/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_EnableControllerInterrupts           **
**                    (                                                       **
**                      const uint8 Controller,                               **
**                    )                                                       **
**                                                                            **
** Description      : Enable CAN Controller all Interrupts                    **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x05                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_EnableControllerInterrupts
(
  const uint8 Controller
);

/*******************************************************************************
**                                                                            **
** Syntax           : Can_ReturnType Can_17_McmCan_Write                      **
**                    (                                                       **
**                      const Can_HwHandleType Hth,                           **
**                      const Can_PduType* const PduInfo                      **
**                    )                                                       **
**                                                                            **
** Description      : Service to transmit CAN frame                           **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x06                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant(thread-safe)                                  **
**                                                                            **
** Parameters (in)  : Hth - Hardware Transmit Handler                         **
**                    PduInfo - Pointer to SDU user memory, DLC and           **
**                                                            Identifier      **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : CAN_OK/CAN_BUSY/CAN_NOT_OK                              **
**                                                                            **
*******************************************************************************/
extern Can_ReturnType Can_17_McmCan_Write
(
  const Can_HwHandleType Hth,
  const Can_PduType* const PduInfo
);

#if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_MainFunction_Write                   **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Description      : This function performs the polling of TX confirmation   **
**                    when CAN_TX_PROCESSING is set to POLLING                **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x01                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_MainFunction_Write(void);
#endif /* #if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF) */

#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_MainFunction_Read                    **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Description      : This function performs the polling of RX indication     **
**                    when CAN_RX_PROCESSING is set to POLLING                **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x08                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_MainFunction_Read(void);
#endif /* #if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF) */
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_MainFunction_BusOff                  **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Description      : This function performs the polling of bus-off events    **
**                    that are configured statically as 'to be polled'        **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x09                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_MainFunction_BusOff
(
  void
);

/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_MainFunction_Wakeup                  **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Description      : This function performs the polling of wake-up events    **
**                    that are configured statically as 'to be polled'        **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x0A                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_MainFunction_Wakeup
(
  void
);

/*******************************************************************************
** Syntax           : void Can_17_McmCan_MainFunction_Mode                    **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Description      : This function performs the polling of CAN Controller    **
**                    mode transitions                                        **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x0C                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : none                                                    **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_MainFunction_Mode
(
  void
);

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : Std_ReturnType Can_17_McmCan_SetIcomConfiguration       **
**                   (                                                        **
**                     const uint8 Controller,                                **
**                     const IcomConfigIdType ConfigurationId                 **
**                   )                                                        **
**                                                                            **
** Description      : This service shall change the Icom Configuration of a   **
**                    CAN controller to the requested one.                    **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : 0x11                                                    **
**                                                                            **
** Sync/Async       : Asynchronous                                            **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : Controller - Associated CAN Controller                  **
**                    ConfigurationId  - Requested Configuration              **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : E_OK/E_NOT_OK                                           **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Can_17_McmCan_SetIcomConfiguration
(
  const uint8 Controller,
  const IcomConfigIdType ConfigurationId
);
#else
/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro used to generate
 compile time error in case of incorrect configuration*/
#define Can_17_McmCan_SetIcomConfiguration(Controller ,ConfigurationId) \
             (ERROR_Can_17_McmCan_SetIcomConfiguration_NOT_SELECTED)
#endif  /*(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)*/

/*******************************************************************************
**                      Interrupt Service Routines                            **
*******************************************************************************/

#if(CAN_17_MCMCAN_BO_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           : void Can_17_McmCan_IsrBusOffHandler                    **
**                    (                                                       **
**                      const uint8 HwKernelId                                **
**                      const uint8 NodeIdIndex                               **
**                    )                                                       **
**                                                                            **
**  Description      : - This is the interrupt service routine for BusOff     **
**                    handling                                                **
**                    This function will checks any of the CAN Controller     **
**                    associated with Can Module is on BusOff state then      **
**                    calls handler function with controller information      **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : HwKernelId - Requested with associated kernelId        **
**                     NodeIdIndex - Requested with associated node id        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_IsrBusOffHandler
(
  const uint8 HwKernelId, const uint8 NodeIdIndex
);
#endif

#if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_IsrReceiveHandler                    **
**                    (                                                       **
**                      const uint8 HwKernelId                                **
**                      const uint8 NodeIdIndex                               **
**                    )                                                       **
**                                                                            **
** Description      : - This is the interrupt service routine for handling    **
**                      Interrupt of dedicated Rx buffer.                     **
**                      This function checks the CAN Controller is received   **
**                      any messages via Rx dedicated buffer if yes then      **
**                      calls receive handler function                        **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : NA                                                      **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : HwKernelId - Requested with associated kernelId         **
**                    NodeIdIndex - Requested with associated node id         **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_IsrReceiveHandler
(
  const uint8 HwKernelId, const uint8 NodeIdIndex
);
#endif

#if(CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
** Syntax           : void Can_17_McmCan_IsrTransmitHandler                   **
**                    (                                                       **
**                      const uint8 HwKernelId                                **
**                      const uint8 NodeIdIndex                               **
**                    )                                                       **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : HwKernelId - Requested with associated kernelId         **
**                    NodeIdIndex - Requested with associated node id         **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : - This is the interrupt service routine for Transmit    **
**                    Interrupt                                               **
**                    - In case of successful transmission event, this        **
**                    function notifies the upper layer                       **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_IsrTransmitHandler
(
  const uint8 HwKernelId, const uint8 NodeIdIndex
);
#endif

#if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_IsrRxFIFOHandler                    **
**                    (                                                       **
**                      const uint8 HwKernelId                                **
**                      const uint8 NodeIdIndex                               **
**                    )                                                       **
**                                                                            **
** Description      : This is the interrupt service routine for checking      **
**                    Received messaged via RXFIFO                            **
**                    This function shall checks the CAN Controller FIFO      **
**                    status calls appropriate handler function               **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : NA                                                      **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : HwKernelId - Requested with associated kernelId         **
**                    NodeIdIndex - Requested with associated node id         **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
extern void Can_17_McmCan_IsrRxFIFOHandler
(
  const uint8 HwKernelId, const uint8 NodeIdIndex
);
#endif

#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_lRxPeriodHandler                     **
**                    (                                                       **
**                      uint8 HrhRefPeriods                                   **
**                    )                                                       **
**                                                                            **
** Description      : The internal fucntion is used for checking the receive  **
**                    handler is received with new data or not                **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : NA                                                      **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : HrhRefPeriods - Requested with Cyclic time period index **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
/* Function to handle multiple period receive Hw objects */
extern void Can_17_McmCan_lRxPeriodHandler
(
  const uint8 HrhRefPeriods
);
#endif

#if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax           : void Can_17_McmCan_lTxPeriodHandler                     **
**                    (                                                       **
**                      uint8 HthRefPeriods                                   **
**                    )                                                       **
**                                                                            **
** Description      : The internal fucntion is used for checking the transmit **
**                    handler is status as completed or not                   **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : NA                                                      **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non-Reentrant                                           **
**                                                                            **
** Parameters (in)  : HthRefPeriods - Requested with Cyclic time period index **
**                                                                            **
** Parameters (out) : none                                                    **
**                                                                            **
** Return value     : none                                                    **
**                                                                            **
*******************************************************************************/
/* Function for Handling multiple period Transmit Hw object */
extern void Can_17_McmCan_lTxPeriodHandler
(
  const uint8 HthRefPeriods
);
#endif
#define CAN_17_MCMCAN_STOP_SEC_CODE_QM_GLOBAL
/*[cover parentID={566ED99C-0D96-46ac-97BF-E97B04E2C700}]
[/cover] */
/*[cover parentID={4579FE20-92DA-4848-93DB-7AD4FD35DD50}]
[/cover] */
/*[cover parentID={5713A17A-3FA1-427f-A0B6-89125A17689A}]
[/cover] */
/*[cover parentID={E4047F9A-8865-4b2c-9D3C-385479854EAD}]
[/cover] */
/*[cover parentID={EA0715EE-3E3C-4aac-A42E-4B5CFC96CEED}]
[/cover] */
/*[cover parentID={A4F3B858-B167-4b5c-AB7F-390C5F5D2185}]
[/cover] */
/* MISRA2012_RULE_4_10_JUSTIFICATION: Memmap header is repeatedly included
   without safegaurd. It complies to Autosar guidelines. */
#include "Can_17_McmCan_MemMap.h"

#include "Can_17_McmCan_PBcfg.h"
#endif  /* Header inclusion End */
