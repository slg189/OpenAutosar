


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_DEBUG_AND_TEST
#include "TestSoAdApplication.h"
#endif
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv_Domain.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
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

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )

static SoAd_IfTransmitReqStatus_ten SoAd_IfTransmitToBundleRef ( PduIdType                      SoAdSrcPduId_uo , \
                                                                 const PduInfoType *            SoAdSrcPduInfo_cpst, \
                                                                 const PduRouteDestType_tst *   PduRRouteDest_cpst,
                                                                 SoAd_IfTransmitReqStatus_ten * IfTransmitFinalRetVal_pu8 );
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

#if ( SOAD_IF_FANOUT_ENABLED != STD_OFF )
static SoAd_IfTransmitReqStatus_ten SoAd_IfFanOutTransmit     ( PduIdType           SoAdSrcPduId_uo , \
                                                                const PduInfoType * SoAdSrcPduInfo_cpst );
#endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

#if ( ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || (SOAD_NUM_TX_IF_TCP_PDU > 0) )


static Std_ReturnType SoAd_IfTransmitDetChk( PduIdType           SoAdSrcPduId_uo , \
                                             const PduInfoType * SoAdSrcPduInfoPtr_cpuo );

static SoAd_IfTransmitReqStatus_ten SoAd_CallIfTcpIpTransmit  ( PduIdType                    SoAdSrcPduId_uo , \
                                                                SoAd_SoConIdType             StaticSocketId_uo, \
                                                                PduLengthType                PduLength_uo, \
                                                                const PduRouteDestType_tst * PduRouteDest_cpst, \
                                                                const PduInfoType *          SoAdSrcPduInfo_cpst);

#endif /* ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
static Std_ReturnType               SoAd_IfTcpTransmit       ( SoAd_SoConIdType DynSocketId_uo, \
                                                               PduLengthType    PduLength_uo );
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
static SoAd_IfTransmitReqStatus_ten SoAd_IfUdpTransmit       ( SoAd_SoConIdType             StaticSocketId_uo, \
                                                               PduLengthType                PduLength_uo, \
                                                               const PduRouteDestType_tst * PduRouteDest_cpst,
															   const SoAd_CopyTxMetaData_tst* CopyTxMetaData_cpst);
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

/**
 ****************************************************************************************************************************
 * \Function Name : SoAd_IfTransmit()
 *
 * \function description
 * This API initiates a request for transmission of the L-PDU specified by the SoAdSrcPduId.
 *
 * Parameters (in) :
 * \param   PduIdType
 * \arg     SoAdSrcPduId - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                          and thereby specifying the socket to be used for transmission of the data.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfoPtr : A pointer to a structure with socket related data: data length and pointer to a data buffer.
 *
 * Parameters (inout): None
 *
 * Parameters (out):   None
 *
 * Return Parameter :
 * \return   Std_ReturnType
 * \retval   E_OK             - The request has been accepted
 *           E_NOT_OK         - The request has not been accepted,
 *                              e.g. due to a still ongoing transmission in the corresponding socket or
 *                              to be transmitted message is too long.
 *
 ****************************************************************************************************************************
 */
Std_ReturnType SoAd_IfTransmit( PduIdType           SoAdSrcPduId , \
                                const PduInfoType * SoAdSrcPduInfoPtr )
{
    /* Variable to store the submode */
    SoAd_SoConSubMode_ten        lSoConSubMode_en;

    /* Variable to store the close req */
    uint16                       lsocConCloseReqCnt_u16;

    /* Variable to store the open req */
    uint16                       lsocConOpenReqCnt_u16;

#if ( ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || (SOAD_NUM_TX_IF_TCP_PDU > 0) )

    Std_ReturnType               lRetVal_u8;

    /* Variable to store the function return value */
    Std_ReturnType               lFunctionRetVal_u8;

    /* Variable to hold SoAd_IfTransmit request status(transmitted, buffered or failed) */
    SoAd_IfTransmitReqStatus_ten lIfTransmitReqStatus_en;

    /* Variable to hold the index of the static socket connection table */
    SoAd_SoConIdType             lIdxStaticsocket_uo;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType             lIdxDynSocket_uo;

#if(STD_ON == SOAD_METADATA_ENABLED)
    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType             lStartStaticSocketId_uo ;

    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType             lEndStaticSocketId_uo ;
#endif

    /* variable to hold the total pdu length of the SoAd Frame */
    PduLengthType                lTotalPduLength_uo;

#if ( SOAD_IF_FANOUT_ENABLED != STD_OFF )
    /* Variable to hold the number of PduRouteDests configured for this PduRoute */
    PduIdType                    lNumberOfPduRouteDests_uo;
#endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

    /* Pointer to hold the Pdu route destination structure */
    const PduRouteDestType_tst * lPduRouteDest_cpst;

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
    /* Routing Group Status */
    boolean                      lRgStatus_b;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

    /* variable to hold the length of the pdu header based on the Header flag */
    PduLengthType                lPduHdrLen_uo;

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
    /* Variable to hold the the protocol type */
    TcpIp_ProtocolType           lProtocol_en;
#endif

    /* Variable to hold PduRouteDestMapped to socket conn ref or group ref */
    boolean                      lIdentifyTxSocketConnOrGroupRef_b;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8      = E_NOT_OK;

    lRetVal_u8 = E_NOT_OK;

    lRetVal_u8 = SoAd_IfTransmitDetChk(SoAdSrcPduId, SoAdSrcPduInfoPtr);

    SchM_Enter_SoAd_IfPduInUse();
    if((lRetVal_u8 == E_OK) && (FALSE == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txPduDyninfo_pst->isIfTxCalled_b ))
    {
        /* set flag to true to avoid further IF Tx requests for same PDUID */
        SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txPduDyninfo_pst->isIfTxCalled_b = TRUE;
        SchM_Exit_SoAd_IfPduInUse();

        /* Condition checks:
          * 1. If there is no DET error if DET is switch On.
          * 2. UL of the Pdu should be type IF.
          * 3. Check if txIfTcpPduIndex_u8 is 0xFF, it will be 0xFF for all UDP sockets
          * 4. Check If txIfTcpPduIndex_u8 is != 0xFF and remaining length to be transmitted is 0, otherwise different pdu's data will be merged.
          * 5. SoAd_ULIfUdpTxConfirmFlag_ab falg is FALSE.
          */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
         if ( ( 0xFF == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txIfTcpPduIndex_u8 )
         ||   ( ( 0xFF != SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txIfTcpPduIndex_u8 )
         &&     ( SoAdSrcPduInfoPtr->SduLength <= ( 0xFFFFFFFFu - SoAd_IfTcpPduTxPendingStatus_ast[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txIfTcpPduIndex_u8].ifTcpTxConfRemainingLen_au32 ) ) ) )
#endif /* (SOAD_NUM_TX_IF_TCP_PDU > 0) */
         {
             /* Initialize the local variables which are declared above */
             lIdxStaticsocket_uo     = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
             lIfTransmitReqStatus_en = SOAD_IF_TRANSMISSION_E_NOT_OK;

             /* Extract pointer to first element of Pdu route destination structure */
             lPduRouteDest_cpst = ( SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].PduRouteDestConf_cpst );

    #if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
             lIdentifyTxSocketConnOrGroupRef_b = lPduRouteDest_cpst[0].identifyTxSocketConnOrGroupRef_b;
    #else
             lIdentifyTxSocketConnOrGroupRef_b = SOAD_SOCKET_CONNECTION_REFERENCE;
    #endif

    #if ( SOAD_IF_FANOUT_ENABLED != STD_OFF )
             /* Get the number of PduRouteDests configured with this PduRoute */
             lNumberOfPduRouteDests_uo = SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].PduRouteDestNum_uo ;

             /* If Fanout is configured for given Pdu's, then SoAd will handle the transmission via separate function */
             if( 1U < lNumberOfPduRouteDests_uo )
             {
                 lIfTransmitReqStatus_en = SoAd_IfFanOutTransmit(SoAdSrcPduId, SoAdSrcPduInfoPtr);
             }
             else
    #endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

    #if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
             /* if Socket reference is linked to socket connection group and
              * more than one socket connections are configured for the group,
              * If Yes call the internal function SoAd_IfTransmitToBundleRef */
             if( ( SOAD_SOCKET_BUNDLE_REFERENCE == lPduRouteDest_cpst[0].identifyTxSocketConnOrGroupRef_b ) &&
                 ( 1U < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo ) )
             {
                 /* lIfTransmitReqStatus_en initialised to SOAD_IF_TRANSMISSION_OK as for a succesfull transmit also If_Transmit will Return E_NOT_OK if not initialised with SOAD_IF_TRANSMISSION_OK */
                 lIfTransmitReqStatus_en = SOAD_IF_TRANSMISSION_OK;

#if(STD_ON == SOAD_METADATA_ENABLED)
                 /* Check if requested PDU has metadata configured */
                 if(TRUE == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].isTxPduMetaDataCfgd_b)
                 {
                     /*[SWS_SoAd_00739] :
                      * In case of a transmit request for a SoAdPduRoute that refers to a global PDU structure configured
                      * with a MetaDataItem of the type SOCKET_CONNECTION_ID_16 and the contained SoAdPduRouteDest refers
                      * to a socket connection group, SoAd shall only perform the transmission on the socket connection identified by PduInfoType.MetaDataPtr instead of the whole group.*/

                     /* fetch the start socket id from Group table */
                     lStartStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].startSoConIdx_uo;

                     /* fetch the end socket id */
                     lEndStaticSocketId_uo = lStartStaticSocketId_uo + SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo;

                     /* check if metadata value lies in the range of socket connection group */
                     if((lStartStaticSocketId_uo <= *SoAdSrcPduInfoPtr->MetaDataPtr) && (*SoAdSrcPduInfoPtr->MetaDataPtr < lEndStaticSocketId_uo))
                     {
                         /* update the socket id with the metadata pointer value */
                         lIdxStaticsocket_uo = *SoAdSrcPduInfoPtr->MetaDataPtr;
                     }
                     else
                     {
                         /* Update transmit request status to E_NOT_OK */
                         lIfTransmitReqStatus_en = SOAD_IF_TRANSMISSION_E_NOT_OK;

                         /* Report DET error as per [SWS_SoAd_00738]*/
                         SOAD_DET_REPORT_ERROR( SOAD_IF_TRANSMIT_API_ID, SOAD_E_INV_METADATA )
                     }
                 }
                 else
