

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED
#include "SoAd_Cbk.h"

#include "TcpIp_Lib.h"

#include "SoAd_Prv_Domain.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

#if( SOAD_ECUC_RB_RTE_IN_USE == STD_ON)
#include "SchM_SoAd.h"
#endif

#ifdef SOAD_DEBUG_AND_TEST
#include "TestSoAdApplication.h"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
static void           SoAd_InitializenPduUdpPduIdsBuffer              ( const SoAd_ConfigType * SoAdConfig_cpst);
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

static void           SoAd_ResetDynSockTables                         ( const SoAd_SoConIdType NrOfActiveSockets_cuo,
                                                                        const SoAd_SoConIdType NrOfActiveInetSockets_cuo );

static void           SoAd_ResetDynSockRemoteAddrTables               ( const SoAd_SoConIdType NrOfActiveSockets_cuo,
                                                                        const SoAd_SoConIdType NrOfActiveInetSockets_cuo );

#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
static void           SoAd_ResetDynRxBuffTables                       ( const SoAd_ConfigType * SoAdConfigPtr );
#endif

static void           SoAd_ResetPdudynInfo                            ( const SoAd_ConfigType * SoAdConfigPtr );

static Std_ReturnType SoAd_TriggerAutoManualOpenSoCon                 ( SoAd_SoConIdType StaticSocketId_uo);

static void           SoAd_ProcessCloseReq                            ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_PerformCloseActionSeq                      ( SoAd_SoConIdType   StaticSocketId_uo, \
                                                                        SoAd_SoConModeType SoConMode_en);

static void           SoAd_ProcessOpenReq                             ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_PerformOpenSeq                             ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_SocketOpenActions                          ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_TcpIpChangeParameter                       ( SoAd_SoConIdType StaticSocketId_uo,
                                                                        SoAd_SoConIdType SoConGroupId_uo);

static void           SoAd_ActivateTcpUdpSockets                      ( SoAd_SoConIdType StaticSocketId_uo );

static Std_ReturnType SoAd_CallTcpIpClose                             ( SoAd_SoConIdType StaticSocketId_uo );

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
static void           SoAd_ResetRoutingGroupStatus                    ( const SoAd_ConfigType * SoAdConfig_cpst );
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF)

static Std_ReturnType SoAd_UpdateTcpServTblIfTcpListenSocketPresent   ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_ActivateTcpServerSocket                    ( SoAd_SoConIdType StaticSocketId_uo );

static Std_ReturnType SoAd_TcpServerSocketTcpIpClose                  ( SoAd_SoConIdType StaticSocketId_uo );

#endif /* SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF */

#if (SOAD_UDP_GRP_PRESENT != STD_OFF)
static Std_ReturnType SoAd_UpdateUdpGrpTblIfUdpSocketPresent          ( SoAd_SoConIdType StaticSocketId_uo );

static Std_ReturnType SoAd_UdpGroupSocketTcpIpClose                   ( SoAd_SoConIdType StaticSocketId_uo );

#endif /* SOAD_UDP_GRP_PRESENT != STD_OFF */

#if (SOAD_TCP_CLIENT_PRESENT != STD_OFF)
static void           SoAd_ActivateTcpClientSocket                    ( SoAd_SoConIdType StaticSocketId_uo );
#endif /* SOAD_TCP_CLIENT_PRESENT != STD_OFF */

#if (SOAD_UDP_PRESENT != STD_OFF)
static void           SoAd_ActivateUdpSocket                          ( SoAd_SoConIdType StaticSocketId_uo );
#endif  /*SOAD_UDP_PRESENT != STD_OFF*/


#if (SOAD_TP_PRESENT != STD_OFF)
static void           SoAd_ProcessTpReq                               ( SoAd_SoConIdType StaticSocketId_uo );

static void           SoAd_ProcessTransmissionReq                     ( SoAd_SoConIdType StaticSocketId_uo );

#if ( SOAD_TP_TCP_PRESENT != STD_OFF )
static void           SoAd_TpTcpTransmit                              ( SoAd_SoConIdType StaticSocketId_uo, \
                                                                        PduLengthType    AvailableLength_uo, \
                                                                        boolean *        TpTransmitError_pb );
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

#if ( SOAD_TP_UDP_PRESENT != STD_OFF )
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
static Std_ReturnType SoAd_CopyPduDataToTpUdpTransmitBuffer           ( SoAd_SoConIdType StaticSocketId_uo, \
                                                                        PduLengthType    AvailableLength_uo, \
                                                                        uint8            PduHdrSize_u8, \
                                                                        boolean *        TpTransmitError_pb);
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */
static void           SoAd_TpUdpTransmit                              ( SoAd_SoConIdType StaticSocketId_uo, \
                                                                        PduLengthType    AvailableLength_uo, \
                                                                        boolean *        TpTransmitError_pb );
#endif /* SOAD_TP_UDP_PRESENT != STD_OFF */

#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( SOAD_TIMEOUT_HANDLING_ENABLED == STD_ON )
static void           SoAd_TimeOutParamHandling                       ( SoAd_SoConIdType StaticSocketId_uo );
#endif /*  SOAD_TIMEOUT_HANDLING_ENABLED == STD_ON */

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
static void           SoAd_ProcessIfRoutingGrpTransmissionReq         ( void );

#ifdef SOAD_DEBUG_AND_TEST
static void           SoAd_TriggerIfRgTransmissionToTcpIp             ( const PduRouteDestType_tst * PduRRouteDest_cpst, \
                                                                        PduLengthType                TotalLength_uo, \
                                                                        PduIdType                    TxPduId_uo, \
                                                                        SoAd_SoConIdType             PduRouteDestNum_uo);
#else
static void           SoAd_TriggerIfRgTransmissionToTcpIp             ( const PduRouteDestType_tst * PduRRouteDest_cpst, \
                                                                        PduLengthType                TotalLength_uo );
#endif

static Std_ReturnType SoAd_IfRgTransmissionToSocketRef                ( SoAd_SoConIdType StaticSocketId_uo, \
                                                                        PduLengthType    TotalLength_uo);

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
#ifdef SOAD_DEBUG_AND_TEST
static void SoAd_IfRgTransmissionToAllGroupSocketsInBundRef           ( const PduRouteDestType_tst * PduRRouteDest_cpst, \
                                                                        PduLengthType                TotalLength_uo, \
                                                                        PduIdType                    TxPduId_uo, \
                                                                        SoAd_SoConIdType             PduRouteDestNum_uo);
#else
static void           SoAd_IfRgTransmissionToAllGroupSocketsInBundRef ( const PduRouteDestType_tst * PduRRouteDest_cpst, \
                                                                        PduLengthType                TotalLength_uo );
#endif
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF)
static PduLengthType  SoAd_Minimum                                    ( PduLengthType Param1_uo, \
                                                                        PduLengthType Param2_uo );
#endif /* (SOAD_TP_PRESENT != STD_OFF) */

static void           SoAd_ProcessReconnectReq                        (SoAd_SoConIdType StaticSocketId_uo);

#if (SOAD_UDP_PRESENT != STD_OFF)
static void           SoAd_ProcessTcpIpCloseReq                       (SoAd_SoConIdType StaticSocketId_uo);
#endif

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

/*
 ***************************************************************************************************
 * Prototype for Static functions: End
 ***************************************************************************************************
 */
#define SOAD_START_SEC_VAR_CLEARED_8
#include "SoAd_MemMap.h"

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
/* Static variable to store the Pdu data handling the trigger transmit */
static uint8 SoAd_PduTxBuf_au8[SOAD_MAX_TXPDU_SIZE + SOAD_PDUHDR_SIZE];
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#define SOAD_STOP_SEC_VAR_CLEARED_8
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_VAR_INIT_8
#include "SoAd_MemMap.h"
boolean SoAd_InitFlag_b;

#define SOAD_STOP_SEC_VAR_INIT_8
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_VAR_INIT_32
#include "SoAd_MemMap.h"
const SoAd_ConfigType * SoAd_CurrConfig_cpst;
#define SOAD_STOP_SEC_VAR_INIT_32
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_CODE_SLOW
#include "SoAd_MemMap.h"
/**
 ***************************************************************************************************
 * \Function Name : SoAd_Init()
 *
 * \Function description
 * This service initializes all global variables of a Socket Adaptor instance and puts it into the idle state.
 * It has no return value because software errors in initialization data shall be detected during configuration time
 * (e.g. by configuration tool).
 * Furthermore, if a hardware error occurs it shall be reported via the error manager modules.
 *
 * Parameters (in):
 * \param   SoAd_ConfigType*
 * \arg     SoAdConfigPtr - Pointer to the configuration data of the SoAd module.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
/* SoAd shall use this function to initialize the global variables, but currently SoAd shall directly take the configurations done
 * from SoAd_PbCfg.c file and make use of the configured variables. So, No dynamic initialization, post build support is possible for SoAd.
 * Here, SoAdConfigPtr is unused parameter currently.*/

/****************************************************************************************************************/
/****************************************** [SWS_SoAd_00211] ****************************************************/
/****************************************************************************************************************/
/****************************************************************************************************************/
/** SoAd_Init shall store the access to the configuration structure for subsequent API calls                  ***/
/****************************************************************************************************************/
void SoAd_Init( const SoAd_ConfigType * SoAdConfigPtr )
{
    /* Variable to hold configuration pointer */
    const SoAd_ConfigType *            lConfigPtr_cpst;

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    /* local pointer to hold the IF tcp pdu tx pending status */
    SoAd_IfTcpPduTxPendingStatus_tst * lIfTcpPduTxPendingStatus_pst;
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

    /* Variable to Loop through all the configurations */
    uint16                             lIdxForLoop_u16 ;

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
    /* Loop through all the PDU Route Destinations */
    SoAd_SoConIdType                   lIdxOfPduRouteDest_uo ;
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

    boolean                            lDetErrorFlag_b;

    /* Check whether selected module variant is Post build*/
#if (SOAD_CFG_VARIANT == SOAD_CFG_VARIANT_POSTBUILD)
    lConfigPtr_cpst  = SoAdConfigPtr;
#else
    /* Ignore the passed parameter in case of Pre-compile*/
    (void)SoAdConfigPtr;

    lConfigPtr_cpst  = &SoAd_Config[0];
#endif /* SOAD_CFG_VARIANT == SOAD_CFG_VARIANT_POSTBUILD */

    lDetErrorFlag_b = FALSE;

    if (
#if (SOAD_CFG_VARIANT != SOAD_CFG_VARIANT_POSTBUILD )
        ( NULL_PTR != SoAdConfigPtr )
#else
        ( NULL_PTR == SoAdConfigPtr )
#endif /* SOAD_CFG_VARIANT != SOAD_CFG_VARIANT_POSTBUILD  */

        /* DET Error Reporting */
        /* Check whether SoAd_CurrConfig_cpst pointer is valid or not */
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_TxPduConfig_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_RxPduConfig_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_StaticSocConfig_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_StaticSocGroupConfig_cpst )
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_RoutingGroup_ROM_cpb )
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_RoutingGrTriggerable_ROM_cpb )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_IfTriggerTransmitCallbacks_cpst )
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdIf_UdpTxPduIndex_cpu16 )
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_UdpTransmitBufferInfo_cpu8 )
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdIf_nPduUdpTransmitInfo_pst )
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if (SOAD_IF_PRESENT != STD_OFF )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdIf_TxCallbacks_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdIf_RxCallbacks_cpst )
#endif /* SOAD_IF_PRESENT != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_TxCallbacks_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_RxCallbacks_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_StartOfReceptionCallbacks_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_copyTxDataCallbacks_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAdTp_copyRxDataCallbacks_cpst )
#endif /* SOAD_TP_PRESENT != STD_OFF */

       || ( NULL_PTR == lConfigPtr_cpst->SoAd_LocalIpAddrAsgnChgCbk_cpst )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_SoConModeChgCallbacks_cpst )
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_RoutingGroupSoConStructMap_pst )
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */
       || ( NULL_PTR == lConfigPtr_cpst->SoAd_LocalAddrIdInfo_past )
      )
    {
        lDetErrorFlag_b = TRUE;
    }

    SOAD_DET_CHECK_ERROR_VOID((TRUE == lDetErrorFlag_b), SOAD_INIT_API_ID, SOAD_E_INIT_FAILED )

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00723] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** SoAd_Init() initializes all global variables of a Socket Adaptor instance and puts all socket connections ***/
    /**                 into the state SOAD_SOCON_OFFLINE.                                                       ****/
    /****************************************************************************************************************/
    /* Initialization of local IP address index state to default value */
    for( lIdxForLoop_u16 = 0; lIdxForLoop_u16 < SOAD_MAX_TCPIP_LOCALADDRIDX ; lIdxForLoop_u16++ )
    {
        lConfigPtr_cpst->SoAd_LocalAddrIdInfo_past[lIdxForLoop_u16].localAddrIdx_State_en = TCPIP_IPADDR_STATE_UNASSIGNED;
    }

    /* Initialization of Dynamic socket handle array */
    for( lIdxForLoop_u16 = 0; lIdxForLoop_u16 < SOAD_NUM_TCPIP_SOCKETS ; lIdxForLoop_u16++ )
    {
        SoAd_TcpIpSocHandleToDynSockIdx_auo[lIdxForLoop_u16] = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;
    }

    /* Initialization of SoAd_GroupTcpIpSocketId_auo array */
    for( lIdxForLoop_u16 = 0; lIdxForLoop_u16 < SOAD_CONFIG_NUM_STATIC_SOCKETGRP ; lIdxForLoop_u16++ )
    {
        SoAd_GroupTcpIpSocketId_auo[lIdxForLoop_u16]   = SOAD_CFG_SOCKET_ID_DEFAULT_VALUE ;
    }

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* Initialization of PduId Buffer in nPduBuffer array */
    SoAd_InitializenPduUdpPduIdsBuffer( lConfigPtr_cpst );
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

    /* SOAD_ROUTING_GROUPS_ENABLE switch to activate the code, if there is one routing group in the project configuration atleast */
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
    /* Call the below function to update the Routing Groups status to the data structure associated to socket and Routing Group
     * from the constant array SoAd_RoutingGroup_ROM_cpb[]
     */
    SoAd_ResetRoutingGroupStatus( lConfigPtr_cpst );
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if ( ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF ) || ( SOAD_NUM_TX_IF_UDP_PDU != STD_OFF ) )
    for( lIdxForLoop_u16 = 0; lIdxForLoop_u16 < lConfigPtr_cpst->SoAd_noTxPdus_cuo ; lIdxForLoop_u16++ )
    {
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
        /* Initialization of Trigger Transmit request array to default value */
        for( lIdxOfPduRouteDest_uo = 0; ( lIdxOfPduRouteDest_uo < lConfigPtr_cpst->SoAd_TxPduConfig_cpst[lIdxForLoop_u16].PduRouteDestNum_uo ); lIdxOfPduRouteDest_uo++ )
        {
            if(NULL_PTR != lConfigPtr_cpst->SoAd_TxPduConfig_cpst[lIdxForLoop_u16].PduRouteDestConf_cpst[lIdxOfPduRouteDest_uo].triggerTransmissionInfo_puo)
            {
                (lConfigPtr_cpst->SoAd_TxPduConfig_cpst[lIdxForLoop_u16].PduRouteDestConf_cpst[lIdxOfPduRouteDest_uo].triggerTransmissionInfo_puo->triggerTransmitRequest_u8)   = STD_OFF ;
#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
                (lConfigPtr_cpst->SoAd_TxPduConfig_cpst[lIdxForLoop_u16].PduRouteDestConf_cpst[lIdxOfPduRouteDest_uo].triggerTransmissionInfo_puo->triggerTransmitSocketIdx_uo) = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
            }
        }
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#if ( SOAD_NUM_TX_IF_UDP_PDU != STD_OFF )
        /* Initialization of Upper layer Confirmation array to default value */
        SoAd_ULIfUdpTxConfirmFlag_ab[lIdxForLoop_u16] = FALSE ;
#endif /* SOAD_NUM_TX_IF_UDP_PDU != STD_OFF */

    }
#endif /*  ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF ) || ( SOAD_NUM_TX_IF_UDP_PDU != STD_OFF ) */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    /* Reset the TxConfirmation pending flag during warm boot up */
    for( lIdxForLoop_u16 = 0; lIdxForLoop_u16 < SOAD_NUM_TX_IF_TCP_PDU ; lIdxForLoop_u16++ )
    {
        lIfTcpPduTxPendingStatus_pst = &SoAd_IfTcpPduTxPendingStatus_ast[lIdxForLoop_u16];
        lIfTcpPduTxPendingStatus_pst->ifTcpTxConfRemainingLen_au32  = 0U ;
        lIfTcpPduTxPendingStatus_pst->ifTcpTxConfReceivedStatus_b   = FALSE ;
    }
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

    /* Initialization of Dynamic configuration info with default value during ECU warm boot up */
    SoAd_ResetDynSockTables( lConfigPtr_cpst->SoAd_noActiveSockets_uo, lConfigPtr_cpst->SoAd_noInetActiveSockets_uo );

#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
    /* Reset SoAd Rx buffer tables */
    SoAd_ResetDynRxBuffTables(lConfigPtr_cpst);
#endif

    /* Reset Pdu Dynamic Info*/
    SoAd_ResetPdudynInfo(lConfigPtr_cpst);

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
/* variable to hold dropped udp packets count at SoAd level */
    SoAd_RxUdpDropCnt_u32 = 0U;
/* variable to hold dropped tcp packets count at SoAd level */
    SoAd_RxTcpDropCnt_u32 = 0U;
#endif /* SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON */

    /* Copying the PB configuration pointer to SoAd pointer */
    SoAd_CurrConfig_cpst = lConfigPtr_cpst;

    SoAd_InitFlag_b = TRUE;

#ifdef SOAD_DEBUG_AND_TEST
    SoAd_Init_SIMTest();
#endif

}

#define SOAD_STOP_SEC_CODE_SLOW
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_InitializenPduUdpPduIdsBuffer()
 *
 * \Function description
 * This service shall initialize the nPduUdp PduId Buffer with 0xFF
 *
 * \param   const SoAd_ConfigType*
 * \arg     SoAdConfig_cpst - Pointer to the configuration data of the SoAd module.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_InitializenPduUdpPduIdsBuffer(const SoAd_ConfigType * SoAdConfig_cpst)
{
    /* variable to hold the npdu tx info */
    SoAd_nPduUdpTxInfo_tst *          lnPduUdpTxInfo_pst;

    /* local pointer to hold the nPduUdpTxPduDescriptor info */
    SoAd_nPduUdpTxPduDescriptor_tst * lnPduUdpTxPduDescriptor_pst;

    /* Variable to Loop through all the configurations */
    PduIdType                         lnPduUdpTxTableIndex_uo ;

    /* Loop through for all TxPduIds field to reset it */
    SoAd_SoConIdType                  lIdxnPduIdFieldLoop_u8 ;

    for( lnPduUdpTxTableIndex_uo = 0; lnPduUdpTxTableIndex_uo < SoAdConfig_cpst->SoAdIf_noOfnPduUdpTxBuffers_cuo ; lnPduUdpTxTableIndex_uo++ )
    {
        lnPduUdpTxInfo_pst = &((SoAdConfig_cpst->SoAdIf_nPduUdpTransmitInfo_pst)[lnPduUdpTxTableIndex_uo]);
        lnPduUdpTxInfo_pst->nPduUdpTxTimer_u32 = 0xFFFFFFFFU;
        lnPduUdpTxInfo_pst->nPduUdpTxBuffCopiedLen_u16 = 0U;

        /* loop through for each entry in the mapped Pdu descriptor to reset it */
        for( lIdxnPduIdFieldLoop_u8 = 0; lIdxnPduIdFieldLoop_u8 < lnPduUdpTxInfo_pst->nPduUdpTxPduIdSize_cu8 ; lIdxnPduIdFieldLoop_u8++ )
        {
            lnPduUdpTxPduDescriptor_pst = &(lnPduUdpTxInfo_pst->nPduUdpTxPduDescriptor_cpst[lIdxnPduIdFieldLoop_u8]);
            /* Reset the Pdu descriptor entry with default values */
            lnPduUdpTxPduDescriptor_pst->nPduUdpTxPduId_uo = SOAD_CFG_PDU_ID_DEFAULT_VALUE;
            lnPduUdpTxPduDescriptor_pst->nPduUdpTxPduDataLength_uo = 0U;
            lnPduUdpTxPduDescriptor_pst->nPduUdpTxPduHeaderId_u32 = 0U;
        }
    }
}
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

/**
 *********************************************************************************************************
 * \Function Name : SoAd_ResetDynSockTables()
 *
 * \Function description
 * This service shall reset the configuration parameters SoAd_DyncSocConfig_ast, SoAd_DynSockIndx_auo
 * to default value and it will be called from SoAd Init function during boot up.
 *
 * \param   const SoAd_SoConIdType
 * \arg     NrOfActiveSockets_cuo       - No of active sockets configured.
 * \param   const SoAd_SoConIdType
 * \arg     NrOfActiveInetSockets_cuo   - No of active IPv4 sockets configured.
 *
 * Parameters (inout)   : None
 *
 * Parameters (out)     : None
 *
 * Return value         : None
 *
 ********************************************************************************************************
 */
