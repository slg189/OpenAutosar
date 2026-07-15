

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_SoAdTpRxIndication_ParameterCheck(PduIdType RxPduId, Std_ReturnType result_u8)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_RxIndication, DOIP_E_UNINIT);
    }
    else if (!DoIP_SoCon_IsTcpIndex(RxPduId))
    {
        DOIP_DET(DOIP_DET_APIID_TP_RxIndication, DOIP_E_INVALID_PDU_SDU_ID);
    }
    /* Only E_OK or E_NOT_OK are valid */
    else if ((result_u8 != E_OK) && (result_u8 != E_NOT_OK))
    {
        DOIP_DET(DOIP_DET_APIID_TP_RxIndication, DOIP_E_INVALID_PARAMETER);
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

void DoIP_SoAdTpRxIndication(PduIdType RxPduId, Std_ReturnType result)
{
    PduIdType pdurPdu_uo = 0;
    DoIP_Type_TCPConnection *tcpCon_pst = NULL_PTR;

    /* Perform paramter check */
    if (DoIP_SoAdTpRxIndication_ParameterCheck(RxPduId, result))
    {
        /* Map pduId to tcp-connection */
        tcpCon_pst = DoIP_SoCon_GetTCPCon(DoIP_SoCon_GetSocketConnection(RxPduId));

        if (tcpCon_pst->RX_st.channelState_en == DOIP_CHANNEL_MULTIFRAME_MESSAGE_E)
        {
            if (DoIP_Channel_GetPduRRxPduId(tcpCon_pst->RX_st.channelIdx_u16, &pdurPdu_uo))
            {
                PduR_DoIPRxIndication(pdurPdu_uo, result);
            }
        }
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

