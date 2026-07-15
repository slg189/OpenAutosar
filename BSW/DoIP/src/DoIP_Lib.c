

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

static uint8 DoIP_VIN_au8[17];
boolean DoIP_IsVinReady_b;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

uint8* DoIP_IntGetVin(void)
{
    /* If DoIP_VIN_au8 is already set, no need to call Dcm */
    if (DoIP_IsVinReady_b == FALSE)
    {
        /* Call to Dcm_GetVin was succesful, DoIP_VIN_au8 is set */
        if (Dcm_GetVin(DoIP_VIN_au8) == E_OK)
        {
            DoIP_IsVinReady_b = TRUE;
        }
        /* Bytes should be filled with DoIPVinInvalidityPattern */
        else
        {
            DOIP_MEMSET(DoIP_VIN_au8, (sint8)DOIP_CFG_VinInvalidityPattern, 17u);
        }
    }

    return (DoIP_VIN_au8);
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