static void SoAd_ResetDynSockTables( const SoAd_SoConIdType NrOfActiveSockets_cuo,
                                     const SoAd_SoConIdType NrOfActiveInetSockets_cuo )
{
    /* Loop through all Dynamic Socket configurations */
    SoAd_SoConIdType             lIdxDynSocket_uo ;

    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* --------------------------------------------- */
    /* Reset dynamic socket table                    */
    /* --------------------------------------------- */

    /* Initialization of Dynamic Configuration of structure array to default */
    for( lIdxDynSocket_uo = 0; lIdxDynSocket_uo < NrOfActiveSockets_cuo; lIdxDynSocket_uo++ )
    {
        /* copy the global array index address to local pointer for further processing */
        lSoAdDyncSocConfig_pst =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        lSoAdDyncSocConfig_pst->rxPduInfoPtr_cpst               = NULL_PTR ;
        lSoAdDyncSocConfig_pst->txPduInfoPtr_cpst               = NULL_PTR ;
        lSoAdDyncSocConfig_pst->socConMode_en                   = SOAD_SOCON_OFFLINE ;
        lSoAdDyncSocConfig_pst->socConSubMode_en                = SOAD_SOCON_CLOSE ;
        lSoAdDyncSocConfig_pst->socConRequestedMode_en          = SOAD_EVENT_IDLE ;
        lSoAdDyncSocConfig_pst->soConDynLocalPort_u16           = SOAD_DEFAULT_PORT ;
        lSoAdDyncSocConfig_pst->socConOpenReqCnt_u16            = 0;
        lSoAdDyncSocConfig_pst->socConCloseReqCnt_u16           = 0;

#if (SOAD_TP_PRESENT != STD_OFF )
        lSoAdDyncSocConfig_pst->tpTxRemainingLength_uo          = 0 ;
        lSoAdDyncSocConfig_pst->tpTransmittedLength_uo          = 0 ;
        lSoAdDyncSocConfig_pst->tpTcpCopyTxDataLength_uo        = 0 ;
        lSoAdDyncSocConfig_pst->tpTransmissionOnGoing_b         = FALSE;
        lSoAdDyncSocConfig_pst->tpReceptionOngoing_b            = FALSE;
        lSoAdDyncSocConfig_pst->tpTransmitRequest_b             = FALSE;
        lSoAdDyncSocConfig_pst->tpTxTotPduLen_uo                = 0;
#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 )
        lSoAdDyncSocConfig_pst->UdpAliveSupervisionTimer_u32    = 0xFFFFFFFFU;
#endif /* SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 */

        lSoAdDyncSocConfig_pst->txPduHeaderId_u32               = SOAD_DEFAULT_PDU_HDR_ID;

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
        lSoAdDyncSocConfig_pst->ifTxTcpPduLenReqByUL_uo         = 0 ;
        lSoAdDyncSocConfig_pst->ifTxTcpAckedDataLen_uo          = 0;
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

        lSoAdDyncSocConfig_pst->ifTxCopiedLength_uo             = 0 ;

        lSoAdDyncSocConfig_pst->idxDynSocConfigTable_uo         = lIdxDynSocket_uo ;
        lSoAdDyncSocConfig_pst->idxStaticSoc_uo                 = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;

        lSoAdDyncSocConfig_pst->idxTxPduTable_uo                = SOAD_CFG_PDU_ID_DEFAULT_VALUE ;
        lSoAdDyncSocConfig_pst->idxRxPduTable_uo                = SOAD_CFG_PDU_ID_DEFAULT_VALUE;

        lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo            = SOAD_CFG_SOCKET_ID_DEFAULT_VALUE ;

#if (SOAD_TCP_PRESENT != STD_OFF)
        lSoAdDyncSocConfig_pst->rxTcpRemainingPduLengthToDicard_u32 = 0U;
#endif

#if (SOAD_IF_PRESENT != STD_OFF )
        lSoAdDyncSocConfig_pst->ifTransmissionOnGoing_b         = FALSE ;
#endif /* SOAD_IF_PRESENT != STD_OFF */

        lSoAdDyncSocConfig_pst->rxOngoingCnt_u8                 = 0 ;

#if (SOAD_TP_TCP_PRESENT != STD_OFF)
        lSoAdDyncSocConfig_pst->tpTxTcpTransmitRetryCnt_u16     = 0 ;
        lSoAdDyncSocConfig_pst->tpTxTcpBufferAvailable_b        = TRUE ;
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

#if (SOAD_TP_UDP_PRESENT != STD_OFF)
        lSoAdDyncSocConfig_pst->tpUdpTxTriggered_b              = FALSE;
#endif /* SOAD_TP_UDP_PRESENT != STD_OFF */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
        lSoAdDyncSocConfig_pst->nPduUdpTxTriggered_b            = FALSE;
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if ( SOAD_IF_FANOUT_ENABLED != STD_OFF )
        lSoAdDyncSocConfig_pst->ifSkipFanOutTransmission_b      = FALSE ;
#endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

        lSoAdDyncSocConfig_pst->statusAbort_b                   = FALSE ;
        lSoAdDyncSocConfig_pst->discardNewTxRxReq_b             = FALSE ;
        lSoAdDyncSocConfig_pst->ifTpTxSoAdHdrCopied_b           = FALSE ;
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
        lSoAdDyncSocConfig_pst->ifRgTxOngoing_b                 = FALSE ;
        lSoAdDyncSocConfig_pst->ifRgTxPending_b                 = FALSE ;
#endif

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
        lSoAdDyncSocConfig_pst->ZeroCopyConnChanged_b           = FALSE;
#endif /*SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF*/
        lSoAdDyncSocConfig_pst->PostponeReleaseRemoteAddr_b     =  FALSE ;
        /* Initialization of Dynamic socket index array */
        SoAd_DynSockIndx_auo[lIdxDynSocket_uo] = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;
    }

    /* Because the index to any of the static socket tables containing remote address info is not valid,
     * reset all entries in both the IPv4 and the IPv6 specific tables. */
    SoAd_ResetDynSockRemoteAddrTables(NrOfActiveSockets_cuo, NrOfActiveInetSockets_cuo);
}

/**
 *********************************************************************************************************
 * \Function Name : SoAd_ResetDynSockRemoteAddrTables()
 *
 * \Function description
 * This service shall reset the remote IP address and remote port values in the dynamic socket connection
 * tables for both IPv4 (SoAd_DynSoConRemoteAddrInet_pst) and IPv6 (SoAd_DynSoConRemoteAddrInet6_pst) domain.
 * Both tables must be reset independently from the idxDynSoConRemoteAddr_uo field in the SoAd_DyncSocConfig_ast
 * table entry because, at the moment of SoAd_Init, although the idxDynSoConRemoteAddr_uo is configured correctly
 * all the time, the static socket index (the idxStaticSoc_uo field) is set to a default value untill the socket
 * is actually initialized. Because the idxStaticSoc_uo field is the only way to retrieve the domain (IPv4/IPv6)
 * for both the static socket and the dynamic socket, there is no way to connect these structures at the time of init.
 * Therefore, the safest way is to reset the entire tables.
 *
 * Parameters (in) :
 * \param   const SoAd_SoConIdType
 * \arg     NrOfActiveSockets_cuo       - No of active sockets configured.
 * \param   const SoAd_SoConIdType
 * \arg     NrOfActiveInetSockets_cuo   - No of active IPv4 sockets configured.
 *
 * Parameters (inout)   : None
 *
 * Parameters (out)     : None
 *
 * Return value         : None
 *
 ********************************************************************************************************
 */
static void SoAd_ResetDynSockRemoteAddrTables( const SoAd_SoConIdType NrOfActiveSockets_cuo,
                                               const SoAd_SoConIdType NrOfActiveInetSockets_cuo )
{
    uint32 lSocketLoopIdx_u32;
    uint32 lAddrLoopIdx_u32;
    const SoAd_SoConIdType lNrOfActiveInet6Sockets_cuo = NrOfActiveSockets_cuo - NrOfActiveInetSockets_cuo;

    /* Reset all the entries in the SoAd_DynSoConRemoteAddrInet_pst table */
    for( lSocketLoopIdx_u32 = 0; lSocketLoopIdx_u32 < NrOfActiveInetSockets_cuo; lSocketLoopIdx_u32++ )
    {
        SoAd_DynSoConRemoteAddrInet_pst[lSocketLoopIdx_u32].addr_u32 = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet_pst[lSocketLoopIdx_u32].port_u16 = SOAD_DEFAULT_PORT;
    }

    /* Reset all the entries in the SoAd_DynSoConRemoteAddrInet6_pst table */
    for( lSocketLoopIdx_u32 = 0; lSocketLoopIdx_u32 < lNrOfActiveInet6Sockets_cuo; lSocketLoopIdx_u32++ )
    {
        for( lAddrLoopIdx_u32 = 0; lAddrLoopIdx_u32 < 4u; lAddrLoopIdx_u32++ )
        {
            SoAd_DynSoConRemoteAddrInet6_pst[lSocketLoopIdx_u32].addr_au32[lAddrLoopIdx_u32] = SOAD_DEFAULT_IP;
        }

        SoAd_DynSoConRemoteAddrInet6_pst[lSocketLoopIdx_u32].port_u16 = SOAD_DEFAULT_PORT;
    }
}

/**
 *********************************************************************************************************
 * \Function Name : SoAd_ResetDynRxBuffTables()
 *
 * \Function description
 * This service shall reset the Rx buffer tables to default values
 *
 * Parameters (in)      :
 * \param   SoAd_ConfigType*
 * \arg     SoAdConfigPtr - Pointer to the configuration data of the SoAd module.
 *
 * Parameters (inout)   : None
 *
 * Parameters (out)     : None
 *
 * Return value         : None
 *
 ********************************************************************************************************
 */
#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
static void SoAd_ResetDynRxBuffTables( const SoAd_ConfigType * SoAdConfigPtr )
{
    /* Variable to Loop through all the configurations */
    SoAd_SoConIdType CurrentStaticSocketId_uo;

    /* --------------------------------------------- */
    /* Reset Rx buffer dynamic table                 */
    /* --------------------------------------------- */

    /* Reset the dynamic Rx buffer table for each static socket that has a Rx buffer configured */
    for ( CurrentStaticSocketId_uo=0; CurrentStaticSocketId_uo < SoAdConfigPtr->SoAd_noStaticSockets_uo; CurrentStaticSocketId_uo++ )
    {
        if( SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst != NULL_PTR )
        {
            /* Buffer pointer */
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->RxNxtFree_pu8 = (uint8*)(SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->RxBuffStrt_cpu8);
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->RxNxtData_pu8 = (uint8*)(SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->RxBuffStrt_cpu8);

            /* Rx Pdu desc */
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->DataFragCurrentRxPduDesc_st.PduDataLength_u32 = 0U;
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->DataFragCurrentRxPduDesc_st.RxPduId_uo = 0U;
#if (SOAD_TP_PRESENT != STD_OFF)
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->TpDataRemainingLenToTransfer_u32 = 0U;
#endif

            /* Current size */
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->DataFragCurrentSize_u16 = 0U;
#if (SOAD_TP_PRESENT != STD_OFF)
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->TpDataCurrentSize_u16 = 0U;
#endif
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->HeaderFragCurrentSize_u8 = 0U;

            /* Flags */
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->DataFragReceptionInProgress_b = FALSE;
            SoAdConfigPtr->SoAd_StaticSocConfig_cpst[CurrentStaticSocketId_uo].dynRxBuffTableEntry_pst->HeaderFragReceptionInProgress_b = FALSE;
        }
    }

    return;
}
#endif

/**
 *********************************************************************************************************
 * \Function Name : SoAd_ResetPdudynInfo()
 *
 * \Function description
 * This service shall reset the Tx and Rx Pdu dynamic info
 *
 * Parameters (in)      :
 * \param   SoAd_ConfigType*
 * \arg     SoAdConfigPtr - Pointer to the configuration data of the SoAd module.
 *
 * Parameters (inout)   : None
 *
 * Parameters (out)     : None
 *
 * Return value         : None
 *
 ********************************************************************************************************
 */
static void SoAd_ResetPdudynInfo( const SoAd_ConfigType * SoAdConfigPtr )
{

    PduIdType lPduLoopIdx_uo;

    /* Reset the dynamic Tx Pdu table info for each Tx pdu configured */
    for ( lPduLoopIdx_uo = 0; lPduLoopIdx_uo < SOAD_CONFIG_NUM_TXPDU; lPduLoopIdx_uo++ )
    {
#if(SOAD_IF_PRESENT != STD_OFF)
        SoAdConfigPtr->SoAd_TxPduConfig_cpst[lPduLoopIdx_uo].txPduDyninfo_pst->isIfTxCalled_b       = FALSE;
#endif
#if (SOAD_TP_PRESENT != STD_OFF)
        SoAdConfigPtr->SoAd_TxPduConfig_cpst[lPduLoopIdx_uo].txPduDyninfo_pst->isTpTxCalled_b       = FALSE;
        SoAdConfigPtr->SoAd_TxPduConfig_cpst[lPduLoopIdx_uo].txPduDyninfo_pst->isCancelTpTxCalled_b = FALSE;
#endif
    }

#if (SOAD_TP_PRESENT != STD_OFF)
    /* Reset the dynamic Rx Pdu table info for each Rx pdu configured */
    for ( lPduLoopIdx_uo=0; lPduLoopIdx_uo < SOAD_CONFIG_NUM_RXPDU; lPduLoopIdx_uo++ )
    {
        SoAdConfigPtr->SoAd_RxPduConfig_cpst[lPduLoopIdx_uo].rxPduDynInfo_pst->isCancelTpRxCalled_b = FALSE;
    }
#endif
}


/****************************************************************************************************
 * scheduled functions
 ****************************************************************************************************/
/**
 ***************************************************************************************************
 * \Function Name : SoAd_MainFunction()
 *
 * \Function description
 * This API Schedules the Socket Adaptor. (Entry point for scheduling).
 *
 * \param   : None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
/* SoAd Main function period shall be configurable, but current software is not using the configurable parameter for scheduling MainFunction
 * Main function is responsible for making sockets OFFLINE, RECONNECT, ONLINE, TP Transmissions and TriggerTransmit actions
 * In future Main function shall support, Pending receptions also, if upper layer not having enough buffer to store data in SoAd_RxIndication() */

void SoAd_MainFunction( void )
{
    /* Variable to hold the return value  */
    Std_ReturnType            lRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType          lIdxDynSocket_uo;

    /* Index for looping across active sockets*/
    SoAd_SoConIdType          lIdxStaticSocket_uo ;

    /* Index of TxPduConfig table */
    PduIdType                 lIdxTxPdu_uo;

    /* Tx Pdu Id to which TxConfirmation will be given */
    PduIdType                 lTxPduId_uo;

    /* As per Autosar [SWS_SoAd_00283] requirement */
    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_VOID( ( FALSE == SoAd_InitFlag_b ), SOAD_MAIN_FUNCTION_API_ID, SOAD_E_NOTINIT )

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
    /* --------------------------------------------- */
    /* Tx Confirmation to UL                         */
    /* --------------------------------------------- */

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00544]*****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** In case of a UDP socket connection the SoAd shall call the upper layer with the configured transmit       ***/
    /** confirmation function (<Up>_[SoAd][If]TxConfirmation>()) within the next SoAd_MainFunction() after the    ***/
    /**             latest TcpIp_UdpTransmit() call returning successfully                                        ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00647]*****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** In case of multiple socket connections, SoAd shall call the upper layer with the configured transmit      ***/
    /** confirmation function (<Up>_[SoAd][If]TxConfirmation>()) only once after transmission on all related      ***/
    /**             socket connections succeded                                                                   ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /* As per SWS_SoAd_00544 & SWS_SoAd_00647 SoAd IF TxConfirmation will be transmitted only once in Main function */
    /*Loop for All Tx pdus which are of IF type and are linked to UDP sockets*/
    for(lIdxTxPdu_uo = 0; lIdxTxPdu_uo < SoAd_CurrConfig_cpst ->SoAdIf_noUdpTxPdus_cuo; lIdxTxPdu_uo++)
    {
        /* The exclusive area SoAd_ULIfUdpTxConfirm is used to avoid race condition when SoAd_ULIfUdpTxConfirm flag is modified */
        SchM_Enter_SoAd_ULIfUdpTxConfirm();

        /*Check for the TxConfirm flag for the Pdu*/
        if( FALSE != SoAd_ULIfUdpTxConfirmFlag_ab[SoAd_CurrConfig_cpst ->SoAdIf_UdpTxPduIndex_cpu16[lIdxTxPdu_uo]] )
        {
            lTxPduId_uo = SoAd_CurrConfig_cpst ->SoAdIf_UdpTxPduIndex_cpu16[lIdxTxPdu_uo];

            /*clear the TxConfirm flag. Next SoAd_IfTransmit request can be taken.*/
            SoAd_ULIfUdpTxConfirmFlag_ab[SoAd_CurrConfig_cpst ->SoAdIf_UdpTxPduIndex_cpu16[lIdxTxPdu_uo]] = FALSE;

            SchM_Exit_SoAd_ULIfUdpTxConfirm();

            /* Call back the upper layer with txPduId to inform about IfTxConfirmation  */
            SoAd_CurrConfig_cpst->SoAdIf_TxCallbacks_cpst[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lTxPduId_uo].idxIfTxConfirm_u8].TxCallback_pfn(SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lTxPduId_uo].ulTxPduId_uo);

        }
        else
        {
            SchM_Exit_SoAd_ULIfUdpTxConfirm();
        }

    }

    /********************************************************************************************************/
    /*****************************  End: process of TxConfirmation for IF UDP PduRoutes *********************/
    /********************************************************************************************************/
#endif

    /* loop through all the static sockets for the processing */
    for ( lIdxStaticSocket_uo=0; ( lIdxStaticSocket_uo < SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo ); lIdxStaticSocket_uo++ )
    {
        /********************************************************************************************************/
        /**********************Start: Tigger Open request for automatic and Manual sockets***********************/
        /********************************************************************************************************/

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00589]: Step (2)*******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /***Open is either (a) explicitly requested by a previous SoAd_OpenSoCon() call which has not been revoked by ***/
        /***a following SoAd_CloseSoCon() call or (b) implicitly requested when SoAdSocketAutomaticSoConSetup is TRUE ***/
        /****************************************************************************************************************/

        lRetVal_u8 = SoAd_TriggerAutoManualOpenSoCon(lIdxStaticSocket_uo);

        /********************************************************************************************************/
        /************************End: Tigger Open request for automatic and Manual sockets***********************/
        /********************************************************************************************************/

        if( E_OK == lRetVal_u8 )
        {
            /* Get the index of the dynamic soc table */
            lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo);

            switch( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en )
            {
                /********************************************************************************************************/
                /********************Start: process Closing of a socket connection req by UL ****************************/
                /********************************************************************************************************/
                case SOAD_EVENT_OFFLINEREQ_UL:
                {
                    /* Perform the socket Close request requested by UL/SoAd */
                    SoAd_ProcessCloseReq(lIdxStaticSocket_uo);
                    break;
                }

                /********************************************************************************************************/
                /********************* End: process Closing of a socket connection req by UL ****************************/
                /********************************************************************************************************/

                /********************************************************************************************************/
                /********************Start: process Closing of a socket connection req by SoAd **************************/
                /********************************************************************************************************/
                case SOAD_EVENT_OFFLINEREQ_SOAD:
                {
                    SoAd_PerformCloseActionSeq(lIdxStaticSocket_uo, SOAD_SOCON_RECONNECT);
                    break;
                }

                /********************************************************************************************************/
                /********************* End: process Closing of a socket connection req by SoAd***************************/
                /********************************************************************************************************/

                /********************************************************************************************************/
                /*****************************  Start: process opening of a socket connection   **************************/
                /********************************************************************************************************/
                case SOAD_EVENT_ONLINEREQ:
                {
                    /* Perform the socket Open request requested by UL/SoAd */
                    SoAd_ProcessOpenReq(lIdxStaticSocket_uo);
                    break;
                }

                case SOAD_EVENT_RECONNECTREQ_SOAD:
                {
                    /* Perform Reconnect request requested by SoAd*/
                    SoAd_ProcessReconnectReq(lIdxStaticSocket_uo);
                    break;
                }

#if (SOAD_UDP_PRESENT != STD_OFF)
                case SOAD_EVENT_OFFLINEREQ_TCPIP:
                {
                    /* Perform TcpIp event */
                    SoAd_ProcessTcpIpCloseReq(lIdxStaticSocket_uo);
                    break;
                }
#endif

                /********************************************************************************************************/
                /*****************************  End: process opening of a socket connection **************************/
                /********************************************************************************************************/

                default:
                {
                    /* Nothing to do */
                    break;
                }
            }

            /* If socket is closed previously, then dynamic index mapped to static socket can be invalid, so fetch the dynamic
             * socket index from static socket once again before processing it for TP transmission.
             */
            /* Get the index of the dynamic soc table */
            lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo);

            if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
            {
#if (SOAD_TP_PRESENT != STD_OFF)
                /* --------------------------------------------- */
                /* Execute TP processing                         */
                /* --------------------------------------------- */
                SoAd_ProcessTpReq( lIdxStaticSocket_uo );
#endif


#if (SOAD_TIMEOUT_HANDLING_ENABLED   == STD_ON)
                /* --------------------------------------------- */
                /* Handle timers                                 */
                /* --------------------------------------------- */

                SoAd_TimeOutParamHandling( lIdxStaticSocket_uo );

#endif /*End of (SOAD_TIMEOUT_HANDLING_ENABLED   == STD_ON ) */
            }
            if(TRUE == (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].PostponeReleaseRemoteAddr_b))
            {
                if ( E_OK == SoAd_IsRemoteAddrUnLock(lIdxStaticSocket_uo, lIdxDynSocket_uo))
                {
                    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
                    SchM_Enter_SoAd_RemoteAddrUpdate();

                    /* copy remote address from static config to dynamic config table*/
                    SoAd_CopyRemoteAddrFromStaticSocToDynSoc( ((const SoAd_StaticSocConfigType_tst *) \
                            &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo])), \
                            ((SoAd_DyncSocConfigType_tst *) &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])));

                    SchM_Exit_SoAd_RemoteAddrUpdate();

                    (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].PostponeReleaseRemoteAddr_b) = FALSE;
                }
            }
        }
    }/*End of the For loop with all sockets*/

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
    /* --------------------------------------------- */
    /* Execute IF Routing Group Transmit processing  */
    /* --------------------------------------------- */
    SoAd_ProcessIfRoutingGrpTransmissionReq();
