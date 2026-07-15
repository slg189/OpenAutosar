

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_IfCancelTransmit_ParameterCheck(PduIdType DoIPPduRTxId_uo)
{
    boolean retVal_b = FALSE;
    uint16 channelIdx_u16;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_IF_CancelTransmit, DOIP_E_UNINIT) ;
    }
    else if (!DoIP_Channel_GetChannelIdxForTxPduId(DoIPPduRTxId_uo, &channelIdx_u16))
    {
        DOIP_DET(DOIP_DET_APIID_IF_CancelTransmit, DOIP_E_INVALID_PDU_SDU_ID) ;
    }
    else
    {
        retVal_b = TRUE;
    }
    return retVal_b;
}

Std_ReturnType DoIP_IfCancelTransmit(PduIdType DoIPPduRTxId)
{
    /* Perform paramter check */
    if (DoIP_IfCancelTransmit_ParameterCheck(DoIPPduRTxId))
    {
        /* Currently not supported */
        DOIP_DET(DOIP_DET_APIID_IF_CancelTransmit, DOIP_E_NOT_SUPPORTED);
    }

    return ( E_NOT_OK);
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

