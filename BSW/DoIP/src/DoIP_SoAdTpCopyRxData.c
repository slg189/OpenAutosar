

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* HIS METRIC LEVEL VIOLATION IN DoIP_SoAdTpCopyRxData_ParameterCheck: Each check is precondition for later checks/better readability */
/***********************************************************************************************************************
 Function name    : DoIP_SoAdTpCopyRxData_ParameterCheck
 Description      : In this function the input parameter will be checked up
***********************************************************************************************************************/
static Std_ReturnType DoIP_SoAdTpCopyRxData_ParameterCheck(PduIdType RxPduId_uo, const PduInfoType* PduInfoPtr,
        const PduLengthType* bufferSize_pcuo)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_UNINIT);
    }
    else if (PduInfoPtr == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_PARAM_POINTER);
    }
    else if (bufferSize_pcuo == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_PARAM_POINTER);
    }
    else if (!DoIP_SoCon_IsTcpIndex(RxPduId_uo))
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_INVALID_PDU_SDU_ID);
    }
    else if (DoIP_ActivationLineStatus_u8 != DOIP_ACTIVATION_LINE_ACTIVE)
    {
       /*Do nothing As per SWS_DoIP_00202 just ignore the request and return negative value*/
    }
    else
    {
        retVal_u8 = E_OK;
    }

    return retVal_u8;
}
/***********************************************************************************************************************
 Function name    : DoIP_SoAdTpCopyRxData_ReceivedNewMessage
 Description      : In this function the received message will be further processed. If enough data is received to evaluate the
                    header, DoIP will enter the next deeper Level, otherwise DoIP will store the received bytes into the
                    fragmented header buffer and will try to complete this in the next CopyRxData call.
***********************************************************************************************************************/
static BufReq_ReturnType DoIP_SoAdTpCopyRxData_ReceivedNewMessage(DoIP_Type_TCPConnection* tcpCon_pst,
                                                                  const PduInfoType* PduInfoPtr,
                                                                  PduLengthType* bufferSize_puo,
                                                                  const uint16 *SduDataCurrPos_p16)
{
    BufReq_ReturnType retVal_en = BUFREQ_E_NOT_OK;
    /*
     * Check the current fragment header size. If something was received in the last call, this call should update this fragment.
     * If the Header size is received, we can proceed as new Message. Otherwise, the Header Fragment will be updated.
     */
    if(tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 > 0)
    {
        if((tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 + PduInfoPtr->SduLength) >= DOIP_HEADER_SIZE )
        {
            /* copy the missing bytes of header into the fragment buffer */
            DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragHdrBuffer_au8[tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16],
                        PduInfoPtr->SduDataPtr,
                        (DOIP_HEADER_SIZE - tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16));


            tcpCon_pst->RX_st.ProcessedLength_u16 += (DOIP_HEADER_SIZE - tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16);

            retVal_en = DoIP_DiagnosticMessage_MessageHandler
                     (tcpCon_pst,
                      &tcpCon_pst->RxFragHdrBuffer_st.RxFragHdrBuffer_au8[0],
                      &PduInfoPtr->SduDataPtr[( DOIP_HEADER_SIZE - tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 )],
                      (PduInfoPtr->SduLength - ( DOIP_HEADER_SIZE - tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 )),
                      bufferSize_puo);

            /* Reset the HeaderFragCurrentSize_u16 once complete DoIP header is received */
            tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 = 0;
        }
        else
        {
            /* doesnt receive enough to evaluate the header -> copy into header fragment buffer and wait for the next message */
            DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragHdrBuffer_au8[tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16],
                        PduInfoPtr->SduDataPtr,
                        PduInfoPtr->SduLength );

            tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 += PduInfoPtr->SduLength;
            tcpCon_pst->RX_st.ProcessedLength_u16 += PduInfoPtr->SduLength;

            retVal_en = BUFREQ_OK;
        }
    }
    else if ((PduInfoPtr->SduLength - (*SduDataCurrPos_p16)) >= (uint16)DOIP_HEADER_SIZE)
    {
        tcpCon_pst->RX_st.ProcessedLength_u16 += DOIP_HEADER_SIZE;

        retVal_en = DoIP_DiagnosticMessage_MessageHandler(tcpCon_pst,
                                                        &PduInfoPtr->SduDataPtr[(*SduDataCurrPos_p16)],
                                                        &PduInfoPtr->SduDataPtr[(*SduDataCurrPos_p16) + DOIP_HEADER_SIZE],
                                                        (PduInfoPtr->SduLength - ( (*SduDataCurrPos_p16) + DOIP_HEADER_SIZE ) ),
                                                        bufferSize_puo);
    }
    else
    {
        /* doesnt receive enough to evaluate the header -> copy into header fragment buffer and wait for the next message */
        DOIP_MEMCPY(&tcpCon_pst->RxFragHdrBuffer_st.RxFragHdrBuffer_au8[tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16],
                           &(PduInfoPtr->SduDataPtr[*SduDataCurrPos_p16]),
                           ( PduInfoPtr->SduLength - *SduDataCurrPos_p16));

        tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 += ( PduInfoPtr->SduLength - *SduDataCurrPos_p16);
        tcpCon_pst->RX_st.ProcessedLength_u16 += tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16;

        retVal_en = BUFREQ_OK;
    }

    return retVal_en;
}
/***********************************************************************************************************************
 Function name    : DoIP_SoAdTpCopyRxData
 Description      : This is an API function to start a reception of diagnotic message or get the available buffer size of DoIP
                    This function is non reentrant for the same RxPduId, but reentrant for different RxPduIds
***********************************************************************************************************************/
BufReq_ReturnType DoIP_SoAdTpCopyRxData(PduIdType RxPduId, const PduInfoType* PduInfoPtr, PduLengthType* bufferSizePtr)
{
    BufReq_ReturnType retVal_en = BUFREQ_E_NOT_OK;
    DoIP_Type_TCPConnection *tcpCon_pst;
    uint16 SduDataCurrPos_u16 = 0;     /* Variable to hold the current position of SduData pointer */

    if (E_OK == DoIP_SoAdTpCopyRxData_ParameterCheck(RxPduId, PduInfoPtr, bufferSizePtr))
    {
        // get runtime TCPConnection Structure
        tcpCon_pst = DoIP_SoCon_GetTCPCon(DoIP_SoCon_GetSocketConnection(RxPduId));

        /* DoIP does not have a own buffer for a whole message, but only some bytes to store header information.
         * In this call, we dont have any header information, for which target this request is addressed.
         * Because of that we return always the max buffer size.
         */
		*bufferSizePtr = DOIP_CFG_MAX_REQUEST_BYTES + DOIP_HEADER_SIZE;

		/* Initialize the ProcessedLength to 0 if new packet is received */
        tcpCon_pst->RX_st.ProcessedLength_u16 = 0;

        /* Available buffersize requested */
        // PduInfoPtr->SduDataPtr == NULL_PTR was observed during debug sessions, but is not specified in SoAd spec 4.5
        if ((PduInfoPtr->SduLength == 0) || (PduInfoPtr->SduDataPtr == NULL_PTR))
        {
            retVal_en = BUFREQ_OK;
        }
        else
        {
            // loop over all possible messages inside of one frame
            while(tcpCon_pst->RX_st.ProcessedLength_u16 < PduInfoPtr->SduLength)
            {
                SduDataCurrPos_u16 = tcpCon_pst->RX_st.ProcessedLength_u16;

                switch (tcpCon_pst->RX_st.channelState_en)
                {
                    case DOIP_CHANNEL_NEW_MESSAGE_E:
                    {
                        retVal_en = DoIP_SoAdTpCopyRxData_ReceivedNewMessage(tcpCon_pst,
                                                                           PduInfoPtr,
                                                                           bufferSizePtr,
                                                                           &SduDataCurrPos_u16);
                        break;
                    }

                    case DOIP_CHANNEL_MULTIFRAME_MESSAGE_E:
                    {
                        retVal_en = DoIP_DiagnosticMessage_ContinuePreviousMessage(tcpCon_pst, PduInfoPtr, bufferSizePtr);

                        break;
                    }
                    case DOIP_CHANNEL_CONTINUE_FRAGMENT_E:
                    {
                        DoIP_DiagnosticMessage_DiagnosticMessageHandler(tcpCon_pst,
                                                                        PduInfoPtr->SduDataPtr,
                                                                        PduInfoPtr->SduLength,
                                                                        bufferSizePtr);
                        retVal_en = BUFREQ_OK;
                        break;
                    }

                    case DOIP_CHANNEL_DISCARD_MESSAGE_E:
                    {
                        tcpCon_pst->RX_st.payloadLengthCounter_u32 += PduInfoPtr->SduLength;

                        // if PduR returns not BUFREQ_OK for a reception we shall discard all tcp segments until the next diag message
                        // so stay in discard state until the beginning next diagnostic message
                        if (tcpCon_pst->RX_st.totalPayloadLength_u32 <= tcpCon_pst->RX_st.payloadLengthCounter_u32)
                        {
                            if(tcpCon_pst->RX_st.channelIdx_u16 < DOIP_CFG_CHANNEL_ARRAY_SIZE)
                            {
                                DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].channelACKState_en =
                                        (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst,
                                                                           tcpCon_pst->RX_st.channelIdx_u16,
                                                                           DoIP_Channel_ast[tcpCon_pst->RX_st.channelIdx_u16].ackBuffer_au8,
                                                                           FALSE)) ?
                                   DOIP_CHANNEL_TX_SENDING_E : DOIP_CHANNEL_TX_READY2SEND_E;
                            }
                            else
                            {
                                tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en =
                                        (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst,
                                                                          tcpCon_pst->RX_st.channelIdx_u16,
                                                                          tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8,
                                                                          FALSE))?
                                  DOIP_TCP_TX_SENDING_E : DOIP_TCP_TX_BLOCKED_E;
                            }

                            // abort the while loop
                            // the correct value is not important for abort the loop
                            tcpCon_pst->RX_st.ProcessedLength_u16 = PduInfoPtr->SduLength -
                                    (uint16)(tcpCon_pst->RX_st.payloadLengthCounter_u32 - tcpCon_pst->RX_st.totalPayloadLength_u32);

                            // reset the channel for new requests
                            tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
                        }
                        else
                        {
                            // update the NACK Msg
                            DoIP_DiagnosticMessage_UpdateAcknowledgeMessageData(PduInfoPtr->SduDataPtr,PduInfoPtr->SduLength,tcpCon_pst);
                            // abort the while loop
                            tcpCon_pst->RX_st.ProcessedLength_u16 = PduInfoPtr->SduLength;
                        }

                        retVal_en = BUFREQ_OK;
                        break;
                    }

                    default:
                    {
                        // in this case SoAd calls DoIP_SoAdTpCopyRxData() twice, without a return of the first call. This is
                        // for tcp connections forbidden and violate the tcp data flow. The TCPConnection will be closed.
                        DOIP_DET(DOIP_DET_APIID_TP_CopyRxData, DOIP_E_SHOULD_NOT_REACH);

                        break;
                    }
                }

                if(BUFREQ_OK != retVal_en)
                {
                    tcpCon_pst->RX_st.ProcessedLength_u16 = PduInfoPtr->SduLength;
                    tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
                }
            }
        }
    }
    return retVal_en;
}
#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

