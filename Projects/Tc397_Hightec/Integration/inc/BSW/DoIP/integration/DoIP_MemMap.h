/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */


/* MemMap.h for AUTOSAR DoIP R4.2 Rev 2 */


/* MR12 RULE 1.2 VIOLATION: MemMap header file should not have header file protection */
#define MEMMAP_ERROR

/* ============================   BSW   ============================ */
#if defined DOIP_START_SEC_VAR_CLEARED
    #if defined DOIP_WAS_STARTED_SEC_VAR_CLEARED
        #error DOIP_WAS_STARTED_SEC_VAR_CLEARED already defined
    #endif
    #define BSW_START_SEC_DEFAULT_RAM0_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_RAM0_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_VAR_CLEARED
    #define DOIP_WAS_STARTED_SEC_VAR_CLEARED

#elif defined DOIP_STOP_SEC_VAR_CLEARED
    #if ! defined DOIP_WAS_STARTED_SEC_VAR_CLEARED
        #error DOIP_STOP_SEC_VAR_CLEARED without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_VAR_CLEARED
    #endif
    #define BSW_STOP_SEC_DEFAULT_RAM0_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_RAM0_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_VAR_CLEARED

#elif defined DOIP_START_SEC_VAR_INIT
    #if defined DOIP_WAS_STARTED_SEC_VAR_INIT
        #error DOIP_WAS_STARTED_SEC_VAR_INIT already defined
    #endif
    #define BSW_START_SEC_DEFAULT_RAM0_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_RAM0_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_VAR_INIT
    #define DOIP_WAS_STARTED_SEC_VAR_INIT

#elif defined DOIP_STOP_SEC_VAR_INIT
    #if ! defined DOIP_WAS_STARTED_SEC_VAR_INIT
        #error DOIP_STOP_SEC_VAR_INIT without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_VAR_INIT
    #endif
    #define BSW_STOP_SEC_DEFAULT_RAM0_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_RAM0_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_VAR_INIT

#elif defined DOIP_START_SEC_CONST
    #if defined DOIP_WAS_STARTED_SEC_CONST
        #error DOIP_WAS_STARTED_SEC_CONST already defined
    #endif
    #define BSW_START_SEC_DEFAULT_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_CONST
    #define DOIP_WAS_STARTED_SEC_CONST

#elif defined DOIP_STOP_SEC_CONST
    #if ! defined DOIP_WAS_STARTED_SEC_CONST
        #error DOIP_STOP_SEC_CONST without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_CONST
    #endif
    #define BSW_STOP_SEC_DEFAULT_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_CONST

#elif defined DOIP_START_SEC_CODE
    #if defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE already defined
    #endif
    #define BSW_START_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_CODE
    #define DOIP_WAS_STARTED_SEC_CODE

#elif defined DOIP_STOP_SEC_CODE
    #if ! defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_STOP_SEC_CODE without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_CODE
    #endif
    #define BSW_STOP_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_CODE

#elif defined DOIP_START_SEC_CODE
    #if defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE already defined
    #endif
    #define BSW_START_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_CODE
    #define DOIP_WAS_STARTED_SEC_CODE

#elif defined DOIP_STOP_SEC_CODE
    #if ! defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_CODE
    #endif
    #define BSW_STOP_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_CODE

#elif defined DoIP_START_SEC_CODE
    #if defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE already defined
    #endif
    #define BSW_START_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DoIP_START_SEC_CODE
    #define DOIP_WAS_STARTED_SEC_CODE

#elif defined DoIP_STOP_SEC_CODE
    #if ! defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_CODE
    #endif
    #define BSW_STOP_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DoIP_STOP_SEC_CODE

#elif defined DOIP_START_SEC_CONFIG_DATA_UNSPECIFIED
    #if defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE already defined
    #endif
    #define BSW_START_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_START_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_START_SEC_CONFIG_DATA_UNSPECIFIED
    #define DOIP_WAS_STARTED_SEC_CODE

#elif defined DOIP_STOP_SEC_CONFIG_DATA_UNSPECIFIED
    #if ! defined DOIP_WAS_STARTED_SEC_CODE
        #error DOIP_WAS_STARTED_SEC_CODE without START_SEC
    #else
        #undef DOIP_WAS_STARTED_SEC_CODE
    #endif
    #define BSW_STOP_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef BSW_STOP_SEC_DEFAULT_CODE
    #undef MEMMAP_ERROR
    #undef DOIP_STOP_SEC_CONFIG_DATA_UNSPECIFIED

#elif defined MEMMAP_ERROR
    #error "DoIP_MemMap.h, wrong #pragma command"
#endif
