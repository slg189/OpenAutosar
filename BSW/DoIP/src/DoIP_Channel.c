

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

DoIP_Type_ChannelTransmit_tst DoIP_Channel_ast[DOIP_CFG_CHANNEL_ARRAY_SIZE];

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_Channel_PrepareDiagnosticMessageHeader(uint16 channelIdx_u16, const PduInfoType* pduInfoType_pcst)
{
    uint16 address_u16;
    uint8* msg_pu8 = DoIP_Channel_ast[channelIdx_u16].txBuffer_au8;

    /* Header */
    // todo: this part is copying two times-> refractor it should be only copied one time inside of CopyTxData
    /* PayloadLength: SudLength + SA + TA */
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_DIAG_MSG, pduInfoType_pcst->SduLength + 2u + 2u, msg_pu8);
    msg_pu8 += DOIP_HEADER_SIZE;

    /* Payload */
    /* LogicalAddressDoIP */
    address_u16 = rba_DiagLib_ByteOrderUtils_Htons(DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].targetAddr_u16);
    DOIP_MEMCPY(msg_pu8, &address_u16, 2u); //SA: DoIP node address
    msg_pu8 += 2;

    /* LogicalAddressTester */
    address_u16 = rba_DiagLib_ByteOrderUtils_Htons(
            DoIP_activeConfig_pcst->testerCfg_pcst[DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].testerIdx_u8].testerSA_u16);
    DOIP_MEMCPY(msg_pu8, &address_u16, 2u); //TA: tester address is TA for sending

    DoIP_Channel_ast[channelIdx_u16].channelState_en = DOIP_CHANNEL_TX_READY2SEND_E;
}

void DoIP_Channel_Release(uint16 channelIdx_u16, Std_ReturnType result_u8)
{
    PduIdType pdurPdu_uo = 0;

    if (channelIdx_u16 < DoIP_activeConfig_pcst->channelSize_u16)
    {
        if (DoIP_Channel_ast[channelIdx_u16].channelState_en != DOIP_CHANNEL_TX_IDLE_E)
        {
            if (DoIP_Channel_GetPduRTxPduId(channelIdx_u16, &pdurPdu_uo))
            {
                PduR_DoIPTxConfirmation(pdurPdu_uo, result_u8);
            }
        }
        DoIP_Channel_ast[channelIdx_u16].channelState_en = DOIP_CHANNEL_TX_IDLE_E;
    }
}

void DoIP_Channel_ReleaseAllChannelsForTester(const DoIP_Cfg_TesterType_tst* tester_pcst, Std_ReturnType result_u8)
{
    uint8 idx_u8;

    if(tester_pcst != NULL_PTR)
    {
        /* Check Queue*/
        for (idx_u8 = 0; idx_u8 < tester_pcst->numOfchannel_u16; idx_u8++)
        {
            DoIP_Channel_Release((tester_pcst->ChannelJumpTableIdx_u16 + idx_u8), result_u8);

        }
    }
}

boolean DoIP_Channel_NewMessage(uint16 channelIdx_u16, const PduInfoType* pduInfoType_pcst)
{
    boolean isExecuted_b = FALSE;

    /* New message from upper layer (state is idle, infoType is valid */
    if (DoIP_Channel_ast[channelIdx_u16].channelState_en == DOIP_CHANNEL_TX_IDLE_E)
    {
        /* TODO lock needed ? or only called synchron / once per PDU from PduR */
        SchM_Enter_DoIP();
        if (DoIP_Channel_ast[channelIdx_u16].channelState_en == DOIP_CHANNEL_TX_IDLE_E)
        {
            DoIP_Channel_ast[channelIdx_u16].channelState_en = DOIP_CHANNEL_TX_LOCK_E;
            isExecuted_b = TRUE;
        }
        SchM_Exit_DoIP();

        /* Prepare header and store request (READY2SEND) */
        if (isExecuted_b)
        {
            DoIP_Channel_PrepareDiagnosticMessageHeader(channelIdx_u16, pduInfoType_pcst);
        }
    }

    return isExecuted_b;
}

