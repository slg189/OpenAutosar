

#ifndef ETHTSYN_H
#define ETHTSYN_H

#include "StbM.h"
#include "EthTSyn_Types.h"
#include "EthTSyn_Cfg.h"

#ifdef ETHTSYN_CONFIGURED

/* Vendor Id */
#define ETHTSYN_VENDOR_ID                       6U

/* EthTSyn module Id */
#define ETHTSYN_MODULE_ID                       164U

/* EthTSyn instance Id */
#define ETHTSYN_INSTANCE_ID                     0x0U

#define ETHTSYN_CODE

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/* EthTSyn_00035: Initialises the Time Synchronization over Ethernet */
void EthTSyn_Init( const EthTSyn_ConfigType *configPtr );

/* EthTSyn_00036: Returns the version information of this module */
void EthTSyn_GetVersionInfo( Std_VersionInfoType *versioninfo );

/* EthTSyn_00039: This API is used to turn on and off the TX capabilities of the EthTSyn */
void EthTSyn_SetTransmissionMode( uint8                        CtrlIdx,
                                  EthTSyn_TransmissionModeType Mode    );

# if( ETHTSYN_ECUC_RB_RTE_IN_USE == STD_OFF)
/* EthTSyn_00044: Main function for cyclic call / resp. Sync, Follow_Up and Pdelay_Req transmissions */
void EthTSyn_MainFunction( void );
# endif

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

/*
 ***************************************************************************************************
 * Extern declarations
 ***************************************************************************************************
 */
#define ETHTSYN_START_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"

extern const EthTSyn_ConfigType EthTSyn_ConfigSet;

#define ETHTSYN_STOP_SEC_CONST_UNSPECIFIED
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_CONFIGURED */

#endif /* ETHTSYN_H */
