

#ifndef SOAD_H
#define SOAD_H


/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "ComStack_Types.h"
#include "SoAd_Types.h"
#include "TcpIp_Types.h"
#include "SoAd_Cfg.h"

/**************************************************************************************************/
/*  Common Published Information                                                                  */
/**************************************************************************************************/

/* Vendor ID */
#define SOAD_VENDOR_ID  6u

/* Module ID */
#define SOAD_MODULE_ID                      56u

/* As of now, SoAd is supported only single instance, so SoAd Instance Id will be '0' in all the scenario */
#define SOAD_INSTANCE_ID                    0u

#if (SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON)

#define SOAD_MEAS_DROP_TCP                  0x01U
#define SOAD_MEAS_DROP_UDP                  0x02U
#define SOAD_MEAS_ALL                       0xFFU

typedef uint8 SoAd_MeasurementIdxType;

#endif /* (SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON) */


/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/****************************************************************************************************************
**                                   Global Variable Declaration                                                **
****************************************************************************************************************/

#define SOAD_START_SEC_CONST_UNSPECIFIED
#include "SoAd_MemMap.h"
extern const SoAd_ConfigType SoAd_ConfigSet[];
#define SOAD_STOP_SEC_CONST_UNSPECIFIED
#include "SoAd_MemMap.h"

/****************************************************************************************************************
**                                   Global Function Prototypes                                                **
****************************************************************************************************************/
#define SOAD_START_SEC_CODE_SLOW
#include "SoAd_MemMap.h"
extern void           SoAd_Init                           ( const SoAd_ConfigType * SoAdConfigPtr ) ;

#define SOAD_STOP_SEC_CODE_SLOW
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"
#if (SOAD_VERSION_INFO_API == STD_ON)
extern void           SoAd_GetVersionInfo                 ( Std_VersionInfoType *   versioninfo );
#endif

#if( SOAD_ECUC_RB_RTE_IN_USE == STD_OFF)
extern void           SoAd_MainFunction                   ( void );
#endif

extern Std_ReturnType SoAd_OpenSoCon                      ( SoAd_SoConIdType           SoConId );

extern Std_ReturnType SoAd_CloseSoCon                     ( SoAd_SoConIdType           SoConId ,\
                                                            boolean                    Abort );

extern Std_ReturnType SoAd_TpCancelTransmit               ( PduIdType                  PduId);

extern Std_ReturnType SoAd_TpCancelReceive                ( PduIdType                  PduId);

extern Std_ReturnType SoAd_GetSoConId                     ( PduIdType                  TxPduId ,\
                                                            SoAd_SoConIdType *         SoConIdPtr );

extern Std_ReturnType SoAd_IfTransmit                     ( PduIdType                  SoAdSrcPduId ,\
                                                            const PduInfoType *        SoAdSrcPduInfoPtr );

extern Std_ReturnType SoAd_IfRoutingGroupTransmit         ( SoAd_RoutingGroupIdType    id );

extern Std_ReturnType SoAd_IfSpecificRoutingGroupTransmit ( SoAd_RoutingGroupIdType    id,   \
                                                            SoAd_SoConIdType           SoConId );

extern Std_ReturnType SoAd_TpTransmit                     ( PduIdType                  SoAdSrcPduId ,\
                                                            const PduInfoType *        SoAdSrcPduInfoPtr );

extern Std_ReturnType SoAd_SetRemoteAddr                  ( SoAd_SoConIdType           SoConId ,\
                                                            const TcpIp_SockAddrType * RemoteAddrPtr );

extern Std_ReturnType SoAd_RequestIpAddrAssignment        ( SoAd_SoConIdType           SoConId , \
                                                            TcpIp_IpAddrAssignmentType Type , \
                                                            const TcpIp_SockAddrType * LocalIpAddrPtr, \
                                                            uint8                      Netmask, \
                                                            const TcpIp_SockAddrType * DefaultRouterPtr);

extern Std_ReturnType SoAd_GetPhysAddr                    ( SoAd_SoConIdType           SoConId ,\
                                                            uint8 *                    PhysAddrPtr );

extern Std_ReturnType SoAd_GetRemoteAddr                  ( SoAd_SoConIdType           SoConId ,\
                                                            TcpIp_SockAddrType *       IpAddrPtr );

extern Std_ReturnType SoAd_GetLocalAddr                   ( SoAd_SoConIdType           SoConId , \
                                                            TcpIp_SockAddrType *       LocalAddrPtr, \
                                                            uint8 *                    NetmaskPtr, \
                                                            TcpIp_SockAddrType *       DefaultRouterPtr);

extern Std_ReturnType SoAd_ReleaseIpAddrAssignment        ( SoAd_SoConIdType           SoConId);

extern Std_ReturnType SoAd_EnableRouting                  ( SoAd_RoutingGroupIdType    id );

extern Std_ReturnType SoAd_DisableRouting                 ( SoAd_RoutingGroupIdType    id );

extern Std_ReturnType SoAd_EnableSpecificRouting          ( SoAd_RoutingGroupIdType    id, \
                                                            SoAd_SoConIdType           SoConId);

extern Std_ReturnType SoAd_DisableSpecificRouting         ( SoAd_RoutingGroupIdType    id, \
                                                            SoAd_SoConIdType           SoConId);

extern Std_ReturnType SoAd_SetUniqueRemoteAddr            ( SoAd_SoConIdType           SoConId, \
                                                            const TcpIp_SockAddrType * RemoteAddrPtr, \
                                                            SoAd_SoConIdType *         AssignedSoConIdPtr);

extern Std_ReturnType SoAd_ChangeParameter                ( SoAd_SoConIdType           SoConId, \
                                                            SoAd_ParamIdType           ParameterId, \
                                                            uint8 *                    ParameterValuePtr);

extern Std_ReturnType SoAd_ReadDhcpHostNameOption         ( SoAd_SoConIdType           SoConId, \
                                                            uint8 *                    length, \
                                                            uint8 *                    data );

extern Std_ReturnType SoAd_WriteDhcpHostNameOption        ( SoAd_SoConIdType           SoConId, \
                                                            uint8                      length, \
                                                            const uint8 *              data );

extern void SoAd_ReleaseRemoteAddr                        ( SoAd_SoConIdType           SoConId);


extern Std_ReturnType SoAd_IsRemoteAddrUnLock( SoAd_SoConIdType SoConId_uo,   \
                                               SoAd_SoConIdType IdxDynSocket_uo);

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
extern Std_ReturnType SoAd_GetAndResetMeasurementData    (SoAd_MeasurementIdxType      MeasurementIdx, \
                                                          boolean                      MeasurementResetNeeded, \
                                                          uint32 *                     MeasurementDataPtr);
#endif /* (SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON ) */

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
extern Std_ReturnType SoAd_ZeroCopyIfTransmit         ( PduIdType                          SoAdSrcPduId , \
                                                        const Eth_ZeroCopyTxDataInfo_tst * ZeroCopyData_pcst,\
                                                        boolean                            TxConfFlag_b,
                                                        uint32 *                           TxId_pu32 );

#endif/* ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)*/

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

/*** BSD Socket Interfaces supported by SoAd ****/


/****************************************************************************************************************
**                                   External Variables Declarations                                           **
****************************************************************************************************************/

#endif