#endif




}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_TriggerAutoManualOpenSoCon()
 *
 * \Function description
 * This service shall provide the TxConfirmation to UL for each IF UDP Pdu's which are transmitted via SoAd_IfTransmit() api successfully,
 * it will be called from Main function.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK               - Open Request has been triggered successfully
 *           E_NOT_OK           - Open Request not triggered
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_TriggerAutoManualOpenSoCon(SoAd_SoConIdType StaticSocketId_uo)
{
    /* variable to store the return value of the function */
    Std_ReturnType               lFunctionRetVal_en;

    /* Variable to store the submode */
    SoAd_SoConSubMode_ten        lSoConSubMode_en;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /*Initialization of local variables */
    lFunctionRetVal_en      = E_NOT_OK;

    /* Get the index of related socket connection group */
    lIdxSoConGroup_uo        = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo         = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Enter if only the SocConnection set is Automatic (not for Manual)
     * Opening of the Automatic sockets shall be handled in first MainFunction
     * just after SoAd_Init is called. Here, a link shall be created between static socket table and dynamic socket table */
    if( TRUE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpAutoConSetup_b )
    {
        /* Check Dynamic socket id is reserved for the static socket id, If not assume that it is called from first main function,
         * so reserve the new dynamic socket index */
        if( lIdxDynSocket_uo >= SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo )
        {
            /* Get the available dynamic socket from the list by calling a function */
            SoAd_ReserveFreeDynSocket(StaticSocketId_uo, &lIdxDynSocket_uo);

            /* check for dynamic socket is valid */
            if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
            {

               /*set the event to onlinereq such that the actions shall be taken to open the socket in next steps */
               SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en = SOAD_EVENT_ONLINEREQ;

               /* set the return value to E_OK */
               lFunctionRetVal_en = E_OK;
            }
        }
        else
        {
            /* set the return value to E_OK, if dynamic socket is already present(not a first main function) */
            lFunctionRetVal_en = E_OK;
        }

    }/* End : if(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].soConGrpAutoConSetup_b == TRUE ) */
    else
    {
        /********************************************************************************************************/
        /*****************************End: Open request setting for Manual sockets**************************/
        /********************************************************************************************************/

        /* Condition check: the lIdxDynSocket_uo should be valid */
        if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
        {
            /* load the address of dynamic socket index to local pointer for further processing */
            lSoAdDyncSocConfig_pst = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

            lSoConSubMode_en = lSoAdDyncSocConfig_pst->socConSubMode_en;

            /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
             SchM_Enter_SoAd_OpenCloseReq();

            /* If the socket is not yet opened, then evaluate the number of open requests to know if the socket need to go ONLINE */
            if( (lSoConSubMode_en == SOAD_SOCON_CLOSE)  && (lSoAdDyncSocConfig_pst->socConOpenReqCnt_u16 > lSoAdDyncSocConfig_pst->socConCloseReqCnt_u16) )
            {
                /* Set the event to onlinereq such that the actions shall be taken to open the socket in next steps */
                lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_ONLINEREQ;
            }

            /* If the socket is already opened, then evaluate the number of close requests to know if the socket need to go OFFLINE */
            else if( (lSoAdDyncSocConfig_pst->socConCloseReqCnt_u16 > 0)       &&
                     (lSoAdDyncSocConfig_pst->socConCloseReqCnt_u16 >= lSoAdDyncSocConfig_pst->socConOpenReqCnt_u16) )
            {
                /*set the event to offlinereq such that the actions shall be taken to close the socket in next steps */
                lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_OFFLINEREQ_UL;

                /* Reset the both of the counters if close request has been accepted successfully */
                /* (open and close requests counters are not used anymore as the socket now enter in closing procedure) */
                /* (this step is required to ensure that further calls of Tx functions will be rejected) */
                lSoAdDyncSocConfig_pst->socConOpenReqCnt_u16  = 0;
                lSoAdDyncSocConfig_pst->socConCloseReqCnt_u16 = 0;

            }

            /* No evaluation in other cases */
            else
            {
                /* Nothing to do */
            }

            SchM_Exit_SoAd_OpenCloseReq();

            /* Set the return value to E_OK if dynamic socket is valid to process the open/close requests or Tp Transmission requests */
            lFunctionRetVal_en = E_OK;
        }
    }

    return lFunctionRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessCloseReq()
 *
 * \Function description
 * This service shall Process the close request triggered by UL or SoAd
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessCloseReq( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Variable to hold the return value */
    Std_ReturnType               lRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /*Initialization of local variables */
    lRetVal_u8              = E_NOT_OK;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo        = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst  = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* enter into switch case depending upon the current state */
    switch(lSoAdDyncSocConfig_pst->socConSubMode_en)
    {
        /* If socket submode is SOAD_SOCON_CLOSE means, there exist a link between static socket table and dynamic socket table.
         * no other links not yet created, socket is just about to start for starting actions, meanwhile closing request has come.
         * So, just reset the link between Static socket and dynamic socket tables, make the submode to SOAD_EVENT_IDLE */

        /* Use case: SoAd_OpenSoCon() and then SoAd_CloseSoCon() immediately */
        case SOAD_SOCON_CLOSE:
        {
            SchM_Enter_SoAd_ReserveFreeDynSoc();
            /* update the "index of dynamic soc config table" in Static soc table */
            ( *SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo) = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

            /* Update the variable: "index of the static soc table" in dynamic socket table */
            lSoAdDyncSocConfig_pst->idxStaticSoc_uo = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
            SchM_Exit_SoAd_ReserveFreeDynSoc();

            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
            SchM_Enter_SoAd_RemoteAddrUpdate();

            /* Reset the remote information in dynamic table */
            SoAd_SetDyncSocConfigRemoteAddrtoDefault(lSoAdDyncSocConfig_pst);

            SchM_Exit_SoAd_RemoteAddrUpdate();

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
            lSoAdDyncSocConfig_pst->ZeroCopyConnChanged_b = TRUE;
#endif /*SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF*/

            /* reset the requested mode back to IDLE */
            lSoAdDyncSocConfig_pst->socConRequestedMode_en  = SOAD_EVENT_IDLE;

            /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
            SchM_Enter_SoAd_SocConModeProperties();

            /* reset the sub state to SOAD_EVENT_IDLE */
            lSoAdDyncSocConfig_pst->socConSubMode_en     = SOAD_SOCON_IDLE;

            lSoAdDyncSocConfig_pst->socConMode_en        = SOAD_SOCON_OFFLINE;

            SchM_Exit_SoAd_SocConModeProperties();

            /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
            SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_OFFLINE );

            break;
        }

        case SOAD_SOCON_TCPIP_ALLOCATED:
        {
            /* it is specific scenario where TcpIp_SoAdGetSocket() succeed, but bind failed, but socket is valid */
            if(SOAD_NUM_TCPIP_SOCKETS > lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo)
            {
                lRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                          lSoAdDyncSocConfig_pst->statusAbort_b);

                /* condition check: if the TcpIp socket close is successful  */
                if( E_OK == lRetVal_u8 )
                {
                    /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
                    SoAd_ResetDynSockTblByEntry(StaticSocketId_uo, lIdxDynSocket_uo, SOAD_SOCON_OFFLINE );

                    /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
                    SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_OFFLINE );
                }
            }

            break;
        }

        /* socket submodes SOAD_SOCON_BOUND and SOAD_SOCON_TCPWAITFORREMOTE treated in same way.
         * In these cases, sockets are not yet ACTIVE, so no active Tp sessions are possible.
         * Simple close the related TcpIp socket, finally go to OFFLINE, if TcpIpEvent is called back */
        case SOAD_SOCON_BOUND:
        case SOAD_SOCON_TCPWAITFORREMOTE:
        {
            /* call a function which shall initiate closing activities by calling TcpIp_Close */
            lRetVal_u8 = SoAd_CallTcpIpClose(StaticSocketId_uo);

            /* condition check: if the TcpIp socket close is successful  */
            if( E_OK == lRetVal_u8 )
            {
                /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
                SoAd_ResetDynSockTblByEntry(StaticSocketId_uo, lIdxDynSocket_uo, SOAD_SOCON_OFFLINE );

                /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
                SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_OFFLINE );
            }

            break;
        }

        /* socket submode is SOAD_SOCON_ACTIVE, the SoAd has to terminate the active Tp sessions if there,
         * and then TcpIp sockets will be closed, finally go to OFFLINE, if TcpIpEvent is called back */
        case SOAD_SOCON_ACTIVE:
        {
            /*if close is requested by UL and socket sub state is either SOAD_SOCON_ACTIVE,
             * then below function will be triggered
             */
            SoAd_PerformCloseActionSeq(StaticSocketId_uo, SOAD_SOCON_OFFLINE);
            break;
        }

        /* As the socket submode is SOAD_SOCON_READYTOCLOSE, means already
         * activities related to closing actions are initiated and SoAd_TcpIpEvent also called back with
         * CLOSED event for this socket, so Goto OFFLINE.
         *
         * If upper layer calls SoAd_CloseSoCon() and before processing the closing request in next MainFunction,
         * If TcpIp closes the socket because of its internal issues and calls SoAd_TcpIpEvent before the MainFunction,
         * Then SoAd misses the the steps like terminating active Tp sessions before going to OFFLINE */
        case SOAD_SOCON_READYTOCLOSE:
        {
            /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
            SoAd_ResetDynSockTblByEntry(StaticSocketId_uo, lIdxDynSocket_uo, SOAD_SOCON_OFFLINE );

            /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
            SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_OFFLINE );
            break;
        }

        /*As socket submode is SOAD_SOCON_WAITFORCLOSE, means SoAd is called TcpIp_Close for this socket
         * and waiting for SoAd_TcpIpEvent. So No actions shall be taken if upper layer request to close the socket again.
         * As the socket is already in the closing proceedings, so simply break from the switch case*/
        case SOAD_SOCON_WAITFORCLOSE: break;

        default:
        {
            /* Nothing to do */
            break;
        }

    }

}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_PerformCloseActionSeq()
 *
 * \Function description
 * This service shall Perform the close request triggered when socket state is SOAD_SOCON_ACTIVE
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 * \param   SoAd_SoConModeType
 * \arg     SoConMode_en- The mode which shall be informed to the upper layer
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_PerformCloseActionSeq( SoAd_SoConIdType   StaticSocketId_uo, \
                                        SoAd_SoConModeType SoConMode_en )
{
    /* Local pointer to store the Dynamic array */
SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType         lIdxDynSocket_uo;

    /* Variable to hold the return value  */
    Std_ReturnType           lRetVal_u8;

    /*Initialization of local variables */
    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst  =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
    SchM_Enter_SoAd_TxRxReq();

    /* check if there are no ongoing IF Tx and Rx*/
    if((lSoAdDyncSocConfig_pst->rxOngoingCnt_u8 == 0)
#if (SOAD_IF_PRESENT != STD_OFF )
     && (lSoAdDyncSocConfig_pst->ifTransmissionOnGoing_b == FALSE)
#endif
      )
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00637] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** SoAd shall perform the following actions within SoAd_MainFunction() to close a socket connection:         ***/
        /** 1. Terminate active TP sessions (if any) and notify the upper layer about the termination                 ***/
        /** 2. Disable further transmission or reception for this socket connection, i.e. new transmit requests       ***/
        /**    shall be rejected with E_NOT_OK and received messages shall simply be discarded                        ***/
        /** 3. Close related TcpIp sockets                                                                            ***/
        /** 4. Change the state of the socket connection to SOAD_SOCON_OFFLINE if closing of the socket connection    ***/
        /**    results from a SoAd_CloseSoCon() request or to SOAD_SOCON_RECONNECT otherwise                          ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        /* Set the global variables to skip the further transmission and reception */
        lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = TRUE;

        SchM_Exit_SoAd_TxRxReq();

        /* As per Autosar, Terminate ongoing transmission and reception and provide the callbacks to UL */
        SoAd_TerminateActiveSession(lIdxDynSocket_uo);

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00637]: step(2) *******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** SoAd shall perform the following actions within SoAd_MainFunction() to close a socket connection:         ***/
        /** 3. Close related TcpIp sockets                                                                            ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        /* call a function which shall initiate closing activities by calling TcpIp_Close */
        lRetVal_u8 = SoAd_CallTcpIpClose(StaticSocketId_uo);

        /* condition check: if the TcpIp socket close is successful */
        if( E_OK == lRetVal_u8 )
        {
            /****************************************************************************************************************/
            /****************************************** [SWS_SoAd_00637]: step(2 & 4) ***************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /** SoAd shall perform the following actions within SoAd_MainFunction() to close a socket connection:         ***/
            /** 2. Disable further transmission or reception for this socket connection, i.e. new transmit requests       ***/
            /**    shall be rejected with E_NOT_OK and received messages shall simply be discarded                        ***/
            /** 4. Change the state of the socket connection to SOAD_SOCON_OFFLINE if closing of the socket connection    ***/
            /**    results from a SoAd_CloseSoCon() request or to SOAD_SOCON_RECONNECT otherwise                          ***/
            /****************************************************************************************************************/
            /****************************************************************************************************************/

            /* if socket is closed and state is changed to OFFLINE, then new transmission and reception will automatically discarded */
            /* call the function to reset the dynamic data structure and states to default values */

            /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
            SoAd_ResetDynSockTblByEntry(StaticSocketId_uo, lIdxDynSocket_uo, SoConMode_en );

            /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
            SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SoConMode_en );
        }

    }
    else
    {
        SchM_Exit_SoAd_TxRxReq();
    }

}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessOpenReq()
 *
 * \Function description
 * This service shall Process the open request triggered by UL or SoAd
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessOpenReq(SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /*Initialization of local variables */
    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo        = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst  = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    switch(lSoAdDyncSocConfig_pst->socConSubMode_en)
    {
    /* socket is at main mode OFFLINE, so all actions from the beginning shall be done to open a socket.
     * First by updating the entries in SoAd_TcpServerConfig_s or SoAd_UdpConfig_s if applicable,
     * then do the other opening actions */
        case SOAD_SOCON_CLOSE:
        {
            /* call a function which performs all open socket activities*/
            SoAd_PerformOpenSeq(StaticSocketId_uo);
            break;
        }

        case SOAD_SOCON_TCPIP_ALLOCATED:
        {
            /*call a function which performs opening actions*/
            SoAd_SocketOpenActions(StaticSocketId_uo);
            break;
        }

        /* Socket submode is SOAD_SOCON_BOUND, means TcpIp_SoAdGetSocket, TcpIp_Bind are already finished
         * in last main functions, and waiting to perform either TcpIp_Connect or TcpIp_Listen calls etc*/
        case SOAD_SOCON_BOUND:
        {
            /* call a function which performs all open socket activities after success ful bound */
            SoAd_ActivateTcpUdpSockets(StaticSocketId_uo);

            break;
        }

        default:
        {
            /* Nothing to do */
            break;
        }
    }
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_PerformOpenSeq()
 *
 * \Function description
 * This service shall update the listen socket to Tcp server table if already created and UDP Group socket to UDP Gro table if already created.
 * And SoAd_SocketOpenActions() function will be called to do the action necessary for opening the new socket connection
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_PerformOpenSeq(SoAd_SoConIdType StaticSocketId_uo )
{
    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType lIdxSoConGroup_uo;

#if ((SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF))
    /* Variable to hold the return value */
    Std_ReturnType   lRetVal_u8;

    lRetVal_u8 = E_NOT_OK;
#endif

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00589]: Step (3 & 4)***************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /***(3)remote address is set (either specified by configuration or set via the function SoAd_SetRemoteAddr()*****/
    /***(4)local IP address is assigned, i.e. SoAd_LocalIpAddrAssignmentChg() has been called with the related  *****/
    /***            LocalAddrId and TCPIP_IPADDR_STATE_-ASSIGNED as State                                       *****/
    /****************************************************************************************************************/

    /* Condition check: Local IP address assignment is done.
     * No other check is performed in this situation, because the default value (0xFFFF) is a valid configuration
     * for the remote IP address or the remote port number.
     */

    if( TCPIP_IPADDR_STATE_ASSIGNED == SoAd_CurrConfig_cpst->SoAd_LocalAddrIdInfo_past[SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpLocalAddressIdx_u8].localAddrIdx_State_en)
    {
#if ((SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF))
        switch(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en)
        {
#if (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF)
            /* check if it is TCP protocol */
            case TCPIP_IPPROTO_TCP:
            {
                /* check if the socket belongs to TCP server group */
                if( STD_OFF == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].tcpProtocolConf_cpst->tcpInitiate_u8 )
                {
                    /* call function to check group listen socket Id is already assigned*/
                    lRetVal_u8 = SoAd_UpdateTcpServTblIfTcpListenSocketPresent(StaticSocketId_uo);
                }

                break;
            }
#endif /*SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF*/

#if (SOAD_UDP_GRP_PRESENT != STD_OFF)
            /* check if it is UDP protocol */
            case TCPIP_IPPROTO_UDP:
            {
                /* check if the socket belongs to UDP multi sockets group, Protocol is UDP and Multiple sockets are present
                 * in the group (NOT only a single socket) */
                if( 1 < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo )
                {
                    /* call function to check group Udp socket Id is already assigned*/
                    lRetVal_u8 = SoAd_UpdateUdpGrpTblIfUdpSocketPresent(StaticSocketId_uo);
                }

                break;
            }
#endif /*SOAD_UDP_GRP_PRESENT != STD_OFF*/
            default:
            {
                /* Nothing to do */
                break;
            }

        }


        /* Enter into the condition if lRetVal_u8 is != E_OK,
         * It will be E_NOT_OK if (socket belongs to TCP serverGroup & Listen socket is not assigned for this TcpServerSocket connection group) or
         * (socket belongs to UDP Group & UDP socket is not assigned for this UDPServerSocket connection group)
         */
        if( E_OK != lRetVal_u8 )
#endif
        {
            /*call a function which performs opening actions*/
            SoAd_SocketOpenActions(StaticSocketId_uo);
        }
    }
    /*As either local ip address is not assigned or remote address is NULL socket shall not be open this time*/
    else
    {
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en = SOAD_SOCON_RECONNECT;


        /* call a function which shall invoke the upper layer call back for mode change */
        SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );

    }

}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_SocketOpenActions()
 *
 * \Function description
 * This service shall perform the opening actions of a new socket connection
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_SocketOpenActions( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the internal function retval  */
    Std_ReturnType               lRetVal_u8;

    /* variable to hold the socket identifier representing the requested socket */
    TcpIp_SocketIdType           lTcpIpSockId_uo;

    /* declaration of variable to hold TcpIp address families like ipv4 or ipv6 */
    TcpIp_DomainType             lDomain_en;

    /* declaration of variable to hold Protocol type used by a socket */
    TcpIp_ProtocolType           lProtocol_en;

    /* variable to hold IP address identifier representing the local IP address and EthIf controller to bind the socket to */
    TcpIp_LocalAddrIdType        lLocalAddrId_u8;

    /* variable to hold Local port to which the socket shall be bound */
    uint16                       lLocalPort_u16;

    /* variable to hold Local port to which the socket shall be bound */
    uint16                       lLocalPortCopy_u16;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /*Initialization of variables */
    lTcpIpSockId_uo                 = SOAD_CFG_SOCKET_ID_DEFAULT_VALUE;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo                = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst          = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* Check for TcpIp has already assigned a socket for this connection, in previous main functions.
     * If it is already assigned, then no need to call again TcpIp_SoAdGetSocket to get a socket from TcpIp layer.
     * If it is not assigned yet, then perform the below steps */
    if( SOAD_NUM_TCPIP_SOCKETS <= lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo )
    {
        /* Get the domain of the requested socket connection to be opened from the static soc table */
        lDomain_en      = (TcpIp_DomainType)SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].domain_en;

        /* Get the protocol of the requested socket connection to be opened from the static soc table */
        lProtocol_en    = (TcpIp_ProtocolType)SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en;

#ifdef SOAD_DEBUG_AND_TEST
        if (CDTest_SoAd_TcpIpGetSocStatus_au8[StaticSocketId_uo] == 1)  // TCP_GETSOCON_FAIL
        {
            lDomain_en = TCPIP_AF_NONE;
        }
#endif

        /****************************************************************************************************************/
        /*************************** [SWS_SoAd_00590, SWS_SoAd_00638]: Step (1)******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** Get an appropriate socket from TcpIp by calling TcpIp_SoAdGetSocket() with the TcpIp_DomainType          ***/
        /*** implicitly specified by SoAdSocketLocalAddressRef, and the protocol type specified by SoAdSocketProtocol ***/
        /****************************************************************************************************************/

        /* call for TcpIp_SoAdGetSocket to get TcpIp socket linked to this dynamic socket */
        lRetVal_u8 = TcpIp_SoAdGetSocket( lDomain_en, lProtocol_en, &lTcpIpSockId_uo);

#ifdef SOAD_DEBUG_AND_TEST
        CDTest_SoAd_TcpIpGetSoc_au8[StaticSocketId_uo] = lRetVal_u8;
#endif
        /*TcpIp_SoAdGetSocketcket() is success and lTcpIpSockId_uo is valid */
        if((E_OK == lRetVal_u8) &&
           (SOAD_NUM_TCPIP_SOCKETS > lTcpIpSockId_uo ))
        {
            /* index of the array is socket handle give by tcpip layer and element of the array is dynamic index */
            SoAd_TcpIpSocHandleToDynSockIdx_auo[lTcpIpSockId_uo] = lIdxDynSocket_uo;

            /* Update the dynamic socket table content with TcpIp socket Id */
            lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo = lTcpIpSockId_uo;

            /* change the state to SOAD_SOCON_TCPIP_ALLOCATED if TcpIp_SoAdGetSocket() is success */
            lSoAdDyncSocConfig_pst->socConSubMode_en     = SOAD_SOCON_TCPIP_ALLOCATED;

#if ((SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF))
            /****************************************************************************************************************/
            /******************************************* [SWS_SoAd_00638]: Step (d)******************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /***                    Assign the Listen-Socket to the socket connection group                               ***/
            /****************************************************************************************************************/

            /* get the listen socket id for the group */
            (*SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].groupTcpIpSocketId_puo) = lTcpIpSockId_uo;
#endif /* (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF) */

            /****************************************************************************************************************/
            /**************************************[SWS_SoAd_00590, SWS_SoAd_00638]: Step (2)********************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /***                                Change the socket specific parameters                                     ***/
            /****************************************************************************************************************/

            /* Call the SoAd_TcpIpChangeParameter() api as per SWS_SoAd_00689 */
            SoAd_TcpIpChangeParameter( StaticSocketId_uo, lIdxSoConGroup_uo );
        }
    }

    /* check for successfully got a socket connection from TCP/IP now or
     * got socket from TcpIp layer already in previous main functions */
    if(SOAD_NUM_TCPIP_SOCKETS > lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo )
    {
        /* Check if socket is allocated and not bound yet. If it not Bound already, call for Binding.
        * If TcpIp_Bind was successful in previous main functions, socket shall be in SOAD_SOCON_BOUND, then
        * no need to call TcpIp_Bind again */
        if( SOAD_SOCON_TCPIP_ALLOCATED == lSoAdDyncSocConfig_pst->socConSubMode_en )
        {
            /* Get the local IP from the static soc table */
            lLocalAddrId_u8 = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpLocalAddressIdx_u8;

#ifdef SOAD_DEBUG_AND_TEST
            if (CDTest_SoAd_TcpBindStatus_au8[StaticSocketId_uo] == 1) // TCP_BINDSOCON_FAIL
            {
                lLocalAddrId_u8 = (TcpIp_LocalAddrIdType)0xFFFF;
            }
#endif
            /* Get the local port from the static soc table */
            lLocalPort_u16     = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpLocalPort_u16;
            lLocalPortCopy_u16 = lLocalPort_u16;

            /****************************************************************************************************************/
            /******************************[SWS_SoAd_00590, SWS_SoAd_00638]: Step (3)****************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /***Bind the socket to the local address and port by calling TcpIp_Bind() with the local address identifier   ***/
            /***specified by SoAdSocketLocalAddressRef and local port specified by SoAdSocketLocalPort                    ***/
            /****************************************************************************************************************/

            /* call for TcpIp_Bind to bind the dynamic socket with local ip and local port */
            lRetVal_u8 = TcpIp_Bind( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo, lLocalAddrId_u8, &lLocalPort_u16 );

#ifdef SOAD_DEBUG_AND_TEST
            CDTest_SoAd_TcpBindResult_au8[StaticSocketId_uo] = lRetVal_u8;
#endif
            /* Check TcpIp_Bind() is success and requested port is assigned to TcpIp socket
             * It means, if valid port other than wild card is passed as argument to TcpIp_Bind() function,
             * the same port has to be assigned and returned back to SoAd.
             */
            if( ( E_OK == lRetVal_u8 ) &&
                ( ( lLocalPortCopy_u16 == TCPIP_PORT_ANY ) ||
                  ( lLocalPortCopy_u16 == lLocalPort_u16 ) ) )
            {
                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();

                /* change the state state of the socket connection to SOAD_SOCON_BOUND if bind success */
                lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_BOUND;

               /* Load the Local port value into dynamic table
                * If configured local port is ANY i.e zero, then TcpIp_Bind shall allocate a random(running) local port number
                * So, store the local port number in dynamic table after the update */
                lSoAdDyncSocConfig_pst->soConDynLocalPort_u16 = lLocalPort_u16 ;

                SchM_Exit_SoAd_SocConModeProperties();

                /* call a function which performs after bound actions to open a socket */
                SoAd_ActivateTcpUdpSockets(StaticSocketId_uo);

            }
            else
            {
                lSoAdDyncSocConfig_pst->socConMode_en = SOAD_SOCON_RECONNECT;
                /* call a function which shall invoke the upper layer call back for mode change */
                SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );
            }
        }
    }
    /* As failed to get a dynamic socket connection from TCP/IP */
    else
    {
        lSoAdDyncSocConfig_pst->socConMode_en = SOAD_SOCON_RECONNECT;
        /* call a function which shall invoke the upper layer call back for mode change */
        SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );
    }
}


