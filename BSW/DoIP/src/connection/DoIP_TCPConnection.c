

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

uint8 DoIP_NumOfActiveTCPConnections_u8;
DoIP_Type_TCPConnection DoIP_TCPConnection_ast[DOIP_CFG_MAX_NUM_OF_TCP_CONN];
static uint8 DoIP_TCPConnection_Index;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

#ifdef DoIP_TEST_REENTRANCY
#include  "DoIP_UnitTest_TestReentrancy.h"
#else
#define SIMULATE_REENTRANCY_TX(a,b,c,d)
#endif

void DoIP_TCPConnection_Create(DoIP_ConnectionType_tst* soCon_pst)
{
// do we need the all this, tcpCon is an local variable and will not have any effect.
    DoIP_Type_TCPConnection* tcpCon_pst;

    if (DoIP_TCPConnection_Index < DOIP_CFG_MAX_NUM_OF_TCP_CONN)
    {
        tcpCon_pst = &DoIP_TCPConnection_ast[DoIP_TCPConnection_Index];
        DoIP_TCPConnection_Index++;

        tcpCon_pst->soCon_pst = soCon_pst;
    }
}

void DoIP_TCPConnection_Init(void)
{
    DoIP_TCPConnection_Index = 0;
    DoIP_NumOfActiveTCPConnections_u8 = 0;
}


void DoIP_TCPConnection_MainFunction(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Timer */
    DoIP_TCPConnection_HandleTimer(tcpCon_pst);

    /* Routing Activation */
    DoIP_RoutingActivation_MainFunction(tcpCon_pst);

    /* AliveCheck */
    DoIP_AliveCheck_MainFunction(tcpCon_pst);

    /* Messages */
    DoIP_TCPConnection_HandleMessages(tcpCon_pst);
}

void DoIP_TCPConnection_HandleMessages(DoIP_Type_TCPConnection* tcpCon_pst)
{
    uint8 i;
    const DoIP_Cfg_TesterType_tst* tester;

    /* Local copy because of multi task*/
    tester = tcpCon_pst->ra_st.testerCfg_pcst;

    /* Is a tester active on this connection?*/
    if(tester != NULL_PTR)
    {
        /* Check Queue*/
        for (i = 0; i < tester->numOfchannel_u16; i++)
        {
            DoIP_Channel_SendMessage(tcpCon_pst, tester->ChannelJumpTableIdx_u16 + i);
        }
    }
}

void DoIP_TCPConnection_HandleTimer(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Alive check timer */
    if (tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 > 0u)
    {
        tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32--;

        if (tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 == 0u)
        {
            /* alive check timeout */
            DoIP_TCPConnection_SetResetNeeded(tcpCon_pst);
        }
    }
    /* Inactivity timer */
    if (tcpCon_pst->timerInactivity_u32 > 0u)
    {
        tcpCon_pst->timerInactivity_u32--;

        if (tcpCon_pst->timerInactivity_u32 == 0u)
        {
            /* connection is inactive */
            DoIP_TCPConnection_SetResetNeeded(tcpCon_pst);
        }
    }
}

void DoIP_TCPConnection_Open(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Initialize TCP socket */
    /* Initialize the Routing activation structure */
    DoIP_RoutingActivation_Reset(&tcpCon_pst->ra_st);

    tcpCon_pst->timerInactivity_u32 = DOIP_CFG_InitialInactivityTime_Cycles;

    /* Initialize the Alive check structure */
    tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_IDLE;
    tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 = 0;

    /* Init TX state machine structure */
    tcpCon_pst->TX_st.payloadType_u16 = 0;
    tcpCon_pst->TX_st.totalPayloadLength_u32 = 0;
    tcpCon_pst->TX_st.channelIdx_u16 = DOIP_INVALID_PDUID;
    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
    tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b = FALSE;
    tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_READY_E;

    /* Init RX state machine structure */
    tcpCon_pst->RX_st.payloadType_u16 = 0;
    tcpCon_pst->RX_st.totalPayloadLength_u32 = 0;
    tcpCon_pst->RX_st.channelIdx_u16 = DOIP_INVALID_PDUID;
    tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
    tcpCon_pst->RX_st.ProcessedLength_u16 = 0;

    tcpCon_pst->RxFragHdrBuffer_st.HeaderFragCurrentSize_u16 = 0;
    tcpCon_pst->RxFragHdrBuffer_st.DiagMsgFragCurrentSize_u16 = 0;

    DoIP_NumOfActiveTCPConnections_u8++;
}

void DoIP_TCPConnection_Close(DoIP_Type_TCPConnection* tcpCon_pst)
{
    /* Reset TCP socket */
    DoIP_Channel_ReleaseAllChannelsForTester(tcpCon_pst->ra_st.testerCfg_pcst, E_NOT_OK);

    DoIP_RoutingActivation_Reset(&tcpCon_pst->ra_st);

    tcpCon_pst->aliveCheck_st.aliveCheckState_u8 = DOIP_ALIVECHECK_IDLE;

    tcpCon_pst->RX_st.channelState_en = DOIP_CHANNEL_NEW_MESSAGE_E;
    tcpCon_pst->RX_st.channelIdx_u16 = DOIP_INVALID_PDUID;

    tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
    tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b = FALSE;
    tcpCon_pst->TX_st.txDiagACKBuffer_st.nackState_en = DOIP_TCP_TX_READY_E;
    tcpCon_pst->TX_st.channelIdx_u16 = DOIP_INVALID_PDUID;

    tcpCon_pst->aliveCheck_st.aliveCheckTimer_u32 = 0;
    tcpCon_pst->timerInactivity_u32 = 0;

    DoIP_NumOfActiveTCPConnections_u8--;
}

