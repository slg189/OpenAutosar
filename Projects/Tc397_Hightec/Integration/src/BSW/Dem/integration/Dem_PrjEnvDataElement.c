/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



/**
 * \file
 * \ingroup DEM_PRJ_TPL
 * \brief Read functions for internal data elements
 *
 * Implement read functions for project-specific internal data elements that are referenced by
 * DemInternalDataElementClass/DemDataElementReadFnc
 */

#include "Dem_Prv_InternalAPI.h"
#include "Dem_PrjEnvDataElement.h"

/**
 * \brief Provide the data for an internal data element.
 *
 * Called when the data for the internal data element is needed (e.g. on freeze frame capture or getting the extended
 * data records) and the internal data element was configured to be DEM_PRJ_SPECIFIC_DATA_ON_RETRIEVE or
 * DEM_PRJ_SPECIFIC_DATA_ON_REPORT.
 *
 * Copy the method, change the name into something more meaningful and enter the name in
 * DemInternalDataElementClass\DemRbDataElementReadFnc
 *
 * \param[out] Buffer        The buffer to write the data of the internal data element into. The buffer has the size
 *                           specified in DemInternalDataElementClass/DemDataElementDataSize.
 * \param[in]  internalData  Information about the read request
 */
#define DEM_START_SEC_CODE
#include "Dem_MemMap.h"
Std_ReturnType Dem_ReadPrjInternalDataElement_0(uint8* Buffer, const Dem_InternalEnvData* internalData)
{
    /* Please fill your implementation here !
     * see following functions for examples:
     * Dem_ReadEventStatusByte, Dem_ReadAgingCtrUpCnt, Dem_ReadAgingCtrDownCnt, Dem_ReadOvflInd, Dem_ReadMaxFdcDuringCurrentCycle */

    return E_NOT_OK; /* Dummy return value is provided here. It has to be changed to something meaningful based on implementation */
}
#define DEM_STOP_SEC_CODE
#include "Dem_MemMap.h"