#endif/* (STD_ON == SOAD_METADATA_ENABLED) */
                 {
                     /* If metadata item is not configured then transmit over all sockets in group */
                     lIfTransmitReqStatus_en = SoAd_IfTransmitToBundleRef(SoAdSrcPduId, SoAdSrcPduInfoPtr, &lPduRouteDest_cpst[0], &lIfTransmitReqStatus_en);
                 }

             }
             else
    #endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
             {
    #if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
                 /* Get the static socket index value for the associated TxPdu from the particular pdu route destination structure */
                 lIdxStaticsocket_uo = (SOAD_SOCKET_BUNDLE_REFERENCE == lIdentifyTxSocketConnOrGroupRef_b) ? \
                                             /* If only one socket connection is configured for the group, then fetch the socket id directly form group table */
                                             ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].startSoConIdx_uo ):
                                             /* if Socket reference is linked to socket connection */
                                             ( lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo );
    #else
                 lIdxStaticsocket_uo = lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo;
    #endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
             }

             /* Below condition would be valid for the following scenario,
              * 1. If Socket reference is linked to socket connection group and only one socket connection configured for the group
              * 2. If Socket reference is linked to socket connection directly
              * 3. If Socket reference is linked to socket connection group but MetaDataItem is configured for the requested PduId
              */
             if( SOAD_CFG_SOCON_ID_DEFAULT_VALUE != lIdxStaticsocket_uo )
             {
                 /* Initialize the local variables which are declared above */
                 lIfTransmitReqStatus_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
                 lIdxDynSocket_uo        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
                 lTotalPduLength_uo      = 0;

                 /****************************************************************************************************************/
                 /****************************************** [SWS_SoAd_00560] ****************************************************/
                 /****************************************************************************************************************/
                 /****************************************************************************************************************/
                 /*** If SoAd_IfTransmit() is called with SoAdSrcPduId specifying a SoAdPduRouteDest which belongs only to     ***/
                 /*** inactive RoutingGroups, SoAd shall always skip the transmission for this SoAdPduRouteDest and shall      ***/
                 /*** consider the transmission as successful unless all SoAdPduRouteDest of a SoAdPduRoute belong only to     ***/
                 /*** inactive RoutingGroups. In the latter case SoAd shall return E_NOT_OK.                                   ***/
                 /****************************************************************************************************************/

    #if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
                 /* call the internal function to retrieve the Routing Group status for the combination of RG's Id and associated SoCon Id */
                 /* Check Routing Group is enabled for the fetched socket */
                 lRgStatus_b = ( 0U < lPduRouteDest_cpst[0].numOfTxRoutingGroups_uo ) ? \
                                     ( SoAd_GetRoutingGroupsEnabledStatus( lPduRouteDest_cpst[0].txRoutingGroupConf_cpuo,
                                                                           lPduRouteDest_cpst[0].numOfTxRoutingGroups_uo,
                                                                           lIdxStaticsocket_uo ) ) : TRUE;

                 /* Condition check: The routing group associated with this pduId shall be in Enabled state */
                 if( FALSE != lRgStatus_b )
    #endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */
                 {
                     /* Get the additional length if headerEnable_b for TxPDU is enabled,
                      * extra  SOAD_PDUHDR_SIZE shall be added in front of the PDU if header is enabled */
                     lPduHdrLen_uo = ( PduLengthType ) ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].soConGrpId_uo].headerEnable_b == TRUE ) ? SOAD_PDUHDR_SIZE : 0U );

                     /****************************************************************************************************************/
                     /****************************************** [SWS_SoAd_00539]: Step (2)*******************************************/
                     /****************************************************************************************************************/
                     /****************************************************************************************************************/
                     /*****************if the PDU length > 0 or SoAdPduHeaderEnable is TRUE, otherwise SoAd shall Skip further     ***/
                     /***                processing and return with E_NOT_OK                                                       ***/
                     /****************************************************************************************************************/
                     /* Calculate the total length */
                     lTotalPduLength_uo     =  ( ( SoAdSrcPduInfoPtr->SduLength ) + lPduHdrLen_uo );

                     /*[SWS_SoAd_00539] Call the related TcpIp transmit function depending on the connection type if the PDU length > 0 or SoAdPduHeaderEnable is TRUE,
                      * otherwise SoAd shall Skip further processing and return with E_NOT_OK*/

                     /*Proceed with the API if length to transmit is greater than 0*/
                     lIdxDynSocket_uo = ( 0U != lTotalPduLength_uo ) ?
                                     /* Get "the dynamic socket index value" from the static socket table  */
                                     (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].SoAd_Prv_idxDynSocTable_puo ):
                                     SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

                 }/*[SWS_SoAd_00560] all SoAdPduRouteDest of a SoAdPduRoute belong only to inactive RoutingGroups. In this case SoAd shall return E_NOT_OK
                  * Return value already set to E_NOT_OK at Initialization*/

                 /* Condition checks:
                  * 1. Check Dynamic socket index is valid.
                  * 2. Check for socket connection is ONLINE.
                  * 3. Check socket is not requested to close by UL.
                  */

                 if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
                 {

                     lSoConSubMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en;

                     /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                     SchM_Enter_SoAd_OpenCloseReq();

                     lsocConCloseReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConCloseReqCnt_u16;

                     lsocConOpenReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConOpenReqCnt_u16;


                     SchM_Exit_SoAd_OpenCloseReq();

                     if( ( SOAD_SOCON_ACTIVE == lSoConSubMode_en ) && ( ( 0U == lsocConCloseReqCnt_u16 ) || ( lsocConCloseReqCnt_u16 < lsocConOpenReqCnt_u16 ) ) )
                     {
                         /****************************************************************************************************************/
                           /****************************************** [SWS_SoAd_00653] ****************************************************/
                           /****************************************************************************************************************/
                           /****************************************************************************************************************/
                           /*** The service SoAd_IfTransmit() shall skip the transmit request and return E_NOT_OK if there is already    ***/
                           /*** an IF or TP transmission ongoing on the related socket connection identified by SoAdSrcPduId             ***/
                           /****************************************************************************************************************/

                         /* Condition checks:
                         * 1. Check IF Transmission is ongoing.
                         * 2. Check IF Transmission is ongoing.
                         * 3. Check socket is requested to close by SoAd.
                         * 4. Check any of the pdu transmission is still ongoing, it means no TxConfirmation from lower layer.
                         */
                         /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */

						 /* get the value of socket group Id*/
                     	 lIdxSoConGroup_uo     = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].soConGrpId_uo ;

						 SchM_Enter_SoAd_TxRxReq();

                         if( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b )
#if (SOAD_TP_PRESENT != STD_OFF )
                         &&  ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTransmissionOnGoing_b )
