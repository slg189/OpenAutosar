

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "SoAd_Prv.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

/**
 ***************************************************************************************************
 * \Function Name : SoAd_GetVersionInfo()
 *
 * \function description
 * Service for passing the Information through a pointer "*VersionInfo".
 *
 * Parameters (in)      : None
 *
 * Parameters (inout)   : None
 *
 * Parameters (out)     :
 * \param   Std_VersionInfoType
 * \arg     versioninfo -Pointer to where to store the version information of this module.
 *
 * Return value         : None
 *
 *
 ***************************************************************************************************
 */
#if (SOAD_VERSION_INFO_API == STD_ON)

void SoAd_GetVersionInfo(Std_VersionInfoType * versioninfo)
{
    /********************************* Start: DET Checking *************************************/

    /* Check whether versioninfo pointer is valid or not */
    SOAD_DET_CHECK_ERROR_VOID( ( NULL_PTR == versioninfo ), SOAD_GET_VERSION_INFO_API_ID, SOAD_E_PARAM_POINTER )

    /********************************* Start: DET Checking *************************************/

    versioninfo->vendorID          = SOAD_VENDOR_ID;
    versioninfo->moduleID          = SOAD_MODULE_ID;
    versioninfo->sw_major_version  = SOAD_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version  = SOAD_SW_MINOR_VERSION;
    versioninfo->sw_patch_version  = SOAD_SW_PATCH_VERSION;
}

#endif

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