void DoIP_TCPConnection_SetCloseNeeded(const DoIP_Type_TCPConnection* tcpCon_pcst)
{
    DoIP_Connection_SetCloseNeeded(tcpCon_pcst->soCon_pst);
}

void DoIP_TCPConnection_SetResetNeeded(const DoIP_Type_TCPConnection* tcpCon_pcst)
{
    DoIP_Connection_SetResetNeeded(tcpCon_pcst->soCon_pst);
}

boolean DoIP_TCPConnection_SendMessage(DoIP_Type_TCPConnection* tcpCon_pst, uint16 channelIdx_u16, const uint8* msg_pu8,
        boolean isDiagMessage)
{
    boolean retVal_b = FALSE;
    PduInfoType txInfo_pst;
    boolean isTCPConnectionLocked_b = FALSE;


    /* Lock */
    if (tcpCon_pst->TX_st.tcpState_en == DOIP_TCP_TX_READY_E)
    {
        SchM_Enter_DoIP();
        if (tcpCon_pst->TX_st.tcpState_en == DOIP_TCP_TX_READY_E)
        {
            DOIP_ASSERT((tcpCon_pst->TX_st.channelIdx_u16 == DOIP_INVALID_PDUID),
                    DOIP_DET_APIID_TCPCONNECTION, DOIP_E_ASSERT);
            tcpCon_pst->TX_st.channelIdx_u16 = channelIdx_u16;
            tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_BLOCKED_E;
            isTCPConnectionLocked_b = TRUE;
        }
        SchM_Exit_DoIP();
    }

    if  (TRUE == isTCPConnectionLocked_b)
    {
        /* Get values from msg buffer_au8 - msg_pu8 is in network byte order */
        tcpCon_pst->TX_st.payloadType_u16 = DoIP_Header_GetPayloadType(msg_pu8);
        tcpCon_pst->TX_st.totalPayloadLength_u32 = DoIP_Header_GetPayloadLength(msg_pu8);

//        This macro points to a test function, which is used to check the reentrancy of this function.
//        This macro is only enabled for test purposes
        SIMULATE_REENTRANCY_TX(tcpCon_pst, channelIdx_u16, msg_pu8, isDiagMessage);

        /* Set txInfo_pst */
        txInfo_pst.SduLength = (PduLengthType) (tcpCon_pst->TX_st.totalPayloadLength_u32 + DOIP_HEADER_SIZE);
        // txInfo_pst.SduDataPtr is in every case not relevant because of the autosar TP transport mechanism
        // Data copied only with function CopyTxData(). Lower layer provides target buffer for upper layer (created by sdu length
        // parameter in API call TpTransmit())
        txInfo_pst.SduDataPtr = NULL_PTR;

        tcpCon_pst->TX_st.lastKnownBufferSize_uo = 0;

        /* TODO check if call to SoAd_IfTransmit should also be supported here */
        if (SoAd_TpTransmit(DoIP_SoCon_GetSoAdPdu(tcpCon_pst->soCon_pst), &txInfo_pst) != E_OK)
        {
            //todo: do we need this if? is a close of a connection in generic nack or nack/ack case correct?
            if (isDiagMessage)
            {
                DoIP_Channel_Release(tcpCon_pst->TX_st.channelIdx_u16, E_NOT_OK);
                tcpCon_pst->TX_st.channelIdx_u16 = DOIP_INVALID_PDUID;
            }
            else
            {
                DoIP_TCPConnection_SetCloseNeeded(tcpCon_pst);
            }
            tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
        }
        else
        {
            tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_SENDING_E;
            retVal_b = TRUE;
        }
    }

    return retVal_b;
}

boolean DoIP_TCPConnection_GetTCPConnForChannelIdx(uint16 channelIdx_u16, DoIP_Type_TCPConnection** tcpCon_ppst)
{
    uint16 sa_u16;
    uint16 ta_u16;
    uint16 tcpIdx_u16;
    boolean retVal_b = FALSE;
    DoIP_Type_TCPConnection *tmpTcpCon_p = NULL_PTR;

    DOIP_ASSERT((channelIdx_u16 < DoIP_activeConfig_pcst->channelSize_u16), DOIP_DET_APIID_TCPCONNECTION, DOIP_E_ASSERT);

    for (tcpIdx_u16 = 0; tcpIdx_u16 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; tcpIdx_u16++)
    {
        sa_u16 = DoIP_activeConfig_pcst->testerCfg_pcst[DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].testerIdx_u8].testerSA_u16;
        ta_u16 = DoIP_activeConfig_pcst->channel_pcst[channelIdx_u16].targetAddr_u16;

        tmpTcpCon_p = &DoIP_TCPConnection_ast[tcpIdx_u16];

        /* Find TCP-connection (Connection is active + SA is registered)*/
        if (DoIP_SoCon_IsActive(tmpTcpCon_p->soCon_pst))
        {
            /* Check if RA is active */
            if (DoIP_RoutingActivation_IsSourceAddressActive(&tmpTcpCon_p->ra_st, sa_u16))
            {
                /* Check if TA is enabled by RA */
                if (DoIP_DiagnosticMessageCheckSaTaAtConfiguration(sa_u16, ta_u16, tmpTcpCon_p->ra_st.raCfg_pcst))
                {
                    *tcpCon_ppst = tmpTcpCon_p;
                    retVal_b = TRUE;
                    break;
                }
            }
        }
    }

    return retVal_b;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

