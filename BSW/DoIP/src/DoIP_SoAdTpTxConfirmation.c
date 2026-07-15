

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_SoAdTpTxConfirmation_ParameterCheck(PduIdType TxPduId, Std_ReturnType result_u8)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_TxConfirmation, DOIP_E_UNINIT);
    }
    else if (!DoIP_SoCon_IsTcpIndex(TxPduId))
    {
        DOIP_DET(DOIP_DET_APIID_TP_TxConfirmation, DOIP_E_INVALID_PDU_SDU_ID);
    }
    /* Only E_OK or E_NOT_OK are valid */
    else if ((result_u8 != E_OK) && (result_u8 != E_NOT_OK))
    {
        DOIP_DET(DOIP_DET_APIID_TP_TxConfirmation, DOIP_E_INVALID_PARAMETER);
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

void DoIP_SoAdTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
    DoIP_Type_TCPConnection* tcpCon_pst = NULL_PTR;
    /* Perform paramter check */
    if (DoIP_SoAdTpTxConfirmation_ParameterCheck(TxPduId, result))
    {
        /* Map pduId to tcp-connection */
        tcpCon_pst = DoIP_SoCon_GetTCPCon(DoIP_SoCon_GetSocketConnection(TxPduId));

        /* Set inactivity timer once TxConfirmation for any transmition is received */
        DoIP_TCPConnection_SetGeneralInactivity(tcpCon_pst);

        /* Call subfunction depending on payloadType */
        switch (tcpCon_pst->TX_st.payloadType_u16)
        {
            case DOIP_PAYLOAD_TYPE_ALIVE_CHECK_REQ:
                DoIP_AliveCheck_TxConfirmation(tcpCon_pst);
                break;
            case DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_RESP:
                DoIP_RoutingActivation_TxConfirmation(tcpCon_pst);
                break;
            case DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK:
                DoIP_GenericAcknowledge_TpTxConfirmation(tcpCon_pst);
                break;
            case DOIP_PAYLOAD_TYPE_DIAG_MSG_ACK:
            case DOIP_PAYLOAD_TYPE_DIAG_MSG_NACK:
                DoIP_DiagnosticMessage_AcknowledgeTxConfirmation(tcpCon_pst);
                break;
            case DOIP_PAYLOAD_TYPE_DIAG_MSG:
                DoIP_DiagnosticMessage_TxConfirmation(tcpCon_pst, result);
                break;
            default:
                DOIP_DET(DOIP_DET_APIID_TP_TxConfirmation, DOIP_E_SHOULD_NOT_REACH);
                /* Nothing to do */
                break;
        }
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