#if (SOAD_UDP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessTcpIpCloseReq()
 *
 * \Function description
 * This service shall process any TcpIp close request.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * \return  None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessTcpIpCloseReq(SoAd_SoConIdType StaticSocketId_uo)
{

    SoAd_SoConIdType lIdxDynSocket_uo;

#if ( SOAD_UDP_GRP_PRESENT != STD_OFF )
    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType lIdxSoConGroup_uo;

    /* Variable to Loop through all the configurations */
    SoAd_SoConIdType lIdxForLoop_uo ;

    /* Variable to store the Index of the static socket connection */
    SoAd_SoConIdType lIdxStaticSocketLoop_uo;
#endif

#if (SOAD_UDP_GRP_PRESENT != STD_OFF)
    /* Get the static group socket index from static socket table */
    lIdxSoConGroup_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    if( 1U < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo )
    {
        /* fetch the start socket id from Group table and loop through for max socket couent */
        lIdxStaticSocketLoop_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].startSoConIdx_uo;

        for( lIdxForLoop_uo = 0; lIdxForLoop_uo < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo; lIdxForLoop_uo++)
        {
            /* Get the index of the dynamic soc table */
            lIdxDynSocket_uo             = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocketLoop_uo].SoAd_Prv_idxDynSocTable_puo);

            /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
            SchM_Enter_SoAd_TxRxReq();

            /* check dynamic socket index and tcpip socket id are valid and if there is no ongoing Tx and Rx */
            if( ( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo ) && \
                ( SOAD_NUM_TCPIP_SOCKETS > SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo )&&\
                (0 == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].rxOngoingCnt_u8)
#if (SOAD_IF_PRESENT != STD_OFF )
                && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
#endif
              )
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00643]: step(1) *******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /***            Within SoAd_TcpIpEvent() with Event set to TCPIP_UDP_CLOSED, SoAd shall                       ***/
                /***     remove the assignment of the TcpIp socket identified by SocketId from the related UDP socket         ***/
                /***                                            connection group                                              ***/
                /****************************************************************************************************************/

                SchM_Exit_SoAd_TxRxReq();

                SoAd_TerminateActiveSession(lIdxDynSocket_uo);

                /* Reset the entry in SoAd_TcpIpSocHandleToDynSockIdx_auo[] */
                SoAd_TcpIpSocHandleToDynSockIdx_auo[SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo] = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;

                /* call for a function which shall close or reconnect the SoAd sockets */
                /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
                SoAd_ResetDynSockTblByEntry( StaticSocketId_uo, lIdxDynSocket_uo, SOAD_SOCON_RECONNECT );

                /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
                SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

            }
            else
            {
                SchM_Exit_SoAd_TxRxReq();
            }

            /* increment the static socket id to move the next socket in the group */
            ++lIdxStaticSocketLoop_uo;
        }
    }
    else
#endif/*SOAD_UDP_GRP_PRESENT != STD_OFF*/
    {
        lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);


        /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
        SchM_Enter_SoAd_TxRxReq();

        /*check if socket is valid and there is no ongoing Tx and Rx */
        if( ( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo ) && \
            ( SOAD_NUM_TCPIP_SOCKETS > SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo )&&
            (0 == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].rxOngoingCnt_u8)
#if (SOAD_IF_PRESENT != STD_OFF )
            && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
#endif
          )
        {
            SchM_Exit_SoAd_TxRxReq();

            SoAd_TerminateActiveSession(lIdxDynSocket_uo);

            /* Reset the entry in SoAd_TcpIpSocHandleToDynSockIdx_auo[] */
            SoAd_TcpIpSocHandleToDynSockIdx_auo[SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo] = SOAD_CFG_SOCON_ID_DEFAULT_VALUE ;

            /* call a function which resets the members of dynamic structure and dynamic index link from static structure */
            SoAd_ResetDynSockTblByEntry( StaticSocketId_uo, lIdxDynSocket_uo, SOAD_SOCON_RECONNECT );

            /* call a function which shall invoke the upper layer call back for mode change after mode updated*/
            SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

        }
        else
        {
            SchM_Exit_SoAd_TxRxReq();
        }

    }



}
#endif

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessReconnectReq()
 *
 * \Function description
 * This service shall process any socket state requested to be changes to bound.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout):None
 *
 * Parameters (out): None
 *
 * \return  None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessReconnectReq(SoAd_SoConIdType StaticSocketId_uo)
{
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst;

    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Get dynamic socket from static socket */
    lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
    {
        /* Copy the global array index address to local pointer for further processing */
        lSoAdDyncSocConfig_pst = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
        SchM_Enter_SoAd_TxRxReq();

        /* Check if there is no ongoing Rx and IF Tx*/
        if(
#if (SOAD_IF_PRESENT != STD_OFF )
          (lSoAdDyncSocConfig_pst->ifTransmissionOnGoing_b == FALSE) &&
#endif
           (lSoAdDyncSocConfig_pst->rxOngoingCnt_u8 == 0))
        {
            /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
            SchM_Enter_SoAd_SocConModeProperties();

            /* Set socon submode to Bound */
            lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_BOUND;

            /* set socon mode to Reconnect*/
            lSoAdDyncSocConfig_pst->socConMode_en    = SOAD_SOCON_RECONNECT;

            SchM_Exit_SoAd_SocConModeProperties();

            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
            SchM_Enter_SoAd_RemoteAddrUpdate();

            /* copy remote address from static config to dynamic config table*/
            SoAd_CopyRemoteAddrFromStaticSocToDynSoc( ((const SoAd_StaticSocConfigType_tst *) \
                    &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])), lSoAdDyncSocConfig_pst);

            SchM_Exit_SoAd_RemoteAddrUpdate();

            /* Reset discardNewTxRxReq_b flag*/
            lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = FALSE;

            SchM_Exit_SoAd_TxRxReq();

            /* Call a function which shall invoke the upper layer call back for mode change */
            SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

        }
        else
        {
            SchM_Exit_SoAd_TxRxReq();
        }
    }

}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_TcpIpChangeParameter()
 *
 * \Function description
 * This service shall call TcpIP_ChangeParameter() for supported parameters
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo  - Index of Static socket connection table.
 * \param   SoAd_SoConIdType
 * \arg     SoConGroupId_uo- Index of static socket connection group table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_TcpIpChangeParameter( SoAd_SoConIdType StaticSocketId_uo,
                                       SoAd_SoConIdType SoConGroupId_uo)
{

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Variable to store the dynamic socket connection table index */
    SoAd_SoConIdType   lIdxDynSocket_uo;

    /* variable to hold the TcpIp Handle */
    TcpIp_SocketIdType lTcpIpHandle_uo;

    /* variable to hold the SoAd tcpip priority */
    uint8              lTcpIpFramePriority_u8;

#if (SOAD_IPv6_SUPPORT_ENABLE != STD_OFF)
    /* variable to hold the SoAd tcpip flow label */
    uint32             lTcpIpFlowLabel_u32;
    uint8              lTcpIpFlowLabel_u8[4];

    /* variable to hold the SoAd TcpIp Differentiated Services Field */
    uint8              lTcpIpDscp_u8;
#endif

#if (SOAD_TCP_PRESENT != STD_OFF)

#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
    /* variable to hold the out of order frame */
    uint8              lOutoforder_u8;
#endif

#if((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF ))
    TcpIp_TlsConnectionIdType lTlsConnId_uo;
    uint8                     lTlsConnId_au8[2];
#endif/* ((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF )) */

    /* variable to hold the Nagle algorithm */
    uint8              lTcpIpNagle_u8;

#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
    /* variable to hold the RxBufferMin in big endian format */
    uint8             lRxBufMin_au8[2];
#endif
#if (SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF)
    /* variable to hold the KeepAlive Time */
    uint32            lTcpKeepAliveTime_u32;

    /* variable to hold the Tcp Keep Alive parameters in big endian format */
    uint8             lTcpKeepAliveValue_au8[4];

    /* variable to hold the KeepAlive Interval */
    uint32            lTcpKeepAliveIntvl_u32;

     /* variable to hold the KeepAlive Probe Max */
    uint16            lTcpKeepAliveProbeMax_u16;

    /* variable to hold the keep alive */
    uint8             lTcpIpKeepAlive_u8;
#endif/* SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF */
#endif /* SOAD_TCP_PRESENT != STD_OFF */

    /* Get dynamic socket from static socket */
    lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /*Initialization of variables */
    lTcpIpHandle_uo = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo;

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGroupId_uo]);

    /* --------------------------------------------- */
    /* Change Frame Prio parameter                   */
    /* --------------------------------------------- */

    /* Call the TcpIp_ChangeParameter() api as per SWS_SoAd_00689 */
    lTcpIpFramePriority_u8 = lSoAdStaticSoConGrpConfig_cpst->soConGrpFramePriority_u8;

    /* Call the TcpIp_ChangeParameter() for frame priority if it is configured */
    if( SOAD_ETH_VLAN_FRAMEPRIO_MAXVAL >= lTcpIpFramePriority_u8 )
    {
        /* Call TcpIp_ChangeParameter api to set the TCPIP_PARAMID_FRAMEPRIO */
        (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                     (TcpIp_ParamIdType)TCPIP_PARAMID_FRAMEPRIO, \
                                     ( &lTcpIpFramePriority_u8 ));
    }

#if (SOAD_IPv6_SUPPORT_ENABLE != STD_OFF)
    /* --------------------------------------------- */
    /* Change Flow Label parameter                   */
    /* --------------------------------------------- */

    /* Get the Flow Label from configured parameter soConGrpFlowLabel_u32 */
    lTcpIpFlowLabel_u32 = lSoAdStaticSoConGrpConfig_cpst->soConGrpFlowLabel_u32;

    /* Call the TcpIp_ChangeParameter() for flow label if it is configured as per SWS_SoAd_00689 */
    if( SOAD_CFG_FLOWLABEL_DEFAULT_VALUE != lTcpIpFlowLabel_u32 )
    {
        /* As TcpIpChnage parameter API only allows ptr to Unit8, so pass the address of Uint8 after converting RxBufferMin to BigEndian */
        TcpIp_WriteU32( &lTcpIpFlowLabel_u8[0], lTcpIpFlowLabel_u32);

        /* Call TcpIp_ChangeParameter api to set the TCPIP_PARAMID_FLOWLABEL */
        (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                     (TcpIp_ParamIdType)TCPIP_PARAMID_FLOWLABEL, \
                                     ( &lTcpIpFlowLabel_u8[0] ));
    }

    /* --------------------------------------------- */
    /* Change DSCP parameter                         */
    /* --------------------------------------------- */

    /* Get the Differentiated Services Field from configured parameter soConGrpDscp_u8 */
    lTcpIpDscp_u8 = lSoAdStaticSoConGrpConfig_cpst->soConGrpDscp_u8;

    /* Call the TcpIp_ChangeParameter() for flow label if it is configured as per SWS_SoAd_00689 */
    if( SOAD_CFG_DSCP_DEFAULT_VALUE != lTcpIpDscp_u8 )
    {
        /* Call TcpIp_ChangeParameter api to set the TCPIP_PARAMID_DSCP */
        (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                     (TcpIp_ParamIdType)TCPIP_PARAMID_DSCP, \
                                     ( &lTcpIpDscp_u8 ));

    }

#endif

#if (SOAD_TCP_PRESENT != STD_OFF)
    if( TCPIP_IPPROTO_TCP == lSoAdStaticSoConGrpConfig_cpst->soConGrpProtocol_en )
    {
        /* --------------------------------------------- */
        /* Change Nagle parameter                        */
        /* --------------------------------------------- */

        /* If configured tcpNoDelay_u8 parameter is valid */
        if(INVALID != lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpNoDelay_u8)
        {
            /* Get the TcpIpNagle from configured parameter tcpNoDelay_u8 */
            lTcpIpNagle_u8 = ((lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpNoDelay_u8 == STD_ON) ? STD_OFF : STD_ON);

            /* Call TcpIp_ChangeParameter api to make TCPIP_PARAMID_TCP_NAGLE active */
            (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                         TCPIP_PARAMID_TCP_NAGLE, \
                                         ( &lTcpIpNagle_u8 ) );
        }

#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
        /* --------------------------------------------- */
        /* Change Out of order Parameter                 */
        /* --------------------------------------------- */

        /* If configured tcpOutofOrder_b parameter is valid */
        if(FALSE != lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpOutofOrder_b)
        {
            /* This parameter is passed as 0 as it is just to assign the buffer per socket */
            lOutoforder_u8 = 0;

            /* Call TcpIp_ChangeParameter api to make TCPIP_PARAMID_TCP_OUTOFORDER active */
            (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                         TCPIP_PARAMID_TCP_OUTOFORDER, \
                                         &lOutoforder_u8 );
        }
#endif

#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
        /* --------------------------------------------- */
        /* Change Window Size parameter                  */
        /* --------------------------------------------- */

        /* If Rx buffer is configured, we need to indicate to Tcp the SoAd Rx buffer size */
        if( SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].dynRxBuffTableEntry_pst != NULL_PTR )
        {
            /* As TcpIpChnage parameter API only allows ptr to Unit8, so pass the address of Uint8 after converting RxBufferMin to BigEndian */
            TcpIp_WriteU16( &lRxBufMin_au8[0],
                            SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].dynRxBuffTableEntry_pst->TotalRxBuffSize_cu16 );

            /* Call TcpIp_ChangeParameter api to give TCPIP_PARAMID_TCP_RXWND_MAX size */
            (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                         TCPIP_PARAMID_TCP_RXWND_MAX, \
                                         ( &lRxBufMin_au8[0] ) );
        }
#endif

#if (SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF)
        /* --------------------------------------------- */
        /* Change Keep Alive parameter                   */
        /* --------------------------------------------- */

        /* Get the TcpIpKeepAlive_u8 from configured parameter tcpKeepAlive_u8 */
        lTcpIpKeepAlive_u8     =  lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAlive_u8;

        /* Call TcpIp_ChangeParameter api to make TCPIP_PARAMID_TCP_KEEPALIVE active */
        (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                     TCPIP_PARAMID_TCP_KEEPALIVE, \
                                     ( &lTcpIpKeepAlive_u8 ) );

        /* If KeepAlive feature is enabled then only update the KeepAliveTime and KeepAliveInterval */
        if( STD_ON == lTcpIpKeepAlive_u8 )
        {
             /* If KeepAlive Time is configured for this socket connection group, call rba_EthTcp_ChangeParameter and update the KeepAlive Time */
             if( SOAD_CFG_TCPKEEPALIVE_DEFAULT_VALUE != lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveTimeIdx_uo )
             {
                 /* Get the TcpKeepAlive Interval from the SoAd_Config_TcpKeepAliveTime_cau32[] */
                 lTcpKeepAliveTime_u32 = SoAd_CurrConfig_cpst->SoAd_Config_TcpKeepAliveTime_cpu32[lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveTimeIdx_uo];

                 /* As TcpIpChnage parameter API only allows ptr to unit8, so pass the address of uint8 after converting  lTcpKeepAliveTime to BigEndian */
                 TcpIp_WriteU32( &lTcpKeepAliveValue_au8[0], \
                                 ( lTcpKeepAliveTime_u32 ) );

                 /* Call TcpIp_ChangeParameter api to update TCPIP_PARAMID_TCP_KEEPALIVE_TIME  */
                 (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                              TCPIP_PARAMID_TCP_KEEPALIVE_TIME, \
                                              ( &(lTcpKeepAliveValue_au8[0]) ) );
              }

#if (SOAD_TCP_KEEP_ALIVE_PROBEMAX_PRESENT != STD_OFF)
              /* If KeepAlive Max Probes are configured for this socket connection group, call rba_EthTcp_ChangeParameter and update the KeepAlive Max Probes */
              if( SOAD_CFG_TCPKEEPALIVE_DEFAULT_VALUE != lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveProbeMaxIdx_uo )
              {
                  /* Get the TcpKeepAlive Probe Max from the SoAd_Config_TcpKeepAliveProbeMax_cpu16[] */
                  lTcpKeepAliveProbeMax_u16 = SoAd_CurrConfig_cpst->SoAd_Config_TcpKeepAliveProbeMax_cpu16[lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveProbeMaxIdx_uo];

                  /* As TcpIpChnage parameter API only allows ptr to unit8, so pass the address of uint8 after converting  lTcpKeepAliveInterval to BigEndian */
                  TcpIp_WriteU16( &lTcpKeepAliveValue_au8[0], \
                                  ( lTcpKeepAliveProbeMax_u16 ) );

                  /* Call TcpIp_ChangeParameter api to make TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL active */
                  (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                               TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX, \
                                               ( &(lTcpKeepAliveValue_au8[0]) ) );

              }
#endif /* SOAD_TCP_KEEP_ALIVE_PROBEMAX_PRESENT != STD_OFF */

#if (SOAD_TCP_KEEP_ALIVE_INTERVAL_PRESENT != STD_OFF)
              /* If KeepAlive Interval is configured for this socket connection group, call rba_EthTcp_ChangeParameter and update the KeepAlive Interval */
              if( SOAD_CFG_TCPKEEPALIVE_DEFAULT_VALUE != lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveIntervalIdx_uo )
              {
                 /* Get the TcpKeepAlive Interval from the SoAd_Config_TcpKeepAliveInterval_cau32[] */
                 lTcpKeepAliveIntvl_u32 = SoAd_CurrConfig_cpst->SoAd_Config_TcpKeepAliveInterval_cpu32[lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpKeepAliveIntervalIdx_uo];

                 /* As TcpIpChnage parameter API only allows ptr to unit8, so pass the address of uint8 after converting  lTcpKeepAliveInterval to BigEndian */
                 TcpIp_WriteU32( &lTcpKeepAliveValue_au8[0], \
                                 ( lTcpKeepAliveIntvl_u32 ) );

                 /* Call TcpIp_ChangeParameter api to make TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL active */
                 (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                              TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL, \
                                              ( &(lTcpKeepAliveValue_au8[0]) ) );
              }
#endif /* SOAD_TCP_KEEP_ALIVE_INTERVAL_PRESENT != STD_OFF */
        }

#endif/* SOAD_TCP_KEEP_ALIVE_ENABLE != STD_OFF */

#if((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF ))
        /* Get the Tls conn Id from configured parameter tcpTlsConnId_uo */
        lTlsConnId_uo     =  lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpTlsConnId_uo;

        /*Check if tcp connection refers to secure connection */
        if( SOAD_TLS_CONN_DEFAULT_VALUE != lTlsConnId_uo )
        {
            /* TcpIp_TlsConnectionIdType can be uint8 or uint16 .so check  lTlsConnId_uo is in range uint16 or uint8 */
#if(TCPIP_SIZEOF_TLS_CONN_ID_TYPE == 2U)

            /* As TcpIpChnage parameter API only allows ptr to unit8, so pass the address of uint8 after converting  lTlsConnId_uo to BigEndian */
            TcpIp_WriteU16( &lTlsConnId_au8[0], lTlsConnId_uo );

#else
            lTlsConnId_au8[0] = (uint8)lTlsConnId_uo;
#endif/* (TCPIP_SIZEOF_TLS_CONN_ID_TYPE == 2U) */

            /* Call TcpIp_ChangeParameter api to set the TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT */
            (void)TcpIp_ChangeParameter( lTcpIpHandle_uo,\
                                         (TcpIp_ParamIdType)TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT, \
                                         ( &lTlsConnId_au8[0] ));

        }/*Check if tcp connection refers to secure connection */
#endif/* ((SOAD_TCP_TLS_ENABLED != STD_OFF) && ( SOAD_TCP_TLSREF_CFGD != STD_OFF )) */

    }
#endif /* SOAD_TCP_PRESENT != STD_OFF */
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ActivateTcpUdpSockets()
 *
 * \Function description
 * This service shall do activities after SoAd socket is bound with TcpIp socket
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */

/* This function shall do the activities, after socket is successfully Bound to valid local port and Local IP.
 * This shall handle separate activities for different protocols */
static void SoAd_ActivateTcpUdpSockets(SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType                          lIdxSoConGroup_uo;

     /*Initialization of variables */
    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo]);

    switch(lSoAdStaticSoConGrpConfig_cpst->soConGrpProtocol_en)
    {
    /* for TCP socket connections */
#if (SOAD_TCP_PRESENT != STD_OFF)
        case TCPIP_IPPROTO_TCP:
        {
#if (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF)
            /* check for SoAdSocketTcpInitiate is enabled for this soc connection (client) */
            if( STD_OFF == lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpInitiate_u8 )
            {
                 /****************************************************************************************************************/
                 /******************************************* [SWS_SoAd_00638]: Step (e)******************************************/
                 /****************************************************************************************************************/
                 /****************************************************************************************************************/
                 /***         Activate the socket connection to accept connections from remote nodes                           ***/
                 /****************************************************************************************************************/
                 /* call a function which invokes opening of a Tcp server socket*/
                 SoAd_ActivateTcpServerSocket(StaticSocketId_uo);
            }
            /* check for SoAdSocketTcpInitiate is enabled for this soc connection (client) */
            else /* ( STD_ON == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].tcpProtocolConf_cpst->tcpInitiate_u8 ) */
#endif /* SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF */

            {
#if (SOAD_TCP_CLIENT_PRESENT != STD_OFF)
                /* call a function which invokes opening of a Tcp client socket as per SWS_SoAd_00590 step(4)*/
                SoAd_ActivateTcpClientSocket(StaticSocketId_uo);
#endif /* SOAD_TCP_CLIENT_PRESENT != STD_OFF */
            }

            break;
        }
#endif /* SOAD_TCP_PRESENT != STD_OFF */

#if (SOAD_UDP_PRESENT != STD_OFF)
    /* for UDP soc connections, If socket is already Bound, then SoAd shall take a decision as per SWS_SoAd_00591 */
        case TCPIP_IPPROTO_UDP:
        {
            /* call a function which invokes opening of a udp socket*/
            SoAd_ActivateUdpSocket(StaticSocketId_uo);
            break;
        }
#endif  /*SOAD_UDP_PRESENT != STD_OFF*/

        default:
        {
            /* Nothing to do */
            break;
        }
    }/*End for : If protocol is TCP*/
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_CallTcpIpClose()
 *
 * \Function description
 * This service shall do activities for calling a TcpIp close
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK               - Tcp socket closed
 *           E_NOT_OK           - Tcp socket not closed
 *
 ***************************************************************************************************
 */

