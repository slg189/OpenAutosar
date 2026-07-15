/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef RBA_DEMOBDBASIC_IUMPREXTENSION_H
#define RBA_DEMOBDBASIC_IUMPREXTENSION_H

#include "rba_DemObdBasic_Types.h"

/**
 * \file
 * \ingroup DEM_PRJ_TPL
 * Called by Dem_DcmGetInfoTypeValue08/Dem_DcmGetInfoTypeValue0B.
 * Fill the output according your needs.
 *
 * Hint:
 * rba_DemObdBasic_Ratio_GetMinRatioIdxForGroup can be used to get the min ratio of the group.
 *
 */
Std_ReturnType rba_DemObdBasic_Iumpr_PrepareDcmOutput(Dcm_OpStatusType OpStatus, uint8* Iumprdata, uint8* IumprdataBufferSize);

#endif
