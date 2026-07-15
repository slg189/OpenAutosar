


#ifndef ETHTSYN_CBK_H
#define ETHTSYN_CBK_H

#include "Eth_GeneralTypes.h"            /* Required for Eth_FrameType */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/* ETHTSYN_00040: Handles a received frame received by the indexed controller */
void EthTSyn_RxIndication( uint8          CtrlIdx,
                           Eth_FrameType  FrameType,
                           boolean        IsBroadcast,
                           uint8         *PhysAddrPtr,
                           uint8         *DataPtr,
                           uint16         LenByte );

/* ETHTSYN_00041: Confirms frame transmission by the indexed controller */
void EthTSyn_TxConfirmation( uint8 CtrlIdx,
                             uint8 BufIdx  );

/* ETHTSYN_00043: Allows resetting state machine in case of unexpected Link loss */
void EthTSyn_TrcvLinkStateChg( uint8                 CtrlIdx,
                               EthTrcv_LinkStateType TrcvLinkState );

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_CBK_H */
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


/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
*/


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