static Std_ReturnType SoAd_CallTcpIpClose( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *              lSoAdDyncSocConfig_pst ;

    /* variable to store the return value */
    Std_ReturnType                            lRetVal_u8;

    /* variable to store the return value */
    Std_ReturnType                            lFunctionRetVal_u8;

    /* Variable to hold index of the socket connection group */
    SoAd_SoConIdType                          lIdxSoConGroup_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType                          lIdxDynSocket_uo;

    /*Initialization of variables */
    lRetVal_u8              = E_NOT_OK;
    lFunctionRetVal_u8      = E_NOT_OK;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo        = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst  =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* Get the index of socket connection group table */
    lIdxSoConGroup_uo       = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo;

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo]);

    switch( lSoAdStaticSoConGrpConfig_cpst->soConGrpProtocol_en )
    {
#if (SOAD_TCP_PRESENT != STD_OFF)
        /* If TCP Protocol */
        case TCPIP_IPPROTO_TCP:
        {
#if (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF)
            /* check if the socket belongs to TCP server group */
            if( STD_OFF == lSoAdStaticSoConGrpConfig_cpst->tcpProtocolConf_cpst->tcpInitiate_u8 )
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00642]: step(1) *******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** To close related TcpIp sockets on closing a socket connection within SoAd_MainFunction(), SoAd shall      ***/
                /** perform the following actions:                                                                            ***/
                /** (1) In case of a TCP socket connection:                                                                   ***/
                /** (b) If all socket connections of a TCP socket connection group have been closed by SoAd_CloseSoCon():     ***/
                /**     Close the related Listen-Socket by calling TcpIp_CloseSocket() with parameter abort set to the same   ***/
                /**     value as provided by SoAd_CloseSoCon()or set to FALSE in case closing was not initiated by            ***/
                /**     SoAd_CloseSoCon()                                                                                     ***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                /* Call the Internal function to close the TCP server socket */
                lRetVal_u8 = SoAd_TcpServerSocketTcpIpClose( StaticSocketId_uo );
            }
            else
#endif/*SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF*/
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00642]: step(1) *******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** To close related TcpIp sockets on closing a socket connection within SoAd_MainFunction(), SoAd shall      ***/
                /** perform the following actions:                                                                            ***/
                /** (1) In case of a TCP socket connection:                                                                   ***/
                /** (a) Close the related socket by calling TcpIp_CloseSocket() with parameter abort set to the same value    ***/
                /**     as provided by SoAd_CloseSoCon()or set to FALSE in case closing was not initiated by SoAd_CloseSoCon()***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /* call TcpIp to close the client socket */
                lRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo , \
                                          lSoAdDyncSocConfig_pst->statusAbort_b );
            }

            break;
        }
#endif /* SOAD_TCP_PRESENT != STD_OFF */

#if (SOAD_UDP_PRESENT != STD_OFF)
        /* If UDP Protocol */
        case TCPIP_IPPROTO_UDP:
        {
#if (SOAD_UDP_GRP_PRESENT != STD_OFF)

            /* check if the socket belongs to UDP group */
            if( 1 < lSoAdStaticSoConGrpConfig_cpst->maxSoConChannel_uo )
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00642]: step(2) *******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** To close related TcpIp sockets on closing a socket connection within SoAd_MainFunction(), SoAd shall      ***/
                /** perform the following actions:                                                                            ***/
                /** (2) In case of a UDP socket connection:                                                                   ***/
                /** (b) If all socket connections of a UDP socket connection group have been closed by SoAd_CloseSoCon():     ***/
                /**     Close the related UDP socket by calling TcpIp_CloseSocket() with parameter abort set to the same      ***/
                /**     value as provided by SoAd_CloseSoCon()or set to FALSE in case closing was not initiated by            ***/
                /**     SoAd_CloseSoCon().                                                                                    ***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                /* Call the internal function to close the UDP Group socket */
                lRetVal_u8 = SoAd_UdpGroupSocketTcpIpClose( StaticSocketId_uo );
            }
            else
#endif/*SOAD_UDP_GRP_PRESENT != STD_OFF*/
            {
                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00642]: step(2) *******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** To close related TcpIp sockets on closing a socket connection within SoAd_MainFunction(), SoAd shall      ***/
                /** perform the following actions:                                                                            ***/
                /** (2) In case of a UDP socket connection:                                                                   ***/
                /** (a) If the socket connection is NOT part of a socket connection group (i.e. there is only one socket      ***/
                /**     connection in the socket connection group configuration container): Close the related socket by       ***/
                /**     calling TcpIp_CloseSocket() with parameter abort set to the same value as provided by                 ***/
                /**     SoAd_CloseSoCon()or set to FALSE in case closing was not initiated by SoAd_CloseSoCon()               ***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                /* call TcpIp to close the UDP single socket */
                lRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo ,
                                          lSoAdDyncSocConfig_pst->statusAbort_b );
            }

            break;
        }
#endif /* SOAD_UDP_PRESENT != STD_OFF */

        default:
        {
            /* Nothing to do */
            break;
        }
    }

    /* If TcpIp close was successful */
    if( E_OK == lRetVal_u8 )
    {
        /* If TcpIP Close was called for tcp established socket, then this if condition will be succeed */
        if (lSoAdDyncSocConfig_pst->socConSubMode_en != SOAD_SOCON_READYTOCLOSE )
        {
            /* Issue:
             * ******
             * If socket close is triggered Internally by SoAd(Due to Ul_TpCopyRxData failed in SoAd_RxIndication ) for any of the UDP socket, socket will goes to RECONNECT state
             * and it has to come back to ONLINE as per the requirement. But in the below line socConSubMode_en is changed to SOAD_SOCON_WAITFORCLOSE and hence SoAd socket will
             * wait for SoAd_TcpIpEvevnt infinitely.
             * Since TcpIp component already provides the TcpIpEvent, it won't provide the TcpIpEvent once again and socket wont't come to ONLINE.
             *
             * Fix for issue:
             * **************
             *  socConSubMode_en should not be changed below, if socket state to changed to goes back to ONLINE.
             *  So added the below if condition to avoid the socConSubMode_en change and socConRequestedMode_en would set to SOAD_EVENT_ONLINEREQ when socket was RECONNECT state,
             *  It means if socket close is triggered by SoAd.
             */
            if(lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_ONLINEREQ)
            {
                /* goto a state where SoAd shall wait for SoAd_TcpIpEvent call back */
                lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_WAITFORCLOSE ;
            }
        }
        else
        {
            /* state of socket SOAD_SOCON_READYTOCLOSE means, SoAd_TcpIpEvent is already called for CLOSED, so
             * set return value to E_OK such that the socket shall be closed in same main function.
             *
             * Ideally for all UDP sockets SoAd_TcpIpEvent will be called immediately and it will processed in the
             * same Mainfunction. For all TCP sockets it will be processed in the next Main function after
             * TcpIp Event is called with TCP_CLOSED.
             */
            lFunctionRetVal_u8 = E_OK;
        }
    }

    /* Return the value  */
    return(lFunctionRetVal_u8);
}

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ResetRoutingGroupStatus()
 *
 * \Function description
 * This service shall reset the Routing Group status to default and it will be called from
 * SoAd Init function during boot up.
 *
 * Parameters (in):None
 *
 * Parameters (inout):
 * \param   const SoAd_ConfigType*
 * \arg     SoAdConfig_cpst - Pointer to the configuration data of the SoAd module.
 *
 * Parameters (out): None
 *
 ***************************************************************************************************
 */
static void SoAd_ResetRoutingGroupStatus( const SoAd_ConfigType * SoAdConfig_cpst )
{
    /* Local pointer to store the SoAd_RoutingGroupSoConStructMap_tst array for given index*/
    SoAd_RoutingGroupSoConStructMap_tst * lSoAdRoutingGroupSoConStructMap_pst;

    /* Loop through all the configurations */
    uint16                                lIdxForLoop_u16 ;

    /* Loop through the routing groups to configure the default value */
    for( lIdxForLoop_u16 = 0; ( lIdxForLoop_u16 < SoAdConfig_cpst->SoAd_noRoutingGrps_cuo ); lIdxForLoop_u16++ )
    {
        /* copy the global array address to local pointer for given index */
        lSoAdRoutingGroupSoConStructMap_pst = &(SoAdConfig_cpst->SoAd_RoutingGroupSoConStructMap_pst[lIdxForLoop_u16]);

        /* Update the Routing Group status to socket Routing Group mapping data structure */
        lSoAdRoutingGroupSoConStructMap_pst->rgStatus_b = SoAdConfig_cpst ->SoAd_RoutingGroup_ROM_cpb[lIdxForLoop_u16];

        /* Update the Specific Routing Group check flag to false */
        lSoAdRoutingGroupSoConStructMap_pst->specificRgCheck_b = FALSE;
    }

}
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

#if (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_UpdateTcpServTblIfTcpListenSocketPresent()
 *
 * \Function description
 * This service shall update Tcp config table, if socket belongs to Tcp server group
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK       - If Tcp listen socket Id presents for group
 *           E_NOT_OK   - no Tcp listen socket presents
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_UpdateTcpServTblIfTcpListenSocketPresent( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the return value */
    Std_ReturnType               lFunctionRetVal_en;

    /*Variable to hold the TCP Listen socket Id for the matched Group */
    TcpIp_SocketIdType           lGrpListenSocketId_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxListenDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /*Initialization of variables */
    lFunctionRetVal_en              = E_NOT_OK;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst      = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* get the listen socket id for the group */
    lGrpListenSocketId_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].groupTcpIpSocketId_puo);

    /* Check if, already a listen socket Id is assigned for this group, If listen socket is not assigned, then
     * MainFunction shall take care about how to proceed further. like calling TcpIp_SoAdGetSocket, TcpIp_Bind etc.*/

    /* condition check: whether listen socket id is already created */
    if( SOAD_NUM_TCPIP_SOCKETS > lGrpListenSocketId_uo )
    {
        /* if listen socket id is already craeted, then get the dynamic index mapped to listen socket id from SoAd_TcpIpSocHandleToDynSockIdx_auo[] array */
        lIdxListenDynSocket_uo = SoAd_TcpIpSocHandleToDynSockIdx_auo[lGrpListenSocketId_uo];

        /* set the lFunctionRetVal_en to E_OK to inform the caller that listen socket id was already created */
        lFunctionRetVal_en = E_OK;

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00638]: Step (2)*******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /***        In case the Listen-Socket is already assigned to the socket connection:                           ***/
        /***        (a) Activate the socket connection to accept connections from remote nodes                        ***/
        /***specified by SoAdSocketLocalAddressRef and local port specified by SoAdSocketLocalPort                    ***/
        /****************************************************************************************************************/

        /* if socConSubMode_en is any one of the below state, the socket can goes to SOAD_SOCON_TCPWAITFORREMOTE state */
        switch(SoAd_DyncSocConfig_ast[lIdxListenDynSocket_uo].socConSubMode_en)
        {
            case SOAD_SOCON_TCPWAITFORREMOTE:
            case SOAD_SOCON_ACTIVE:
            case SOAD_SOCON_WAITFORCLOSE:
            {
                /* Load the Local port value from previously opened server socket */
                lSoAdDyncSocConfig_pst->soConDynLocalPort_u16 = SoAd_DyncSocConfig_ast[SoAd_TcpIpSocHandleToDynSockIdx_auo[lGrpListenSocketId_uo]].soConDynLocalPort_u16 ;

                /* index of the array is sockethandle give by tcpip layer and element of the array is dyanamic index */
                SoAd_TcpIpSocHandleToDynSockIdx_auo[lGrpListenSocketId_uo] = lIdxDynSocket_uo;

                /* Update the dynamic socket table content with TcpIp socket Id */
                lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo = lGrpListenSocketId_uo;

                /* Set the connection request state to socket open request sent */
                lSoAdDyncSocConfig_pst->socConSubMode_en  = SOAD_SOCON_TCPWAITFORREMOTE;

                lSoAdDyncSocConfig_pst->socConMode_en     = SOAD_SOCON_RECONNECT;

                /* call a function which shall invoke the upper layer call back for mode change,
                 * Here socket is still not ONLINE, so Instead of staying/going to RECOONECT */
                SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );

                break;
            }
            default:
            {
                /* Nothing to do */
                break;
            }
        }
    }
    return lFunctionRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ActivateTcpServerSocket()
 *
 * \Function description
 * This service shall activate the TCP server socket by calling Tcp api's
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ActivateTcpServerSocket(SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* variable to hold the return value of Internal function */
    Std_ReturnType               lRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /*Initialization of variables */
    lRetVal_u8                  = E_NOT_OK;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst      = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /****************************************************************************************************************/
    /******************************************* [SWS_SoAd_00638]: Step (f)******************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** Listen for a remote connection requests on the Listen-Socket by calling TcpIp_TcpListen() with           ***/
    /*** MaxChannels set to the number of socket connections that are part of the TCP socket connection group     ***/
    /****************************************************************************************************************/

     /* Server : call TcpIp_TcpListen for the socket by mentioning maximum allowed socket connections  */
     lRetVal_u8 = TcpIp_TcpListen( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                   SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo );

     /* Condition check: try to open the socket connection in next main function, if retrun value is NOT_OK */
    if( E_OK == lRetVal_u8 )
    {
        /* Update the socConStatus with socket waiting for remote to respond, means
         * SoAd is opened up for accepting msgs from clients, and it is waiting for the remote to respond. */
        lSoAdDyncSocConfig_pst->socConSubMode_en      = SOAD_SOCON_TCPWAITFORREMOTE ;
    }

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00686] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** Within SoAd_MainFunction() and after successfully performing the open actions, SoAd shall change         ***/
    /*** the state of the socket connection to SOAD_SOCON_RECONNECT in case of                                    ***/
    /***            (1) a TCP socket connection                                                                   ***/
    /****************************************************************************************************************/

    lSoAdDyncSocConfig_pst->socConMode_en  = SOAD_SOCON_RECONNECT;
    /* call a function which shall invoke the upper layer call back for mode change */
    SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_TcpServerSocketTcpIpClose()
 *
 * \Function description
 * This service shall close the TCP server socket by calling TcpIp_Close api
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK       - Tcp socket closed successfully
 *           E_NOT_OK   - Tcp socket not closed
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_TcpServerSocketTcpIpClose( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* variable to store the return value */
     Std_ReturnType              lFunctionRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxTcpServerDynSocket_uo;

    /* Variable to hold the other active sockets availability */
    boolean                      lOtherActiveSocketPresent_b;

    /*Initialization of variables */
    lFunctionRetVal_u8            = E_NOT_OK;
    lOtherActiveSocketPresent_b   = TRUE;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst      =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* Check if the established socket is still Active then only call TcpIp_Close()to close Estd socket
     * Use case: TcpIp_Event is already reported TCP_CLOSED for this socket, then Estd socket is not valid OR
     * There is no Established socket allocated for this socket */
    if( SOAD_SOCON_ACTIVE == lSoAdDyncSocConfig_pst->socConSubMode_en )
    {
        /* call TcpIp to close the established socket socket */
        lFunctionRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                          lSoAdDyncSocConfig_pst->statusAbort_b );

        /* Changing of any parameters related to SoAd_TcpServerConfig_s table and calling of TcpIp_Close() for a listen socket
         * is to be handled after SoAd_TcpIpEvent() reported for this established socket, but not depending upon lFunctionRetVal_u8 */
    }
    /* Check atleast listen socket is active for given SoCon Id */
    else
    {
        /* Get the dynamic socket index for the matched listen socket id */
        lIdxTcpServerDynSocket_uo    = SoAd_TcpIpSocHandleToDynSockIdx_auo[lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo];

        /* If socket matched to listen socket via SoAd_TcpIpSocHandleToDynSockIdx_auo[] and that socket is requested to close,
         * then SoAd has to loop through all other sockets in the group and fetch the other active socket if there and
         * assign it to in the same array SoAd_TcpIpSocHandleToDynSockIdx_auo[] for future access.
         */
        if( lIdxTcpServerDynSocket_uo == lIdxDynSocket_uo )
        {
            /* check if any other socket in the same group is being active,
             * if so please get the matched dynamic socket index of the active static socket */
            lOtherActiveSocketPresent_b = SoAd_GetOtherActiveDynSocketForGroup(StaticSocketId_uo, &lIdxDynSocket_uo);
        }
        else
        {
            /* if already matched socket to listen array is not requested to close, then same index will be assigned to retain the value */
            lIdxDynSocket_uo = lIdxTcpServerDynSocket_uo;
        }

        /* If listen socket is mapped to some other sockets, then simply reset the data structure without closing the listen socket.
         * this listen socket will not be closed untill all the established socket mapped to listen sockets are closed sockets,
         * it will be closed when closing the last established socket.
         */
        if( TRUE == lOtherActiveSocketPresent_b )
        {
            /* Assume 2 sockets(soc1 and soc2) are opened successfully, now SoAd_TcpIpSocHandleToDynSockIdx_auo[listen]
             * array will pointing to soc2's dynamic index.
             * when we get the close request from UL for soc2(last opened socket) and if we reset the
             * SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array, then linking willbe destroyed, afterwards we can't use the
             * SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array for future access.
             *
             * So, whenever we are closing the socket which is pointed by SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array,
             * SoAd has to reassign the dynamic index which is still open/active.
             */
            SoAd_TcpIpSocHandleToDynSockIdx_auo[lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo] = lIdxDynSocket_uo;

            /* change the state of the socket connection to READYTOCLOSE */
            lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_READYTOCLOSE;

            /* Set the lFunctionRetVal_u8 to E_OK, so that socket will be closed in the same main function */
            lFunctionRetVal_u8 = E_OK;

        }
        else
        {
           /* call TcpIp to close the listen socket, Abort value for listen socket close is of no significance
            * here Abort value sent as last socket close request abort value retrieved from Dynamic table
            * Example: there are 4 sockets in the group and all are ONLINE, then if first 3 sockets are closed via
            * SoAd_CloseSoCon with ABORT TRUE, then all estd are closed but listen is still not asked for close beacuse
            * 4th socket is still ONLINE. Then SoAd_CloseSoCon called for 4th socket with ABORT FALSE, then Listen socket is
            * also closed after 4th socket's estd is closed. Listen socket close is called with FALSE, which is the request
            * given for the last socket in the group.SWS_SoAd_00642 (1.b) */
            lFunctionRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                              lSoAdDyncSocConfig_pst->statusAbort_b);
        }

    }

    return lFunctionRetVal_u8;
}
#endif/*SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF*/


#if (SOAD_UDP_GRP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_UpdateUdpGrpTblIfUdpSocketPresent()
 *
 * \Function description
 * This service shall update Udp config table, if socket belongs to udp multi socket group
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK       - If UDP Group socket Id presents for group
 *           E_NOT_OK   - no UDP socket presents for given match
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_UpdateUdpGrpTblIfUdpSocketPresent( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the return value */
    Std_ReturnType               lFunctionRetVal_en;

    /*Variable to hold the UDP group socket Id for the matched Group */
    TcpIp_SocketIdType           lUdpGrpSocketId_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxUdpGroupDynSocket_uo;

    /*Initialization of variables */
    lFunctionRetVal_en              = E_NOT_OK;
    lUdpGrpSocketId_uo              = SOAD_CFG_SOCKET_ID_DEFAULT_VALUE;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo                = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst          = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* get the UDP group socket id for the group */
    lUdpGrpSocketId_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].groupTcpIpSocketId_puo);

    /* Check if already a Group socket Id is assigned for this group, If Group socket id is not assigned, then
     * MainFunction shall take care about how to proceed further. like calling TcpIp_SoAdGetSocket, TcpIp_Bind etc.*/

    /* condition check: whether UDP group socket id is already created */
    if( SOAD_NUM_TCPIP_SOCKETS > lUdpGrpSocketId_uo )
    {
        /* if UDP group socket id is already created, then get the dynamic index mapped to listen socket id from SoAd_TcpIpSocHandleToDynSockIdx_auo[] array */
        lIdxUdpGroupDynSocket_uo = SoAd_TcpIpSocHandleToDynSockIdx_auo[lUdpGrpSocketId_uo];

        /* set the flag to avoid the new socket creation in the caller function */
        lFunctionRetVal_en = E_OK;

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00639]: Step (2)*******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /***        In case the UDP socket is already assigned to the socket connection group:                        ***/
        /***        (a) Activate the socket connection for communication via the shared UDP socket of                 ***/
        /***                the socket connection group                                                               ***/
        /****************************************************************************************************************/

        /* if socConSubMode_en is any one of the below state, the socket can goes to SOAD_SOCON_TCPWAITFORREMOTE state */
        switch(SoAd_DyncSocConfig_ast[lIdxUdpGroupDynSocket_uo].socConSubMode_en)
        {
            case SOAD_SOCON_BOUND:
            case SOAD_SOCON_ACTIVE:
            {
                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();

                /* Load the Local port value from previously opened server socket */
                lSoAdDyncSocConfig_pst->soConDynLocalPort_u16 = SoAd_DyncSocConfig_ast[SoAd_TcpIpSocHandleToDynSockIdx_auo[lUdpGrpSocketId_uo]].soConDynLocalPort_u16 ;

                 /* check about array index of the array is sockethandle give by tcpip layer and element of the array is dyanamic index */
                SoAd_TcpIpSocHandleToDynSockIdx_auo[lUdpGrpSocketId_uo] = lIdxDynSocket_uo;

                /* Update the dynamic socket table content with TcpIp socket Id */
                lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo = lUdpGrpSocketId_uo;

                /* Set the connection request state to socket open request sent */
                lSoAdDyncSocConfig_pst->socConSubMode_en  = SOAD_SOCON_BOUND;

                SchM_Exit_SoAd_SocConModeProperties();

                /* call a function which invokes opening of a udp socket*/
                SoAd_ActivateUdpSocket(StaticSocketId_uo);

                break;
            }

            default:
            {
                /* Nothing to do */
                break;
            }

        }
    }

    return lFunctionRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_UdpGroupSocketTcpIpClose()
 *
 * \Function description
 * This service shall close the UDP Group socket by calling TcpIp_Close api
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * \return  Std_ReturnType
 * \retval   E_OK       - UDP socket closed successfully
 *           E_NOT_OK   - UDP socket not closed
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_UdpGroupSocketTcpIpClose( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* variable to store the return value */
     Std_ReturnType              lFunctionRetVal_u8;

    /* Variable to hold the other active sockets availability */
    boolean                      lOtherActiveSocketPresent_b;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxUdpGroupDynSocket_uo;

    /*Initialization of variables */
    lFunctionRetVal_u8            = E_NOT_OK;
    lOtherActiveSocketPresent_b   = TRUE;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst      =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* Get the dynamic socket index for the matched Udp Group socket id */
    lIdxUdpGroupDynSocket_uo    = SoAd_TcpIpSocHandleToDynSockIdx_auo[lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo];

    /* If socket matched to Udp Group socket via SoAd_TcpIpSocHandleToDynSockIdx_auo[] and that socket is requested to close,
     * then SoAd has to loop through all other sockets in the group and fetch the other active socket if there and
     * assign it to in the same array SoAd_TcpIpSocHandleToDynSockIdx_auo[] for future access.
     */
    if( lIdxUdpGroupDynSocket_uo == lIdxDynSocket_uo )
    {
        /* check if any other socket in the same group is being active,
         * if so please get the cmatched dynamic socket index of the active static socket */
        lOtherActiveSocketPresent_b = SoAd_GetOtherActiveDynSocketForGroup(StaticSocketId_uo, &lIdxDynSocket_uo);
    }
    else
    {
        /* if already matched socket to UDP Group socket array is not requested to close, then same index will be assigned to retain the value */
        lIdxDynSocket_uo = lIdxUdpGroupDynSocket_uo;
    }

    /* If UDP Group socket is mapped to some other sockets, then simply reset the data structure without closing the group socket.
     * this group socket will not be closed untill all the SoAd sockets are closed, it will be closed when closing the last SoAd socket.
     */
    if( TRUE == lOtherActiveSocketPresent_b )
    {
        /* Assume 2 sockets(soc1 and soc2) are opened successfully, now SoAd_TcpIpSocHandleToDynSockIdx_auo[listen]
         * array will pointing to soc2's dynamic index.
         * when we get the close request from UL for soc2(last opened socket) and if we reset the
         * SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array, then linking willbe destroyed, afterwards we can't use the
         * SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array for future access.
         *
         * So, whenever we are closing the socket which is pointed by SoAd_TcpIpSocHandleToDynSockIdx_auo[listen] array,
         * SoAd has to reassign the dynamic index which is still open/active.
         */
        SoAd_TcpIpSocHandleToDynSockIdx_auo[lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo] = lIdxDynSocket_uo;

        /* Go to the state READYTOCLOSE mode */
        lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_READYTOCLOSE;

        /* Set the lFunctionRetVal_u8 to E_OK, so that socket will be closed in the same main function */
        lFunctionRetVal_u8 = E_OK;
    }
    else
    {
        /* call TcpIp to close the shared socket */
        lFunctionRetVal_u8 = TcpIp_Close( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                          lSoAdDyncSocConfig_pst->statusAbort_b );
    }

    return lFunctionRetVal_u8;
}
#endif/*SOAD_UDP_GRP_PRESENT != STD_OFF*/