#endif /* SOAD_TP_PRESENT != STD_OFF */
                         &&   (  ( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
						       && (   ( SOAD_CFG_PDU_ID_DEFAULT_VALUE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo )
							       || (SoAdSrcPduId == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo) ) )
                               ||  ( TCPIP_IPPROTO_UDP == (SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en)))
                         )
                         {
                             /* Set the transmitReq of the dynamic soc connection to be set,
                              * such that it blocks the further transmission request of IF type on this socket connection */
                             SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = TRUE;

                             SchM_Exit_SoAd_TxRxReq();

                             /* Call the internal function to trigger the TcpIp api's */
                             lIfTransmitReqStatus_en = SoAd_CallIfTcpIpTransmit( SoAdSrcPduId,
                                                                                 lIdxStaticsocket_uo,
                                                                                 lTotalPduLength_uo,
                                                                                 &(lPduRouteDest_cpst[0]),
                                                                                 SoAdSrcPduInfoPtr);

                         	 /* Set the transmitReq of the dynamic soc connection to FALSE */
                             SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = FALSE;

                         }
                         else
                         {
                             SchM_Exit_SoAd_TxRxReq();
                             lIfTransmitReqStatus_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
                         }
                     }
                 }

             }

    #if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
             /* if Socket reference is linked to socket connection group treat lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo as group id
              * if Socket reference is linked to socket connection treat lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo as socket id */
    #if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
             lProtocol_en = (SOAD_SOCKET_BUNDLE_REFERENCE == lIdentifyTxSocketConnOrGroupRef_b) ? \
                                 SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].soConGrpProtocol_en: \
                                 SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].soConGrpId_uo].soConGrpProtocol_en;
    #else
             lProtocol_en = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo].soConGrpId_uo].soConGrpProtocol_en;
    #endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

             if ( TCPIP_IPPROTO_UDP == lProtocol_en )
             {
                 /* The exclusive area SoAd_ULIfUdpTxConfirm is used to avoid race condition when SoAd_ULIfUdpTxConfirm flag is modified */
                 SchM_Enter_SoAd_ULIfUdpTxConfirm();

                 /* Set the SoAd_ULIfUdpTxConfirmFlag_ab flag with E_OK if there is no error at all for any of the PduRoutsDests and Callback also has been registered for corresponding TxPduId */
                 SoAd_ULIfUdpTxConfirmFlag_ab[SoAdSrcPduId] = ( ( NULL_PTR != SoAd_CurrConfig_cpst->SoAdIf_TxCallbacks_cpst[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].idxIfTxConfirm_u8].TxCallback_pfn ) && \
                                                                   ( SOAD_IF_TRANSMISSION_OK == lIfTransmitReqStatus_en ) ) ? \
                                                                       TRUE : FALSE;

                 SchM_Exit_SoAd_ULIfUdpTxConfirm();
             }
             else
    #endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */
             {
    #if (SOAD_NUM_TX_IF_TCP_PDU > 0)
                 if( SOAD_IF_TRANSMISSION_E_NOT_OK == lIfTransmitReqStatus_en )
                 {
                     /* Reset the ifTcpTxConfRemainingLen_au32 zero if any of the transmission goes wrong or unable to transmit via some socket,
                      * Ultimately, SoAd shall skip the TxConfirmation if SoAd_IfTransmit returns E_NOT_OK */
                     SoAd_IfTcpPduTxPendingStatus_ast[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txIfTcpPduIndex_u8].ifTcpTxConfRemainingLen_au32 = 0;
                 }
    #endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */
             }

             (void)lIdentifyTxSocketConnOrGroupRef_b;
             /* conversion of SoAd_IfTransmitReqStatus_ten enum to Std_ReturnType */
             lFunctionRetVal_u8 = ( SOAD_IF_TRANSMISSION_E_NOT_OK == lIfTransmitReqStatus_en ) ? E_NOT_OK : E_OK;

             /* Reset flag to false to allow further IF Tx requests for same PDUID */
             SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txPduDyninfo_pst->isIfTxCalled_b = FALSE;

         }/*End of If condition: Check SoAd Module should be Initialized */

    }
    else
    {
        SchM_Exit_SoAd_IfPduInUse();
    }

    return(lFunctionRetVal_u8);
#else
    (void)SoAdSrcPduId;
    (void)SoAdSrcPduInfoPtr;
    return(E_NOT_OK);
#endif /* ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) */
}

#if ( ( ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) ) && (SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF) )
/**
 ****************************************************************************************************************************
 * \Function Name : SoAd_IfTransmitToBundleRef()
 *
 * \function description
 * This API initiates a request for transmission of the L-PDU specified by the SoAdSrcPduId if socket connection
 * reference is mapped to socket connection group.
 *
 * Parameters (in) :
 * \param   PduIdType
 * \arg     SoAdSrcPduId_uo - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                          and thereby specifying the socket to be used for transmission of the data.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfo_cpst - A pointer to a structure with socket related data: data length and pointer to a data buffer.
 * \param   PduRouteDestType_tst *
 * \arg     PduRRouteDest_cpst  - Pointer to a PduRouteDest structure
 *
 * Parameters (inout): None
 *
 * Parameters (out):   None
 *
 * Return Parameter :
 * \return   SoAd_IfTransmitReqStatus_ten
 * \retval   SOAD_IF_TRANSMISSION_OK        - The request has been accepted and transmitted to lower layer successfully,
 *           SOAD_IF_TRANSMISSION_E_NOT_OK  - The request has not been accepted,
 *           SOAD_IF_NPDU_BUFFERING_OK      - The request has been accepted, but is is not transmitted to lower layer, instead
 *                                            it stored in corresponding nPduUdpTxBuffer since pdu is configured with TRIGGER_NEVER option,
 *
 ****************************************************************************************************************************
 */
static SoAd_IfTransmitReqStatus_ten SoAd_IfTransmitToBundleRef( PduIdType                      SoAdSrcPduId_uo , \
                                                                const PduInfoType *            SoAdSrcPduInfo_cpst, \
                                                                const PduRouteDestType_tst *   PduRRouteDest_cpst, \
                                                                SoAd_IfTransmitReqStatus_ten * IfTransmitFinalRetVal_pu8 )
{
    /* Variable to store the submode */
    SoAd_SoConSubMode_ten        lSoConSubMode_en;

    /* Variable to store the close req count*/
    uint16                       lsocConCloseReqCnt_u16;

    /* Variable to store the open req count */
    uint16                       lsocConOpenReqCnt_u16;

    /* Variable to store the return value of the internal functions */
    SoAd_IfTransmitReqStatus_ten lRetVal_en;

    /* Variable to store the return value */
    SoAd_IfTransmitReqStatus_ten lFunctionRetVal_en;

    /* Variable to store the previous return value */
    SoAd_IfTransmitReqStatus_ten lPrevRetVal_en;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Index for looping across static sockets*/
    SoAd_SoConIdType             lIdxSocketForLoop_uo ;

    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType             lStaticSocketId_uo ;

    /* variable to hold the total pdu length of the SoAd Frame */
    PduLengthType                lTotalPduLength_uo;

    /* Variable to store the Routing Group Status */
    boolean                      lRgStatus_b;

    /* variable to hold the length of the pdu header based on the Header flag */
    PduLengthType                lPduHdrLen_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Initialize the local variables which are declared above */
    lFunctionRetVal_en      = SOAD_IF_TRANSMISSION_OK;
    lPrevRetVal_en          = SOAD_IF_TRANSMISSION_E_NOT_OK;
    lRgStatus_b             = TRUE;
    lIdxSocketForLoop_uo    = 0;

    /* Get the additional length if headerEnable_b for TxPDU is enabled,
     * extra  SOAD_PDUHDR_SIZE shall be added in front of the PDU if header is enabled */
    lPduHdrLen_uo = ( PduLengthType ) ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].headerEnable_b == TRUE ) ? SOAD_PDUHDR_SIZE : 0U );

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00539]: Step (2)*******************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*****************if the PDU length > 0 or SoAdPduHeaderEnable is TRUE, otherwise SoAd shall Skip further     ***/
    /***                processing and return with E_NOT_OK                                                       ***/
    /****************************************************************************************************************/
    /* Calculate the total length */
    lTotalPduLength_uo     =  ( ( SoAdSrcPduInfo_cpst->SduLength ) + lPduHdrLen_uo );

    /*[SWS_SoAd_00539] Call the related TcpIp transmit function depending on the connection type if the PDU length > 0 or SoAdPduHeaderEnable is TRUE,
     * otherwise SoAd shall Skip further processing and return with E_NOT_OK*/

    /*Proceed with the API if length to transmit is greater than 0*/
    if( 0 == lTotalPduLength_uo )
    {
        /*[SWS_SoAd_00539] Call the related TcpIp transmit function depending on the connection type if the PDU length > 0 or SoAdPduHeaderEnable is TRUE,
         * otherwise SoAd shall Skip further processing and return with E_NOT_OK*/

        /* Set the lFunctionRetVal_en to SOAD_IF_TRANSMISSION_E_NOT_OK to skip further processing as per SWS_SoAd_00539 if length is 0 */
        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;

        /* set the lIdxSocketForLoop_uo to maxSoConChannel_uo to avoid the while loop execution */
        lIdxSocketForLoop_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo;
    }

    /* fetch the start socket id from Group table and loop through for max socket count */
    lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].startSoConIdx_uo;

    /* loop through all the sockets connection through the socket connection group for the processing */
    while(lIdxSocketForLoop_uo < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[PduRRouteDest_cpst->txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo)
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00560] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** If SoAd_IfTransmit() is called with SoAdSrcPduId specifying a SoAdPduRouteDest which belongs only to     ***/
        /*** inactive RoutingGroups, SoAd shall always skip the transmission for this SoAdPduRouteDest and shall      ***/
        /*** consider the transmission as successful unless all SoAdPduRouteDest of a SoAdPduRoute belong only to     ***/
        /*** inactive RoutingGroups. In the latter case SoAd shall return E_NOT_OK.                                   ***/
        /****************************************************************************************************************/

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
        /* call the internal function to retrieve the Routing Group status for the combination of RG's Id and associated SoCon Id */
        /* Check Routing Group is enabled for the fetched socket */
        lRgStatus_b = ( 0U < PduRRouteDest_cpst->numOfTxRoutingGroups_uo ) ? \
                            ( SoAd_GetRoutingGroupsEnabledStatus(PduRRouteDest_cpst->txRoutingGroupConf_cpuo, \
                                                                 PduRRouteDest_cpst->numOfTxRoutingGroups_uo, \
                                                                 lStaticSocketId_uo)): \
                                                                 TRUE;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

        /* Condition check: The routing group associated with this pduId shall be in Enabled state */
        /*[SWS_SoAd_00560] all SoAdPduRouteDest of a SoAdPduRoute belong only to inactive RoutingGroups. In this case SoAd shall return E_NOT_OK
         * Return value already set to E_NOT_OK at Initialization*/
        lIdxDynSocket_uo = ( FALSE != lRgStatus_b ) ?
                                /* Get "the dynamic socket index value" from the static socket table  */
                                (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo ):
                                        /* Load the default value to dynamic index */
                                        SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

        if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
        {
            /* Condition checks:
             * 1. Check Dynamic socket index is valid.
             * 2. Check for socket connection is ONLINE.
             * 3. Check socket is not requested to close by UL.
             */

            lSoConSubMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en;

            /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
            SchM_Enter_SoAd_OpenCloseReq();

            lsocConCloseReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConCloseReqCnt_u16;

            lsocConOpenReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConOpenReqCnt_u16;



            SchM_Exit_SoAd_OpenCloseReq();

            if( ( SOAD_SOCON_ACTIVE == lSoConSubMode_en ) && ( ( 0U == lsocConCloseReqCnt_u16 ) || ( lsocConCloseReqCnt_u16 < lsocConOpenReqCnt_u16 ) ) )

                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00653] ****************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /*** The service SoAd_IfTransmit() shall skip the transmit request and return E_NOT_OK if there is already    ***/
                /*** an IF or TP transmission ongoing on the related socket connection identified by SoAdSrcPduId             ***/
                /****************************************************************************************************************/
            {
                /* Condition checks:
                 * 1. Check IF Transmission is ongoing.
                 * 2. Check IF Transmission is ongoing.
                 * 3. Check socket is requested to close by SoAd.
                 * 4. Check any of the pdu transmission is still ongoing, it means no TxConfirmation from lower layer.
                 */
				 /* get the value of socket group Id*/
            	lIdxSoConGroup_uo       = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].soConGrpId_uo ;
                /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
                SchM_Enter_SoAd_TxRxReq();

                if ( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b )
    #if (SOAD_TP_PRESENT != STD_OFF )
                &&   ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTransmissionOnGoing_b )
    #endif /* SOAD_TP_PRESENT != STD_OFF */
                &&   ( ( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
                        &&  ( ( SOAD_CFG_PDU_ID_DEFAULT_VALUE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo )
                            || ( SoAdSrcPduId_uo == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo ) ) )
                       || ( TCPIP_IPPROTO_UDP == (SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en) ) )
                )
                {

                    /* Set the transmitReq of the dynamic soc connection to be set,
                     * such that it blocks the further transmission request of IF type on this socket connection */
                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = TRUE;

                    SchM_Exit_SoAd_TxRxReq();

                    /* Call the internal function to trigger the TcpIp api's */
                    lRetVal_en = SoAd_CallIfTcpIpTransmit( SoAdSrcPduId_uo,
                                                           lStaticSocketId_uo,
                                                           lTotalPduLength_uo,
                                                           PduRRouteDest_cpst,
                                                           SoAdSrcPduInfo_cpst);

                    /* Set the transmitReq of the dynamic soc connection to FALSE */
                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = FALSE;


                    /* update the lFunctionRetVal_en only when lRetVal_en is SOAD_IF_TRANSMISSION_E_NOT_OK.
                     * If 3 pdu dests are configured and if 1st transmission is failed and 2nd is succeed,
                     * then SoAd has to return E_NOT_OK as per SWS_SoAd_00648, so we should not update the
                     * lFunctionRetVal_en for success scenario in this case.
                     */
                    if( SOAD_IF_TRANSMISSION_E_NOT_OK == lRetVal_en)
                    {
                        /* Set lFunctionRetVal_en to SOAD_IF_TRANSMISSION_E_NOT_OK */
                        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;

                        /* load the IfTransmitFinalRetVal_pu8 with SOAD_IF_TRANSMISSION_E_NOT_OK if any one of the transmission is failed */
                        (*IfTransmitFinalRetVal_pu8) = SOAD_IF_TRANSMISSION_E_NOT_OK;

                    }
                    else
                    {
                        /* If any one of the transmision is success, lPrevRetVal_en set to E_OK */
                        lPrevRetVal_en = lRetVal_en;
                    }
                }
                else
                {
                    SchM_Exit_SoAd_TxRxReq();
                    lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
                }
            }
        }

        /* increment the socket id to loop through for next socket */
        lStaticSocketId_uo++;

        /* increment the while loop count to loop to allow the next socket id */
        lIdxSocketForLoop_uo++;
    }

    /* This condition is used to update the lFunctionRetVal_en based on the following scenarios
     * 1. If all the sockets mapped to pdu route dest is not ONLINE then SoAd shall return E_NOT_OK.
     *    or If all routing groups are disable for all the pdu dests, then also SoAd shall return E_NOT_OK as per as per SWS_SoAd_00560.
     *    ((lIdxOfPduRouteDest_uo + 1) == lNumberofPduRouteDests_uo) is satisfy this requirement.
     *    lFunctionRetVal_en has to be updated if lPrevRetVal_en == SOAD_IF_TRANSMISSION_E_NOT_OK, otherwise we can assure that
     *    some transmission was already successful for atleast any of the Pdu route dest
     * 2. lFunctionRetVal_en can be updated to SOAD_IF_TRANSMISSION_E_NOT_OK if lFunctionRetVal_en is SOAD_IF_TRANSMISSION_OK, otherwise its not required
     */
    if( ( SOAD_IF_TRANSMISSION_E_NOT_OK != lFunctionRetVal_en) && ( SOAD_IF_TRANSMISSION_E_NOT_OK == lPrevRetVal_en ) )
    {
        /*set the lFunctionRetVal_en to SOAD_IF_TRANSMISSION_E_NOT_OK to return it to caller of the function */
        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
    }

    return lFunctionRetVal_en;
}
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */

#if ( ( ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) ) && (SOAD_IF_FANOUT_ENABLED != STD_OFF) )
/**
 ****************************************************************************************************************************
 * \Function Name : SoAd_IfFanOutTransmit()
 *
 * \function description
 * This API initiates a request for transmission of the L-PDU specified by the SoAdSrcPduId if Fanout is configured for the given Pdu.
 *
 * Parameters (in) :
 * \param   PduIdType
 * \arg     SoAdSrcPduId_uo - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                          and thereby specifying the socket to be used for transmission of the data.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfo_cpst - A pointer to a structure with socket related data: data length and pointer to a data buffer.
 *
 * Parameters (inout): None
 *
 * Parameters (out):   None
 *
 * Return Parameter :
 * \return   SoAd_IfTransmitReqStatus_ten
 * \retval   SOAD_IF_TRANSMISSION_OK        - The request has been accepted and transmitted to lower layer successfully,
 *           SOAD_IF_TRANSMISSION_E_NOT_OK  - The request has not been accepted,
 *           SOAD_IF_NPDU_BUFFERING_OK      - The request has been accepted, but is is not transmitted to lower layer, instead
 *                                            it stored in corresponding nPduUdpTxBuffer since pdu is configured with TRIGGER_NEVER option,
 *
 ****************************************************************************************************************************
 */
