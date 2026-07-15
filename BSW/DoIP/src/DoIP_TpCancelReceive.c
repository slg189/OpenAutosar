

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_TpCancelReceive_ParameterCheck(PduIdType DoIPPduRRxId)
{
    boolean retVal_b = FALSE;
    uint16 channelIdx_u16;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_CancelReceive, DOIP_E_UNINIT);
    }
    else if (!DoIP_Channel_GetChannelIdxForRxPduId(DoIPPduRRxId, &channelIdx_u16))
    {
        DOIP_DET(DOIP_DET_APIID_TP_CancelReceive, DOIP_E_INVALID_PDU_SDU_ID);
    }
    else
    {
        retVal_b = TRUE;
    }

    return retVal_b;

}

Std_ReturnType DoIP_TpCancelReceive(PduIdType DoIPPduRRxId)
{
    /* Perform paramter check */
    if (DoIP_TpCancelReceive_ParameterCheck(DoIPPduRRxId))
    {
        /* Currently not supported */
        DOIP_DET(DOIP_DET_APIID_TP_CancelReceive, DOIP_E_NOT_SUPPORTED);
    }

    return ( E_NOT_OK);
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

