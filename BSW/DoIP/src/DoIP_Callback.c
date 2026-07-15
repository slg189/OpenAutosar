

#include "DoIP_Prv.h"

Std_ReturnType DoIP_Callback_GetGidCallback(uint8* Data_pu8)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    if (DoIP_activeConfig_pcst->callbackGetGid_pfct != NULL_PTR)
    {
        /* Call the configured callback-function */
        retVal_u8 = DoIP_activeConfig_pcst->callbackGetGid_pfct(Data_pu8);
    }
    return retVal_u8;
}

Std_ReturnType DoIP_Callback_PowerModeCallback(DoIP_PowerStateType* PowerState_pu8)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    if (DoIP_activeConfig_pcst->callbackPowerMode_pfct != NULL_PTR)
    {
        /* Call the configured callback-function */
        retVal_u8 = DoIP_activeConfig_pcst->callbackPowerMode_pfct(PowerState_pu8);
    }
    return retVal_u8;
}

Std_ReturnType DoIP_Callback_TriggerGIDSynchronization(void)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    if (DoIP_activeConfig_pcst->callbackTriggerGidSync_pfct != NULL_PTR)
    {
        /* Call the configured callback-function */
        retVal_u8 = DoIP_activeConfig_pcst->callbackTriggerGidSync_pfct();
    }
    return retVal_u8;
}