void DoIP_Channel_SendMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint16 channelIdx_u16)
{
    // try to send a generic header NACK message (Prio 1)
    if (TRUE == tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b)
    {
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, tcpCon_pst->RX_st.channelIdx_u16, tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackBuffer_au8, FALSE))
        {
            tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b = FALSE;
        }
    }
    // try to send a NACK message, which was stored in tcp NACK buffer, because the channelidx is invalid (Prio 2)
    else if (DOIP_TCP_TX_BLOCKED_E == tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en)
    {
        // when the the Connection has several channels and all channels have a NACK/ACK message in the buffer
        // the SoAd doesnt know, which message should be copied at first.
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, tcpCon_pst->RX_st.channelIdx_u16, tcpCon_pst->TX_st.txDiagACKBuffer_st.txDiagNACKBuffer_au8, FALSE))
        {
            tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_SENDING_E;
        }
    }
    // try to send a ACK/NACK message, which was stored in channel buffer, because channelidx is valid (Prio 3)
    else if (DOIP_CHANNEL_TX_READY2SEND_E == DoIP_Channel_ast[channelIdx_u16].channelACKState_en)
    {
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, channelIdx_u16, DoIP_Channel_ast[channelIdx_u16].ackBuffer_au8, TRUE))
        {
            DoIP_Channel_ast[channelIdx_u16].channelACKState_en = DOIP_CHANNEL_TX_SENDING_E;
        }
    }
    // try to send a target tx message (Prio 4)
    else if (DOIP_CHANNEL_TX_READY2SEND_E == DoIP_Channel_ast[channelIdx_u16].channelState_en)
    {
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, channelIdx_u16, DoIP_Channel_ast[channelIdx_u16].txBuffer_au8, TRUE))
        {
            DoIP_Channel_ast[channelIdx_u16].channelState_en = DOIP_CHANNEL_TX_SENDING_E;
        }
    }
    else
    {
        // do nothing
    }


}

boolean DoIP_Channel_GetChannelIdxForRxPduId(PduIdType pduId_uo, uint16* channelIdx_pu16)
{
    boolean pduIdValid_b = FALSE;

    /* Check if pdu is in range of channel array */
    if (pduId_uo < DoIP_activeConfig_pcst->channelSize_u16)
    {
        /* Check if that channel has a valid PduId */
        if (DoIP_activeConfig_pcst->channel_pcst[pduId_uo].doIPPduRRxPduId_uo == pduId_uo)
        {
            pduIdValid_b = TRUE;
            *channelIdx_pu16 = pduId_uo;
        }
    }

    return pduIdValid_b;
}

boolean DoIP_Channel_GetChannelIdxForTxPduId(PduIdType pduId_uo, uint16* channelIdx_pu16)
{
    boolean pduIdValid_b = FALSE;

    /* Check if pdu is in range of channel array */
    if (pduId_uo < DoIP_activeConfig_pcst->channelSize_u16)
    {
        /* Check if that channel has a valid PduId */
        if (DoIP_activeConfig_pcst->channel_pcst[pduId_uo].doIPPduRTxPduId_uo == pduId_uo)
        {
            pduIdValid_b = TRUE;
            *channelIdx_pu16 = pduId_uo;
        }
    }

    return pduIdValid_b;
}

boolean DoIP_Channel_GetPduRTxPduId(uint16 channelIdx_u16, PduIdType* pduId_puo)
{
    boolean pduIdValid_b = FALSE;

    if (channelIdx_u16 < DoIP_activeConfig_pcst->channelSize_u16)
    {
        if (DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].pduRTxPdu_uo != DOIP_INVALID_PDUID)
        {
            *pduId_puo = DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].pduRTxPdu_uo;
            pduIdValid_b = TRUE;
        }
    }
    return pduIdValid_b;
}

boolean DoIP_Channel_GetPduRRxPduId(uint16 channelIdx_u16, PduIdType* pduId_puo)
{
    boolean pduIdValid_b = FALSE;

    if (channelIdx_u16 < DoIP_activeConfig_pcst->channelSize_u16)
    {
        if (DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].pduRRxPdu_uo != DOIP_INVALID_PDUID)
        {
            *pduId_puo = DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].pduRRxPdu_uo;
            pduIdValid_b = TRUE;
        }
    }
    return pduIdValid_b;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

