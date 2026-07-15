

#ifndef ETHIF_CBK_H
#define ETHIF_CBK_H

#include "EthIf_Types.h"
/* BSW-866 */
#include "EthIf_Integration.h"

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"
/* ETHIF085: Handles a received frame received by the indexed controller */
void EthIf_RxIndication               ( uint8 CtrlIdx,
                                        Eth_FrameType FrameType,
                                        boolean IsBroadcast,
                                        uint8 * PhysAddrPtr,
                                        Eth_DataType * DataPtr,
                                        uint16 LenByte );

/* ETHIF091: Confirms frame transmission by the indexed controller */
/* BSW-866 */
/* BSW-875 */
void EthIf_TxConfirmation_Internal    ( uint8 CtrlIdx,
                                        uint8 BufIdx,
                                        const Eth_DataType * DataPtr,
                                        uint16 LenByte );
/* END BSW-875 */
/* END BSW-866 */

/* ETHIF232: Indicate the mode change of a Transceiver */
void EthIf_TrcvModeIndication         ( uint8 TrcvIdx,
                                        EthTrcv_ModeType TrcvMode );


/* ETHIF91055: Indicates if the port mode changes for a particular port of the switch. */
void EthIf_SwitchPortModeIndication   ( uint8 SwitchIdx,
                                        uint8 SwitchPortIdx,
                                        EthTrcv_ModeType PortMode );

#if (ETHIF_SWITCH_ENABLE_LINKSTATE_INTERRUPT == STD_ON)
/* RB SPECIFIC: Indicate the link state change to ACTIVE of a Switch Port */
void EthIf_SwitchLinkUpIndication     ( uint8 SwitchIdx,
                                        uint8 PortIdx  );

/* RB SPECIFIC: Indicate the link state change to DOWN of a Switch Port */
void EthIf_SwitchLinkDownIndication   ( uint8 SwitchIdx,
                                        uint8 PortIdx  );
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#define ETHIF_START_SEC_CODE_FAST
#include "EthIf_MemMap.h"
#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON )
/* RB SPECIFIC: Confirms ZeroCopy frame transmission by the indexed controller */
void EthIf_ZeroCopyTxConfirmation( uint8 CtrlIdx_u8,
                                   uint16 TxId_u16,
                                   const Eth_DataType * HeaderPtr_pcu8,
                                   uint16 HeaderLen_u16 );
#endif
#define ETHIF_STOP_SEC_CODE_FAST
#include "EthIf_MemMap.h"

#endif /* ETHIF_CBK_H */
