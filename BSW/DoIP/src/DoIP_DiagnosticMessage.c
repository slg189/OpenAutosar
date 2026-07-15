

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessageCheckSaTaConfiguration
 Description      : This function checks, whether the SA and TA are known.
***********************************************************************************************************************/
static boolean DoIP_DiagnosticMessageCheckSaTaConfiguration(uint16 sa_u16, uint16 ta_u16)
{
    boolean retVal_b = FALSE;
    uint8 raIdx_u8;
    uint8 testerIdx_u8;
	uint8 k_u8;
	uint8 l_u8;

    if (DoIP_RoutingActivation_IsSourceAddressKnown(sa_u16, &testerIdx_u8))
    {
        for (k_u8 = 0; k_u8 < DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].numOfRoutingActivations_u8; k_u8++)
        {
            raIdx_u8 =
                    DoIP_activeConfig_pcst->raJT_pcst[(DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].routingActivationJumpTableIndex_u16
                            + k_u8)].routingActivationIdx_u8;

            for (l_u8 = 0; l_u8 < DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].numOfTargetAddresses_u8; l_u8++)
            {
                if (DoIP_activeConfig_pcst->targetAddressJT_pcst[DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].targetAddressJumpTableIdx_u16
                        + l_u8].targetAddress_u16 == ta_u16)
                {
                    retVal_b = TRUE;
                }
            }
        }
    }

    return retVal_b;
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessageCheckSaTaAtConfiguration
 Description      : This function checks, whether the SA and TA are known. Additionally this function checks, whether this
                    SA/TA combination is supported by the according RA
***********************************************************************************************************************/
boolean DoIP_DiagnosticMessageCheckSaTaAtConfiguration(uint16 sa_u16, uint16 ta_u16, const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst)
{
    boolean retVal_b = FALSE;
    uint8 raIdx_u8;
    uint8 testerIdx_u8;
    uint8 k_u8;
    uint8 l_u8;


    if (DoIP_RoutingActivation_IsSourceAddressKnown(sa_u16, &testerIdx_u8))
    {
        for (k_u8 = 0; k_u8 < DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].numOfRoutingActivations_u8; k_u8++)
        {
            raIdx_u8 =
                    DoIP_activeConfig_pcst->raJT_pcst[(DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].routingActivationJumpTableIndex_u16
                            + k_u8)].routingActivationIdx_u8;

            if (raCfg_pcst != NULL_PTR)
            {
                if (DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].routingActivationNumber_u8 == raCfg_pcst->routingActivationNumber_u8)
                {
                    for (l_u8 = 0; l_u8 < DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].numOfTargetAddresses_u8; l_u8++)
                    {
                        if (DoIP_activeConfig_pcst->targetAddressJT_pcst[DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].targetAddressJumpTableIdx_u16
                                + l_u8].targetAddress_u16 == ta_u16)
                        {
                            retVal_b = TRUE;
                        }
                    }

                    /* break the loop if routing activation number is matched */
                    break;
                }
            }
        }
    }

    return retVal_b;
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_CheckAndSetChannel
 Description      : This function gets with the SA/TA combination the according channel
***********************************************************************************************************************/
static boolean DoIP_DiagnosticMessage_CheckAndSetChannel(uint16 sa_u16, uint16 ta_u16, uint16* channel_pu16)
{
    boolean retVal_b = FALSE;
    uint16 idx_u16;

    for (idx_u16 = 0; idx_u16 < DoIP_activeConfig_pcst->channelSize_u16; idx_u16++)
    {
        if ((DoIP_activeConfig_pcst->testerCfg_pcst[DoIP_activeConfig_pcst->channel_pcst[idx_u16].testerIdx_u8].testerSA_u16 == sa_u16)
                && (DoIP_activeConfig_pcst->channel_pcst[idx_u16].targetAddr_u16 == ta_u16))
        {
            /* Only channels which have valid PDUID to PduR are allowed */
            if (DoIP_activeConfig_pcst->channel_pcst[idx_u16].doIPPduRRxPduId_uo != DOIP_INVALID_PDUID)
            {
                *channel_pu16 = idx_u16;
                retVal_b = TRUE;
                break;
            }
        }
    }

    return retVal_b;
}

/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_CheckHeader
 Description      : This function checks the SA/TA combination and do some related checks.
***********************************************************************************************************************/
static uint8 DoIP_DiagnosticMessage_CheckHeader(const uint8* DiagMsgMinPayloadBuf_cpu8, DoIP_Type_TCPConnection* tcpCon_pst)
{
    uint16 sourceAddr_u16;
    uint16 targetAddr_u16;
    uint8 retVal_u8 = DOIP_DM_NACKCODE_UNDEFINED;

    sourceAddr_u16 = DoIP_PRV_READ_16BITS_AT_POS( DiagMsgMinPayloadBuf_cpu8, DoIP_PRV_DIAG_MSG_SRC_ADDR_IN_PAYLOAD_POS );
    targetAddr_u16 = DoIP_PRV_READ_16BITS_AT_POS( DiagMsgMinPayloadBuf_cpu8, DoIP_PRV_DIAG_MSG_TRG_ADDR_IN_PAYLOAD_POS );

    //check tester SA is registered to established connection -> nack code 0x02, close socket
    if (!DoIP_RoutingActivation_IsSourceAddressActive(&tcpCon_pst->ra_st, sourceAddr_u16))
    {
        retVal_u8 = DOIP_DM_NACKCODE_INVALID_SA;
    }
    //check TA & SA are connected with current activation -> nack 0x03, discard msg
    else if (!DoIP_DiagnosticMessageCheckSaTaConfiguration(sourceAddr_u16, targetAddr_u16))
    {
        retVal_u8 = DOIP_DM_NACKCODE_UNKNOWN_TA;
    }
    //check payload length is bigger than doIPMaxrequestBytes -> nack 0x04, discard msg
    else if (tcpCon_pst->RX_st.totalPayloadLength_u32 > (DOIP_CFG_MAX_REQUEST_BYTES - 4u))
    {
        retVal_u8 = DOIP_DM_NACKCODE_MSG_TOO_LARGE;
    }
    //check TA is activated -> nack 0x06, discard msg
    else if (!DoIP_DiagnosticMessageCheckSaTaAtConfiguration(sourceAddr_u16, targetAddr_u16, tcpCon_pst->ra_st.raCfg_pcst))
    {
        retVal_u8 = DOIP_DM_NACKCODE_TA_UNREACHABLE;
    }
    else if (!DoIP_DiagnosticMessage_CheckAndSetChannel(sourceAddr_u16, targetAddr_u16, &tcpCon_pst->RX_st.channelIdx_u16))
    {
        retVal_u8 = DOIP_DM_NACKCODE_TA_UNREACHABLE;
    }
    else
    {
        retVal_u8 = DOIP_DM_ACKCODE_SUCCESS;
    }

    return retVal_u8;
}

/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_PrepareAcknowledgeMessage
 Description      : This function prepare the NACK/ACK message. the response code will be skipped and updated later.
***********************************************************************************************************************/
static void DoIP_DiagnosticMessage_PrepareAcknowledgeMessage(const uint8* saTa_pcu8, const uint8* payload_pcu8, PduLengthType sduLength_u16,
        uint16 ackLength_u16, uint8* msg_pu8)
{
    uint16 payloadType_u16 = DOIP_PAYLOAD_TYPE_DIAG_MSG_ACK;
    uint32 payloadLength_u32 = DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN + 1u + ackLength_u16; // "+ 1u" for resp code
    uint16 SourceAddr_u16;
    uint16 TargetAddr_u16;
    uint8  MsgCurrPos_u8;

    TargetAddr_u16 = DoIP_PRV_READ_16BITS_AT_POS(saTa_pcu8, DoIP_PRV_DIAG_MSG_SRC_ADDR_IN_PAYLOAD_POS );
    SourceAddr_u16 = DoIP_PRV_READ_16BITS_AT_POS(saTa_pcu8, DoIP_PRV_DIAG_MSG_TRG_ADDR_IN_PAYLOAD_POS );

    /* clean buffer (usefull if available data is less than requested by ackLength) - "+ 1u" for resp code */
    DOIP_MEMSET( (msg_pu8 + DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN), 0, (ackLength_u16 + 1u) );

    /* Header */
    DoIP_Header_Write(payloadType_u16, payloadLength_u32, msg_pu8);
    MsgCurrPos_u8 = DOIP_HEADER_SIZE;

    /* Payload */
    /* copy source and target address */
    DoIP_PRV_WRITE_16BITS_AT_POS( msg_pu8, ( MsgCurrPos_u8 + DoIP_PRV_DIAG_MSG_SRC_ADDR_IN_PAYLOAD_POS ), SourceAddr_u16 )
    DoIP_PRV_WRITE_16BITS_AT_POS( msg_pu8, ( MsgCurrPos_u8 + DoIP_PRV_DIAG_MSG_TRG_ADDR_IN_PAYLOAD_POS ), TargetAddr_u16 )

    /* 1 to skip resp code - set in DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage */
    MsgCurrPos_u8 += DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN + 1u;

    /* acknowledgment data */
    ackLength_u16 = DOIP_MIN(ackLength_u16, sduLength_u16);
    if (ackLength_u16 != 0)
    {
        DOIP_MEMCPY((msg_pu8 + MsgCurrPos_u8), payload_pcu8, ackLength_u16);
    }
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage
 Description      : This function trigger the ACK/NACK message. In case of blocked TX connection, the messages will be stored
                    into the accordning ACK/NACK buffer.
***********************************************************************************************************************/
void DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint8 respCode_u8, uint16 channel_u16, uint8* ackBuffer_pu8)
{
    ackBuffer_pu8[DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN] = respCode_u8;

    // set state to send message
    if (DOIP_DM_ACKCODE_SUCCESS != respCode_u8)
    {
        DoIP_Header_WriteOnlyPayloadType(DOIP_PAYLOAD_TYPE_DIAG_MSG_NACK, ackBuffer_pu8);
        // Discard the next messages, if this not all data already received
        if (tcpCon_pst->RX_st.payloadLengthCounter_u32 < tcpCon_pst->RX_st.totalPayloadLength_u32)
        {
            tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_DISCARD_MESSAGE_E;
        }
        else
        {
            tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
        }
    }
    else
    {
        // complete message was received and forwarded to upper layer, go back to state DOIP_CHANNEL_NEW_MESSAGE_E to receive new messages
        tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
    }

    // send only NACK in case of singleframe message. In multiframe case, DoIP waits until all frames received and sends for the last segment the nack
    if (tcpCon_pst->RX_st.channelState_en != DOIP_CHANNEL_DISCARD_MESSAGE_E)
    {
        // try to send NACK directly, if not then store into channel buffer and retry in the next mainfunction again
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, tcpCon_pst->RX_st.channelIdx_u16, ackBuffer_pu8, FALSE) )
        {
            if (DOIP_DM_ACKCODE_SUCCESS != respCode_u8)
            {
                if(DOIP_CFG_CHANNEL_ARRAY_SIZE > channel_u16)
                {
                    DoIP_Channel_ast[channel_u16].channelACKState_en = DOIP_CHANNEL_TX_SENDING_E;
                }
                else
                {
                    tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_SENDING_E;
                }
            }
            else
            {
                if(DOIP_CFG_CHANNEL_ARRAY_SIZE > channel_u16)
                {
                    DoIP_Channel_ast[channel_u16].channelACKState_en = DOIP_CHANNEL_TX_SENDING_E;
                }
            }
        }
        else
        {
            if (DOIP_DM_ACKCODE_SUCCESS != respCode_u8)
            {
                if(DOIP_CFG_CHANNEL_ARRAY_SIZE > channel_u16)
                {
                    DoIP_Channel_ast[channel_u16].channelACKState_en = DOIP_CHANNEL_TX_READY2SEND_E;
                }
                else
                {
                    tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_BLOCKED_E;
                }
            }
            else
            {
                if(DOIP_CFG_CHANNEL_ARRAY_SIZE > channel_u16)
                {
                    DoIP_Channel_ast[channel_u16].channelACKState_en = DOIP_CHANNEL_TX_READY2SEND_E;
                }
            }
        }
    }
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_AcknowledgeTxConfirmation
 Description      : This function handles the TXConfirmation of a ACK/NACK message and release according buffer in tcpCon
                    or channel. In case of a NACK, because of invalid SA, than we close the socket.
***********************************************************************************************************************/
void DoIP_DiagnosticMessage_AcknowledgeTxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst)
{
    uint8 responseCode_u8;

    if(DOIP_CFG_CHANNEL_ARRAY_SIZE > tcpCon_pst->TX_st.channelIdx_u16 )
    {
        DoIP_Channel_ast[tcpCon_pst->TX_st.channelIdx_u16].channelACKState_en = DOIP_CHANNEL_TX_IDLE_E;
        tcpCon_pst->TX_st.channelIdx_u16 = DOIP_INVALID_PDUID;
    }
    else
    {
        if(DOIP_TCP_TX_WAIT4CONFIRMATION_E == tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en)
        {
            responseCode_u8 = tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8[DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN];

            /* Tcp connection should be closed if responseCode_u8 is DOIP_DM_NACKCODE_INVALID_SA(0x02) as per [SWS_DoIP_00123] */
            if(DOIP_DM_NACKCODE_INVALID_SA == responseCode_u8)
            {
                /* Set flag to close connection */
                DoIP_TCPConnection_SetCloseNeeded(tcpCon_pst);
            }

            tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_READY_E;
        }
    }

    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_TxConfirmation
 Description      : This function handles the TXConfirmation of a diagnostic message and release the Channel buffer
***********************************************************************************************************************/
void DoIP_DiagnosticMessage_TxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst, Std_ReturnType result_u8)
{
    DoIP_Channel_Release(tcpCon_pst->TX_st.channelIdx_u16, result_u8);
    tcpCon_pst->TX_st.channelIdx_u16 = DOIP_INVALID_PDUID;
    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_CopyDataToPduR
 Description      : This function trigger the copying of data to pduR. In case of singleframe message DoIP trigger
                    directly the PduR_DoIPRxIndication and try to send the ACK/NACK. In case of multiframe message,
                    DoIP goes into multiframe state and expect the next frame in the the next CopyRxData call.
***********************************************************************************************************************/
static BufReq_ReturnType DoIP_DiagnosticMessage_CopyDataToPduR(DoIP_Type_TCPConnection *tcpCon_pst,
        PduInfoType* pduInfo_ptst, PduIdType pdurPdu_uo, PduLengthType sduLength_u16, PduLengthType* bufferSize_puo)
{
    BufReq_ReturnType retVal_en = BUFREQ_OK;
    PduLengthType UpLayerBufferSize_uo;
    PduLengthType sduLengthCounter_tu16 = 0;

    /* Check, whether the upper layer has a smaller buffer, than the payload length,
     * if yes, than transmit only the amount of bytes which is equal to lastKnownBufferSize */
    pduInfo_ptst->SduLength = DOIP_MIN(sduLength_u16, tcpCon_pst->RX_st.lastKnownBufferSize_uo);

    /* Transmit everything we have, possibly in several loops */
    while ((retVal_en == BUFREQ_OK) && (pduInfo_ptst->SduLength > 0))
    {
        UpLayerBufferSize_uo = 0;

        retVal_en = PduR_DoIPCopyRxData(pdurPdu_uo, pduInfo_ptst, &UpLayerBufferSize_uo);

        if (retVal_en == BUFREQ_OK)
        {
            /* Remember how many bytes have already been transmitted */
            sduLengthCounter_tu16 += pduInfo_ptst->SduLength;
            /* jump to place, where the next package is located */
            pduInfo_ptst->SduDataPtr += pduInfo_ptst->SduLength;
            /* get the sduLength of the next message, which can be send */
            pduInfo_ptst->SduLength = DOIP_MIN(UpLayerBufferSize_uo, (sduLength_u16 - sduLengthCounter_tu16));

            *bufferSize_puo = UpLayerBufferSize_uo;
            tcpCon_pst->RX_st.lastKnownBufferSize_uo = UpLayerBufferSize_uo;

            /* No more space left, but still data to copy -> error */
            if ((UpLayerBufferSize_uo == 0) && (sduLengthCounter_tu16 != sduLength_u16))
            {
                retVal_en = BUFREQ_E_NOT_OK;
            }
        }
    }

    if (retVal_en == BUFREQ_OK)
    {
        /* is message complete send? */
        if (tcpCon_pst->RX_st.payloadLengthCounter_u32 == tcpCon_pst->RX_st.totalPayloadLength_u32)
        {

            DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(tcpCon_pst,
                                                                 DOIP_DM_ACKCODE_SUCCESS,
                                                                 tcpCon_pst->RX_st.channelIdx_u16,
                                                                 DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8);
            PduR_DoIPRxIndication(pdurPdu_uo, E_OK);
        }
        /* another part needs ot be transmitted */
        else
        {
            tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_MULTIFRAME_MESSAGE_E;
        }
    }

    return retVal_en;
}

/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_UpdateAcknowledgeMessageData
 Description      : This function will update the payload for the NACK/ACK message in case of multiframe message
***********************************************************************************************************************/
/* MR12 RULE 8.13 VIOLATION: The tool does not recognize that the pointer is used in a memcopy macro and data is written, this is a tool issue */
void DoIP_DiagnosticMessage_UpdateAcknowledgeMessageData(const uint8* sdu_pcu8, PduLengthType pdurSduLength_uo,
        const DoIP_Type_TCPConnection *tcpCon_pst)
{
    /* update acknowledgement data if necessary */
    uint32 bytesToAdd_u32 = 0;
    uint32 ackLength_u32 = DoIP_RoutingActivation_GetAckNackResponseDataLength(tcpCon_pst->ra_st.testerCfg_pcst);

    /* something left to add? */
    if (ackLength_u32 > (tcpCon_pst->RX_st.payloadLengthCounter_u32 - DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN))
    {
        /* calculate number of bytes to add */
        bytesToAdd_u32 = ackLength_u32 - (tcpCon_pst->RX_st.payloadLengthCounter_u32 - DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN);
        bytesToAdd_u32 = DOIP_MIN(bytesToAdd_u32, pdurSduLength_uo);
        if (bytesToAdd_u32 != 0u)
        {
            /* copy data after the position of response code */
            DOIP_MEMCPY(
                    DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8 + DOIP_MSG_POS_CONTENT + 1u + tcpCon_pst->RX_st.payloadLengthCounter_u32,
                    sdu_pcu8,
                    bytesToAdd_u32);
        }
    }
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_ContinuePreviousMessage
 Description      : This function will be called if we have a multiframe transmission. DoIP already stored the Header data
                    and expect payload only
***********************************************************************************************************************/
BufReq_ReturnType DoIP_DiagnosticMessage_ContinuePreviousMessage(DoIP_Type_TCPConnection *tcpCon_pst,
        const PduInfoType* pduInfo_pcst, PduLengthType* bufferSize_puo)
{
    BufReq_ReturnType pdurRet_en = BUFREQ_E_NOT_OK;
    PduIdType pdurPdu_uo;
    PduInfoType pdurInfo_st;
    uint8 NackCode_u8;

    // get the current payloadLength of this segment. This is the min between the SduLength and the Diff between the
    // total payload length and length of data, whch i already send (payloadLengthCounter)
    PduLengthType pdurSduLength_uo = DOIP_MIN(pduInfo_pcst->SduLength,
            (PduLengthType)(tcpCon_pst->RX_st.totalPayloadLength_u32 - tcpCon_pst->RX_st.payloadLengthCounter_u32));

    if (TRUE == DoIP_Channel_GetPduRRxPduId(tcpCon_pst->RX_st.channelIdx_u16, &pdurPdu_uo))
    {
        DoIP_DiagnosticMessage_UpdateAcknowledgeMessageData(pduInfo_pcst->SduDataPtr, pdurSduLength_uo, tcpCon_pst);

        /* We have to transmit from the beginning, no header to skip */
        pdurInfo_st.SduDataPtr = pduInfo_pcst->SduDataPtr;

        // update the payloadLengthCounter, ProcessedLength with the payloadLength of this segment.
        tcpCon_pst->RX_st.payloadLengthCounter_u32 += pdurSduLength_uo;
        tcpCon_pst->RX_st.ProcessedLength_u16 += pdurSduLength_uo;

        pdurRet_en = DoIP_DiagnosticMessage_CopyDataToPduR(tcpCon_pst, &pdurInfo_st, pdurPdu_uo, pdurSduLength_uo, bufferSize_puo);

        if( (BUFREQ_E_OVFL == pdurRet_en) || (BUFREQ_E_NOT_OK == pdurRet_en) )
        {
            NackCode_u8 = (BUFREQ_E_OVFL == pdurRet_en) ? DOIP_DM_NACKCODE_OUT_OF_MEM : DOIP_DM_NACKCODE_TP_ERROR;

            DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(tcpCon_pst,
                                                                 NackCode_u8,
                                                                 tcpCon_pst->RX_st.channelIdx_u16,
                                                                 DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8);
            /* [SWS_DoIP_00217]
             * If PduR_DoIPTpCopyRxData returns BUFREQ_E_NOT_OK, the DoIP module shall
             * react as described in SWS_DoIP_00174, discard all the TCP data until the next DoIP
             * message and call the PduR_DoIPTpRxIndication with the according PduId and the
             * result set to E_NOT_OK
             */
            //send PduR_DoIPRxIndication directly with the result E_NOT_OK
            PduR_DoIPRxIndication(pdurPdu_uo, E_NOT_OK);

            // complete message was received and forwarded to upper layer, go back to state DOIP_CHANNEL_NEW_MESSAGE_E to receive new messages
            tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
        }
    }

    return pdurRet_en;
}
/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_DiagnosticMessageHandler
 Description      : This function handles the diagnostic messages if the header is valid. In case of incomplete Sa or TA
                    the current message will be stored in into the diag msg fragment buffer and will be updated with the
                    next message. Depends on the SA/TA combination this function trigger the copying of the RX message
                    or send in the bad case a NACK.
***********************************************************************************************************************/
void DoIP_DiagnosticMessage_DiagnosticMessageHandler(DoIP_Type_TCPConnection* tcpCon_pst,
                                                     uint8* payload_pu8,
                                                     PduLengthType PayloadLength_u16,
                                                     PduLengthType* bufferSize_puo)
{
    BufReq_ReturnType pdurRet_en = BUFREQ_E_NOT_OK;
    PduIdType pdurPdu_uo = 0xFF;
    PduLengthType UpLayerBufferSize_uo = 0;
    boolean MinDiagPayloadRcvd_b = FALSE;
    uint8* DiagMsgMinPayloadBufStart_pu8 = NULL_PTR;
    uint8* DiagMsgMinPayloadBufEnd_pu8 = NULL_PTR;
    uint8 respCode_u8;
    PduInfoType pdurInfo_st;
    PduLengthType pdurSduLengthWoSaTa_tu16;
    boolean failed_startOfReception_b;
    uint16 ackLength_u16;

    /*
     * it may can happen, that we received only header but without a SA/TA or at least one payload byte
     * in that case we store the payload fragment and wait for the next CopyRxData call to fullfill the entry
     * condition of min DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN. Only in that case, we start to send the message to PduR.
     * This check should be made at the beginning of the diagnostic message handler
     */
    if(tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 > 0)
    {
       if((tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 + PayloadLength_u16) >= DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN)
       {
           DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragDiagMsg_au8[tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16],
                       payload_pu8,
                       (DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN - tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16));

           // update the sduPayloadLength, because we received not the complete SA/TA
           tcpCon_pst->RX_st.ProcessedLength_u16 +=  (DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN - tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16);
           PayloadLength_u16 -= (DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN - tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16);
           DiagMsgMinPayloadBufStart_pu8 = &tcpCon_pst->RxFragHdrBuffer_st.RxFragDiagMsg_au8[0];
           // end at payload position (4 - (already transmitted bytes of SA/TA))
           DiagMsgMinPayloadBufEnd_pu8   = &payload_pu8[DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN -
                                           tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16];
           MinDiagPayloadRcvd_b = TRUE;
           tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 = 0;
       }
       else
       {
           DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragDiagMsg_au8[tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16],
                       payload_pu8,
                       PayloadLength_u16);

           tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 += PayloadLength_u16;
           tcpCon_pst->RX_st.ProcessedLength_u16 += PayloadLength_u16;
           // another part needs to be transmitted
           tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_CONTINUE_FRAGMENT_E;

       }
    }
    // at least payload received?
    else if (PayloadLength_u16 >= DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN)
    {
        tcpCon_pst->RX_st.ProcessedLength_u16 += DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN;

        PayloadLength_u16 -= DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN;
        DiagMsgMinPayloadBufStart_pu8 = &payload_pu8[0];
        DiagMsgMinPayloadBufEnd_pu8   = &payload_pu8[DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN];
        MinDiagPayloadRcvd_b = TRUE;
    }
    else
    {
        /* not enough payload received to evaluate the SA/TA combination -> store into msg fragment buffer and wait for next message */
        DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragDiagMsg_au8[tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16],
                    payload_pu8,
                    PayloadLength_u16);

        tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 += PayloadLength_u16;
        tcpCon_pst->RX_st.ProcessedLength_u16 += PayloadLength_u16;
        tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_CONTINUE_FRAGMENT_E;
    }

    if(TRUE == MinDiagPayloadRcvd_b)
    {
        // it is possible, that an invalid tester without a RA will call DoIP. In that case, we dont have a valid tester_cfg structure
        // and we return 0 NACK bytes inside of the invalid SA NACK
        if(tcpCon_pst->ra_st.testerCfg_pcst == NULL_PTR)
        {
            ackLength_u16 = 0u;
        }
        else
        {
            // RA was done before and we have a valid tester_cfg structure
            ackLength_u16 = tcpCon_pst->ra_st.testerCfg_pcst->numByteDiagAckNack_u16;
        }

        pdurSduLengthWoSaTa_tu16 =  DOIP_MIN(PayloadLength_u16, (PduLengthType) tcpCon_pst->RX_st.totalPayloadLength_u32 - DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN); // payload to PduR without the SA/TA
        // set the channelIdx to invalid. only if the Sa/Ta is valid, the channelIdx will be updated
        tcpCon_pst->RX_st.channelIdx_u16 = DOIP_INVALID_PDUID;

        /* perform checks for SA/TA */
        respCode_u8 = DoIP_DiagnosticMessage_CheckHeader(DiagMsgMinPayloadBufStart_pu8, tcpCon_pst);

        tcpCon_pst->RX_st.payloadLengthCounter_u32 = DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN + pdurSduLengthWoSaTa_tu16;

        tcpCon_pst->RX_st.ProcessedLength_u16 += pdurSduLengthWoSaTa_tu16;

        if (DOIP_DM_ACKCODE_SUCCESS != respCode_u8)
        {
            /* In case of NACK, the channelIdx_u16 is invalid we cannot map this message to a channel buffer ->
             * use the NACK buffer in TCPConnection run time structure
             */
                DoIP_DiagnosticMessage_PrepareAcknowledgeMessage(DiagMsgMinPayloadBufStart_pu8,
                                                                 DiagMsgMinPayloadBufEnd_pu8,
                                                                 pdurSduLengthWoSaTa_tu16,
                                                                 ackLength_u16,
                                                                 tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8);

                DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(tcpCon_pst,
                                                                     respCode_u8,
                                                                     tcpCon_pst->RX_st.channelIdx_u16,
                                                                     tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8);

        }
        else
        {

            /* In case of ACK, we can map the message directly to  slot in the channel ack buffer  */
            DoIP_DiagnosticMessage_PrepareAcknowledgeMessage(DiagMsgMinPayloadBufStart_pu8,
                                                             DiagMsgMinPayloadBufEnd_pu8,
                                                             pdurSduLengthWoSaTa_tu16,
                                                             ackLength_u16,
                                                             DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8);

            if (TRUE == DoIP_Channel_GetPduRRxPduId(tcpCon_pst->RX_st.channelIdx_u16, &pdurPdu_uo))
            {
                /*
                 *  [SWS_DoIP_00216]
                 *  If PduR_DoIPTpStartOfReception returns BUFREQ_E_NOT_OK or
                 *  BUFREQ_E_OVFL, the DoIP module shall react as described in SWS_DoIP_00174
                 *  and discard all the TCP data until the next DoIP message.
                 *
                 *  [SWS_DoIP_00217]
                 *  If PduR_DoIPTpCopyRxData returns BUFREQ_E_NOT_OK, the DoIP module shall
                 *  react as described in SWS_DoIP_00174, discard all the TCP data until the next DoIP
                 *  message and call the PduR_DoIPTpRxIndication with the according PduId and the
                 *  result set to E_NOT_OK
                 */
                failed_startOfReception_b = FALSE;

                // SA & TA are not transmitted -> skip
                pdurInfo_st.SduDataPtr = DiagMsgMinPayloadBufEnd_pu8;
                pdurInfo_st.SduLength = pdurSduLengthWoSaTa_tu16;

                /*
                 * [SWS_DoIP_00212]
                 * If the DoIP module has received sufficient data to evaluate the DoIP header, the
                 * payload type is diagnostic message and the Routing was already activated for the
                 * SourceAddress/TargetAddress combination on this DoIPInterface, the DoIP module
                 * shall call the PduR_DoIPTpStartOfReception with the according id set to the
                 * DoIPPduRRxPduId matching the SourceAddress/TargetAddress combination of the
                 * diagnostic message on this DoIPInterface, set the info.SduLength to the already
                 * received diagnostic data, set the info->SduDataPtr to the buffer containing the
                 * received diagnostic data and set the TpSduLength to the total size of the diagnostic
                 * message extracted from DoIP Header.
                 */
                pdurRet_en = PduR_DoIPStartOfReception(pdurPdu_uo,
                                                    &pdurInfo_st,
                                                    (PduLengthType) (tcpCon_pst->RX_st.totalPayloadLength_u32 - DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN),
                                                    &UpLayerBufferSize_uo);

                if ( (pdurRet_en == BUFREQ_E_NOT_OK) || (pdurRet_en == BUFREQ_E_OVFL) )
                {
                    failed_startOfReception_b = TRUE;
                }
                else
                {
                    if(UpLayerBufferSize_uo != 0)
                    {
                        tcpCon_pst->RX_st.lastKnownBufferSize_uo = UpLayerBufferSize_uo;

                        pdurRet_en = DoIP_DiagnosticMessage_CopyDataToPduR(tcpCon_pst,
                                                                           &pdurInfo_st,
                                                                           pdurPdu_uo,
                                                                           pdurSduLengthWoSaTa_tu16,
                                                                           bufferSize_puo);
                    }
                }

                if (pdurRet_en == BUFREQ_E_OVFL)
                {
                   DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(tcpCon_pst,
                                                                        DOIP_DM_NACKCODE_OUT_OF_MEM,
                                                                        tcpCon_pst->RX_st.channelIdx_u16,
                                                                        DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8);


                   if(FALSE == failed_startOfReception_b)
                   {
                       PduR_DoIPRxIndication(pdurPdu_uo, E_NOT_OK);
                   }
                }
                else if (pdurRet_en == BUFREQ_E_NOT_OK)
                {
                    DoIP_DiagnosticMessage_TriggerSendAcknowledgeMessage(tcpCon_pst,
                                                                         DOIP_DM_NACKCODE_TP_ERROR,
                                                                         tcpCon_pst->RX_st.channelIdx_u16,
                                                                         DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8);
                    if(FALSE == failed_startOfReception_b)
                    {
                        PduR_DoIPRxIndication(pdurPdu_uo, E_NOT_OK);
                    }
                }
                else
                {
                    /* do nothing */
                }

            }
            else
            {
                DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_SHOULD_NOT_REACH);
            }
        }
    }
}

