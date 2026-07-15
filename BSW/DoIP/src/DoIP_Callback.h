

#ifndef DOIP_CALLBACK_H
#define DOIP_CALLBACK_H

#include "DoIP_Prv_Types.h"

Std_ReturnType DoIP_Callback_GetGidCallback(uint8* Data_pu8);
Std_ReturnType DoIP_Callback_PowerModeCallback(DoIP_PowerStateType* PowerState_pu8);
Std_ReturnType DoIP_Callback_TriggerGIDSynchronization(void);

#endif