static SoAd_IfTransmitReqStatus_ten SoAd_IfFanOutTransmit( PduIdType           SoAdSrcPduId_uo , \
                                                           const PduInfoType * SoAdSrcPduInfo_cpst )
{
    /* Pointer to hold the Pdu route destination structure */
    const PduRouteDestType_tst * lPduRouteDest_cpst;

    /* variable to hold the length of the pdu header based on the Header flag */
    PduLengthType                lPduHdrLen_uo;

    /* variable to hold the total pdu length of the SoAd Frame */
    PduLengthType                lTotalPduLength_uo;

    /* Variable to store the return value of the internal functions */
    SoAd_IfTransmitReqStatus_ten lRetVal_en;

    /* Variable to store the previous return value */
    SoAd_IfTransmitReqStatus_ten lPrevRetVal_en;

    /* Variable to store the return value */
    SoAd_IfTransmitReqStatus_ten lFunctionRetVal_en;

    /* Variable to store the submode */
    SoAd_SoConSubMode_ten        lSoConSubMode_en;

    /* Variable to store the close req count */
    uint16                       lsocConCloseReqCnt_u16;

    /* Variable to store the open req count */
    uint16                       lsocConOpenReqCnt_u16;

    /* Variable to hold the index of the static socket connection table */
    SoAd_SoConIdType             lIdxStaticsocket_uo;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

#if(STD_ON == SOAD_METADATA_ENABLED)
    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType             lStartStaticSocketId_uo ;

    /* variable to store index of the static socket connection one by one for the group */
    SoAd_SoConIdType             lEndStaticSocketId_uo ;
#endif

    /* Variable to hold the number of PduRouteDests configured for this PduRoute */
    PduIdType                    lNumberOfPduRouteDests_uo;

    /* Variable to hold the index of PduRouteDests configured for this PduRoute */
    SoAd_SoConIdType             lIdxOfPduRouteDest_uo;

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    /* Variable to store the Routing Group Status */
    boolean                      lRgStatus_b;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

    /* Initialize the local variables which are declared above */
    lFunctionRetVal_en  = SOAD_IF_TRANSMISSION_OK;
    lPrevRetVal_en      = SOAD_IF_TRANSMISSION_E_NOT_OK;
    lTotalPduLength_uo  = 0;

    /* Extract pointer to first element of Pdu route destination structure */
    lPduRouteDest_cpst = ( SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].PduRouteDestConf_cpst );

    /* Get the number of PduRouteDests configured with this PduRoute */
    lNumberOfPduRouteDests_uo = SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].PduRouteDestNum_uo ;


    /* Loop through all the route dests to transmit the Pdu */
    for( lIdxOfPduRouteDest_uo = 0; ( lIdxOfPduRouteDest_uo < lNumberOfPduRouteDests_uo ); lIdxOfPduRouteDest_uo++)
    {
        lIdxDynSocket_uo = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

        lIdxStaticsocket_uo = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

#if ( SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF )
        /* if Socket reference is linked to socket connection group */
        if( SOAD_SOCKET_BUNDLE_REFERENCE == lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].identifyTxSocketConnOrGroupRef_b )
        {
#if((STD_ON == SOAD_METADATA_ENABLED))
            /* Check if requested PDU has metadata configured */
            if(TRUE == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].isTxPduMetaDataCfgd_b)
            {
                /*[SWS_SoAd_00739] :
                 * In case of a transmit request for a SoAdPduRoute that refers to a global PDU structure configured
                 * with a MetaDataItem of the type SOCKET_CONNECTION_ID_16 and the contained SoAdPduRouteDest refers
                 * to a socket connection group, SoAd shall only perform the transmission on the socket connection identified by PduInfoType.MetaDataPtr instead of the whole group.*/

                /* fetch the start socket id from Group table */
                lStartStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].txStaticSocketOrGroupIdx_uo].startSoConIdx_uo;

                /* fetch the end socket id */
                lEndStaticSocketId_uo = lStartStaticSocketId_uo + SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].txStaticSocketOrGroupIdx_uo].maxSoConChannel_uo;

                /* check if metadata value lies in the range of socket connection group */
                if((lStartStaticSocketId_uo <= *SoAdSrcPduInfo_cpst->MetaDataPtr) && (*SoAdSrcPduInfo_cpst->MetaDataPtr < lEndStaticSocketId_uo))
                {
                    /* update the socket id with the metadata pointer value */
                    lIdxStaticsocket_uo = *SoAdSrcPduInfo_cpst->MetaDataPtr;
                }
                else
                {
                    /* Report DET error as per [SWS_SoAd_00738]*/
                    SOAD_DET_REPORT_ERROR( SOAD_IF_TRANSMIT_API_ID, SOAD_E_INV_METADATA )
                }
            }
            else
#endif/* (STD_ON == SOAD_METADATA_ENABLED) */
            {
                /* If metadata item is not configured then transmit over all sockets in socket connection group */
                lPrevRetVal_en = SoAd_IfTransmitToBundleRef(SoAdSrcPduId_uo, SoAdSrcPduInfo_cpst, &lPduRouteDest_cpst[lIdxOfPduRouteDest_uo], &lFunctionRetVal_en );
            }

        }
        else
#endif /* SOAD_SOCON_TX_BUNDLE_REF_ENABLE != STD_OFF */
        /* if Socket reference is linked to socket connection */
        {
            lIdxStaticsocket_uo = lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].txStaticSocketOrGroupIdx_uo;
        }

        /* Below condition would be valid for the following scenario,
         * 1. If Socket reference is linked to socket connection group and only one socket connection configured for the group
         * 2. If Socket reference is linked to socket connection directly
         * 3. If Socket reference is linked to socket connection group but MetaDataItem is configured for the requested PduId
         */
        if( SOAD_CFG_SOCON_ID_DEFAULT_VALUE != lIdxStaticsocket_uo )
        {

            /****************************************************************************************************************/
            /****************************************** [SWS_SoAd_00560] ****************************************************/
            /****************************************************************************************************************/
            /****************************************************************************************************************/
            /*** If SoAd_IfTransmit() is called with SoAdSrcPduId specifying a SoAdPduRouteDest which belongs only to  ***/
            /*** inactive RoutingGroups, SoAd shall always skip the transmission for this SoAdPduRouteDest and shall      ***/
            /*** consider the transmission as successful unless all SoAdPduRouteDest of a SoAdPduRoute belong only to     ***/
            /*** inactive RoutingGroups. In the latter case SoAd shall return E_NOT_OK.                                   ***/
            /****************************************************************************************************************/

#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
            /* call the internal function to retrieve the Routing Group status for the combination of RG's Id and associated SoCon Id */
            /* Check Routing Group is enabled for the fetched socket */
            lRgStatus_b = ( 0U < lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].numOfTxRoutingGroups_uo ) ? \
                                (SoAd_GetRoutingGroupsEnabledStatus( \
                                        lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].txRoutingGroupConf_cpuo, \
                                        lPduRouteDest_cpst[lIdxOfPduRouteDest_uo].numOfTxRoutingGroups_uo, \
                                        lIdxStaticsocket_uo)): \
                                        TRUE;

            /* Condition check: The routing group associated with this pduId shall be in Enabled state */
            if( FALSE != lRgStatus_b )
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */
            {
                /* Get the additional length 8 bytes(Header(4) + Length(4)) if headerEnable_b for TxPDU is enabled,
                 * extra  SOAD_PDUHDR_SIZE shall be added in front of the PDU if header is enabled */
                lPduHdrLen_uo = ( PduLengthType ) ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].soConGrpId_uo].headerEnable_b == TRUE ) ? SOAD_PDUHDR_SIZE : 0U );

                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00539]: Step (2)*******************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /***************** if the PDU length > 0 or SoAdPduHeaderEnable is TRUE, otherwise SoAd shall Skip further    ***/
                /***                processing and return with E_NOT_OK                                                       ***/
                /****************************************************************************************************************/
                /* Calculate the total length */
                lTotalPduLength_uo     =  ( SoAdSrcPduInfo_cpst->SduLength + lPduHdrLen_uo );

                /*Proceed with the API if length to transmit is greater than 0*/
                if( 0U != lTotalPduLength_uo )
                {
                    /* Get "the dynamic socket index value" from the static socket table  */
                    lIdxDynSocket_uo   = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].SoAd_Prv_idxDynSocTable_puo );
                }
                else
                {
                    /*[SWS_SoAd_00539] Call the related TcpIp transmit function depending on the connection type if the PDU length > 0 or SoAdPduHeaderEnable is TRUE,
                     * otherwise SoAd shall Skip further processing and return with E_NOT_OK*/

                    /* Set the lFunctionRetVal_en to SOAD_IF_TRANSMISSION_E_NOT_OK to skip further processing as per SWS_SoAd_00539 if length is 0 */
                    lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
                }

            }/*[SWS_SoAd_00560] all SoAdPduRouteDest of a SoAdPduRoute belong only to inactive RoutingGroups. In this case SoAd shall return E_NOT_OK
             * Return value already set to E_NOT_OK at Initialization*/

            /* Condition checks:
             * 1. Check Dynamic socket index is valid.
             * 2. Check for socket connection is ONLINE.
             * 3. Check socket is not requested to close by UL.
             */

            if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
            {
                /* Condition checks:
                 * 1. Check Dynamic socket index is valid.
                 * 2. Check for socket connection is ONLINE.
                 * 3. Check socket is not requested to close by UL.
                 */

                lSoConSubMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en;

                /* The exclusive area SoAd_OpenCloseReq is used to avoid race condition when socket open or close is requestedby UL or SoAd */
                SchM_Enter_SoAd_OpenCloseReq();

                lsocConCloseReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConCloseReqCnt_u16;

                lsocConOpenReqCnt_u16 = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConOpenReqCnt_u16;

                SchM_Exit_SoAd_OpenCloseReq();

                if( ( SOAD_SOCON_ACTIVE == lSoConSubMode_en ) && ( ( 0U == lsocConCloseReqCnt_u16 ) || ( lsocConCloseReqCnt_u16 < lsocConOpenReqCnt_u16 ) ) )

                    /****************************************************************************************************************/
                    /****************************************** [SWS_SoAd_00653] ****************************************************/
                    /****************************************************************************************************************/
                    /****************************************************************************************************************/
                    /*** The service SoAd_IfTransmit() shall skip the transmit request and return E_NOT_OK if there is already    ***/
                    /*** an IF or TP transmission ongoing on the related socket connection identified by SoAdSrcPduId             ***/
                    /****************************************************************************************************************/
                {
                    /* Condition checks:
                     * 1. Check IF Transmission is ongoing.
                     * 2. Check IF Transmission is ongoing.
                     * 3. Check socket is requested to close by SoAd.
                     * 4. Check any of the pdu transmission is still ongoing, it means no TxConfirmation from lower layer.
                     */
                	/* get the value of socket group Id*/
                	lIdxSoConGroup_uo       = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].soConGrpId_uo ;

                    /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
                    SchM_Enter_SoAd_TxRxReq();

                    if ( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b )
#if (SOAD_TP_PRESENT != STD_OFF )
                &&   ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTransmissionOnGoing_b )
