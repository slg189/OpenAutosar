

#ifndef SOAD_CBK_H
#define SOAD_CBK_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

#include "ComStack_Types.h"
#include "SoAd.h"

/**************************************************************************************************/
/*  Common Published Information                                                                  */
/**************************************************************************************************/

/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************/

/****************************************************************************************************************
**                                   Global Callback Function Prototypes                                       **
****************************************************************************************************************/
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

extern void              SoAd_RxIndication                   ( TcpIp_SocketIdType         SocketId, \
                                                               const TcpIp_SockAddrType * RemoteAddrPtr, \
                                                               uint8 *                    BufPtr, \
                                                               uint16                     Length );

extern BufReq_ReturnType SoAd_CopyTxDataMetaData             ( TcpIp_SocketIdType         SocketId, \
                                                                uint8 *                   BufPtr, \
                                                                uint16                    BufLength, \
                                                                const void *             CopyTxMetaData_cpst );

extern BufReq_ReturnType SoAd_CopyTxData                     ( TcpIp_SocketIdType         SocketId , \
                                                               uint8 *                    BufPtr, \
                                                               uint16                     BufLength);

extern void              SoAd_TxConfirmation                 ( TcpIp_SocketIdType         SocketId, \
                                                               uint16                     Length );

extern Std_ReturnType    SoAd_TcpAccepted                    ( TcpIp_SocketIdType         SocketId, \
                                                               TcpIp_SocketIdType         SocketIdConnected, \
                                                               const TcpIp_SockAddrType * RemoteAddrPtr );

extern void              SoAd_TcpConnected                   ( TcpIp_SocketIdType         SocketId);

extern void              SoAd_TcpIpEvent                     ( TcpIp_SocketIdType         SocketId, \
                                                               TcpIp_EventType            Event );

extern void              SoAd_LocalIpAddrAssignmentChg       ( TcpIp_LocalAddrIdType      IpAddrId, \
                                                               TcpIp_IpAddrStateType      State );

extern void              SoAd_ZeroCopyTxConfirmation         ( TcpIp_SocketIdType         SocketId_u16, \
                                                               uint32                     TxId_u32 );

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif
