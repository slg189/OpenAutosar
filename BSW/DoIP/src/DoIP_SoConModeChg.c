

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_SoConModeChg_ParameterCheck(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_SoConModeChg, DOIP_E_UNINIT);
    }
    else if ((DoIP_ActivationLineStatus_u8 != DOIP_ACTIVATION_LINE_ACTIVE) && (Mode == SOAD_SOCON_ONLINE))
    {
        /*Do nothing As per SWS_DoIP_00202 just ignore the request and return negative value*/
        /* It is required to accept Mode as OFFLINE even when ActivationLineStatus is INACTIVE to close the according Connection below */

    }
    else if (!DoIP_SoCon_IsSoAdSoConIdValid(SoConId))
    {
        DOIP_DET(DOIP_DET_APIID_SoConModeChg, DOIP_E_INVALID_PARAMETER);
    }
    else
    {
        retVal_b = TRUE;
    }

    return retVal_b;
}

void DoIP_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode)
{
    if (DoIP_SoConModeChg_ParameterCheck(SoConId, Mode))
    {
        DoIP_ConnectionType_tst* soCon_pcst = DoIP_SoCon_GetForSoConId(SoConId);
        soCon_pcst->soAdConMode_en = Mode;

        (Mode != SOAD_SOCON_ONLINE) ? DoIP_Connection_Close(soCon_pcst): DoIP_Connection_Open(soCon_pcst);
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

