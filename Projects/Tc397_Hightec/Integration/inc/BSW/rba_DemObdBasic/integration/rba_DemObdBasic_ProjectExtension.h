/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef RBA_DEMOBDBASIC_PROJECTEXTENSION_H
#define RBA_DEMOBDBASIC_PROJECTEXTENSION_H

#include "Dem_Types.h"

/**
 * \file
 * \ingroup DEM_PRJ_TPL
 * \brief Variant handling for OBD support
 *
 * If DemRbOBDSupportObdVariantCoding is enabled, these method decide whether the OBD behaviour should be disabled.
 */

/**
 * \brief Whether to suppress the MIL output.
 *
 * This shall be used for OBD variant handling to switch between OBD and non OBD variants dynamically during runtime.
 * The intended usecase are projects which implement both OBD and non-OBD variants with same software.
 *
 * \return  TRUE if the MIL output shall be suppressed by variant handling. FALSE when normal OBD handling is required.
 *
 * \warning CAUTION: THIS CALLBACK SHOULD ONLY BE USED FOR VARIANT HANDLING. FOR OBD COMPLIANT VEHICLES THIS CALLBACK
 *                   MUST ALWAYS RETURN FALSE!!! OTHERWISE OBD CONFORMITY IS VIOLATED.
 *
 * Integration hint: Dem doesn't store the variant information in NVRAM. Variant information must be handled outside
 * the Dem and based on that this callback shall be implemented project specific to return TRUE or FALSE.
 * However this callback won't stop storage of permant DTCs. In addition to disabling the MIL, if it is required to
 * stop storage of permant DTCs in the non-OBD variant, then the callback
 * rba_DemObdBasic_IsPermanentDTCStorageDisabled() shall be used.
 */
boolean rba_DemObdBasic_IsObdDeactivatedByVariant(void);

/**
 * \brief Whether to suppress the storage of permanent DTCs to permanent memory.
 *
 * This callback is provided to stop the storage of OBD permanent DTCs for the usecase to stop storage of permanent
 * DTCs during production.
 *
 * \return  TRUE if the entering of OBD DTCs to permanent memory shall be blocked.
 *          FALSE when normal storage to permanent memory is required.
 *
 * \warning CAUTION: THIS CALLBACK SHOULD ONLY BE USED DURING PRODUCTION. NEVER USE THIS CALLBACK FOR OTHER PURPOSES!!!
 *                   OTHERWISE OBD CONFORMITY IS VIOLATED.
 *
 * Integration hint: Dem doesn't store the status of "permant DTCs storage being disabled" in NVRam and hence if this
 * blocking is required across power cycles, then it must be handled outside Dem and based on that this callback shall
 * be implemented project specific to return TRUE or FALSE.
 */
boolean rba_DemObdBasic_IsPermanentDTCStorageDisabled(void);

#endif
