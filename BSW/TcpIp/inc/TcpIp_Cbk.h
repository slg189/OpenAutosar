

#ifndef TCPIP_CBK_H
#define TCPIP_CBK_H

#include "TcpIp_Cfg.h"      /* Required for TCPIP_CONFIGURED */
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Types.h"

#include "EthIf.h"          /* Required for Eth_FrameType */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern void TcpIp_RxIndication( uint8 CtrlIdx,
                                Eth_FrameType FrameType,
                                boolean IsBroadcast,
                                uint8 * PhysAddrPtr,
                                uint8 * DataPtr,
                                uint16 LenByte );

extern void TcpIp_LocalIpAddrAssignmentChg( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                            TcpIp_IpAddrStateType State_en,
                                            const TcpIp_IPAddrParamType_tun * IPAddrParam_pcun,
                                            TcpIp_IpAddrAssignmentType IPAddrAssignMethod_en,
                                            const TcpIp_DomainType IpDomainType_cu32);

/* Callback function to inform TcpIp about the Duplicate Adrress Detection Status  */
extern void TcpIp_DuplicateAddressIndication( TcpIp_LocalAddrIdType                 LocalAddrId_u8,
                                              TcpIp_DadStateType_ten                DadState_en,
                                              const TcpIp_IPAddrParamType_tun *     IPAddrParam_pcun,
                                              const uint8 *                         RemotePhysAddrPtr_pcu8);
#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#define TCPIP_START_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
extern void TcpIp_ZeroCopyTxConfirmation( uint8 EthIfCtrlIdx_u8,
                                          uint32 TxId_u32 );
#endif

#define TCPIP_STOP_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

#endif /* TCPIP_CBK_H */
