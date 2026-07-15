

#ifndef TCPIP_ETHSM_H_
#define TCPIP_ETHSM_H_

#include "TcpIp_Cfg.h"      /* Required for TCPIP_CONFIGURED */
#ifdef TCPIP_CONFIGURED

#include "TcpIp_GeneralTypes.h"

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern Std_ReturnType TcpIp_RequestComMode( uint8 CtrlIdx,
                                            TcpIp_StateType State );

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
#endif /* TCPIP_ETHSM_H_ */