#endif /* SOAD_TP_PRESENT != STD_OFF */
                &&   ( ( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
                        && (  ( SOAD_CFG_PDU_ID_DEFAULT_VALUE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo )
                            || ( SoAdSrcPduId_uo == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo) ) )
                        || ( TCPIP_IPPROTO_UDP == (SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en) ) )
                     )
                    {
                        /* Set the transmitReq of the dynamic soc connection to be set,
                         * such that it blocks the further transmission request of IF type on this socket connection */
                        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = TRUE;

                        SchM_Exit_SoAd_TxRxReq();

                        /* Call the internal function to trigger the TcpIp api's*/
                        lRetVal_en = SoAd_CallIfTcpIpTransmit( SoAdSrcPduId_uo,
                                                               lIdxStaticsocket_uo,
                                                               lTotalPduLength_uo,
                                                               &(lPduRouteDest_cpst[lIdxOfPduRouteDest_uo]),
                                                               SoAdSrcPduInfo_cpst );

                         /* Set the transmitReq of the dynamic soc connection to FALSE */
                         SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = FALSE;

                        /* update the lFunctionRetVal_en only when lRetVal_en is SOAD_IF_TRANSMISSION_E_NOT_OK.
                         * If 3 pdu dests are configured and if 1st transmission is failed and 2nd is succeed,
                         * then SoAd has to return E_NOT_OK as per SWS_SoAd_00648, so we should not update the
                         * lFunctionRetVal_en for success scenario in this case.
                         */
                        /* For achieving the HIS compliance for this function, conditional operator has been used by removing if else statement.
                         * Incase if we found that conditional opeartor is taking more run time than if else statement then
                         * we will go back to if else statement, in this case it would be a small HIS violance interms of level
                         */

                        /* If any one of the transmision is success, lPrevRetVal_en set to E_OK */
                        lPrevRetVal_en = ((SOAD_IF_TRANSMISSION_E_NOT_OK != lRetVal_en) && \
                                          (lPrevRetVal_en == SOAD_IF_NPDU_BUFFERING_OK))? lPrevRetVal_en: lRetVal_en;

                        lFunctionRetVal_en = ( SOAD_IF_TRANSMISSION_E_NOT_OK == lRetVal_en) ? SOAD_IF_TRANSMISSION_E_NOT_OK : \
                                            ((lFunctionRetVal_en != SOAD_IF_TRANSMISSION_OK)? lFunctionRetVal_en: lRetVal_en);

                    }
                    else
                    {
                        SchM_Exit_SoAd_TxRxReq();
                        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
                    }
                }
            }

        }
    }

    /* This condition is used to update the lFunctionRetVal_en based on the following scenarios
     * 1. If all the sockets mapped to pdu route dest is not ONLINE then SoAd shall return E_NOT_OK.
     *    or If all routing groups are disable for all the pdu dests, then also SoAd shall return E_NOT_OK as per as per SWS_SoAd_00560.
     *    ((lIdxOfPduRouteDest_uo + 1) == lNumberofPduRouteDests_uo) is satisfy this requirement.
     *    lFunctionRetVal_en has to be updated if lPrevRetVal_en == SOAD_IF_TRANSMISSION_E_NOT_OK, otherwise we can assure that
     *    some transmission was already successful for atleast any of the Pdu route dest
     * 2. lFunctionRetVal_en can be updated to SOAD_IF_TRANSMISSION_E_NOT_OK if lFunctionRetVal_en is SOAD_IF_TRANSMISSION_OK, otherwise its not required
     */
    if( ( SOAD_IF_TRANSMISSION_E_NOT_OK != lFunctionRetVal_en) && ( SOAD_IF_TRANSMISSION_E_NOT_OK == lPrevRetVal_en ) )
    {
        /*set the lFunctionRetVal_en to SOAD_IF_TRANSMISSION_E_NOT_OK to return it to caller of the function */
        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
    }

    return lFunctionRetVal_en;
}
#endif /* SOAD_IF_FANOUT_ENABLED != STD_OFF */

#if ( ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) )

/**
 ***************************************************************************************************
 * \Function Name : SoAd_CallIfTcpIpTransmit()
 *
 * \Function description
 * This function shall act as wrpper to call the Tcp/Udp specific transmit functions
 *
 * Parameters (in):
 * \param   PduIdType
 * \arg     SoAdSrcPduId_uo - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                            and thereby specifying the socket to be used for transmission of the data.
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier Table
 * \param   PduLengthType
 * \arg     PduLength_uo  - No of bytes to be transmitted
 * \param   const PduRouteDestType_tst*
 * \arg     PduRouteDest_cpst - A pointer to a structure of PduRouteDest to which packet needs to be transmitted.
 *                              it is passed as argument to fetch the txPduHeaderId_u32, triggerTransmission_en and txUdpTriggerTimeout_32_32
 *                              members of the PduRouteDestType_tst structure.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfo_cpst - A pointer to a structure with socket related data: data length and pointer to a data buffer.
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * \return   SoAd_IfTransmitReqStatus_ten
 * \retval   SOAD_IF_TRANSMISSION_OK        - The request has been accepted and transmitted to lower layer successfully,
 *           SOAD_IF_TRANSMISSION_E_NOT_OK  - The request has not been accepted,
 *           SOAD_IF_NPDU_BUFFERING_OK      - The request has been accepted, but is is not transmitted to lower layer, instead
 *                                            it stored in corresponding nPduUdpTxBuffer since pdu is configured with TRIGGER_NEVER option,
 *
 ***************************************************************************************************
 */
static SoAd_IfTransmitReqStatus_ten SoAd_CallIfTcpIpTransmit( PduIdType                    SoAdSrcPduId_uo , \
                                                              SoAd_SoConIdType             StaticSocketId_uo, \
                                                              PduLengthType                PduLength_uo, \
                                                              const PduRouteDestType_tst * PduRouteDest_cpst, \
                                                              const PduInfoType *          SoAdSrcPduInfo_cpst )
{

    /* variable to store the return value of internal function call */
    SoAd_IfTransmitReqStatus_ten lFunctionRetVal_en;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType             lIdxSoConGroup_uo;

    /* Variable to store if the remote address of static socket is wildcard (ANY)  */
    boolean                      lStaticSocConfigHasWildcardAnyAddr_b;

	/* Flag to store request for post Tx action for automatic socket */
    boolean                      lIsReconnectActionReq_b;

    /* Variable to store the port for static socket */
    uint16                       lStaticSocPort_u16;

	SoAd_CopyTxMetaData_tst      lCopyTxMetaData_st;

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
    /* variable to store the return value of internal function call */
    Std_ReturnType lRetVal_u8;

    /* Initialization of local variables declared above */
    lRetVal_u8              = E_NOT_OK;
#endif/* (SOAD_NUM_TX_IF_TCP_PDU > 0) */

    lIsReconnectActionReq_b = FALSE;

    /* Get "the dynamic socket index value" from the static socket table  */
    lIdxDynSocket_uo        = ( *SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo );

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo       = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* [SWS_SoAd_00731] */
    /* If SoAd_IfTransmit was called with PduInfoPtr->SduDataPtr set to NULL_PTR, SoAd shall use <Up>_[SoAd][If]TriggerTransmit>() to retrieve the PDU data from the upper layer. */

      if( (SoAdSrcPduInfo_cpst->SduDataPtr != NULL_PTR)
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
        ||  ((SoAdSrcPduInfo_cpst->SduDataPtr == NULL_PTR) && (SoAd_CurrConfig_cpst->SoAd_IfTriggerTransmitCallbacks_cpst != NULL_PTR))
#endif /* SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF */
        )
       {

#ifdef SOAD_DEBUG_AND_TEST
        SoAd_IfTransmissionOnGoing_tst = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b;
#endif

        /* Update "the index of the txpdu" in dynamic config table */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo = SoAdSrcPduId_uo ;

        /* Load the txPduHeaderId_u32 to dynamic table to use it in SoAd_CopyTxData( ) function */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduHeaderId_u32  = PduRouteDest_cpst->txPduHeaderId_u32;

        /* Load the txPduInfoPtr of the dynamic soc connection with SoAdSrcPduInfoPtr,
         * This can be used for retrieval of data transmission in SoAd_copyTxData function call */
        /* Load the txPduInfoPtr of the dynamic soc connection with SoAdSrcPduInfoPtr provided by Upper layer */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst      = (const PduInfoType*) SoAdSrcPduInfo_cpst;

		lCopyTxMetaData_st.TxPduId_uo 	 	= SoAdSrcPduId_uo;
		lCopyTxMetaData_st.idxStaticSoc_uo 	= StaticSocketId_uo;
		lCopyTxMetaData_st.txPduInfoPtr_cpst  =  (const PduInfoType*) SoAdSrcPduInfo_cpst;

        switch( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en )
        {
#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
            /* Enter into the condition if the configured protocol is TCP */
            case TCPIP_IPPROTO_TCP:
            {   /* Not supported for ICAM 200 */

                lRetVal_u8 = SoAd_IfTcpTransmit(lIdxDynSocket_uo, PduLength_uo );

                /* covert the SoAd_IfTcpTransmit retrun value to SoAd_IfTransmitReqStatus_ten */
                lFunctionRetVal_en = (lRetVal_u8 == E_OK)? SOAD_IF_TRANSMISSION_OK: SOAD_IF_TRANSMISSION_E_NOT_OK;

                break;
            }

#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
            /* Enter into the condition if the configured protocol is UDP */
            case TCPIP_IPPROTO_UDP:
            {
                lFunctionRetVal_en = SoAd_IfUdpTransmit( StaticSocketId_uo, PduLength_uo, PduRouteDest_cpst, &lCopyTxMetaData_st );
                break;
            }

#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */
            /* Invalid Protocol*/
            default:
            {
                /* lFunctionRetVal_en set to SOAD_IF_TRANSMISSION_E_NOT_OK since this socket has invalid protocol.*/
                /*It may have been updated to E_OK in previous successful trasmission*/
                lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;

                /* Update "the index of the txpdu" in dynamic config table */
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo   = SOAD_CFG_PDU_ID_DEFAULT_VALUE ;
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst  = NULL_PTR ;
                break;
            }
        }

        lStaticSocConfigHasWildcardAnyAddr_b = SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( ((const SoAd_StaticSocConfigType_tst *) &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])) );
        lStaticSocPort_u16                   = SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG( ((const SoAd_StaticSocConfigType_tst *) &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])) );

        /* If the transmission is successful and socket is Automatic, do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
        if( ( SOAD_IF_TRANSMISSION_OK == lFunctionRetVal_en ) && \
             ( ( TRUE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo].soConGrpAutoConSetup_b ) && \
              ( ( TCPIP_PORT_ANY == lStaticSocPort_u16 ) || lStaticSocConfigHasWildcardAnyAddr_b ) ))
        {
            lIsReconnectActionReq_b = TRUE;
        }

        /* Reset it to zero, so that SoAd_CopyTxData function will copy the data from 0th index of the buffer */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTxCopiedLength_uo = 0;

        /* reset the ifTpTxSoAdHdrCopied_b flag to FALSE */
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTpTxSoAdHdrCopied_b = FALSE;

    }

    /* If SduDataPtr is a NULL_PTR and Trigger Transmit is Null pointer is non supported use case */
    else
    {
        /* Report DET error */
        SOAD_DET_REPORT_ERROR( SOAD_IF_TRANSMIT_API_ID, SOAD_E_INV_ARG )

        /* Set the return value to NOT_OK */
        lFunctionRetVal_en = SOAD_IF_TRANSMISSION_E_NOT_OK;
    }
    if( ( TRUE == lIsReconnectActionReq_b ) || ( SOAD_EVENT_RECONNECTREQ_SOAD == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConRequestedMode_en) )
    {
        /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
        SchM_Enter_SoAd_TxRxReq();

        /*Check if there is on going Rx or IF Tx operation on this socket,If so perform post PDU operation in main function
         * to avoid race around condtion  */
        if((SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].rxOngoingCnt_u8 == 0)
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
                && (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b == FALSE)
#endif
           )
        {
#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
            /* If protocol is UDP change the socket to bound and overwrite remote address with configured */
            if( TCPIP_IPPROTO_UDP == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].soConGrpProtocol_en )
            {
                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();
                /*  do as per SWS_SoAd_00582 and SWS_SoAd_00644 */
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en = SOAD_SOCON_BOUND;

                /* Set the connection request state to reconnect such that it shall be tried in next main function */

                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en = SOAD_SOCON_RECONNECT;

                SchM_Exit_SoAd_SocConModeProperties();

                /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
                SchM_Enter_SoAd_RemoteAddrUpdate();
                SoAd_CopyRemoteAddrFromStaticSocToDynSoc( ((const SoAd_StaticSocConfigType_tst *) \
                        &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo])), & SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

                SchM_Exit_SoAd_RemoteAddrUpdate();

                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b = FALSE;
            }
