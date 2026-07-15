


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Cbk.h"

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


#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
/*
 ***************************************************************************************************
 * Prototype for Static functions: End
 ***************************************************************************************************

 ****************************************************************************************************************************
 * \Function Name : SoAd_ZeroCopyIfTransmit()
 *
 * \function description
 * This API initiates a request for Zero Copy transmission of the L-PDU specified by the SoAdSrcPduId.
 *
 * Parameters (in) :
 * \param   PduIdType
 * \arg     SoAdSrcPduId - This parameter contains a unique identifier referencing to the PDU Routing Table
 *                          and thereby specifying the socket to be used for transmission of the data.
 * \param   const PduInfoType*
 * \arg     SoAdSrcPduInfoPtr : A pointer to a structure with socket related data: data length and pointer to a data buffer.
 *
 * \param  boolean
 * \arg    TxConfFlag_b :
 *
 * Parameters (inout): None
 *
 * Parameters (out):
 * \param  TcpIp_SocketIdType
 * \arg    TcpIpSocketId_u16 :  TcpIp Socket connection
 *
 * \param  uint32
 * \arg    TxId_pu32 :
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
Std_ReturnType SoAd_ZeroCopyIfTransmit( PduIdType                          SoAdSrcPduId , \
                                        const Eth_ZeroCopyTxDataInfo_tst * ZeroCopyData_pcst,\
                                        boolean                            TxConfFlag_b,\
                                        uint32 *                           TxId_pu32 )
{

    /* Variable to hold the return value of the TcpIp_UdpTransmit function call*/
    TcpIp_ReturnType                         lRetVal_ZeroCopyUdpTransmit_en;

    /* Variable to store the return value */
    Std_ReturnType                            lFunctionRetVal_u8;

    /* Variable to hold the index of the static socket connection table */
    SoAd_SoConIdType                          lIdxStaticsocket_uo;

    /* Variable to hold the index of the dynamic socket connection table*/
    SoAd_SoConIdType                          lIdxDynSocket_uo;

    /* variable to hold the total pdu length of the SoAd Frame */
    PduLengthType                             lTotalPduLength_uo;

    /* Structure to store the remote address information (if it is an IPv4 address) */
    TcpIp_SockAddrInetType                    lRemoteSockAddrInet_st;

    /* Structure to store the remote address information (if it is an IPv6 address) */
    TcpIp_SockAddrInet6Type                   lRemoteSockAddrInet6_st;

    /* Pointer to the structure holding the remote address */
    TcpIp_SockAddrType *                      lRemoteSockAddr_pst;

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst *      lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSoConGrpConfigType_tst * lSoAdStaticSoConGrpConfig_cpst;

    /* Pointer to hold the Pdu route destination structure */
    const PduRouteDestType_tst *              lPduRouteDest_cpst;

    /* Variable to store the submode */
    SoAd_SoConSubMode_ten                     lSoConSubMode_en;

    /* Variable to store the close req count */
    uint16                                    lsocConCloseReqCnt_u16;

    /* Variable to store the open req count */
    uint16                                    lsocConOpenReqCnt_u16;
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
    /* Routing Group Status */
    boolean                                   lRgStatus_b;
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8      = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_ZERO_COPY_IF_TRANSMIT_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoAdSrcPduInfoPtr & SduDataPtr pointers are valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == ZeroCopyData_pcst ), SOAD_ZERO_COPY_IF_TRANSMIT_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /* Check whether SoAdSrcPduId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noTxPdus_cuo <= SoAdSrcPduId ), SOAD_ZERO_COPY_IF_TRANSMIT_API_ID, SOAD_E_INV_PDUID, E_NOT_OK )

    /* Check whether Pdu length is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL(  ( ZeroCopyData_pcst->DataLen_u16 > SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].txPduLength_uo ), \
                                    SOAD_ZERO_COPY_IF_TRANSMIT_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

      /********************************* Start: DET Checking *************************************/

    /* Condition checks:
     * 1. If there is no DET error if DET is switch On.
     * 2. UL of the Pdu should be type IF.
     * 3. Check if txIfTcpPduIndex_u8 is 0xFF, it will be 0xFF for all UDP sockets
     * 4. Check If txIfTcpPduIndex_u8 is != 0xFF and remaining length to be transmitted is 0, otherwise different pdu's data will be merged.
     * 5. SoAd_ULIfUdpTxConfirmFlag_ab falg is FALSE.
     */

        /* Extract pointer to first element of Pdu route destination structure */
        lPduRouteDest_cpst = ( SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[SoAdSrcPduId].PduRouteDestConf_cpst );

        lIdxStaticsocket_uo = lPduRouteDest_cpst[0].txStaticSocketOrGroupIdx_uo;


        /* Below condition would be valid for the following scenario,
         * 1. If Socket reference is linked to socket connection group and only one socket connection configured for the group
         * 2. If Socket reference is linked to socket connection directly
         */
        if( SOAD_CFG_SOCON_ID_DEFAULT_VALUE != lIdxStaticsocket_uo )
        {
            /* Initialize the local variables which are declared above */
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

                /* Calculate the total length */
                lTotalPduLength_uo     =   ZeroCopyData_pcst->DataLen_u16;

                /*[SWS_SoAd_00539] Call the related TcpIp transmit function depending on the connection type if the PDU length > 0,
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

                if( ( SOAD_SOCON_ACTIVE == lSoConSubMode_en) && ( ( 0U == lsocConCloseReqCnt_u16) || ( lsocConCloseReqCnt_u16 < lsocConOpenReqCnt_u16) ) )
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
                    SchM_Enter_SoAd_TxRxReq();

                    if( ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b )
#if (SOAD_IF_PRESENT != STD_OFF )
                    &&  ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
#endif
                    &&  ( ( SOAD_CFG_PDU_ID_DEFAULT_VALUE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo )  ||
                          ( SoAdSrcPduId == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo ) ) )
                    {

                        /* [SWS_SoAd_00731] */
                        /* If SoAd_IfTransmit was called with PduInfoPtr->SduDataPtr set to NULL_PTR, SoAd shall use <Up>_[SoAd][If]TriggerTransmit>() to retrieve the PDU data from the upper layer. */

                        /* Check if PduInfoPtr->SduDataPtr is not NULL*/
                        /* (other cases of IfTransmit called with SduDataPtr as NULL_PTR are not implemented yet) */
                        /* Enter into the condition if the configured protocol is UDP */
                        if((ZeroCopyData_pcst->DataBufferList_ppu8 != NULL_PTR) && \
                                (SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo].soConGrpId_uo ].soConGrpProtocol_en == TCPIP_IPPROTO_UDP))

                        {
                            /* Set the transmitReq of the dynamic soc connection to be set,
                             * such that it blocks the further transmission request of IF type on this socket connection */
#if (SOAD_IF_PRESENT != STD_OFF )
                            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = TRUE;
#endif

                            SchM_Exit_SoAd_TxRxReq();
                            /* Update "the index of the txpdu" in dynamic config table */
                            SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo = SoAdSrcPduId ;

                            /* Get static socket configuration pointers */
                            lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticsocket_uo]);

                            /* Get static socket Group configuration pointers */
                            lSoAdStaticSoConGrpConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo]);

                            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
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


                            /* call to initiate the transmission by TcpIp_UdpTransmit */
                            lRetVal_ZeroCopyUdpTransmit_en = TcpIp_ZeroCopyUdpTransmit( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo,
                                                                       ZeroCopyData_pcst,
                                                                       lRemoteSockAddr_pst,
                                                                       TxConfFlag_b,
                                                                       SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ZeroCopyConnChanged_b,
                                                                       TxId_pu32 );


                           /* Conversion of TcpIp_ReturnType to SoAd_IfTransmitReqStatus_ten */
                           if(TCPIP_OK == lRetVal_ZeroCopyUdpTransmit_en)
                           {
                               lFunctionRetVal_u8 = E_OK;
                               SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ZeroCopyConnChanged_b=FALSE;
                           }

