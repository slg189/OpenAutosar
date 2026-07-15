

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_AliveCheck_MainFunction(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Sending of alive check was requested by a routingactivation of another TCP-Connection*/
    if (tcpCon_pst->aliveCheck_st.aliveCheckState_u8 == DOIP_ALIVECHECK_REQUESTED)
    {
        DoIP_Header_Write(DOIP_PAYLOAD_TYPE_ALIVE_CHECK_REQ, DOIP_PAYLOAD_LENGTH_ALIVE_CHECK_REQ,
                tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8);
        tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_READY2SEND;
    }

    /* AliveBuffer is prepared, try to send message */
    if (tcpCon_pst->aliveCheck_st.aliveCheckState_u8 == DOIP_ALIVECHECK_READY2SEND)
    {
        if (TRUE == DoIP_TCPConnection_SendMessage(tcpCon_pst, DOIP_INVALID_PDUID, tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8,
                FALSE))
        {
            tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_WAIT4CONFIRMATION;
        }
    }

    /* Tester-Response was received, check values in AliveBuffer */
    if (tcpCon_pst->aliveCheck_st.aliveCheckState_u8 == DOIP_ALIVECHECK_RECEIVED)
    {
        DoIP_AliveCheck_RxHandle(tcpCon_pst);
    }
}

void DoIP_AliveCheck_TxConfirmation(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Set the timer to the configured number of cylces + 1 */
    tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 = DOIP_CFG_AliveCheckResponseTimeout_Cycles;
    /* Set state to waiting for tester response */
    tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_WAIT4RESPONSE;
    /* Message was send -> back to idle */
    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
}

void DoIP_AliveCheck_RxHandle(DoIP_Type_TCPConnection* tcpCon_pst)
{
    uint16 sourceAddress_u16;
    DOIP_MEMCPY(&sourceAddress_u16, tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8 + DOIP_MSG_POS_CONTENT, 2u);
    sourceAddress_u16 = rba_DiagLib_ByteOrderUtils_Ntohs(sourceAddress_u16);

    if (DoIP_RoutingActivation_IsSourceAddressRegistered(&tcpCon_pst->ra_st, sourceAddress_u16))
    {
        /* Set inactivity timer once alive check response is received */
        tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 = 0;
        DoIP_TCPConnection_SetGeneralInactivity(tcpCon_pst);
        tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_IDLE;
    }
    else
    {
        /* Set flag to close connection */
        DoIP_TCPConnection_SetCloseNeeded(tcpCon_pst);
    }
}

void DoIP_AliveCheck_Request(DoIP_Type_TCPConnection* tcpCon_pst)
{
    if (DoIP_AliveCheck_IsIdle(tcpCon_pst))
    {
        tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_REQUESTED;
    }
}

boolean DoIP_AliveCheck_IsIdle(const DoIP_Type_TCPConnection* tcpCon_pcst)
{
    return (tcpCon_pcst->aliveCheck_st.aliveCheckState_u8 == DOIP_ALIVECHECK_IDLE);
}

BufReq_ReturnType DoIP_AliveCheck_ReceivedNewMessage(DoIP_Type_TCPConnection* tcpCon_pst, const uint8* header_pu8, const uint8* payload_pu8)
{
    DOIP_MEMCPY(tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8, header_pu8, DOIP_HEADER_SIZE);

    DOIP_MEMCPY(&tcpCon_pst->aliveCheck_st.aliveCheckBuffer_au8[DOIP_HEADER_SIZE], payload_pu8, tcpCon_pst->RX_st.totalPayloadLength_u32);

    /* set RX.ProcessedLength */
    // a cast to uint16 is ok, because the alive check msg is smaller than a uint32 msg
    tcpCon_pst->RX_st.ProcessedLength_u16 += (uint16)tcpCon_pst->RX_st.totalPayloadLength_u32;

    tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_RECEIVED;
    return BUFREQ_OK;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

