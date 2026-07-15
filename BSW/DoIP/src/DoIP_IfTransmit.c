

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_IfTransmit_ParameterCheck(PduIdType DoIPPduRTxId, const PduInfoType* DoIPPduRTxInfoPtr)
{
    boolean retVal_b = FALSE;
    uint16 channelIdx_u16;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_IF_Transmit, DOIP_E_UNINIT) ;
    }
    else if (!DoIP_Channel_GetChannelIdxForTxPduId(DoIPPduRTxId, &channelIdx_u16))
    {
        DOIP_DET(DOIP_DET_APIID_IF_Transmit, DOIP_E_INVALID_PDU_SDU_ID) ;
    }
    else if (DoIPPduRTxInfoPtr == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_IF_Transmit, DOIP_E_PARAM_POINTER);
    }
    else
    {
        retVal_b = TRUE;
    }
    return retVal_b;
}

Std_ReturnType DoIP_IfTransmit(PduIdType DoIPPduRTxId, const PduInfoType* DoIPPduRTxInfoPtr)
{
    /* Perform paramter check */
    if (DoIP_IfTransmit_ParameterCheck(DoIPPduRTxId, DoIPPduRTxInfoPtr))
    {
        /* Currently not supported */
        DOIP_DET(DOIP_DET_APIID_IF_Transmit, DOIP_E_NOT_SUPPORTED) ;
    }
    return (E_NOT_OK);
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