#endif/*( SOAD_NUM_TX_IF_UDP_PDU > 0 ) */

            SchM_Exit_SoAd_TxRxReq();

            /* Give mode change call back to UL */
            SoAd_PerformPostPduTransmissionAction(StaticSocketId_uo);

        }
        else
        {
            /* Block further Tx and Rx*/
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

    return lFunctionRetVal_en;
}
#endif /* ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 ) */

#if (SOAD_NUM_TX_IF_TCP_PDU > 0)
/**
 **************************************************************************************************************************************
 * \Function Name : SoAd_IfTcpTransmit()
 *
 * \Function description
 * This function shall call the TcpIp_TcpTransmit() api to tranmit the PDU to lower layer and update the globals tables accordingly
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     DynSocketId_uo      - Index of SoAd Dynamic Socket identifier table
 * \param   PduLengthType
 * \arg     PduLength_uo        - No of bytes to be transmitted
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * \return  Std_ReturnType
 * \retval   E_OK               - PDU has been transmitted successfully
 *           E_NOT_OK           - PDU has not been transmitted
 *
 ***************************************************************************************************************************************
 */
static Std_ReturnType SoAd_IfTcpTransmit( SoAd_SoConIdType DynSocketId_uo, \
                                          PduLengthType    PduLength_uo )
{
    /* variable to store the return value of TcpIp_TcpTransmit function call */
    Std_ReturnType lRetVal_TcpTransmit_u8;

    /* Variable to hold the Index of the Tx Pdu table */
    PduIdType      lIdxTxPduTable_uo;

    /* Initialize the local variables which are declared above */
    lRetVal_TcpTransmit_u8      = E_NOT_OK;

    /* Get the index of Tx Pdu Id from Dynamic table */
    lIdxTxPduTable_uo           = SoAd_DyncSocConfig_ast[DynSocketId_uo].idxTxPduTable_uo;

    /* ForceRetrieve : Indicates how the TCP/IP stack retrieves data from SoAd.
     * FALSE: The TCP/IP stack shall retrieve the whole data indicated by lTotalPduLength_uo from the upper layer
     *      via one or multiple SoAd_CopyTxData() calls within the context of this transmit function.
     * TRUE: The TCP/IP stack may retrieve up to lTotalPduLength_uo data from the upper layer.
     *      It is allowed to retrieve less than lTotalPduLength_uo bytes.
     *  Note: Not retrieved data will be provided by SoAd
     *      with the next call to TcpIp_TcpTransmit (along with new data if available). */

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00542]  ***************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** In case of a TCP socket connection the SoAd shall call TcpIp_TcpTransmit() with SocketId specified in     ***/
    /** the SocketConnection, the PDU length specified in the SoAd_IfTransmit() call, as AvailableLength and      ***/
    /**             ForceRetrieve set to TRUE                                                                     ***/
    /****************************************************************************************************************/
    /****************************************************************************************************************/

    /* call to initiate the transmission by TcpIp_TcpTransmit */
    lRetVal_TcpTransmit_u8 = TcpIp_TcpTransmit( SoAd_DyncSocConfig_ast[DynSocketId_uo].socketHandleTcpIp_uo, NULL_PTR, PduLength_uo, SOAD_FORCE_RETRIEVE_TRANSMISSION_TRUE );

    if( E_OK == lRetVal_TcpTransmit_u8 )
    {
        /* Update the ifTxTcpPduLenReqByUL_uo with PduLength_uo to use it in SoAd_IfTcpTxConfirmationToUl() function */
        SoAd_DyncSocConfig_ast[DynSocketId_uo].ifTxTcpPduLenReqByUL_uo += PduLength_uo ;

        /* increment the ifTcpTxConfRemainingLen_au32 with PduLength_uo, so that SoAd can wait for Tcp to ACK this much bytes */
        SoAd_IfTcpPduTxPendingStatus_ast[SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPduTable_uo].txIfTcpPduIndex_u8].ifTcpTxConfRemainingLen_au32 += PduLength_uo;
    }
    else
    {
        /*Reset the Dynamic parameters */
        /* Update the idxTxPduTable_uo & txPduInfoPtr_cpst in dynamic config table to default */
        SoAd_DyncSocConfig_ast[DynSocketId_uo].idxTxPduTable_uo     = SOAD_CFG_PDU_ID_DEFAULT_VALUE ;
        SoAd_DyncSocConfig_ast[DynSocketId_uo].txPduInfoPtr_cpst    = NULL_PTR ;
    }

    return lRetVal_TcpTransmit_u8;
}
#endif /* SOAD_NUM_TX_IF_TCP_PDU > 0 */

#if ( SOAD_NUM_TX_IF_UDP_PDU > 0 )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_IfUdpTransmit()
 *
 * \Function description
 * This function shall call the TcpIp_UdpTransmit() api to tranmit the PDU to lower layer and update the globals tables accordingly
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo   - Index of SoAd Static Socket identifier table
 * \param   PduLengthType
 * \arg     PduLength_uo        - No of bytes to be transmitted
 * \param   const PduRouteDestType_tst*
 * \arg     PduRouteDest_cpst - A pointer to a structure of PduRouteDest to which packet needs to be transmitted.
 *                              it is passed as argument to fetch the txPduHeaderId_u32, triggerTransmission_en and txUdpTriggerTimeout_32_32
 *                              members of the PduRouteDestType_tst structure.
 *
 *
 * Parameters (inout): None
 *
 * Parameters (out):    None
 *
 * \return   SoAd_IfTransmitReqStatus_ten
 * \retval   SOAD_IF_TRANSMISSION_OK        - The request has been accepted and transmitted to lower layer successfully,
 *           SOAD_IF_TRANSMISSION_E_NOT_OK  - The request has not been accepted,
 *           SOAD_IF_NPDU_BUFFERING_OK      - The request has been accepted, but is is not transmitted to lower layer, instead
 *                                            it stored in corresponding nPduUdpTxBuffer since pdu is configured with TRIGGER_NEVER option,
 *
 ***************************************************************************************************
 */
