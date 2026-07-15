

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

boolean DoIP_Initialized_b;
const DoIP_ConfigType* DoIP_activeConfig_pcst;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

#if(DOIP_VARIANT == DOIP_POST_BUILD_SELECTABLE)
/***********************************************************************************************************************
 Function name    : DoIP_Prv_PbCfgCheck
 Syntax           : DoIP_Prv_PbCfgCheck(DoIPConfigPtr)
 Description      : Function to check PostBuild Configuration Pointer validity.
 Parameter        : const DoIP_ConfigType*
 Return value     : Std_ReturnType
***********************************************************************************************************************/
static Std_ReturnType DoIP_Prv_PbCfgCheck(const DoIP_ConfigType *DoIPConfigPtr)
{
    Std_ReturnType retval = E_NOT_OK;
    uint8 ConfigId;

    /*Check if the incoming DoIPConfigPtr is valid pointer and present in the file DoIP_PbCfg.c*/
        for(ConfigId = 0; ConfigId < DOIP_NO_OF_PBS_CONFIGSETS; ConfigId++)
        {
            if(DoIP_CfgPtrs[ConfigId] == DoIPConfigPtr)
            {
                retval = E_OK;
                break;
            }
        }

        return retval;
}
#endif



void DoIP_Init(const DoIP_ConfigType* DoIPConfigPtr)
{
    /* Set active config pointer */
#if(DOIP_VARIANT == DOIP_PRE_COMPILE)
    if (DoIPConfigPtr == NULL_PTR)
    {
        DoIP_activeConfig_pcst = &(DoIP_Config);
    }
    else
    {
        DoIP_activeConfig_pcst = DoIPConfigPtr;
    }
#else
    DoIP_activeConfig_pcst = DoIPConfigPtr;

    if(DoIP_activeConfig_pcst == NULL_PTR)
    {
        DOIP_DET(DOIP_DET_APIID_Init, DOIP_E_INIT_FAILED); /*TRACE:SWS_DoIP_00148*/
    }

#if(DOIP_VARIANT == DOIP_POST_BUILD_SELECTABLE)
    else if(DoIP_Prv_PbCfgCheck(DoIPConfigPtr) != E_OK)
    {
        DOIP_DET(DOIP_DET_APIID_Init, DOIP_E_INIT_FAILED); /*TRACE:SWS_DoIP_00148*/
    }
#endif
    else
#endif
    {
        /* Init connections */
        DoIP_Connection_Init();

        /* Set activationLineStatus*/
        DoIP_ActivationLineStatus_u8 = DOIP_ACTIVATION_LINE_INACTIVE;
        DoIP_AL_StatusChg_u8 = DOIP_AL_LOW_IDLE;

        /* IpAddrAssigment */
        DoIP_IsIPAddressAssignmentRequested_b = FALSE;
        DoIP_IsIPAddressAssigned_b = FALSE;
        DoIP_OpenConnections_b = FALSE;

        /* GIDVIN sync state */
        DoIP_IsVinReady_b = FALSE;

#if (TRUE == DOIP_CFG_VinGidMaster)
        DoIP_GIDSyncState_u8 = DOIP_GIDSYNC_IDLE;
#endif /* TRUE == DOIP_CFG_VinGidMaster */

        DoIP_Initialized_b = TRUE;

    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

