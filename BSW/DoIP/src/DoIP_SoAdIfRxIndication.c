

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* HIS METRIC LEVEL VIOLATION IN DoIP_SoAdIfRxIndication_ParameterCheck: Each check is precondition for later checks/better readability */
static boolean DoIP_SoAdIfRxIndication_ParameterCheck(PduIdType RxPduId_uo, const PduInfoType* pduInfo_pcst)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET( DOIP_DET_APIID_IF_RxIndication, DOIP_E_UNINIT );
    }
    else if (!DoIP_SoCon_IsUdpIndex(RxPduId_uo))
    {
        DOIP_DET( DOIP_DET_APIID_IF_RxIndication, DOIP_E_INVALID_PDU_SDU_ID );
    }
    else if ((pduInfo_pcst == NULL_PTR) || (pduInfo_pcst->SduDataPtr == NULL_PTR))
    {
        DOIP_DET( DOIP_DET_APIID_IF_RxIndication, DOIP_E_PARAM_POINTER );
    }
    else if (DoIP_ActivationLineStatus_u8 != DOIP_ACTIVATION_LINE_ACTIVE)
    {
        /*Do nothing As per SWS_DoIP_00202 just ignore the request and return negative value*/
    }
    else if (!(DoIP_SoCon_IsActive(DoIP_UDPConnection_Get(RxPduId_uo)->soCon_pcst)) || !(DoIP_IsIPAddressAssigned_b))
    {
        /* retVal_b is already set to FALSE */
    }
    else
    {
        retVal_b = TRUE;
    }
    return retVal_b;
}

void DoIP_SoAdIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    boolean isInvalidRequest_b = FALSE;
    uint8 respCode_u8 = 0;
    uint8 queueIdx_u8 = 1; //start with 1, becuse the idx 0 is reserved for NACK msg
    uint16 payloadType_u16;
    uint32 payLoadLength_32;
    uint32 totCopiedLength_32 = 0;
    DoIP_Type_UDPConnection_tst* udpCon_pst = NULL_PTR;

    /* Perform paramter check */
    if (DoIP_SoAdIfRxIndication_ParameterCheck(RxPduId, PduInfoPtr))
    {
        /* Initialise the udpCon_pst to that UDP connection on which this request has been received */
        udpCon_pst = DoIP_UDPConnection_Get(RxPduId);

        /* Try to process all messages from buffer */
        while ((totCopiedLength_32 < PduInfoPtr->SduLength) && /* not all messages processed */
        (isInvalidRequest_b == FALSE) && /* valid data in message buffer */
        (queueIdx_u8 < DOIP_CFG_MaxUDPQueueSize) /* queue not full */)
        {
            if (udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 == DOIP_UDP_DATA_EMPTY)
            {
                /* Parse the received UDP request here and also trigger the transmission of response from here */
                if (PduInfoPtr->SduLength >= DOIP_HEADER_SIZE)
                {
                    DOIP_MEMCPY(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8, &PduInfoPtr->SduDataPtr[totCopiedLength_32],
                            DOIP_HEADER_SIZE);
                    if (E_NOT_OK == DoIP_GenericAcknowledge_UDPHeaderHandler(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8, &respCode_u8))
                    {
                        DoIP_GenericAcknowledge_PrepareResponse(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8, respCode_u8);
                        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
                        isInvalidRequest_b = TRUE;
                    }
                    else
                    {
                        payloadType_u16 = DoIP_Header_GetPayloadType(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);
                        payLoadLength_32 = DoIP_Header_GetPayloadLength(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);

                        switch (payloadType_u16)
                        {
                            case DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ:
                            case DOIP_PAYLOAD_TYPE_ENTITY_STATUS_REQ:
                            case DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_REQ:
                                udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_RECEIVED;
                                /* Payload length is 0*/
                                totCopiedLength_32 += DOIP_HEADER_SIZE;
                                break;

                            case DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ:
                            case DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ:
                                udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_RECEIVED;
                                DOIP_MEMCPY(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8 + DOIP_HEADER_SIZE,
                                        PduInfoPtr->SduDataPtr + (totCopiedLength_32 + DOIP_HEADER_SIZE),
                                        payLoadLength_32);
                                totCopiedLength_32 += (payLoadLength_32 + DOIP_HEADER_SIZE);
                                break;
                            // The following cases are only reachable if DOIP_CFG_GENERIC_HEADER_IGNORE_INVALID_UDP is set to TRUE
                            case DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK:
                            case DOIP_PAYLOAD_TYPE_VEHICLE_ID_RESP:
                            case DOIP_PAYLOAD_TYPE_ENTITY_STATUS_RESP:
                            case DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_RESP:
                                /* Silently ignore if requests other than UDP are received.
                                 * Just consume the message bytes in the buffer. */
                                totCopiedLength_32 += (payLoadLength_32 + DOIP_HEADER_SIZE);

                                /*if the reset of the remote adress fails in the context of IfRxIndication,
                                 * the reset will be retried in the next mainfunction*/
						        DoIP_SetRemoteAddressToWildCard(udpCon_pst);
                                break;

                            default:
                                /* invalid request */
                                isInvalidRequest_b = TRUE;

                                /*if the reset of the remote adress fails in the context of IfRxIndication,
                                 * the reset will be retried in the next mainfunction*/
                                DoIP_SetRemoteAddressToWildCard(udpCon_pst);
                                break;
                        }
                    }
                }
                else
                {
                    /* Any UDP frame is expected to be at least 8 bytes
                     * - Protocol Version (1 byte),
                     * - Inverse Protocol Version (1 byte),
                     * - Payload Type (2 bytes) and
                     * - Payload Length (4 bytes) */
                    DoIP_GenericAcknowledge_PrepareResponse(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8,
                    DOIP_NACK_PROTOCOL_INFO_INCORRECT);
                    udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
                    isInvalidRequest_b = TRUE;
                }
            }
            else
            {
                queueIdx_u8++;
            }
        }
        /* If queue is full and not all message are consumed from the buffer: */
        if ((totCopiedLength_32 < PduInfoPtr->SduLength) && (queueIdx_u8 == DOIP_CFG_MaxUDPQueueSize)
                && (isInvalidRequest_b == FALSE))
        {
            /* 0th position of the queue is used for storing the NACK buffer overflow*/
            DoIP_GenericAcknowledge_PrepareResponse(udpCon_pst->DoIP_UDPQueue_au8[0].buffer_au8, DOIP_NACK_OUT_OF_MEMORY);
            udpCon_pst->DoIP_UDPQueue_au8[0].udpQueueState_u8 = DOIP_UDP_DATA_READY;
        }
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