#if (SOAD_TCP_CLIENT_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ActivateTcpClientSocket()
 *
 * \Function description
 * This service shall open a tcp client socket
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ActivateTcpClientSocket(SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the retval  */
    Std_ReturnType               lRetVal_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Variable to store if the remote IP address of dynamic socket has wildcard value */
    boolean                      lDyncSocConfigHasWildcardIpAddr_b;

    /* Variable to store the port for dynamic socket */
    uint16                       lDyncSocPort_u16;

    /* Structure to store the remote address information (if it is an IPv4 address) */
    TcpIp_SockAddrInetType       lRemoteSockAddrInet_st;

    /* Structure to store the remote address information (if it is an IPv6 address) */
    TcpIp_SockAddrInet6Type      lRemoteSockAddrInet6_st;

    /* Pointer to the structure holding the remote address */
    TcpIp_SockAddrType *         lRemoteSockAddr_pst;

    /*Initialization of variables */
    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo             = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst       = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    lDyncSocConfigHasWildcardIpAddr_b   = SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdDyncSocConfig_pst);
    lDyncSocPort_u16                    = SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG(lSoAdDyncSocConfig_pst);

    /* Check if Remote address is valid or not,
     * We should have a valid remote address (NO wild card is allowed) to initiate with remote for 3-way hand shake */
    if(( TCPIP_PORT_ANY != lDyncSocPort_u16 ) && ( !lDyncSocConfigHasWildcardIpAddr_b ))
    {
        /* Load the remoteAddr structure values from the dynamic socket */
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_BuildSockAddr( lSoAdDyncSocConfig_pst,
                              SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].domain_en,
                              ((TcpIp_SockAddrInetType *) &lRemoteSockAddrInet_st),
                              ((TcpIp_SockAddrInet6Type *) &lRemoteSockAddrInet6_st),
                              &lRemoteSockAddr_pst );

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00590]: Step (4)*******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** In case of a TCP socket initiate the TCP connection by calling TcpIp_TcpConnect().                       ***/
        /****************************************************************************************************************/

        /* client : call TcpIp_TcpConnect for connecting the socket  */
        lRetVal_u8 = TcpIp_TcpConnect( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                       lRemoteSockAddr_pst );

        /* Condition check: try to open the socket connection in next main function, if retrun value is NOT_OK */
        if( E_OK == lRetVal_u8 )
        {
            /* Update the socConStatus with socket waiting for remote to respond, means
             * SoAd is initiated establishment of socket connection, and it is waiting for the remote to respond. */
            lSoAdDyncSocConfig_pst->socConSubMode_en      = SOAD_SOCON_TCPWAITFORREMOTE ;
        }

    }


    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00686] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** Within SoAd_MainFunction() and after successfully performing the open actions, SoAd shall change         ***/
    /*** the state of the socket connection to SOAD_SOCON_RECONNECT in case of                                    ***/
    /***            (1) a TCP socket connection                                                                   ***/
    /****************************************************************************************************************/

    lSoAdDyncSocConfig_pst->socConMode_en = SOAD_SOCON_RECONNECT;
    /* call a function which shall invoke the upper layer call back for mode change */
    SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );

}
#endif  /*SOAD_TCP_CLIENT_PRESENT != STD_OFF*/

#if (SOAD_UDP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ActivateUdpSocket()
 *
 * \Function description
 * This service shall open a udp socket
 * It has no return value .
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of static socket connection table.
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ActivateUdpSocket( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

#if (SOAD_TP_PRESENT != STD_OFF)
    /* Variable to store the index of RxPduTable */
    PduIdType                    lIdxRxPdu_uo;
#endif  /*SOAD_TP_PRESENT != STD_OFF*/

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Variable to store if the remote IP address of dynamic socket has wildcard value */
    boolean                      lDyncSocConfigHasWildcardIpAddr_b;

    /* Variable to store the port for dynamic socket */
    uint16                       lDyncSocPort_u16;

    /*Initialization of variables */
    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo         = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst   =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
    SchM_Enter_SoAd_RemoteAddrUpdate();

    lDyncSocConfigHasWildcardIpAddr_b   = SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdDyncSocConfig_pst);
    lDyncSocPort_u16                    = SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG(lSoAdDyncSocConfig_pst);

    SchM_Exit_SoAd_RemoteAddrUpdate();

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00591] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** Within SoAd_MainFunction() and after successfully performing the open actions, SoAd shall change the     ***/
    /*** state of the socket connection to SOAD_SOCON_ONLINE in case of a UDP socket and either                   ***/
    /*** SoAdSocketUdpListenOnly is set to TRUE or a remote address is set by a value that does not contain       ***/
    /***                                any wildcards                                                             ***/
    /****************************************************************************************************************/

    /* If the remote address is set by a value that does not contain any wildcards and NULL (means Valid address)
     *  or SoAdSocketUdpListenOnly is set to TRUE , socket shall go to online */
    if( ( STD_ON == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].udpProtocolConf_cpst->listenOnlyUDP_u8 ) ||
        ( !lDyncSocConfigHasWildcardIpAddr_b && ( TCPIP_PORT_ANY != lDyncSocPort_u16 ) ) )
    {
        /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
        SchM_Enter_SoAd_SocConModeProperties();

        /*  SoAd shall inform all upper layers related to the socket connection */
        /* change the sub state of the socket connection
         * 1 socket supports 1 upper layer. Multiple upper layers per socket is not supported.*/
        lSoAdDyncSocConfig_pst->socConSubMode_en  = SOAD_SOCON_ACTIVE;

        lSoAdDyncSocConfig_pst->socConMode_en     = SOAD_SOCON_ONLINE;

        SchM_Exit_SoAd_SocConModeProperties();

        /* call a function which shall invoke the upper layer call back for mode change */
        SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_ONLINE );

#if (SOAD_TP_PRESENT != STD_OFF)
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00595] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** For socket connection with PDU Header mode disabled (SoAdPduHeaderEnable = FALSE) and an upper layer     ***/
        /*** with TP-API, SoAd shall call <Up>_[SoAd][Tp]StartOfReception() with TpSduLength = 0 at the end of        ***/
        /***                                the connection setup                                                      ***/
        /****************************************************************************************************************/

        /* If header is not enabled for the socket connection group, call the UL startOfReception as per SWS_SoAd_00595*/
        /* The start of reception shall be called for upper layer of a socket only if the type is TP,
         * If Socket id of the received Rx pdu is exactly match with socket if configured for the pdu,
         * This is a valid condition if same Rx pdu is mapped to different socket and Rx pdu's are not aligned sequencially to single socket*/

        if( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].headerEnable_b == FALSE ) &&
            ( 1U == SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].numberOfRxPdusConfigured_u8 ) )
        {
            /* get the first RxPduId mapped to socket */
            lIdxRxPdu_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].rxPduIds_pcuo[0];

            if( SOAD_TP == SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[lIdxRxPdu_uo].rxUpperLayerType_en )
            {
                /* call start of reception related to all Rx pdus*/
                SoAd_InformTpStartOfReception(StaticSocketId_uo, lIdxRxPdu_uo);
            }
        }
#endif  /*SOAD_TP_PRESENT != STD_OFF*/

    }
    else
    {

        lSoAdDyncSocConfig_pst->socConMode_en = SOAD_SOCON_RECONNECT;

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00686] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** Within SoAd_MainFunction() and after successfully performing the open actions, SoAd shall change         ***/
        /*** the state of the socket connection to SOAD_SOCON_RECONNECT in case of                                    ***/
        /***     (2) a UDP socket connection that is configured with a remote address containing wildcards            ***/
        /****************************************************************************************************************/

        /* call a function which shall invoke the upper layer call back for mode change, Socket shall goes to RECONNECT mode
         * and tries to go to ONLINE in next main function/Rx_Indication  */
        SoAd_ModeChgCallbackToUl(StaticSocketId_uo, SOAD_SOCON_RECONNECT );
    }

}
#endif  /*SOAD_UDP_PRESENT != STD_OFF*/

#if (SOAD_TP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessTpReq()
 *
 * \Function description
 *  This function shall execute the TP transmission and reception processing for each Tx Pdu's
 *  Note: This is an internal function shall be called from SoAd_MainFunction().
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of SoAd Static Socket identifier Table
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessTpReq( SoAd_SoConIdType StaticSocketId_uo )
{
    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* --------------------------------------------- */
    /* TP transmission                               */
    /* --------------------------------------------- */

    /* Case 1:When SoAd_TpTransmit is called by upper layer, SoAd shall store the request and process it main function
     * So, here SoAd shall check any pending transmissions for this socket and start the TpTransmit activties
     *
     * Case 2: If already TP transmission request is started in last main functions, but not completed.
     * Then SoAd shall check, is there any confirmation for previous data transmission and still some data is pending
     * to be transmitted, then again SoAd shall continue the TpTransmit activities */

    /* Check remaining length > 0, else all the data has been triggered already */
    if( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTxRemainingLength_uo > 0U )
    {
        /* [SWS_SoAd_00552] Perform the TP transmit requested by UL */
        SoAd_ProcessTransmissionReq( StaticSocketId_uo );
    }

#if ( SOAD_RX_BUFF_ENABLE != STD_OFF )
    /* --------------------------------------------- */
    /* TP reception from buffer to UL                */
    /* --------------------------------------------- */

    /* If Rx buffer is configured */
    if( SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].dynRxBuffTableEntry_pst != NULL_PTR )
    {
        /* Call a function to execute the cyclic processing of TP data transfer from Rx buffer to UL */
        /* (function is called with FALSE because it is not required to stop further TP sessions) */
        SoAd_CopyTpDataFromRxBufferToUl( StaticSocketId_uo, FALSE );
    }
#endif

    return;
}


/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessTransmissionReq()
 *
 * \Function description
 *  This function shall process the tranmission requests which are stored in SoAd_TpTransmit() function for each Tx Pdu's
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - Index of SoAd Static Socket identifier Table
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessTransmissionReq( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to store the retval of Ul_copyTxDataCallback */
    BufReq_ReturnType            lRetVal_copyTxDataCbk_en;

    /* Index of TxPduConfig table */
    PduIdType                    lIdxTxPdu_uo;

    /* structure to store destination buffer and the number of bytes to copy*/
    PduInfoType                  lPduInfo_st;

    /* variable to store the available data length at the upper layer in bytes*/
    PduLengthType                lAvailableLength_uo;

    /* variable to store txpduid */
    PduIdType                    lUlTxPduId_uo;

    /* Indicates the remaining number of bytes that are available in the UL Tx buffer */
    PduLengthType                lAvailUlDataLen_uo;

    /* variable to store the info of TpTransmit status*/
    boolean                      lTpTransmitError_b;

    /* To store the Pdu Header size, 0 if header disbaled and 8 if header enabled*/
    uint8                        lPduHdrSize_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
    /* variable to store the return value */
    Std_ReturnType               lRetVal_u8;
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

    /*Initialization of local variables which are declared above */
    lRetVal_copyTxDataCbk_en      = BUFREQ_E_NOT_OK;
    lPduInfo_st.SduLength         = 0;
    lPduInfo_st.SduDataPtr        = NULL_PTR;
    lAvailUlDataLen_uo            = 0;
    lTpTransmitError_b            = FALSE;
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
    lRetVal_u8                    = E_OK;
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo              = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    /* Extract pointer to Dynamic structrue for given index */
    lSoAdDyncSocConfig_pst        =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo               = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /*Get the index of the TxPdu table */
    lIdxTxPdu_uo =  lSoAdDyncSocConfig_pst->idxTxPduTable_uo ;

    /* Get the upper layer TxPdu Id from TxPduConfig table */
    lUlTxPduId_uo = SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo].ulTxPduId_uo ;

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00552]: Step (1) ******************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*************** Query the available amount of data at the upper layer by calling the configurable **************/
    /************** callback function <Up>_[SoAd][Tp]CopyTxData() with PduInfoType.SduLength = 0. *******************/
    /****************************************************************************************************************/

    /*SoAd shall call the upper layer to get the available data for this transmission, Then SoAd shall call TcpIpLayer for Tx
     * This shall be done by giving lPduInfo_st.SduLength   = 0 in upper layer call back */

    /* Condition check: call back in the upper layer should be configured */
    if(NULL_PTR != SoAd_CurrConfig_cpst->SoAdTp_copyTxDataCallbacks_cpst[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo].idxTpCopyTxDataCallback_u8].soAdCopyTxDataCbk_pfn )
    {
        /* call to upper layer to get the data to be passed to TCP layer */
        lRetVal_copyTxDataCbk_en = SoAd_CurrConfig_cpst->SoAdTp_copyTxDataCallbacks_cpst[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo].idxTpCopyTxDataCallback_u8].soAdCopyTxDataCbk_pfn \
                                                                (lUlTxPduId_uo,(const PduInfoType*) &lPduInfo_st, NULL_PTR, &lAvailUlDataLen_uo );
    }

    /* Check if CopyTxData is returned OK and Upper layer has some buffer to copy the data */
    if( ( BUFREQ_OK == lRetVal_copyTxDataCbk_en ) &&
        ( 0 < lAvailUlDataLen_uo ) )
    {
        /* check if header is enabled and no packet has been transmitted so far */
        /*set '8' bytes to variable, If header is disabled then this variabled holds '0' value in it */
        lPduHdrSize_u8 = ( ( FALSE != SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].headerEnable_b ) && \
                           ( FALSE == lSoAdDyncSocConfig_pst->ifTpTxSoAdHdrCopied_b ) ) ? SOAD_PDUHDR_SIZE : 0U;

        /* Get the least value which we can be able to fetch from UL_CopyTxData() function */
        lAvailableLength_uo = SoAd_Minimum((lAvailUlDataLen_uo + lPduHdrSize_u8 ), lSoAdDyncSocConfig_pst->tpTxRemainingLength_uo);

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00552]: Step (2) ******************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /********* Depending on the connection type: retrieve data and call the related TcpIp transmit function *********/
        /****************************************************************************************************************/
        switch( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en )
        {

#if ( SOAD_TP_TCP_PRESENT != STD_OFF )
            /* The protocol is TCP */
            case TCPIP_IPPROTO_TCP:
            {
                /* Call the SoAd_TpTcpTransmit() internal function which shall call TcpIp_TcpTransmit() api to transmit the PDU data */
                SoAd_TpTcpTransmit(StaticSocketId_uo, lAvailableLength_uo, &lTpTransmitError_b);

                break;
            }
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

#if ( SOAD_TP_UDP_PRESENT != STD_OFF )
            /* The protocol is UDP */
            case TCPIP_IPPROTO_UDP:
            {
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
                if( SOAD_CFG_TP_UDP_TXBUFFER_DEFAULT_VALUE > SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].tpUdpTxBuffIdx_uo )
                {
                    /****************************************************************************************************************/
                    /****************************************** [SWS_SoAd_00553]: Step (1) ******************************************/
                    /****************************************************************************************************************/
                    /* PDU data will be loaded to TP UDP Transmit buffer from Ul */
                    lRetVal_u8 = SoAd_CopyPduDataToTpUdpTransmitBuffer(StaticSocketId_uo, lAvailableLength_uo, lPduHdrSize_u8, &lTpTransmitError_b);

                    lAvailableLength_uo = lSoAdDyncSocConfig_pst->tpTxTotPduLen_uo;
                }


                if(E_OK == lRetVal_u8)
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */
                {
                    /* Call the SoAd_TpUdpTransmit() internal function which shall call TcpIp_UdpTransmit() api to transmit the PDU data */
                    SoAd_TpUdpTransmit(StaticSocketId_uo, lAvailableLength_uo, &lTpTransmitError_b);
                }

                break;
            }
            /* If the configured protocol is not UDP or NOT TCP */
#endif /* SOAD_TP_UDP_PRESENT != STD_OFF */

            default:
                /* Set lTpTransmitError_b value to TRUE for further processing */
                lTpTransmitError_b = TRUE;
                break;

        }/*End of switch: condition check for protocol */
    }
    else
    {
        /* Set lTpTransmitError_b value to TRUE for further processing */
        lTpTransmitError_b = TRUE;

#if ( SOAD_TP_TCP_PRESENT != STD_OFF )
        /* If the protocol is TCP : SWS_SoAd_00651: socket shall be closed */
        if(( BUFREQ_E_NOT_OK == lRetVal_copyTxDataCbk_en )  &&
           ( TCPIP_IPPROTO_TCP == (SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en) ))
        {
            /****************************************************************************************************************/
            /****************************************** [SWS_SoAd_00651]: Step(a)********************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /****************** SoAd shall disable further transmission or reception for this socket connection *************/
            /****************************************************************************************************************/

            /* set the global variables to skip the further transmission and reception */
            lSoAdDyncSocConfig_pst->discardNewTxRxReq_b  = TRUE;

            /****************************************************************************************************************/
            /****************************************** [SWS_SoAd_00651]: Step(b)********************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /****************** SoAd shall close the socket connection in the next SoAd_MainFunction() **********************/
            /****************************************************************************************************************/

            /* set the state such that, the socket shall be closed in next main function */
            lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_OFFLINEREQ_SOAD ;
        }
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */
    }/* End for: if( lRetVal_copyTxDataCbk_en == BUFREQ_OK && lAvailUlDataLen_uo != 0 )*/

    /*Condition check : As error occurred while copying the data */
    if( FALSE != lTpTransmitError_b )
    {
        /****************************************************************************************************************/
        /********************************* [SWS_SoAd_00651] && [SWS_SoAd_00652] *****************************************/
        /****************************************************************************************************************/

        /* If the protocol is UDP : SWS_SoAd_00652: the related socket connection is not closed in this case*/
        /* If the protocol is TCP : SWS_SoAd_00651: the related socket connection shall be closed */
        /* If the protocol is TCP : SWS_SoAd_00670: the related socket connection is not closed in this case*/
        /* call a function which resets all dynamic table parameters related to TP and inform upper layer */
        SoAd_ResetTpTxDyncParameters(lIdxDynSocket_uo, lIdxTxPdu_uo, E_NOT_OK);
    }

    /********************************************************************************************************/
    /*****************************  End: process of transmission of any pending TP request *****************/
    /********************************************************************************************************/
}

#if ( SOAD_TP_TCP_PRESENT != STD_OFF )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_TpTcpTransmit()
 *
 * \Function description
 * This function shall call the TcpIp_TcpTransmit() api to tranmit the PDU to lower layer and update the globals tables accordingly
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier Table
 * \param   PduLengthType
 * \arg     AvailableLength_uo  - No of bytes to be transmitted
 *
 * Parameters (inout):
 * \param   boolean *
 * \arg     TpTransmitError_pb  - TRUE:  If any error ocurred after calling the lower layer function
 *                                FALSE: No error occured, requested packet transmitted successfully
 *
 * Parameters (out):    None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_TpTcpTransmit( SoAd_SoConIdType StaticSocketId_uo,
                                PduLengthType    AvailableLength_uo,
                                boolean *        TpTransmitError_pb)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *         lSoAdDyncSocConfig_pst ;

    /* variable to store the return value of TcpIp_TcpTransmit function */
    Std_ReturnType                       lRetVal_TcpTransmit_u8;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType                     lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType                     lIdxSoConGroup_uo;

    /* Variable to store if the remote IP address of static socket has a wildcard value */
    boolean                              lStaticSocConfigHasWildcardIpAddr_b;

    /* Variable to store the port for static socket */
    uint16                               lStaticSocPort_u16;

    /*Initialization of variables */
    lRetVal_TcpTransmit_u8 = E_NOT_OK;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst       = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo                = (*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo);

    /* copy the global array index address to local pointer for further processing */
    lSoAdDyncSocConfig_pst          =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo               = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    lStaticSocConfigHasWildcardIpAddr_b = SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdStaticSocConfig_cpst);
    lStaticSocPort_u16                  = SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG(lSoAdStaticSocConfig_cpst);

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00554] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** In case of a TCP socket connection the SoAd shall call TcpIp_TcpTransmit() with SocketId specified in the ***/
    /**  SocketConnection, the PDU length set to the value returned by availableDataPtr of the previous call to   ***/
    /**             <Up>_[SoAd][Tp]CopyTxData() as AvailableLength and ForceRetrieve set to FALSE.                ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /* ForceRetrieve : Indicates how the TCP/IP stack retrieves data from SoAd.
     * FALSE: The TCP/IP stack shall retrieve the whole data indicated by lAvailableLength_uo from the upper layer
     *      via one or multiple SoAd_CopyTxData() calls within the context of this transmit function.
     * TRUE: The TCP/IP stack may retrieve up to lAvailableLength_uo data from the upper layer.
     *      It is allowed to retrieve less than lAvailableLength_uo bytes.
     *  Note: Not retrieved data will be provided by SoAd
     *      with the next call to TcpIp_TcpTransmit (along with new data if available). */

    lRetVal_TcpTransmit_u8 = TcpIp_TcpTransmit( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo, NULL_PTR, (uint32)AvailableLength_uo, SOAD_FORCE_RETRIEVE_TRANSMISSION_FALSE );

    /* If the TcpIp_TcpTransmit is OK */
    if( E_OK == lRetVal_TcpTransmit_u8 )
    {
        /*Reset the retry counter*/
        lSoAdDyncSocConfig_pst->tpTxTcpTransmitRetryCnt_u16 = 0;

        /* Check entire requested packet has been transmitted */
        if( 0 == lSoAdDyncSocConfig_pst->tpTxRemainingLength_uo )
        {
            /* If all the requested bytes has been transmitted, set tpTransmitRequest_b to FALSE */
            lSoAdDyncSocConfig_pst->tpTransmitRequest_b = FALSE;

            /* If the transmission is successful and socket is Automatic, do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
            if( ( TRUE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo].soConGrpAutoConSetup_b ) &&
                ( ( TCPIP_PORT_ANY == lStaticSocPort_u16 ) || lStaticSocConfigHasWildcardIpAddr_b ) )
            {
                /* call the internal function to do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
                SoAd_PerformPostPduTransmissionAction(StaticSocketId_uo);
            }
        }

        /*SWS_SoAd_00667*/
        /* condition check: Immediate Tcp Tx confirmation is requested for TP, which means No need to wait till the
         * confirmations given by the TcpIp Layer. SoAd it self take a decision depending upon the data copied to TcpIp
         * layer
         */
        if( STD_ON == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].tcpProtocolConf_cpst->tcpImmediateTpTxConfirm_u8 )
        {
            /****************************************************************************************************************/
            /****************************************** [SWS_SoAd_00667] ****************************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /** In case of a TCP socket connection configured with SoAdSocketTcpImmediateTpTxConfirmation set to TRUE the ***/
            /** SoAd shall call the upper layer with the configured transmit confirmation function and E_OK as result     ***/
            /*** withinthe SoAd_MainFunction() after TcpIp_TcpTransmit() returns E_OK.                                    ***/
            /****************************************************************************************************************/
            /****************************************************************************************************************/

            /* call a function to do the calculations of remaining length and inform to upper layer*/
            SoAd_TpTxConfirmationToUl(StaticSocketId_uo, lSoAdDyncSocConfig_pst->idxTxPduTable_uo, lSoAdDyncSocConfig_pst->tpTcpCopyTxDataLength_uo );
        }
    }
    else
    {
        /* Condition Check:
         * 1. Check if E_NOT_OK is due to buffer full
         * 2. Check if the number of retries is configured for infinite attempts or it has not reached the configured limit.
         */
        if( ( FALSE == lSoAdDyncSocConfig_pst->tpTxTcpBufferAvailable_b ) && \
            ( (SOAD_NUM_TP_TCP_RETRY_CNT == 65535) || (lSoAdDyncSocConfig_pst->tpTxTcpTransmitRetryCnt_u16 < SOAD_NUM_TP_TCP_RETRY_CNT) ) )
        {
            /*Clear the buffer full flag. It will be again updated in the next retry if buffer is full*/
            lSoAdDyncSocConfig_pst->tpTxTcpBufferAvailable_b = TRUE;

            /*Increment the retry counter*/
            lSoAdDyncSocConfig_pst->tpTxTcpTransmitRetryCnt_u16++;
        }
        else
        {
            /* Set *TpTransmitError_pb value to TRUE for further processing */
            *TpTransmitError_pb = TRUE;
        }
    }/*End of : if(retVal == E_OK)*/

}
#endif /* SOAD_TP_TCP_PRESENT != STD_OFF */

