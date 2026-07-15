

#ifndef ETHSM_H_
#define ETHSM_H_

#include "ComStack_Types.h"
#include "EthSM_Cfg.h"
#ifdef ETHSM_CONFIGURED

/* Vendor Id */
#define ETHSM_VENDOR_ID                         6U

/* Module Id */
#define ETHSM_MODULE_ID                         143U

/* EthSM instance Id */
#define ETHSM_INSTANCE_ID                       0U

/* SOFTWARE VERSION INFORMATION */
#define ETHSM_SW_MAJOR_VERSION                  10U
#define ETHSM_SW_MINOR_VERSION                  0U
#define ETHSM_SW_PATCH_VERSION                  0U

/* AUTOSAR Version Info */
#define ETHSM_AR_RELEASE_MAJOR_VERSION          4
#define ETHSM_AR_RELEASE_MINOR_VERSION          2
#define ETHSM_AR_RELEASE_REVISION_VERSION       2

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthSM_Types.h"

#include "ComM.h"
#if (!defined(COMM_AR_RELEASE_MAJOR_VERSION) || (COMM_AR_RELEASE_MAJOR_VERSION != ETHSM_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(COMM_AR_RELEASE_MINOR_VERSION) || (COMM_AR_RELEASE_MINOR_VERSION != ETHSM_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif

#include "TcpIp_EthSM.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHSM_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthSM_MemMap.h"

/* Global variable declaration */
/* EthSM network current status per EthIfCtrlIdx */
extern EthSM_NetworkModeStateType    EthSM_NetState_aen[ETHSM_NUM_OF_ETHIFCTRL];

/* ComM mode request stored per EthIfCtrlIdx */
extern ComM_ModeType                 EthSM_ComMReqMode_au8[ETHSM_NUM_OF_ETHIFCTRL];

/* Ctrl mode provided by EthSM_CtrlModeIndication() and is stored per EthIfCtrlIdx */
extern Eth_ModeType                  EthSM_EthIfCtrlMode_aen[ETHSM_NUM_OF_ETHIFCTRL];

/* Trcv link status provided by EthSM_TrcvLinkStateChg() and is stored per EthIfCtrlIdx */
extern EthTrcv_LinkStateType         EthSM_TrcvLinkState_aen[ETHSM_NUM_OF_ETHIFCTRL];

/* TcpIpCtrl status provided by EthSM_TcpIpModeIndication() and is stored per EthIfCtrlIdx */
extern TcpIp_StateType               EthSM_TcpIpState_aen[ETHSM_NUM_OF_ETHIFCTRL];

/* Stores unexpected hardware mode DOWN indication and is stored per EthIfCtrl */
extern boolean                       EthSM_HwError_ab[ETHSM_NUM_OF_ETHIFCTRL];

/* Config pointer */
extern const EthSM_ConfigType *      EthSM_EthChnlPtr_pco;

#define ETHSM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthSM_MemMap.h"


/*
 ***************************************************************************************************
 * Function declaration
 ***************************************************************************************************
 */

#define ETHSM_START_SEC_CODE
#include "EthSM_MemMap.h"

void EthSM_Init ( void );

#if(ETHSM_VERSIONINFO_API == STD_ON)
void EthSM_GetVersionInfo               ( Std_VersionInfoType * versioninfo );
#endif

Std_ReturnType EthSM_RequestComMode     ( NetworkHandleType NetworkHandle, ComM_ModeType ComM_Mode );

Std_ReturnType EthSM_GetCurrentComMode  ( NetworkHandleType NetworkHandle, ComM_ModeType * ComM_ModePtr);

/* If RTE is enabled, the main function declarations are present in SchM_EthSM.h. */
#if( ETHSM_ECUC_RB_RTE_IN_USE == STD_OFF )
void EthSM_MainFunction                 ( void );
#endif

#define ETHSM_STOP_SEC_CODE
#include "EthSM_MemMap.h"

#endif /* ETHSM_CONFIGURED */
#endif /* ETHSM_H_ */