#if (SOAD_IF_PRESENT != STD_OFF )
                           /* Set the transmitReq of the dynamic soc connection to FALSE */
                           SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b = FALSE;
#endif
                       }

                       /* If SduDataPtr is a NULL_PTR in non supported use case */
                       else
                       {
                           SchM_Exit_SoAd_TxRxReq();
                           /* Report DET error */
                           SOAD_DET_REPORT_ERROR( SOAD_ZERO_COPY_IF_TRANSMIT_API_ID, SOAD_E_INV_ARG )
                       }

                    }
                    else
                    {
                        SchM_Exit_SoAd_TxRxReq();
                    }

                }
            }


        }

    return(lFunctionRetVal_u8);

}

#endif /* ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF) */

/**
 ***************************************************************************************************
 * \Function Name : SoAd_ZeroCopyTxConfirmation()
 *
 * \Function description
 * The TCP/IP stack calls this function after the data has been acknowledged by the peer for TCP
 *
 *  Parameters (in):
 * \param   TcpIp_SocketIdType
 * \arg     SocketId_u16 - Socket identifier of the related local socket resource.
 * \param   uint32
 * \arg     TxId_u32 - variable to store index of TxPdu config table.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 ***************************************************************************************************
 */
void SoAd_ZeroCopyTxConfirmation  ( TcpIp_SocketIdType SocketId_u16,
                                    uint32             TxId_u32 )
{
#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
    /* Local pointer to store the Tx Pdu table for given index */
     const SoAd_TxPduConfigType_tst * lSoAdTxPduConfig_cpst ;
    /* variable to store index of TxPdu config table  */
    PduIdType                         lIdxTxPdu_uo;

    /* variable to store the index of dynamic socket connection table */
    SoAd_SoConIdType                  lIdxDynSocket_uo;

    /* Initialization of Local variables */
    lIdxTxPdu_uo            = SOAD_CFG_PDU_ID_DEFAULT_VALUE;

    /* Get the index of dynamic config table by using array and SocketId as index */
    lIdxDynSocket_uo        = SoAd_TcpIpSocHandleToDynSockIdx_auo[SocketId_u16];

    /* Condition check: the dynamic socket id should be valid  */
    if( lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo )
    {

        /* Get the "index of the tx pdu config table" from dynamic table */
        lIdxTxPdu_uo = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo;
        /* Get the Tx Pdu pointer for given index */
        lSoAdTxPduConfig_cpst   = &(SoAd_CurrConfig_cpst->SoAd_TxPduConfig_cpst[lIdxTxPdu_uo]);

        /* Call UL_SoAdZeroCopyTxConfirmation */
        if(NULL_PTR != SoAd_CurrConfig_cpst->SoAdIf_ZeroCopyTxConfirms_cpst[lSoAdTxPduConfig_cpst->zeroCopyCallback_u8].ZeroCopyTxConfirm_pfn)
          {
              /* Call back the upper layer with txPduId to inform about zeroCopyTxConfirmation  */
              SoAd_CurrConfig_cpst->SoAdIf_ZeroCopyTxConfirms_cpst[lSoAdTxPduConfig_cpst->zeroCopyCallback_u8].ZeroCopyTxConfirm_pfn(lIdxTxPdu_uo, TxId_u32);
          }

        /* Reset the txpduid in dynamic config table since the TxConfirmation was invalid for UDP*/
        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].idxTxPduTable_uo  = SOAD_CFG_PDU_ID_DEFAULT_VALUE ;
    }
#else
        (void)SocketId_u16;
        (void)TxId_u32;
#endif /* ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF) */
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
