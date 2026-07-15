/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */




/* All the pragma definitions needed by RTE & SchM are provided in this file. */
/* Projects shall include this file irrespective of whether RTE is enabled / disabled in WdgM. */
/* MR12 RULE 1.2, DIR 4.10 VIOLATION: MemMap header concept - no protection against multiple inclusion intended */
/*
 *********************************************************************************************************
 * Defines/Macros
 *********************************************************************************************************
*/

#ifndef WdgM_MEMMAP_H
#define WdgM_MEMMAP_H

/* MR12 RULE 20.5 VIOLATION: AUTOSAR MemMap concept requires #undef, AUTOSAR MemMap requirements are incompatible to MISRA */
#if defined WdgM_START_SEC_CODE
	#undef WdgM_START_SEC_CODE
	#define WDGM_START_SEC_CODE
#elif defined WdgM_STOP_SEC_CODE
	#undef WdgM_STOP_SEC_CODE
	#define WDGM_STOP_SEC_CODE
#elif defined WdgM_START_SEC_CODE_FAST
	#undef WdgM_START_SEC_CODE_FAST
	#define WDGM_START_SEC_CODE_FAST
#elif defined WdgM_STOP_SEC_CODE_FAST
	#undef  WdgM_STOP_SEC_CODE_FAST
	#define WDGM_STOP_SEC_CODE_FAST
#elif defined WdgM_START_SEC_CODE_SLOW
	#undef WdgM_START_SEC_CODE_SLOW
	#define WDGM_START_SEC_CODE_SLOW
#elif defined WdgM_STOP_SEC_CODE_SLOW
	#undef  WdgM_STOP_SEC_CODE_SLOW
	#define WDGM_STOP_SEC_CODE_SLOW	
#endif

#include "WdgM_Cfg_MemMap.h"
/*
***************************************************************
*/
#undef WdgM_MEMMAP_H
#endif//WdgM_MEMMAP_H
