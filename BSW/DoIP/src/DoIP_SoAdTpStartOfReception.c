

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_SoAdTpStartOfReception_ParameterCheck(PduIdType RxPduId, PduLengthType TpSduLength,
        const PduLengthType* bufferSize_puo)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_TP_StartOfReception, DOIP_E_UNINIT);
    }
    else if (bufferSize_puo == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_TP_StartOfReception, DOIP_E_PARAM_POINTER);
    }
    else if (TpSduLength != 0)
    {
        DOIP_DET(DOIP_DET_APIID_TP_StartOfReception, DOIP_E_INVALID_PARAMETER);
    }
    else if (!DoIP_SoCon_IsTcpIndex(RxPduId))
    {
        DOIP_DET(DOIP_DET_APIID_TP_StartOfReception, DOIP_E_INVALID_PDU_SDU_ID);
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

BufReq_ReturnType DoIP_SoAdTpStartOfReception(PduIdType RxPduId, const PduInfoType* info, PduLengthType TpSduLength,
        PduLengthType* bufferSizePtr)
{
    BufReq_ReturnType retVal_b = BUFREQ_E_NOT_OK;

    /* "info" is defined because of compatibility aspects but is not used. */
    /* The following line uses the parameter and avoids a compiler warning: */
    (void)info;

    if (DoIP_SoAdTpStartOfReception_ParameterCheck(RxPduId, TpSduLength, bufferSizePtr))
    {
        /* DoIP does not have a own buffer for a whole message, but only some bytes to store header information.
         * In this call, we dont have any header information, for which target this request is addressed. Because of that we return
         * always the max buffer size.
         */
        *bufferSizePtr = DOIP_CFG_MAX_REQUEST_BYTES + DOIP_HEADER_SIZE;
        retVal_b = BUFREQ_OK;
    }

    return retVal_b;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

