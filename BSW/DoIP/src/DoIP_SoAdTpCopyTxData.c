

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* HIS METRIC LEVEL VIOLATION IN DoIP_SoAdTpCopyTxData_ParameterCheck: Each check is precondition for later checks/better readability */
static boolean DoIP_SoAdTpCopyTxData_ParameterCheck(PduIdType TxPduId, const PduInfoType* PduInfoPtr,
        const RetryInfoType* retry, const PduLengthType* availableDataPtr)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_UNINIT);
    }
    else if (PduInfoPtr == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_PARAM_POINTER);
    }
    else if (availableDataPtr == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_PARAM_POINTER);
    }
    else if (retry != NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_INVALID_PARAMETER);
    }
    else if (!DoIP_SoCon_IsTcpIndex(TxPduId))
    {
        DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_INVALID_PDU_SDU_ID);
    }
    else if (DoIP_ActivationLineStatus_u8 != DOIP_ACTIVATION_LINE_ACTIVE)
    {
        /*Do nothing As per SWS_DoIP_00202 just ignore the request and return negative value*/
    }
    else
    {
        retVal_b = TRUE;
    }

    return retVal_b;
}

BufReq_ReturnType DoIP_SoAdTpCopyTxData(PduIdType TxPduId, const PduInfoType* PduInfoPtr, RetryInfoType* retry,
        PduLengthType* availableDataPtr)
{
    BufReq_ReturnType retVal_en = BUFREQ_E_NOT_OK;
    DoIP_Type_TCPConnection *tcpCon_pst;
    uint16 channelIdx_u16;
    /* Perform paramter check */
    if (DoIP_SoAdTpCopyTxData_ParameterCheck(TxPduId, PduInfoPtr, retry, availableDataPtr))
    {
        tcpCon_pst = DoIP_SoCon_GetTCPCon(DoIP_SoCon_GetSocketConnection(TxPduId));
        /* Available buffersize requested */
        if((PduInfoPtr->SduLength == 0)||(PduInfoPtr->SduDataPtr == NULL_PTR))
        {
            *availableDataPtr = (PduLengthType) (((tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE)
                    - tcpCon_pst->TX_st.lastKnownBufferSize_uo));
            retVal_en = BUFREQ_OK;
        }
        else
        {
            if (tcpCon_pst->TX_st.tcpState_en == DOIP_TCP_TX_SENDING_E)
            {
                if (tcpCon_pst->TX_st.payloadType_u16 == DOIP_PAYLOAD_TYPE_DIAG_MSG)
                {
                    PduIdType pdurPdu_puo = 0;

                    channelIdx_u16 = tcpCon_pst->TX_st.channelIdx_u16;

                    //TxPduId is given by caller (SoAd) so must be the DoIP id
                    //we now have to get the pdu id of the module that is getting called (PduR)
                    if (DoIP_Channel_GetPduRTxPduId(channelIdx_u16, &pdurPdu_puo))
                    {
                        PduInfoType pdurInfo;
                        uint16 headerTxLength = 0;
                        uint16 headerRemaining = 0;

                        if (tcpCon_pst->TX_st.lastKnownBufferSize_uo < (DOIP_HEADER_SIZE + 4u))
                        {
                            // The header transfer is not yet done.
                            // The header gets filled from DoIP layer through memcopy.
                            headerRemaining = (DOIP_HEADER_SIZE + 4u) - tcpCon_pst->TX_st.lastKnownBufferSize_uo;

                            // Transfer as much as possible of the header
                            headerTxLength = DOIP_MIN(PduInfoPtr->SduLength, headerRemaining);
                            DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                    &(DoIP_Channel_ast[channelIdx_u16].txBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]), headerTxLength);
//                            headerRemaining -= headerTxLength;
                            tcpCon_pst->TX_st.lastKnownBufferSize_uo += headerTxLength;
                            retVal_en = BUFREQ_OK;
                        }

                        if((PduInfoPtr->SduLength - headerTxLength) > (uint16)0 )
                        {
                            pdurInfo.SduDataPtr = &PduInfoPtr->SduDataPtr[headerTxLength];

                            // In case there is no room for payload in this transfer, SduLength will be set to 0 here.
                            // This is required to query the PduR for the amount of available data.
                            pdurInfo.SduLength = PduInfoPtr->SduLength - headerTxLength;

                            // Copy data from upper layer. In case SduLength is 0, only the available data gets queried from PduR
                            // The available data ptr is a flow control mechanism.
                            // The PduR provides the number of available data bytes for the next copy request.
                            retVal_en = PduR_DoIPCopyTxData(pdurPdu_puo, &pdurInfo, retry, availableDataPtr);

                            if (retVal_en == BUFREQ_OK)
                            {
                                tcpCon_pst->TX_st.lastKnownBufferSize_uo += pdurInfo.SduLength;

                                // Overall available data is what the PduR reported plus remaining header
//                                *availableDataPtr = *availableDataPtr + headerRemaining;

                                // The tcpCon_pst->TX_st.payloadLength_u32 already contains the extra 4 bytes SA and TA
                                // Therefore only add DOIP_HEADER_SIZE to get the full transfer length
                                if (tcpCon_pst->TX_st.lastKnownBufferSize_uo == (tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE))
                                {
                                    // All of the originally requested data has been transferred
                                    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_WAIT4CONFIRMATION_E;
                                }
                                else if(tcpCon_pst->TX_st.lastKnownBufferSize_uo > (tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE))
                                {
                                    // More data copied by SoAd than originally transferred by PduR!
                                    DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_SHOULD_NOT_REACH);
                                }
                                else
                                {
                                    // do nothing
                                }
                            }
                            else //busy
                            {
                                /* tcpCon_pst->lastKnownBufferSize_uo is not updated in this context, as the return val
                                 * is set to BUSY from the application which is passed to SoAd, so in the next
                                 * call/retry from SoAd, it will try to recheck with the Application where the
                                 * header will also be re-written onto the buffer provided by SoAd */
                            }
                        }
                    }
                }
                else if ((PduInfoPtr->SduLength + tcpCon_pst->TX_st.lastKnownBufferSize_uo)
                        <= (tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE))
                {
                    /* Select correct buffer depending on payload */
                    switch(tcpCon_pst->TX_st.payloadType_u16)
                    {
                        case DOIP_PAYLOAD_TYPE_ALIVE_CHECK_REQ:
                            DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                    &(tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                    PduInfoPtr->SduLength);
                            break;
                        case DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_RESP:
                            DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                    &(tcpCon_pst->ra_st.buffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                    PduInfoPtr->SduLength);
                            break;
                        case DOIP_PAYLOAD_TYPE_DIAG_MSG_ACK:
                            DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                   &(DoIP_Channel_ast[tcpCon_pst->TX_st.channelIdx_u16].ackBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                   PduInfoPtr->SduLength);
                            break;
                        case DOIP_PAYLOAD_TYPE_DIAG_MSG_NACK:
                            if (tcpCon_pst->TX_st.channelIdx_u16 < DOIP_CFG_CHANNEL_ARRAY_SIZE)
                            {
                                DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                       &(DoIP_Channel_ast[tcpCon_pst->TX_st.channelIdx_u16].ackBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                       PduInfoPtr->SduLength);
                            }
                            else
                            {
                                DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                       &(tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                       PduInfoPtr->SduLength);

                                tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_WAIT4CONFIRMATION_E;
                            }
                            break;
                        case DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK:
                            DOIP_MEMCPY(PduInfoPtr->SduDataPtr,
                                    &(tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackBuffer_au8[tcpCon_pst->TX_st.lastKnownBufferSize_uo]),
                                    PduInfoPtr->SduLength);
                            break;
                        default:
                            DOIP_DET(DOIP_DET_APIID_TP_CopyTxData, DOIP_E_SHOULD_NOT_REACH);
                            break;
                    }

                    tcpCon_pst->TX_st.lastKnownBufferSize_uo += PduInfoPtr->SduLength;

                    if (tcpCon_pst->TX_st.lastKnownBufferSize_uo == (tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE))
                    {
                        tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_WAIT4CONFIRMATION_E;
                        *availableDataPtr = 0;
                    }
                    else
                    {
                        *availableDataPtr = (PduLengthType) (((tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE)
                                - tcpCon_pst->TX_st.lastKnownBufferSize_uo));
                    }

                    retVal_en = BUFREQ_OK;
                }
                else
                {
                    // do nothing
                }
            }
        }

    }

    return retVal_en;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

