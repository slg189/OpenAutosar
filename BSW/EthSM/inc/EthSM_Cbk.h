

#ifndef ETHSM_CBK_H_
#define ETHSM_CBK_H_

#include "EthSM_Cfg.h"
#ifdef ETHSM_CONFIGURED

#include "TcpIp.h"

#define ETHSM_START_SEC_CODE
#include "EthSM_MemMap.h"

extern void EthSM_CtrlModeIndication( uint8 CtrlIdx,
                                      Eth_ModeType CtrlMode  );

extern void EthSM_TrcvLinkStateChg ( uint8 CtrlIdx,
                                     EthTrcv_LinkStateType TransceiverLinkState);

extern Std_ReturnType EthSM_TcpIpModeIndication ( uint8 CtrlIdx,
                                                  TcpIp_StateType TcpIpState);

#define ETHSM_STOP_SEC_CODE
#include "EthSM_MemMap.h"

#endif /* ETHSM_CONFIGURED */

#endif /* ETHSM_CBK_H_ */
