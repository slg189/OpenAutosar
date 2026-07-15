

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

/**
 **************************************************************************************************
 * DoIP_MainFunction - DoIP Main Function which shall be invoked cyclically by OS
 *
 * \param           void
 *
 * \retval          void
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

void DoIP_MainFunction(void)
{
    DoIP_IpAddressAssignmentMainFunction();

#if (TRUE == DOIP_CFG_VinGidMaster)
    /* MR12 RULE 13.5 VIOLATION: No persistent side effects inside of DoIP_Callback_TriggerGIDSynchronization()*/
    if ((DoIP_GIDSyncState_u8 == DOIP_GIDSYNC_PENDING) &&
        (E_OK == DoIP_Callback_TriggerGIDSynchronization()))
    {
        DoIP_GIDSyncState_u8 = DOIP_GIDSYNC_SUCCESS;
    }
#endif /* TRUE == DOIP_CFG_VinGidMaster */

    DoIP_Connection_MainFunction();
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

