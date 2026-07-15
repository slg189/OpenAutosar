

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/* HIS METRIC LEVEL VIOLATION IN DoIP_TpTransmit_ParameterCheck: Each check is precondition for later checks/better readability */
static boolean DoIP_TpTransmit_ParameterCheck(PduIdType DoIPPduRTxId_uo, const PduInfoType* DoIPPduRTxInfo_pcst,
        uint16* channelIdx_pu16)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_Transmit, DOIP_E_UNINIT);
    }
    else if (DoIPPduRTxInfo_pcst == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_Transmit, DOIP_E_PARAM_POINTER);
    }
    else if (DoIPPduRTxInfo_pcst->SduLength == 0)
    {
        DOIP_DET(DOIP_DET_APIID_TP_Transmit, DOIP_E_INVALID_PARAMETER);
    }
    else if (!DoIP_Channel_GetChannelIdxForTxPduId(DoIPPduRTxId_uo, channelIdx_pu16))
    {
        DOIP_DET(DOIP_DET_APIID_TP_Transmit, DOIP_E_INVALID_PDU_SDU_ID);
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

Std_ReturnType DoIP_TpTransmit(PduIdType DoIPPduRTxId, const PduInfoType* DoIPPduRTxInfoPtr)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    uint16 channelIdx_u16;
    DoIP_Type_TCPConnection* tcpCon_pst = NULL_PTR;


    if (DoIP_TpTransmit_ParameterCheck(DoIPPduRTxId, DoIPPduRTxInfoPtr, &channelIdx_u16))
    {
        /* Get matching TCP-connection */
		if(TRUE == DoIP_TCPConnection_GetTCPConnForChannelIdx(channelIdx_u16, &tcpCon_pst))
        {
            /* Try to add new message to this channel */
            if (DoIP_Channel_NewMessage(channelIdx_u16, DoIPPduRTxInfoPtr))
            {
                retVal_u8 = E_OK;
            }

            /* If new message is stored in this channel, try to send */
            DoIP_Channel_SendMessage(tcpCon_pst, channelIdx_u16);
        }
    }
    return (retVal_u8);
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

