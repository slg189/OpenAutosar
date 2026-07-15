

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

DoIP_ActivationLineRequestType DoIP_ActivationLineStatus_u8;
uint8 DoIP_AL_StatusChg_u8;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_ActivationLineSwitchInactive(void)
{
    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_ActivationLineSwitchInactive, DOIP_E_UNINIT);
    }
    else
    {
        // store an new ActivationLine change only if the previous change was processed completly
        if ((DoIP_ActivationLineStatus_u8 == DOIP_ACTIVATION_LINE_ACTIVE) && (DoIP_AL_StatusChg_u8 == DOIP_AL_HIGH_IDLE))
        {
            DoIP_AL_StatusChg_u8 = DOIP_AL_SWITCH_ONLINE2OFFLINE;
        }
        /* Set status accordingly */
        DoIP_ActivationLineStatus_u8 = DOIP_ACTIVATION_LINE_INACTIVE;


    }
}

void DoIP_ActivationLineSwitchActive(void)
{
    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_ActivationLineSwitchActive, DOIP_E_UNINIT);
    }
    else
    {
        // store an new ActivationLine change only if the previous change was processed completly
        if ((DoIP_ActivationLineStatus_u8 == DOIP_ACTIVATION_LINE_INACTIVE) && (DoIP_AL_StatusChg_u8 == DOIP_AL_LOW_IDLE))
        {
            DoIP_AL_StatusChg_u8 = DOIP_AL_SWITCH_OFFLINE2ONLINE;
        }

        /* Set status accordingly */
        DoIP_ActivationLineStatus_u8 = DOIP_ACTIVATION_LINE_ACTIVE;

    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

