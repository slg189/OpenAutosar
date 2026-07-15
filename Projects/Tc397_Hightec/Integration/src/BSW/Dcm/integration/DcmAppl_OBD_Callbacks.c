/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

#include "DcmAppl_OBD_Callbacks_Cfg.h"

#if (DCMAPPL_OBD_CALLBACKS_DEM_CFG_AVAILABLE)
#include "Dem_Cfg.h"

#if (DEM_CFG_OBD != DEM_CFG_OBD_OFF)


#include "rba_DemObdBasic_Dcm.h"
#include "rba_DemObdBasic_Prv.h"
#include "DcmDspObd_Mode1_Inf.h"


#define DEM_START_SEC_CODE
#include "Dem_MemMap.h"



#define DEM_STOP_SEC_CODE
#include "Dem_MemMap.h"

#endif

#endif