#if ( SOAD_TP_UDP_PRESENT != STD_OFF )
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_CopyPduDataToTpUdpTransmitBuffer()
 *
 * \Function description
 * This function shall call the copy all the UL PDu data to TP UDP Transmit Buffer.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier Table
 * \param   PduLengthType
 * \arg     AvailableLength_uo  - No of bytes to be transmitted
 * \param   uint8
 * \arg     PduHdrSize_u8       - SoAd Pdu Header length
 *                                8 - If header is enabled
 *                                0 - If header is diabled
 *
 * Parameters (inout):
 * \param   boolean *
 * \arg     TpTransmitError_pb  - TRUE:  If any error ocurred after calling the lower layer function
 *                                FALSE: No error occured, requested packet transmitted successfully
 *
 * Parameters (out):    None
 *
 * \return  Std_ReturnType
 * \retval   E_OK             - UL returns E_OK
 *           E_NOT_OK         - UL returns E_NOT_OK
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_CopyPduDataToTpUdpTransmitBuffer( SoAd_SoConIdType StaticSocketId_uo,
                                                             PduLengthType    AvailableLength_uo,
                                                             uint8            PduHdrSize_u8,
                                                             boolean *        TpTransmitError_pb )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *         lSoAdDyncSocConfig_pst ;

    /* Local pointer to store the Tx Pdu table for given index */
    const SoAd_TxPduConfigType_tst *     lSoAdTxPduConfig_cpst ;

    /* Local pointer to store the TP UDP Transmit Buffer array */
    uint8 *                              lSoAdTpUdpTxSocketBuffer_pu8;

    /* structure to store destination buffer and the number of bytes to copy*/
    PduInfoType                          lPduInfo_st;

    /* variable to store the return value */
    Std_ReturnType                       lFunctionRetVal_u8;

    /* Index of TxPduConfig table */
    PduIdType                            lIdxTxPdu_uo;

    /* Variable to store the retval of Ul_copyTxDataCallback */
    BufReq_ReturnType                    lRetVal_copyTxDataCbk_en;

    /* parameter is used to retransmit data because problems occurred in transmitting it the last time */
    RetryInfoType                        lRetry_st;

    /* Indicates the remaining number of bytes that are available in the UL Tx buffer */
    PduLengthType                        lAvailUlDataLen_uo;

    /* Index of TP UDP Tx Buffer array to which PDU data shoule be loaded */
    PduLengthType                        lTpUdpTxBufferIndex_uo;

    /* PDU data length exluding the header size */
    PduLengthType                        lTotalDataLen_uo;

    /* Initialize the local variables which are declared above */
    lFunctionRetVal_u8       = E_NOT_OK;
    lRetVal_copyTxDataCbk_en = BUFREQ_OK;
    lTpUdpTxBufferIndex_uo    = 0;
    lAvailUlDataLen_uo       = AvailableLength_uo;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* copy the global array index address to local pointer for further processing */
    lSoAdDyncSocConfig_pst    = &(SoAd_DyncSocConfig_ast[(*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo)]);

    /*Get the index of the TxPdu table */
    lIdxTxPdu_uo =  lSoAdDyncSocConfig_pst->idxTxPduTable_uo ;

    /* Get the Tx Pdu pointer for given index */
    lSoAdTxPduConfig_cpst           = &(SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo]);

    /* Load the TP UDP transmit buffer to local pointer for further use */
    lSoAdTpUdpTxSocketBuffer_pu8 = (SoAd_CurrConfig_cpst->SoAdTp_UdpTransmitBufferInfo_cpu8)[lSoAdStaticSocConfig_cpst->tpUdpTxBuffIdx_uo].tpUdpTxBuffer_pu8;

    /* compute the PDU data length by subtracting the header length */
    lTotalDataLen_uo = lSoAdDyncSocConfig_pst->tpTxTotPduLen_uo - PduHdrSize_u8;

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00553]: Step (1) ******************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*****************************In case of a UDP socket connection the SoAd shall**********************************/
    /**** retrieve all available data from the upper layer to a SoAd TP transmit buffer via the configurable    *****/
    /**** callback function <Up>_[SoAd][Tp]CopyTxData() with PduInfoType.SduLength set to the value returned by *****/
    /**************************** availableDataPtr of the previous call and *****************************************/
    /****************************************************************************************************************/

    /* Loop through until all PDU data requested at SoAd_TpTransmit( ) is copied to TP UDP Transmit Buffer,
     * Suppose if any error returned by UL_CopyTxData callback function, simply break the loop */
    while( ( lTpUdpTxBufferIndex_uo < lTotalDataLen_uo ) && \
           ( 0 < lAvailUlDataLen_uo ) && \
           ( BUFREQ_OK == lRetVal_copyTxDataCbk_en ) )
    {
        /* Load the lPduInfo_st.SduLength with UL available length which was retrieved from previous UL_CopyTxData and
         * lPduInfo_st.SduDataPtr to the corresponding pointer
         */
        lPduInfo_st.SduDataPtr  = &(lSoAdTpUdpTxSocketBuffer_pu8[lTpUdpTxBufferIndex_uo]);
        lPduInfo_st.SduLength   = lAvailUlDataLen_uo;

        /* Reset the below parameters before calling the UL callback function for TP Tx */
        lRetry_st.TxTpDataCnt       = 0;
        lAvailUlDataLen_uo          = 0;

        /* call to upper layer to get the data to be passed to TCP layer */
        lRetVal_copyTxDataCbk_en = SoAd_CurrConfig_cpst->SoAdTp_copyTxDataCallbacks_cpst[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo].idxTpCopyTxDataCallback_u8].soAdCopyTxDataCbk_pfn \
                                                                (lSoAdTxPduConfig_cpst->ulTxPduId_uo,(const PduInfoType*) &lPduInfo_st, &lRetry_st, &lAvailUlDataLen_uo );

        /* Check if UL_CopyTxData is returned BUFREQ_OK and Upper layer has some buffer to copy the data */
        if( BUFREQ_OK == lRetVal_copyTxDataCbk_en )
        {
            /* load the lTpUdpTxBufferIndex_uo to the copied length, so that it can be used to terminate if all data's
             * are copied, or else it can be used to adjust the buffer for next UL_CopyTxData callback
             */
            lTpUdpTxBufferIndex_uo += lPduInfo_st.SduLength;
        }
    }

    /* If all data's are not copied to TP UDP Tx Buffer, set the TpTransmitError_pb to TRUE to provide the
     * Ul_TxConfirmation callback with E_NOT_OK at caller function.
     */
    if( (lTpUdpTxBufferIndex_uo < lTotalDataLen_uo) && \
        ( BUFREQ_OK != lRetVal_copyTxDataCbk_en ) )
    {
        *TpTransmitError_pb = TRUE;
    }

    /* If Buffer is not availble in UL to copy it to SoAd buffer, then SoAd shall return E_NOT_OK,
       So that it can be tried in next MainFunction */
    lFunctionRetVal_u8 = ( ( BUFREQ_OK != lRetVal_copyTxDataCbk_en ) || \
                           ( (lTpUdpTxBufferIndex_uo < lTotalDataLen_uo ) && \
                             ( 0 == lAvailUlDataLen_uo ) ) )? E_NOT_OK : E_OK;

    return lFunctionRetVal_u8;
}
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET > 0 */

/**
 ***************************************************************************************************
 * \Function Name : SoAd_TpUdpTransmit()
 *
 * \Function description
 * This function shall call the TcpIp_UdpTransmit() api to tranmit the PDU to lower layer and update the globals tables accordingly
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier Table
 * \param   PduLengthType
 * \arg     AvailableLength_uo  - No of bytes to be transmitted
 *
 * Parameters (inout):
 * \param   boolean *
 * \arg     TpTransmitError_pb  - TRUE:  If any error ocurred after calling the lower layer function
 *                                FALSE: No error occured, requested packet transmitted successfully
 *
 * Parameters (out):    None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_TpUdpTransmit( SoAd_SoConIdType StaticSocketId_uo,
                                PduLengthType    AvailableLength_uo,
                                boolean *        TpTransmitError_pb)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *         lSoAdDyncSocConfig_pst ;

    /* Variable to hold the return value of the TcpIp_UdpTransmit function call*/
    TcpIp_ReturnType                     lRetVal_UdpTransmit_en;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType                     lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType                     lIdxSoConGroup_uo;

    /* Structure to store the remote address information (if it is an IPv4 address) */
    TcpIp_SockAddrInetType               lRemoteSockAddrInet_st;

    /* Structure to store the remote address information (if it is an IPv6 address) */
    TcpIp_SockAddrInet6Type              lRemoteSockAddrInet6_st;

    /* Pointer to the structure holding the remote address */
    TcpIp_SockAddrType *                 lRemoteSockAddr_pst;

    /* Variable to store if the remote IP address of static socket has wildcard value */
    boolean                              lStaticSocConfigHasWildcardIpAddr_b;

    /* Variable to store the port for static socket */
    uint16                               lStaticSocPort_u16;

    /*Initialization of local variables */
    lRetVal_UdpTransmit_en   = TCPIP_E_NOT_OK;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo         = (*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo);

    /* copy the global array index address to local pointer for further processing */
    lSoAdDyncSocConfig_pst   =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo         = lSoAdStaticSocConfig_cpst->soConGrpId_uo ;

#if ( (SOAD_UDP_GRP_PRESENT != STD_OFF) && (SOAD_TP_UDP_PRESENT != STD_OFF) )
    if( 1 < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo )
    {
        lSoAdDyncSocConfig_pst->tpUdpTxTriggered_b = TRUE;
    }
#endif /* (SOAD_UDP_GRP_PRESENT != STD_OFF) && (SOAD_TP_UDP_PRESENT != STD_OFF) */

    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
    SchM_Enter_SoAd_RemoteAddrUpdate();

    /* Load the remoteAddr structure values from the dynamic socket */
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_BuildSockAddr( lSoAdDyncSocConfig_pst,
                          SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].domain_en,
                          ((TcpIp_SockAddrInetType *) &lRemoteSockAddrInet_st),
                          ((TcpIp_SockAddrInet6Type *) &lRemoteSockAddrInet6_st),
                          &lRemoteSockAddr_pst );

    SchM_Exit_SoAd_RemoteAddrUpdate();

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00553]: Step (2)  *****************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** call TcpIp_UdpTransmit() with SocketId and remote address specified in the SocketConnection and the PDU   ***/
    /** length specified in the SoAd_TpTransmit() call as TotalLength after all data have been successfully       ***/
    /**             retrieved within one or multiple SoAd main function execution cycles                          ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /* Call the TcpIp_UdpTransmit for Tx,
     * NULL_PTR is passed which shall invoke SoAd_CopyTxData cbk from TcpIp layer.
     * For UDP, SoAd shall give the remoteAddr of the socket connection.
     * If remoteAddr contains wild cards, then TcpIp layer shall take care about it */
    lRetVal_UdpTransmit_en = TcpIp_UdpTransmit( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                                 ( NULL_PTR),
                                                 lRemoteSockAddr_pst,
                                                 (uint16)AvailableLength_uo );

    if( TCPIP_OK == lRetVal_UdpTransmit_en )
    {
#if ( SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET == 0 )
        /* Check entire requested packet has been transmitted */
        if( 0 == lSoAdDyncSocConfig_pst->tpTxRemainingLength_uo )
#endif /* SOAD_NUM_TP_UDP_TX_BUFFER_SOCKET == 0 */
        {
            /* If all the requested bytes has been transmitted, set tpTransmitRequest_b to FALSE */
            lSoAdDyncSocConfig_pst->tpTransmitRequest_b = FALSE;

            lStaticSocConfigHasWildcardIpAddr_b = SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdStaticSocConfig_cpst);
            lStaticSocPort_u16                 = SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG(lSoAdStaticSocConfig_cpst);


            /* If the transmission is successful and socket is Automatic, do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
            if( (( TRUE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo].soConGrpAutoConSetup_b ) && \
                ( ( TCPIP_PORT_ANY == lStaticSocPort_u16 ) || lStaticSocConfigHasWildcardIpAddr_b )) || \
                ( SOAD_EVENT_RECONNECTREQ_SOAD == lSoAdDyncSocConfig_pst->socConRequestedMode_en) )
            {

                /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
                SchM_Enter_SoAd_TxRxReq();

                /* call the internal function to do as per SWS_SoAd_00582 and SWS_SoAd_00644 if no Rx ongoing*/
                if(lSoAdDyncSocConfig_pst->rxOngoingCnt_u8 == 0)
                {
                    /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                    SchM_Enter_SoAd_SocConModeProperties();

                    /* do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
                    /* Set the connection request state to reconnect */
                    lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_BOUND;

                    lSoAdDyncSocConfig_pst->socConMode_en    = SOAD_SOCON_RECONNECT;

                    SchM_Exit_SoAd_SocConModeProperties();

                    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
                    SchM_Enter_SoAd_RemoteAddrUpdate();

                    SoAd_CopyRemoteAddrFromStaticSocToDynSoc( ((const SoAd_StaticSocConfigType_tst *) \
                            &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])), lSoAdDyncSocConfig_pst);

                    SchM_Exit_SoAd_RemoteAddrUpdate();

                    lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = FALSE;

                    SchM_Exit_SoAd_TxRxReq();
                    /* call a function which shall invoke the upper layer call back for mode change */
                    SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

                }
                else
                {
                    lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = TRUE;
                    SchM_Exit_SoAd_TxRxReq();

                    /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                    SchM_Enter_SoAd_OpenCloseReq();

                    if((lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_UL) && \
                       (lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_SOAD) && \
                       (lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_TCPIP))
                    {
                        /* set requested mode to reconnect request SoAd to process post PDU Tx actions in main function */
                        lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_RECONNECTREQ_SOAD;
                    }

                    SchM_Exit_SoAd_OpenCloseReq();
                }
            }
        }

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00557] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** In case of a UDP socket connection the SoAd shall call the upper layer with the configured transmit       ***/
        /** confirmation function (<Up>_[SoAd][Tp]TxConfirmation>()) and E_OK as result within the SoAd_MainFunction()***/
        /**             after TcpIp_UdpTransmit() returns with TCPIP_OK                                               ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        /* call a function to do the calculations of remaining length and inform to upper layer */
        SoAd_TpTxConfirmationToUl(StaticSocketId_uo, lSoAdDyncSocConfig_pst->idxTxPduTable_uo, AvailableLength_uo );
    }
    else
    {
        /* Set *TpTransmitError_pb value to TRUE for further processing */
        *TpTransmitError_pb = TRUE;
    }

#if ( (SOAD_UDP_GRP_PRESENT != STD_OFF) && (SOAD_TP_UDP_PRESENT != STD_OFF) )
    if( 1 < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo )
    {
        lSoAdDyncSocConfig_pst->tpUdpTxTriggered_b = FALSE;
    }
#endif /* (SOAD_UDP_GRP_PRESENT != STD_OFF) && (SOAD_TP_UDP_PRESENT != STD_OFF) */

}
#endif /* SOAD_TP_UDP_PRESENT != STD_OFF */
#endif /* SOAD_TP_PRESENT != STD_OFF */

#if ( SOAD_TIMEOUT_HANDLING_ENABLED == STD_ON )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_TimeOutParamHandling()
 *
 * \Function description
 * This function shall perform the handlings for all TimeOut parameters
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier Table
 *
 * Parameters (inout): None
 *
 * Parameters (out):  None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_TimeOutParamHandling( SoAd_SoConIdType StaticSocketId_uo )
{
#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* variable to store the nPduUdpTxBuffer to local variable for further access */
    SoAd_nPduUdpTxInfo_tst * lnPduUdpTransmitInfo_pst;
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

#if ( SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0U )
    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType         lIdxDynSocket_uo;

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

    SchM_Enter_SoAd_UdpSupervisionTimer();

    if((0xFFFFFFFFU > SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].UdpAliveSupervisionTimer_u32) && \
       (0U < SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].UdpAliveSupervisionTimer_u32))
    {
        /* Decrement by 1 in every main function since UdpAliveSupervisionTimer_u32 is multiple of SoAd Main period */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].UdpAliveSupervisionTimer_u32--;
    }

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00695] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** If UDP socket connection is configured with SoAdSocketUdpAliveSupervisionTimeout and the alive           ***/
    /*** supervision timer runs out, SoAd shall                                                                   ***/
    /*** (a)  Change the state of socket connection to SOAD_SOCON_RECONNECT                                       ***/
    /*** (b)  deactive the alive supervision timer and                                                            ***/
    /*** (c)  reset the remote address to the configured remote address at SoAd_MainFunction()                    ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    if(0U == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].UdpAliveSupervisionTimer_u32)
    {
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].UdpAliveSupervisionTimer_u32 = 0xFFFFFFFFU;

        SchM_Exit_SoAd_UdpSupervisionTimer();

        /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
        SchM_Enter_SoAd_TxRxReq();

        /*check if there is ongoing IF Tx and Rx */
        if(
#if (SOAD_IF_PRESENT != STD_OFF )
           (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b == FALSE) &&
#endif
           (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].rxOngoingCnt_u8 == 0))
        {
            /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
            SchM_Enter_SoAd_SocConModeProperties();

            /* Set the connection request state to reconnect such that it shall be tried in next main function */
            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en = SOAD_SOCON_BOUND;

            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en    = SOAD_SOCON_RECONNECT;

            SchM_Exit_SoAd_SocConModeProperties();

            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
            SchM_Enter_SoAd_RemoteAddrUpdate();

            /* Reset the remote address to the configured remote address */
            SoAd_CopyRemoteAddrFromStaticSocToDynSoc( (SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst + StaticSocketId_uo), \
                    ((SoAd_DyncSocConfigType_tst *) &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])) );

            SchM_Exit_SoAd_RemoteAddrUpdate();

            SchM_Exit_SoAd_TxRxReq();

            /* call a function which shall invoke the upper layer call back for mode change */
            SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );
        }
        else
        {
            /* discard new Tx and Rx requests */
            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b = TRUE;

            SchM_Exit_SoAd_TxRxReq();

            /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
            SchM_Enter_SoAd_OpenCloseReq();

            if((SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_UL) && \
               (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_SOAD) && \
               (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_TCPIP))
            {
                /* set requested mode to reconnect request SoAd to process post PDU Tx actions in main function */
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en = SOAD_EVENT_RECONNECTREQ_SOAD;
            }

            SchM_Exit_SoAd_OpenCloseReq();
        }

    }
    else
    {
        SchM_Exit_SoAd_UdpSupervisionTimer();
    }
#endif /* SOAD_CONFIG_NUM_OF_UDP_SUPERVISION_TIMEOUT > 0 */

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* --------------------------------------------- */
    /* nPdu Timer                                    */
    /* --------------------------------------------- */

    if( SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].nPduUdpTxTableIndex_uo < SOAD_CFG_nPDU_UDP_TX_DEFAULT_VALUE )
    {
        lnPduUdpTransmitInfo_pst = &((SoAd_CurrConfig_cpst->SoAdIf_nPduUdpTransmitInfo_pst) \
                                    [SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].nPduUdpTxTableIndex_uo]);

        /* The exclusive area nPduTxTimer is used to avoid race condition when nPduTxTimer is modified*/
        SchM_Enter_SoAd_nPduUdpTxTimer();

        if((0xFFFFFFFFU > lnPduUdpTransmitInfo_pst->nPduUdpTxTimer_u32) && \
           (0U < lnPduUdpTransmitInfo_pst->nPduUdpTxTimer_u32))
        {
            /* Decrement by 1 in every main function since nPduUdpTxTimer_u32 is multiple of SoAd Main period */
            lnPduUdpTransmitInfo_pst->nPduUdpTxTimer_u32--;
        }

        /* If nPduUdpTxTimer_u32 is 0 means time is expired, so call the below internal function to
         * transmit buffer data for the socket.
         */
        if( 0U == lnPduUdpTransmitInfo_pst->nPduUdpTxTimer_u32 )
        {
            SchM_Exit_SoAd_nPduUdpTxTimer();

            /* [SWS_SoAd_00550] */
            /* Within SoAd_MainFunction() SoAd shall transmit all PDUs stored for a socket connection (if any) by calling TcpIp_UdpTransmit() if the */
            /* nPdu specific timer expired. (SRS_Eth_00116) */

            /* Execute the sending of all Pdus stored in nPdu */
            SoAd_nPduUdpTxSendAllPdus(StaticSocketId_uo);
        }
        else
        {
            SchM_Exit_SoAd_nPduUdpTxTimer();
        }
    }
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */
    (void)StaticSocketId_uo;
}
#endif /*  SOAD_TIMEOUT_HANDLING_ENABLED == STD_ON ) */

