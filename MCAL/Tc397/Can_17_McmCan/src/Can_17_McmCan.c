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
**  FILENAME     : Can_17_McmCan.c                                            **
**                                                                            **
**  VERSION      : 1.40.0_38.0.0                                              **
**                                                                            **
**  DATE         : 2020-04-16                                                 **
**                                                                            **
**  VARIANT      : Variant PB                                                 **
**                                                                            **
**  PLATFORM     : Infineon AURIX2G                                           **
**                                                                            **
**  AUTHOR       : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**  VENDOR       : Infineon Technologies                                      **
**                                                                            **
**  TRACEABILITY : [cover parentID={75972939-822C-41cc-8B6A-327F181E274A}]    **
**                 [cover parentID={72DABBEB-F27B-4677-B6B4-B53F634341BA}]    **
**                                                                            **
**  DESCRIPTION  : CAN Driver source file                                     **
**                                                                            **
**  SPECIFICATION(S) : Specification of CAN Driver, AUTOSAR Release 4.2.2     **
**                                                                            **
**  MAY BE CHANGED BY USER : no                                               **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
/* Include the declarations of Can functions and types */
#include "Can_17_McmCan.h"

/* [cover parentID={6CE56ADD-37B9-4c96-BC08-609A206511DC}]
[/cover] */
/* [cover parentID={C0E673D5-B80C-40f5-9944-70221E106146}]
[/cover] */
/* Register definition file for Aurix2G target */
#include "IfxSrc_reg.h"
/* MCAL header file containing global macros, type definitions and functions
needed by all MCAL drivers */
#include "McalLib.h"
#if(CAN_17_MCMCAN_INIT_DEINIT_API_MODE != CAN_17_MCMCAN_MCAL_SUPERVISOR)
#include "McalLib_OsStub.h"
#endif

/* Include the declarations of Can functions and types */
#include "SchM_Can_17_McmCan.h"

/* Include the declarations of CanIf callback functions and types */
#include "CanIf_Cbk.h"

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/* Include default error detection header file if DET is Enabled */
#include "Det.h"
#endif

/*******************************************************************************
**                       Version check                                        **
*******************************************************************************/
/* [cover parentID={527E2B9D-7EFB-422d-A9BF-282FD3E78B84}]
  Version across the files
[/cover] */
/* Check if the correct version of Can_17_McmCan.h is included */
#ifndef CAN_17_MCMCAN_AR_RELEASE_MAJOR_VERSION
#error "CAN_17_MCMCAN_AR_RELEASE_MAJOR_VERSION is not defined. "
#endif

#if ( CAN_17_MCMCAN_AR_RELEASE_MAJOR_VERSION != 4U )
#error "CAN_17_MCMCAN_AR_RELEASE_MAJOR_VERSION does not match. "
#endif

/* Check for definition of SW version Information */
#ifndef CAN_17_MCMCAN_SW_MAJOR_VERSION
#error "CAN_17_MCMCAN_SW_MAJOR_VERSION is not defined. "
#endif

#ifndef CAN_17_MCMCAN_SW_MINOR_VERSION
#error "CAN_17_MCMCAN_SW_MINOR_VERSION is not defined. "
#endif

#ifndef CAN_17_MCMCAN_SW_PATCH_VERSION
  #error "CAN_17_MCMCAN_SW_PATCH_VERSION is not defined. "
#endif

/* Check for Correct inclusion of headers */
#if ( CAN_17_MCMCAN_SW_MAJOR_VERSION != 10U )
#error "CAN_17_MCMCAN_SW_MAJOR_VERSION does not match. "
#endif

#if ( CAN_17_MCMCAN_SW_MINOR_VERSION != 40U )
#error "CAN_17_MCMCAN_SW_MINOR_VERSION does not match. "
#endif

#if (CAN_17_MCMCAN_SW_PATCH_VERSION != 2U)
  #error "CAN_17_MCMCAN_SW_PATCH_VERSION does not match. "
#endif

/* Inter Module Check */

/* [cover parentID={A3DC8FC4-FD65-43c0-A540-925A1AC9702B}]
  Version check for DET
[/cover] */
/*
  VERSION CHECK FOR DET MODULE INCLUSION
*/
#if ( CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)

#ifndef DET_AR_RELEASE_MAJOR_VERSION
#error "DET_AR_RELEASE_MAJOR_VERSION is not defined. "
#endif

#if ( DET_AR_RELEASE_MAJOR_VERSION != CAN_17_MCMCAN_AR_RELEASE_MAJOR_VERSION )
#error "DET_AR_RELEASE_MAJOR_VERSION does not match. "
#endif

#endif /* #if ( CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

/*******************************************************************************
**                       Private Macro Definitions                            **
*******************************************************************************/
/* Interrupt flag check count */
/* MISRA2012_RULE_2_5_JUSTIFICATION: Used in multiple configuration
combination */
#define CAN_17_MCMCAN_FLAG_CHECK_COUNT                      (uint8)3U

/* CLEAR IR Flags */
#define CAN_17_MCMCAN_RF0L_CLEAR                            (uint32)0x00000008U
#define CAN_17_MCMCAN_RF1L_CLEAR                            (uint32)0x00000080U


/* Clear event FULL and LOST condition in IR */
/* MISRA2012_RULE_2_5_JUSTIFICATION: Used in multiple configuration
combination */
#define CAN_17_MCMCAN_TEFF_TEFL_CLEAR                       (uint32)0x0000C000U

/* Register write macros */
#define CAN_17_MCMCAN_REG_ONE                               (uint32)1

/* Values used by the CAN driver */
#define CAN_17_MCMCAN_ZERO                                  (uint8)0
#define CAN_17_MCMCAN_ONE                                   (uint8)1
#define CAN_17_MCMCAN_TWO                                   (uint8)2
#define CAN_17_MCMCAN_THREE                                 (uint8)3
#define CAN_17_MCMCAN_FOUR                                  (uint8)4
#define CAN_17_MCMCAN_FIVE                                  (uint8)5
#define CAN_17_MCMCAN_SIX                                   (uint8)6
#define CAN_17_MCMCAN_EIGHT                                 (uint8)8

#define CAN_17_MCMCAN_HUNDRED                               (uint8)100

#if ((CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) || \
     (CAN_17_MCMCAN_FD_ENABLE == STD_ON))
#define CAN_17_MCMCAN_SEVEN                                 (uint8)7
#endif

#define CAN_17_MCMCAN_SIXTEEN                               (uint8)16

#if ((CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)|| \
     (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON))
#define CAN_17_MCMCAN_NO_HOH_VAL                            (uint8)255
#endif

#if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
#define CAN_17_MCMCAN_ELEVEN                                (uint8)11
#define CAN_17_MCMCAN_FIFTEEN                               (uint8)15
/* Shift operation values used by the CAN driver */
#define CAN_17_MCMCAN_SHIFT_TWENTYFOUR                      (uint8)24
/* Message RAM element size with FD support */
#define CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_FD               (uint32)72
#endif

/* RAM BYTE size */
#define CAN_17_MCMCAN_RAM_BYTES                             (uint32)4

/* Shift operation values used by the CAN driver */
#define CAN_17_MCMCAN_SHIFT_EIGHTEEN                        (uint8)18

/* Values for clear and Register data check */
#define CAN_17_MCMCAN_DATA_ZERO             (Ifx_UReg_32Bit) (0x00000000U)

/* Value for Address Check */
#define CAN_17_MCMCAN_ADDRESS_CHECK         (uint32) (0x00000000U)

/* Values for clear IR register */
#define CAN_17_MCMCAN_CLEAR_ALL_INTERRUPTS   (uint32)(0x3FCFFFFFU)

/* Values for Set Global register for reject RTR and Non Match Frames */
#define CAN_17_MCMCAN_GFC_REJT_RTR_NONMATCH   (uint32)(0x0000003FU)

/* Value for clear new data register */
#define CAN_17_MCMCAN_CLEAR_NEWDATA_REG       (uint32)(0xFFFFFFFFU)

/* Value for MASK the Message RAM Offset address */
#define CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK  (uint32) (0x0000FFFC)

/* value for Tx buffer Index */
#define CAN_17_MCMCAN_TX_BUFF_MASK            (uint32)(0x00000001U)
/* Set for identify the MsgID is extended */
#define CAN_17_MCMCAN_SET_XTD_BIT_MASK        (0x80000000U)

/* used for Mask value for identify the INIT */
#define CAN_17_MCMCAN_CCCR_INIT_SET_MASK      (uint32)(0x00000001U)

/* used for Mask value for identify the INIT */
#define CAN_17_MCMCAN_CCCR_CCE_SET_MASK      (uint32)(0x00000003U)

/* Value used for the CLK selection */
#define CAN_17_MCMCAN_MCR_CLKSEL_SET          (Ifx_UReg_32Bit)(0x00000003U)

/* Value used for enable the CLK configuration change */
#define CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE                   (0xC0000000U)

/* Value used for NDAT2 Register buffer Position access */
#define CAN_17_MCMCAN_THIRTY_TWO                            (uint8)32

/* Value used for check bit is set or not */
#define CAN_17_MCMCAN_BIT_CLEAR_VAL                         (Ifx_UReg_32Bit)0

/* Bit Set Value */
#define CAN_17_MCMCAN_BIT_SET_VAL                           (Ifx_UReg_32Bit)1

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* Message Id Mask Value */
#define CAN_17_MCMCAN_MSG_MASK_VAL                          (Can_IdType)0
#endif

#if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)
/* Clear DRX (dedicated buffer) flag in IR register */
#define CAN_17_MCMCAN_CLEAR_IE_DRX_BIT                  (uint32) (0x00080000U)
#endif /* #if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON) */

#if (CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)
/* Clear TEFN (Tx Event FIFO New Entry) flag in IR register */
#define CAN_17_MCMCAN_CLEAR_IE_TEFN_BIT                 (uint32) (0x00001000U)
#endif /* #if (CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON) */

#if(CAN_17_MCMCAN_BO_INTERRUPT_PROCESSING == STD_ON)
/* Clear BO (Bus_Off Status) flag in IR register */
#define CAN_17_MCMCAN_CLEAR_IE_BO_BIT                   (uint32) (0x02000000U)
#endif
#if ((CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON) || \
     (CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON))
/* Clear RF1W,RF1F flag in IR register */
#define CAN_17_MCMCAN_CLEAR_IR_RF1_WF_BITS             (uint32) (0x00000060U)

/* Clear RF0W,RF0F flag in IR register */
#define CAN_17_MCMCAN_CLEAR_IR_RF0_WF_BITS             (uint32) (0x00000006U)
#endif

/* Macro to get the core ID */
#define CAN_17_MCMCAN_GETCOREID ((uint8)(Mcal_GetCpuIndex()))

#if (CAN_17_MCMCAN_DEINIT_API == STD_ON)
/* Status to indicate that CAN is initialized */
#define CAN_17_MCMCAN_DEINITPENDING              ((uint32)2)
/* Status to indicate that CAN is de-initailized */
#define CAN_17_MCMCAN_UNINIT                      ((uint32)0)
#endif /* #if(CAN_17_MCMCAN_DEINIT_API == STD_ON) */

/* Status to indicate that CAN is initialized */
#define CAN_17_MCMCAN_READY                       ((uint32)1)

/* SiZE of the variable to be read */
#define CAN_17_MCMCAN_INIT_STATUS_SIZE            2

/* Disable CAN clock */
#define CAN_17_MCMCAN_CLK_DISABLE                ((uint32)0)

/* Number of transmit hardware object buffers present */
#define CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE             ((uint32)32)

#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Number of transmit hardware object buffers present */
#define CAN_17_MCMCAN_MAX_SWOBJECT_INDEX                    ((uint16)384)
#endif

/* Values used for FD mask and Extended Id Mask */
#define CAN_17_MCMCAN_EXTENDED_ID_SET             (uint32)(0x80000000U)
#define CAN_17_MCMCAN_EXTENDED_ID_MASK            (uint32)(0x1FFFFFFFU)
#define CAN_17_MCMCAN_STANDARD_ID_MASK            (uint32)(0x000007FFU)
#if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
#define CAN_17_MCMCAN_ID_FD_ENABLE_SET            (uint32)(0x40000000U)
/* Value used for set the FD status for upper-layer */
#define CAN_17_MCMCAN_SET_FD_MASK                 (0x40000000U)
#endif

/* Message RAM element size with FD support */
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_NFD              (uint32)16

/* CCR.INIT bit status check mask value */
#define CAN_17_MCMCAN_NODE_INIT_MODE              (0x00000001U)
#define CAN_17_MCMCAN_NODE_INIT_CCE_SET           (uint32)(0x00000001)
#define CAN_17_MCMCAN_CCCR_CSA_STATUS                        (0x00000008U)
#define CAN_17_MCMCAN_MODULE_DISABLE_STATUS                 (0x00000002U)
#define CAN_17_MCMCAN_MCR_RBUSY_STATUS                 (0x10000000U)
#define CAN_17_MCMCAN_NODE_PSR_MODE              (0x00000018U)

#if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
/* Value used for Max FD length operation */
#define CAN_17_MCMCAN_MAX_FD_LENGTH                (uint8)64
#define CAN_17_MCMCAN_CCCR_FDOE_STATUS             (uint32)(0x00000100U)
#define CAN_17_MCMCAN_CCCR_BRSE_STATUS             (uint32)(0x00000200U)
#endif

/* Normal DLC length */
#define CAN_17_MCMCAN_MAX_DATA_LENGTH                       (uint8) (8)

/* Maximum number of transmit message buffers available */
#define CAN_17_MCMCAN_MAX_TX_BUFFER_INDEX                   (uint8) (32)

/*******************************************************************************
**                         User Mode Macros                                   **
*******************************************************************************/

/* [cover parentID={1A65EADD-AFD0-4845-B2D2-8257E086DD67}]
[/cover] */
#if(CAN_17_MCMCAN_INIT_DEINIT_API_MODE == CAN_17_MCMCAN_MCAL_SUPERVISOR)
/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro is defined for
   User mode support in code. No side effects foreseen by violating this MISRA
   rule. */
#define CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG(RegAdd,Data)   \
    Mcal_WritePeripEndInitProtReg(RegAdd,Data)

#else

/* MISRA2012_RULE_4_9_JUSTIFICATION: Function like macro is defined for
   User mode support in code. No side effects foreseen by violating this MISRA
   rule. */
#define CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG(RegAdd,Data)   \
    MCAL_LIB_WRITEPERIPENDINITPROTREG(RegAdd,Data)

#endif

/******************************************************************************
**                      Private Type Definitions                             **
******************************************************************************/
/* Data type used for core specific CAN driver global data storage */
/* [cover parentID={74FB2479-A60D-417a-B557-5B1530ED4D89}]
[/cover] */
typedef struct
{
  /* Pointer to the CAN transmit buffer Mask */
  uint32* CanTxMask;
  /* Pointer to Backup variable used to restore during BO */
  uint32* BackupCanTxMask;
  /* Can controller mode */
  CanIf_ControllerModeType* CanControllerModePtr;
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  uint8* CanInterruptStatusPtr;
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  uint8* CanInterruptTempStatusPtr;
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  uint8* CanDisableIntrpCountPtr;
  /* Used to store swPduHandle passed by CanIf */
  PduIdType* CanSwObjectHandlePtr;

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  /* This variable stores the wake on bus off status on ICOM configuration */
  boolean* CanIcomConfgBOStatusPtr;
  /* This variable stores the Pretended network support status */
  boolean* CanIcomEnableStatusPtr;
  /* If this variable is SET, Can_Write API will be blocked indicating that ICOM
  enable is in progress and filters are getting updated */
  boolean* CanIcomBlockWriteCalls;
  /* This variable used to stores the current ICOM Configuration ID */
  uint8* CanActiveIcomCfgIdPtr;
  /* ICOM Message counter counter value */
  uint16* CanIcomMsgCntrValPtr;
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
  (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  /* Variable used to hold the order of transmit that have occured */
  uint16* CanTxSwObjPriorityPtr;
  #endif

} Can_17_McmCan_CoreStatesType;
/* [cover parentID={B5190528-1363-4e35-8769-DEC665200F41}]
[/cover] */
typedef enum
{
  CAN_17_MCMCAN_EQUAL = 0,
  CAN_17_MCMCAN_NOT_EQUAL = 1,
  CAN_17_MCMCAN_NAND = 2
} Can_17_McmCan_TimeoutCheckType;

/******************************************************************************
**                      Private Function Declarations                        **
******************************************************************************/
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars
in generated code due to Autosar Naming constraints.*/
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

#if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
LOCAL_INLINE uint8 Can_17_McmCan_lCheckQueueMask(const uint8 ControllerId,
const Can_17_McmCan_CoreConfigType* const CanConfig,
const uint32* const CanQueueSlotMask);
#endif

/* Function to initialize CAN Module used global variables */
LOCAL_INLINE void Can_17_McmCan_lInitGlobalVar(
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function to initialize CAN Module clock/timing registers */
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lInitClkSetting(
    const Can_17_McmCan_ConfigType* const Config,
    const uint8 KernelIndx);

/* Function to initialize the global CAN Module */
static Std_ReturnType Can_17_McmCan_lGlobalInit(
    const Can_17_McmCan_ConfigType* const Config);

/* Function to initialize the Core specific CAN Module */
static Std_ReturnType Can_17_McmCan_lCoreInit(
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function to initialize CAN Module Node registers */
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lInitController(
    const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function to initialize Tx Hw message objects */
LOCAL_INLINE void Can_17_McmCan_lInitTxMsgObj(const uint8 Controller,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
/* Function to cancel the pending Tx request */
static void Can_17_McmCan_lCancelPendingTx (const uint8 ControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
/* Internal function for Handling Tx write operation */
static Can_ReturnType Can_17_McmCan_lWriteMsgObj(
    const Can_HwHandleType HthIndex,
    const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
/* Function to handle trigger transmit handle */
LOCAL_INLINE Can_ReturnType Can_17_McmCan_lUpdatePduInfo(
    const Can_HwHandleType HthIndex,
    Can_PduType* const TempPduInfo, const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
#endif /* #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON) */

#if((CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)|| \
((CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)&& \
 (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF)))
/* Function for handling Tx confirmation operation */
static void Can_17_McmCan_lTxEventHandler (const uint8 HwControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
#endif

#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON) && \
(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
static Std_ReturnType Can_17_McmCan_lPeriodCheckSwHandle(
    const uint8 CtrlIndex,
    const uint16 SwObjIndx, const uint8 HthRefPeriods,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);
#endif

#if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON)
/* Function to set the nominal baudrate */
static Std_ReturnType Can_17_McmCan_lSetBaudrate(const uint8 Controller,
    const uint16 BaudrateIndx,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
#endif /* #if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON) */

#if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)

/* Function to set the FD support baudrate */
static Std_ReturnType Can_17_McmCan_lFDSetBaudrate(const uint8 Controller,
    const uint16 FdIndex,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON) */

/* Function to disable the Hw support interrupts */
LOCAL_INLINE void Can_17_McmCan_lDisableInterrupts (const uint8 Controller,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function to enable the Hw support interrupts */
LOCAL_INLINE void Can_17_McmCan_lEnableInterrupts (const uint8 Controller,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function for state changes if wakeup is disabled */
static Can_ReturnType Can_17_McmCan_lSetModeWuSupportOff(
    const uint8 Controller,
    const Can_StateTransitionType Transition,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

/* Function to initialize Rx Hw message objects */
LOCAL_INLINE void Can_17_McmCan_lInitRxMsgObj(const uint8 Controller,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
/* Function to clear all received messages to avoid unexpected reception
 condition */
static void  Can_17_McmCan_lClearReceivedMsg(const uint8 HwControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

/* Function to set the filter mask for standard messages */
LOCAL_INLINE void Can_17_McmCan_lSIDFilter_Config (const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
/* Function to set the filter mask for Extended messages */
LOCAL_INLINE void Can_17_McmCan_lXIDFilter_Config (const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#if(((CAN_17_MCMCAN_RX_POLLING_PROCESSING == STD_ON)&& \
    (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF))|| \
    (CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)|| \
    ((CAN_17_MCMCAN_WU_POLLING_PROCESSING == STD_ON) && \
    (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)))
/* Function for Handling data reception */
static void Can_17_McmCan_lReceiveHandler(const uint8 HwControllerId,
    const uint8 CheckBuffType,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);
#endif

#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)
static void Can_17_McmCan_lCheckIfFifoMessageLost(const uint8 ControllerId,
                                     const Can_17_McmCan_RxBufferType RxBufferType);
#endif

/* Function for Handling data retrieve for the message buffer */
static void Can_17_McmCan_lRxExtractData(const uint8 HwControllerId,
    const uint8 RxBuffIndex,
    const Can_17_McmCan_RxBufferType RxBuffer,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);


/* Handler for Bus-Off condition */
static void Can_17_McmCan_lBusOffHandler(const uint8 HwControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)

/* Function for Activate controller with Pretended mode */
LOCAL_INLINE void Can_17_McmCan_lActivateIcom (const uint8 Controller,
    const IcomConfigIdType ConfigurationId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

/* Function for De-activate controller with Pretended mode */
static void Can_17_McmCan_lDeactivateIcom (const uint8 Controller,
    const IcomConfigIdType ConfigurationId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

/* Function to do ICOM message validation for greater or smaller operation */
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lIcomCheckGreaterSmaller(
    const uint8 ValData, const uint8 ConfigData,
    uint8* const CheckVal,
    const Can_17_McmCan_IcomSignalOperType SignalOperType);

/* Function for Validate signal conditions */
static Std_ReturnType Can_17_McmCan_lIcomValidateMsgSignal(
    const uint16 MessageIndex,
    const PduInfoType* const PduInfoPtr,
    const Can_17_McmCan_ConfigType* const CanConfig);

/* Function for Validate receive message conditions */
static void Can_17_McmCan_lIcomValidateMsg(Can_HwType* const Mailbox,
    const PduInfoType* const PduInfoPtr,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */

/* Function for Hw failure time-out detection */
static Std_ReturnType Can_17_McmCan_lTimeOut (
    const volatile uint32* const Can_RegAddress,
    const uint32 Check_Val,
    const Can_17_McmCan_TimeoutCheckType CheckStatus);

    /* Function for Driver state check */
static Std_ReturnType Can_17_McmCan_lMainDetUninitConfig(
    const uint8 ServiceID,
    const uint8 CoreIndex);

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/* Function for validate the requested configuration */
static Std_ReturnType Can_17_McmCan_lInitDetCheck(
    const Can_17_McmCan_ConfigType *const Config,
    const uint8 CoreIndex);

/* Function to validate the Initialization status of the specific core */
static Std_ReturnType Can_17_McmCan_lDetUninitConfig(
    const uint8 ServiceID,
    const uint8 CoreIndex);
/* Function for mode change request validation */
static Std_ReturnType Can_17_McmCan_lModeDetCheck(
    const uint8 Controller,
    const Can_StateTransitionType Transition,
    const Can_17_McmCan_CoreStatesType* const CanCoreState);

/* Function for validate the requested controller id */
static Std_ReturnType Can_17_McmCan_lDetParamController(
    const uint8 CoreIndex,
    const uint8 Controller,
    const uint8 ServiceID);

/* Function for Write request validation */
static Std_ReturnType Can_17_McmCan_lWriteDetCheck(
    const Can_HwHandleType Hth,
    const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

/* Function to check validity of the Message ID passed for a write function */
static Std_ReturnType Can_17_McmCan_lWriteIdCheck(
    const Can_HwHandleType Hth,
    const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig);

#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
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
/* MISRA2012_RULE_20_1_JUSTIFICATION: Memmap header usage as per Autosar
guideline. */
/* MISRA2012_RULE_4_10_JUSTIFICATION: Memmap header is repeatedly included
without safegaurd. It complies to Autosar guidelines. */
#include "Can_17_McmCan_MemMap.h"

/*******************************************************************************
**                      Private Constant Definitions                          **
*******************************************************************************/
#if (CAN_17_MCMCAN_CORE0_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore0 [CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER * \
                                        CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];

#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore0[CAN_17_MCMCAN_CORE0_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore0[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE0_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core0State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore0[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore0[0],
  /* Can controller mode */
  &CanControllerModeCore0[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore0[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore0[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore0[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore0[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore0[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore0[0],
  /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore0[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore0[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore0[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore0[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE0_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE0_ACTIVATION == STD_ON) */

#if (MCAL_NO_OF_CORES > 1U)
#if (CAN_17_MCMCAN_CORE1_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER * \
     CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore1[CAN_17_MCMCAN_CORE1_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore1[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE1_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core1State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore1[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore1[0],
  /* Can controller mode */
  &CanControllerModeCore1[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore1[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore1[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore1[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore1[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore1[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore1[0],
  /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore1[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore1[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore1[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore1[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE1_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE1_ACTIVATION == STD_ON) */
#endif /* #if (MCAL_NO_OF_CORES > 1U) */

#if (MCAL_NO_OF_CORES > 2U)
#if (CAN_17_MCMCAN_CORE2_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER * \
     CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore2[CAN_17_MCMCAN_CORE2_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore2[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE2_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core2State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore2[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore2[0],
  /* Can controller mode */
  &CanControllerModeCore2[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore2[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore2[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore2[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore2[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore2[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore2[0],
  /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore2[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore2[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore2[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore2[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE2_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE2_ACTIVATION == STD_ON) */
#endif /* #if (MCAL_NO_OF_CORES > 2U) */

#if (MCAL_NO_OF_CORES > 3U)
#if (CAN_17_MCMCAN_CORE3_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER * \
     CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore3[CAN_17_MCMCAN_CORE3_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore3[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE3_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core3State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore3[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore3[0],
  /* Can controller mode */
  &CanControllerModeCore3[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore3[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore3[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore3[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore3[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore3[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore3[0],
    /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore3[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore3[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore3[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore3[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE3_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE3_ACTIVATION == STD_ON) */
#endif /* #if (MCAL_NO_OF_CORES > 3U) */

#if (MCAL_NO_OF_CORES > 4U)
#if (CAN_17_MCMCAN_CORE4_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER * \
     CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore4[CAN_17_MCMCAN_CORE4_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore4[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE4_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core4State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore4[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore4[0],
  /* Can controller mode */
  &CanControllerModeCore4[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore4[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore4[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore4[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore4[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore4[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore4[0],
  /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore4[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore4[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore4[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore4[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE4_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE4_ACTIVATION == STD_ON) */
#endif /* #if (MCAL_NO_OF_CORE4 > 1U) */

#if (MCAL_NO_OF_CORES > 5U)
#if (CAN_17_MCMCAN_CORE5_ACTIVATION == STD_ON)
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
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
/* Can controller mode */
static CanIf_ControllerModeType \
CanControllerModeCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* Variable to store the Transmit buffer Mask */
static uint32 CanTxMaskCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
static uint32 BackupCanTxMaskCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable stores status of whether interrupts are enabled for
*  the controllers associated for the core */
static uint8 CanInterruptStatusCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable stores temperory status of whether interrupts are
* enabled for the controller of the core*/
static uint8 CanInterruptTempStatusCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable stores count value of disable Interrupt called for the
controller in the core */
static uint8 CanDisableIntrpCountCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* Used to store swPduHandle passed by CanIf */
static PduIdType CanSwObjectHandleCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER * \
     CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];

#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/* Variable used to hold the order of transmit that have occured */
static uint16 CanTxSwObjPriorityCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER * \
  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE];
#endif

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* This variable stores the wake on bus off status on ICOM configuration */
static boolean CanIcomConfgBOStatusCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable stores the Pretended network support status */
static boolean CanIcomEnableStatusCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable if SET will block the Can_Write API indicating that ICOM
  enable is in progress and filters are getting updated */
static boolean CanIcomBlockWriteCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* This variable used to stores the current ICOM Configuration ID */
static uint8 CanActiveIcomCfgIdCore5[CAN_17_MCMCAN_CORE5_NOOF_CONTROLLER];
/* ICOM Message counter counter value */
static uint16 CanIcomMsgCntrValCore5[CAN_17_MCMCAN_NOOF_ICOM_MSGCONFIGURATIONS];
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_CORE5_UNSPECIFIED
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
/* Structure holding core 0 specific CAN states */
static const Can_17_McmCan_CoreStatesType Can_17_McmCan_Core5State =
{
  /* Pointer to the Variable to store the Transmit buffer Mask */
  &CanTxMaskCore5[0],
  /* Pointer to the Variable to store the back-up of transmit buffer, to be restored on Bus-Off */
  &BackupCanTxMaskCore5[0],
  /* Can controller mode */
  &CanControllerModeCore5[0],
  /* This variable stores status of whether interrupts are enabled for
   *  the controllers associated for the core */
  &CanInterruptStatusCore5[0],
  /* This variable stores temperory status of whether interrupts are
   * enabled for the controller of the core*/
  &CanInterruptTempStatusCore5[0],
  /* This variable stores count value of disable Interrupt called for the
  controller in the core */
  &CanDisableIntrpCountCore5[0],
  /* Used to store swPduHandle passed by CanIf */
  &CanSwObjectHandleCore5[0]

  #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  ,
  /* This variable stores the wake on bus off status on ICOM configuration */
  &CanIcomConfgBOStatusCore5[0],
  /* This variable stores the Pretended network support status */
  &CanIcomEnableStatusCore5[0],
  /* Block Can_Write during Icom Configuration */
  &CanIcomBlockWriteCore5[0],
  /* This variable used to stores the current ICOM Configuration ID */
  &CanActiveIcomCfgIdCore5[0],
  /* ICOM Message counter counter value */
  &CanIcomMsgCntrValCore5[0]
  #endif

  #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
       (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
  ,
  /* Variable used to hold the order of transmit that have occured */
  &CanTxSwObjPriorityCore5[0]
  #endif

};
/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE5_UNSPECIFIED
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
#endif /* #if (CAN_17_MCMCAN_CORE5_ACTIVATION == STD_ON) */
#endif /* #if (MCAL_NO_OF_CORES > 5U) */

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_CONST_QM_GLOBAL_UNSPECIFIED
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

/* Variable to hold the pointers to variables holding core specific CAN
states */
static const Can_17_McmCan_CoreStatesType* const \
Can_17_McmCan_GblCoreState[MCAL_NO_OF_CORES] =
{
  #if (CAN_17_MCMCAN_CORE0_ACTIVATION == STD_ON)
  &Can_17_McmCan_Core0State
  #else
  (Can_17_McmCan_CoreStatesType*)0
  #endif
  #if (MCAL_NO_OF_CORES > 1U)
  #if (CAN_17_MCMCAN_CORE1_ACTIVATION == STD_ON)
  , &Can_17_McmCan_Core1State
  #else
  , (Can_17_McmCan_CoreStatesType*)0
  #endif
  #endif
  #if (MCAL_NO_OF_CORES > 2U)
  #if (CAN_17_MCMCAN_CORE2_ACTIVATION == STD_ON)
  , &Can_17_McmCan_Core2State
  #else
  , (Can_17_McmCan_CoreStatesType*)0
  #endif
  #endif
  #if (MCAL_NO_OF_CORES > 3U)
  #if (CAN_17_MCMCAN_CORE3_ACTIVATION == STD_ON)
  , &Can_17_McmCan_Core3State
  #else
  , (Can_17_McmCan_CoreStatesType*)0
  #endif
  #endif
  #if (MCAL_NO_OF_CORES > 4U)
  #if (CAN_17_MCMCAN_CORE4_ACTIVATION == STD_ON)
  , &Can_17_McmCan_Core4State
  #else
  , (Can_17_McmCan_CoreStatesType*)0
  #endif
  #endif
  #if (MCAL_NO_OF_CORES > 5U)
  #if (CAN_17_MCMCAN_CORE5_ACTIVATION == STD_ON)
  , &Can_17_McmCan_Core5State
  #else
  , (Can_17_McmCan_CoreStatesType*)0
  #endif
  #endif
};

#if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
static const uint8 Can_17_McmCan_GblFdDlcConvDB[CAN_17_MCMCAN_SIXTEEN] = \
{0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
#endif

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_CONST_QM_GLOBAL_UNSPECIFIED
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

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
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

/* Initialize status for all cores. */
/* MISRA2012_RULE_8_9_JUSTIFICATION: This 32 bit variable holds the status of
the CAN module accross the different cores hence cannot be placed in
block scope */
static uint32 Can_17_McmCan_InitStatus;
/* Pointer to store configuration set pointer */
static const Can_17_McmCan_ConfigType *Can_17_McmCan_kGblConfigPtr;

/* MISRA2012_RULE_5_1_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_2_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_4_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
/* MISRA2012_RULE_5_5_JUSTIFICATION: External identifiers going beyond 32 chars.
in generated code due to Autosar Naming constraints.*/
#define CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
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
**                      Global Function Definitions                           **
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
** Traceability   : [cover parentID={196A1432-EAB1-461a-BD6C-259784BF6397}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_Init                                **
**                    (                                                       **
**                      const Can_17_McmCan_ConfigType* const Config          **
**                    )                                                       **
**                                                                            **
**  Description      : Driver Module Initialization function                  **
**                    * This function initializes:                            **
**                    * Static variables, including flags                     **
**                    * CAN HW Unit global hardware settings                  **
**                    * Controller specific settings for each CAN Controller  **
**  All CAN Controllers will be in state CANIF_CS_STOPPED after initialization**
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x00                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters(in)   : Config - Pointer to all cores CAN driver configuration **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_Init(const Can_17_McmCan_ConfigType* const Config)
{
  uint8 CoreIndex;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  Std_ReturnType ApiStatus;

  #if ((CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_OFF)&& \
       (CAN_17_MCMCAN_MASTER_CORE_ALLOCATION == STD_ON))
  ApiStatus = E_OK;
  #endif

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={062510C8-FDBF-4251-9197-125FE758220C}]
  [/cover] */
  #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* [cover parentID={CF54DB20-E407-4ec6-863C-107A31F6E3FD}]
  [/cover] */
  /* Do the initialization DET checks */
  ApiStatus = Can_17_McmCan_lInitDetCheck(Config, CoreIndex);
  /* Check all DET is passed */
  /* [cover parentID={9438B1D7-5A49-4b5c-A516-070F22851D4E}]
  Check all DET is passed
  [/cover] */
  if (E_NOT_OK != ApiStatus)
  #endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
  /* [cover parentID={E9841F16-4DE5-48d3-8BA8-3DFEA3C6A1BC}]
  Is it Master Core
  [/cover] */
  if(MCAL_MASTER_COREID == CoreIndex)
    {
      /* Core copy of the master configuration structure */
      Can_17_McmCan_kGblConfigPtr = Config;
      /* Global Initialization Function */
      /* [cover parentID={365D29BD-D7A0-4171-A75D-98441782984B}]
      [/cover] */
      /* [cover parentID={70B9C2F0-4AE1-4940-B3BF-BDFA369B5330}]
      [/cover] */
      ApiStatus = Can_17_McmCan_lGlobalInit(Config);
    }
    #if (CAN_17_MCMCAN_MASTER_CORE_ALLOCATION == STD_OFF)
    else
    #else
    /* [cover parentID={385BB402-BB7C-4728-B54E-62D6C1ABB764}]
    CAN resources is allocated to the master core
    [/cover] */
    if(E_NOT_OK != ApiStatus)
    #endif
    {
      /* Extract the global core state variable */
      CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
      /* Core specific configuration structure extracted */
      CanConfig = Config->CanCoreConfigPtr[CoreIndex];
      /* [cover parentID={AA5CE85F-0158-4c85-80EB-B27C0E43E9D8}]
      [/cover] */
      /* Call core specific Initialization function */
      ApiStatus = Can_17_McmCan_lCoreInit(CanConfig, CanCoreState);
    }
  }
  /* [cover parentID= {42075D10-4075-4c99-BF15-2909396B7001}]
  If all the initial settings are done successfully
  [/cover] */
  /* Check all the initial settings are done successfully */
  if(E_NOT_OK != ApiStatus)
  {
    /* Set with Driver status as READY*/
    /* [cover parentID={01C0A3ED-D171-4cb6-BC94-7C206F04B658}]
    [/cover] */
    Mcal_SetBitAtomic( (sint32*)(void*)&Can_17_McmCan_InitStatus, \
      (sint32) (CoreIndex*CAN_17_MCMCAN_INIT_STATUS_SIZE), \
      CAN_17_MCMCAN_INIT_STATUS_SIZE,(uint32) (CAN_17_MCMCAN_READY) );
  }
  /* [cover parentID=  {58C2AD10-4E15-4607-B445-870A8E9B1F66}]
  [/cover] */
}
#if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON)
/******************************************************************************
** Traceability   : [cover parentID=]                                        **
**                                                                           **
** Syntax           : Std_ReturnType Can_17_McmCan_CheckBaudrate             **
**                    (                                                      **
**                      const uint8 Controller,                              **
**                      const uint16 Baudrate                                **
**                    )                                                      **
**                                                                           **
** Description      : Checks CAN Controller Baudrate                         **
** [/cover]                                                                  **
**                                                                           **
** Service ID       : 0x0e                                                   **
**                                                                           **
** Sync/Async       : Synchronous                                            **
**                                                                           **
** Reentrancy       : Reentrant                                              **
**                                                                           **
** Parameters (in)  : Controller - Associated CAN Controller                 **
**                    Baudrate -  Baudrate to be checked                     **
**                                                                           **
** Parameters (out) : none                                                   **
**                                                                           **
** Return value     : E_OK/E_NOT_OK                                          **
**                                                                           **
******************************************************************************/
Std_ReturnType Can_17_McmCan_CheckBaudrate( const uint8 Controller,
    const uint16 Baudrate )
{
  /* return value variable */
  Std_ReturnType ApiStatus;
  /* Offset for the end index of the baud rate configurations */
  uint16 EndBaudrateCfgIndx;
  /* start index of the baudrate configuration for this controller */
  uint16 StartBaudrateCfgIndx;
  /* Condition to break the while loop */
  uint8 LoopBreak;
  /* calling core index */
  uint8 CoreIndex;
  /* controller index counter */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;

  /* Set with Value ZERO */
  LoopBreak = CAN_17_MCMCAN_ZERO;
  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={D186055C-2A14-4cb0-807C-AD0F41D3B1C8}]
  [/cover] */
  /* [cover parentID={FD38CAC0-3C12-45c0-A196-36A9B59A36C9}]
  [/cover] */
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)

  /* [cover parentID={C7E22275-4959-4b97-829D-5989A492D844}]
  [/cover] */
  /* Check if status is initilaized */
  ApiStatus = Can_17_McmCan_lDetUninitConfig(CAN_17_MCMCAN_SID_CHECKBAUDRATE, \
              CoreIndex);
  /* [cover parentID={7EA31E31-610D-4c99-AE2E-C13DE62B0DD4}]
  Check if status is initilaized
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* Check the requested controllerId is Valid or not */
    /* [cover parentID={282ACAB9-14A6-4054-A4C4-6FD3A7576464}]
    [/cover] */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_CHECKBAUDRATE);
  }
  /* [cover parentID={5149A284-26BF-41e5-8066-942958454C43}]
    If status OK
    [/cover] */
  if(E_NOT_OK != ApiStatus)
  #else
  /* Initialize the local state variable */
  ApiStatus = E_OK;
  #endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Retrieve the kernel index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;

    /* Set Start index */
    StartBaudrateCfgIndx = \
                           CanConfig->CanControllerConfigPtr[ContIndx].CanBaudrateCfgIndx;
    /* Set with Total no of baudrate index */
    EndBaudrateCfgIndx = \
                         CanConfig->CanControllerConfigPtr[ContIndx].CanNoOfBaudrateCfg;
    while((EndBaudrateCfgIndx > CAN_17_MCMCAN_ZERO) && (LoopBreak == \
          CAN_17_MCMCAN_ZERO))
    {
      /* Check if requested baudrate is supported by the controller*/
      /* [cover parentID={B0D9E5F9-6573-4ba7-91A6-8082521508A8}]
      Check if requested baudrate is supported by the controller
      [/cover] */
      if(Baudrate == \
          CanConfig->CanBaudrateConfigPtr[StartBaudrateCfgIndx].CanBaudrateCfg)
      {
        LoopBreak = CAN_17_MCMCAN_ONE;
      }
      else
      {
        /* point to next baud-rate configuration */
        StartBaudrateCfgIndx++;
      }
      /* Decrement the configuration count */
      EndBaudrateCfgIndx--;
    }
    /* [cover parentID={FA6C8D15-9905-411c-ACB5-6F788EA45462}]
       Check Loop break Status
      [/cover] */
    /* Check Loop break Status */
    if(LoopBreak == CAN_17_MCMCAN_ZERO)
    {
      #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)

      /* [cover parentID={A73459B0-A3CC-4fb0-8CF2-845E5128947F}]
      [/cover] */
      /* Report invalid baudrate to DET */
      /* [cover parentID={43C1F68D-897E-419a-9B82-CBCA7A45B834}]
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                      CAN_17_MCMCAN_SID_CHECKBAUDRATE, CAN_17_MCMCAN_E_PARAM_BAUDRATE);
      #endif

      /* Set as not supported */
      /* [cover parentID={2CEB2679-6FEC-4607-B876-896572E1DAA0}]
      [/cover] */
      ApiStatus = E_NOT_OK;
    }
  }

  /* [cover parentID={F1C21B5D-12C0-4efc-8F2F-97530C6E7011}]
  [/cover] */
  return ApiStatus;
}

/*******************************************************************************
** Traceability   : [cover parentID={56D0417E-7E0D-4304-A6C8-90C6613023D9}]   **
**                    [cover parentID={F642EA72-9EA3-4c90-A08B-07B0477EF852}] **
**                                                                            **
**  Syntax           : Std_ReturnType Can_17_McmCan_SetBaudrate               **
**                    (                                                       **
**                     const uint8 Controller,                                **
**                     const uint16 BaudRateConfigID                          **
**                    )                                                       **
**                                                                            **
**  Description      : This service shall set the baud rate configuration of  **
**                    the CAN controller.                                     **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x0f                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                    BaudRateConfigID - References a baud rate configuration **
**                    by ID                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
Std_ReturnType Can_17_McmCan_SetBaudrate(const uint8 Controller,
                                         const uint16 BaudRateConfigID)
{
  /* return value variable */
  Std_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* controller index counter */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* [cover parentID={F198BB9F-E919-417e-981B-AAC1306B8793}]
  DET is enabled
  [/cover] */

  /* [cover parentID={A38611DD-E2C1-4901-8712-9DE70BA7A92F}]
  [/cover] */
  ApiStatus = Can_17_McmCan_lDetUninitConfig(CAN_17_MCMCAN_SID_SETBAUDRATE, \
              CoreIndex);
  if(E_NOT_OK != ApiStatus)
  {

    /* [cover parentID={CB0C342F-BD1C-41da-80EB-BAD62381F003}]
    [/cover] */
    /* Check the requested controllerId is Valid or not */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_SETBAUDRATE);
  }
  /* [cover parentID={D9EA81AD-42D5-42f1-A3EC-3683071A1DBB}]
    CAN driver state validity return value
    [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* Retrieve the controller index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;
    /* Check if baudrate is supported/configured */
    /* [cover parentID={832137E8-E200-46db-94F9-DA2575D59A18}]
        Baudrate config ID validity
        [/cover] */
    if(BaudRateConfigID >= Can_17_McmCan_kGblConfigPtr-> \
        CanCoreConfigPtr[CoreIndex]-> \
        CanControllerConfigPtr[ContIndx].CanNoOfBaudrateCfg)
    {
      /* [cover parentID={A62338AA-1D66-476f-974B-C7D1480CDC74}]
      Det error CAN_17_MCMCAN_E_PARAM_BAUDRATE
      [/cover] */

      /* Report invalid baudrate to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_SETBAUDRATE, CAN_17_MCMCAN_E_PARAM_BAUDRATE);
      /* Set return value with NOT_OK */
      ApiStatus = E_NOT_OK;
    }
  }
  if(E_NOT_OK != ApiStatus)
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Retrieve the controller index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;
    /* [cover parentID={1A0D993A-7642-4e1f-B5F4-B13077DC2131}]
    Check Can Controller Mode
    [/cover] */
    if(CANIF_CS_STOPPED != CanCoreState->CanControllerModePtr[ContIndx])
    {
      /* Set return value with NOT_OK */
      ApiStatus = E_NOT_OK;
    }
    else
    {
      /* [cover parentID={5315F5C7-9322-4d21-86F3-278A00D47848}]
      Set Baud rate of requested controller
      [/cover] */
      /* Set Baudrate of requested controller */
      ApiStatus = \
                  Can_17_McmCan_lSetBaudrate(ContIndx, BaudRateConfigID, CanConfig);
    }
  }

  /* [cover parentID={3D4BD778-D2BE-4a68-A93C-D09E2D8B991A}]
  Merge
  [/cover] */

  /* [cover parentID={BAFDCD11-604E-4f70-90CD-02973F4FF4A0}]
  Return E_NOT_OK
  [/cover] */
  return ApiStatus;
}
#endif

/*******************************************************************************
** Traceability   : [cover parentID={10315407-9F74-45ef-AC03-36E1505C5B44}]   **
**                                                                            **
**  Syntax           : Can_ReturnType Can_17_McmCan_SetControllerMode         **
**                    (                                                       **
**                      const uint8 Controller,                               **
**                      const Can_StateTransitionType Transition              **
**                    )                                                       **
**                                                                            **
**  Description      : Performs software triggered state transitions of the   **
**                    CAN Controller State machine                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x03                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                    Transition - Requested transition                       **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : CAN_OK/CAN_NOT_OK                                      **
**                                                                            **
*******************************************************************************/
Can_ReturnType Can_17_McmCan_SetControllerMode(const uint8 Controller,
                                    const Can_StateTransitionType Transition)
{

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Det check status */
  Std_ReturnType DetStatus;
  #endif /*   #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

  /* return value variable */
  Can_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={821ED450-4890-4928-9090-EFE686932D7E} ]
  [/cover] */
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Initialize the return value */
  ApiStatus = CAN_OK;
  /* Check if status is initilaized */
  /* [cover parentID={3FE88B0C-772C-46ee-87AC-94B85C095401}]
  If status is OK
  [/cover] */
  DetStatus = \
              Can_17_McmCan_lDetUninitConfig(CAN_17_MCMCAN_SID_SETCONTROLLERMODE, \
                  CoreIndex);
  /* [cover parentID={F38CB961-6522-481d-A821-EB9EF81360FA}]
  If status is OK
  [/cover] */

  if(E_NOT_OK != DetStatus)
  {
    /* Check the requested controllerId is Valid or not */
    /* [cover parentID={803E0AD4-7434-4572-AF72-1CBACBD675CF}]
    If status is OK
    [/cover] */
    DetStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_SETCONTROLLERMODE);
  }
  /* [cover parentID={559A1581-72F9-42c0-BBDE-9E79B2B2AEB4}]
  If status is OK
  [/cover] */
  if(E_NOT_OK != DetStatus)
  {
    /* Check if requested state transition is valid */
    DetStatus = Can_17_McmCan_lModeDetCheck(Can_17_McmCan_kGblConfigPtr-> \
                                            CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex, Transition, \
                                            Can_17_McmCan_GblCoreState[CoreIndex]);
  }
  /* [cover parentID={52252E66-3284-4924-84EA-F9DB982C1B71}]
  If status is OK
  [/cover] */

  if(E_OK != DetStatus)
  {
    ApiStatus = CAN_NOT_OK;
  }
  /* [cover parentID={259AD625-9E64-4406-BEE2-9AB9C820208E}]
  If status is OK
  [/cover] */
  if(CAN_NOT_OK != ApiStatus)
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Retrieve the controller index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;
    /* Perform state transition If Wakeup is Disabled */
    ApiStatus = Can_17_McmCan_lSetModeWuSupportOff(ContIndx, Transition,
                CanCoreState, CanConfig);
    /* Check The Hw Register is set properly with requested State */
    /* [cover parentID={902AB1F9-A872-4db0-8FA1-8B253B54061B}]
    If status is OK
    [/cover] */
    if( CAN_OK == ApiStatus)
    {
      /* Notify successful state transition to upper layer */
      CanIf_ControllerModeIndication(Controller,
                                     CanCoreState->CanControllerModePtr[ContIndx]);
    }
  }
  /* [cover parentID={4E86AD1C-4F26-4fd5-8EFA-ACBA57DF3C91}]
  Return
  [/cover] */
  /* return Api status */
  return ApiStatus;
}

/*******************************************************************************
** Traceability   : [cover parentID={88889393-8A52-4a81-98E6-A7FF930AC5F0}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_DisableControllerInterrupts         **
**                    (                                                       **
**                      const uint8 Controller                                **
**                    )                                                       **
**                                                                            **
**  Description      : Disables CAN Controller all Interrupts                 **
**                                                                            **
**  Service ID       : 0x04                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_DisableControllerInterrupts(const uint8 Controller)
{

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Det check status */
  Std_ReturnType ApiStatus;
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* [cover parentID={C8563E01-AACB-4ffe-B54C-110D5FD7D7FB}]
  DET is enabled
  [/cover] */
  /* Check if status is initilaized */
  ApiStatus = Can_17_McmCan_lDetUninitConfig( \
              CAN_17_MCMCAN_SID_DISABLECONTROLLERINTERRUPTS, CoreIndex);
  /* [cover parentID={A213EC1C-7237-4112-BB1C-11D56EF1C794}]
  if status ok
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* [cover parentID={C8BADA98-6FC9-49de-B61C-9EEF1AC324B6}]
    Check requested controller is valid
    [/cover] */
    /* Check if requested Controller is valid */
    /* [cover parentID={19E68920-CE83-4a93-B207-1C6CF6A7BEB5}]
    [/cover] */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_DISABLECONTROLLERINTERRUPTS);
  }
  /* [cover parentID={EDF457F7-0A35-4206-A16D-AB2EE6656214}]
  Controller validity return value
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Retrieve the controller index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;

    /* Disable controller interrupts */
    Can_17_McmCan_lDisableInterrupts(ContIndx, CanConfig, CanCoreState);
  }
  /* [cover parentID={FA8016DA-C19C-445d-AB73-2C5FBAD88044}]
  Merge
  [/cover] */
}

/*******************************************************************************
** Traceability   : [cover parentID={62DE3D9D-C847-4f77-B002-0EC887A129AB}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_EnableControllerInterrupts          **
**                    (                                                       **
**                      const uint8 Controller                                **
**                    )                                                       **
**                                                                            **
**  Description      : Enable CAN Controller all Interrupts                   **
**                                                                            **
**  Service ID       : 0x05                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_EnableControllerInterrupts(const uint8 Controller)
{

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Det check status */
  Std_ReturnType ApiStatus;
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={ADCCC99D-104C-413e-A4A7-F83EAD748A14}]
  [/cover] */
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* [cover parentID={BFAB4B15-FC23-4043-A1CC-3028D65120E4}]
  [/cover] */
  ApiStatus = Can_17_McmCan_lDetUninitConfig( \
              CAN_17_MCMCAN_SID_ENABLECONTROLLERINTERRUPTS, CoreIndex);
  /* [cover parentID={A69F5DC2-D3AF-4f9f-AD54-25854B5F177D}]
  if status ok
  [/cover] */

  if(E_NOT_OK != ApiStatus)
  {
    /* [cover parentID={920BFF81-A655-4e2d-ADF6-0223524D241A}]
    [/cover] */
    /* Check if requested Controller is valid */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_ENABLECONTROLLERINTERRUPTS);
  }
  /* [cover parentID={CBA1EB3E-CF16-4216-BFDD-E98DED68884F}]
  Controller validity return value
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Retrieve the controller index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;
    /* Enable Controller Interrupts */
    Can_17_McmCan_lEnableInterrupts(ContIndx, CanConfig, CanCoreState);
  }
  /* [cover parentID={1D8D8E21-E76D-43e6-95C5-6F15FA346884}]
  End
  [/cover] */
}

/*******************************************************************************
** Traceability   : [cover parentID={CFC67599-E6B0-4ecd-9618-5CF8729B0119}]   **
**                                                                            **
**  Syntax           : Can_ReturnType Can_17_McmCan_Write                     **
**                    (                                                       **
**                      const Can_HwHandleType Hth,                           **
**                      const Can_PduType* const PduInfo                      **
**                    )                                                       **
**                                                                            **
**  Description      : Service to transmit CAN frame                          **
**                                                                            **
**  Service ID       : 0x06                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant(thread-safe)                                 **
**                                                                            **
**  Parameters (in)  : Hth - Hardware Transmit Handler                        **
**                    PduInfo - Pointer to SDU user memory, DLC and           **
**                                                            Identifier      **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : CAN_OK/CAN_BUSY/CAN_NOT_OK                             **
**                                                                            **
*******************************************************************************/
Can_ReturnType Can_17_McmCan_Write(const Can_HwHandleType Hth,
                                   const Can_PduType* const PduInfo )
{

  Can_HwHandleType HthIndex;
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Det check status */
  Std_ReturnType DetStatus;
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

  /* Return value */
  Can_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={669C3608-B3D7-46e4-B0CF-03172B27DBA2}]
  DET is enabled
  [/cover] */
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Check if status is initilaized */
  /* [cover parentID={59F909E8-43A1-4e05-97B1-32292623FBB5}]
    [/cover] */
  DetStatus = Can_17_McmCan_lDetUninitConfig(CAN_17_MCMCAN_SID_WRITE, CoreIndex);
  /* [cover parentID= {21AC1D8B-EDDA-46a9-8274-57D84FB89D2B}]
  CAN driver state validity return value
  [/cover] */
  if(E_NOT_OK != DetStatus)
  {
    /* Check for DET */
    /* [cover parentID={0F940541-3270-469e-B139-296203E6E2DD}]
    [/cover] */
    DetStatus = Can_17_McmCan_lWriteDetCheck(Hth, PduInfo, \
                Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr\
                [CoreIndex]);
  }
  /* [cover parentID={A79E8D0E-23BE-4e0a-906F-1B1E71AF48EB}]
  Return value of CAN Pdu and Hth chack against CAN config
  [/cover] */
  if (E_OK != DetStatus)
  {
    /* Request with invalid input ,Set return value with E_NOT_OK */
    ApiStatus = CAN_NOT_OK;
  }
  else
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Extract HTH index for accessing corresponding configuration */
    HthIndex = Can_17_McmCan_kGblConfigPtr-> \
               CanHthIndexPtr[Hth - (Can_17_McmCan_kGblConfigPtr->CanNoOfHrh)].\
               CanHthCoreSpecIndex;
    /* Call function for write message into Tx buffer and request for
    Transmission */
    ApiStatus = Can_17_McmCan_lWriteMsgObj(HthIndex, PduInfo, CanCoreState, \
                                           CanConfig);
  }


  /* [cover parentID={B161C701-918C-4c4b-BF4C-3CC4AC6B175A}]
  Return
  [/cover] */
  return ApiStatus;
}

/*******************************************************************************
            Scheduled functions
*******************************************************************************/
#if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF)
/*******************************************************************************
** Traceability   : [cover parentID={0DFEBF6A-E273-4601-ABA3-0FD3C5635523}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_MainFunction_Write                  **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
**  Description      : This function performs the polling of TX confirmation  **
**                    when CAN_TX_PROCESSING is set to POLLING                **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x01                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_MainFunction_Write( void )
{
  /* [cover parentID={F9B21214-AA72-4f21-8B95-878CA54AFC31}]
  Tx Polling Process is Enabled
  [/cover] */
  #if(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)

  /* Det check status */
  Std_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 CtrlIndex;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={9620144A-4BA4-458c-A9FD-09C720111CBA}]
  [/cover] */
  /* Check if status is initilaized */
  ApiStatus = \
              Can_17_McmCan_lMainDetUninitConfig(CAN_17_MCMCAN_SID_MAINFUNCTIONWRITE, \
                  CoreIndex);
  /* [cover parentID={B9CB4235-6229-4d60-B124-6703ABC6C513}]
    If status is Ok
    [/cover] */
  if(E_OK == ApiStatus)
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Call transmit handler function for each of the CAN controllers
    configured as POLLING mode for transmission successful event handling */
    for(CtrlIndex = CAN_17_MCMCAN_ZERO; CtrlIndex < CanConfig->CanCoreContCnt; \
        CtrlIndex++)
    {
      /* [cover parentID={64AC5815-F17B-4ca1-B5FD-E89F51EF99DF}]
      Requested controller transmit event handling is of polling type
      and the controller mode is STARTED
      [/cover] */
      /* Check the Tx processing is configured as POLLING */
      if((CAN_17_MCMCAN_POLLING == \
          CanConfig->CanEventHandlingConfigPtr[CtrlIndex].CanEventType \
          [CAN_17_MCMCAN_ZERO]) \
          && (CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CtrlIndex]))
      {
        /* [cover parentID={CAD339A2-658C-4155-96B9-AAB45420B169}]
        Invoke Transmit Handler
        [/cover] */
        /* [cover parentID={214D632C-55DF-4d73-9AC6-3350D140EEBB}]
        Invoke Transmit Handler
        [/cover] */
        if(NULL_PTR != CanConfig->CanTxHwObjectConfigPtr)
        {
          /* Invoke internal function to process transmit confirmation */
          Can_17_McmCan_lTxEventHandler(CtrlIndex, CanCoreState, CanConfig);
        }
      }
    }
  }
  #endif  /* #if(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON) */
  /* [cover parentID={CE488F2F-F189-447d-9D1F-1C6737067BFA}]
  [/cover] */

}
#endif /* #if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF) */


#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF)
/*******************************************************************************
** Traceability   : [cover parentID={4DF70F32-AF8B-4771-8288-7B236390D1C3}]   **
**                  [cover parentID={45061A30-5EB8-46cf-9681-9EAB76965D54}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_MainFunction_Read                   **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
**  Description      : This function performs the polling of RX indication    **
**                    when CAN_RX_PROCESSING is set to POLLING                **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x08                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_MainFunction_Read( void )
{
  /* Det check status */
  Std_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 CtrlIndex;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Ifx_CAN_N *NodeRegAddressPtr;
  Ifx_UReg_32Bit RxFxwm;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* Check if status is initilaized */
  /* [cover parentID={400B50C4-D748-462a-AB38-3389AE8A0108}]
  [/cover] */
  ApiStatus = Can_17_McmCan_lMainDetUninitConfig(  \
              CAN_17_MCMCAN_SID_MAINFUNCTIONREAD, CoreIndex);
  /* [cover parentID={59028030-EDEC-4e3f-A516-5BD3CB012AD0}]
      CAN driver state validity return value
      [/cover] */
  if(E_OK == ApiStatus)
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Call transmit handler function for each of the CAN controllers
     configured as POLLING mode for transmission successful event handling */
    for(CtrlIndex = CAN_17_MCMCAN_ZERO; CtrlIndex < CanConfig->CanCoreContCnt; \
        CtrlIndex++)
    {
      /* Read the exact Node Start address */
      NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[CtrlIndex].CanNodeAddress;

      /* [cover parentID={D4F9DE02-7644-43b3-A022-B65E05081A04}]
      Requested controller receive event handling is of polling type and
      the controller mode is STARTED
      [/cover] */
      /* Check the Rx processing is configured as POLLING */
      /* [cover parentID={09DEBD46-C667-4683-A75D-2BD1B3457541}]
      The controller mode is STARTED
      [/cover] */
      if(CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CtrlIndex])
      {
        /* Poll all 3, Rx Dedicated, Rx FIFO - 0, Rx FIFO - 1 */
        if(NodeRegAddressPtr->IE.B.DRXE == 0U)
        {
           Can_17_McmCan_lReceiveHandler(CtrlIndex, CAN_17_MCMCAN_ZERO, \
                                      CanCoreState, CanConfig);
        }
        else
        {
           /* Poll for FIFO - 0 */
           /* [cover parentID={F66F907A-2859-4b8f-AB3F-4F1FB191CAB4}]
           Check if FIFO 0 is not empty for polling
           [/cover] */
           if(NodeRegAddressPtr->RX.F0S.B.F0FL != 0U)
           {
             RxFxwm = NodeRegAddressPtr->RX.F0C.B.F0WM;
             /* [cover parentID={6CCB7D42-6824-4f20-89C0-02C8A834FEF8}]
             Check if the length of FIFO0 is greater than watermark length
             [/cover] */
             if(NodeRegAddressPtr->RX.F0S.B.F0FL >= RxFxwm)
             {
               /* [cover parentID={B6EC69B6-2A59-4489-9B93-0EFEEE65D9C0}]
               Invoke Receive Handler
               [/cover] */
               Can_17_McmCan_lReceiveHandler(CtrlIndex, CAN_17_MCMCAN_TWO, \
                                      CanCoreState, CanConfig);
             }
           }
           /* Poll for FIFO - 1 */
           /* [cover parentID={93976A4E-B5EC-4f81-A0E5-BD78E42C6B25}]
           Check if FIFO 1 is not empty for polling
           [/cover] */
           if(NodeRegAddressPtr->RX.F1S.B.F1FL != 0U)
           {
             RxFxwm = NodeRegAddressPtr->RX.F1C.B.F1WM;
             /* [cover parentID={D912A681-C229-43f7-9509-21BC8EC943C7}]
             Check if the length of FIFO1 is greater than watermark length
             [/cover] */
             if(NodeRegAddressPtr->RX.F1S.B.F1FL >= RxFxwm)
             {
               Can_17_McmCan_lReceiveHandler(CtrlIndex, CAN_17_MCMCAN_THREE, \
                                      CanCoreState, CanConfig);
             }
           }
        }
      }
    }
  }
}
/* [cover parentID={3C597AFE-641B-4faf-9A93-B4FD81C2BF09}]
End
[/cover] */
#endif /* #if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF) */

/*******************************************************************************
**                                                                            **
** Traceability   : [cover parentID={6E7658DE-1E4B-4468-97D1-869C21AB0495}]   **
**  Syntax           : void Can_17_McmCan_MainFunction_BusOff                 **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
**  Description      : This function performs the polling of bus-off events   **
**                    that are configured statically as 'to be polled'        **
**                                                                            **
**  Service ID       : 0x09                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_MainFunction_BusOff(void)
{

  #if(CAN_17_MCMCAN_BO_POLLING_PROCESSING == STD_ON)

  /* Det check status */
  Std_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 CtrlIndex;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={EEDF2035-D93E-42e7-925F-8E326D2F5E1D}]
  [/cover] */
  /* Check if status is initilaized */
  ApiStatus = Can_17_McmCan_lMainDetUninitConfig( \
              CAN_17_MCMCAN_SID_MAINFUNCTIONBUSOFF, \
              CoreIndex);
  /* [cover parentID={7C472617-CF79-4a95-AEB7-8ECD7E351F63}]
  CAN driver state validity return value
  [/cover] */
  if(E_OK == ApiStatus)
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Call bus-off handler function for each of the CAN controllers
    configured as POLLING mode for bus-off event handling */
    for(CtrlIndex = CAN_17_MCMCAN_ZERO; CtrlIndex < CanConfig->CanCoreContCnt; \
        CtrlIndex++)
    {
      /* [cover parentID={B735E851-C542-4855-8618-2162C8222243}]
      Bus Off Polling Process is Enabled
      [/cover] */
      /* [cover parentID={9186D66E-F8BC-4552-8670-4CEC4AB50F09}]
      Requested controller bus off event handling is of polling type
      [/cover] */
      /* Check the BusOff processing mode as Polling */
      if(CAN_17_MCMCAN_POLLING == CanConfig-> \
          CanEventHandlingConfigPtr[CtrlIndex].CanEventType[CAN_17_MCMCAN_TWO])
      {
        /* [cover parentID={71D2608D-D970-4403-ACA2-7BEC7AA154D6}]
        ICOM Bus off is enabled
        [/cover] */
        #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
        /* [cover parentID={89208869-D38A-440d-92EF-90E600599406}]
        ICOM is turned ON?
        [/cover] */
        if((TRUE != CanCoreState->CanIcomEnableStatusPtr[CtrlIndex]) || \
            (FALSE != CanCoreState->CanIcomConfgBOStatusPtr[CtrlIndex]))
        {
          /* Invoke Bus-Off Handler */
          /* [cover parentID={A26950E9-D247-4c48-A43B-4860963EA081}]
          Invoke Bus Off Handler
          [/cover] */
          Can_17_McmCan_lBusOffHandler(CtrlIndex, CanCoreState, CanConfig);
        }
        #else

        /* Invoke Bus-Off Handler */
        /* [cover parentID={A26950E9-D247-4c48-A43B-4860963EA081}]
        Invoke Bus Off Handler
        [/cover] */
        Can_17_McmCan_lBusOffHandler(CtrlIndex, CanCoreState, CanConfig);
        #endif
      }
    }
  }
  /* [cover parentID={3780E7F2-F932-46a9-B40B-A12FA86C2673}]
  [/cover] */
  #endif /* #if(CAN_17_MCMCAN_BO_POLLING_PROCESSING == STD_ON) */

}

/*******************************************************************************
** Traceability   : [cover parentID={185859A3-F84D-408d-88DB-0D4A648E8849}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_MainFunction_Wakeup                 **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
**  Description      : This function performs the polling of wake-up events   **
**                    that are configured statically as 'to be polled'        **
**                                                                            **
**  Service ID       : 0x0A                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_MainFunction_Wakeup(void)
{
  /* [cover parentID={2466E081-CEB3-4c01-B071-1D65E3DF9782}]
  Wake Up Polling Process is Enabled
  [/cover] */
  #if((CAN_17_MCMCAN_WU_POLLING_PROCESSING == STD_ON) && \
  (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON))

  /* Det check status */
  Std_ReturnType ApiStatus;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 CtrlIndex;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={6125D0DC-2FDD-4991-A2E4-12E731FFA668}]
  Check if status is initilaized
  [/cover] */
  ApiStatus = Can_17_McmCan_lMainDetUninitConfig( \
              CAN_17_MCMCAN_SID_MAINFUNCTIONWAKEUP, CoreIndex);
  /* [cover parentID={D819E02B-E79C-4576-B9D5-79398475ADE6}]
  CAN driver state validity return value
  [/cover] */
  if(E_OK == ApiStatus)
{
  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Call wakeup handler function for each of the CAN controllers
    configured as POLLING mode for wakeup event handling */
    for(CtrlIndex = CAN_17_MCMCAN_ZERO; CtrlIndex < CanConfig->CanCoreContCnt; \
        CtrlIndex++)
    {
      /* [cover parentID={7A953952-FFF4-4ea7-8068-05D4964FF7B2}]
      [/cover] */
      /* Check if the controller is activated with PN mode */
      if((CAN_17_MCMCAN_POLLING == \
          CanConfig->CanEventHandlingConfigPtr[CtrlIndex].CanEventType \
          [CAN_17_MCMCAN_THREE]) \
          && (TRUE == CanCoreState->CanIcomEnableStatusPtr[CtrlIndex]))
      {
        /* Invoke function for check the receive message wakeup events */
        Can_17_McmCan_lReceiveHandler(CtrlIndex, CAN_17_MCMCAN_ZERO, \
                                      CanCoreState, CanConfig);
      }
    }
  }
  #endif /* #if((CAN_17_MCMCAN_WU_POLLING_PROCESSING == STD_ON) && \
  (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)) */

}

/*******************************************************************************
** Traceability   : [cover parentID={2A250146-E646-440d-868C-6AA4892FCD9E}]   **
**  Syntax           : void Can_17_McmCan_MainFunction_Mode                   **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
**  Description      : This function performs the polling of CAN Controller   **
**                    mode transitions                                        **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x0C                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_MainFunction_Mode(void)
{
  /* The mode indication */
}

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/* [cover parentID={FC11D271-8E0A-4c0c-B069-BF3771D2B5E4}]
Can_17_McmCan_SetIcomConfiguration
[/cover] */
/*******************************************************************************
** Traceability   : [cover parentID={B6D12824-46DD-40e0-9479-46512A0EC7D1}]   **
**                                                                            **
**  Syntax           : Std_ReturnType Can_17_McmCan_SetIcomConfiguration      **
**                   (                                                        **
**                     const uint8 Controller,                                **
**                     const IcomConfigIdType ConfigurationId                 **
**                   )                                                        **
**                                                                            **
**  Description      : This service shall change the Icom Configuration of a  **
**                    CAN controller to the requested one.                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x10                                                   **
**                                                                            **
**  Sync/Async       : Asynchronous                                           **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                    ConfigurationId  - Requested Configuration              **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
Std_ReturnType Can_17_McmCan_SetIcomConfiguration (const uint8 Controller,
    const IcomConfigIdType ConfigurationId)
{
  /* Det check status */
  Std_ReturnType ApiStatus;
  /* Icom error variable */
  IcomSwitch_ErrorType IcomError;
  /* calling core index */
  uint8 CoreIndex;
  /* Core specific controller index */
  uint8 ContIndx;
  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* core specifc global variable structure */
  const Can_17_McmCan_CoreStatesType* CanCoreState;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* [cover parentID={C449A2F4-7C94-4b2e-8139-812F8302A685}]
  [/cover] */
  #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Check if status is initilaized */
  ApiStatus = Can_17_McmCan_lDetUninitConfig( \
              CAN_17_MCMCAN_SID_SETICOMCONFIGURATION, CoreIndex);
  if(E_NOT_OK != ApiStatus)
  {
    /* Check the requested controllerId is Valid or not */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, Controller, \
                CAN_17_MCMCAN_SID_SETICOMCONFIGURATION);
  }
  /* Check if configuration ID is valid */
  /* [cover parentID={BAF66F05-4975-445c-AD7C-10C5B469BACF}]
    Check if configuration ID is valid
    [/cover] */
  if((ConfigurationId > CAN_17_MCMCAN_NOOF_ICOM_CONFIGURATIONS) && \
      (E_NOT_OK != ApiStatus))
  {
    /* [cover parentID={59065786-EA5E-4b5e-AD12-EC2B58EC2601}]
    Det error CAN_17_MCMCAN_E_ICOM_CONFIG_INVALID
    [/cover] */
    /* Report invalid  configuration ID to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_SETICOMCONFIGURATION, CAN_17_MCMCAN_E_ICOM_CONFIG_INVALID);
    ApiStatus = E_NOT_OK;
  }
  if (E_OK == ApiStatus)
  #else
  /* Initializing ApiStatus */
  ApiStatus = E_OK;
  #endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Retrieve the kernel index offset for the core */
    ContIndx = Can_17_McmCan_kGblConfigPtr-> \
               CanLogicalControllerIndexPtr[Controller].CanLCoreSpecContIndex;
    /* Check the CAN controller status*/
    /* [cover parentID={3353F6BF-53BF-458b-B197-19076CAE705B}]
    Controller mode is STARTED and ICOM config pointer is not a NULL_PTR
    [/cover] */
    if ((CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[ContIndx]) && \
        (NULL_PTR != Can_17_McmCan_kGblConfigPtr->CanIcomConfigPtr))
    {
      if (CAN_17_MCMCAN_ZERO <(uint8) ConfigurationId)
      {
        /* Block Can_Writes once the I-com API is accepted */
        CanCoreState->CanIcomBlockWriteCalls[ContIndx] = TRUE;

        /* [cover parentID={CFCE76D6-AEB0-455c-AC11-70B8E42CF4F2}]
        Activate the ICOM settings
        [/cover] */
        /* Activate ICOM settings*/
        Can_17_McmCan_lActivateIcom(ContIndx,ConfigurationId,CanCoreState, \
        CanConfig);
        /* [cover parentID={32A6F62D-2135-4f7c-9FB5-A200EB3C139D}]
        Set ICOM status to ICOM_SWITCH_E_OK
        [/cover] */
        /* Set IcomError as ICOM_SWITCH_E_OK */
        IcomError = ICOM_SWITCH_E_OK;
      }
      /* [cover parentID={42EBE452-1686-4593-A7DB-DAF5AA9D9C97}]
      ICOM configuration ID passed is ZERO
      [/cover] */
      /* [cover parentID={675AECD8-F1E6-40e7-8F0B-F4DD4736F9D8}]
      Is Icom Activated
      [/cover] */
      /* Check the configuration ID */
      else if (FALSE != CanCoreState->CanIcomEnableStatusPtr[Controller])
      {
        /* [cover parentID={FC1E24F4-75FC-45d9-83B5-934309C2B206}]
        Deactivate the ICOM settings
        [/cover] */
        /* De Activate ICOM settings*/
        Can_17_McmCan_lDeactivateIcom(ContIndx, ConfigurationId, CanCoreState, \
                                      CanConfig);
        /* [cover parentID={32A6F62D-2135-4f7c-9FB5-A200EB3C139D}]
        Set ICOM status to ICOM_SWITCH_E_OK
        [/cover] */
        /* Set IcomError as ICOM_SWITCH_E_OK */
        IcomError = ICOM_SWITCH_E_OK;
      }
      else
      {
        /* Set ApiStatus as E_NOT_OK */
        ApiStatus = E_NOT_OK;
        /* [cover parentID={67D050FB-2C63-4d4b-8E20-178756F7167C}]
        Set ICOM status as ICOM_SWITCH_E_FAILED
        [/cover] */
        /* Set IcomError as ICOM_SWITCH_E_FAILED */
        IcomError = ICOM_SWITCH_E_FAILED;
      }
    }
    else
    {
      /* Set ApiStatus as E_NOT_OK */
      ApiStatus = E_NOT_OK;
      /* [cover parentID={67D050FB-2C63-4d4b-8E20-178756F7167C}]
      Set ICOM status as ICOM_SWITCH_E_FAILED
      [/cover] */
      /* Set IcomError as ICOM_SWITCH_E_FAILED */
      IcomError = ICOM_SWITCH_E_FAILED;
    }
    /* [cover parentID={6AF5732D-8D8D-485a-9AAC-BB4BD677B985}]
    Notify the upper layer
    [/cover] */
    /* Notify the upper layer */
    CanIf_CurrentIcomConfiguration(Controller, ConfigurationId, IcomError);
  }
  /* [cover parentID={9716C92B-C90D-4b57-908C-F6B9555BF4FA}]
  Return E_NOT_OK
  [/cover] */
  /* [cover parentID={331EE248-DACF-4c6f-9ED7-D36C4CD22D49}]
  Return
  [/cover] */
  return ApiStatus;
}
#endif

#if (CAN_17_MCMCAN_DEINIT_API == STD_ON)
/*******************************************************************************
** Traceability   : [cover parentID={1720B00B-1498-4297-8799-1D5E2027A61A}]   **
**                                                                            **
**  Syntax           : void Can_17_McmCan_DeInit                              **
**                    (                                                       **
**                     void                                                   **
**                    )                                                       **
**                                                                            **
**  Description      : This function for De-initializes the CAN Driver.       **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : 0x65                                                   **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : none                                                   **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_DeInit (void)
{
  Ifx_CAN *MCMBaseAddressPtr;
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_CLC DataAddress_CLC;
  /* Variable to scan across the configured kernel set  */
  uint8 KernelIndx;
  uint8 CoreIndex;
  #if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  /* Expected Initialization variable status in master core */
  uint32 ExpInitStatus;
  #endif /* #if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;

  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  Std_ReturnType Status;
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* [cover parentID={3D624900-890B-4ed6-ACA8-836B7DFE335B}]
      [/cover] */
  #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
  /* Check if CAN driver is initialized */
  /* [cover parentID={7ABFA6CF-0E88-4d8d-A9E9-CCF95AC2D148}]
      [/cover] */
  Status = Can_17_McmCan_lDetUninitConfig(CAN_17_MCMCAN_SID_DEINIT, CoreIndex);
  #if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  /* Calculation for expected Initialization variable status in master core */
  ExpInitStatus = CAN_17_MCMCAN_REG_ONE << ((uint32)MCAL_MASTER_COREID * \
                  (uint32)CAN_17_MCMCAN_INIT_STATUS_SIZE);
  /* Master core deinit called only after slave core deinit */
  /* [cover parentID={4AA986F4-1A5F-4e3d-8453-223BBD493C10}]
  if the CAN is configured in the core
    [/cover] */
  if((E_NOT_OK != Status) && (MCAL_MASTER_COREID == CoreIndex) && \
      (Can_17_McmCan_InitStatus != ExpInitStatus))
  {
    /* Transition error */
    /* [cover parentID={9D5E628C-2B95-45b1-9A6F-6CB7CD04563E}]
    [/cover] */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_DEINIT, CAN_17_MCMCAN_E_SLAVE_CORE_INIT);
    /* return function call not sucessful*/
    Status = E_NOT_OK;
  }
  #endif /* #if(CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */
  /* [cover parentID={90C3F49A-0148-4af1-B324-51D70A20FF66}]
  if status is ok
    [/cover] */
  if (E_NOT_OK != Status)
  #endif /* #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
  {
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Core specific configuration structure extracted */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Change the driver state from Initialized state */
    /* Set with Driver status as READY*/
    /* [cover parentID={2C4DC05A-1C14-4ca1-A2F1-30C5DC095E2C}]
    [/cover] */
    /* [cover parentID={813EB0F0-5F6A-4c98-AFB4-3AFA80A75353}]
    [/cover] */
    Mcal_SetBitAtomic( (sint32*)(void*)&Can_17_McmCan_InitStatus, \
                       (sint32) (CoreIndex * CAN_17_MCMCAN_INIT_STATUS_SIZE), \
                       CAN_17_MCMCAN_INIT_STATUS_SIZE, (uint32) (CAN_17_MCMCAN_DEINITPENDING) );
    /* Initialize all Global variable with default value */
    /* [cover parentID={367FFAD3-5C04-47d4-B9BA-52B096198AA9}]
    [/cover] */
    #if(CAN_17_MCMCAN_MASTER_CORE_ALLOCATION == STD_OFF)
    /* [cover parentID={46529656-C882-4da8-A239-1E83098ADF66}]
    CAN_17_MCMCAN_MASTER_CORE_ALLOCATION is disabled and core is not master core
    [/cover] */
    if(MCAL_MASTER_COREID != CoreIndex)
    #endif
    {
      Can_17_McmCan_lInitGlobalVar(CanConfig, CanCoreState);
    }
    /* [cover parentID={9B3923C6-8921-44f1-AE95-77B424EFA1E4}]
     If Master Core is selected
    [/cover] */
    if(MCAL_MASTER_COREID == CoreIndex)
    {
      /* [cover parentID={91B0921A-4464-40e5-9283-5F04A4B9B308}]
      [/cover] */
      for (KernelIndx = CAN_17_MCMCAN_ZERO; KernelIndx  \
           < Can_17_McmCan_kGblConfigPtr->CanNoOfKernel; \
           KernelIndx++)
      {
        /* Set with Kernel base address */
        MCMBaseAddressPtr = \
                            Can_17_McmCan_kGblConfigPtr->CanMCMModuleConfigPtr[KernelIndx]. \
                            CanBaseAddress;
        /* Take a copy of the resgiter values */
        DataAddress_CLC.U = MCMBaseAddressPtr->CLC.U;
        /* Set DISR bit in CLC register to Disable the CAN module  */
        DataAddress_CLC.B.DISR = CAN_17_MCMCAN_ONE;
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG( \
            (volatile void*) & (MCMBaseAddressPtr->KRSTCLR.U), CAN_17_MCMCAN_REG_ONE);
        /* [cover parentID={26139B1F-CBB2-4abb-B29F-8109B7D0CE3D}]
        [/cover] */
        /* [cover parentID={F9EBC60A-1176-42b3-BCBA-8FBF6667888D}]
        [/cover] */
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG( \
            (volatile void*) & (MCMBaseAddressPtr->KRST0.U), \
            CAN_17_MCMCAN_REG_ONE);
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG( \
            (volatile void*) & (MCMBaseAddressPtr->KRST1.U), \
            CAN_17_MCMCAN_REG_ONE);
        /* [cover parentID={E16E705C-8A0C-4f31-87B5-049A464BE0C9}]
        [/cover] */
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG( \
            (volatile void*) & (MCMBaseAddressPtr->CLC.U), \
            DataAddress_CLC.U);
      }
      Can_17_McmCan_kGblConfigPtr = NULL_PTR;
    }
    /* Set the status of the core status for CAN */
    Mcal_SetBitAtomic( (sint32*)(void*)&Can_17_McmCan_InitStatus, \
                       (sint32) (CoreIndex * CAN_17_MCMCAN_INIT_STATUS_SIZE), \
                       CAN_17_MCMCAN_INIT_STATUS_SIZE, (uint32) (CAN_17_MCMCAN_UNINIT) );
  }
  /* [cover parentID={FC76F5A5-D3DA-4fc3-8377-C79E8B289951}]
  [/cover] */
}
#endif

#if (CAN_17_MCMCAN_VERSION_INFO_API == STD_ON)
/* [cover parentID={7B39AE21-BE9D-41fb-8064-37A8164B1510}]
Can_17_McmCan_GetVersionInfo
[/cover] */
/*******************************************************************************
** Traceability   : [cover parentID={DFF7807D-69E0-42c4-AE95-F08E2299FC2D}]   **
**                                                                            **
** Syntax           : void Can_17_McmCan_GetVersionInfo                       **
**                    (                                                       **
**                      Std_VersionInfoType* const versioninfo                **
**                    )                                                       **
**                                                                            **
** Description      : - This function returns the version information of this **
**                      module.The version information include : Module ID,   **
**                      Vendor ID,Vendor specific version numbers             **
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
void Can_17_McmCan_GetVersionInfo(Std_VersionInfoType* const versioninfo)
{

  #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON )
  /* [cover parentID={91C85D96-0FA9-4db2-99A7-1C27FD54723E}]
  DET is enabled
  [/cover] */
  /* [cover parentID={CD74AE77-3AD8-48db-9884-410074732021}]
  [/cover] */
  if ((versioninfo) == NULL_PTR)
  {
    /* [cover parentID={07B8D0A0-8243-4145-8BBA-F3CDC1763AA6}]
    Det error CAN_17_MCMCAN_E_PARAM_POINTER
    [/cover] */
    Det_ReportError((uint16)CAN_17_MCMCAN_MODULE_ID,
                    CAN_17_MCMCAN_INSTANCE_ID,
                    CAN_17_MCMCAN_SID_GETVERSIONINFO,
                    CAN_17_MCMCAN_E_PARAM_POINTER
                   );
  }
  else
  #endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON )  */
  {

    ((Std_VersionInfoType*)(versioninfo))->moduleID = CAN_17_MCMCAN_MODULE_ID;
    ((Std_VersionInfoType*)(versioninfo))->vendorID = CAN_17_MCMCAN_VENDOR_ID;
    ((Std_VersionInfoType*)(versioninfo))->sw_major_version =
      (uint8)CAN_17_MCMCAN_SW_MAJOR_VERSION;
    ((Std_VersionInfoType*)(versioninfo))->sw_minor_version =
      (uint8)CAN_17_MCMCAN_SW_MINOR_VERSION;
    ((Std_VersionInfoType*)(versioninfo))->sw_patch_version =
      (uint8)CAN_17_MCMCAN_SW_PATCH_VERSION;
  }

  /* [cover parentID={CF6036C3-803C-466c-B52C-5F008AECD9F2}]
  End
  [/cover] */
}
#endif /* #if( (CAN_17_MCMCAN_VERSION_INFO_API == STD_ON) */
/*******************************************************************************
** Traceability     : [cover parentID={BD8FE8B5-E2AB-4939-AE72-5CFE21FD53F2}] **                                                       **
**                                                                            **
**  Syntax           :static Std_ReturnType Can_17_McmCan_lGlobalInit         **
**                    (                                                       **
**                      const Can_17_McmCan_ConfigType* const Config          **
**                    )                                                       **
**                                                                            **
**  Description      : Master module Initialization function                  **
**                    * This function initializes:                            **
**                    * CAN HW Unit global hardware settings                  **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters(in)   : Config - Pointer to all cores CAN driver configuration **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lGlobalInit(
    const Can_17_McmCan_ConfigType* const Config)
{
  Std_ReturnType ApiStatus;
  uint8 KerIndex;
  /* Set kernel index with zero */
  KerIndex = CAN_17_MCMCAN_ZERO;
  ApiStatus = E_NOT_OK;
  /* Initialize clock settings depends on the no of Kernels used */
  while(KerIndex < Config->CanNoOfKernel)
  {
    /* [cover parentID={750CDB25-8A13-4692-BD6B-B409B5E35C25}]
       Invoke internal function for Clock Settings
    [/cover] */
    ApiStatus = Can_17_McmCan_lInitClkSetting(Config, KerIndex);
    /* [cover parentID={A5E529AC-F4BE-4ffe-BB32-A90F4DF64109}]
     For all configured number of kernels or when clock setting for a kernel fails
    [/cover] */
    if(E_OK != ApiStatus)
    {
      /* Set condition for break loop */
      KerIndex = Config->CanNoOfKernel;
    }
    else
    {
      /* Increment with one */
      KerIndex = KerIndex + CAN_17_MCMCAN_ONE;
    }
  }
  return ApiStatus;
}
/*******************************************************************************
** Traceability     : [cover parentID={5167E051-6336-4bd5-9944-1882E570BDC6}] **                                                        **
**                                                                            **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lCoreInit          **
**                    (                                                       **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig,  **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState      **
**                    )                                                       **
**                                                                            **
**  Description      : Configured core Initialization function                **
**                    * This function initializes:                            **
**                    * CAN core specific global variables                    **
**                    * CAN core specific controller settings                 **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters(in)   :CanConfig- Pointer to core specific CAN driver          **
**                    configuration                                           **
**                    CanCoreState - Pointer to states of respective core     **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lCoreInit(
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  Std_ReturnType ApiStatus;
  uint8 CtrlIndex;

  /* Initialize the error variable */
  ApiStatus = E_OK;

  /* Initialize all the Global variables */
  Can_17_McmCan_lInitGlobalVar(CanConfig, CanCoreState);

  /* Set controller index with zero */
  CtrlIndex = CAN_17_MCMCAN_ZERO;
  /* [cover parentID={F5B648CD-B829-4140-AFDA-9A809BA76E50}]
  For all controllers configured for this core
  [/cover] */
  /* Initialize the CAN controller */
  while((CtrlIndex < CanConfig->CanCoreContCnt) && (E_OK == ApiStatus))
  {
    /* Invoke internal function for the CAN Controller Initialization */
    ApiStatus = Can_17_McmCan_lInitController(CtrlIndex, CanConfig, CanCoreState);
    /* Increment with one */
    CtrlIndex = CtrlIndex + CAN_17_MCMCAN_ONE;
  }
  return ApiStatus;
}
/*******************************************************************************
** Traceability   : [cover parentID={C4964D9F-B613-4e31-BA02-D38ACFE184B8}]   **
**                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lInitGlobalVar         **
**                    (                                                       **
**                     Can_17_McmCan_CoreStatesType* const CanCoreState       **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes global variables of CAN driver             **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : CanConfig - core specific config root structure        **
**                     CanCoreState - Pointer to global structure             **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : none                                                **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lInitGlobalVar(
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  /* controller index count */
  uint8 CntrlIndx;
  /* SIze of the transmit message buffer */
  uint16 TxHwIndex;
  uint16 SwObjIndx;

  /* Clear interrupt status and disable count */
  for(CntrlIndx = CAN_17_MCMCAN_ZERO; CntrlIndx < CanConfig->CanCoreContCnt; \
      CntrlIndx++)
  {
    CanCoreState->CanDisableIntrpCountPtr[CntrlIndx] = CAN_17_MCMCAN_ZERO;
    CanCoreState->CanInterruptStatusPtr[CntrlIndx] = CAN_17_MCMCAN_ZERO;
    CanCoreState->CanInterruptTempStatusPtr[CntrlIndx] = CAN_17_MCMCAN_ZERO;
    CanCoreState->CanControllerModePtr[CntrlIndx] = CANIF_CS_UNINIT;
    /* Clear software PDU handle array */
    for (TxHwIndex = CAN_17_MCMCAN_ZERO; TxHwIndex < \
         CAN_17_MCMCAN_MAX_TX_BUFFER_INDEX; \
         TxHwIndex++)
    {
      /* Calculate Sw Obj Handle index */
      SwObjIndx = (uint16)((CntrlIndx * \
                            CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + TxHwIndex);
      CanCoreState->CanSwObjectHandlePtr[SwObjIndx] = CAN_17_MCMCAN_ZERO;
      #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
      (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
      CanCoreState->CanTxSwObjPriorityPtr[SwObjIndx] = \
          CAN_17_MCMCAN_MAX_SWOBJECT_INDEX;
      #endif
    }
  }
}
/*******************************************************************************
** Traceability     : [cover parentID={9EEB7EAB-D2C8-4cbf-BD18-5467E493D360}] **                                                                            **
**  Syntax           : LOCAL_INLINE Std_ReturnType                            **
**                             Can_17_McmCan_lInitClkSetting                  **
**                    (                                                       **
**                      const Can_17_McmCan_ConfigType* const Config,         **
**                      const uint8 KernelIndx                                **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes clock control register, fractional divider **
**                    register and module control register                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : KernelIndx: Logical core specific kernel indexing      **
**                     Config: - Pointer to CAN driver configuration          **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lInitClkSetting(
    const Can_17_McmCan_ConfigType* const Config,
    const uint8 KernelIndx)
{
  volatile Ifx_CAN* MCMBaseAddressPtr;
  uint32 ClockSelConfigValue;
  volatile uint32 MCRValueTemp;
  uint8 ActNodeCount;
  Std_ReturnType ApiStatus;

  /* Copy the MCMCAN module base address */
  MCMBaseAddressPtr = \
                      Config-> CanMCMModuleConfigPtr[KernelIndx].CanBaseAddress;

  /* Disable CAN kernel clock via CLC */
  /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
  pointer is used.No side effect has been seen. */
  CAN_17_MCMCAN_INIT_DEINIT_WRITE_PERIP_ENDINIT_PROTREG( \
      (volatile void*) & (MCMBaseAddressPtr->CLC), CAN_17_MCMCAN_CLK_DISABLE);

  /* check disable bit is set or not */
  /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
  pointer is used.No side effect has been seen. */
  ApiStatus = Can_17_McmCan_lTimeOut( \
                                      (const volatile uint32*)(&(MCMBaseAddressPtr->CLC.U)), \
                                      (uint32)CAN_17_MCMCAN_MODULE_DISABLE_STATUS, CAN_17_MCMCAN_NOT_EQUAL);
  /* Check Hw is set properly or not within the Time-out */
  /* [cover parentID={6AF3C846-1225-4ae9-99A8-8CB04FB865CD}]
     Hw is set properly or not within the Time-out
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    MCRValueTemp = MCMBaseAddressPtr->MCR.U;
    /* Set CLK selection value with no CLK supply */
    ClockSelConfigValue = (uint32) CAN_17_MCMCAN_ZERO;
    /* Read all the active node and set clock settings */
    for(ActNodeCount = CAN_17_MCMCAN_ZERO; ActNodeCount < \
        CAN_17_MCMCAN_NOOF_NODES_PER_KERNEL; ActNodeCount++)
    {
      /* Set with CAN node Hw index for clock set */
      /* [cover parentID={B54DCAF8-A50A-4b7f-B186-E61620D07A25}]
      Check node is used
      [/cover] */
      if(FALSE != \
          Config->CanMCMModuleConfigPtr[KernelIndx].CanUsedHwCfgIndx[ActNodeCount])
      {
        /* Set configured module with clock selection as enable */
        ClockSelConfigValue |= CAN_17_MCMCAN_MCR_CLKSEL_SET << \
                               (ActNodeCount * CAN_17_MCMCAN_TWO);
      }
    }
    MCRValueTemp |= (CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE | ClockSelConfigValue);

    /* Set MCR.CI and MCR.CCCE to enable clock selection register write
    Set Clock selection for both Sync and Sync of selected Nodes in the
    SubSystem.
    */
    MCMBaseAddressPtr->MCR.U = (Ifx_UReg_32Bit)MCRValueTemp;
    MCRValueTemp &= ~(CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE);
    /* Reset MCR.CI and MCR.CCCE to Clock Change disabled  */
    MCMBaseAddressPtr->MCR.U = MCRValueTemp;

    MCRValueTemp = MCMBaseAddressPtr->MCR.U;
    /* start RAM Initialization */
    MCMBaseAddressPtr->MCR.U |= CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE;
    /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
    pointer is used.No side effect has been seen. */
    ApiStatus = Can_17_McmCan_lTimeOut( \
      (const volatile uint32*)(&(MCMBaseAddressPtr->MCR.U)), \
      (uint32)CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE, CAN_17_MCMCAN_EQUAL);
    /*[cover parentID={6C0257D9-6084-4f36-AB46-9C10CE6E79B2}]
       Is Status Ok?
       [/cover] */
    if(E_NOT_OK != ApiStatus)
    {
      /* Wait until CANn_MCR.RBUSY is 0b */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      ApiStatus = Can_17_McmCan_lTimeOut( \
        (const volatile uint32*)(&(MCMBaseAddressPtr->MCR.U)), \
        (uint32)CAN_17_MCMCAN_MCR_RBUSY_STATUS, CAN_17_MCMCAN_NOT_EQUAL);
      /*[cover parentID={433C9993-75E6-4c64-BE04-495AD92329A2}]
       Is Status Ok?
       [/cover] */
      if(E_NOT_OK != ApiStatus)
      {
        /* Initialize the RAM */
        MCMBaseAddressPtr->MCR.B.RINIT = CAN_17_MCMCAN_ZERO;
        MCMBaseAddressPtr->MCR.B.RINIT = CAN_17_MCMCAN_ONE;
        MCRValueTemp = MCMBaseAddressPtr->MCR.U;
        /* Wait until CANn_MCR.RBUSY is 0b */
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        ApiStatus = Can_17_McmCan_lTimeOut( \
          (const volatile uint32*)(&(MCMBaseAddressPtr->MCR.U)), \
          (uint32)CAN_17_MCMCAN_MCR_RBUSY_STATUS, CAN_17_MCMCAN_NOT_EQUAL);
        MCMBaseAddressPtr->MCR.B.RINIT = CAN_17_MCMCAN_ZERO;
        /* Finish RAM Initialization */
        MCMBaseAddressPtr->MCR.U &= ~CAN_17_MCMCAN_CLKSEL_WRITE_ENABLE;
      }
    }
  }
  return ApiStatus;
}

/*******************************************************************************
** Traceability     : [cover parentID={7C4B0203-2A5D-4ae3-8DFD-C262F50F1971}] **                                                                           **
**  Syntax           : LOCAL_INLINE Std_ReturnType                            **
**                     Can_17_McmCan_lInitController                          **
**                    (                                                       **
**                      const uint8 ControllerId,                             **
**                     const Can_17_McmCan_CoreConfigType* const CanConfig,   **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState      **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes CAN Controller Registers and Enables CAN   **
**                    Controller Interrupts                                   **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId: Index of core specific Controller        **
**                     CanConfig - Pointer to CAN driver configuration        **
**                     CanCoreState - Pointer to global structure             **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : none                                                **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lInitController(
    const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  const Can_17_McmCan_ControllerBaudrateConfigType *CanBaudratePtr;
  const volatile uint32* RegAddressPtr;
  uint16 BaudRateDefaultCfg;
  Std_ReturnType ApiStatus;
  uint8 IntLineOffset;

  IntLineOffset = CAN_17_MCMCAN_FOUR * \
                  CanConfig->CanControllerConfigPtr[ControllerId].CanControllerHwId;
  ApiStatus = E_OK;
  /* Set actual Controller configuration index */
  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;
  /* Interrupt Line assignment for BUSOFF */
  NodeRegAddressPtr->GRINT1.B.BOFF = CAN_17_MCMCAN_TWO + IntLineOffset;
  /* Interrupt Line assignment for TxEventFIFO */
  NodeRegAddressPtr->GRINT1.B.TEFIFO = CAN_17_MCMCAN_ONE + IntLineOffset;
  /* Interrupt Line assignment for RxFIFO buffer watermark level */
  NodeRegAddressPtr->GRINT1.B.WATI = CAN_17_MCMCAN_THREE + IntLineOffset;
  /* Interrupt Line assignment for dedicated receive buffer */
  NodeRegAddressPtr->GRINT2.B.REINT = CAN_17_MCMCAN_ZERO + IntLineOffset;
  /* Interrupt Line assignment for Rx FIFO 0 buffer Full */
  NodeRegAddressPtr->GRINT2.B.RXF0F = CAN_17_MCMCAN_THREE + IntLineOffset;
  /* Interrupt Line assignment for Rx FIFO 1 buffer Full */
  NodeRegAddressPtr->GRINT2.B.RXF1F = CAN_17_MCMCAN_THREE + IntLineOffset;
  /* Check Controller in Software Initialization mode with Configuration
  Change Enable. */
  /* [cover parentID={7DC66300-8269-47a7-9F98-6A1790EEF225}]
  Controller in software initialization mode with config change enabled
  [/cover] */
  if(CAN_17_MCMCAN_BIT_CLEAR_VAL == NodeRegAddressPtr->CCCR.B.INIT)
  {
    /* Set with Initialization mode */
    NodeRegAddressPtr->CCCR.B.INIT = CAN_17_MCMCAN_ONE;

    /* Set with Register Address */
    /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
     pointer is used.No side effect has been seen. */
    RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
    /* Wait until the INIT bits become set */
    ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
      CAN_17_MCMCAN_NODE_INIT_CCE_SET,CAN_17_MCMCAN_EQUAL);
  }
  /* Check Hw is set properly or not within the Time-out */
  /* [cover parentID={95D450C7-4431-41d6-B854-FFFF7E96C5B6}]
  If Hw is set properly
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* Set Configuration Change Enable bit */
    NodeRegAddressPtr->CCCR.B.CCE = CAN_17_MCMCAN_ONE;

    /* Set with Register Address */
    /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
     pointer is used.No side effect has been seen. */
    RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
    /* Wait until the Configuration Change Enable bits become set */
    ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
                                       CAN_17_MCMCAN_CCCR_CCE_SET_MASK, CAN_17_MCMCAN_EQUAL);
    /*[cover parentID={76016B53-1F74-438e-9A08-6AF916FC5C18}]
        Is Status Ok?
    [/cover] */
    if(E_NOT_OK != ApiStatus)
    {
      /* Set NPCR with the Rxinput/Loopback functionality */
      NodeRegAddressPtr->NPCR.U    = CanConfig-> \
                                     CanControllerConfigPtr[ControllerId].CanNPCRValue;
      /* Read Baud rate configuration Index */
      CanBaudratePtr = CanConfig->CanBaudrateConfigPtr;
      /* Select default baud rate configuration of the controller */
      BaudRateDefaultCfg = CanConfig-> \
                           CanControllerConfigPtr[ControllerId].CanDefaultBRCfgIndx;
      /* Set NBTP with Nominal baud-rate */
      NodeRegAddressPtr->NBTP.U =
        CanBaudratePtr[BaudRateDefaultCfg].CanControllerBaudrate;

      #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
      /* [cover parentID={56D0C81C-41E5-4c75-AEE1-176B5352FF46}]
       CAN FD configuration
      [/cover] */
      if(TRUE == CanBaudratePtr[BaudRateDefaultCfg].CanFdConfigEnabled)
      {
        /* Invoke internal function for set FD baud-rate */
        ApiStatus = Can_17_McmCan_lFDSetBaudrate(ControllerId,
                    CanBaudratePtr[BaudRateDefaultCfg].CanFDIndex, CanConfig);
      }
      /* [cover parentID={E2D9AA37-4D28-46a6-9DA1-9203C2BAA72D}]
      Is Status Ok?
      [/cover] */
      if(E_NOT_OK != ApiStatus)
      #endif
      {
        /* Clear All pending Interrupts */
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_ALL_INTERRUPTS;
        /* [cover parentID={769C6736-DB97-4e11-B7D2-0BD181691F57}]
        Is SID Config pointer valid?
        [/cover] */
        if(NULL_PTR != CanConfig->CanSIDFilterConfigPtr)
        {
          /* Invoke function to Set standard ID filter setting */
          Can_17_McmCan_lSIDFilter_Config(ControllerId, CanConfig);
        }
        /* [cover parentID={1209C71F-83EF-46e2-9D8E-CAE8D2292585}]
        Check for XID filter config pointer validity
        [/cover] */
        if(NULL_PTR != CanConfig->CanXIDFilterConfigPtr)
        {
          /* Invoke function to Set Extended ID filter setting */
          Can_17_McmCan_lXIDFilter_Config(ControllerId, CanConfig);
        }
        /* Receive Message RAM settings*/
        Can_17_McmCan_lInitRxMsgObj(ControllerId, CanCoreState, CanConfig);
        if(NULL_PTR != CanConfig->CanTxHwObjectConfigPtr)
        {
          /* Transmit MSG RAM Settings */
          Can_17_McmCan_lInitTxMsgObj(ControllerId, CanCoreState, CanConfig);
        }

        /* Check the BusOff processing mode and Enable BO Interrupt accordingly */
        /* [cover parentID={F34F2A57-68EF-485c-A3BB-7CA922FD28F6}]
        Bus off processing mode
        [/cover] */
        if (CAN_17_MCMCAN_INTERRUPT == \
            CanConfig->CanEventHandlingConfigPtr[ControllerId].  \
            CanEventType[CAN_17_MCMCAN_TWO])
        {
          /* Enable BO Interrupt(Bus_Off Status Interrupt Enable)of requested
          controller */
          CanCoreState->CanInterruptStatusPtr[ControllerId] |= CAN_17_MCMCAN_ONE;
        }
        /* [cover parentID={7A66CD62-B930-477a-BC98-B30C55192E29}]
        Set Controller mode to CANIF_CS_STOPPED
        [/cover] */
        /* Set Controller with STOP mode */
        CanCoreState->CanControllerModePtr[ControllerId] = CANIF_CS_STOPPED;

        #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)

        /* Deactivate Pretended Networking after initialization */

        /* [cover parentID={068C379F-B8E3-41aa-817F-07F3658B0148}]
        Deactivate Pretended Networking after initialization
        [/cover] */
        CanCoreState->CanIcomEnableStatusPtr[ControllerId] = FALSE;
        CanCoreState->CanIcomBlockWriteCalls[ControllerId] = FALSE;
        #endif

        CanCoreState->CanInterruptTempStatusPtr[ControllerId] =
          CanCoreState->CanInterruptStatusPtr[ControllerId];
      }
    }
  }
  return ApiStatus;
}
#if (CAN_17_MCMCAN_SET_BAUDRATE_API == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={52E04677-D94C-44a9-B99A-215C596DA2E0}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lSetBaudrate                 **
**                    (                                                       **
**                       const uint8 Controller,                              **
**                       const uint16 BaudrateIndx,                           **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes the Node Bit Timing Register               **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : ControllerId - CAN Controller Id                       **
**                    ControllerBaudrate - Register setting corresponding     **
**                                         to the baud-rate                   **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK/E_NOT_OK                                          **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lSetBaudrate(const uint8 Controller,
    const uint16 BaudrateIndx,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  /* [cover parentID={87911171-97C7-4cb2-A4A8-1C63DCC43F21}]
  Can_17_McmCan_lSetBaudrate
  [/cover] */
  uint16 StartBaudrateCfgIndx;
  uint16 ActualBaudrateIndx;
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  const Can_17_McmCan_ControllerBaudrateConfigType *CanBaudratePtr;
  Std_ReturnType ApiStatus;

  CanBaudratePtr = CanConfig->CanBaudrateConfigPtr;

  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* Set with Start index of Baudrate configuration */
  StartBaudrateCfgIndx = \
                         CanConfig->CanControllerConfigPtr[Controller].CanBaudrateCfgIndx;
  /* Set with exact Baudrate config index   */
  ActualBaudrateIndx  = BaudrateIndx + StartBaudrateCfgIndx;
  /* Set NBTP with Nominal baud-rate */
  NodeRegAddressPtr->NBTP.U =
    CanBaudratePtr[ActualBaudrateIndx].CanControllerBaudrate;

  #if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* Check if the FD configuration is */
  /* [cover parentID={FC860130-C9DA-430d-9630-1358E0FD206A}]
  CAN FD configuration is enabled
  [/cover] */
  if(TRUE == CanBaudratePtr[ActualBaudrateIndx].CanFdConfigEnabled)
  {
    /* Invoke internal function for set FD baud-rate */
    ApiStatus = Can_17_McmCan_lFDSetBaudrate(Controller, \
                CanBaudratePtr[ActualBaudrateIndx].CanFDIndex, CanConfig);
  }
  else
  {
    /* Disable FD Operation mode */
    NodeRegAddressPtr->CCCR.B.FDOE = CAN_17_MCMCAN_ZERO;

    /* Wait until the INIT bits become set */
    /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
    pointer is used.No side effect has been seen. */
    ApiStatus = Can_17_McmCan_lTimeOut( \
                                        (const volatile uint32*)&NodeRegAddressPtr->CCCR.U, \
                                        CAN_17_MCMCAN_CCCR_FDOE_STATUS, CAN_17_MCMCAN_NOT_EQUAL);
  }
  #else
  ApiStatus = E_OK;
  #endif
  return(ApiStatus);
}
#endif

#if(CAN_17_MCMCAN_FD_ENABLE == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={11202A75-6696-46fd-9135-52F35ACDDD6E}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lFDSetBaudrate               **
**                    (                                                       **
**                       const uint8 Controller,                              **
**                       const uint16 FdIndex,                                **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes the Data Bit Timing Register               **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - CAN Controller Id                         **
**                    FdIndex - FD budrate corresponding array index          **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lFDSetBaudrate(const uint8 Controller,
    const uint16 FdIndex,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  Std_ReturnType ApiStatus;
  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* [cover parentID={78934EF7-E52F-4bb9-8ACA-836B12590D20}]
  Update DBTP register for the controller with FD baudrate value passed
  [/cover] */
  /* Set DBTP with FD configuration set */
  NodeRegAddressPtr->DBTP.U = \
                              CanConfig->CanFDConfigParamPtr[FdIndex].CanControllerFDBaudrate;
  /* [cover parentID={2544BBA5-38CE-4f6b-8639-9636525AD94E}]
  Update Transmitter Delay Compensation (TDCR) Register with the
  transmitter delay compensation value for the FD index passed
  [/cover] */
  /*Set Transmitter Delay Compensation Register with value */
  NodeRegAddressPtr->TDCR.U =
    CanConfig->CanFDConfigParamPtr[FdIndex].CanTrcvDelyComp;
  /* [cover parentID={32EC2721-B304-407d-AFF7-447D2762F472}]
  FD Operation mode as enabled by setting FDOE
  [/cover] */
  /* Set FD Operation mode as Enabled */
  NodeRegAddressPtr->CCCR.B.FDOE = CAN_17_MCMCAN_ONE;

  /* Wait until the INIT bits become set */
  /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
  pointer is used.No side effect has been seen. */
  ApiStatus = Can_17_McmCan_lTimeOut( \
    (const volatile uint32*)&NodeRegAddressPtr->CCCR.U, \
    CAN_17_MCMCAN_CCCR_FDOE_STATUS,CAN_17_MCMCAN_EQUAL);
    /* [cover parentID={D02A3411-8ADB-4e7b-9210-F89C3F1F303F}]
    Is Status Ok?
    [/cover] */
  if(E_OK == ApiStatus)
  {
    /* if BRS is enabled */
    /* [cover parentID={35B6C3D4-FB01-4a5d-8A4E-389FF9CE03B4}]
    Transmit BRS for the given CAN FD index is enabled
    [/cover] */
    if(TRUE == CanConfig->CanFDConfigParamPtr[FdIndex].CanTxBRSEnable)
    {
      /* [cover parentID={748FD593-B97F-48e6-93BE-94D5E6BD774E}]
      Enable Bit rate switching for transmission by setting the BRSE bit
      of the CCCR register for the current controller passed
      [/cover] */
      NodeRegAddressPtr->CCCR.B.BRSE = CAN_17_MCMCAN_ONE;
      /* Wait until the INIT bits become set */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      ApiStatus = Can_17_McmCan_lTimeOut( \
        (const volatile uint32*)&NodeRegAddressPtr->CCCR.U, \
        CAN_17_MCMCAN_CCCR_BRSE_STATUS,CAN_17_MCMCAN_EQUAL);
    }
    else
    {
      /* [cover parentID={C7BC8E2F-B84F-4d73-94A3-C67A4663276C}]
      Disable Bit rate switching for transmission by resetting the BRSE bit of
      the CCCR register for the current controller passed
      [/cover] */
      NodeRegAddressPtr->CCCR.B.BRSE = CAN_17_MCMCAN_ZERO;
      /* Wait until the INIT bits become set */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      ApiStatus = Can_17_McmCan_lTimeOut( \
        (const volatile uint32*)&NodeRegAddressPtr->CCCR.U, \
        CAN_17_MCMCAN_CCCR_BRSE_STATUS,CAN_17_MCMCAN_NOT_EQUAL);
    }
  }
  return(ApiStatus);
}
#endif
/*******************************************************************************
** Traceability     : [cover parentID={2ABF5A62-2C21-4dbd-A582-667E61C8FB73}] **                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lInitTxMsgObj          **
**                    (                                                       **
**                     const uint8 Controller                                 **
**                     Can_17_McmCan_CoreStatesType* const CanCoreState       **
**                     const Can_17_McmCan_CoreConfigType* const CanConfig    **
**                    )                                                       **
**                                                                            **
**  Description    : Initializes transmit message objects                     **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId: Index of core specific Controller        **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lInitTxMsgObj(const uint8 Controller,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint32 CanTxBufferStartAddress;
  uint32 CanTxEventStartAddress;
  uint32 CanTxSlotIndex = 0U;

  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_TX_EFC  TxTempEvntReg;
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_TX_BC  TxTempbuffReg;
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_TX_ESC  TxTempbuffSizeReg;
  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* Set with MessageRAM Tx buffer section start address */
  CanTxBufferStartAddress = CanConfig-> \
                            CanControllerMsgRAMMapConfigPtr[Controller]. \
                            CanControllerMsgRAMMap[CAN_17_MCMCAN_SIX];
  /* Set with value Zero */
  TxTempEvntReg.U = CAN_17_MCMCAN_DATA_ZERO;
  /* Set with value Zero */
  TxTempbuffReg.U = CAN_17_MCMCAN_DATA_ZERO;
  /* Set with value Zero */
  TxTempbuffSizeReg.U = CAN_17_MCMCAN_DATA_ZERO;

  /* Check Tx Event buffer is allocated or not */
  /* [cover parentID={1691C839-6EFD-4be0-A0A0-37513FAA4220}]
  Tx Event buffer is allocated
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanTxBufferStartAddress)
  {
    /* Set with MessageRAM Tx Event buffer section start address */
    CanTxEventStartAddress = CanConfig->
                             CanControllerMsgRAMMapConfigPtr[Controller]. \
                             CanControllerMsgRAMMap[CAN_17_MCMCAN_FIVE];
    /* Set TX Event FIFO Size */
    TxTempEvntReg.B.EFS = \
                          CanConfig->CanControllerMsgRAMMapConfigPtr[Controller].CanTxEvntFIFOSize;
    /* Set Tx Event FIFO Configuration register with Event FIFO Start Address*/
    TxTempEvntReg.B.EFSA = (uint16) ((CanTxEventStartAddress & \
                                      CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO);
    /* Set Offset for the TBSA */
    TxTempbuffReg.B.TBSA = (uint16) ((CanTxBufferStartAddress & \
                                      CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO);


    #if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
    /*  Check if Queue is Enabled */
    /* [cover parentID={F464C10C-74ED-4a8e-9EED-A7E335710EC6}]
    Tx Queue status is Enabled
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerMsgRAMMapConfigPtr[Controller].CanTxQueueStatus)
    {
      /* Set for Tx Queue operation */
      TxTempbuffReg.B.TFQM = CAN_17_MCMCAN_ONE;
      /* Set Tx Queue size */
      TxTempbuffReg.B.TFQS  = CanConfig->
                              CanControllerMsgRAMMapConfigPtr[Controller].CanTxQueueSize;
      /* Set No of used TX dedicated buffer */
      TxTempbuffReg.B.NDTB = CanConfig->
                             CanControllerMsgRAMMapConfigPtr[Controller].CanTxDedBuffCount;
    }
    else
    #endif

    {
      /* Set No of used TX dedicated buffer */
      TxTempbuffReg.B.NDTB = CanConfig->
        CanControllerMsgRAMMapConfigPtr[Controller].CanTxDedBuffCount;
    }

    /* Clear mask before usage */
    CanCoreState->CanTxMask[Controller] = 0;

    /* Arrive at the Tx slots available */
  /* [cover parentID={95CB2453-8B84-4810-A01F-63A10493ED47}]
    Create Mask for Tx Queue and Dedicated
    [/cover] */
    for(CanTxSlotIndex = 0; CanTxSlotIndex < ((uint32)
       ((uint32)TxTempbuffReg.B.NDTB + (uint32)TxTempbuffReg.B.TFQS)); CanTxSlotIndex++)
    {
       CanCoreState->CanTxMask[Controller] |= ((uint32)1U << CanTxSlotIndex);
    }

    /* Copy to back-up buffer mask as well */
    CanCoreState->BackupCanTxMask[Controller] = CanCoreState->CanTxMask[Controller];

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* Load Tx Buffer Configuration register with configuration settings */
    /* [cover parentID={F511F855-98DE-41dd-9F34-8747C9ACF684}]
    FD Support status for the current controller
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[Controller].CanFDSupport)
    {
      /* Set Tx Buffer Data Field Size for FD support */
      TxTempbuffSizeReg.B.TBDS = CAN_17_MCMCAN_SEVEN;
    }
    #endif

    /* [cover parentID={16222DC0-2394-4b68-9201-9C7A680C3E06}]
    Tx processing for current controller is Interrupt mode
    [/cover] */
    /* Check Tx processing mode and Enable Tx Interrupt accordingly */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_ZERO])
    {
      /* Enable TxFIFO event Interrupt(Tx Event FIFO New Entry Interrupt
       Enable)of requested controller */
      CanCoreState->CanInterruptStatusPtr[Controller] |= CAN_17_MCMCAN_TWO;
    }
    /* Set Event FIFO configuration Register */
    NodeRegAddressPtr->TX.EFC.U = TxTempEvntReg.U;
    /* Set Tx message buffer configuration Register */
    NodeRegAddressPtr->TX.BC.U = TxTempbuffReg.U;
    /* Set Tx message buffer size configuration Register */
    NodeRegAddressPtr->TX.ESC.U = TxTempbuffSizeReg.U;
  }
}

/*******************************************************************************
** Traceability     : [cover parentID={E44FFE7B-9375-4fc3-80C1-1FBBEEF7C5F3}] **                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lInitRxMsgObj          **
**                    (                                                       **
**                     const uint8 Controller                                 **
**                     Can_17_McmCan_CoreStatesType* const CanCoreState       **
**                     const Can_17_McmCan_CoreConfigType* const CanConfig    **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes receive message objects                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId: Index of core specific Controller        **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lInitRxMsgObj(const uint8 Controller,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_RX_ESC RxTempBuffSizeConfig;
  #endif
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_RX_F0C RxTempFIFO0Config;
  /* MISRA2012_RULE_19_2_JUSTIFICATION:Union is used to access the register
  contents directly without shift and OR operation. */
  Ifx_CAN_N_RX_F1C RxTempFIFO1Config;
  uint32 CanRxBufferCfgData;

  RxTempFIFO0Config.U = CAN_17_MCMCAN_DATA_ZERO;
  RxTempFIFO1Config.U = CAN_17_MCMCAN_DATA_ZERO;

  #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  RxTempBuffSizeConfig.U = CAN_17_MCMCAN_DATA_ZERO;
  #endif
  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* [cover parentID={FAED0B2A-27ED-4114-9D61-85227F5AC06F}]
  Set Filter for reject non matching and RTR message
  [/cover] */
  /* Set Filter for reject non matching and RTR message */
  NodeRegAddressPtr->GFC.U = CAN_17_MCMCAN_GFC_REJT_RTR_NONMATCH;
  /* Copy Rx buffer start address */
  CanRxBufferCfgData = CanConfig-> \
                       CanControllerMsgRAMMapConfigPtr[Controller]. \
                       CanControllerMsgRAMMap[CAN_17_MCMCAN_FOUR];
  /* [cover parentID={24CF46B0-4388-48e8-A3DF-4EAB7C2649A8}]
  If Address is not NULL
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanRxBufferCfgData)
  {
    /* Set Offset for the RBSA */
    NodeRegAddressPtr->RX.BC.B.RBSA = (uint16) \
                                      ((CanRxBufferCfgData & \
                                        CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO);


    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* [cover parentID={773BB5A2-4B6D-4945-A657-FCB9DABE9754}]
    Is FD Support is enabled
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[Controller].CanFDSupport)
    {
      /* Set RX Buffer Data Field Size for FD support */
      RxTempBuffSizeConfig.B.RBDS = CAN_17_MCMCAN_SEVEN;
    }
    #endif
    /* [cover parentID={B31A738C-6FF6-43f1-802D-1DDD6173C19F}]
    If Rx processing mode is Interrupt
    [/cover] */
    /* Check Rx processing mode and Enable Rx Interrupt accordingly */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_ONE])
    {
      /* Enable Receive interrupts of dedicated buffer */
      CanCoreState->CanInterruptStatusPtr[Controller] |= CAN_17_MCMCAN_FOUR;
    }
  }

  /* [cover parentID={2730AFD7-3DB0-4dfe-A836-221A50446664}]
  FIFO0 Support ON
  [/cover] */
  /* Read FIFO0 Start Address of requested controller */
  CanRxBufferCfgData = CanConfig-> \
                       CanControllerMsgRAMMapConfigPtr[Controller]. \
                       CanControllerMsgRAMMap[CAN_17_MCMCAN_TWO];
  /* [cover parentID={17B4C526-82D0-41d5-814C-378AB49306EC}]
  If Address Exists
  [/cover] */
  if( CAN_17_MCMCAN_ADDRESS_CHECK != CanRxBufferCfgData)
  {
    /* Set Offset for the F0SA */
    RxTempFIFO0Config.B.F0SA = (uint16) \
                               ((CanRxBufferCfgData & \
                                 CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO);
    /* Set with the Rx FIFO0 Size*/
    RxTempFIFO0Config.B.F0S = CanConfig->
                              CanControllerMsgRAMMapConfigPtr[Controller].CanRxFIFO0Size;
    /* Set with the Rx FIFO0 Threshold (WatermarkLevel)*/
    RxTempFIFO0Config.B.F0WM = CanConfig->
                               CanControllerMsgRAMMapConfigPtr[Controller].CanRxFIFO0Threshold;

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* [cover parentID={6EDC40D7-1228-4615-9868-158852979B27}]
    If FD Support is enabled
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[Controller].CanFDSupport)
    {
      /* Set RX Buffer Data Field Size for FD support */
      RxTempBuffSizeConfig.B.F0DS = CAN_17_MCMCAN_SEVEN;
    }
    #endif

    /* [cover parentID={9F75E497-E68C-4e86-A00E-3C3095C51297}]
    Is Rx processing mode is Interrupt
    [/cover] */
    /* Check Rx processing mode and Enable Rx Interrupt accordingly */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_ONE])
    {
      /* Enable Receive interrupts depends on the buffer Type used */
      CanCoreState->CanInterruptStatusPtr[Controller] |= CAN_17_MCMCAN_EIGHT;
    }
    /* Set the configuration */
    NodeRegAddressPtr->RX.F0C.U = RxTempFIFO0Config.U;
  }

  /* [cover parentID={970F31A6-FE62-4deb-A70E-4BE2BDE16C3B}]
  FIFO1 Support ON
  [/cover] */
  /* Read FIFO1 Start Address of requested controller */
  CanRxBufferCfgData = CanConfig-> \
                       CanControllerMsgRAMMapConfigPtr[Controller]. \
                       CanControllerMsgRAMMap[CAN_17_MCMCAN_THREE];
  /* [cover parentID={E7206761-88E7-4868-A4F0-12DBC6BEC78C}]
  If Address Exists
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanRxBufferCfgData)
  {
    /* Set Offset for the RXFIFO1 */
    RxTempFIFO1Config.B.F1SA  = \
                                (uint16)((CanRxBufferCfgData & \
                                          CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO);
    /* Set with Rx FIFO 1 Size*/
    RxTempFIFO1Config.B.F1S = \
                              CanConfig->CanControllerMsgRAMMapConfigPtr[Controller].CanRxFIFO1Size;
    /* Set with the Rx FIFO0 Threshold (WatermarkLevel)*/
    RxTempFIFO1Config.B.F1WM = \
                               CanConfig->CanControllerMsgRAMMapConfigPtr[Controller].CanRxFIFO1Threshold;

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* [cover parentID={32E8045B-8F18-4e47-9AF5-11FA949AEEF5}]
    If FD Support is enabled
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[Controller].CanFDSupport)
    {
      /* Set RX Buffer Data Field Size for FD support */
      RxTempBuffSizeConfig.B.F1DS = CAN_17_MCMCAN_SEVEN;
    }
    #endif

    /* [cover parentID={C3A2A141-A862-4ee7-8237-19C733ACAF5C}]
    Is Rx processing mode is Interrupt
    [/cover] */
    /* Check Rx processing mode and Enable Rx Interrupt accordingly */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_ONE])
    {
      /* Enable Receive interrupts depends on the buffer Type used */
      CanCoreState->CanInterruptStatusPtr[Controller] |= CAN_17_MCMCAN_SIXTEEN;
    }
    /* Set the configuration */
    NodeRegAddressPtr->RX.F1C.U = RxTempFIFO1Config.U;
  }

  #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  /* Set Rx Message RAM data Size */
  NodeRegAddressPtr->RX.ESC.U = RxTempBuffSizeConfig.U;
  #endif
}

/*******************************************************************************
** Traceability     : [cover parentID={B7A147D3-D308-4d22-9D34-F5E41D237DDA}] **                                                                          **
**  Syntax           : static Can_ReturnType                                  **
**                             Can_17_McmCan_lSetModeWuSupportOff             **
**                     (                                                      **
**                        const uint8 Controller,                             **
**                        const Can_StateTransitionType Transition,           **
**                        Can_17_McmCan_CoreStatesType* const CanCoreState,   **
**                        const Can_17_McmCan_CoreConfigType* const CanConfig **
**                      )                                                     **
**                                                                            **
**  Description      : Performs software triggered state transitions of the   **
**                    CAN Controller State machine when wakeup support is     **
**                    Disabled                                                **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller Configuration Id**
**                    Transition - Requested transition                       **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : CAN_OK : The HW state has been changed with requested  **
**                                 Transition                                 **
**                    CAN_NOT_OK : The HW status has not been changed with    **
**                                 requested Transition                       **
**                                                                            **
*******************************************************************************/
static Can_ReturnType Can_17_McmCan_lSetModeWuSupportOff(
    const uint8 Controller,
    const Can_StateTransitionType Transition,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  /* Pointer to CAN node register type */
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  /* register address to be checked for timeout */
  const volatile uint32* RegAddressPtr;
  /* Api return value */
  Can_ReturnType RetVal;
  /* return value for timeout check */
  Std_ReturnType ApiStatus;

  /* Set Value with E_NOT_OK */
  RetVal = CAN_NOT_OK;
  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;

  switch (Transition)
  {
    case CAN_T_START :
    {
      /* [cover parentID={A28D888E-5AA2-4e5f-A538-D800260574F9}]
      Check for all Transition Value-1
      [/cover] */
      /* Clear receive message object to avoid older notification */
      Can_17_McmCan_lClearReceivedMsg(Controller, CanConfig);
      /* Set CAN controller with normal operation mode */
      NodeRegAddressPtr->CCCR.B.INIT = CAN_17_MCMCAN_ZERO;
      /* Set with Register Address */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
      /* Check the Hw status update */
      /* [cover parentID={AF7A0560-C039-4dec-B95A-25D6CFB11C21}]
      If Hw state changes successfully
      [/cover] */
      ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
                                         (uint32) CAN_17_MCMCAN_NODE_INIT_MODE, CAN_17_MCMCAN_NOT_EQUAL);
      /* [cover parentID={40ECCD6D-8DFF-4211-BBC1-FC1C33947E85}]
      Check the Hw status update
      [/cover] */
      /* Hw state changes successfully then set Sw flag */
      /* [cover parentID={720A095C-C6F2-4c71-94BB-C0B090F1BEB0}]
      Is status Ok ?
      [/cover] */
      if(E_OK == ApiStatus)
      {
        /* Clear any registered interrupt */
        NodeRegAddressPtr->IR.U = NodeRegAddressPtr->IR.U;

        /* [cover parentID={33C1F910-85B0-4430-AA7E-2AD19123F015}]
        Enable all configured interrupts
        [/cover] */
        /* Enable all interrupts */
        CanCoreState->CanInterruptTempStatusPtr[Controller] = \
            CanCoreState->CanInterruptStatusPtr[Controller];
        /* Set with Register Address */
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->PSR.U));
        /* Check the Hw status update */
        ApiStatus = Can_17_McmCan_lTimeOut( \
                                            (const volatile uint32*)RegAddressPtr, \
                                            (uint32) CAN_17_MCMCAN_NODE_PSR_MODE, CAN_17_MCMCAN_NAND);
        /* [cover parentID={FB6D89D0-9946-4868-A605-F62BFFB48EEE}]
        Check if timeout has occured
        [/cover] */
        if(E_OK != ApiStatus)
        {
          /* Set return value with CAN_NOT_OK */
          RetVal = CAN_NOT_OK;
        }
        else
        {
          /* [cover parentID={E7A6971F-A6B2-41e4-99B6-48B062FE55AD}]
          Check for ICOM feature is enabled
          [/cover] */
          #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
          /* [cover parentID={53257BEE-C5E0-4546-A37D-BE3CABED1AB4}]
          Check if ICOM is enabled
          [/cover] */
          if(CAN_17_MCMCAN_ZERO != \
              CanCoreState->CanActiveIcomCfgIdPtr[Controller])
          {
            /* [cover parentID={0271DE7F-C0FA-4e76-BF51-E34E19D59472}]
            Deactivate ICOM
            [/cover] */
            Can_17_McmCan_lDeactivateIcom(Controller, CAN_17_MCMCAN_ZERO, \
                                          CanCoreState, CanConfig);
          }
          #endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */
          /* Set current mode to STARTED */
          CanCoreState->CanControllerModePtr[Controller] = CANIF_CS_STARTED;
          /* Set return value with CAN_OK */
          RetVal = CAN_OK;
        }
      }
    }
    break;
    case CAN_T_SLEEP :
    {
      /* [cover parentID={0FB0FFE1-A9BF-4625-AEEE-0C91556E298D}]
        Check for all Transition Value-2
        [/cover] */
      /* Set CCR with Clock Stop request */
      NodeRegAddressPtr->CCCR.B.CSR = CAN_17_MCMCAN_ONE;
      /* Set with Register Address */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
      /* [cover parentID={D339BFC0-01C9-4a6f-95A7-B19279F7CBCC}]
      Wait for the Clock Stop Acknowledge
      [/cover] */
      /* Wait for the Clock Stop Acknowledge */
      ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
                                         (uint32)CAN_17_MCMCAN_CCCR_CSA_STATUS, CAN_17_MCMCAN_EQUAL);

      /* [cover parentID={686F4572-03A0-4340-98CD-2B2E96515727}]
      Is Hw is set properly
      [/cover] */
      /* Check Hw is set properly or not within the Time-out */
      if(E_OK == ApiStatus)
      {
        /* [cover parentID={9CBD591A-C3D3-42ca-BD0B-0BF5B2C0D76D}]
        Disable all configured interrupts
        [/cover] */
        /* Disable all interrupts */
        CanCoreState->CanInterruptTempStatusPtr[Controller] = \
            CAN_17_MCMCAN_ZERO;
        /* Set current mode to SLEEP */
        CanCoreState->CanControllerModePtr[Controller] = CANIF_CS_SLEEP;
        /* Set return value with E_OK */
        RetVal = CAN_OK;
      }
    }
    break;
    case CAN_T_STOP :
    {
      /* [cover parentID={F74C1850-5FB1-4334-8198-FF0DD6F6C600}]
      Check for all Transition Value-3
      [/cover] */
      /* [cover parentID={23230BFC-4DD0-4a70-AE2E-378CE22AA0E4}]
      Request for cancel pending request
      [/cover] */
      /* Request for cancel pending request */
      Can_17_McmCan_lCancelPendingTx(Controller, CanCoreState, CanConfig);
      /* Set CAN controller with INIT mode */
      NodeRegAddressPtr->CCCR.B.INIT = CAN_17_MCMCAN_ONE;
      /* Set with Register Address */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
      /* [cover parentID={D3BFCEAA-D8DA-4a9f-9AD6-500C52B176FB}]
      Wait for the Clock Stop Acknowledge
      [/cover] */
      /* Wait for the Clock Stop Acknowledge */
      ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*) \
        RegAddressPtr, CAN_17_MCMCAN_CCCR_INIT_SET_MASK,CAN_17_MCMCAN_EQUAL);

      /* [cover parentID={588152E2-95A3-490b-BF97-9B677117466F}]
      If Hw state changes successfully
      [/cover] */
      /* Hw state changes successfully then set SW flag */
      if(E_OK == ApiStatus)
      {
        /* Disable all interrupts */
        CanCoreState->CanInterruptTempStatusPtr[Controller] = \
            CAN_17_MCMCAN_ZERO;
        /* Set Configuration Change Enable bit */
        NodeRegAddressPtr->CCCR.B.CCE = CAN_17_MCMCAN_ONE;
        /* Set with Register Address */
        /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
        pointer is used.No side effect has been seen. */
        RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
        /* Wait for the Clock Stop Acknowledge */
        ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*) \
          RegAddressPtr, CAN_17_MCMCAN_CCCR_CCE_SET_MASK,CAN_17_MCMCAN_EQUAL);
        /* [cover parentID={17802C03-1946-4f17-832D-DAE66A7ABA21}]
        ICOM is activated for the controller
        [/cover] */
        if(E_OK == ApiStatus)
        {
          #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
          if(CAN_17_MCMCAN_ZERO != \
              CanCoreState->CanActiveIcomCfgIdPtr[Controller])
          {
            /* [cover parentID={9FE15F43-E706-4954-BFAF-C39BF2422CF8}]
            Deactivate ICOM
            [/cover] */
            Can_17_McmCan_lDeactivateIcom(Controller, CAN_17_MCMCAN_ZERO, \
                                          CanCoreState, CanConfig);
          }
          #endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */
          /* Set current mode to STOPPED */
          CanCoreState->CanControllerModePtr[Controller] = CANIF_CS_STOPPED;
          /* Set return value with E_OK */
          RetVal = CAN_OK;
        }
      }
    }
    break;
    default:  /* case CAN_T_WAKEUP */
    {
      /* Reset the Clock Stop Request */
      NodeRegAddressPtr->CCCR.B.CSR = CAN_17_MCMCAN_ZERO;
      /* Set with Register Address */
      /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
      pointer is used.No side effect has been seen. */
      RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
      /* [cover parentID={02A3871B-D153-441e-AD56-F549AC7900AC}]
      Wait for the Clock Stop Acknowledge
      [/cover] */
      /* Wait for the No clock stop acknowledge */
      ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*) \
        RegAddressPtr, (uint32)CAN_17_MCMCAN_CCCR_CSA_STATUS, \
        CAN_17_MCMCAN_NOT_EQUAL);
      /* [cover parentID={DE6B7946-02F3-4382-A40F-D11B98A2416C}]
      if Hw state changes successfully
      [/cover] */
      /* Check Hw is set properly or not within the Time-out If yes then set Sw
      flag */
      if(E_OK == ApiStatus)
      {
        /* Disable all interrupts */
        CanCoreState->CanInterruptTempStatusPtr[Controller] = \
            CAN_17_MCMCAN_ZERO;
        /* Set current mode to STOPPED */
        CanCoreState->CanControllerModePtr[Controller] = CANIF_CS_STOPPED;
        /* Set return value with E_OK */
        RetVal = CAN_OK;
      }
    }
    break;
  }
  SchM_Enter_Can_17_McmCan_CanIntCtrl();
  /* [cover parentID={F3871B24-CD29-460b-9309-C5CCE4A6B431}]
  If disable Interrupt count is ZERO and the state transition has
  occured succesfully
  [/cover] */
  if ((CAN_17_MCMCAN_ZERO == CanCoreState->CanDisableIntrpCountPtr[Controller])
    && (CAN_OK == RetVal))
  {
    /* Enable/Disable BO Interrupt(Bus_Off Status Interrupt Enable)of requested
    controller */
    NodeRegAddressPtr->IE.B.BOE = \
       CanCoreState->CanInterruptTempStatusPtr[Controller] & CAN_17_MCMCAN_ONE;
    /* Enable/Disable TxFIFO event Interrupt(Tx Event FIFO New Entry Interrupt
    Enable)of requested controller */
    NodeRegAddressPtr->IE.B.TEFNE = \
      ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
      CAN_17_MCMCAN_ONE) & CAN_17_MCMCAN_ONE);
    /* Enable/Disable Receive interrupts of dedicated buffer */
    NodeRegAddressPtr->IE.B.DRXE = \
      ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
      CAN_17_MCMCAN_TWO) & CAN_17_MCMCAN_ONE);
    /* Enable/Disable Receive FIFO0 Watermark interrupts depends on the buffer Type used */
    NodeRegAddressPtr->IE.B.RF0WE = \
    ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
    CAN_17_MCMCAN_THREE) & CAN_17_MCMCAN_ONE);
    /* Enable/Disable Receive FIFO 0 FUll interrupts depends on the buffer Type used */
    NodeRegAddressPtr->IE.B.RF0FE = \
    ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
    CAN_17_MCMCAN_THREE) & CAN_17_MCMCAN_ONE);
    /* Enable/Disable Receive FIFO1 Watermark interrupts depends on the buffer Type used */
    NodeRegAddressPtr->IE.B.RF1WE = \
    ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
    CAN_17_MCMCAN_FOUR) & CAN_17_MCMCAN_ONE);
    /* Enable/Disable Receive FIFO 1 FUll interrupts depends on the buffer Type used */
    NodeRegAddressPtr->IE.B.RF1FE = \
    ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
    CAN_17_MCMCAN_FOUR) & CAN_17_MCMCAN_ONE);
  }
  SchM_Exit_Can_17_McmCan_CanIntCtrl();
  return RetVal;
}

/*******************************************************************************
** Traceability   : [cover parentID={A92A9706-2381-4fd3-A925-36AFF492CB8E}]   **
**                                                                            **
**  Syntax           :LOCAL_INLINE void Can_17_McmCan_lDisableInterrupts      **
**                   (                                                        **
**                      const uint8 Controller,                               **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig,  **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState      **
**                   )                                                        **
**                                                                            **
**  Description      : Disables CAN Controller Interrupts                     **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lDisableInterrupts (const uint8 Controller,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;

  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* [cover parentID={921F2114-3F35-4619-BF18-625800CD8F21}]
  [/cover] */
  /* [cover parentID={10696534-AB0B-466e-907E-01ECE53F050D}]
  Enter Critical Section call
  [/cover] */
  /* Enter Critical Section */
  SchM_Enter_Can_17_McmCan_CanIntCtrl();
  /* [cover parentID={A709A9A8-6D79-45a6-A189-F5F0FD6C1B45}]
  Check Disable interrupt count is at reset value
  [/cover] */
  if (CAN_17_MCMCAN_ZERO == CanCoreState->CanDisableIntrpCountPtr[Controller])
  {
    /* [cover parentID={BC56B154-2603-4bf8-BD0A-3E3D15801929}]
    Check Tx interrupt is enabled
    [/cover] */
    /* Check Tx interrupt is enabled */
    if(CAN_17_MCMCAN_ONE == \
        ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
          CAN_17_MCMCAN_ONE) & CAN_17_MCMCAN_ONE))
    {
      /* Disable Tx interrupt */
      NodeRegAddressPtr->IE.B.TEFNE = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={EC0C0BDC-5DD4-48ad-ABFE-71A2771FEE23}]
    Check dedicated Rx interrupt is enabled
    [/cover] */
    /* Check dedicated Rx interrupt is enabled */
    if(CAN_17_MCMCAN_ONE == \
        ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
          CAN_17_MCMCAN_TWO) & CAN_17_MCMCAN_ONE))
    {
      /* Disable Rx interrupt */
      NodeRegAddressPtr->IE.B.DRXE = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={84C21DAB-78CD-4bed-AFD3-7C1DCE6609D0}]
    Check Bus-off interrupt is enabled
    [/cover] */
    /* Check Bus-off interrupt is enabled */
    if(CAN_17_MCMCAN_ONE == (CanCoreState-> \
                             CanInterruptTempStatusPtr[Controller] & CAN_17_MCMCAN_ONE))
    {
      /* Disable Bus-off interrupt */
      NodeRegAddressPtr->IE.B.BOE = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={7C3FFC76-F532-4150-96FF-52C017910FE0}]
    Check Rx FIFO0 interrupt is enabled
    [/cover] */
    /* Check Rx FIFO0 interrupt is enabled */
    if(CAN_17_MCMCAN_ONE == ((CanCoreState-> \
                              CanInterruptTempStatusPtr[Controller] >> \
                              CAN_17_MCMCAN_THREE) & CAN_17_MCMCAN_ONE))
    {
      /* Disable FIFO0 watermark interrupt */
      NodeRegAddressPtr->IE.B.RF0WE = CAN_17_MCMCAN_ZERO;
      /* Disable FIFO0 full interrupt */
      NodeRegAddressPtr->IE.B.RF0FE = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={4ED4D4EF-A14E-4a77-BEDB-2B2265D2F85C}]
    Check Rx FIFO1 interrupt is enabled
    [/cover] */
    /* Check Rx FIFO1 interrupt is enabled */
    if(CAN_17_MCMCAN_ONE == ((CanCoreState-> \
                              CanInterruptTempStatusPtr[Controller] >> \
                              CAN_17_MCMCAN_FOUR) & CAN_17_MCMCAN_ONE))
    {
      /* Disable FIFO1 watermark interrupt */
      NodeRegAddressPtr->IE.B.RF1WE = CAN_17_MCMCAN_ZERO;
      /* Disable FIFO1 full interrupt */
      NodeRegAddressPtr->IE.B.RF1FE = CAN_17_MCMCAN_ZERO;
    }
  }
  /* Count the number of times Can_17_McmCan_DiableControllerInterrupts API
  is called */
  CanCoreState->CanDisableIntrpCountPtr[Controller] = \
      CanCoreState->CanDisableIntrpCountPtr[Controller] + CAN_17_MCMCAN_ONE;
  /* [cover parentID={921F2114-3F35-4619-BF18-625800CD8F21}]
  [/cover] */
  /* Exit Critical Section */
  SchM_Exit_Can_17_McmCan_CanIntCtrl();
}

/*******************************************************************************
** Traceability   : [cover parentID={02E9A839-B32F-4bd8-8C8A-E98E11A06D5E}]   **
**                                                                            **
**  Syntax           :LOCAL_INLINE void Can_17_McmCan_lEnableInterrupts       **
**                   (                                                        **
**                      const uint8 Controller,                               **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig,  **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState      **
**                   )                                                        **
**                                                                            **
**  Description      : Enable CAN Controller Interrupts                       **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lEnableInterrupts (const uint8 Controller,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;

  /* Get Node Register address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* [cover parentID={BA1DDBF8-9AEC-4e76-B6FF-B9B3597CAFF2}]
  Enter Critical Section call
  [/cover] */
  /* [cover parentID={B3999980-346F-4b50-A961-D4F071098113}]
  [/cover] */
  /* Enter Critical Section */
  SchM_Enter_Can_17_McmCan_CanIntCtrl();
  /* [cover parentID={A61F9762-6392-4d5d-9F01-F19D77EA175B}]
  Check disable interrupt count is greater than zero.
  [/cover] */
  /* Check Can_17_McmCan_DisableControllerInterrupts has been called before,If
   no then don't perform any action. */
  if (CanCoreState->CanDisableIntrpCountPtr[Controller] > CAN_17_MCMCAN_ZERO)
  {
    /* Incremental interrupt disable/enable count */
    CanCoreState->CanDisableIntrpCountPtr[Controller] = \
        CanCoreState->CanDisableIntrpCountPtr[Controller] - CAN_17_MCMCAN_ONE;
    /* Check if disable count is zero or not */
    /* [cover parentID={FC7AA1C8-179D-4388-BDCE-972A6D27F4AB}]
    Check if disable count is zero or not
    [/cover] */
    if (CAN_17_MCMCAN_ZERO == CanCoreState->CanDisableIntrpCountPtr[Controller])
    {
      /* Re enable the status of Interrupt */
      /* Enable BO Interrupt(Bus_Off Status Interrupt Enable)of requested
      controller */
      NodeRegAddressPtr->IE.B.BOE = \
                                    CanCoreState->CanInterruptTempStatusPtr[Controller] & CAN_17_MCMCAN_ONE;
      /* Enable TxFIFO event Interrupt(Tx Event FIFO New Entry Interrupt
      Enable)of requested controller */
      NodeRegAddressPtr->IE.B.TEFNE = \
                                      ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
                                        CAN_17_MCMCAN_ONE) & CAN_17_MCMCAN_ONE);
      /* Enable Receive interrupts of dedicated buffer */
      NodeRegAddressPtr->IE.B.DRXE = \
                                     ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
                                       CAN_17_MCMCAN_TWO) & CAN_17_MCMCAN_ONE);
      /* Enable Receive FIFO0 watermark interrupts depends on the buffer Type used */
      NodeRegAddressPtr->IE.B.RF0WE = \
                                      ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
                                        CAN_17_MCMCAN_THREE) & CAN_17_MCMCAN_ONE);
      /* Enable Receive FIFO0 Full interrupts depends on the buffer Type used */
      NodeRegAddressPtr->IE.B.RF0FE = \
                                      ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
                                        CAN_17_MCMCAN_THREE) & CAN_17_MCMCAN_ONE);
      /* Enable Receive FIFO1 watermark interrupts depends on the buffer Type used */
      NodeRegAddressPtr->IE.B.RF1WE = \
                                     ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
                                       CAN_17_MCMCAN_FOUR) &  CAN_17_MCMCAN_ONE);
      /* Enable Receive FIFO1 Full interrupts depends on the buffer Type used */
      NodeRegAddressPtr->IE.B.RF1FE = \
        ((CanCoreState->CanInterruptTempStatusPtr[Controller] >> \
          CAN_17_MCMCAN_FOUR) &  CAN_17_MCMCAN_ONE);
    }
  }
  /* [cover parentID={B3999980-346F-4b50-A961-D4F071098113}]
  [/cover] */
  /* Exit Critical Section */
  SchM_Exit_Can_17_McmCan_CanIntCtrl();
}
#if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={FC5DA7C1-F783-49e2-A1CC-FCAC1753A56F}] **                                                                            **
**  Syntax        : LOCAL_INLINE Can_ReturnType Can_17_McmCan_lUpdatePduInfo  **
**                    (                                                       **
**                       const Can_HwHandleType HthIndex,                     **
**                       Can_PduType* TempPduInfo,                            **
**                       const Can_PduType* const PduInfo,                    **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                     )                                                      **
**                                                                            **
**  Description    : Programs Message Object for transmission                 **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HthIndex - HTH index in the configuration structure    **
**                    PduInfo - Pointer to SDU user memory, DLC and           **
**                              Identifier                                    **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : CAN_OK - Write command has been accepted               **
**                   CAN_BUSY - No message object available or pre-emptive    **
**                              call of Can_17_McmCan_Write that can't be     **
**                              implemented re-entrant                        **
**                   CAN_NOT_OK - development error occurred                  **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE Can_ReturnType Can_17_McmCan_lUpdatePduInfo(
    const Can_HwHandleType HthIndex,
    Can_PduType* const TempPduInfo, const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  /* PduInfoType structure variable */
  PduInfoType PduInfoTrigger;
  PduInfoType *WritePduInfoPtr;
  const Can_17_McmCan_TxHwObjectConfigType *TxObjPtr;
  Std_ReturnType TriggerStatus;
  Can_ReturnType ApiStatus;

  /* Set value with OK */
  ApiStatus = CAN_OK;
  /* Set with TX object configuration start index */
  TxObjPtr = CanConfig->CanTxHwObjectConfigPtr;
  /* initialize Pointers PduInfoPtr and TempPduInfo */
  /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
  is agreed as we refer to this address while passing the pointer and usage
  of this variable is limited only to this call*/
  WritePduInfoPtr = &PduInfoTrigger;
  /* Check Trigger Transmit is Enabled */
  /* [cover parentID={04AC4257-F643-43e8-99D3-EBC156D07C6E}]
    Check Trigger Transmit is Enabled
    [/cover] */
  if(TRUE == TxObjPtr[HthIndex].CanTrigTxStatus)
  {

    /*Store Data pointer and Data length*/
    WritePduInfoPtr->SduDataPtr = PduInfo->sdu;
    WritePduInfoPtr->SduLength =  (uint8)(PduInfo->length);
    /* [cover parentID={F0374D8B-60E3-4ab4-8FE3-7603DB0449AE}]
    [/cover] */
    /* CanIf_TriggerTransmit() to acquire the PDUs data */
    TriggerStatus = CanIf_TriggerTransmit(PduInfo->swPduHandle, WritePduInfoPtr);
    /* [cover parentID={FD2C996D-7554-4999-B056-773E081E56BC}]
    Check Trigger Status
    [/cover] */
    if(E_NOT_OK != TriggerStatus)
    {
      /*Store the updated Data pointer and Data length*/
      TempPduInfo->sdu = WritePduInfoPtr->SduDataPtr;
      TempPduInfo->length = (uint8)(WritePduInfoPtr->SduLength);
    }
    else
    {
      #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* [cover parentID={A27902BD-64B0-4990-8BA1-DD6CE6F07202}]
      [/cover] */
      /* Report to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_POINTER);
      #endif
      /* Set value with OK */
      ApiStatus = CAN_NOT_OK;
    }
  }
  else
  {
    /*Store the Data pointer and Data length*/
    TempPduInfo->sdu = PduInfo->sdu;
    TempPduInfo->length = PduInfo->length;
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON) */

#if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
/*******************************************************************************
** Traceability   :  [cover parentID={D0A7E05E-23E0-4e41-98D8-FE87B3AB14CC}]  **
**                                                                            **
**  Syntax           : static uint8 Can_17_McmCan_lCheckQueueMask
(const uint8 ControllerId,                                                    **
const Can_17_McmCan_CoreConfigType* const CanConfig,                          **
uint32* const CanQueueSlotMask)                                               **
**                                                                            **
**                                                                            **
**  Description    :  Check for next available transmit Queue slot            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : ControllerId - Controller id for which Queue slot check**
**                     is made                                                **
**                     CanConfig - Can config pointer                         **
**                     CanQueueSlotMask - pointer to Queue mask updated       **
**                     for controller                                         **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : 0xFF - If no slot is found in Tx Queue                 **
**                   : 0 to Queue size - Next available free slot             **
**                                                                            **
*******************************************************************************/
static uint8 Can_17_McmCan_lCheckQueueMask(const uint8 ControllerId,
const Can_17_McmCan_CoreConfigType* const CanConfig,
const uint32* const CanQueueSlotMask)
{
  uint32 CanTxIndex;
  uint8 SlotValue = 0xFF;
  uint32 CanSlotmask;
  /* MISRA2012_RULE_19_2_JUSTIFICATION: Copy of register content is taken to stack,
  used to access bits with-in the stack variable. */
  Ifx_CAN_N_TX_BC  TxTempbuffReg;
  volatile const Ifx_CAN_N *NodeRegAddressPtr;

  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;

  TxTempbuffReg.U = NodeRegAddressPtr->TX.BC.U;

  /* Check for slot in Queue */
  /* [cover parentID={538FB8CD-D833-43a9-A1A6-292667CF4B98}] Check for slot
  [/cover] */
  for(CanTxIndex = (uint32)(TxTempbuffReg.B.NDTB); CanTxIndex < ((uint32)
        ((uint32)TxTempbuffReg.B.NDTB + (uint32)TxTempbuffReg.B.TFQS)); CanTxIndex++)
  {
    CanSlotmask = ((uint32)1U << CanTxIndex);
    /* [cover parentID={E1BE3A34-12AF-42a2-A0ED-FC72BDC0EE13}] Check if Tx Queue slot available
    [/cover] */
    if((*CanQueueSlotMask & CanSlotmask) == CanSlotmask)
    {
      SlotValue = (uint8) CanTxIndex;
      break;
    }
  }

  return SlotValue;
}
#endif

/*CYCLOMATIC_Can_17_McmCan_lWriteMsgObj_JUSTIFICATION: This Function has a
cyclomatic Complexity of 18 because this function handles can write for the
dedicated and multiplexed transmission. All write related handling is
implemented as part of this function and more redable to have it in single function.
*/
/*******************************************************************************
** Traceability   : [cover parentID={24A1ABDD-424C-4af5-8711-FB56D2461FCF}]   **
**                                                                            **
**  Syntax           : static Can_ReturnType Can_17_McmCan_lWriteMsgObj       **
**                    (                                                       **
**                       const Can_HwHandleType HthIndex,                     **
**                       const Can_PduType* const PduInfo,                    **
**                       Can_17_McmCan_CoreStatesType* const CanCoreState,    **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                     )                                                      **
**                                                                            **
**  Description    : Programs Message Object for transmission                 **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HthIndex - HTH index in the configuration structure    **
**                    PduInfo - Pointer to SDU user memory, DLC and           **
**                              Identifier                                    **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : CAN_OK - Write command has been accepted               **
**                   CAN_BUSY - No message object available or pre-emptive    **
**                              call of Can_17_McmCan_Write that can't be     **
**                              implemented re-entrant                        **
**                   CAN_NOT_OK - development error occurred                  **
**                                                                            **
*******************************************************************************/
static Can_ReturnType Can_17_McmCan_lWriteMsgObj(
    const Can_HwHandleType HthIndex,
    const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  volatile Ifx_CAN_TXMSG* TxMsgBufferPtr;
  const Can_17_McmCan_TxHwObjectConfigType *TxObjPtr;
  const Can_17_McmCan_ControllerMsgRAMConfigType *MsgRamCfgPtr;
  Can_PduType *TempPduInfo;
  Can_PduType PduTemp;
  uint32 TxWriteStartAddr;
  uint32 BuffIndex;
  uint8 BuffElementIndex = 0U;
  Can_ReturnType ApiStatus;
  uint8 TxMsgDLC;
  uint8 DataBytePos;
  uint8 ControllerId;
  uint16 SwObjIndx;

  #if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  Can_17_McmCan_TxBufferType TxBuffType;
  #endif

  /* Set Message RAM configuration pointer with configuration address */
  MsgRamCfgPtr = CanConfig->CanControllerMsgRAMMapConfigPtr;
  /* Set with TX object configuration start index */
  TxObjPtr = CanConfig->CanTxHwObjectConfigPtr;
  /* [cover parentID={083BC331-9160-4caf-8113-27A6FC5E1098}]
  Multiplexed transimssion is enabled
  [/cover] */
  #if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  /* Copy the buffer Type of requested HTH */
  TxBuffType = TxObjPtr[HthIndex].CanTxBufferType;
  #endif
  /* Set with the associated controller of requested HTH */
  ControllerId = Can_17_McmCan_kGblConfigPtr->CanLogicalControllerIndexPtr \
                 [TxObjPtr[HthIndex].HwControllerId].CanLCoreSpecContIndex;
  /* Get Node Register address */
  NodeRegAddressPtr = CanConfig-> \
                      CanControllerConfigPtr[ControllerId].CanNodeAddress;
  /* Set with Message RAM start address of Tx buffer */
  TxWriteStartAddr = MsgRamCfgPtr[ControllerId]. \
                     CanControllerMsgRAMMap[CAN_17_MCMCAN_SIX];
  /* Set value with OK */
  ApiStatus = CAN_OK;

  /* Copy the buffer Index for Mask */
  BuffIndex = (uint32)(CAN_17_MCMCAN_TX_BUFF_MASK << (TxObjPtr[HthIndex]. \
                       CanTxBuffIndx));

  /* [cover parentID={EC87F7B9-D77C-4a3f-9F3E-D94C2E054494}]
  Enter Critical Section call
  [/cover] */
  /* [cover parentID={6E1CBB09-0089-421f-AFD2-C08A9E6EAEB6}]
  [/cover] */
  /* Enter Critical Section */
  SchM_Enter_Can_17_McmCan_CanWrMO();

  #if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_OFF)
  /* [cover parentID={58143A88-35A7-4220-95E2-5634FFA8CA5D}] Check for
  buffer free
  [/cover] */
  /* check if buffer is free or not */
  if((CanCoreState->CanTxMask[ControllerId] & BuffIndex) != BuffIndex)
  {
    /* [cover parentID={9A15F3CC-7C7F-45ba-BFF6-175F4FDC04C5}]
    Set return value as Busy
    [/cover] */
    ApiStatus = CAN_BUSY;
    BuffElementIndex = (uint8)CAN_17_MCMCAN_ZERO;
  }
  else
  {
    /* Copy the exact buffer element offset */
    BuffElementIndex = TxObjPtr[HthIndex].CanTxBuffIndx;
  }
  #else
  /* Check Tx buffer Type for Start address calculation and buffer status */
  /* [cover parentID={173F25BC-AF6C-48b3-B55B-05EEAE85E097}]
  [/cover] */
  /* Dedicated buffer type */
  if(CAN_17_MCMCAN_TX_DED_BUFFER == TxBuffType)
  {
    /* [cover parentID={D4AE8421-4CF4-4d62-9E9F-7A1DD412975E}]
    [/cover] */
    /* [cover parentID={58143A88-35A7-4220-95E2-5634FFA8CA5D}]
    Is buffer Free
    [/cover] */
    if((CanCoreState->CanTxMask[ControllerId] & BuffIndex) != BuffIndex)
    {
      /* [cover parentID={9A15F3CC-7C7F-45ba-BFF6-175F4FDC04C5}]
      [/cover] */
      /* [cover parentID={6CFB6E77-7FEB-49f7-AB24-C25B021E7CC5}]
      Set return value as Busy
      [/cover] */
      ApiStatus = CAN_BUSY;
      BuffElementIndex = (uint8)CAN_17_MCMCAN_ZERO;
    }
    else
    {
      /* Copy the exact buffer element offset */
      BuffElementIndex = TxObjPtr[HthIndex].CanTxBuffIndx;
    }
  }
  /* [cover parentID={4B7CC083-04A5-46ab-AE13-7E997C720E65}]
  [/cover] */
  else
  {
    /* [cover parentID={D2536F08-9EC3-43be-B165-98C3D0AB16ED}]
    [/cover] */
    /* [cover parentID={65F98883-86B6-4258-A828-57110E43C470}]
    Set Retuen value to BUSY
    [/cover] */
    /* Check TXQUEUE is Free or not */
    if(CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->TX.FQS.B.TFQF)
    {
      /* [cover parentID={9A15F3CC-7C7F-45ba-BFF6-175F4FDC04C5}]
      [/cover] */
      /* [cover parentID={01A8A051-9F22-4523-B752-240B11476297}]
      Return BUSY
      [/cover] */
      ApiStatus = CAN_BUSY;
      BuffElementIndex = (uint8)CAN_17_MCMCAN_ZERO;
    }
    else
    {
      /* Check for next free slot in Tx Queue */
      BuffElementIndex = Can_17_McmCan_lCheckQueueMask(ControllerId,
      CanConfig, &CanCoreState->CanTxMask[ControllerId]);

      /* [cover parentID={B9C5F877-61F3-4595-8A1F-B9AC3BBF9347}]
      Check for available slot
      [/cover] */
      if(BuffElementIndex == 0xFFU)
      {
        ApiStatus = CAN_BUSY;
      }
      else
      {
        /* [cover parentID={1716BABC-B97B-415f-B1C2-DB0AEBA80690}]
        block Tx slot
        [/cover] */
        BuffIndex = (uint32)(CAN_17_MCMCAN_TX_BUFF_MASK << BuffElementIndex);
      }
    }
  }
  #endif /* #if (CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_OFF) */


  #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
  /* [cover parentID={AD06B201-F7D9-4675-9F13-6D85FAFCA119}]
  Public ICOM is enabled
  [/cover] */
  /* [cover parentID={1D8F3616-5772-4d56-8ED1-88E3DD52331C}]
  Check controller is enabled for pretended networking
  [/cover] */
  if((ApiStatus == CAN_OK) && (TRUE == CanCoreState->CanIcomBlockWriteCalls[ControllerId]))
  {
    /* [cover parentID={65F98883-86B6-4258-A828-57110E43C470}]
    Return BUSY
    [/cover] */
    /* Set return value as Busy */
    ApiStatus = CAN_BUSY;
  }
  #endif

  /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
  is agreed as we refer to this address while passing the pointer and usage
  of this variable is limited only to this call*/
  TempPduInfo = &PduTemp;
  #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
   /* [cover parentID={1BAFBAB1-8880-4308-B735-36B46C11103E}]
  Check if the return value is CAN_OK
  [/cover] */
  if(ApiStatus == CAN_OK)
  {
    /* [cover parentID={2048703A-0E1A-4692-8533-D2CD73C8672E}]
    [/cover] */
    /* Call for trigger transmit status operations */
    ApiStatus = Can_17_McmCan_lUpdatePduInfo(HthIndex, TempPduInfo, PduInfo, CanConfig);
  }
  #else
  /*Store the Data pointer and Data length*/
  TempPduInfo->sdu = PduInfo->sdu;
  TempPduInfo->length = PduInfo->length;
  #endif /* #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON) */
  /* [cover parentID={EA70837F-4841-45b4-805E-B2D4569B0E3C}]
  Return value is CAN_OK
  [/cover] */
  if(CAN_OK == ApiStatus)
  {
    /* Lock Tx Queue slot - Only if all checks are passed */
    CanCoreState->CanTxMask[ControllerId] &= ~((uint32)1U << BuffElementIndex);

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* [cover parentID={A66B9A3D-FE07-43dc-A9DD-DC2BF806916B}]
    [/cover] */
    /* Check if FD support is enabled for the controller*/
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[ControllerId].CanFDSupport)
    {

      /* Set buffer size with FD support */
      TxWriteStartAddr = (TxWriteStartAddr + (  \
                          CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_FD * BuffElementIndex));
    }
    else
    #endif

    {
      /* Set buffer size with normal buffer */
      TxWriteStartAddr = TxWriteStartAddr + (  \
                         CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_NFD * BuffElementIndex);
    }
    /* Set with Transmit message buffer0 pointer */
    /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
    cast the address of Dedicated Tx buffer Type to the Tx buffer elements
    structure type */
    /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
    integer type. Permitted for special function registers.*/
    /* Set with Transmit message buffer0 pointer */
    TxMsgBufferPtr = (volatile Ifx_CAN_TXMSG*) TxWriteStartAddr ;
    /* Reset the T0 Msg buffer being accessed */
    TxMsgBufferPtr->T0.U = (uint32)CAN_17_MCMCAN_ZERO;
    /* Reset the RTR bit to avoid transmit messages triggered by
    remote transmission requests */
    TxMsgBufferPtr->T0.B.RTR = CAN_17_MCMCAN_ZERO;

    /* Check if the requested Id is extended or standard */
    /* [cover parentID={55A345E3-2E98-4e91-89C7-E3037EDD0F70}]
    [/cover] */
    /* [cover parentID={F5C8C104-A834-4ec7-AE45-811E40AC6921}]
    [/cover] */
    if(CAN_17_MCMCAN_EXTENDED_ID_SET == \
        (PduInfo->id & CAN_17_MCMCAN_EXTENDED_ID_SET))
    {
      /* [cover parentID={2009DBAB-A5F6-4e87-BE49-6C1912F107ED}]
      Store extended Id and set XTD bit
      [/cover] */
      /* Store extended Id and set XTD bit */
      TxMsgBufferPtr->T0.B.XTD = CAN_17_MCMCAN_ONE;
      TxMsgBufferPtr->T0.B.ID  = \
                                 ((PduInfo->id) & CAN_17_MCMCAN_EXTENDED_ID_MASK);
    }
    else
    {
      /* shift Standard Identifier to position 28..18 */
      TxMsgBufferPtr->T0.B.ID = (PduInfo->id & CAN_17_MCMCAN_STANDARD_ID_MASK) \
                                << CAN_17_MCMCAN_SHIFT_EIGHTEEN;
      /* Set Id Type as Standard */
      TxMsgBufferPtr->T0.B.XTD = CAN_17_MCMCAN_ZERO;
    }

    /* Set DLC with requested value */
    TxMsgDLC = TempPduInfo->length;
    TxMsgBufferPtr->T1.U = (uint32)CAN_17_MCMCAN_ZERO;

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* [cover parentID={6D99BD79-EB4F-4680-9BB1-231FF17DD5C8}]
    Check FD frame needs to be transmit
    [/cover] */
    /* Check FD frame need to be transmit */
    if(CAN_17_MCMCAN_ID_FD_ENABLE_SET == \
        (PduInfo->id & CAN_17_MCMCAN_ID_FD_ENABLE_SET))
    {
      /* Set Tx message with FD */
      TxMsgBufferPtr->T1.B.FDF = CAN_17_MCMCAN_ONE;
    }
    else
    {
      /* Set Tx message with Normal */
      TxMsgBufferPtr->T1.B.FDF = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={01141D65-83F6-4991-8F29-69D10AB63A94}]
    [/cover] */
    /* [cover parentID={54997C5D-274B-4e87-B39F-AE4FE0DACEF7}]
    Check CAN FD frames transmitted with bit rate switching
    [/cover] */
    if(CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->CCCR.B.BRSE)
    {
      /* Enable BRS */
      TxMsgBufferPtr->T1.B.BRS = CAN_17_MCMCAN_ONE;
    }
    else
    {
      /* Disable BRS */
      TxMsgBufferPtr->T1.B.BRS = CAN_17_MCMCAN_ZERO;
    }
    /* [cover parentID={D012BE97-F2DB-4de9-9D92-0151C2C196F5}]
    Check DLC is greater than maximum FD number of bytes
    [/cover] */
    /* Check DLC length and calculate the Hw support value if DLC > 24 */
    if (TempPduInfo->length > CAN_17_MCMCAN_SHIFT_TWENTYFOUR)
    {
      /* Set DLC length with Hw Supported value */
      TxMsgDLC = (((TempPduInfo->length + CAN_17_MCMCAN_FIFTEEN) >> \
                   CAN_17_MCMCAN_FOUR) + CAN_17_MCMCAN_ELEVEN);
    }
    /* [cover parentID={12A3AA1C-59FB-40de-B1B7-08B7FDF9B20C}]
    Check DLC is greater than normal message
    [/cover] */
    /* Check DLC length and calculate the Hw support value if DLC > 8 */
    else if (TempPduInfo->length > CAN_17_MCMCAN_EIGHT)
    {
      /* Set DLC length with Hw Supported value */
      TxMsgDLC = (((TempPduInfo->length + CAN_17_MCMCAN_THREE) >> \
                   CAN_17_MCMCAN_TWO) + CAN_17_MCMCAN_SIX);
    }
    else
    {
      /* Do Nothing */
    }
    #endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON) */

    /* Write DLC value to the T1 buffer */
    TxMsgBufferPtr->T1.B.DLC = TxMsgDLC;

    /* Write Event FIFO */
    TxMsgBufferPtr->T1.B.EFC = CAN_17_MCMCAN_ONE;

    /* Calculate Sw Obj Handle index */
    SwObjIndx = (uint16)((ControllerId * \
                          CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + BuffElementIndex);
    /* [cover parentID={E8DDD82E-BA0D-4872-84C4-0F67E7522B18}]
    [/cover] */
    /* Store sPduHandle Id for Tx confirmation process */
    CanCoreState->CanSwObjectHandlePtr[SwObjIndx] = PduInfo->swPduHandle;

    /* Write Message Marker with SwHandle Index (HTH Indx - Controller StartHTH
    Index) Index for the Tx confirmation process */
    TxMsgBufferPtr->T1.B.MM = BuffElementIndex;

    /* Set data byte position with zero index */
    DataBytePos = CAN_17_MCMCAN_ZERO;

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* Convert DLC length to Number of Data Bytes supported by Hw */
    TxMsgDLC = Can_17_McmCan_GblFdDlcConvDB[TxMsgDLC];
    #endif

    /* Set loop for write with Number of Data Byte */
    while(TxMsgDLC > DataBytePos)
    {
      #if (CAN_17_MCMCAN_FD_ENABLE == STD_OFF)
      /* Store the data to the Tx Buffer */
      TxMsgBufferPtr->DB[DataBytePos].U = TempPduInfo->sdu[DataBytePos];
      #else
      /* [cover parentID={54997C5D-274B-4e87-B39F-AE4FE0DACEF7}]
      Check byte position is with in the DLC
      [/cover] */
      if(DataBytePos < TempPduInfo->length)
      {
        /* [cover parentID={773E49C8-EEB4-4732-B164-BFEF1F4B02AB}]
        Store the data to the Tx Buffer
        [/cover] */
        /* Copy Message data to the Tx Buffer */
        TxMsgBufferPtr->DB[DataBytePos].U = TempPduInfo->sdu[DataBytePos];
      }
      else
      {
        /* Store with padding values to the Tx buffer */
        TxMsgBufferPtr->DB[DataBytePos].U = TxObjPtr[HthIndex].CanFdPaddValue;
      }
      #endif    /* (CAN_17_MCMCAN_FD_ENABLE == STD_OFF) */

      /* Set for next data byte */
      DataBytePos++;
    }
    /* [cover parentID={83E2512D-CB7A-4a18-9AEA-2FC0DCD6A5C7}]
    Request for the Transmission
    [/cover] */
    NodeRegAddressPtr->TX.BAR.U = (Ifx_UReg_32Bit)BuffIndex;
  }

  /* [cover parentID={85BA8CAE-3074-406e-B64D-361C3FAA9D22}]
  [/cover] */
  /* [cover parentID={6E1CBB09-0089-421f-AFD2-C08A9E6EAEB6}]
  [/cover] */
  /* Exit Critical Section */
  SchM_Exit_Can_17_McmCan_CanWrMO();

  return ApiStatus;
}
/*******************************************************************************
** Traceability     : [cover parentID={56493419-1258-4521-BF29-67D088707B8D}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lRxExtractData               **
**                    (                                                       **
**                      const uint8 HwControllerId,                           **
**                      const uint8 RxBuffIndex,                              **
**                      const Can_17_McmCan_RxBufferType RxBuffer,            **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState,     **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : This function extracts data from the message object    **
**                    and notifies upper layer                                **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HwControllerId - Associated CAN Controller             **
**                     RxBuffIndex - Bit Position within MSPND Register       **
**                     RxBuffer - receive buffer type used which can be       **
**                                       Dedicated RX/FIFO0/FIFO1             **
**                     CanConfig - Pointer to CAN driver configuration        **
**                     CanCoreState - Pointer to global structure             **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : none                                                **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lRxExtractData(const uint8 HwControllerId,
    const uint8 RxBuffIndex,
    const Can_17_McmCan_RxBufferType RxBuffer,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  const Can_17_McmCan_ControllerMsgRAMConfigType *MsgRamCfgPtr;
  volatile const Ifx_CAN_RXMSG *RxMsgRAMPtr;
  Can_HwType RxMailBoxInfo;
  PduInfoType RxPduInfo;
  const Can_17_McmCan_SIDFilterConfigType *CanSIDFilterPtr;
  const Can_17_McmCan_XIDFilterConfigType *CanXIDFilterPtr;
  uint32 RxReadStartAddr;
  uint32 RxReadOffsetAddr;
  uint16 FiltrStartIndx;
  uint8  RxMsgDLC;
  uint8  RxMsgFilterIndx;
  uint8  RxMsgCnt;
  Can_IdType RxMsgId;
  #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  uint8 CanRxMessageData[CAN_17_MCMCAN_NOOF_CONTROLLER] \
  [CAN_17_MCMCAN_MAX_FD_LENGTH];
  #else
  uint8 CanRxMessageData[CAN_17_MCMCAN_NOOF_CONTROLLER] \
  [CAN_17_MCMCAN_MAX_DATA_LENGTH];
  #endif
  #if (CAN_17_MCMCAN_LPDU_RX_CALLOUT == STD_ON)
  boolean RetVal;
  #endif

  /* Set Message RAM configuration pointer with configuration address */
  MsgRamCfgPtr = CanConfig->CanControllerMsgRAMMapConfigPtr;
  /* Check requested buffer type */
  /* [cover parentID={CC1A3A21-1E66-4fa4-9FCB-BD30E3B458C4}]
  Requested Buffer type
  [/cover] */
  if(CAN_17_MCMCAN_RX_FIFO0 == RxBuffer)
  {
    /* Read Rx Message RAM start address of RXFIFO0 */
    RxReadStartAddr = MsgRamCfgPtr[HwControllerId]. \
                      CanControllerMsgRAMMap[CAN_17_MCMCAN_TWO];
  }
  else if(CAN_17_MCMCAN_RX_FIFO1 == RxBuffer)
  {
    /* Read Rx Message RAM start address of RXFIFO1 */
    RxReadStartAddr = \
                      MsgRamCfgPtr[HwControllerId]. \
                      CanControllerMsgRAMMap[CAN_17_MCMCAN_THREE];
  }
  else
  {
    /* Read Rx Message RAM start address of dedicated buffer */
    RxReadStartAddr = \
                      MsgRamCfgPtr[HwControllerId]. \
                      CanControllerMsgRAMMap[CAN_17_MCMCAN_FOUR];
  }
  /* Set OFFSET address with  buffer position */
  RxReadOffsetAddr = RxBuffIndex;
  /* [cover parentID={33A22D31-6C26-4717-925E-939EE7EE3DCB}]
  Start Address value
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != RxReadStartAddr)
  {
    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* Check if FD support is enabled for the controller*/
    /* [cover parentID={0ABD541D-73FA-404a-9AD7-1AFE9DE0D81C}]
    IS CAN FD support Enable
    [/cover] */
    if(TRUE == \
        CanConfig->CanControllerConfigPtr[HwControllerId].CanFDSupport)
    {
      /* Load the data receive message buffer address */
      /*IFX_MISRA_RULE_11_03_STATUS=The pointer cast is used to
      cast the address of Dedicated Rx buffer Type to the Rx buffer elements
      structure type  */
      RxReadStartAddr = RxReadStartAddr + (RxReadOffsetAddr
                                           * CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_FD);
    }
    else
    #endif

    {

      /* Load the data receive message buffer address */
      RxReadStartAddr = RxReadStartAddr + (RxReadOffsetAddr \
                                           * CAN_17_MCMCAN_MSG_RAM_ELEMENT_SIZE_NFD);
    }

    /* Load the data receive message buffer address */
    /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to cast the
    address of Dedicated Rx buffer Type to the Rx buffer elements structure
    type */
    /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and integer
    type. Permitted for special function registers.*/
    RxMsgRAMPtr = (volatile Ifx_CAN_RXMSG*) RxReadStartAddr;
    /* Extract DLC information */
    RxMsgDLC = RxMsgRAMPtr->R1.B.DLC;
    /* Read Filter Index */
    RxMsgFilterIndx = RxMsgRAMPtr->R1.B.FIDX;

    /* Extract receive Msg.ID  and ID Type */
    /* [cover parentID={E48CA5D5-56B5-4124-87E7-E4FC09F758C9}]
      Message Id not configured
      [/cover] */
    if (CAN_17_MCMCAN_BIT_CLEAR_VAL == RxMsgRAMPtr->R0.B.XTD)
    {
      /* Read received message Id and Message type */
      RxMsgId = RxMsgRAMPtr->R0.B.ID >> CAN_17_MCMCAN_SHIFT_EIGHTEEN;
      /* First index value of standard filter set */
      FiltrStartIndx = CanConfig-> \
                       CanControllerConfigPtr[HwControllerId]. \
                       CanControllerMOMap[CAN_17_MCMCAN_TWO];
      /* Set SID filter list Address */
      CanSIDFilterPtr = CanConfig->CanSIDFilterConfigPtr;
      /* Read and store the HOH Id*/
      RxMailBoxInfo.Hoh = \
                          CanSIDFilterPtr[FiltrStartIndx + RxMsgFilterIndx].CanSidHwObjId;
    }
    else
    {
      /* [cover parentID={F220106E-922D-4368-9EBD-0C83BDB56ABB}]
      Set message ID type as extended and extract the message ID
      [/cover] */
      /* Set message ID type as Extended */
      RxMsgId = CAN_17_MCMCAN_SET_XTD_BIT_MASK;
      /* Copy message ID  */
      RxMsgId |= (Can_IdType)RxMsgRAMPtr->R0.B.ID;
      /* First index value of Extended filter set */
      FiltrStartIndx = CanConfig-> \
                       CanControllerConfigPtr[HwControllerId]. \
                       CanControllerMOMap[CAN_17_MCMCAN_FOUR];
      /* Set XID filter list Address */
      CanXIDFilterPtr = CanConfig->CanXIDFilterConfigPtr;
      /* Read and store the HOH Id*/
      RxMailBoxInfo.Hoh = \
        CanXIDFilterPtr[FiltrStartIndx+RxMsgFilterIndx].CanXidHwObjId;
    }
    /* Read receive data information from R1 RAM element */
    /* Read FD status of received Message */
    /* [cover parentID={4A6EFA0F-F343-460b-B81F-8C7F73B53812}]
      Set FD status of MsgId
      [/cover] */
    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    if( CAN_17_MCMCAN_BIT_SET_VAL == RxMsgRAMPtr->R1.B.FDF)
    {
      /* [cover parentID={3CBFC192-2B42-491a-95FA-C9EBE7401C43}]
      Set FD bit of MsgId
      [/cover] */
      /* Set FD bit of MsgId */
      RxMsgId |= (Can_IdType)CAN_17_MCMCAN_SET_FD_MASK;
    }
    #endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON) */
    /* Clear to read from Byte0 */
    RxMsgCnt = CAN_17_MCMCAN_ZERO;

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
    /* Convert DLC length to Number of Data Bytes supported by Hw */
    RxMsgDLC = Can_17_McmCan_GblFdDlcConvDB[RxMsgDLC];
    #endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON) */

    /* Copy Message from RAM section to local data buffer */
    while(RxMsgCnt < RxMsgDLC)
    {
      /* copy data from byte0 to n the buffer */
      CanRxMessageData[HwControllerId][RxMsgCnt] = RxMsgRAMPtr->DB[RxMsgCnt].U;
      /* Increment to point next byte */
      RxMsgCnt++;
    }
    /* Call CanIf_RxIndication function with receive message information */
    RxMailBoxInfo.CanId = RxMsgId;
    RxMailBoxInfo.ControllerId = HwControllerId;
    RxPduInfo.SduLength = RxMsgDLC;
    /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
    is agreed as we refer to this address while passing the pointer and usage
    of this variable is limited only to this call*/
    RxPduInfo.SduDataPtr = \
                           &CanRxMessageData[HwControllerId][CAN_17_MCMCAN_ZERO];

    /* [cover parentID={2BCD3D07-EC74-4f1b-AAD8-90099FBCD5FE}]
    CAN public ICOM support is enabled
    [/cover] */
    #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
    /* [cover parentID={F4D157C9-719B-425f-B679-E0410C730006}]
    Check ICOM is enabled for current controller
    [/cover] */
    /* Check ICOM enabled */
    if(TRUE == CanCoreState->CanIcomEnableStatusPtr[HwControllerId])
    {
      /* Call Message validate function */
      /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
      is agreed as we refer to this address while passing to the called function
      as a parameter (pass by value).Making this global limits the requirement
      of reentrancy in this function.*/
      Can_17_McmCan_lIcomValidateMsg(&RxMailBoxInfo, &RxPduInfo, \
                                     CanCoreState, CanConfig);
    }
    else
    #else
    UNUSED_PARAMETER(CanCoreState);
    #endif /* #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */

    {
      /* [cover parentID={596D7688-6694-4d2d-B461-7988F57A09DB}]
      Is CAN LPdu callout function configured?
      [/cover] */
      #if (CAN_17_MCMCAN_LPDU_RX_CALLOUT == STD_ON)
      /* Call the LPDU callout function to check the return value */
      /* [cover parentID={32C3F587-53C3-498a-A9E0-9CA7367E5E5B}]
       [/cover] */
      /* Does Can Lpdu callout returns true? */
      /* [cover parentID={525B2DC0-A182-4d0d-89B2-A64FEE86FE32}]
       [/cover] */
      RetVal = Can_17_McmCan_kGblConfigPtr->CanLPduRxCalloutFuncPtr((uint8)(RxMailBoxInfo.Hoh),
                                                                      RxMsgId,
                                                                      RxMsgDLC,
                                                                      RxPduInfo.SduDataPtr);

      /* Notification function CanIf_RxIndication should be called only if the
         callout function returns true */
      /* [cover parentID={3860A3E0-DB04-40d3-9C1B-55D4BA0E8418}]
      Can Lpdu callout returns true?
      [/cover] */
      if(RetVal == TRUE)
      #endif /* #if (CAN_17_MCMCAN_LPDU_RX_CALLOUT == STD_ON) */
      {
        /* get logical Id to be passed to upper layer */
        RxMailBoxInfo.ControllerId = \
                                    CanConfig->CanControllerIndexingPtr[HwControllerId];

        /* [cover parentID={F495F68A-E87C-4161-BBF1-02187F5BF35D}]
        Notify to reception of message to CAN IF layer
        [/cover] */
        /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
        is agreed as we refer to this address while passing to the called function
        as a parameter (pass by value).Making this global limits the requirement
        of reentrancy in this function.*/
        CanIf_RxIndication(&RxMailBoxInfo, &RxPduInfo);
      }
    }
  }
}
/*******************************************************************************
** Traceability     : [cover parentID={C3CEA0E7-2745-4d57-9B07-A608552CF60F}] **                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lSIDFilter_Config      **
**                    (                                                       **
**                      const uint8 ControllerId,                             **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes receive message objects                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId - Indexing of core specific CAN Controller**
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lSIDFilter_Config (const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  /* [cover parentID={0FA49C2C-6E8D-44eb-B65F-AE22B3660BEF}]
  Can_17_McmCan_lSIDFilter_Config
  [/cover] */
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  volatile Ifx_CAN_STDMSG *SIDFilterDataPtr;
  const Can_17_McmCan_SIDFilterConfigType *CanSIDPtr;
  uint32 SIDFilterStartAddr;
  uint32 SIDFilterRealAddr;
  uint32 SIDFiltrOffset;
  uint16 SIDFiltrIndx;
  uint16 SIDFiltrStartIndx;
  uint16 SIDFiltrEndIndx;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = CanConfig-> \
                      CanControllerConfigPtr[ControllerId].CanNodeAddress;
  /* Set SID filter list Address */
  CanSIDPtr = CanConfig->CanSIDFilterConfigPtr;
  /* Set with first SID filter Index */
  SIDFiltrStartIndx = \
                      CanConfig->CanControllerConfigPtr[ControllerId]. \
                      CanControllerMOMap[CAN_17_MCMCAN_TWO];
  /* Set with End SID filter Index */
  SIDFiltrEndIndx = \
                    CanConfig->CanControllerConfigPtr[ControllerId]. \
                    CanControllerMOMap[CAN_17_MCMCAN_THREE];
  /* Read Standard Identifier filter Start address */
  SIDFilterStartAddr = CanConfig-> \
                       CanControllerMsgRAMMapConfigPtr[ControllerId]. \
                       CanControllerMsgRAMMap[CAN_17_MCMCAN_ZERO];
  /* Load with Start address offset */
  NodeRegAddressPtr->SIDFC.B.FLSSA = \
                                     ((uint16)((SIDFilterStartAddr & \
                                         CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO));
  /* Set with Standard section RAM size */
  NodeRegAddressPtr->SIDFC.B.LSS = (uint8) SIDFiltrEndIndx;
  /* Set Offset with First element */
  SIDFiltrOffset = CAN_17_MCMCAN_ZERO;
  /* Set with all standard filter configuration */
  for(SIDFiltrIndx = SIDFiltrStartIndx; SIDFiltrIndx < \
      (SIDFiltrEndIndx + SIDFiltrStartIndx); SIDFiltrIndx++)
  {
    /* Set with SID element address */
    SIDFilterRealAddr = SIDFilterStartAddr + (CAN_17_MCMCAN_RAM_BYTES * \
                        SIDFiltrOffset);
    /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
    cast the address of Standard filter Type to the Standard filter elements
    structure type */
    /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
    integer type. Permitted for special function registers.*/
    SIDFilterDataPtr = (volatile Ifx_CAN_STDMSG*)SIDFilterRealAddr;
    /* Set Filter Data to the Filter element */
    SIDFilterDataPtr->S0.U = CanSIDPtr[SIDFiltrIndx].CanSIDFiltEleS0;
    /* Increment for access next element */
    SIDFiltrOffset++;
  }
}

/*******************************************************************************
** Traceability   : [cover parentID={FCE63161-A42C-41c4-9951-5F0A58190ED5}]   **
**                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lXIDFilter_Config      **
**                    (                                                       **
**                      const uint8 ControllerId,                             **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : Initializes receive message objects                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId - Indexing of core specific CAN Controller**
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lXIDFilter_Config (const uint8 ControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  /* [cover parentID={979EB8B7-1593-4049-814A-E96E01AA31D7}]
  Can_17_McmCan_lXIDFilter_Config
  [/cover] */
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  volatile Ifx_CAN_EXTMSG *XIDFilterPtr;
  const Can_17_McmCan_XIDFilterConfigType *CanXIDPtr;
  uint32 XIDFilterStartAddr;
  uint32 XIDFilterRealAddr;
  uint32 XIDFilterData;
  uint16 XIDFiltrIndx;
  uint16 XIDFiltrStartIndx;
  uint16 XIDFiltrEndIndx;
  uint8 XIDOffsetIndx;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;
  /* Set with start Index */
  XIDFiltrStartIndx = \
                      CanConfig->CanControllerConfigPtr[ControllerId]. \
                      CanControllerMOMap[CAN_17_MCMCAN_FOUR];
  /* Set with end Index */
  XIDFiltrEndIndx = \
                    CanConfig->CanControllerConfigPtr[ControllerId]. \
                    CanControllerMOMap[CAN_17_MCMCAN_FIVE];
  /* Set XID filter list Address */
  CanXIDPtr = CanConfig->CanXIDFilterConfigPtr;
  /* Read Extended Identifier filter Start address */
  XIDFilterStartAddr = CanConfig-> \
                       CanControllerMsgRAMMapConfigPtr[ControllerId]. \
                       CanControllerMsgRAMMap[CAN_17_MCMCAN_ONE];
  /* Load with Start address offset */
  NodeRegAddressPtr->XIDFC.B.FLESA = \
                                     ((uint16)((XIDFilterStartAddr & \
                                         CAN_17_MCMCAN_RAM_ADDRESS_OFFSET_MASK) >> CAN_17_MCMCAN_TWO));
  /* Set with Extended section RAM size */
  NodeRegAddressPtr->XIDFC.B.LSE = (uint8)XIDFiltrEndIndx;
  /* Set with first Filter Index */
  XIDOffsetIndx = CAN_17_MCMCAN_ZERO;
  /* Set with all Extended filter configuration */
  for(XIDFiltrIndx = XIDFiltrStartIndx; XIDFiltrIndx <
      (XIDFiltrStartIndx + XIDFiltrEndIndx); XIDFiltrIndx++)
  {
    /* Set element offset address F0 */
    XIDFilterRealAddr = XIDFilterStartAddr + (XIDOffsetIndx * \
                        CAN_17_MCMCAN_RAM_BYTES * CAN_17_MCMCAN_TWO);
    /* Read Filter F0 configuration data */
    XIDFilterData = CanXIDPtr[XIDFiltrIndx].CanXIDFiltEleF0;
    /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
    cast the address of Extended filter Type to the extended filter elements
    structure type */
    /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
    integer type. Permitted for special function registers.*/
    XIDFilterPtr = (volatile Ifx_CAN_EXTMSG*) XIDFilterRealAddr;
    /* Set Filter element F0 with configuration data */
    XIDFilterPtr->F0.U = XIDFilterData;
    /* Read Filter F1 configuration data */
    XIDFilterData = CanXIDPtr[XIDFiltrIndx].CanXIDFiltEleF1;
    /* Set Filter element F1 with configuration data */
    XIDFilterPtr->F1.U = XIDFilterData;
    /* Increment to next offset */
    XIDOffsetIndx++;
  }
}

/*******************************************************************************
         Receive ,Transmit ,Wakeup and Bus-Off  Handler
*******************************************************************************/
/*CYCLOMATIC_Can_17_McmCan_lReceiveHandler_JUSTIFICATION: This Function has
cyclomatic Complexity of 16 as it handles receive of dedicated, FIFO0 and FIFO1
as part of same function and it is logical to have it same functions since all
receive mechanims are handled in this function.
*/
#if(((CAN_17_MCMCAN_RX_POLLING_PROCESSING == STD_ON)&& \
  (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF))|| \
  (CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)|| \
  ((CAN_17_MCMCAN_WU_POLLING_PROCESSING == STD_ON) && \
  (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)))
/*******************************************************************************
** Traceability     : [cover parentID={DA288EF6-8632-4470-B2DB-98E7946FF40E}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lReceiveHandler              **
**                    (                                                       **
**                      const uint8 HwControllerId,                           **
**                      const uint8 CheckBuffType,                            **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState,     **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : - This function is the core part of the interrupt      **
**                      service routine for Receive Interrupt                 **
**                    - In case of successful reception event, this           **
**                      function calls the function                           **
**                     Can_17_McmCan_lRxExtractData to extract data from the  **
**                                message object and notify upper layer       **
**                    - This function is also called by the polling function  **
**                      Can_17_McmCan_MainFunction_Read for controllers       **
**                      configured with CanRxProcessing as POLLING            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HwControllerId - Associated CAN Controller             **
**                    CheckBuffType -  Data received buffer type              **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lReceiveHandler(const uint8 HwControllerId,
    const uint8 CheckBuffType,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint32 RxNDATValue;
  uint32 RxNDATValue0;
  uint32 RxNDATValue1;
  uint32 RxDedMask;
  uint8 RxHandleBuffPos;
  uint8 RegSet;
  uint8 loopExitFlag = 0;
  uint8 FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[HwControllerId].CanRxFIFO0Size;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[HwControllerId].CanNodeAddress;

  /* [cover parentID={2A8D12D0-8282-405f-B6F6-14ADF180E0ED}]
  Recieve data buffer is dedicated Type
  [/cover] */
  if((CheckBuffType == CAN_17_MCMCAN_ONE) || \
     (CheckBuffType == CAN_17_MCMCAN_ZERO))
  {
    RxNDATValue0 = NodeRegAddressPtr->NDAT1.U;
    RxNDATValue1 = NodeRegAddressPtr->NDAT2.U;
    /* Set with first bit position*/
    RxHandleBuffPos = CAN_17_MCMCAN_ZERO;
    /* Read first NDAT0 buffer value */
    RxNDATValue = RxNDATValue0;

    /* Read NDAT1 and NDAT2 */
    /* [cover parentID={3B9767A1-E113-45c6-8D5B-D04491A4B8FD}]
    Loop to read the data registers
    [/cover] */
    for(RegSet = CAN_17_MCMCAN_ZERO; RegSet < CAN_17_MCMCAN_TWO ; RegSet++ )
    {
      /* Set mask value */
      RxDedMask = CAN_17_MCMCAN_ONE;
      /* Check any one of the Rx dedicated buffer is received with new
      data */
      /* [cover parentID={C3D288D2-4D80-403f-9020-4FD74933F9C7}]
      Loop to run when Rx dedicated buffer is received with new data
      [/cover] */
      /* [cover parentID={C58BE040-07E4-4009-B353-D8623E3DE0FC}]
      loop to get the Rx dedicated buffers
      [/cover] */
      while(CAN_17_MCMCAN_DATA_ZERO != (Ifx_UReg_32Bit)RxNDATValue)
      {
        /* Check buffer with mask position is received data or not */
        /* [cover parentID={F199317D-9898-4000-BF43-AC12EF98E323}]
        Rx data with read mask position has recieved data
        [/cover] */
        if(CAN_17_MCMCAN_DATA_ZERO != (Ifx_UReg_32Bit)(RxNDATValue & RxDedMask))
        {
          /* Invoke Can_17_McmCan_lRxExtractData function for retrieve data from
          message */
          /* [cover parentID={1C5EF04F-1946-4e93-A9BA-4D853C26D687}]
          Retrieve data from recieved message
          [/cover] */
          Can_17_McmCan_lRxExtractData(HwControllerId, RxHandleBuffPos, \
                                       CAN_17_MCMCAN_RX_DED_BUFFER, CanCoreState, CanConfig);
          /* Clear the set bit position of NDAT Reg value */
          RxNDATValue = RxNDATValue & (~RxDedMask);
        }
        /* Set mask for check next buffer status */
        RxDedMask = RxDedMask << CAN_17_MCMCAN_ONE;
        /* Increment bit position */
        RxHandleBuffPos++;
      }
      /* After completion of read NDAT1 load NDAT_value with NDAT2 register
         value */
      RxNDATValue = RxNDATValue1;
      /* Set Buffer position from value 32 for NDAT2 */
      RxHandleBuffPos = CAN_17_MCMCAN_THIRTY_TWO;
    }
    /* Clear NDAT Register for receive new message */
    NodeRegAddressPtr->NDAT1.U = RxNDATValue0;
    NodeRegAddressPtr->NDAT2.U = RxNDATValue1;
  }
    /* [cover parentID={C1B9A52E-BCC8-448e-A838-F4B44269384B}]
  Recieve buffer type is Rx FIFO 0
 [/cover] */
  if((CheckBuffType == CAN_17_MCMCAN_TWO) || \
     (CheckBuffType == CAN_17_MCMCAN_ZERO))
  {
    /* Check Received CAN message is lost or not */
    /* [cover parentID={6B2EFACA-75BB-44e0-A0A2-7BE1D2A73CB5}]
    Check received CAN message is lost
    [/cover] */
    if(NodeRegAddressPtr->IR.B.RF0L != CAN_17_MCMCAN_BIT_CLEAR_VAL)
    {
      /* [cover parentID={79D189C7-BDAC-473f-B6F8-2E7C41F4886E}]
     DET is enabled
     [/cover] */
      #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* [cover parentID={4EAE11EC-14C9-4470-9B6F-F8715D79FC8C}]
      Det error CAN_E_DATALOST
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_RECEIVEHANDLER, CAN_17_MCMCAN_E_DATALOST);
      #endif
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_RF0L_CLEAR;
    }

    /* [cover parentID={2ED59A98-5533-4f06-A457-DCE6B1F2057E}]
    Multiplexed Handling
    [/cover] */
    /*Loop till all messages are read in RXFIFO0 */
    /* [cover parentID={16B9B0F2-002C-4ea9-82B6-30AFF0D6D767}]
    Loop till all messages are read in Rx FIFO 0 or read max configured messages in FIFO
    [/cover] */
    while((NodeRegAddressPtr->RX.F0S.B.F0FL > 0U) && (loopExitFlag < FifoSize))
    {
      loopExitFlag++;
      /* a. Read all FIFO slots and acknowledge to free up slots
         b. Notify upper layer */
      /* Read Rx FIFO 0 Get Index */
      RxHandleBuffPos = NodeRegAddressPtr->RX.F0S.B.F0GI;
      /* Invoke data retrieve function */
      Can_17_McmCan_lRxExtractData(HwControllerId, RxHandleBuffPos, \
                                   CAN_17_MCMCAN_RX_FIFO0, CanCoreState, CanConfig);
      /* Set read Acknowledge of buffer */
      NodeRegAddressPtr->RX.F0A.U = RxHandleBuffPos;
    }
  }
  /* [cover parentID={718D7E08-4563-4e45-A662-4E14CD08E759}]
Recieve buffer type is Rx FIFO 1
[/cover] */
  if((CheckBuffType == CAN_17_MCMCAN_THREE) || \
      (CheckBuffType == CAN_17_MCMCAN_ZERO))
  {
    FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[HwControllerId].CanRxFIFO1Size;
    loopExitFlag = 0;
    /* Check Received CAN message is lost or not */
    /* [cover parentID={AE3A81FA-3AA2-4049-A631-C6031F38A6F6}]
    Received CAN message is lost
   [/cover] */
    if(NodeRegAddressPtr->IR.B.RF1L != CAN_17_MCMCAN_BIT_CLEAR_VAL)
    {
      /* [cover parentID={6CC7AA60-5D45-457e-9BC0-AD3EE6F3FEBA}]
      DET is enabled
      [/cover] */
      #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* [cover parentID={6ED3CB77-D482-454c-9CCB-E1817E119ED2}]
      Det error CAN_E_DATALOST
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_RECEIVEHANDLER, CAN_17_MCMCAN_E_DATALOST);
      #endif
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_RF1L_CLEAR;
    }

    /* Loop till all messages are read in RXFIFO1 */
    /* [cover parentID={2671F589-C647-4d56-8B99-1049990268CD}]
    Loop till all messages are read in Rx FIFO 1 or read max configured messages in FIFO
    [/cover] */
    while((NodeRegAddressPtr->RX.F1S.B.F1FL >0U) && (loopExitFlag < FifoSize))
    {
      loopExitFlag++;
      /* a. Read all FIFO slots and acknowledge to free up slots
         b. Notify upper layer */
      /* Read Rx FIFO 1 Get Index */
      RxHandleBuffPos = NodeRegAddressPtr->RX.F1S.B.F1GI;
      /* Invoke data retrieve function */
      /* [cover parentID={0EAB950D-3211-444b-8055-EB49093ACBA6}]
      Retrieve data from recieved message
      [/cover] */
      /* [cover parentID={9649DFF9-D089-49dd-BE3F-BC035BC50E3F}]
      Retrieve data from recieved message
     [/cover] */
      Can_17_McmCan_lRxExtractData(HwControllerId, RxHandleBuffPos, \
                                   CAN_17_MCMCAN_RX_FIFO1, CanCoreState, CanConfig);
      /* Set read Acknowledge of buffer */
      NodeRegAddressPtr->RX.F1A.U = RxHandleBuffPos;
    }
  }
}
#endif /* #if(((CAN_17_MCMCAN_RX_POLLING_PROCESSING == STD_ON)&& \
  (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_OFF))|| \
  (CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)|| \
  ((CAN_17_MCMCAN_WU_POLLING_PROCESSING == STD_ON) && \
  (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON))) */

#if((CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)|| \
    ((CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)&& \
    (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF)))
/*******************************************************************************
** Traceability     : [cover parentID={BC51BE45-5022-41aa-B8D7-131F161BD213}] **
**                    [cover parentID={5178FA72-B5C6-4672-B206-324EA587C343}] **                                                                **
**  Syntax           : static void Can_17_McmCan_lTxEventHandler              **
**                    (                                                       **
**                      const uint8 HwControllerId,                           **
**                     const Can_17_McmCan_CoreStatesType* const CanCoreState,**
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : - This function shall read all the Tx event FIFO       **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HwControllerId - Associated CAN Controller             **
**                     CanConfig - Pointer to CAN driver configuration        **
**                     CanCoreState - Pointer to global structure             **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lTxEventHandler (const uint8 HwControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  volatile const Ifx_CAN_TXEVENT *TxEvntReadData;
  const Can_17_McmCan_ControllerMsgRAMConfigType*MsgRamCfgPtr;
  uint32 TxEvntFIFOstartAddr;
  uint32 TxEvntFIFORealAddr;
  uint32 TxEvntFIFOGetIndex;
  Can_HwHandleType TxHandleIndx;
  uint8 TxEvntFillLvlVal;
  uint8 LoopExitFlag;
  uint16 SwObjIndx;
  uint32 TxSlotIndex;
  PduIdType pduHandle;
  /* Set pointer variable with Message RAM Configuration structure */
  MsgRamCfgPtr = CanConfig->CanControllerMsgRAMMapConfigPtr;
  /* Read the exact Node Start address */
  NodeRegAddressPtr =
    CanConfig->CanControllerConfigPtr[HwControllerId].CanNodeAddress;
  /* Raise DET if LOST is raised as HW event */
  /* [cover parentID={7F23492C-21EE-4c9b-A457-5502B0BDB7FE}]
  Is TX event FULL?
  [/cover] */
  if(CAN_17_MCMCAN_BIT_CLEAR_VAL != NodeRegAddressPtr->IR.B.TEFL)
  {
      /* [cover parentID={BBE5518D-5FD2-42de-B2B3-4DFB04088FED}]
      Is DET On?
      [/cover] */
       #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* Report to DET */
      /* [cover parentID={C0C93AA6-38A1-4024-8B01-DF9C1E165A82}]
      Raise DET CAN_17_MCMCAN_E_DATALOST
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_TRANSMITHANDLER, CAN_17_MCMCAN_E_DATALOST);
      #endif
      /* Clear TEFN and TEFL flags as well if SET */
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_TEFF_TEFL_CLEAR;
  }
    /* Set Tx Event FIFO Start Address */
  TxEvntFIFOstartAddr =
    MsgRamCfgPtr[HwControllerId].CanControllerMsgRAMMap[CAN_17_MCMCAN_FIVE];
  /* Set safety loop exit flag with value zero */
  LoopExitFlag = CAN_17_MCMCAN_ZERO;
  /* Read Fill Level value */
  TxEvntFillLvlVal =  NodeRegAddressPtr->TX.EFS.B.EFFL;
  /* [cover parentID={5B6E431C-9DFA-4ba3-8C9B-04CEE48021CB}]
  Read Tx event FIFO register for read No of stored Events and
  CAN_NOOF_TX_HW_BUFF_AVAILABLE > LoopExitFlag
  [/cover] */
  while((CAN_17_MCMCAN_ZERO != TxEvntFillLvlVal) && \
        (CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE > LoopExitFlag))
  {
    /* Read TxEvent buffer index */
    TxEvntFIFOGetIndex = NodeRegAddressPtr->TX.EFS.B.EFGI;
    /* Set Real Event FIFO element address */
    TxEvntFIFORealAddr = TxEvntFIFOstartAddr + (TxEvntFIFOGetIndex * \
                         (uint32)CAN_17_MCMCAN_EIGHT);
    /* Load EventFIFO element address */
    /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
    cast the address of Tx Event Type to the event elements  structure type */
    /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
    integer type. Permitted for special function registers.*/
    TxEvntReadData = (volatile Ifx_CAN_TXEVENT*) TxEvntFIFORealAddr;
    /* Read Message Marker */
    TxHandleIndx = (Can_HwHandleType) TxEvntReadData->E1.B.MM;
    /* Calculate Sw Obj Handle index */
    SwObjIndx = (uint16)((HwControllerId * \
                          CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + TxHandleIndx);

    /* Release Tx buffer before notification by copying the id */
    pduHandle = CanCoreState->CanSwObjectHandlePtr[SwObjIndx];

    TxSlotIndex = ((uint32)1U << TxHandleIndx);

    /* [cover parentID={F0AF9C4C-B453-4615-87CC-3A7390ACCAEB}]
    Check if this element belong to configured Tx objects
    [/cover] */
    /* Check if this element belong to configured Tx objects */
    if((TxSlotIndex & CanCoreState->BackupCanTxMask[HwControllerId]) == TxSlotIndex)
    {
    /* [cover parentID={212700AB-D789-4c69-8030-ADEDE87932CD}]
      Can_write reentrancy
      [/cover] */
      /* Enter Critical Section */
      SchM_Enter_Can_17_McmCan_CanWrMO();

      /* Release Transmit object */
      CanCoreState->CanTxMask[HwControllerId] |= TxSlotIndex;

    /* [cover parentID={2417BD54-88B9-4c59-A38D-396F170D0445}]
      Can_write reentrancy
      [/cover] */
      /* Exit Critical Section */
      SchM_Exit_Can_17_McmCan_CanWrMO();
    }

    /* Acknowledge shall set to free the Tx Event FIFO */
    NodeRegAddressPtr->TX.EFA.B.EFAI = (uint8) TxEvntFIFOGetIndex;

  /* Set with Exact PduHandle Stored Index value */
    /* [cover parentID={CF5C13A2-E51C-4f13-8152-B8B4591F3FFF}]
    Call Transmission confirmation to upper-layer with L-PDU handle
    [/cover] */
    CanIf_TxConfirmation(pduHandle);


    /* Increment exit loop condition for safety */
    LoopExitFlag++;

    /* Read Fill Level value */
    TxEvntFillLvlVal =  NodeRegAddressPtr->TX.EFS.B.EFFL;
  }
}
#endif /* #if((CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)|| \
  ((CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)&& \
  (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_OFF))) */

/*******************************************************************************
** Traceability     : [cover parentID={C5F1A7D3-7724-40d7-96B8-040E817345EE}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lBusOffHandler               **
**                    (                                                       **
**                      const uint8 HwControllerId,                           **
**                      Can_17_McmCan_CoreStatesType* const CanCoreState,     **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : The function used to handle the controller while in    **
**                    bus-off state                                           **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HwControllerId - Associated CAN Controller             **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lBusOffHandler(const uint8 HwControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  const volatile Ifx_CAN_N *NodeRegAddressPtr;
  Ifx_UReg_32Bit TempInitStatus;
  uint8 LogicalHwControllerId;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[HwControllerId].CanNodeAddress;
  /* Read Controller mode status */
  TempInitStatus = NodeRegAddressPtr->CCCR.B.INIT;
  /* [cover parentID={D5432A7A-C352-4415-9D98-2EF0777415BC}]
  Controller in Bus OFF state
  [/cover] */
  /* Check the M_CAN is in BusOff state or not */
  /* Note: Bus off notification to application will be provided only once   */
  if( (CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->PSR.B.BO) && \
      (CAN_17_MCMCAN_BIT_SET_VAL == TempInitStatus) &&
      (CanCoreState->CanControllerModePtr[HwControllerId] == CANIF_CS_STARTED))
  {
    /* [cover parentID={E538497B-FAF6-4ab2-B3BE-D071E5CB386D}]
    [/cover] */
    /* invoke function to cancel all pending request */
    Can_17_McmCan_lCancelPendingTx(HwControllerId, CanCoreState, CanConfig);

    /* [cover parentID={CD1DD19F-D41B-4aa2-B119-CA627803E165}]
    Set Controller mode to CANIF_CS_STOPPED state
    [/cover] */
    /* Set controller mode with STOP state */
    CanCoreState->CanControllerModePtr[HwControllerId] = CANIF_CS_STOPPED;
    LogicalHwControllerId = CanConfig->CanControllerIndexingPtr[HwControllerId];
    /* [cover parentID={838455CD-FCFA-45e0-B4C7-8FBEBACB92A0}]
    Call Bus_Off notification function to notify upper layer
    [/cover] */
    /* Call Bus_Off notification function to notify upper layer */
    CanIf_ControllerBusOff(LogicalHwControllerId);
  }
}

#if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)
/*******************************************************************************
**  Traceability     : [cover parentID={8AF91135-3E29-4afe-911C-23E827A3053F}]**                                                                            **
**  Syntax           :  void Can_17_McmCan_lTxPeriodHandler                   **
**                    (                                                       **
**                      const uint8 HthRefPeriods                             **
**                    )                                                       **
**                                                                            **
**  Description      :  Function used for handling Tx confirmation process    **
**                      with multiple write period                            **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HthRefPeriods - The write operation cyclic period      **
**                    configuration index                                     **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_lTxPeriodHandler (const uint8 HthRefPeriods)
{
  /* [cover parentID={F9523C12-9D77-4f10-9665-B61721931A56}]
  [/cover] */
  #if(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)
  /* Variable holding the core index value */
  uint8 CoreIndex;
  /* Det check status */
  Std_ReturnType ApiStatus;
  Std_ReturnType SwObjFound;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  volatile const Ifx_CAN_TXEVENT *TxEvntReadData;
  const Can_17_McmCan_ControllerMsgRAMConfigType*MsgRamCfgPtr;
  uint8  CtrlIndex;
  uint8 TxHandleIndx;
  uint32 TxEvntFIFOstartAddr;
  uint32 TxEvntFIFORealAddr;
  uint32 TxEvntFIFOGetIndex;
  uint8 TxEvntFillLvlVal;
  uint8 LoopExitFlag;
  uint16 SwObjIndx;
  uint8 count_i;
  uint8 count;
  uint8 TxCount[CAN_17_MCMCAN_NOOF_CONTROLLER] = {0};

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Check if status is initilaized */
  ApiStatus = \
              Can_17_McmCan_lMainDetUninitConfig(CAN_17_MCMCAN_SID_MAINFUNCTIONWRITE, \
                  CoreIndex);

  /* [cover parentID={20BD2FAC-8864-420e-8B8A-186261C0E038}]
  Check if ref period is configured for current core and Status is OK?
  [/cover] */
  if((E_NOT_OK != ApiStatus) && (CAN_17_MCMCAN_NO_HOH_VAL == \
    Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex]-> \
    CanHthPeriodIndexPtr[HthRefPeriods]))
  {
    ApiStatus = E_NOT_OK;
  }
  if(E_NOT_OK != ApiStatus)
  {
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Set pointer variable with Message RAM Configuration structure */
    MsgRamCfgPtr = CanConfig->CanControllerMsgRAMMapConfigPtr;
    /* [cover parentID={49A0E288-32A0-462c-AFE0-4207196A3F32}]
    Call transmit handler function for each of the CAN controllers
    [/cover] */
    for(CtrlIndex = CAN_17_MCMCAN_ZERO; CtrlIndex < CanConfig->CanCoreContCnt; \
        CtrlIndex++)
    {
      /* [cover parentID={D5C52A11-422E-4504-A1A2-10893B61A054}]
      Requested controller transmit event handling is of polling type
      and the controller mode is STARTED
      Check the Tx processing is configured as POLLING
      [/cover] */
      if((CAN_17_MCMCAN_POLLING == \
          CanConfig->CanEventHandlingConfigPtr[CtrlIndex]. \
          CanEventType[CAN_17_MCMCAN_ZERO]) \
          && (CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CtrlIndex]))
      {
        /* Reset the message order count */
        TxCount[CtrlIndex] = CAN_17_MCMCAN_ZERO;
        count = CAN_17_MCMCAN_ZERO;
        /* search through controller specific Tx buffer priority list */
        /* [cover parentID={36F56C52-C9EA-465d-96B2-5F50A2FB6BDB}]
        search through controller specific Tx buffer priority list
        [/cover] */
        while(count < CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE)
        {
          /* retrive the Sw object handle for the current priority */
          SwObjIndx = CanCoreState->CanTxSwObjPriorityPtr[((CtrlIndex * \
                      CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + count)];
          /* [cover parentID={FD9AF92F-CDD0-44ba-825E-984E8949287A}]
          Check If SwObj index is same as configured Index
          [/cover] */
          if(CAN_17_MCMCAN_MAX_SWOBJECT_INDEX == SwObjIndx)
          {
            /* Reset the message order count */
            TxCount[CtrlIndex] = count;
            count = (uint8)CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE;
          }
          else
          {
            /* [cover parentID={D8CCA0AD-8BA9-4ee8-AA91-A2227D82A00D}]
            check if the sw object handle is configured for this period
            [/cover] */
            SwObjFound = Can_17_McmCan_lPeriodCheckSwHandle(CtrlIndex, \
                         SwObjIndx, HthRefPeriods, CanConfig, CanCoreState);


            if(E_OK == SwObjFound)
            {
              /* [cover parentID={651FFA17-3769-4d8a-A842-91AE71DD65E7}]
              Loop for total no of HW buffer
              [/cover] */
              for(count_i = count; count_i < \
                  (CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE - CAN_17_MCMCAN_ONE); \
                  count_i++)
              {
                CanCoreState->CanTxSwObjPriorityPtr[((CtrlIndex * \
                  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + count_i)] =
                  CanCoreState->CanTxSwObjPriorityPtr[((CtrlIndex * \
                  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + count_i + \
                  CAN_17_MCMCAN_ONE)];
                CanCoreState->CanTxSwObjPriorityPtr[((CtrlIndex * \
                  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + count_i + \
                  CAN_17_MCMCAN_ONE)] = CAN_17_MCMCAN_MAX_SWOBJECT_INDEX;
              }
              /* Reset the message order count */
              TxCount[CtrlIndex] = count;
            }
            else
            {
              /* Reset the message order count */
              TxCount[CtrlIndex] = count + CAN_17_MCMCAN_ONE;
            }
          }
          count++;
        }

        /* Read the exact Node Start address */
        NodeRegAddressPtr = \
          CanConfig->CanControllerConfigPtr[CtrlIndex].CanNodeAddress;
        /* Set Tx Event FIFO Start Address */
        TxEvntFIFOstartAddr = \
          MsgRamCfgPtr[CtrlIndex].CanControllerMsgRAMMap[CAN_17_MCMCAN_FIVE];
        /* [cover parentID={5D7F64FF-EA7F-4b84-B730-86BE2BF13904}]
        Check if TEFL is set
        [/cover] */
        if(CAN_17_MCMCAN_BIT_CLEAR_VAL != NodeRegAddressPtr->IR.B.TEFL)
        {
          /* Raise DET if LOST is raised as HW event */
          /* [cover parentID={5B56C826-81B6-4b7e-BDA7-5A6C6F2EE5F2}]
          Is DET ON?
          [/cover] */
          #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
          /* [cover parentID={ACF36E33-12C4-4848-8A38-7AE7B0B9ADDB}]
          Raise DET CAN_17_MCMCAN_E_DATALOST
          [/cover] */
          Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_TRANSMITHANDLER, CAN_17_MCMCAN_E_DATALOST);
          #endif
          /* Clear TEFN and TEFL flags as well if SET */
          NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_TEFF_TEFL_CLEAR;
        }
        /* Set safety loop exit flag with value zero */
        LoopExitFlag = CAN_17_MCMCAN_ZERO;
        /* Read Fill Level value */
        TxEvntFillLvlVal =  NodeRegAddressPtr->TX.EFS.B.EFFL;

        while((CAN_17_MCMCAN_ZERO != TxEvntFillLvlVal) && \
              (CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE > LoopExitFlag))
        {
          /* Read TxEvent buffer index */
          TxEvntFIFOGetIndex = NodeRegAddressPtr->TX.EFS.B.EFGI;
          /* Set Real Event FIFO element address */
          TxEvntFIFORealAddr = TxEvntFIFOstartAddr + \
                               (TxEvntFIFOGetIndex * (uint32)CAN_17_MCMCAN_EIGHT);
          /* Load EventFIFO element address */
          /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to cast
          the address of Tx Event Type to the event elements  structure type */
          /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
          integer type. Permitted for special function registers.*/
          TxEvntReadData = (volatile Ifx_CAN_TXEVENT*) TxEvntFIFORealAddr;
          /* Read Message Marker */
          TxHandleIndx = (uint8)(TxEvntReadData->E1.B.MM);
          /* Calculate Sw Obj Handle index */
          SwObjIndx = (uint16)(CtrlIndex * \
                               CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + TxHandleIndx;
          SwObjFound = Can_17_McmCan_lPeriodCheckSwHandle(CtrlIndex, \
                       SwObjIndx, HthRefPeriods, CanConfig, CanCoreState);
          /* [cover parentID={4040DC33-C6A8-4ae5-86D7-932555453881}]
          Check if Sw Obj Handle index is valid ?
          [/cover] */
          if(E_OK != SwObjFound)
          {
            CanCoreState->CanTxSwObjPriorityPtr[((CtrlIndex * \
                                                  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + \
                                                 TxCount[CtrlIndex])] = SwObjIndx;
            TxCount[CtrlIndex] = TxCount[CtrlIndex] + CAN_17_MCMCAN_ONE;
          }
          /* Acknowledge shall set to free the Tx Event FIFO */
          NodeRegAddressPtr->TX.EFA.B.EFAI = (uint8) TxEvntFIFOGetIndex;
          /* Increment exit loop condition for safety */
          LoopExitFlag++;
          /* Read Fill Level value */
          TxEvntFillLvlVal =  NodeRegAddressPtr->TX.EFS.B.EFFL;
        }
      }
    }
  }
  #endif /* #if(CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON) */
}
#endif /* #if (CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)) */

#if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON) && \
     (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
/*******************************************************************************
** Traceability     : [cover parentID={3A79409C-F659-4c8b-B069-5EB2BFA14FD7}] **                                                                            **
**  Syntax           :  Std_ReturnType Can_17_McmCan_lPeriodCheckSwHandle     **
**                    (                                                       **
**                      const uint8 CtrlIndex,                                **
**                      const uint16 SwObjIndx,                               **
**                      const uint8 HthRefPeriods,                            **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig,  **
**                      const Can_17_McmCan_CoreStatesType* const CanCoreState**
**                    )                                                       **
**                                                                            **
**  Description      :  Function used for handling Tx confirmation according  **
**                      to the multiple write period called                   **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : HthRefPeriods - The write operation cyclic period      **
**                    configuration index                                     **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lPeriodCheckSwHandle(
    const uint8 CtrlIndex,
    const uint16 SwObjIndx, const uint8 HthRefPeriods,
    const Can_17_McmCan_CoreConfigType* const CanConfig,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  const Can_17_McmCan_PeriodHthMaskConfigType *HthPeriodInfo;
  const Can_17_McmCan_HthMaskObjectConfigType *HthConfigInfo;
  #if(CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  const Can_17_McmCan_ControllerMsgRAMConfigType*MsgRamCfgPtr;
  #endif
  Std_ReturnType SwObjFound;
  Can_HwHandleType  HthCnt;
  Can_HwHandleType  HthStartIndx;
  Can_HwHandleType  HthEndIndx;
  uint8 CoreHthRefPeriod;
  uint8  HwControllerId;
  uint8  CoreControllerId;
  Can_HwHandleType TxHandleIndx;
  uint8 HthMaskVal;
  uint16 SwObjIndxCalc;
  Can_HwHandleType CoreSpecHth;
  uint32 TxSlotIndex;
  PduIdType pduHandle;

  #if(CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  uint8 CoreTxQueueSize;
  Can_HwHandleType HthIndex;
  #endif
  Can_17_McmCan_TxBufferType BufferType;

  /* Set pointer with HTH period set information */
  HthPeriodInfo = CanConfig->CanPeriodHthMaskConfigPtr;
  /* Set pointer with HTH period handler information */
  HthConfigInfo = CanConfig->CanHthMaskObjectConfigPtr;
  /* Retrieve Core specific Hth period reference index */
  CoreHthRefPeriod = CanConfig->CanHthPeriodIndexPtr[HthRefPeriods];
  /* Read the cyclic corresponding handler first index */
  HthStartIndx = HthPeriodInfo[CoreHthRefPeriod].CanPerHthStartIndx;
  /* Read the cyclic corresponding handler first index */
  HthEndIndx = HthPeriodInfo[CoreHthRefPeriod].CanPerHthEndIndx;
  #if(CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
  /* Set pointer variable with Message RAM Configuration structure */
  MsgRamCfgPtr = CanConfig->CanControllerMsgRAMMapConfigPtr;
  #endif
  /* Reset object found flag */
  SwObjFound = E_NOT_OK;
  HthCnt = HthStartIndx;
  /* [cover parentID={C784F5C2-29A8-4563-B083-DD0C6C59AB6D}]
  Read all the HTH  status corresponding with period
  [/cover] */
  while((HthCnt < (HthStartIndx + HthEndIndx)) && (E_NOT_OK == SwObjFound))
  {
    /* Read Hw associated with HTH */
    HwControllerId = HthConfigInfo[HthCnt].HwControllerId;
    /* retrive core specific controller index */
    CoreControllerId = Can_17_McmCan_kGblConfigPtr-> \
                       CanLogicalControllerIndexPtr[HwControllerId].CanLCoreSpecContIndex;
    /* [cover parentID={B51CC71A-A81C-410c-A169-A9C89D1F99F2}]
    Check If Controller Id is configured
    [/cover] */
    if(CoreControllerId == CtrlIndex)
    {
      TxHandleIndx = HthConfigInfo[HthCnt].CanPerHthHwObjId - \
                     Can_17_McmCan_kGblConfigPtr->CanNoOfHrh;
      /* Retrive core specific Hth of the hardware object */
      CoreSpecHth = Can_17_McmCan_kGblConfigPtr-> \
                    CanHthIndexPtr[TxHandleIndx].CanHthCoreSpecIndex;
      /* Read Mask value */
      HthMaskVal = CanConfig->CanTxHwObjectConfigPtr[CoreSpecHth]. \
                   CanTxBuffIndx;
      /* Retrive core specific Hth of the hardware object */
      BufferType = \
                   CanConfig->CanTxHwObjectConfigPtr[CoreSpecHth].CanTxBufferType;
      /* [cover parentID={8F959ADA-A11D-4904-93C2-3EC18A61E061}]
      Check if buffer is dedicated buffer
      [/cover] */
      if(CAN_17_MCMCAN_TX_DED_BUFFER == BufferType)
      {
        /* Calculate Sw Obj Handle index */
        SwObjIndxCalc = (uint16)((CoreControllerId * \
                                  CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + HthMaskVal);

        /* [cover parentID={DFEEDF20-9DC4-4ecf-9565-DF85B91C3331}]
        Check if calculated SW obj handle is same as configuredSW obj handle
        [/cover] */
        if(SwObjIndx == SwObjIndxCalc)
        {
      /* Get Pdu Handle */
          pduHandle = CanCoreState->CanSwObjectHandlePtr[SwObjIndx];
          TxSlotIndex = ((uint32)1 << HthMaskVal);

      /* Check if slot is valid Tx slot */
          /* [cover parentID={DBE87262-8F3F-4c79-A87C-DBF97F775F04}]
          Check if it is a vlid tx slot
          [/cover] */
          if((TxSlotIndex & CanCoreState->BackupCanTxMask[CoreControllerId]) == TxSlotIndex)
          {
            /* Enter Critical Section */
            /* [cover parentID={B3548DD9-8B5D-4510-B249-0395A4686BA3}]
            Enter critical section
            [/cover] */
            SchM_Enter_Can_17_McmCan_CanWrMO();

      /* Release Tx Element */
            CanCoreState->CanTxMask[CoreControllerId] |= TxSlotIndex;

            /* Exit Critical Section */
            /* [cover parentID={639F9DAD-F435-487e-814C-C718F4E3B81B}]
            Exit critical section
            [/cover] */
            SchM_Exit_Can_17_McmCan_CanWrMO();
          }

          /* Set with Exact PduHandle Stored Index value */
          /* Call Transmission confirmation to upper-layer with L-PDU handle */
          CanIf_TxConfirmation(pduHandle);

          /* handler configured for this period in the current core */
          SwObjFound = E_OK;
        }
      }
      #if(CAN_17_MCMCAN_MULTIPLEXED_TRANSMISSION == STD_ON)
      else
      {
        CoreSpecHth = MsgRamCfgPtr[CoreControllerId].CanTxDedBuffCount;
        CoreTxQueueSize = MsgRamCfgPtr[CoreControllerId].CanTxQueueSize;
        HthIndex = CoreSpecHth;
        /* [cover parentID={4FB9A88A-0256-4351-86F3-45DAFE253A11}]
        Scan through Hth buffers used
        [/cover] */
        while((HthIndex < (CoreSpecHth + CoreTxQueueSize)) && \
              (E_NOT_OK == SwObjFound))
        {
          /* Calculate Sw Obj Handle index */
          SwObjIndxCalc = (uint16)((CoreControllerId * \
                                    CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + HthIndex);
          /* [cover parentID={03A928B4-9D15-41ed-AE04-2CDB1043F16F}]
          Check ifcalculated SW obj handle is same as configured SW obj handle
          [/cover] */
          if(SwObjIndx == SwObjIndxCalc)
          {
            pduHandle = CanCoreState->CanSwObjectHandlePtr[SwObjIndx];

            TxSlotIndex = ((uint32)1U << HthIndex);

            /* [cover parentID={CE35500A-3271-4820-8438-969C79F6690C}]
            Check if this element belong to Tx object
            [/cover] */
            /* Check if this element belong to Tx object */
            if((TxSlotIndex & CanCoreState->BackupCanTxMask[HwControllerId]) == TxSlotIndex)
            {
              /* Enter Critical Section */
              /* [cover parentID={AC84AD36-1468-4653-910A-DC24604E1566}]
              Enter critical section
              [/cover] */
              SchM_Enter_Can_17_McmCan_CanWrMO();

              /* Release Tx object Element */
              CanCoreState->CanTxMask[HwControllerId] |= TxSlotIndex;

              /* Exit Critical Section */
              /* [cover parentID={E74A7318-C4F9-4b06-AC32-3B9A986C7091}]
              Exit critical section
              [/cover] */
              SchM_Exit_Can_17_McmCan_CanWrMO();
            }

            /* Set with Exact PduHandle Stored Index value */
            /* Call Transmission confirmation to upper-layer with L-PDU handle*/
            CanIf_TxConfirmation(pduHandle);

            /* handler configured for this period in the current core */
            SwObjFound = E_OK;
          }
          HthIndex ++;
        }
      }
      #endif
    }
    HthCnt++;
  }
  return SwObjFound;
}
#endif /* #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON) && \
               (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON)) */

#if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={B707DAE3-81ED-45c7-A977-9CDA72CEF019}] **
**  Syntax           : void Can_17_McmCan_lCheckIfFifoMessageLost             **
**                    (                                                       **
**                      const uint8 ControllerId,                             **
**                      const Can_17_McmCan_RxBufferType RxBuffer             **
**                    )                                                       **
**                                                                            **
**  Description      :  Function will check if any message is lost in Rx FIFO **
and raise a respective DET                                                    **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId - Controller id to check Rx FIFO for      **
**                     RxBufferType - Indicates Fifo 0 or Fifo 1              **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lCheckIfFifoMessageLost(const uint8 ControllerId,
                                     const Can_17_McmCan_RxBufferType RxBufferType)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;

  /* calling core index */
  uint8 CoreIndex;

  /* core specifc configuration */
  const Can_17_McmCan_CoreConfigType* CanConfig;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;

  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];

  /* Read the exact Node Start address */
  NodeRegAddressPtr =
  CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;


   /* [cover parentID={02C75CDE-0894-43fc-81F1-DBA54E7C46E8}]
   For FIFO 0
   [/cover] */
   if(RxBufferType == CAN_17_MCMCAN_RX_FIFO0)
   {
    /* Check Received CAN message is lost or not */
    /* [cover parentID={BBE313D4-5537-4f9b-8683-99753C26283F}]
    Is the received CAN message is lost?
    [/cover] */
    if(NodeRegAddressPtr->IR.B.RF0L != CAN_17_MCMCAN_BIT_CLEAR_VAL)
    {
      /* [cover parentID={53F2E960-0079-4a30-8108-75F73261811D}]
      Is DET On?
      [/cover] */
      #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* Report to DET */
      /* [cover parentID={67C96A1C-8898-4e1f-BAE2-64408E0DE21E}]
      Report to DET CAN_E_DATALOST
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_RECEIVEHANDLER, CAN_17_MCMCAN_E_DATALOST);
      #endif
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_RF0L_CLEAR;
    }
   }

  /* [cover parentID={A84D4442-37DA-479a-AC4B-2DA3A999EC1A}]
   For FIFO 1
   [/cover] */
  if(RxBufferType == CAN_17_MCMCAN_RX_FIFO1)
  {
    /* Check Received CAN message is lost or not */
    /* [cover parentID={F27F0069-159A-4820-9AD2-2A642DDA0FCA}]
    [/cover] */
    if(NodeRegAddressPtr->IR.B.RF1L != CAN_17_MCMCAN_BIT_CLEAR_VAL)
    {
      /* [cover parentID={E5D8CBF4-6AB4-4d60-A220-E1DC52EF603C}]
      Is DET On?
      [/cover] */
      #if(CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
      /* Report to DET */
      /* [cover parentID={75D4CE5A-9F5C-4af6-A946-CD4D7FA4F10E}]
      Report to DET CAN_E_DATALOST
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_RECEIVEHANDLER, CAN_17_MCMCAN_E_DATALOST);
      #endif
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_RF1L_CLEAR;
    }
  }
}

/*CYCLOMATIC_Can_17_McmCan_lRxPeriodHandler_JUSTIFICATION: This Function has
cyclomatic Complexity of 19 as it handles multi-period mainfunction reads for
Dedicated, RxFIFO0 and RxFIFO1. This function deals with multiple time period
handling when polling is enabled to achieve the Rx notifications in the realtime
order and hance breaking this into mutiple functions impacts readability.
*/
/*******************************************************************************
** Traceability     : [cover parentID={855F23DF-F7AA-4b2d-BB46-206539D0F7B5}] **                                                                            **
**  Syntax           : void Can_17_McmCan_lRxPeriodHandler                    **
**                    (                                                       **
**                      const uint8 HrhRefPeriods                             **
**                    )                                                       **
**                                                                            **
**  Description      :  Function used for handling Rx indication process with **
**                     multiple read period                                   **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : HrhRefPeriods - The read operation cyclic period       **
**                    configuration index                                     **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
void Can_17_McmCan_lRxPeriodHandler (const uint8 HrhRefPeriods)
{
  #if (CAN_17_MCMCAN_RX_POLLING_PROCESSING == STD_ON)
  const  Can_17_McmCan_PeriodHrhMaskConfigType *HrhPeriodInfo;
  const  Can_17_McmCan_HrhMaskObjectConfigType *HrhConfigInfo;
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 CoreHrhRefPeriod;
  uint32 HrhMaskVal_0;
  uint32 HrhMaskVal_1;
  uint32 RxDedMask;
  Can_HwHandleType  HrhCnt;
  uint8  RxHandleBuffPos;
  Can_HwHandleType  HrhStartIndx;
  Can_HwHandleType  HrhEndIndx;
  uint8  ControllerId;
  uint8 HwControllerId;
  uint8 CoreIndex;
  uint32 RxNDATValue0;
  uint32 RxNDATValue1;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  /* Det check status */
  Std_ReturnType ApiStatus;
  uint8 FifoSize = 0;
  uint8 loopExitFlag = 0;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Check if status is initilaized */
  ApiStatus = Can_17_McmCan_lMainDetUninitConfig( \
              CAN_17_MCMCAN_SID_MAINFUNCTIONREAD, CoreIndex);

  /* Check if ref period is configured for current core */
  /* [cover parentID={BD1836E8-2694-4150-B9FE-90BC6E72746A}]
  Is Status OK and period configured for current core ?
  [/cover] */
  if((E_NOT_OK != ApiStatus) && (CAN_17_MCMCAN_NO_HOH_VAL == \
                                 Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex]-> \
                                 CanHrhPeriodIndexPtr[HrhRefPeriods]))
  {
    ApiStatus = E_NOT_OK;
  }

  if(E_OK == ApiStatus)
  {
    /* Local copy of the core specific configuration */
    CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
    /* Local copy of the core specific global states */
    CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
    /* Set pointer with Hrh period set information */
    HrhPeriodInfo = CanConfig->CanPeriodHrhMaskConfigPtr;
    /* Set pointer with Hrh period handler information */
    HrhConfigInfo = CanConfig->CanHrhMaskObjectConfigPtr;
    /* Retrieve Core specific Hrh period reference index */
    CoreHrhRefPeriod = CanConfig->CanHrhPeriodIndexPtr[HrhRefPeriods];
    /* Read the cyclic corresponding handler first index */
    HrhStartIndx = HrhPeriodInfo[CoreHrhRefPeriod].CanPerHrhStartIndx;
    /* Read the cyclic corresponding handler first index */
    HrhEndIndx = HrhPeriodInfo[CoreHrhRefPeriod].CanPerHrhEndIndx;
    /* Read all the Hrh  status corresponding with period */
    /* [cover parentID={39AB089C-F841-4ddc-8D11-8ED9AF549727}]
    Loop through the hrh objects
    [/cover] */
    for(HrhCnt = HrhStartIndx ; HrhCnt < (HrhStartIndx + HrhEndIndx) ; HrhCnt++)
    {
      /* Read Mask value */
      HrhMaskVal_0 = HrhConfigInfo[HrhCnt].CanPerRxbufferMaskvalue0;
      /* Read Mask value */
      HrhMaskVal_1 = HrhConfigInfo[HrhCnt].CanPerRxbufferMaskvalue1;
      /* Read Hw associated with Hrh */
      HwControllerId = HrhConfigInfo[HrhCnt].HwControllerId;
      /* retrive core specific controller index */
      ControllerId = Can_17_McmCan_kGblConfigPtr-> \
                     CanLogicalControllerIndexPtr[HwControllerId].CanLCoreSpecContIndex;
      /* [cover parentID={E8CD6AF9-AB95-4752-9D17-003E31EB97B0}]
      If the controller is in STARTED mode?
      [/cover] */
      if(CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[ControllerId])
      {
      /* Read the exact Node Start address */
      NodeRegAddressPtr = \
                          CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;

      /* Check the buffer Type  */
      /* [cover parentID={5C1C012B-3DE4-40ec-B3EA-7116FC36ED2F}]
      DEDICATED BUFFER
      [/cover] */
      if(CAN_17_MCMCAN_RX_DED_BUFFER == \
          HrhConfigInfo[HrhCnt].CanPerHrhBufferType)
      {
        RxNDATValue0 = NodeRegAddressPtr->NDAT1.U;
        RxNDATValue1 = NodeRegAddressPtr->NDAT2.U;
        /* [cover parentID={053C690A-8F3F-4f4e-99E8-13B1CDCCD09A}]
        Check Buffer is NDAT1
        [/cover] */
        if((HrhMaskVal_0 != CAN_17_MCMCAN_ZERO) && \
            (HrhMaskVal_0 == (RxNDATValue0 & HrhMaskVal_0)))
        {
          /* Set mask value */
          /* [cover parentID={2E187964-20FA-4b91-AC14-0A295261723A}]
          Set Mask
          [/cover] */
          RxDedMask = CAN_17_MCMCAN_ONE;
          /* Set with first bit position*/
          RxHandleBuffPos = CAN_17_MCMCAN_ZERO;
          /* [cover parentID={A45CA670-6985-40ff-AB58-6EF24266B8A7}]
          Message is recieved at current rx buffer index
         [/cover] */
          while(HrhMaskVal_0 != RxDedMask)
          {
            /* Increment bit position */
            RxHandleBuffPos++;
            /* Set mask for check next buffer status */
            RxDedMask = CAN_17_MCMCAN_REG_ONE << RxHandleBuffPos;
          }
          /* Call receive data extract fucntion */
          /* [cover parentID={310F4746-AF5F-402d-BD57-7E0766E739FF}]
          Call receive data extract fucntion
          [/cover] */
          Can_17_McmCan_lRxExtractData(ControllerId, RxHandleBuffPos, \
                                       CAN_17_MCMCAN_RX_DED_BUFFER, CanCoreState, CanConfig);
          /* Clear NDAT Register for receive new message */
          NodeRegAddressPtr->NDAT1.U = HrhMaskVal_0;
        }
        /* [cover parentID={0121253B-A007-4136-9DAE-044AB7C7CBDA}]
        Check Buffer is NDAT2
        [/cover] */
        if ((HrhMaskVal_1 != CAN_17_MCMCAN_ZERO) && \
            (HrhMaskVal_1 == (RxNDATValue1 & HrhMaskVal_1)))
        {
          /* Set mask value */
          RxDedMask = CAN_17_MCMCAN_ONE;
          /* Set with first bit position*/
          RxHandleBuffPos = CAN_17_MCMCAN_ZERO;
          /* [cover parentID={6C679FAC-B37E-4969-9EE5-0B505E2799A6}]
          Message is recieved at current rx buffer index
          [/cover] */
          while(HrhMaskVal_1 != RxDedMask)
          {
            /* Increment bit position */
            RxHandleBuffPos++;
            /* Set mask for check next buffer status */
            /* [cover parentID={9E1BA3E0-D608-4d71-8AFF-8333AB07AA09}]
            Set Mask
            [/cover] */
            RxDedMask = CAN_17_MCMCAN_REG_ONE << RxHandleBuffPos;
          }
          /* Set for read data from NDAT1 buffer */
          RxHandleBuffPos = RxHandleBuffPos + CAN_17_MCMCAN_THIRTY_TWO;
          /* Call receive data extract fucntion */
          /* [cover parentID={34650828-9DE7-41f9-948F-6035590DECD4}]
          Call receive data extract fucntion
          [/cover] */
          Can_17_McmCan_lRxExtractData(ControllerId, RxHandleBuffPos, \
                                       CAN_17_MCMCAN_RX_DED_BUFFER, CanCoreState, CanConfig);
          NodeRegAddressPtr->NDAT2.U = HrhMaskVal_1;
        }
      }
      /* [cover parentID={91F6FD81-C285-4e08-B5E5-7BC8253B0A7C}]
      Is it FIFO0 Buffer
      [/cover] */
      if(CAN_17_MCMCAN_RX_FIFO0 == HrhConfigInfo[HrhCnt].CanPerHrhBufferType)
      {
        FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[ControllerId].CanRxFIFO0Size;

        /* Check and raise DET */
        Can_17_McmCan_lCheckIfFifoMessageLost(ControllerId, CAN_17_MCMCAN_RX_FIFO0);

        /* Clear safety loop count, Clear WM,FULL flags, Read Fill value */
        /* [cover parentID={1D0BDFE9-8A0A-404c-A872-1FE6AD087703}]
        Check if any of the message received by RXFIFO0
        [/cover] */
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IR_RF0_WF_BITS;

        /* Check if any of the message received by RXFIFO0 */
        while((NodeRegAddressPtr->RX.F0S.B.F0FL > 0U) && (loopExitFlag < FifoSize))
        {
          loopExitFlag++;
         /* a. Read all FIFO slots and acknowledge to free up slots
            b. Notify upper layer */
          /* Read Rx FIFO 0 Get Index */
          RxHandleBuffPos = NodeRegAddressPtr->RX.F0S.B.F0GI;
          /* Invoke data retrieve function */
          /* [cover parentID={09519A70-2096-48b0-817D-358FB583EF06}]
          Invoke data retrieve function
          [/cover] */
          Can_17_McmCan_lRxExtractData(ControllerId, RxHandleBuffPos, \
                                       CAN_17_MCMCAN_RX_FIFO0, CanCoreState, CanConfig);
          /* Set read Acknowledge of buffer */
          NodeRegAddressPtr->RX.F0A.B.F0AI = RxHandleBuffPos;
        }
      }
      /* [cover parentID={53C50D54-8F17-4b3d-9F2E-ADDE8BC15AD5}]
      FIFO1 buffer
      [/cover] */
      if(CAN_17_MCMCAN_RX_FIFO1 == HrhConfigInfo[HrhCnt].CanPerHrhBufferType)
      {
        FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[ControllerId].CanRxFIFO1Size;
        loopExitFlag = 0;

        /* Check and raise DET */
        Can_17_McmCan_lCheckIfFifoMessageLost(ControllerId, CAN_17_MCMCAN_RX_FIFO1);

        /* Clear WM,FULL flags, Read Fill value */
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IR_RF1_WF_BITS;

        /* Check if any of the message received by RXFIFO1 */
        /* [cover parentID={ADDCF088-BF2B-4173-96A9-0F4EE6BA090D}]
        Loop till all messages are read in Rx FIFO 1 or read max configured messages in FIFO
        [/cover] */
        while((NodeRegAddressPtr->RX.F1S.B.F1FL >0U) && (loopExitFlag < FifoSize))
        {
          loopExitFlag++;
         /* a. Read all FIFO slots and acknowledge to free up slots
            b. Notify upper layer */
          /* Read Rx FIFO 1 Get Index */
          RxHandleBuffPos = NodeRegAddressPtr->RX.F1S.B.F1GI;
          /* Invoke data retrieve function */
          /* [cover parentID={F271F0E0-0BA0-45dc-B77E-1F878ABAD993}]
          Invoke data retrieve function
          [/cover] */
          Can_17_McmCan_lRxExtractData(ControllerId, RxHandleBuffPos, \
                                       CAN_17_MCMCAN_RX_FIFO1, CanCoreState, CanConfig);
          /* Set read Acknowledge of buffer */
          NodeRegAddressPtr->RX.F1A.B.F1AI = RxHandleBuffPos;
        }
      }
    }
   }
  }
  #endif /* #if (CAN_17_MCMCAN_RX_POLLING_PROCESSING == STD_ON) */
}
#endif /* #if (CAN_17_MCMCAN_RX_MULTI_PERIODS_SUPPORT == STD_ON) */

/*******************************************************************************
** Traceability     : [cover parentID={08B08865-A0FA-426d-BACC-9CBDE3F74BD1}] **                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lCancelPendingTx       **
**                    (                                                       **
**                      const uint8 ControllerId,                             **
**                      const Can_17_McmCan_CoreStatesType* const CanCoreState**
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : This function is used for the cancellation of          **
**                    Transmission,If there any Tx buffer with pending        **
**                    transmission request.                                   **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId - Associated CAN Controller               **
**                     CanCoreState - Pointer to global structure             **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lCancelPendingTx (const uint8 ControllerId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint32 TxPendingStatus;
  uint16 TxHwIndex;
  uint16 SwObjIndx;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;
  /* [cover parentID={96108FD4-DCDD-493c-804F-D21DE07FDA42}]
  Transmit hardware objects are configured for current core
  [/cover] */
  if(NULL_PTR != CanConfig->CanTxHwObjectConfigPtr)
  {
    for (TxHwIndex = CAN_17_MCMCAN_ZERO; TxHwIndex < \
         CAN_17_MCMCAN_MAX_TX_BUFFER_INDEX; TxHwIndex++)
    {
      /* Calculate Sw Obj Handle index */
      SwObjIndx = (uint16)((ControllerId * \
                            CAN_17_MCMCAN_NOOF_TX_HW_BUFF_AVAILABLE) + TxHwIndex);
      CanCoreState->CanSwObjectHandlePtr[SwObjIndx] = CAN_17_MCMCAN_ZERO;
      #if ((CAN_17_MCMCAN_TX_MULTI_PERIODS_SUPPORT == STD_ON)&& \
      (CAN_17_MCMCAN_TX_POLLING_PROCESSING == STD_ON))
      CanCoreState->CanTxSwObjPriorityPtr[SwObjIndx] = \
          CAN_17_MCMCAN_MAX_SWOBJECT_INDEX;
      #endif
    }
    /* [cover parentID={C926FB6D-5374-4b43-AA64-CFA53B46FAF2}]
    Check for pending request
    [/cover] */
    /* Check for pending request */
    if(CAN_17_MCMCAN_DATA_ZERO != NodeRegAddressPtr->TX.BRP.U)
    {
      /* Read Pending Tx Request for buffer */
      TxPendingStatus = NodeRegAddressPtr->TX.BRP.U;
      /* Set the pending request with cancellation request */
      NodeRegAddressPtr->TX.BCR.U = TxPendingStatus;
    }

    /* Rebuild mask Queue eleemnt mask */
    CanCoreState->CanTxMask[ControllerId] =
    CanCoreState->BackupCanTxMask[ControllerId];
  }
}

/*******************************************************************************
** Traceability     : [cover parentID={2928BBAC-9D1E-4219-BBED-D06CF1A2EACD}] **                                                                            **
**  Syntax           : static void Can_17_McmCan_lClearReceivedMsg            **
**                    (                                                       **
**                      const uint8 HwControllerId,                           **
**                      const Can_17_McmCan_CoreConfigType* const CanConfig   **
**                    )                                                       **
**                                                                            **
**  Description      : Clears receive message objects to avoid any unintended **
**                    notification                                            **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : ControllerId - Associated CAN Controller               **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void  Can_17_McmCan_lClearReceivedMsg(const uint8 HwControllerId,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 RxHandleBuffPos;
  uint8 loopExitFlag = 0;
  uint8 FifoSize = 0;

  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[HwControllerId].CanNodeAddress;
  /* Dedicated Rx Message checks */
  /* [cover parentID={6507E831-08CA-4078-884E-FEF5E67DCC9F}]
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanConfig-> \
      CanControllerMsgRAMMapConfigPtr[HwControllerId]. \
      CanControllerMsgRAMMap[CAN_17_MCMCAN_FOUR])
  {
    /* Check if any of the message received by dedicated buffer NDAT1 */
    /* [cover parentID={CA82A735-67F5-4954-A151-88CEBBB0F624}]
    The message is received by dedicated buffer NDAT1
    [/cover] */
    if(CAN_17_MCMCAN_DATA_ZERO != NodeRegAddressPtr->NDAT1.U)
    {
      /* Clear the Register */
      NodeRegAddressPtr->NDAT1.U = CAN_17_MCMCAN_CLEAR_NEWDATA_REG;
    }
    /* Check if any of the message received by dedicated buffer NDAT2 */
    /* [cover parentID={C1EBCA30-73E0-4cbb-80BE-D0B8E1F2928E}]
    The message is received by dedicated buffer NDAT2
    [/cover] */
    if(CAN_17_MCMCAN_DATA_ZERO != NodeRegAddressPtr->NDAT2.U)
    {
      /* Clear the Register */
      NodeRegAddressPtr->NDAT2.U = CAN_17_MCMCAN_CLEAR_NEWDATA_REG;
    }
  }
  /* RxFIFO0 Message checks */
  /* [cover parentID={7DAB05D7-ACE9-4891-9BC9-103699A8D80F}]
  RxFIFO0 Message checks
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanConfig-> \
      CanControllerMsgRAMMapConfigPtr[HwControllerId]. \
      CanControllerMsgRAMMap[CAN_17_MCMCAN_TWO])
  {
    FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[HwControllerId].CanRxFIFO0Size;
    /* Check if any of the message received by RXFIFO0 */
    /* [cover parentID={C8C29F16-63D3-4770-A7B5-D85FE656E164}]
    RxFIFO0 Message checks
    [/cover] */
    while((NodeRegAddressPtr->RX.F0S.B.F0FL > 0U) && (loopExitFlag < FifoSize))
    {
      loopExitFlag++;
      /* Read Rx FIFO 0 Get Index */
      RxHandleBuffPos = NodeRegAddressPtr->RX.F0S.B.F0GI;
      /* Set read Acknowledgement of buffer */
      NodeRegAddressPtr->RX.F0A.U = RxHandleBuffPos;
    }
  }
  /* RxFIFO1 Message checks */
  /* [cover parentID={ACD4DD5E-3FD6-476c-BD26-703E849459D2}]
  RxFIFO1 Message checks
  [/cover] */
  if(CAN_17_MCMCAN_ADDRESS_CHECK != CanConfig-> \
      CanControllerMsgRAMMapConfigPtr[HwControllerId]. \
      CanControllerMsgRAMMap[CAN_17_MCMCAN_THREE])
  {
    FifoSize = CanConfig->CanControllerMsgRAMMapConfigPtr[HwControllerId].CanRxFIFO1Size;
    loopExitFlag = 0;
    /* Check if any of the message received by RXFIFO1 */
    /* [cover parentID={A07022A0-5C78-4630-880B-1A2FFCBCF5AC}]
    RxFIFO1 Message checks
    [/cover] */
    while((NodeRegAddressPtr->RX.F1S.B.F1FL >0U) && (loopExitFlag < FifoSize))
    {
      loopExitFlag++;
      /* Read Rx FIFO 0 Get Index */
      RxHandleBuffPos = NodeRegAddressPtr->RX.F1S.B.F1GI;
      /* Set read Acknowledge of buffer */
      NodeRegAddressPtr->RX.F1A.U = RxHandleBuffPos;
    }
  }
}

/*******************************************************************************
** Traceability     : [cover parentID={F5913DAB-E168-432f-B5BC-C4429CEBC659}] **                                                                            **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lTimeOut           **
**                    (                                                       **
**                      const volatile uint32* const Can_RegAddress,          **
**                      const uint32 Check_Val,                               **
**                      const boolean CheckStatus                             **
**                    )                                                       **
**                                                                            **
**  Description      : This service for a time-out detection in case the      **
**                    hardware doesn't react in the expected time             **
**                    (hardware error) to prevent endless loops.              **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : Can_RegAddress- The address of register which value    **
**                    shall be check.                                         **
**                    Check_Val- The value which is used for comparing with   **
**                    Register value.                                         **
**                    CheckStatus- used for identify the comparison method    **
**                    Method.                                                 **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK: Hw written successfully                          **
**                    E_NOT_OK: Hw is not set with requested value            **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lTimeOut (
    const volatile uint32* const Can_RegAddress,
    const uint32 Check_Val, const Can_17_McmCan_TimeoutCheckType CheckStatus)
{
  uint32 TimeOutResolution;
  uint32 TimeOutCount;
  uint32 TimeOutCountInitial;
  uint32 MeasuredTicks;
  Std_ReturnType TimeOutStatus;

  /* Set with Dem event is reported */
  TimeOutStatus = E_NOT_OK;
  /* Read the delay calibration Time */
  TimeOutResolution = Mcal_DelayTickResolution();

  /* calculate actual delay span */
  TimeOutCount =  (uint32)CAN_17_MCMCAN_TIMEOUT_DURATION / TimeOutResolution ;
  /* Wait for Hardware Loop time-out to check  the setting status of
     status register */
  /* Set with initial value */
  TimeOutCountInitial = Mcal_DelayGetTick();
  do
  {
    /* Count the elapsed time */
    MeasuredTicks = Mcal_DelayGetTick() - TimeOutCountInitial;
    /*[cover parentID={DE447DBA-838D-4ca7-83BD-2EC917BD1887}]
    Is Status True
    [/cover]*/
    if(CAN_17_MCMCAN_EQUAL == CheckStatus)
    {
      /* [cover parentID= {941F296F-B7C1-4fa6-9CE7-5569C15E96A9}]
      Is Status True
      [/cover] */
      if(Check_Val == (((uint32)*Can_RegAddress & Check_Val)))
      {
        TimeOutStatus = E_OK;
      }
    }
    /* [cover parentID={C6CBDD49-BB83-4b50-BC48-6F5A9B74FC0A}]
    Is Status True
   [/cover] */
    else if(CAN_17_MCMCAN_NOT_EQUAL == CheckStatus)
    {
      /* [cover parentID= {4177FD0F-5D7E-481d-B9AA-F8519E55FD5F}]
      Is Status True
      [/cover] */
      if(Check_Val != (((uint32)*Can_RegAddress & Check_Val)))
      {
        TimeOutStatus = E_OK;
      }
    }
    /* [cover parentID={3122C46A-F731-4976-8A77-CBD4C4B2CE91}]
    Is status is True
   [/cover] */
    else
    {
      /*CAN_17_MCMCAN_NAND == CheckStatus*/
      /* [cover parentID= {4177FD0F-5D7E-481d-B9AA-F8519E55FD5F}]
      Is Status True
      [/cover] */
      if(CAN_17_MCMCAN_ZERO != (((uint32)*Can_RegAddress) & (Check_Val)))
      {
        TimeOutStatus = E_OK;
      }
    }
  }
  /* [cover parentID={9186353F-7735-4403-8BF8-C4269FEC4CFD}]
  Is status is true
  [/cover] */
  while ((E_NOT_OK == TimeOutStatus) && \
           ((TimeOutCount * CAN_17_MCMCAN_HUNDRED) >= MeasuredTicks));

  return TimeOutStatus;
}

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           : LOCAL_INLINE void Can_17_McmCan_lActivateIcom          **
**                    (                                                       **
**                       const uint8 Controller ,                             **
**                       const IcomConfigIdType  ConfigurationId,             **
**                       Can_17_McmCan_CoreStatesType * const CanCoreState,   **
**                       const Can_17_McmCan_CoreConfigType * const CanConfig **
**                    )                                                       **
**                                                                            **
**  Description      : Activates CAN pretended networking for a CAN controller**
** [cover parentID={488C84CA-6384-46ea-9B45-C5A8B6FD3931}]                    **
** [/cover]                                                                   **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                    ConfigurationId  - Requested ICOM Configuration         **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE void Can_17_McmCan_lActivateIcom (const uint8 Controller, \
    const IcomConfigIdType ConfigurationId, \
    const Can_17_McmCan_CoreStatesType* const CanCoreState, \
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{

  const Can_17_McmCan_SIDFilterConfigType *CanSIDPtr;
  volatile Ifx_CAN_STDMSG *SIDFilterDataPtr;
  uint32 SIDFilterStartAddr;
  uint32 SIDFilterRealAddr;
  uint16 SIDFiltrStartIndx;
  uint16 SIDFiltrEndIndx;
  uint16 SIDFiltrIndx;
  uint8  SIDOffsetIndx;
  const Can_17_McmCan_XIDFilterConfigType *CanXIDPtr;
  volatile Ifx_CAN_EXTMSG *XIDFilterPtr;
  uint32 XIDFilterStartAddr;
  uint32 XIDFilterRealAddr;
  uint16 XIDFiltrStartIndx;
  uint16 XIDFiltrEndIndx;
  uint16 XIDFiltrIndx;
  uint8  XIDOffsetIndx;

  /* Read the exact Node Start address */
  Ifx_CAN_N *NodeRegAddressPtr = \
                                 CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;
  /* Check if Wakeup on bus off condition is configured*/
  /* [cover parentID= {364ACACF-B21D-46a6-B2E1-525B11513D49}]
  Check if Wakeup on bus off condition is configured
  [/cover] */
  if (FALSE == Can_17_McmCan_kGblConfigPtr-> \
      CanIcomConfigPtr[ConfigurationId - CAN_17_MCMCAN_ONE].CanIcomWakeOnBusOff)
  {
    /* Check if Wakeup on bus off condition is configured*/
    /* [cover parentID={097D0152-B9A7-4708-B874-F5F51524750A}]
    Bus off interrupt of the current CAN node is enabled or not
    [/cover] */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_TWO])
    {
      /* Disable Bus off interrupt of the current CAN node */
      NodeRegAddressPtr->IE.B.BOE = CAN_17_MCMCAN_ZERO;
    }
    /* Set the ICOM Wake on bus off status */
    CanCoreState->CanIcomConfgBOStatusPtr[Controller] = FALSE;
  }
  else
  {
    /* [cover parentID={08AAC771-0EFE-4381-B519-F7CDE3A3A5ED}]
    If Can event handling interrupt enabled ?
    [/cover] */
    if (CAN_17_MCMCAN_INTERRUPT == \
        CanConfig->CanEventHandlingConfigPtr[Controller]. \
        CanEventType[CAN_17_MCMCAN_TWO])
    {
      /* Enable Bus off interrupt of the current CAN node */
      NodeRegAddressPtr->IE.B.BOE = CAN_17_MCMCAN_ONE;
    }
    /* Set the ICOM Wake on bus off status */
    CanCoreState->CanIcomConfgBOStatusPtr[Controller] = TRUE;
  }
  /* Check if Wakeup on bus off condition is configured*/
  /* [cover parentID={DA2E1E7C-505A-4863-B084-B08A3887846E}]
  If SID filter is NULL_PTR
  [/cover] */
  if(NULL_PTR != CanConfig-> CanSIDFilterConfigPtr)
  {
    /* Set offset with first element */
    SIDOffsetIndx = CAN_17_MCMCAN_ZERO;

    /* Set SID filter list Address */
    CanSIDPtr = CanConfig->CanSIDFilterConfigPtr;
    /* Set with first SID filter Index */
    SIDFiltrStartIndx = \
                        CanConfig->CanControllerConfigPtr[Controller]. \
                        CanControllerMOMap[CAN_17_MCMCAN_TWO];
    /* Set with End SID filter Index */
    SIDFiltrEndIndx = \
                      CanConfig->CanControllerConfigPtr[Controller]. \
                      CanControllerMOMap[CAN_17_MCMCAN_THREE];
    /* Read Standard Identifier filter Start address */
    SIDFilterStartAddr = CanConfig-> \
                         CanControllerMsgRAMMapConfigPtr[Controller]. \
                         CanControllerMsgRAMMap[CAN_17_MCMCAN_ZERO];
    /* Set with all standard filter configuration */
    for(SIDFiltrIndx = SIDFiltrStartIndx; SIDFiltrIndx < \
        (SIDFiltrStartIndx + SIDFiltrEndIndx); SIDFiltrIndx++)
    {
      /* Set with SID Filter element address */
      SIDFilterRealAddr = SIDFilterStartAddr + (CAN_17_MCMCAN_RAM_BYTES * \
                          (uint32)SIDOffsetIndx);
      /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
      cast the address of Standard filter Type to the Standard filter elements
      structure type  */
      /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
      integer type. Permitted for special function registers.*/
      /* Increment for access next filter element */
      SIDFilterDataPtr = (volatile Ifx_CAN_STDMSG*)SIDFilterRealAddr;
      /* [cover parentID={0A4FDBB9-8D20-4320-B5C1-209682BE857A}]
      If Pretended Network OFF
      [/cover] */
      if(FALSE == CanSIDPtr[SIDFiltrIndx].CanSidPNSupport)
      {
        /* Set to Reject Message with Filter element */
        SIDFilterDataPtr->S0.B.SFEC = CAN_17_MCMCAN_ZERO;
      }
      /* Increment for access next filter element */
      SIDOffsetIndx++;
    }
  }
  /* [cover parentID={979410E7-2304-4e71-A0BD-BD924D7C9EA5}]
  If XID filter is NULL_PTR
  [/cover] */
  if(NULL_PTR != CanConfig-> CanXIDFilterConfigPtr)
  {

    /* Set with XID filter list configuration address */
    CanXIDPtr = CanConfig->CanXIDFilterConfigPtr;
    /* Read Extended Identifier filter Start address */
    XIDFilterStartAddr = CanConfig-> \
                         CanControllerMsgRAMMapConfigPtr[Controller]. \
                         CanControllerMsgRAMMap[CAN_17_MCMCAN_ONE];
    /* Set with start Index */
    XIDFiltrStartIndx = \
                        CanConfig->CanControllerConfigPtr[Controller]. \
                        CanControllerMOMap[CAN_17_MCMCAN_FOUR];
    /* Set with end Index */
    XIDFiltrEndIndx = \
                      CanConfig->CanControllerConfigPtr[Controller]. \
                      CanControllerMOMap[CAN_17_MCMCAN_FIVE];
    /* Set OFFSET with first element */
    XIDOffsetIndx = CAN_17_MCMCAN_ZERO;
    /* Set with all Extended filter configuration */
    for(XIDFiltrIndx = XIDFiltrStartIndx; XIDFiltrIndx <
        (XIDFiltrStartIndx + XIDFiltrEndIndx); XIDFiltrIndx++)
    {
      /* Set address with XID filter element Address */
      XIDFilterRealAddr = XIDFilterStartAddr + ((uint32)CAN_17_MCMCAN_EIGHT * \
                          (uint32)XIDOffsetIndx);
      /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to cast the
      address of Extended filter Type to the extended filter elements  structure
      type */
      /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
      integer type. Permitted for special function registers.*/
      XIDFilterPtr = (volatile Ifx_CAN_EXTMSG*)XIDFilterRealAddr;
      /* [cover parentID={AA578760-E59C-462a-9A65-81B9E3FFDB67}]
      If Pretended Network OFF
      [/cover] */
      if(FALSE == CanXIDPtr[XIDFiltrIndx].CanXidPNSupport)
      {
        /* Set to Reject Message with Filter element */
        XIDFilterPtr->F0.B.EFEC = CAN_17_MCMCAN_ZERO;
      }
      /* Increment for access next filter element */
      XIDOffsetIndx++;
    }
  }
  /* Store ICOM Configuration ID */
  CanCoreState->CanActiveIcomCfgIdPtr[Controller] = ConfigurationId;

  /*Set the pretended network status for the current CAN controller */
  CanCoreState->CanIcomEnableStatusPtr[Controller] = TRUE;
}
#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */


/*CYCLOMATIC_Can_17_McmCan_lDeactivateIcom_JUSTIFICATION: This Function has a
cyclomatic Complexity of 17 since all ICOM deactivation related conditions are
based on dedciated or Fifo1 and Fifo2 all the related checks are kept in same
function.
*/
#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           : static void Can_17_McmCan_lDeactivateIcom              **
**                    (                                                       **
**                       const uint8 Controller,                              **
**                       const IcomConfigIdType ConfigurationId,              **
**                       Can_17_McmCan_CoreStatesType* const CanCoreState,    **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                    )                                                       **
**                                                                            **
**  Description      : Deactivate CAN pretended networking for a CAN          **
**                     controller                                             **
** [cover parentID={BE2FCC3E-23ED-4ab0-A6C3-8A5E7D285E7A}]                    **
** [/cover]                                                                   **
**                                                                            **
** Service ID       : NA                                                      **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller                 **
**                    ConfigurationId  - Requested ICOM Configuration         **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : none                                                   **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lDeactivateIcom (const uint8 Controller,
    const IcomConfigIdType ConfigurationId,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  const Can_17_McmCan_SIDFilterConfigType *CanSIDPtr;
  volatile Ifx_CAN_STDMSG *SIDFilterPtr;
  volatile Ifx_CAN_EXTMSG *XIDFilterPtr;
  const volatile uint32* RegAddressPtr;
  const Can_17_McmCan_XIDFilterConfigType *CanXIDPtr;
  uint32 SIDFilterStartAddr;
  uint32 SIDFilterRealAddr;
  uint16 SIDFiltrStartIndx;
  uint16 SIDFiltrEndIndx;
  uint16 SIDFiltrIndx;
  uint8 SIDOffsetIndx;
  uint32 XIDFilterStartAddr;
  uint32 XIDFilterRealAddr;
  uint16 XIDFiltrStartIndx;
  uint16 XIDFiltrEndIndx;
  uint16 XIDFiltrIndx;
  uint8 XIDOffsetIndx;
  uint16 IcomMsgIndex;
  uint16 MsgIndex;
  uint16 IcomNoOfRxMsg;
  uint8 IcomConfigId;
  Std_ReturnType ApiStatus;

  /* Get ICOM configuration ID*/
  IcomConfigId = CanCoreState->CanActiveIcomCfgIdPtr[Controller];
  /* Get the first receive configuration index */
  IcomMsgIndex = Can_17_McmCan_kGblConfigPtr->CanIcomConfigPtr \
                 [IcomConfigId - CAN_17_MCMCAN_ONE].CanIcomFirstMsgIndx;
  /* Get the total number of ICOM Receive messages configured*/
  IcomNoOfRxMsg =
    Can_17_McmCan_kGblConfigPtr->CanIcomConfigPtr \
    [IcomConfigId - CAN_17_MCMCAN_ONE].CanIcomNoOfMsgIndx;
  /* Read the exact Node Start address */
  NodeRegAddressPtr = \
                      CanConfig->CanControllerConfigPtr[Controller].CanNodeAddress;

  /* Set with Initialization mode */
  NodeRegAddressPtr->CCCR.B.INIT = CAN_17_MCMCAN_ONE;
  /* Set with Register Address */
  /* MISRA2012_RULE_11_3_JUSTIFICATION:casting apointer to object into a
     pointer is used.No side effect has been seen. */
  RegAddressPtr = (const volatile uint32*) (&(NodeRegAddressPtr->CCCR.U));
  /* Wait until the INIT bits become set */
  ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
              CAN_17_MCMCAN_NODE_INIT_CCE_SET,CAN_17_MCMCAN_EQUAL);

  /* [cover parentID={55ABC673-3427-42f9-B732-C0674571A352}]
  Check if INIT bit is SET as expected
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* Enable configuration change */
    NodeRegAddressPtr->CCCR.B.CCE = CAN_17_MCMCAN_ONE;
    /* Wait until the Configuration Change Enable bits become set */
    ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
                                         CAN_17_MCMCAN_CCCR_CCE_SET_MASK, CAN_17_MCMCAN_EQUAL);
  }

  /* [cover parentID={3126B8E2-B9AD-4d4d-B110-51FC582FFDA0}]
  Check if CCE bit is SET as expected
  [/cover] */
  if(E_OK == ApiStatus)
  {
    /* Check if Wakeup on bus off condition is configured*/
    /* [cover parentID={CB0FE3D4-8D89-4b3c-9BD2-3A9FE061098A}]
      Check if Wakeup on bus off condition is configured and interrupt
      [/cover] */
    if(TRUE != CanCoreState->CanIcomConfgBOStatusPtr[Controller])
    {
      if (CAN_17_MCMCAN_INTERRUPT == \
          CanConfig->CanEventHandlingConfigPtr[Controller]. \
          CanEventType[CAN_17_MCMCAN_TWO])
      {
        /* Re enable the Bus Off interrupt of the requested CAN node */
        NodeRegAddressPtr->IE.B.BOE = CAN_17_MCMCAN_ONE;
      }
    }
    /* [cover parentID={D719E03F-D4BE-4569-8473-A4DF0E8AE018}]
    If SID Config Pointer is not Null
    [/cover] */
    if(NULL_PTR != CanConfig-> CanSIDFilterConfigPtr)
    {
      /* Set SID filter list Address */
      CanSIDPtr = CanConfig->CanSIDFilterConfigPtr;
      /* Set with first SID filter Index */
      SIDFiltrStartIndx = \
                          CanConfig->CanControllerConfigPtr[Controller]. \
                          CanControllerMOMap[CAN_17_MCMCAN_TWO];
      /* Set with End SID filter Index */
      SIDFiltrEndIndx = \
                        CanConfig->CanControllerConfigPtr[Controller]. \
                        CanControllerMOMap[CAN_17_MCMCAN_THREE];
      /* Read Standard Identifier filter Start address */
      SIDFilterStartAddr = CanConfig-> \
                           CanControllerMsgRAMMapConfigPtr[Controller]. \
                           CanControllerMsgRAMMap[CAN_17_MCMCAN_ZERO];
      /* Set Offset with First element*/
      SIDOffsetIndx = CAN_17_MCMCAN_ZERO;
      /* Set with all standard filter configuration */
      for(SIDFiltrIndx = SIDFiltrStartIndx; SIDFiltrIndx <
          (SIDFiltrStartIndx + SIDFiltrEndIndx); SIDFiltrIndx++)
      {
        /* Set with Address of  filter element */
        SIDFilterRealAddr = SIDFilterStartAddr + ((uint32)SIDOffsetIndx * \
                            CAN_17_MCMCAN_RAM_BYTES);
        /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
        cast the address of Standard filter Type to the standard filter
        elements  structure type  */
        /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
        integer type. Permitted for special function registers.*/
        /* Set with First filter element */
        SIDFilterPtr = (volatile Ifx_CAN_STDMSG*)SIDFilterRealAddr;
        /* [cover parentID={E4964635-E2F9-48cb-98F1-6F3129D19994}]
        If Pretended Network OFF
        [/cover] */
        if(FALSE == CanSIDPtr[SIDFiltrIndx].CanSidPNSupport)
        {
          /* [cover parentID={B6024BAF-5E14-450b-9A5B-7068C34F4611}]
          [/cover] */
          /* [cover parentID={8D068AA0-9DA1-4c75-B3F8-1F8040F23865}]
          [/cover] */
          if(CAN_17_MCMCAN_RX_FIFO1 == CanSIDPtr[SIDFiltrIndx].CanSidBufferType)
          {
            /* Set to store in RXFIO1 */
            SIDFilterPtr->S0.B.SFEC = CAN_17_MCMCAN_TWO;
          }
          else if(CAN_17_MCMCAN_RX_FIFO0 == \
                  CanSIDPtr[SIDFiltrIndx].CanSidBufferType)
          {
            /* Set to store in RXFIO0 */
            SIDFilterPtr->S0.B.SFEC = CAN_17_MCMCAN_ONE;
          }
          else
          {
            /* Set to store in RX Dedicated buffer */
            SIDFilterPtr->S0.B.SFEC = CAN_17_MCMCAN_SEVEN;
          }
        }
        /* increment to access next element */
        SIDOffsetIndx++;
      }
    }
    /* [cover parentID={80B1B0F9-7EF8-46a6-A730-C2038990D035}]
      If XID Config Pointer is not Null
      [/cover] */
    if(NULL_PTR != CanConfig-> CanXIDFilterConfigPtr)
    {
      /* Set with XID filter list configuration address */
      CanXIDPtr = CanConfig->CanXIDFilterConfigPtr;
      /* Read Extended Identifier filter Start address */
      XIDFilterStartAddr = CanConfig-> \
                           CanControllerMsgRAMMapConfigPtr[Controller]. \
                           CanControllerMsgRAMMap[CAN_17_MCMCAN_ONE];
      /* Set with start Index */
      XIDFiltrStartIndx = \
                          CanConfig->CanControllerConfigPtr[Controller]. \
                          CanControllerMOMap[CAN_17_MCMCAN_FOUR];
      /* Set with end Index */
      XIDFiltrEndIndx = \
                        CanConfig->CanControllerConfigPtr[Controller]. \
                        CanControllerMOMap[CAN_17_MCMCAN_FIVE];
      /* Set Offset with first element */
      XIDOffsetIndx = CAN_17_MCMCAN_ZERO;
      /* Set with all Extended filter configuration */
      for(XIDFiltrIndx = XIDFiltrStartIndx; XIDFiltrIndx < \
          (XIDFiltrStartIndx + XIDFiltrEndIndx); XIDFiltrIndx++)
      {
        /* Set with offset of XID filter element */
        XIDFilterRealAddr = XIDFilterStartAddr + ((uint32)CAN_17_MCMCAN_EIGHT * \
                            (uint32)XIDOffsetIndx);
        /* Set with First filter element */
        /* MISRA2012_RULE_11_6_JUSTIFICATION: The pointer cast is used to
        cast the address of extended filter type to the extended filter
        elements  structure type  */
        /* MISRA2012_RULE_11_4_JUSTIFICATION: conversion between pointer and
         integer type. Permitted for special function registers.*/
        XIDFilterPtr = (volatile Ifx_CAN_EXTMSG*) XIDFilterRealAddr;
        /* [cover parentID={8C11FDDA-2EDA-42c4-A777-0CC38134A365}]
        If Pretended Network OFF
        [/cover] */
        if(FALSE == CanXIDPtr[XIDFiltrIndx].CanXidPNSupport)
        {
          if(CAN_17_MCMCAN_RX_FIFO1 == CanXIDPtr[XIDFiltrIndx].CanXidBufferType)
          {
            /* Set to store in RXFIO1 */
            XIDFilterPtr->F0.B.EFEC = CAN_17_MCMCAN_TWO;
          }
          else if(CAN_17_MCMCAN_RX_FIFO0 == \
                  CanXIDPtr[XIDFiltrIndx].CanXidBufferType)
          {
            /* Set to store in RXFIO0 */
            XIDFilterPtr->F0.B.EFEC = CAN_17_MCMCAN_ONE;
          }
          else
          {
            /* Set to store in RX Dedicated buffer */
            XIDFilterPtr->F0.B.EFEC = CAN_17_MCMCAN_SEVEN;
          }
        }
        /* increment to access next element */
        XIDOffsetIndx++;
      }
    }

    /* Set with normal operation mode, CCE will be cleared when INIT becomes '0' */
    NodeRegAddressPtr->CCCR.B.INIT = CAN_17_MCMCAN_ZERO;

    /* Check the Hw status update */
    ApiStatus = Can_17_McmCan_lTimeOut((const volatile uint32*)RegAddressPtr, \
                                      (uint32) CAN_17_MCMCAN_NODE_INIT_MODE, CAN_17_MCMCAN_NOT_EQUAL);
    /* [cover parentID={3A5EFDD9-0795-4b4c-A3CD-DFE6BFF3F497}]
    Chek for timeout
    [/cover] */
    if(E_OK == ApiStatus)
    {
      /* [cover parentID={7EBB138D-7A1D-4afc-81D9-CA6C9D7B0635}]
       Enter critical section
       [/cover] */
      SchM_Enter_Can_17_McmCan_IcomMsgCntrVal();

      /* Set with First index value */
      MsgIndex = IcomMsgIndex;

      while(MsgIndex < (IcomMsgIndex + IcomNoOfRxMsg))
      {
        /* Clear Msg Counter values */
        CanCoreState->CanIcomMsgCntrValPtr[MsgIndex] = CAN_17_MCMCAN_ZERO;

        /* Increment message counter index */
        MsgIndex++;
      }

      /* Clear the ICOM Wake on bus off status */
      CanCoreState->CanIcomConfgBOStatusPtr[Controller] = FALSE;
      /* Set the pretended network status for the current CAN controller */
      CanCoreState->CanIcomEnableStatusPtr[Controller] = FALSE;
      /* Clear ICOM Configuration ID */
      CanCoreState->CanActiveIcomCfgIdPtr[Controller] = CAN_17_MCMCAN_ZERO;

      /* Allow Can_Writes - Icom de-activation completed */
      CanCoreState->CanIcomBlockWriteCalls[Controller] = FALSE;
    }
    /* [cover parentID={E2CCDE21-ACA6-449f-9123-96166B0C51C1}]
    Enter critical section
    [/cover] */
    SchM_Exit_Can_17_McmCan_IcomMsgCntrVal();

    UNUSED_PARAMETER(ConfigurationId);
  }
}
#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */

#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           :  static void Can_17_McmCan_lIcomValidateMsg            **
**                     (                                                      **
**                        Can_HwType* const Mailbox,                          **
**                        const PduInfoType* const PduInfoPtr,                **
**                        Can_17_McmCan_CoreStatesType* const CanCoreState,   **
**                        const Can_17_McmCan_CoreConfigType* const CanConfig **
**                     )                                                      **
**                                                                            **
**  Description      : Validates the received message against configured      **
**                    wakeup conditions .                                     **
** [cover parentID={59278E5B-418E-4beb-B6C8-F2F6D8237C2D}]                    **
** [/cover]                                                                   **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Mailbox    : Included Received Message ID,HOH          **
**                                 and Controller Id                          **
**                    PduInfoPtr : Included SDU length and Data pointer       **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Parameters (in-out) : CanCoreState - Pointer to global structure          **
**                                                                            **
**  Return value     : E_OK : The WakeUp condition is caused                  **
**                    E_NOT_OK : The WakeUp condition is not caused           **
**                                                                            **
*******************************************************************************/
static void Can_17_McmCan_lIcomValidateMsg(Can_HwType* const Mailbox,
    const PduInfoType* const PduInfoPtr,
    const Can_17_McmCan_CoreStatesType* const CanCoreState,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  Can_IdType IcomRxMsgId;
  Can_IdType Icom_CheckMsgId;
  uint16 IcomMsgIndex;
  uint16 MsgIndex;
  uint16 IcomNoOfRxMsg;
  uint8 IcomConfigId;
  uint8 ControllerId;
  Std_ReturnType ApiStatus;
  Std_ReturnType RxIndicationStatus;

  /* Set with Controller Configuration Id */
  ControllerId = Mailbox->ControllerId;
  /* Get ICOM configuration ID*/
  IcomConfigId = CanCoreState->CanActiveIcomCfgIdPtr[ControllerId];
  /* Get the first receive configuration index */
  IcomMsgIndex = \
                 Can_17_McmCan_kGblConfigPtr->CanIcomConfigPtr \
                 [IcomConfigId - CAN_17_MCMCAN_ONE].CanIcomFirstMsgIndx;
  /* Get the total number of ICOM Receive messages configured*/
  IcomNoOfRxMsg = \
                  Can_17_McmCan_kGblConfigPtr->CanIcomConfigPtr \
                  [IcomConfigId - CAN_17_MCMCAN_ONE].CanIcomNoOfMsgIndx;
  /* Set with First index value */
  MsgIndex = IcomMsgIndex;
  while(MsgIndex < (IcomMsgIndex + IcomNoOfRxMsg))
  {
    /* Set with Icom Rx message Id */
    IcomRxMsgId = Mailbox->CanId & ~(CAN_17_MCMCAN_SET_XTD_BIT_MASK);
    /* check if CanIcomMessageIdMask is configured */
    /* [cover parentID={7FCE3062-4F0E-42f9-8D63-BFBB915127D4}]
      check if CanIcomMessageIdMask is configured
      [/cover] */
    if(CAN_17_MCMCAN_MSG_MASK_VAL != \
        Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex].CanIcomMaskRef)
    {
      /* Mask the received message ID with the ICOM Message Id mask */
      IcomRxMsgId &= \
                     (Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex].CanIcomMaskRef);
      Icom_CheckMsgId = \
                        (Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex].CanIcomMsgId &\
                         Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex].CanIcomMaskRef);
    }
    else
    {
      Icom_CheckMsgId = Can_17_McmCan_kGblConfigPtr-> \
                        CanIcomMsgConfigPtr[MsgIndex].CanIcomMsgId;
    }
    /* Compare Masked id with configured Id */
    /* [cover parentID={20A7B80B-4B45-4249-A1D9-005D2A4E3234}]
      Compare Masked id with configured Id
      [/cover] */
    if(IcomRxMsgId == Icom_CheckMsgId)
    {
      /* Compare payload length(DLC)and if mismatch set as wake-up */
      /* [cover parentID={3193029F-59A3-4683-9006-5BCDB27C4AC6}]
      Compare payload length(DLC)and if mismatch set as wake-up
      [/cover] */
      if( PduInfoPtr-> SduLength == \
          Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex].CanIcomDLC)
      {
        RxIndicationStatus = E_OK;
        ApiStatus = E_OK;
      }
      else
      {
        /* [cover parentID={670F2400-E590-4930-BD2C-08C2B8FE8951}]
        Check payload length error is configured
        [/cover] */
        /* Set Status as Wakeup condition cause */
        if(TRUE == \
            Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex]. \
            CanIcomLengthErr)
        {
          RxIndicationStatus = E_OK;
          ApiStatus = E_NOT_OK;
        }
        else
        {
          ApiStatus = E_NOT_OK;
          RxIndicationStatus = E_NOT_OK;
        }
      }
      /* Compare masked ID with message Id */
      /* [cover parentID={D088A1AF-E79D-488e-8523-1328EC6F754D}]
      Check if signal comparison is required
      [/cover] */
      if ((E_OK == ApiStatus) && (CAN_17_MCMCAN_ZERO != \
                                  Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex]. \
                                  CanIcomNoOfSignalIndx))
      {
        /* Invoke function for Message signal validation */
        ApiStatus = Can_17_McmCan_lIcomValidateMsgSignal(MsgIndex, PduInfoPtr, \
                    Can_17_McmCan_kGblConfigPtr);
        RxIndicationStatus = ApiStatus;
      }
      /* [cover parentID={45648DCE-4474-45bf-836F-76DAA4681C09}]
      [/cover] */
      /* Check counter value is configured or not */
      if((E_OK == ApiStatus) && (CAN_17_MCMCAN_ZERO != \
                                 Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex]. \
                                 CanIcomCntrVal))
      {
        SchM_Enter_Can_17_McmCan_IcomMsgCntrVal();
        /* Increment Message Counter Value */
        CanCoreState->CanIcomMsgCntrValPtr[MsgIndex]++;
        /* Check if the message with the ID is received n times on the
        communication channel. */
        if(Can_17_McmCan_kGblConfigPtr->CanIcomMsgConfigPtr[MsgIndex]. \
            CanIcomCntrVal == CanCoreState->CanIcomMsgCntrValPtr[MsgIndex])
        {
          /* Set Status as Wakeup condition cause */
          RxIndicationStatus = E_OK;
          CanCoreState->CanIcomMsgCntrValPtr[MsgIndex] = CAN_17_MCMCAN_ZERO;
        }
        else
        {
          /* reset Wake up cause till counter value matches */
          RxIndicationStatus = E_NOT_OK;
        }
        SchM_Exit_Can_17_McmCan_IcomMsgCntrVal();
      }
      /* Set Exit condition if Wakeup Cause is enabled */
      /* [cover parentID={573DD384-AF96-4acb-880E-B6DFDDFE54FA}]
        Check return Value is E_OK
        [/cover] */
      if(E_OK == RxIndicationStatus)
      {
        /* Update mail box info with logical controller ID value */
        Mailbox->ControllerId = \
                                CanConfig->CanControllerIndexingPtr[ControllerId];
        /* Call CanIf_RxIndication received message matches the wakeup */
        /* [cover parentID={397CDE2A-64F0-4cec-B81E-42DF58B135BD}]
        [/cover] */
        CanIf_RxIndication(Mailbox, PduInfoPtr);
        /* Set Exit Condition */
        MsgIndex = (IcomMsgIndex + IcomNoOfRxMsg);
      }
    }
    /* Increment to point next Message configuration*/
    MsgIndex++;
  }
}
#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */
#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           :  LOCAL_INLINE Std_ReturnType                           **
**                      Can_17_McmCan_lIcomCheckGreaterSmaller                **
**                     (                                                      **
**                          const uint8 ValData,const uint8 ConfigData,       **
**                          uint8* const CheckVal,                            **
**                  const Can_17_McmCan_IcomSignalOperType SignalOperType)    **
**                     )                                                      **
**                                                                            **
**  Description      : Validates the received message Signal values for       **
**                     the greater and lesser wakeup conditions.              **
** [cover parentID={A3BCA435-BA18-4435-B958-3572375DFFF9}]                    **
** [/cover]                                                                   **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : SignalIndex : Signal index of the configuration message**
**                                    to be compared with                     **
**                    DataIndex   : Pointer to the information of received    **
**                                   LPDU                                     **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK : The WakeUp condition is caused                  **
**                    E_NOT_OK : The WakeUp condition is not caused           **
**                                                                            **
*******************************************************************************/
LOCAL_INLINE Std_ReturnType Can_17_McmCan_lIcomCheckGreaterSmaller(
    const uint8 ValData, const uint8 ConfigData,
    uint8* const CheckVal,
    const Can_17_McmCan_IcomSignalOperType SignalOperType)
{
  /* API return status */
  Std_ReturnType ApiStatus;
  /* Initialize the Return status */
  ApiStatus =  E_NOT_OK;
  /* [cover parentID={4268D730-023D-44b7-AED0-5636D62E3096}]
  Is SignalOperType is Greater
  [/cover] */
  if(CAN_17_MCMCAN_ICOM_OPER_GREATER == SignalOperType)
  {
    /* [cover parentID={77E7A7AC-DCE7-4dda-859B-088E6BFF919C}]
    Verify the signal value and masked payload data
    [/cover] */
    /* Verify the signal value and masked payload data */
    /* [cover parentID={77E7A7AC-DCE7-4dda-859B-088E6BFF919C}]
    Data in is lesser than compare value
    [/cover] */
    if (ValData < ConfigData)
    {
      ApiStatus = E_NOT_OK;
      (*CheckVal)++;
    }
    /* [cover parentID={818C40E3-4242-4774-AC95-0A2C874EC77C}]
    Data in is greater than compare value
    [/cover] */
    else if ((ValData > ConfigData) && (CAN_17_MCMCAN_ZERO == (*CheckVal)))
    {
      ApiStatus = E_OK;
    }
    else
    {
      /* Do Nothing */
    }
  }
  /* [cover parentID={4268D730-023D-44b7-AED0-5636D62E3096}]
  Is SignalOperType is Smaller
  [/cover] */
  else
  {
    /* Verify the signal value and masked payload data */
    /* [cover parentID={8B9BFF52-0282-4bf5-B356-46D316416BB9}]
    Verify the signal value and masked payload data
    [/cover] */
    if (ValData > ConfigData)
    {
      ApiStatus = E_NOT_OK;
      (*CheckVal)++;
    }
    /* [cover parentID={77E7A7AC-DCE7-4dda-859B-088E6BFF919C}]
    Data in is lesser than compare value
    [/cover] */
    /* [cover parentID={E3F306D6-0C45-4d77-BCB9-0E0293CAC375}]
    Data in is lesser than compare value
    [/cover] */
    else if ((ValData < ConfigData) && (CAN_17_MCMCAN_ZERO == (*CheckVal)))
    {
      ApiStatus = E_OK;
    }
    else
    {
      /* Do Nothing */
    }
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */
#if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
/*******************************************************************************
**                                                                            **
**  Syntax           :  static Std_ReturnType                                 **
**                      Can_17_McmCan_lIcomValidateMsgSignal                  **
**                     (                                                      **
**                        const uint16 MessageIndex,                          **
**                        const PduInfoType* const PduInfoPtr,                **
**                        const Can_17_McmCan_ConfigType* const CanConfig     **
**                     )                                                      **
**                                                                            **
**  Description      : Validates the received message Signal values against   **
**                     the configured wakeup conditions.                      **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : MessageIndex : Signal Wakeup cause check corresponding **
**                                   Message configuration index              **
**                    PduInfoPtr   : Pointer to the information of received   **
**                                   LPDU                                     **
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK : The WakeUp condition is caused                  **
**                    E_NOT_OK : The WakeUp condition is not caused           **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lIcomValidateMsgSignal(
    const uint16 MessageIndex,
    const PduInfoType* const PduInfoPtr,
    const Can_17_McmCan_ConfigType* const CanConfig)
{
  uint32 SignalIndex;
  uint16 FirstSignalIndex;
  uint16 NoOfMsgSignal;
  uint8 MsgCtr;
  uint8 DataIndex;
  uint8 DataCnt;
  uint8 CheckIndex;
  uint8 TempData[CAN_17_MCMCAN_EIGHT];
  Std_ReturnType ApiStatus;

  /* [cover parentID={728ADB13-2277-4569-BBED-C6598A5BA794}]
  [/cover] */
  /* Initialize the temporary array */
  for(DataIndex = CAN_17_MCMCAN_ZERO; DataIndex < CAN_17_MCMCAN_EIGHT; DataIndex++)
  {
    TempData[DataIndex] = CAN_17_MCMCAN_ZERO;
  }
  /* Initialize the Return status */
  ApiStatus =  E_NOT_OK;
  /* First signal index */
  FirstSignalIndex = \
                     CanConfig->CanIcomMsgConfigPtr[MessageIndex].CanIcomFirstSignalIndx;
  /* No of signal configuration */
  NoOfMsgSignal = \
                  CanConfig->CanIcomMsgConfigPtr[MessageIndex].CanIcomNoOfSignalIndx;
  /* Number of bytes to be checked */
  MsgCtr = CanConfig->CanIcomMsgConfigPtr[MessageIndex].CanIcomDLC;
  /* Message signal validation */

  for (SignalIndex = FirstSignalIndex; SignalIndex <
       ((uint32)FirstSignalIndex + (uint32)NoOfMsgSignal); SignalIndex++)
  {

    /*Initialize check index */
    CheckIndex = CAN_17_MCMCAN_ZERO;
    /*MISRA2012_RULE_1_3_JUSTIFICATION:This rule violation
    is agreed as we need to store the address passed is in
    the called function in many scenarios.*/
    uint8* CheckIndex_ptr = &CheckIndex;
    DataCnt = MsgCtr;
    /* [cover parentID={260D89D2-2DEF-4358-B480-ED98DD35D2A1}]
    Reception of messages for configured n times
    [/cover] */
    while(CAN_17_MCMCAN_ZERO != DataCnt)
    {
      DataIndex = DataCnt - CAN_17_MCMCAN_ONE;
      DataCnt--;
      /* Mask the received payload and signal mask value */
      TempData[DataIndex] = (PduInfoPtr->SduDataPtr[DataIndex] \
                             & (CanConfig->CanIcomRxSignalConfigPtr[SignalIndex]. \
                                CanIcomSignalMask[DataIndex]));
      /* Get the Signal operation */
      switch(CanConfig->CanIcomRxSignalConfigPtr[SignalIndex].CanIcomSignalOper)
      {
        /* [cover parentID={3035480D-FFD2-4be8-93E9-3A9945ED2CF0}]
        The ICOM signal operation is AND
        [/cover] */
        case CAN_17_MCMCAN_ICOM_OPER_AND:
        {
          /* Verify the signal value and masked payload data */
          /* [cover parentID={FC150B53-99DD-48ce-BF32-0E3D01A27D6E}]
          Verify the signal value and masked payload data
          [/cover] */
          if (CAN_17_MCMCAN_ZERO != (TempData[DataIndex]& \
                                     (CanConfig->CanIcomRxSignalConfigPtr[SignalIndex]. \
                                      CanIcomSignalValue[DataIndex])))
          {
            ApiStatus = E_OK;
          }
        }
        break;
        /* Verify the signal value and masked payload data */
        /* [cover parentID={A06F59E0-00A6-4813-8EA0-ADE12DBC856E}]
        The ICOM signal operation is Equal
        [/cover] */

        case CAN_17_MCMCAN_ICOM_OPER_EQUAL:
        {
          /* [cover parentID={F1FFF568-488F-4246-AA49-6A856E037E26}]
          Verify the signal value and masked payload data
          [/cover] */
          /* Verify the signal value and masked payload data */
          if (TempData[DataIndex] == \
              (CanConfig->CanIcomRxSignalConfigPtr[SignalIndex]. \
               CanIcomSignalValue[DataIndex]))
          {
            CheckIndex++;
          }
          /* [cover parentID={8F95B2D2-2799-4dfa-9D20-D8A6903FDC1E}]
          Message counter configured
          [/cover] */
          if(CheckIndex == MsgCtr)
          {
            ApiStatus = E_OK;
          }
        }
        break;
        /* [cover parentID={87F7972F-F16B-4831-AFF1-335EE9FB239A}]
        The ICOM signal operation is Greater
        [/cover] */
        case CAN_17_MCMCAN_ICOM_OPER_GREATER:
        {
          /* [cover parentID={A2265027-B208-4012-8497-AD99EF5B938C}]
          Is Status ok?
          [/cover] */
          if(E_OK != ApiStatus)
          {
            ApiStatus = Can_17_McmCan_lIcomCheckGreaterSmaller \
                        (TempData[DataIndex], (CanConfig-> \
                                               CanIcomRxSignalConfigPtr[SignalIndex]. \
                                               CanIcomSignalValue[DataIndex]), \
                         CheckIndex_ptr, CAN_17_MCMCAN_ICOM_OPER_GREATER);
          }
        }
        break;
        /* [cover parentID={40C6BAB3-243F-46e8-859F-0857FAA3D8DA}]
        The ICOM signal operation is Smaller
        [/cover] */
        case CAN_17_MCMCAN_ICOM_OPER_SMALLER:
        {
          /* [cover parentID={F83A3F5A-408E-46cb-960A-843D3AC6B10A}]
          Is Status ok ?
          [/cover] */
          if(E_OK != ApiStatus)
          {
            ApiStatus = Can_17_McmCan_lIcomCheckGreaterSmaller(  \
                        TempData[DataIndex], (CanConfig-> \
                                              CanIcomRxSignalConfigPtr[SignalIndex]. \
                                              CanIcomSignalValue[DataIndex]), \
                        CheckIndex_ptr, CAN_17_MCMCAN_ICOM_OPER_SMALLER);
          }
        }
        break;
        default:
        {
          /* [cover parentID={A9027982-339E-47e4-B54D-05CD0CEF1382}]
          signal value and data when NORed if gives valid value
          [/cover] */
          if (CAN_17_MCMCAN_ZERO != (TempData[DataIndex] ^ \
                                     CanConfig->CanIcomRxSignalConfigPtr[SignalIndex]. \
                                     CanIcomSignalValue[DataIndex]))
          {
            ApiStatus = E_OK;
          }
        }
        break;
      }
    }
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */

/*******************************************************************************
            DET condition check internal functions
*******************************************************************************/
#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={19795EB8-F799-445c-94A7-D709E980FBC6}] **                                                              **
**                                                                            **
** Syntax :        static Std_ReturnType Can_17_McmCan_lInitDetCheck(         **
**                 const Can_17_McmCan_ConfigType *const Config,              **
**                 const uint8 CoreIndex )                                    **
**                                                                            **
** Description :      Local function :Service to do DET Check for Init        **
** [/cover]                                                                   **
**                                                                            **
** Service ID:        None                                                    **
**                                                                            **
** Sync/Async:        Synchronous                                             **
**                                                                            **
** Reentrancy:        Non Reentrant                                           **
**                                                                            **
** Parameters (in):   Config - CAN config pointer                             **
**                    CoreIndex  - Core which is calling this function        **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      Std_ReturnType: status of DET.                          **
**                            E_NOT_OK/E_OK                                   **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lInitDetCheck(
    const Can_17_McmCan_ConfigType *const Config,
    const uint8 CoreIndex)
{
  Std_ReturnType ApiStatus;
  /* Initialize the variable */
  ApiStatus = E_OK;

  /* To verify whether Init has been called with wrong parameter */
  /* Det Check for NULL pointer  */
  /* [cover parentID=  {AFC75F27-5860-4ac2-9C9D-4ECAF1B38F2D}]
    Det Check for NULL pointer
    [/cover] */
  if (NULL_PTR == Config)
  {
    /* [cover parentID={FF84B51F-837D-4b45-801D-462D5D647425}]
    [/cover] */
    /*Report Error, Init has been called with wrong parameter
      NULL Config Pointer */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                    CAN_17_MCMCAN_SID_INIT, CAN_17_MCMCAN_E_INIT_FAILED);
    /* Report Error */
    ApiStatus = E_NOT_OK;
  }

  #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  /* Check if current core is slave core */
  /* [cover parentID={2EF7682E-4DAE-4dc5-BF3A-F54F1DEDD108}]
    Check if current core is slave core
    [/cover] */
  else if(MCAL_MASTER_COREID != CoreIndex)
  {
    /* [cover parentID=  {9FCB486F-7866-474e-9621-D78D124E5378}]
       if core is configured
       [/cover] */
    /* Check if master core is initialized */
    if(CAN_17_MCMCAN_READY !=
        (Mcal_GetBitAtomic(Can_17_McmCan_InitStatus, \
                           (MCAL_MASTER_COREID * ((uint8)CAN_17_MCMCAN_INIT_STATUS_SIZE)), \
                           CAN_17_MCMCAN_INIT_STATUS_SIZE)))
    {
      ApiStatus = E_NOT_OK;
      /* [cover parentID={8176EBEA-757B-43d7-BC3E-1DFEC0BA802F}]
      [/cover] */
      /* Report error */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                      CAN_17_MCMCAN_SID_INIT, CAN_17_MCMCAN_E_MASTER_CORE_UNINIT);
    }
    /* [cover parentID= {262A989B-2938-4e77-8CBB-09C4786036B7}]
    if config ptr is same as master core
    [/cover] */
    /* Check if config ptr is same as master core */
    else if(Can_17_McmCan_kGblConfigPtr != Config)
    {
      ApiStatus = E_NOT_OK;
      /* [cover parentID= {0B962D30-099E-4a4b-8E2C-BCD0F4DE2EDF}]
      Report Error
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                      CAN_17_MCMCAN_SID_INIT, CAN_17_MCMCAN_E_INIT_FAILED);
    }
    /* [cover parentID= {D6915A11-622A-4ac4-A117-7F00336FB2A1}]
    if core is configured
    [/cover] */
    /* Check if core is configured */
    else if(NULL_PTR == Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex])
    {
      ApiStatus = E_NOT_OK;
      /* Report Error */
      /* [cover parentID= {57B0CDA5-A90D-47a1-954C-610EDE1A6889}]
      if core is configured
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                      CAN_17_MCMCAN_SID_INIT, CAN_17_MCMCAN_E_NOT_CONFIGURED);
    }
    else
    {
      /* Do Nothing */
    }
  }
  #endif /* #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */
  else
  {
    /* Do Nothing */
  }
  /* [cover parentID={545ACBF1-A513-4b28-9044-E15771BB30DC}]
    if core is initialized
    [/cover] */
  if((CAN_17_MCMCAN_READY == \
          (Mcal_GetBitAtomic(Can_17_McmCan_InitStatus, \
                             (((sint32)CoreIndex) * ((sint32)CAN_17_MCMCAN_INIT_STATUS_SIZE)), \
                             CAN_17_MCMCAN_INIT_STATUS_SIZE))) && (ApiStatus == E_OK) )
  {
    ApiStatus = E_NOT_OK;
    /* Report Error */
    /* [cover parentID={730A153B-CF7D-456c-8CD2-032476BB5C1F}]
    [/cover] */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                    CAN_17_MCMCAN_SID_INIT, CAN_17_MCMCAN_E_TRANSITION);
  }
  else
  {
    /* Do Nothing */
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */
#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={27564589-7252-4c12-B807-C6A1CA28D45B}] **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lDetUninitConfig   **
**                    (                                                       **
**                      const uint8 ServiceID,                                **
**                      const uint8 CoreIndex                                 **
**                    )                                                       **
**                                                                            **
**  Description      : Checks the status of CAN driver initialization         **
**                    If CAN driver is not initialized, reports to DET and    **
**                    return with E_NOT_OK                                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : ServiceId - Service Id of the calling API              **
**                     DriverState - The driver state of the controller       **
**                     whose state needs to be checked.                       **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : Std_ReturnType                                         **
**                  - E_OK: CAN Driver is initialized                         **
**                  - E_NOT_OK: CAN Driver is not initialized                 **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lDetUninitConfig(
    const uint8 ServiceID, const uint8 CoreIndex)
{
  Std_ReturnType ApiStatus;

  /* [cover parentID={18C37795-B9DA-482d-83B4-0FB9AE5AE239}]
  [/cover] */
  /* Check if CAN driver is un-initialized */
  if(CAN_17_MCMCAN_READY != \
      (Mcal_GetBitAtomic(Can_17_McmCan_InitStatus, \
                         (((sint32)CoreIndex) * ((sint32)CAN_17_MCMCAN_INIT_STATUS_SIZE)), \
                         CAN_17_MCMCAN_INIT_STATUS_SIZE)))
  {

    /* [cover parentID={68CAAF84-82ED-4b4e-BB5F-6D9938FA03CC}]
    [/cover] */
    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                    ServiceID, CAN_17_MCMCAN_E_UNINIT);
    /* return function call not sucessful*/
    ApiStatus = E_NOT_OK;
  }
  else
  {
    UNUSED_PARAMETER(ServiceID);
    /* Set as Drv state as Initialized*/
    ApiStatus = E_OK;
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

/*******************************************************************************
** Traceability     : [cover parentID={58BA3873-8649-4a01-A87C-8E03B7628694}] **                                                                            **
**  Syntax           : static Std_ReturnType                                  **
**                                       Can_17_McmCan_lMainDetUninitConfig   **
**                    (                                                       **
**                      const uint8 ServiceID,                                **
**                      const uint8 CoreIndex                                 **
**                    )                                                       **
**                                                                            **
**  Description      : Checks the status of CAN driver initialization         **
**                    If CAN driver is not initialized, reports to DET and    **
**                    return with E_NOT_OK                                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : ServiceId - Service Id of the calling API              **
**                     DriverState - The driver state of the controller       **
**                     whose state needs to be checked.                       **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : Std_ReturnType                                         **
**                  - E_OK: CAN Driver is initialized                         **
**                  - E_NOT_OK: CAN Driver is not initialized                 **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lMainDetUninitConfig(
    const uint8 ServiceID, const uint8 CoreIndex)
{
  Std_ReturnType ApiStatus;

  /* [cover parentID={004FAF33-C486-4b39-AA0A-F83571728B6A}]
  [/cover] */
  /* Check if CAN driver is un-initialized */
  if(CAN_17_MCMCAN_READY != \
      (Mcal_GetBitAtomic(Can_17_McmCan_InitStatus, \
                         (((sint32)CoreIndex) * ((sint32)CAN_17_MCMCAN_INIT_STATUS_SIZE)), \
                         CAN_17_MCMCAN_INIT_STATUS_SIZE)))
  {

    #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)

    /* [cover parentID={C13017D5-D5EC-441e-AD57-EAB109C1FEB0}]
    [/cover] */
    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                    ServiceID, CAN_17_MCMCAN_E_UNINIT);
    #endif
    /* return function call not sucessful*/
    ApiStatus = E_NOT_OK;
  }
  else
  {
    UNUSED_PARAMETER(ServiceID);
    /* Set as Drv state as Initialized*/
    ApiStatus = E_OK;
  }
  return ApiStatus;
}

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/* [cover parentID={6CDD66D5-A727-47d7-936C-250501448BCC}]
DET is enabled
[/cover] */
/*******************************************************************************
** Traceability     : [cover parentID={F3DCE1FB-C794-4cc3-9BA4-9E35206F6847}] **                                                                            **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lModeDetCheck      **
**                    (                                                       **
**                       const uint8 Controller,                              **
**                       const Can_StateTransitionType Transition,            **
**                     const Can_17_McmCan_CoreStatesType* const CanCoreState **
**                     )                                                      **
**                                                                            **
**  Description      : Checks the validity of software triggered CAN          **
**                    Controller state transition                             **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Non-Reentrant                                          **
**                                                                            **
**  Parameters (in)  : Controller - Associated CAN Controller Id              **
**                    Transition - Requested transition                       **
**                     CanCoreState - Pointer to global structure             **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK - Valid transition                                **
**                    E_NOT_OK - Invalid transition                           **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lModeDetCheck(const uint8 Controller,
    const Can_StateTransitionType Transition,
    const Can_17_McmCan_CoreStatesType* const CanCoreState)
{
  Std_ReturnType ApiStatus;
  ApiStatus = E_OK;

  switch (Transition)
  {
    /* [cover parentID={460052F2-FC60-41de-A2D3-1356A3BD22BE}]
    Current mode is Start
    [/cover] */
    /* Check if current mode is  Start */
    case CAN_T_START :
    {
      /* [cover parentID={5923BF72-0BF1-4c64-804E-A5090E2B5A2A}]
      Current mode is STOPPED
      [/cover] */
      /* Check if current mode is not STOPPED */
      if (CANIF_CS_STOPPED != CanCoreState->CanControllerModePtr[Controller])
      {
        ApiStatus = E_NOT_OK;
      }
    }
    break;

    /* Check if current mode is  Sleep */
    /* [cover parentID={1D410320-C7C2-488f-809E-65EBD8FE9D21}]
    Current mode is SLEEP
    [/cover] */
    case CAN_T_SLEEP :
    {
      /* [cover parentID={5D0E3B19-8273-41ee-8647-4F2ED32F19EC}]
      Current mode is STOPPED or SLEEP
      [/cover] */
      /* if current mode is neither SLEEP nor STOPPED */
      if((CANIF_CS_SLEEP != CanCoreState->CanControllerModePtr[Controller]) && \
          (CANIF_CS_STOPPED != CanCoreState->CanControllerModePtr[Controller]))
      {
        ApiStatus = E_NOT_OK;
      }
    }
    break;
    /* [cover parentID={96CB087E-4C25-4881-AAA7-17CAC4A30B90}]
      Current mode is Stop
      [/cover] */
    /* Check if current mode is  Stop */
    case CAN_T_STOP :
    {
      /* [cover parentID={3343288B-3C63-4861-9A74-322D38B97B2E}]
      Current mode is STOPPED or SLEEP
      [/cover] */
      /* if current mode is neither STARTED nor STOPPED */
      if((CANIF_CS_STARTED != \
          CanCoreState->CanControllerModePtr[Controller]) && \
          (CANIF_CS_STOPPED != CanCoreState->CanControllerModePtr[Controller]))
      {
        ApiStatus = E_NOT_OK;
      }
    }
    break;
    /* [cover parentID={D56EFC3C-497C-4541-9927-EBF8A0C9283B}]
      Current mode is Wake Up
      [/cover] */
    /* Check if current mode is  Wake Up */
    case CAN_T_WAKEUP :
    {
      /* [cover parentID={BCE0B849-7765-4c3b-8DF0-756EDB40572F}]
      Current mode is STOPPED or STARTED
      [/cover] */
      /* if current mode is neither SLEEP nor STOPPED */
      if((CANIF_CS_SLEEP != CanCoreState->CanControllerModePtr[Controller]) && \
          (CANIF_CS_STOPPED != CanCoreState->CanControllerModePtr[Controller]))
      {
        ApiStatus = E_NOT_OK;
      }
    }
    break;
    default:
    {
      /* Invalid state is requested */
      ApiStatus = E_NOT_OK;
    }
    break;
  }
  if(E_OK != ApiStatus)
  {
    /* [cover parentID={2D27662F-B422-4d24-B1E5-A0BFFA7AAF18}]
    Det error CAN_17_MCMCAN_E_TRANSITION
    [/cover] */
    /* Report invalid transition request to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_SETCONTROLLERMODE, \
                    CAN_17_MCMCAN_E_TRANSITION);
  }
  return ApiStatus;
}
#endif /*  #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={18939D9E-445D-4178-A039-4CFA1B36C798}] **                                                                            **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lWriteDetCheck     **
**                    (                                                       **
**                       const Can_HwHandleType Hth,                          **
**                       const Can_PduType* const PduInfo,                    **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                    )                                                       **
**                                                                            **
**  Description      : Handles DET checks for Can_17_McmCan_Write API         **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Hth - Hardware Transmit Handler                        **
**                    PduInfo - Pointer to SDU user memory, DLC and Identifier**
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK - No Development Error                            **
**                    E_NOT_OK - Development Error detected and reported      **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lWriteDetCheck(
    const Can_HwHandleType Hth,
    const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  Can_HwHandleType HthIndex;

  #if ((CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)|| \
  (CAN_17_MCMCAN_FD_ENABLE == STD_ON))
  uint8 LogicalControllerId;
  #endif

  #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  uint8 CoreIndex;
  #endif

  #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
  const volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 ControllerId;
  #endif

  Std_ReturnType ApiStatus = E_OK;

  #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  /* [cover parentID={7FD20809-DAE1-43a0-A455-0A7DC1ACB803}]
  Check if transmit objects are configured in this core
  [/cover] */
  if(NULL_PTR == CanConfig->CanTxHwObjectConfigPtr)
  {
    /* [cover parentID={1938CEB7-260A-405e-81EA-155BF2508492}]
    [/cover] */
    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_NOT_CONFIGURED);
    /* Transmit objects are not configured for this core */
    ApiStatus = E_NOT_OK;
  }
  /* [cover parentID={ABA556EB-B7FD-459c-811F-6C774F518AC9}]
  Sanity of the transmit hardware object by checking its range (include the Rx
  hardware object offset in the check)
  [/cover] */
  else if((Hth >= CAN_17_MCMCAN_NOOF_HARDWARE_OBJECTS) || \
            (Hth < Can_17_McmCan_kGblConfigPtr->CanNoOfHrh))
  {
    /* [cover parentID={154AD793-2358-4352-8745-21904858048B}]
    Report to DET
    [/cover] */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_HANDLE);
    /* Transmit objects are not configured for this core */
    ApiStatus = E_NOT_OK;
  }
  else
  {
    /* Extract HTH index for accessing corresponding configuration */
    HthIndex = Hth - (Can_17_McmCan_kGblConfigPtr->CanNoOfHrh);
    /* Set with the associated controller of requested HTH */
    LogicalControllerId = \
                          Can_17_McmCan_kGblConfigPtr->CanHthIndexPtr[HthIndex]. \
                          CanHthLogicContIndex;
    CoreIndex = CAN_17_MCMCAN_GETCOREID;
    /* [cover parentID={343A75B8-FF81-4cd9-A22C-F8338EE9A041}]
    check validity of controller
    [/cover] */
    ApiStatus = Can_17_McmCan_lDetParamController(CoreIndex, \
                LogicalControllerId, CAN_17_MCMCAN_SID_WRITE);
  }
  #else
  /* [cover parentID={ABA556EB-B7FD-459c-811F-6C774F518AC9}]
  Sanity of the transmit hardware object by checking its range (include the Rx
  hardware object offset in the check)
  [/cover] */
  if((Hth >= CAN_17_MCMCAN_NOOF_HARDWARE_OBJECTS) || \
        (Hth < Can_17_McmCan_kGblConfigPtr->CanNoOfHrh))
  {
    /* [cover parentID={154AD793-2358-4352-8745-21904858048B}]
    Report to DET
    [/cover] */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_HANDLE);
    /* Transmit objects are not configured for this core */
    ApiStatus = E_NOT_OK;
  }
  #endif /* #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */
  /* [cover parentID={088740AE-4624-4d0a-BE64-003CBE2994B4}]
  Controller ID valid
  [/cover] */
  /* [cover parentID={ABA556EB-B7FD-459c-811F-6C774F518AC9}]
  Sanity of the transmit hardware object by checking its range (include the Rx
  hardware object offset in the check)
  [/cover] */
  if (E_NOT_OK != ApiStatus)
  {
    /* [cover parentID={C7EBC16B-A4E5-42a5-BD51-6DCD865D9B78}]
    Pdu info pointer is a NULL_PTR
    [/cover] */
    if (PduInfo == NULL_PTR)
    {
      /* [cover parentID={D3112E3C-90F2-4211-A4B3-02C9D0FC4206}]
      Report to DET
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_POINTER);
      ApiStatus = E_NOT_OK;
    }

    #if (CAN_17_MCMCAN_FD_ENABLE == STD_OFF)
    /* check if DLC is greater than 8 */
    /* [cover parentID={08FE56F5-092A-4bd2-9EF9-7057842E888B}]
    check if DLC is greater than 8
    [/cover] */
    else if (PduInfo->length > CAN_17_MCMCAN_MAX_DATA_LENGTH )
    {
      /* [cover parentID={44B0D617-504A-48f2-AAA1-4ADED8C9CD41}]
      Det error CAN_17_MCMCAN_E_PARAM_DLC
      [/cover] */
      /* Report to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID,
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_DLC);
      /* The DLC length is > 8 */
      ApiStatus = E_NOT_OK;
    }
    #else /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_OFF) */
    /* [cover parentID={CF74323E-DE4B-4e2f-A84B-8DCA3B48D978}]
    DLC not in range
    [/cover] */
    else if (PduInfo->length > CAN_17_MCMCAN_MAX_FD_LENGTH)
    {
      /* [cover parentID={44B0D617-504A-48f2-AAA1-4ADED8C9CD41}]
      Det error CAN_17_MCMCAN_E_PARAM_DLC
      [/cover] */
      /* Report to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_DLC);
      /* The DLC length is > 64 */
      ApiStatus = E_NOT_OK;
    }
    #endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_OFF) */

    else
    {
      #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON)
      /* [cover parentID={B762F111-56A0-4c5d-8EBD-F4F611F40D9B}]
      PDU info out of range
      [/cover] */
      if(PduInfo->length > CAN_17_MCMCAN_MAX_DATA_LENGTH)
      {
        /* Extract HTH index for accessing corresponding configuration */
        HthIndex = Hth - (Can_17_McmCan_kGblConfigPtr->CanNoOfHrh);
        /* Set with the associated controller of requested HTH */
        LogicalControllerId = \
                              Can_17_McmCan_kGblConfigPtr->CanHthIndexPtr[HthIndex]. \
                              CanHthLogicContIndex;
        /* Set with the associated controller of requested HTH */
        ControllerId = Can_17_McmCan_kGblConfigPtr-> \
                       CanLogicalControllerIndexPtr[LogicalControllerId]. \
                       CanLCoreSpecContIndex;
        /* Controller Node Address */
        NodeRegAddressPtr = \
                            CanConfig->CanControllerConfigPtr[ControllerId].CanNodeAddress;
        /* Check the controller with FD mode */
        /* [cover parentID={8B484F34-2192-4711-B12A-593166E72ABC}]
        Check the controller with FD mode
        [/cover] */
        if( CAN_17_MCMCAN_BIT_SET_VAL != NodeRegAddressPtr->CCCR.B.FDOE)
        {
          /* [cover parentID={44B0D617-504A-48f2-AAA1-4ADED8C9CD41}]
          Det error CAN_17_MCMCAN_E_PARAM_DLC
          [/cover] */
          /* Report to DET */
          Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                          CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_DLC);
          /* Set controller without FD mode while FD request */
          ApiStatus = E_NOT_OK;
        }
        else
        {
          /* [cover parentID={1FE0382F-C545-40ed-8BCB-78D97DE14309}]
          [/cover] */
          /* Ckeck the message Id */
          if(CAN_17_MCMCAN_ID_FD_ENABLE_SET != \
              (PduInfo->id & CAN_17_MCMCAN_ID_FD_ENABLE_SET))
          {
            /* [cover parentID={44B0D617-504A-48f2-AAA1-4ADED8C9CD41}]
            Det error CAN_17_MCMCAN_E_PARAM_DLC
            [/cover] */
            /* Report to DET */
            Det_ReportError(CAN_17_MCMCAN_MODULE_ID, \
                            CAN_17_MCMCAN_INSTANCE_ID, CAN_17_MCMCAN_SID_WRITE, \
                            CAN_17_MCMCAN_E_PARAM_DLC);
            /* Set when controller in FD mode and DLC>8,but Id flag
            is not set properly */
            ApiStatus = E_NOT_OK;
          }
        }
      }
      #endif /* #if (CAN_17_MCMCAN_FD_ENABLE == STD_ON) */
    }
  }
  /* [cover parentID={6124CD9E-C2C4-40e1-992B-9A7947DA0430}]
  Check if sdu is a null pointer
  [/cover] */
  if (E_NOT_OK != ApiStatus)
  {
    /* Extract HTH index for accessing corresponding core specific
    configuration */
    HthIndex = Can_17_McmCan_kGblConfigPtr-> \
      CanHthIndexPtr[Hth - (Can_17_McmCan_kGblConfigPtr->CanNoOfHrh)]. \
        CanHthCoreSpecIndex;
    /* Check the validity of the CAN ID passed in the L-Pdu */
    /* [cover parentID={869D4AA2-E454-48e1-B9B6-BE15B7413AE4}]
    Valid CAN ID passed
    [/cover] */
    ApiStatus = Can_17_McmCan_lWriteIdCheck(HthIndex, PduInfo, CanConfig);
  }
  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={1E3A47B2-CF09-4091-AE09-0D558A65DFF6}] **                                                                           **
**  Syntax           : static Std_ReturnType Can_17_McmCan_lWriteIdCheck      **
**                    (                                                       **
**                       const Can_HwHandleType Hth,                          **
**                       const Can_PduType* const PduInfo,                    **
**                       const Can_17_McmCan_CoreConfigType* const CanConfig  **
**                    )                                                       **
**                                                                            **
**  Description      : Handles DET checks related to Message ID passed for    **
**                     Can_17_McmCan_Write API                                **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : Hth - Hardware Transmit Handler specific to core       **
**                    PduInfo - Pointer to SDU user memory, DLC and Identifier**
**                     CanConfig - Pointer to CAN driver configuration        **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : E_OK - No Development Error                            **
**                    E_NOT_OK - Development Error detected and reported      **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lWriteIdCheck(
    const Can_HwHandleType Hth, const Can_PduType* const PduInfo,
    const Can_17_McmCan_CoreConfigType* const CanConfig)
{
  #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
  boolean TrigStatus;
  #endif
  Std_ReturnType ApiStatus = E_OK;

  /* Check requested Id Type is same as with HTH CanIdType */
  /* [cover parentID={8E54F20B-46B6-4e12-84DD-FEAE40115C26}]
      Check requested Id Type is same as with HTH CanIdType
      [/cover] */
  if((uint8)CAN_17_MCMCAN_ID_EXTENDED == CanConfig->
      CanTxHwObjectConfigPtr[Hth].CanTxHwObjIdType)
  {
    /* Check Id type is extended or not */
    /* [cover parentID={3738E817-874C-4c93-88AC-EBEEC072E92D}]
      Check Id type is extended or not
      [/cover] */
    if(CAN_17_MCMCAN_SET_XTD_BIT_MASK != (PduInfo->id & \
                                          CAN_17_MCMCAN_SET_XTD_BIT_MASK))
    {

      /* [cover parentID={278FC26B-FBF3-4c0e-A8AB-0BF302F92B2A}]
      [/cover] */
      /* Report to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_MSGID);
      /* Requested with not supported CanId Type message  */
      ApiStatus = E_NOT_OK;
    }
  }
  /* [cover parentID={5D1530A7-7FA2-4737-800C-640AAFEFB6CF}]
      Check Id type is Standard or not
      [/cover] */
  else if((uint8)CAN_17_MCMCAN_ID_STANDARD == \
          CanConfig->CanTxHwObjectConfigPtr[Hth].CanTxHwObjIdType)
  {
    /* Check Id type is extended or not */
    /* [cover parentID={E062AC43-44B7-4064-9F40-99327AC4CD3E}]
      Check Id type is Standard or not
      [/cover] */
    if(CAN_17_MCMCAN_SET_XTD_BIT_MASK == (PduInfo->id & \
                                          CAN_17_MCMCAN_SET_XTD_BIT_MASK))
    {

      /* [cover parentID={278FC26B-FBF3-4c0e-A8AB-0BF302F92B2A}]
      Report to DET
      [/cover] */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_MSGID);
      /* Requested with not supported CanId Type message  */
      ApiStatus = E_NOT_OK;
    }
  }
  else
  {
    /* Do nothing */
  }
  /* [cover parentID={4770EDB2-C2EC-43f2-B8E2-A7E2BE29209F}]
  [/cover] */
  /* [cover parentID={9D09780A-1403-4d7c-8558-4B1EF0AC0B4E}]
  [/cover] */
  #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON)
  /* [cover parentID={F3AC897B-FA20-4064-BC3D-8263A651D084}]
  Is Status ok
  [/cover] */
  if(E_NOT_OK != ApiStatus)
  {
    /* Read requested HTH is with trigger transmit API is enabled  */
    TrigStatus = \
                 CanConfig->CanTxHwObjectConfigPtr[Hth].CanTrigTxStatus;
    /* Transmit trigger status is False or the sdu in the pdu info is a NULL_PTR
    Check Trigger status */
    if((FALSE == TrigStatus) && (NULL_PTR == PduInfo->sdu))
    {
      /* [cover parentID={44717DBB-E03B-42e9-94EB-B5AC90182EDE}]
      Det error CAN_17_MCMCAN_E_PARAM_POINTER
      [/cover] */
      /* Report to DET */
      Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                      CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_POINTER);
      /* Trigger transmit API is disabled for the requested HTH */
      ApiStatus = E_NOT_OK;
    }
  }
  #else
  /* Check sdu info */
  if((E_NOT_OK != ApiStatus) && (NULL_PTR == PduInfo->sdu))
  {
    /* [cover parentID={44717DBB-E03B-42e9-94EB-B5AC90182EDE}]
    Det error CAN_17_MCMCAN_E_PARAM_POINTER
    [/cover] */
    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    CAN_17_MCMCAN_SID_WRITE, CAN_17_MCMCAN_E_PARAM_POINTER);
    /* Trigger transmit API is disabled for the requested HTH */
    ApiStatus = E_NOT_OK;
  }
  #endif /* #if (CAN_17_MCMCAN_TRIG_TRANSMIT == STD_ON) */

  return ApiStatus;
}

#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

#if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON)
/*******************************************************************************
** Traceability     : [cover parentID={24945FE4-B6E6-451a-B8D2-6B09C759D8A8}] **                                                                            **
**  Syntax     : static Std_ReturnType Can_17_McmCan_lDetParamController      **
**                    (                                                       **
**                      const uint8 CoreIndex,                                **
**                      const uint8 Controller,                               **
**                      const uint8 ServiceID,                                **
**                    )                                                       **
**                                                                            **
**  Description      : Checks if ControllerId is a valid CAN Controller Id    **
**                    If ControllerId is not valid, then report to DET and    **
**                    return with E_NOT_OK                                    **
**  [/cover]                                                                  **
**                                                                            **
**  Service ID       : NA                                                     **
**                                                                            **
**  Sync/Async       : Synchronous                                            **
**                                                                            **
**  Reentrancy       : Reentrant                                              **
**                                                                            **
**  Parameters (in)  : CoreIndex - Core which is calling this function        **
**  Parameters (in)  : Controller - Associated CAN Controller Id              **
**                     ServiceId - Service Id of the calling API              **
**                                                                            **
**  Parameters (out) : none                                                   **
**                                                                            **
**  Return value     : Std_ReturnType                                         **
**                   - E_OK: ControllerId is valid                            **
**                   - E_NOT_OK: ControllerId is invalid                      **
**                                                                            **
*******************************************************************************/
static Std_ReturnType Can_17_McmCan_lDetParamController(
    const uint8 CoreIndex,
    const uint8 Controller,
    const uint8 ServiceID)
{
  Std_ReturnType ApiStatus;

  ApiStatus = E_OK;

  /* Check 'ControllerId' is out of range */
  /* [cover parentID={378E7144-5309-46ea-852E-C7D6FF243F20}]
   Is ControllerId' is out of range
   [/cover] */
  if(Controller >= CAN_17_MCMCAN_NOOF_CONTROLLER)
  {
    /* [cover parentID={433A06CD-4D92-43c7-B000-7DE89BC6AD28}]
    Det error CAN_17_MCMCAN_E_PARAM_CONTROLLER
    [/cover] */
    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    ServiceID, CAN_17_MCMCAN_E_PARAM_CONTROLLER);
    /* Set as requested controller ID is Invalid */
    ApiStatus = E_NOT_OK;
  }
  #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON)
  /* [cover parentID={F58BF72A-4FFD-4b58-B3B4-4CA2CBFC9D58}]
  Is Controllerallocated to current core
  [/cover] */
  if((E_NOT_OK != ApiStatus) && (CoreIndex != \
                                 Can_17_McmCan_kGblConfigPtr->CanLogicalControllerIndexPtr[Controller]. \
                                 CanLCoreAssigned))
  {
    /* [cover parentID={7026EB9F-2D08-4ed2-994D-23A8227F5450}]
    Det error CAN_17_MCMCAN_E_NOT_CONFIGURED
    [/cover] */

    /* Report to DET */
    Det_ReportError(CAN_17_MCMCAN_MODULE_ID, CAN_17_MCMCAN_INSTANCE_ID, \
                    ServiceID, CAN_17_MCMCAN_E_NOT_CONFIGURED);
    /* Set as requested controller ID is Invalid */
    ApiStatus = E_NOT_OK ;
  }
  #endif /*   #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_ON) */

  #if (CAN_17_MCMCAN_MULTICORE_ERROR_DETECT == STD_OFF)
  UNUSED_PARAMETER(CoreIndex);
  #endif

  return ApiStatus;
}
#endif /* #if (CAN_17_MCMCAN_DEV_ERROR_DETECT == STD_ON) */

/*******************************************************************************
            Interrupt Handler
*******************************************************************************/

#if(CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
** Traceability   : [cover parentID={D47A49B6-985A-4c4f-86D5-C17125DAE268}]   **
**                                                                            **
**                                                                            **
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
void Can_17_McmCan_IsrTransmitHandler(const uint8 HwKernelId,
                                      const uint8 NodeIdIndex)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 CntrlIndx;
  uint8 CoreIndex;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  uint8 CheckCount = 0;
  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
  /* Local copy of the core specific global states */
  CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
  /* Retrive core specific controller index */
  /* [cover parentID={72BDFFC9-A38F-4bd9-B5C7-DD371E8ECD40}]
  Controller index passed is configured for INTERRUPT Handling
 [/cover] */
  CntrlIndx = Can_17_McmCan_kGblConfigPtr->CanPhyControllerIndexPtr[ \
              (CAN_17_MCMCAN_FOUR * HwKernelId) + NodeIdIndex].CanPCoreSpecContIndex;
  /* Check the controller mode is Interrupt or not */
  /* [cover parentID={72BDFFC9-A38F-4bd9-B5C7-DD371E8ECD40}]
    Check the controller mode is Interrupt or not
    [/cover] */
  if(CAN_17_MCMCAN_POLLING != CanConfig-> \
      CanEventHandlingConfigPtr[CntrlIndx].CanEventType[CAN_17_MCMCAN_ZERO])
  {
      /* Set with Node Register Start address */
    NodeRegAddressPtr = \
                        CanConfig->CanControllerConfigPtr[CntrlIndx].CanNodeAddress;
    /* Check if Tx Event FIFO New Entry is stored */
    /* [cover parentID={2C77992E-569E-461f-9A24-894A72EDCC38}]
    Loop for TEFN bit clear
    [/cover] */
    while((CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->IR.B.TEFN) && \
        (CheckCount < CAN_17_MCMCAN_FLAG_CHECK_COUNT))
    {
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IE_TEFN_BIT;
      CheckCount++;
    }

    /* [cover parentID={37BEC4E9-A8B9-46eb-959D-5A093D039C26}]
    If Config Pointer is not NULL
    [/cover] */
    if(NULL_PTR != CanConfig->CanTxHwObjectConfigPtr)
    {
     /* If New Event Entry is present then invoke Tx handler */
     /* [cover parentID={F6D5E63E-B4A0-4cff-9931-69495A1D6247}]
     Invoke Tx Handler
     [/cover] */
     Can_17_McmCan_lTxEventHandler(CntrlIndx, CanCoreState, CanConfig);
    }
  }

}
#endif /* #if(CAN_17_MCMCAN_TX_INTERRUPT_PROCESSING == STD_ON) */

#if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
** Traceability   : [cover parentID={B91B38C0-0F62-4a9a-BF22-9EC231B6521E}]   **
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
void Can_17_McmCan_IsrReceiveHandler(const uint8 HwKernelId,
                                     const uint8 NodeIdIndex)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 CntrlIndx;
  uint8 CoreIndex;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  uint8 CheckCount = 0;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
  /* Local copy of the core specific global states */
  CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
  /* Retrive core specific controller index */
  CntrlIndx = Can_17_McmCan_kGblConfigPtr->CanPhyControllerIndexPtr[ \
              (CAN_17_MCMCAN_FOUR * HwKernelId) + NodeIdIndex].CanPCoreSpecContIndex;
  /* Check the controller mode is Interrupt or not */
  /* [cover parentID={3D8A9BC8-3DB0-4663-9882-728500A9D783}]
  Controller index passed is configured for INTERRUPT Handling
  [/cover] */
  if(CAN_17_MCMCAN_POLLING != \
      CanConfig->CanEventHandlingConfigPtr[CntrlIndx]. \
      CanEventType[CAN_17_MCMCAN_ONE])
  {
      /* Set with Node Register Start address */
      NodeRegAddressPtr = \
                        CanConfig->CanControllerConfigPtr[CntrlIndx].CanNodeAddress;
      /* Check if any of the Message stored to Dedicated Rx Buffer */
      /* [cover parentID={4B1788E8-57B4-4d14-BB70-8CCEE77EB482}]
      Loop for DRX bit clear
      [/cover] */
      while((CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->IR.B.DRX) && \
            (CheckCount < CAN_17_MCMCAN_FLAG_CHECK_COUNT))
      {
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IE_DRX_BIT;
        CheckCount++;
      }
      /* [cover parentID={FD1329C9-2BBF-483a-A187-46467202ECD1}]
      Is Controller mode in CANIF_CS_STARTED
      [/cover] */
      /* If current state is START Invoke receive handler function */
      if(CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CntrlIndx])
      {
        /* Call receive handler with buffer type as dedicated */
        /* [cover parentID={6C716C92-D9AA-41c5-BD2B-38FC96CD61E9}]
        Invoke Rx Handler
        [/cover] */
        Can_17_McmCan_lReceiveHandler(CntrlIndx, CAN_17_MCMCAN_ONE, \
                                      CanCoreState, CanConfig);
      }
  }
}
#endif /* #if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON) */

#if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
** Traceability   : [cover parentID={ADE7805B-51A9-4d63-8F4E-91F628C4625C}]   **
**                                                                            **
** Syntax           : void Can_17_McmCan_IsrRxFIFOHandler                     **
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
void Can_17_McmCan_IsrRxFIFOHandler (const uint8 HwKernelId,
                                     const uint8 NodeIdIndex)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 CntrlIndx;
  uint8 CoreIndex;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;
  uint8 CheckCount = 0;
  Ifx_UReg_32Bit RxFxwm;
  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
  /* Local copy of the core specific global states */
  CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
  /* Retrive core specific controller index */
  CntrlIndx = Can_17_McmCan_kGblConfigPtr->CanPhyControllerIndexPtr[ \
              (CAN_17_MCMCAN_FOUR * HwKernelId) + NodeIdIndex].CanPCoreSpecContIndex;
  /* Check the controller mode is Interrupt or not */
  /* [cover parentID={025B7A9D-6559-4d8c-BBA7-AE2C19C76739}]
  Controller index passed is configured for INTERRUPT Handling
  [/cover] */
  if(CAN_17_MCMCAN_POLLING != \
      CanConfig->CanEventHandlingConfigPtr[CntrlIndx]. \
      CanEventType[CAN_17_MCMCAN_ONE])
  {
    /* Set with Node Register Start address */
    NodeRegAddressPtr = \
                        CanConfig->CanControllerConfigPtr[CntrlIndx].CanNodeAddress;

    RxFxwm = NodeRegAddressPtr->RX.F0C.B.F0WM;
    /* Check if FIFO 0 level is greater than or equal to WM */
    /* [cover parentID={F09D073D-1AA6-4b42-83F0-7C91C64B08C8}]
    Check if FIFO 0 level is greater than or equal to WM
    [/cover] */
    /* MISRA2012_RULE_13_5_JUSTIFICATION: No side effects seen during
    this condition check. */
    if(NodeRegAddressPtr->RX.F0S.B.F0FL >= RxFxwm)
    {
      /* [cover parentID={ED7EE307-164D-40e3-BF67-6917034A2CDB}]
      Loop till the RXFIFO 0 watermark bit is cleared
      [/cover] */
      /* [cover parentID={6ACA260B-27BC-4805-9DC8-BF9F4F09531A}]
      Check if the RXFIFO 0 bit is set and retry count is less than 3
      [/cover] */
      while(((NodeRegAddressPtr->IR.U & CAN_17_MCMCAN_CLEAR_IR_RF0_WF_BITS) != 0U) &&
            (CheckCount < CAN_17_MCMCAN_FLAG_CHECK_COUNT))
      {
        /* [cover parentID={6ACA260B-27BC-4805-9DC8-BF9F4F09531A}]
        Check if the RXFIFO 0 bit is set and retry count is less than 3
        [/cover] */
        /* [cover parentID={4EEBC0C3-5ACC-45e2-84D7-5653FC0C3496}]
        Clear the RXXFIFO0 bit and increment the counter
        [/cover] */
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IR_RF0_WF_BITS;
        CheckCount++;
      }

      /* [cover parentID={AB57AAFD-A47C-4e6e-BD8C-1DBADF909240}]
      Is current state STARTED?
      [/cover] */
      /* If current state is STARTED Invoke receive handler function */
      if(CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CntrlIndx])
      {
        /* Call receive handler with buffer type as RXFIFO0 */
        /* [cover parentID={32835110-AFCD-4a05-801B-BFA6978ECE82}]
        Invoke Rx Handler
        [/cover] */
        Can_17_McmCan_lReceiveHandler(CntrlIndx, CAN_17_MCMCAN_TWO, \
                                      CanCoreState, CanConfig);
      }
    }

    RxFxwm = NodeRegAddressPtr->RX.F1C.B.F1WM;
    /* Check if FIFO 1 level is greater than or equal to WM */
    /* [cover parentID={CDC83D92-70B2-41ac-AF14-868EA78E3632}]
    Check if FIFO 1 level is greater than or equal to WM
    [/cover] */
    /* MISRA2012_RULE_13_5_JUSTIFICATION: No side effects seen during
    this condition check. */
    if(NodeRegAddressPtr->RX.F1S.B.F1FL >= RxFxwm)
    {
      CheckCount = 0;
      /* Reset RXFIFO1 flags */
      /* [cover parentID={32FD0856-60DC-4a93-A4DF-4FA3574FADA7}]
      Loop till RXFIFO1 watermark bit is clear
      [/cover] */
      /* [cover parentID={11071436-19A8-44a5-B699-0C92F440661F}]
      Clear RF1 watermark bit and increment retry counter
      [/cover] */
      while(((NodeRegAddressPtr->IR.U & CAN_17_MCMCAN_CLEAR_IR_RF1_WF_BITS) != 0U) &&
            (CheckCount < CAN_17_MCMCAN_FLAG_CHECK_COUNT))
      {
        /* [cover parentID={E1F85112-21F4-4e59-929B-41B347964203}]
        Check if the RF1 watermark bit is SET and the retry count is less than 3
        [/cover] */
        NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IR_RF1_WF_BITS;
        CheckCount++;
      }

      /*[cover parentID= {A31629D4-9B4B-43ee-83AF-529B0FD2BBBA}]
      If current state is STARTED Invoke receive handler function
      [/cover] */
      /* If current state is STARTED Invoke receive handler function */
      if(CANIF_CS_STARTED == CanCoreState->CanControllerModePtr[CntrlIndx])
      {
        /* Call receive handler with buffer type as RXFIFO1 */
        /* [cover parentID={9D37E710-1493-4e31-8E04-952FC67DCCAF}]
        Invoke Rx Handler
        [/cover] */
        Can_17_McmCan_lReceiveHandler(CntrlIndx, CAN_17_MCMCAN_THREE, \
                                      CanCoreState, CanConfig);
      }
    }
  }
}
#endif /* #if(CAN_17_MCMCAN_RX_INTERRUPT_PROCESSING == STD_ON) */

#if(CAN_17_MCMCAN_BO_INTERRUPT_PROCESSING == STD_ON)
/*******************************************************************************
** Traceability   : [cover parentID={7F1E55A5-7758-442f-8BF8-2A8C1F035412}]   **
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
void Can_17_McmCan_IsrBusOffHandler (const uint8 HwKernelId,
                                     const uint8 NodeIdIndex)
{
  volatile Ifx_CAN_N *NodeRegAddressPtr;
  uint8 CntrlIndx;
  uint8 CoreIndex;
  const Can_17_McmCan_CoreStatesType* CanCoreState;
  const Can_17_McmCan_CoreConfigType* CanConfig;

  /* Get core id which is executing currently */
  CoreIndex = CAN_17_MCMCAN_GETCOREID;
  /* Local copy of the core specific configuration */
  CanConfig = Can_17_McmCan_kGblConfigPtr->CanCoreConfigPtr[CoreIndex];
  /* Local copy of the core specific global states */
  CanCoreState = Can_17_McmCan_GblCoreState[CoreIndex];
  /* Retrive core specific controller index */
  CntrlIndx = Can_17_McmCan_kGblConfigPtr->CanPhyControllerIndexPtr[ \
              (CAN_17_MCMCAN_FOUR * HwKernelId) + NodeIdIndex].CanPCoreSpecContIndex;
  /* Check the BusOff for the controller mode is Interrupt or not */
  /* [cover parentID={DE031008-3078-4193-8F5F-44BF79E0C8F3}]
      Check the BusOff for the controller mode is Interrupt or not
      [/cover] */
  if(CAN_17_MCMCAN_POLLING != \
      CanConfig->CanEventHandlingConfigPtr[CntrlIndx]. \
      CanEventType[CAN_17_MCMCAN_TWO])
  {
    /* Set with Node Register Start address */
    NodeRegAddressPtr = \
                        CanConfig->CanControllerConfigPtr[CntrlIndx].CanNodeAddress;
    /* [cover parentID={67FB7A19-A0C2-4f20-B7CA-C939C08D02E5}]
      Check if BusOff status is changed
    [/cover] */
    if( CAN_17_MCMCAN_BIT_SET_VAL == NodeRegAddressPtr->IR.B.BO)
    {
      /* [cover parentID={B0977C05-ED20-4272-9A62-12151CB559C2}]
      Reset the Bus off flag
      [/cover] */
      #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON)
      if((TRUE != CanCoreState->CanIcomEnableStatusPtr[CntrlIndx]) || \
          (FALSE != CanCoreState->CanIcomConfgBOStatusPtr[CntrlIndx]))
      {
        /* If current state is STARTED
        Invoke BusOff handler function */
        Can_17_McmCan_lBusOffHandler(CntrlIndx, CanCoreState, CanConfig);
      }
      #else /* #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */
      /* If current state is STARTED Invoke BusOff handler function */
      Can_17_McmCan_lBusOffHandler(CntrlIndx, CanCoreState, CanConfig);
      #endif /* #if(CAN_17_MCMCAN_PUBLIC_ICOM_SUPPORT == STD_ON) */

      /* Reset BO flag */
      NodeRegAddressPtr->IR.U = CAN_17_MCMCAN_CLEAR_IE_BO_BIT;
    }
  }
}
#endif /* #if(CAN_17_MCMCAN_BO_INTERRUPT_PROCESSING == STD_ON) */
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
/* MISRA2012_RULE_20_1_JUSTIFICATION: Memmap header usage as per Autosar
guideline. */
/* MISRA2012_RULE_4_10_JUSTIFICATION: Memmap header is repeatedly included
without safegaurd. It complies to Autosar guidelines. */
#include "Can_17_McmCan_MemMap.h"