/***********************************************************************************************************************
 Function name    : DoIP_DiagnosticMessage_MessageHandler
 Description      : This function is the Message Handler according ISO 13400-2. If the Header is invalid, DoIP will trigger
                    a Generic Header NACK, if the Header is valid, DoIP start the separate paylaodtype specific message handler
***********************************************************************************************************************/
BufReq_ReturnType DoIP_DiagnosticMessage_MessageHandler( DoIP_Type_TCPConnection*   tcpCon_pst,
                                                         const uint8*               header_pu8,
                                                         uint8*                     payload_pu8,
                                                         PduLengthType              PayloadLength_uo,
                                                         PduLengthType*             bufferSize_puo)
{
    BufReq_ReturnType retVal_en;
    uint8 respCode_u8 = 0;
    boolean isNACKset_b = FALSE;

    /* copy header in internal header buffer */
    tcpCon_pst->RX_st.payloadType_u16          = DoIP_Header_GetPayloadType(header_pu8);
    tcpCon_pst->RX_st.totalPayloadLength_u32   = DoIP_Header_GetPayloadLength(header_pu8);

    /* generic header check, SA/TA will be checked later, because this request could be also a RA request or aliveCheck response */
    if (E_OK == DoIP_GenericAcknowledge_TCPHeaderHandler(tcpCon_pst ,header_pu8, &isNACKset_b, &respCode_u8))
    {
        switch (tcpCon_pst->RX_st.payloadType_u16)
        {
            case DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_REQ:
                retVal_en = DoIP_RoutingActivation_RoutingActivationHandler(tcpCon_pst, header_pu8, payload_pu8);
                break;
            case DOIP_PAYLOAD_TYPE_ALIVE_CHECK_RESP:
                retVal_en = DoIP_AliveCheck_ReceivedNewMessage(tcpCon_pst, header_pu8, payload_pu8);
                break;
            case DOIP_PAYLOAD_TYPE_DIAG_MSG:
                DoIP_DiagnosticMessage_DiagnosticMessageHandler(tcpCon_pst, payload_pu8, PayloadLength_uo, bufferSize_puo);
                /*
                 * If the reception on the channel side will be failed, this will be handled by NACKs.
                 * A different retVal_en to BUFREQ_OK will trigger a socket closure on SoAd side.
                 */
                retVal_en = BUFREQ_OK;
                break;
            default:
                //unknown TCP request -> discard
                retVal_en = BUFREQ_E_NOT_OK;
                break;
         }
    }
    else
    {   /*Check if is NACK set*/
        if(TRUE == isNACKset_b)
        {
            /* store the NACK into TX_st generic nack buffer directly */
            // the Rx.channelIdx_pu16 is DOIP_INVALID_PDUID, because TA is unknown
            DoIP_GenericAcknowledge_PrepareResponse(tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackBuffer_au8, respCode_u8);

            // reset the processedLength, because this is a invalid call and abort the loop

            tcpCon_pst->RX_st.ProcessedLength_u16 += (uint16)tcpCon_pst->RX_st.totalPayloadLength_u32;

            // RX_st.channel_pcstIndex is the same as the TX_st.channelIdx_u16, because this is Index of the channel container, which contains the provided RxPduId or TX_tstPduId of the same TA
            if (FALSE == DoIP_TCPConnection_SendMessage(tcpCon_pst, tcpCon_pst->RX_st.channelIdx_u16, tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackBuffer_au8, FALSE) )
            {
                tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b = TRUE;
            }
        }
        else
        {
            // reset the processedLength, because this is a invalid call and abort the loop

            tcpCon_pst->RX_st.ProcessedLength_u16 += (uint16)tcpCon_pst->RX_st.totalPayloadLength_u32;
        }

        retVal_en = BUFREQ_OK;
    }

    return retVal_en;
}
#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