#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_ProcessIfRoutingGrpTransmissionReq()
 *
 * \Function description
 * This function shall perform the IfRoutingGroupTransmission for requested Routing groups
 *
 * Parameters (in): None
 *
 * Parameters (inout): None
 *
 * Parameters (out):  None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
static void SoAd_ProcessIfRoutingGrpTransmissionReq(void)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Pointer to hold the Pdu route destination structure*/
    const PduRouteDestType_tst *              lPduRouteDest_cpst;

    /* Local pointer to store the Tx pdu array one by one */
    const SoAd_TxPduConfigType_tst *          lSoAdTxPduConfig_cpst;

    /* variable to store the return value of internal function */
    Std_ReturnType                            lRetVal_u8;

    /* Index of TxPduConfig table */
    PduIdType                                 lIdxTxPdu_uo;

    /* Variable to hold the index of the static socket connection table */
    SoAd_SoConIdType                          lIdxStaticSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType                          lIdxSoConGroup_uo;

    /* structure to store destination buffer and the number of bytes to copy*/
    PduInfoType                               lPduInfo_st;

    /* variable to store the available data length at the upper layer in bytes*/
    PduLengthType                             lAvailableLength_uo;

    /* Index Of Pdu Route Dest*/
    SoAd_SoConIdType                          lIdxOfPduRouteDest_uo;

    /* variable to store the buffer offset */
    uint8                                     lBuffOffset_u8;

    /*Initialization of variables */
    lAvailableLength_uo     = 0;

    /********************************************************************************************************/
    /****************************** Autosar Requirement Id: SWS_SoAd_00665 **********************************/
    /*****************************  Start: process of transmission of IfRoutingGroup Tx request *************/
    /********************************************************************************************************/

    /* SoAd_IfRoutingGroupTransmit is called for a routing group Id, then SoAd shall store the trigger transmit request for all the
     * PduRouteDests linked with the given RoutingGrpId, then process UL_SoAdTriggerTransmit calls in MainFunction for all
     * PduRouteDests along with their PDUs*/
    /* Loop through all the Tx PDU route Group */
    for(lIdxTxPdu_uo = 0; lIdxTxPdu_uo < SoAd_CurrConfig_cpst->SoAd_noTxPdus_cuo; lIdxTxPdu_uo++)
    {
        /* Copy the address of the array for particular index to local pointer for further processing */
        lSoAdTxPduConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo]);

        /* Loop through all the PDU route Group dests */
        for(lIdxOfPduRouteDest_uo = 0; lIdxOfPduRouteDest_uo <  lSoAdTxPduConfig_cpst->PduRouteDestNum_uo; lIdxOfPduRouteDest_uo++)
        {
            /* Extract pointer to first element of Pdu route destination structure */
            lPduRouteDest_cpst          = &( lSoAdTxPduConfig_cpst->PduRouteDestConf_cpst[lIdxOfPduRouteDest_uo] );

            /* Condition Check: the Socket connection is ONINE AND the TriggerTransmit is not yet/more requested*/
            if( ( NULL_PTR != lPduRouteDest_cpst->triggerTransmissionInfo_puo) && \
                ( STD_ON  == ( lPduRouteDest_cpst->triggerTransmissionInfo_puo->triggerTransmitRequest_u8 ) ) )
            {
#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
                /* if Socket reference is linked to socket connection group */
                if( SOAD_SOCKET_BUNDLE_REFERENCE == lPduRouteDest_cpst->identifyTxSocketConnOrGroupRef_b )
                {
                    /* Get the index of the socket connection group of this socket connection */
                    lIdxSoConGroup_uo           = lPduRouteDest_cpst->txStaticSocketOrGroupIdx_uo ;
                }
                else
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
                {
                    /* Get the static socket index value for the associated TxPdu from the particular pdu route destination structure */
                    lIdxStaticSocket_uo         = lPduRouteDest_cpst->txStaticSocketOrGroupIdx_uo;

                    /* Get the index of the dynamic soc table */
                   // lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo);

                    /* Get the index of the socket connection group of this socket connection */
                    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].soConGrpId_uo ;
                }

                /* Get static socket Group configuration pointers */
                lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo]);

                /* initialize the lBuffOffset_u8  */
                lBuffOffset_u8 = (lSoAdStaticSoConGrpConfig_cpst->headerEnable_b == TRUE)? SOAD_PDUHDR_SIZE : 0U;

                /* initialize the pduInfoPtr  */
                lPduInfo_st.SduDataPtr = &SoAd_PduTxBuf_au8[lBuffOffset_u8];
                lPduInfo_st.SduLength = 0;

                /* call the UL callback function to fetch the data */
                lRetVal_u8 = SoAd_CurrConfig_cpst->SoAd_IfTriggerTransmitCallbacks_cpst[lSoAdTxPduConfig_cpst->idxTriggerTransmitCallback_u8].TriggerTransmit_pfn(
                                                                                                            lSoAdTxPduConfig_cpst->ulTxPduId_uo, &lPduInfo_st);

                /* Calculate the available length if request buffer for transmission from the upper layer module accepted and SduLength > 0 */
                lAvailableLength_uo = (( E_OK == lRetVal_u8 ) && (lPduInfo_st.SduLength > 0U)) ? (lPduInfo_st.SduLength + lBuffOffset_u8) : 0U ;

                /* Condition check : if header is enabled */
                if( ( TRUE == lSoAdStaticSoConGrpConfig_cpst->headerEnable_b ) && \
                    ( 0U < lAvailableLength_uo ) )
                {
                    /* call the internal function to insert the SoAd Header Id and length to the buffer to be transmitted */
                    SoAd_InsertPduHdrToBuffer(lPduRouteDest_cpst->txPduHeaderId_u32, lPduInfo_st.SduLength, SoAd_PduTxBuf_au8);
                }

                if( 0U < lAvailableLength_uo )
                {
#ifdef SOAD_DEBUG_AND_TEST
                    SoAd_TriggerIfRgTransmissionToTcpIp(lPduRouteDest_cpst, lAvailableLength_uo, lIdxTxPdu_uo, lIdxOfPduRouteDest_uo);
#else
                    SoAd_TriggerIfRgTransmissionToTcpIp(lPduRouteDest_cpst, lAvailableLength_uo);
#endif
                }

                /* Release the trigger transmit request */
                (lPduRouteDest_cpst->triggerTransmissionInfo_puo->triggerTransmitRequest_u8) = STD_OFF;

            }/* End for :Condition Check: the Socket connection is ONINE AND the TriggerTransmit is not yet/more requested */
            /* Condition Check: triggerTransmit was stored AND there are no ongoing transmissions AND SoCon is ONLINE */
        } /* End of for loop */
    } /* End of for loop */

    /********************************************************************************************************/
    /*****************************  End: process of transmission of IfRoutingGroup Tx request *************/
    /********************************************************************************************************/
}

/**
 **************************************************************************************************************************************
 * \Function Name : SoAd_TriggerIfRgTransmissionToTcpIp()
 *
 * \Function description
 * This function shall call the the internal function based on the pdu to socket ref
 *
 * Parameters (in):
 * \param   PduRouteDestType_tst *
 * \arg     PduRRouteDest_cpst  - Pointer to a PduRouteDest structure
 * \param   PduLengthType
 * \arg     TotalLength_uo        - No of bytes to be transmitted
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 ***************************************************************************************************************************************
 */
#ifdef SOAD_DEBUG_AND_TEST
static void SoAd_TriggerIfRgTransmissionToTcpIp( const PduRouteDestType_tst * PduRRouteDest_cpst,
                                                 PduLengthType                TotalLength_uo,
                                                 PduIdType                    TxPduId_uo,
                                                 SoAd_SoConIdType             PduRouteDestNum_uo)
#else
static void SoAd_TriggerIfRgTransmissionToTcpIp( const PduRouteDestType_tst * PduRRouteDest_cpst,
                                                 PduLengthType                TotalLength_uo )
#endif
{
    /* Variable to hold the index of the static socket connection table */
    SoAd_SoConIdType lIdxStaticSocket_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* variable to store the return value of internal function */
    Std_ReturnType   lRetVal_u8;

    /*Initialization of local variables */
    lRetVal_u8              = E_NOT_OK;
    lIdxStaticSocket_uo     = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
    /* if Socket reference is linked to socket connection group */
    if( SOAD_SOCKET_BUNDLE_REFERENCE == PduRRouteDest_cpst->identifyTxSocketConnOrGroupRef_b )
    {
        if( SOAD_CFG_SOCON_ID_DEFAULT_VALUE != PduRRouteDest_cpst->triggerTransmissionInfo_puo->triggerTransmitSocketIdx_uo )
        {
            /* If Trigger transmit is called via SoAd_IfSpecificRoutingGroupTransmit( ), then triggerTransmitRequest_u8 would be set to TRUE
             * and triggerTransmitSocketIdx_uowould be updated with the incoming SoConId. SO that no need of looping all the sockets for the group
             */
            lIdxStaticSocket_uo = PduRRouteDest_cpst->triggerTransmissionInfo_puo->triggerTransmitSocketIdx_uo;
        }
        else
        {
            /* Suppose if Trigger transmit is called via SoAd_IfRoutingGroupTransmit( ), then triggerTransmitRequest_u8 alone would be set to TRUE
             * and SoAd_MainFunction needs to loop through for all the sockets to transmit the PDU data via all.
             */
#ifdef SOAD_DEBUG_AND_TEST
            SoAd_IfRgTransmissionToAllGroupSocketsInBundRef(PduRRouteDest_cpst, TotalLength_uo, TxPduId_uo, PduRouteDestNum_uo);
#else
            SoAd_IfRgTransmissionToAllGroupSocketsInBundRef(PduRRouteDest_cpst, TotalLength_uo);
#endif
        }
    }
    /* if Socket reference is linked to socket connection */
    else
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
    {
        /* Get the static socket index value for the associated TxPdu from the particular pdu route destination structure */
        lIdxStaticSocket_uo         = PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo;
    }

    /* Below condition will be failed if budle ref is configured and SoAd_IfRoutingGroupTransmit() is called to trigger the data */
    if( SOAD_CFG_SOCON_ID_DEFAULT_VALUE != lIdxStaticSocket_uo )
    {
        /* Get "index of the dynamic socket table" from static socket table */
        lIdxDynSocket_uo = ( *SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo );

        /* Condition checks:
         * 1. Check Dynamic socket index is valid.
         * 2. Check for socket connection is ONLINE.
         */

        if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
        {
            if ( SOAD_SOCON_ACTIVE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en )
            {

                /* set the flag after trigger Tx is done*/
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b = TRUE;

                lRetVal_u8 = SoAd_IfRgTransmissionToSocketRef(lIdxStaticSocket_uo, TotalLength_uo);

                /* Reset the fla after trigger Tx is done*/
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b = FALSE;

#ifdef SOAD_DEBUG_AND_TEST
                SoAd_SetTestResultValue_tst((*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo), PduRouteDestNum_uo, TxPduId_uo, lRetVal_u8);
#else
                (void)lRetVal_u8;
#endif
            }
            /* Reset the flag after trigger Tx is done*/
            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxPending_b = FALSE;

        }

    }
}

/**
 **************************************************************************************************************************************
 * \Function Name : SoAd_IfRgTransmissionToSocketRef()
 *
 * \Function description
 * This function shall call the TcpIp_Tcp/UdpTransmit() api to tranmit the PDU to lower layer
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo  - Index of static socket table
 * \param   PduLengthType
 * \arg     TotalLength_uo        - No of bytes to be transmitted
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 ***************************************************************************************************************************************
 */
static Std_ReturnType SoAd_IfRgTransmissionToSocketRef( SoAd_SoConIdType StaticSocketId_uo,
                                                        PduLengthType    TotalLength_uo)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst *      lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *              lSoAdDyncSocConfig_pst ;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType                          lIdxDynSocket_uo;

    /* variable to store the return value of internal function */
    Std_ReturnType                            lRetVal_u8;

    /* Variable to hold the return value of the TcpIp_UdpTransmit function call*/
    TcpIp_ReturnType                          lRetVal_UdpTransmit_en;

    /* Variable to store if the remote IP address of static socket has wildcard value */
    boolean                                   lStaticSocConfigHasWildcardIpAddr_b;

    /* Variable to store the port for static socket */
    uint16                                    lStaticSocPort_u16;

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
    /* Structure to store the remote address information (if it is an IPv4 address) */
    TcpIp_SockAddrInetType                    lRemoteSockAddrInet_st;

    /* Structure to store the remote address information (if it is an IPv6 address) */
    TcpIp_SockAddrInet6Type                   lRemoteSockAddrInet6_st;

    /* Pointer to the structure holding the remote address */
    TcpIp_SockAddrType *                      lRemoteSockAddr_pst;
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

    /*Initialization of local variables */
    lRetVal_u8              = E_NOT_OK;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst   = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo            = (*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst      =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    /* Get static socket Group configuration pointers */
    lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo]);

    switch( lSoAdStaticSoConGrpConfig_cpst->soConGrpProtocol_en )
    {
#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    /* Enter into the condition if the configured protocol is TCP */
        case TCPIP_IPPROTO_TCP:
        {
            /* call to initiate the transmission by TcpIp_TcpTransmit */
            lRetVal_u8 = TcpIp_TcpTransmit( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                            &SoAd_PduTxBuf_au8[0],
                                            TotalLength_uo,
                                            TRUE );
            break;
        }/* End for : if(  SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[idxstatsock].soConGrpProtocol_en == TCPIP_IPPROTO_TCP ) */
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
        /* Enter into the condition if the configured protocol is UDP */
        case TCPIP_IPPROTO_UDP:
        {
            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
            SchM_Enter_SoAd_RemoteAddrUpdate();

            /* Load the remoteAddr structure values from the dynamic socket */
            /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
            SoAd_BuildSockAddr( lSoAdDyncSocConfig_pst,
                                  lSoAdStaticSoConGrpConfig_cpst->domain_en,
                                  ((TcpIp_SockAddrInetType *) &lRemoteSockAddrInet_st),
                                  ((TcpIp_SockAddrInet6Type *) &lRemoteSockAddrInet6_st),
                                  &lRemoteSockAddr_pst );

            SchM_Exit_SoAd_RemoteAddrUpdate();

            /* call to initiate the transmission by TcpIp_UdpTransmit,
             * Here, valid data pointer is passed to TcpIp layer, So no expectation of call back SoAd_CopyTxData from TcpIp */
            lRetVal_UdpTransmit_en = TcpIp_UdpTransmit( lSoAdDyncSocConfig_pst->socketHandleTcpIp_uo,
                                                        ( &SoAd_PduTxBuf_au8[0] ),
                                                        lRemoteSockAddr_pst,
                                                        ( (uint16)TotalLength_uo ) );

            /* conversion from TcpIp_ReturnType to Std_ReturnType */
            lRetVal_u8 = (lRetVal_UdpTransmit_en == TCPIP_OK)? E_OK: E_NOT_OK;

            break;

        }/* End for : if(  SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[idxstatsock].soConGrpProtocol_en == TCPIP_IPPROTO_UDP ) */
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

        default:
        {
            /* Invalid Protocol*/
            break;
        }
    }

    lStaticSocConfigHasWildcardIpAddr_b = SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdStaticSocConfig_cpst);
    lStaticSocPort_u16                 = SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG(lSoAdStaticSocConfig_cpst);

    /* Condition check: Return value is E_OK i.e transmission is successful */
    /* If the transmission is successful and socket is Automatic, do as per SWS_SoAd_00582 and SWS_SoAd_00644 */

    if ( (( E_OK == lRetVal_u8 ) &&
         ( ( TRUE == lSoAdStaticSoConGrpConfig_cpst->soConGrpAutoConSetup_b ) &&
         ( ( TCPIP_PORT_ANY == lStaticSocPort_u16 ) || lStaticSocConfigHasWildcardIpAddr_b ) ))  || \
         ( SOAD_EVENT_RECONNECTREQ_SOAD == lSoAdDyncSocConfig_pst->socConRequestedMode_en))
    {

        if( TCPIP_IPPROTO_UDP == lSoAdStaticSoConGrpConfig_cpst->soConGrpProtocol_en )
        {
            /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
            SchM_Enter_SoAd_TxRxReq();

            /*do as per SWS_SoAd_00582 and SWS_SoAd_00644 if there is no ongoing Rx or IF Tx*/
            if((lSoAdDyncSocConfig_pst->rxOngoingCnt_u8 == 0)
    #if (SOAD_IF_PRESENT != STD_OFF )
                && ( FALSE == lSoAdDyncSocConfig_pst->ifTransmissionOnGoing_b )
    #endif
               )
            {
                /* do as per SWS_SoAd_00582 and SWS_SoAd_00644 */

                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();

                /* Set the connection request state to reconnect such that it shall be tried in next main function */
                lSoAdDyncSocConfig_pst->socConSubMode_en = SOAD_SOCON_BOUND;

                lSoAdDyncSocConfig_pst->socConMode_en    = SOAD_SOCON_RECONNECT;

                SchM_Exit_SoAd_SocConModeProperties();

                /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
                SchM_Enter_SoAd_RemoteAddrUpdate();

                SoAd_CopyRemoteAddrFromStaticSocToDynSoc( ((const SoAd_StaticSocConfigType_tst *) \
                        &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])), lSoAdDyncSocConfig_pst);

                SchM_Exit_SoAd_RemoteAddrUpdate();

                /* Reset flag to allow new Tx Rx requests */
                lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = FALSE;

                SchM_Exit_SoAd_TxRxReq();

                /* call a function which shall invoke the upper layer call back for mode change */
                SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

            }
            else
            {
                /*change socket state to reconnect in next main function */
                lSoAdDyncSocConfig_pst->discardNewTxRxReq_b = TRUE;

                SchM_Exit_SoAd_TxRxReq();

                /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                SchM_Enter_SoAd_OpenCloseReq();

                if((lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_UL) && \
                   (lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_SOAD) && \
                   (lSoAdDyncSocConfig_pst->socConRequestedMode_en != SOAD_EVENT_OFFLINEREQ_TCPIP))
                {
                    /* set requested mode to reconnect request SoAd to process post PDU Tx actions in main function */
                    lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_RECONNECTREQ_SOAD;
                }

                SchM_Exit_SoAd_OpenCloseReq();

            }
        }
        else
        {
            /* call the internal function to do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
            SoAd_PerformPostPduTransmissionAction(StaticSocketId_uo);
        }

    }

    return lRetVal_u8;
}

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
/**
 **************************************************************************************************************************************
 * \Function Name : SoAd_IfRgTransmissionToAllGroupSocketsInBundRef()
 *
 * \Function description
 * This function shall call the TcpIp_Tcp/UdpTransmit() api to tranmit the PDU to lower layer by loop throughing all the sockets in group
 *
 * Parameters (in):
 * \param   PduRouteDestType_tst *
 * \arg     PduRRouteDest_cpst  - Pointer to a PduRouteDest structure
 * \param   PduLengthType
 * \arg     TotalLength_uo        - No of bytes to be transmitted
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 ***************************************************************************************************************************************
 */
#ifdef SOAD_DEBUG_AND_TEST
static void SoAd_IfRgTransmissionToAllGroupSocketsInBundRef( const PduRouteDestType_tst * PduRRouteDest_cpst,
                                                             PduLengthType                TotalLength_uo,
                                                             PduIdType                    TxPduId_uo,
                                                             SoAd_SoConIdType             PduRouteDestNum_uo)
#else
static void SoAd_IfRgTransmissionToAllGroupSocketsInBundRef( const PduRouteDestType_tst * PduRRouteDest_cpst,
                                                             PduLengthType                TotalLength_uo )
#endif
{
    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* Index for looping across static sockets*/
    SoAd_SoConIdType lIdxSocketForLoop_uo ;

    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType lStaticSocketId_uo ;

    /* variable to store the return value of internal function */
    Std_ReturnType   lRetVal_u8;

    /* fetch the start socket id from Group table and loop through for max socket count */
    lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].startSoConIdx_uo;

    /* loop through all the sockets connection through the socket connection group for the processing */
    for( lIdxSocketForLoop_uo = 0 ; \
                    lIdxSocketForLoop_uo < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo; \
                            lIdxSocketForLoop_uo++ )
    {
        /* Get "index of the dynamic socket table" from static socket table */
        lIdxDynSocket_uo = ( *SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo );

        /* Condition checks:
         * 1. Check Dynamic socket index is valid.
         * 2. Check for socket connection is ONLINE.
         */
        /* Check for the elements, whether a routing group is referenced to this PduRouteDest */
        /* Why this check is required? Eventhough it was there in API's.
         * Sequence: Bundle Ref to Group was configured and group has more than 1 socket and only one socket is ONLINE
         * UL calls SoAd_IfRoutingGroupTransmit api and SoAd shall set the triggerTransmitRequest_u8 to TRUE if atleast
         * one socket is ONLINE. So here one again check is required to avoid the memory overriding, suppose if it was OFFLINE.
         */

        if(  SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
        {
            if ( SOAD_SOCON_ACTIVE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en )
            {
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b = TRUE;

                lRetVal_u8 = SoAd_IfRgTransmissionToSocketRef(lStaticSocketId_uo, TotalLength_uo);

                /* Reset the flag after trigger Tx is done*/
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b = FALSE;

#ifdef SOAD_DEBUG_AND_TEST
                SoAd_SetTestResultValue_tst(lIdxDynSocket_uo, PduRouteDestNum_uo, TxPduId_uo, lRetVal_u8);
#else
                (void)lRetVal_u8;
#endif
            }

            /* Reset the flag after trigger Tx is done*/
            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxPending_b = FALSE;

            /* increment the lStaticSocketId_uo by 1 to loop the next socket id */
            lStaticSocketId_uo++;
         }
    }
}
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */

#if (SOAD_TP_PRESENT != STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : SoAd_Minimum()
 *
 * \Function description
 * This function returns the minimum of the two parameters.
 *
 * Parameters (in):
 * \param   PduLengthType
 * \arg     Param1_uo   - First paramter to be compared
 * \param   PduLengthType
 * \arg     Param2_uo   - second paramter to be compared
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: Min value
 *
 ***************************************************************************************************
 */
static PduLengthType SoAd_Minimum(PduLengthType Param1_uo, PduLengthType Param2_uo)
{
    /* variable to hold the minimum value*/
    PduLengthType lMinValue_uo;

    if(Param1_uo < Param2_uo)
    {
        lMinValue_uo = Param1_uo;
    }
    else
    {
        lMinValue_uo = Param2_uo;
    }

    return(lMinValue_uo);
}

#endif /* (SOAD_TP_PRESENT != STD_OFF) */

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
