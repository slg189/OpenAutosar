

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    /* Check if pointer is valid */
    if (versioninfo != NULL_PTR)
    {
        /* Set versioninfo to defined values */
        versioninfo->moduleID = DOIP_MODULE_ID;
        versioninfo->vendorID = DOIP_VENDOR_ID;
        versioninfo->sw_major_version = DOIP_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = DOIP_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = DOIP_SW_PATCH_VERSION;
    }
    else
    {
        DOIP_DET(DOIP_DET_APIID_GetVersionInfo, DOIP_E_PARAM_POINTER);
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