static SoAd_IfTransmitReqStatus_ten SoAd_IfUdpTransmit( SoAd_SoConIdType             StaticSocketId_uo, \
                                                        PduLengthType                PduLength_uo, \
                                                        const PduRouteDestType_tst * PduRouteDest_cpst, \
														const SoAd_CopyTxMetaData_tst* CopyTxMetaData_cpst)
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst *      lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* variable to store the nPduUdpTxBuffer to local variable for further access */
    SoAd_nPduUdpTxInfo_tst *                  lnPduUdpTransmitInfo_pst;
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */

    /* Variable to hole buffer pointer to be transmitted */
    uint8 *                                   lbuffer_pu8;

    /* Structure to store the remote address information (if it is an IPv4 address) */
    TcpIp_SockAddrInetType                    lRemoteSockAddrInet_st;

    /* Structure to store the remote address information (if it is an IPv6 address) */
    TcpIp_SockAddrInet6Type                   lRemoteSockAddrInet6_st;

    /* Pointer to the structure holding the remote address */
    TcpIp_SockAddrType *                      lRemoteSockAddr_pst;

    /* Variable to hold the index of the dynamic socket connection table */
    SoAd_SoConIdType                          lIdxDynSocket_uo;

    /* Variable to hold SoAd_IfTransmitReqStatus_ten value */
    SoAd_IfTransmitReqStatus_ten              lFunctionRetVal_en;

    /* Variable to hold the return value of the TcpIp_UdpTransmit function call*/
    TcpIp_ReturnType                          lRetVal_UdpTransmit_en;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Get "the dynamic socket index value" from the static socket table  */
    lIdxDynSocket_uo        = ( *lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo );

    lFunctionRetVal_en      = SOAD_IF_TRANSMISSION_E_NOT_OK;

#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
    /* Check nPduUdpTxTableIndex_uo is valid, if so Pdu data should be pushed to nPdu functionality */
    if( lSoAdStaticSocConfig_cpst->nPduUdpTxTableIndex_uo < SOAD_CFG_nPDU_UDP_TX_DEFAULT_VALUE )
    {
        lnPduUdpTransmitInfo_pst = &(SoAd_CurrConfig_cpst->SoAdIf_nPduUdpTransmitInfo_pst[lSoAdStaticSocConfig_cpst->nPduUdpTxTableIndex_uo]);

        /* --------------------------------------------- */
        /* IF nPdu transmission                          */
        /* --------------------------------------------- */
        /* [SWS_SoAd_00546] */
        /* In case SoAdTxUdpTriggerMode is set to TRIGGER_NEVER for any PDU route (SoAdPduRouteDest) related to a socket connection and all upper layers belonging */
        /* to the related socket connection have SoAdTxUpperLayerType set to "IF", SoAd shall use the nPdu feature for this socket connection. (SRS_Eth_00116) */

        /* Execute the IF transmission with the nPdu feature */
		/* check if nPduTxTimer has not expired in context of main function*/
        if( (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst->SduDataPtr != NULL_PTR) \
            || ( (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst->SduDataPtr == NULL_PTR) &&\
                 (lnPduUdpTransmitInfo_pst->nPduUdpTxBuffer_cpu8 == NULL_PTR) ) )
        {

	        lFunctionRetVal_en = ( 0U != lnPduUdpTransmitInfo_pst->nPduUdpTxTimer_u32 ) ? \
	                SoAd_nPduUdpTransmit(StaticSocketId_uo, PduLength_uo, PduRouteDest_cpst) : SOAD_IF_TRANSMISSION_E_NOT_OK;
        }

    }
    else
#endif /* SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 */
    {
        /* --------------------------------------------- */
        /* IF normal transmission                        */
        /* --------------------------------------------- */

        /* Initialize the local variables which are declared above */
        lbuffer_pu8                     = NULL_PTR;

        /* Get static socket Group configuration pointers */
        lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo]);

        /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
        SchM_Enter_SoAd_RemoteAddrUpdate();
        /* Load the remoteAddr structure values from the socket connections */
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_BuildSockAddr( ((SoAd_DyncSocConfigType_tst *) &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])),
                              lSoAdStaticSoConGrpConfig_cpst->domain_en,
                              ((TcpIp_SockAddrInetType *) &lRemoteSockAddrInet_st),
                              ((TcpIp_SockAddrInet6Type *) &lRemoteSockAddrInet6_st),
                              &lRemoteSockAddr_pst );
        SchM_Exit_SoAd_RemoteAddrUpdate();

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00540] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** In case of a UDP socket connection the SoAd shall (if not specified otherwise) call TcpIp_UdpTransmit()   ***/
        /** with SocketId and remote address specified in the SocketConnection and the PDU length specified in the    ***/
        /**             SoAd_IfTransmit()call as TotalLength                                                          ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        if( FALSE == lSoAdStaticSoConGrpConfig_cpst->headerEnable_b )
        {
            /* If header is disabled, so no need to put header in front of data. So directly call TcpIp layer with data pointer.
             * If header is enabled call TcpIp layer with NULL pointer. So, SoAd_CopyTxData function will be called to insert the SoAd header */
            /* Load the buffer pointer to local variable */
            lbuffer_pu8   = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst->SduDataPtr;
        }

		if( NULL_PTR != lbuffer_pu8)
		{
			/* call to initiate the transmission by TcpIp_UdpTransmit */
			lRetVal_UdpTransmit_en = TcpIp_UdpTransmit( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo,
														lbuffer_pu8,
														lRemoteSockAddr_pst,
														(uint16)PduLength_uo );

			(void) CopyTxMetaData_cpst;
		}
		else
		{
			/* call to initiate the transmission by TcpIp_UdpTransmit */
		    /* MR12 DIR 1.1 VIOLATION: The input parameters of TcpIp_UdpTransmitMetaData() are declared as (const void*) for generic implementation of TcpIp_UdpTransmitMetaData */
			lRetVal_UdpTransmit_en = TcpIp_UdpTransmitMetaData( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo,
																NULL_PTR,
																lRemoteSockAddr_pst,
																(uint16)PduLength_uo,
															    (const void *) CopyTxMetaData_cpst );
		}
        /* Conversion of TcpIp_ReturnType to SoAd_IfTransmitReqStatus_ten */
        lFunctionRetVal_en = ( TCPIP_E_NOT_OK == lRetVal_UdpTransmit_en ) ? SOAD_IF_TRANSMISSION_E_NOT_OK : \
                                            SOAD_IF_TRANSMISSION_OK;
    }

    /* Reset the txPduInfoPtr_cpst and idxTxPduTable_uo to default for both success and failure scenario */
    /* Reset the txPduInfoPtr_cpst to null */
    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].txPduInfoPtr_cpst   = NULL_PTR ;

    /* Reset the txpduid in dynamic config table since the TxConfirmation was invalid for UDP*/
    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo  = SOAD_CFG_PDU_ID_DEFAULT_VALUE ;

    (void)PduRouteDest_cpst;
    return lFunctionRetVal_en;
}
#endif /* SOAD_NUM_TX_IF_UDP_PDU > 0 */

#if (  ( SOAD_NUM_TX_IF_UDP_PDU > 0 ) || ( SOAD_NUM_TX_IF_TCP_PDU > 0 )  )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_IfTransmitDetChk()
 *
 * \function description
 * This function is called from SoAd_IfTransmit to handle the DET error for function arguments.
 * It covers SWS_SoAd_00213 & SWS_SoAd_00214 requirements
 *
 * Parameters (in) :
 * \param   PduIdType
 * \arg     SoAdSrcPduId_uo - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                            and thereby specifying the socket to be used for transmission of the data.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfoPtr_cpuo : A pointer to a structure with socket related data: data length and pointer to a data buffer.
 *
 * Parameters (inout): None
 *
 * Parameters (out):   None
 *
 * Return value:        Std_ReturnType
 *                      E_OK: No Det reported
 *                      E_NOT_OK: DET reported
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_IfTransmitDetChk( PduIdType           SoAdSrcPduId_uo , \
                                             const PduInfoType * SoAdSrcPduInfoPtr_cpuo )
{

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_IF_TRANSMIT_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoAdSrcPduInfoPtr & SduDataPtr pointers are valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == SoAdSrcPduInfoPtr_cpuo ), SOAD_IF_TRANSMIT_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /* Check whether SoAdSrcPduId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noTxPdus_cuo <= SoAdSrcPduId_uo), SOAD_IF_TRANSMIT_API_ID, SOAD_E_INV_PDUID, E_NOT_OK )

    /* Check whether Pdu length is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAdSrcPduInfoPtr_cpuo->SduLength > SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].txPduLength_uo ) || \
                             ( SOAD_IF != SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].txUpperLayerType_en ) ), SOAD_IF_TRANSMIT_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

#if (STD_ON == SOAD_METADATA_ENABLED)

    /* Check whether Metadata pointer is valid only if Metadata item type SOCKET_CONNECTION_ID_16 is configured */
    SOAD_DET_CHECK_ERROR_RETVAL( ((TRUE == SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId_uo].isTxPduMetaDataCfgd_b) && \
                               (SoAdSrcPduInfoPtr_cpuo->MetaDataPtr == NULL_PTR)), SOAD_IF_TRANSMIT_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

#endif/* (STD_ON == SOAD_METADATA_ENABLED) */


    /********************************* End: DET Checking *************************************/
    return E_OK;

}
#endif

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
