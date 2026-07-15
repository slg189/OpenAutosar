/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



/*********************************************************************************************************************
* CONTAINS THE BSWM-MEMMAP SEC CODE DECLARATIONS
*********************************************************************************************************************/

/* Code Section */
#if defined BswM_START_SEC_CODE
    #undef BswM_START_SEC_CODE
    #define  BSWM_START_SEC_CODE
#elif defined BswM_STOP_SEC_CODE
    #undef BswM_STOP_SEC_CODE
    #define  BSWM_STOP_SEC_CODE
#endif

#if defined BSWM_START_SEC_CODE
    #define  BSW_START_SEC_CODE
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CODE
#elif defined BSWM_STOP_SEC_CODE
    #define  BSW_STOP_SEC_CODE
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CODE

/* Variable Section */
#elif defined BSWM_START_SEC_VAR_INIT_BOOLEAN
    #define  BSW_START_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_INIT_BOOLEAN
#elif defined BSWM_STOP_SEC_VAR_INIT_BOOLEAN
    #define  BSW_STOP_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_INIT_BOOLEAN

#elif defined BSWM_START_SEC_VAR_INIT_8
    #define  BSW_START_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_INIT_8
#elif defined BSWM_STOP_SEC_VAR_INIT_8
    #define  BSW_STOP_SEC_VAR_INIT_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_INIT_8

#elif defined BSWM_START_SEC_VAR_INIT_16
    #define  BSW_START_SEC_VAR_INIT_16
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_INIT_16
#elif defined BSWM_STOP_SEC_VAR_INIT_16
    #define  BSW_STOP_SEC_VAR_INIT_16
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_INIT_16
	
#elif defined BSWM_START_SEC_VAR_INIT_32
    #define  BSW_START_SEC_VAR_INIT_32
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_INIT_32
#elif defined BSWM_STOP_SEC_VAR_INIT_32
    #define  BSW_STOP_SEC_VAR_INIT_32
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_INIT_32

#elif defined BSWM_START_SEC_VAR_INIT_UNSPECIFIED
    #define  BSW_START_SEC_VAR_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_INIT_UNSPECIFIED
#elif defined BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED
    #define  BSW_STOP_SEC_VAR_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_INIT_UNSPECIFIED

#elif defined BSWM_START_SEC_VAR_CLEARED_BOOLEAN
    #define  BSW_START_SEC_VAR_CLEARED_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_CLEARED_BOOLEAN
#elif defined BSWM_STOP_SEC_VAR_CLEARED_BOOLEAN
    #define  BSW_STOP_SEC_VAR_CLEARED_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_CLEARED_BOOLEAN

#elif defined BSWM_START_SEC_VAR_CLEARED_8
    #define  BSW_START_SEC_VAR_CLEARED_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_CLEARED_8
#elif defined BSWM_STOP_SEC_VAR_CLEARED_8
    #define  BSW_STOP_SEC_VAR_CLEARED_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_CLEARED_8

#elif defined BSWM_START_SEC_VAR_CLEARED_16
    #define  BSW_START_SEC_VAR_CLEARED_16
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_CLEARED_16
#elif defined BSWM_STOP_SEC_VAR_CLEARED_16
    #define  BSW_STOP_SEC_VAR_CLEARED_16
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_CLEARED_16
	
#elif defined BSWM_START_SEC_VAR_CLEARED_32
    #define  BSW_START_SEC_VAR_CLEARED_32
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_CLEARED_32
#elif defined BSWM_STOP_SEC_VAR_CLEARED_32
    #define  BSW_STOP_SEC_VAR_CLEARED_32
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_CLEARED_32

#elif defined BSWM_START_SEC_VAR_CLEARED_UNSPECIFIED
    #define  BSW_START_SEC_VAR_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_CLEARED_UNSPECIFIED
#elif defined BSWM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #define  BSW_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_CLEARED_UNSPECIFIED

/*Constant Section */
#elif defined BSWM_START_SEC_CONST_8
    #define  BSW_START_SEC_CONST_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONST_8
#elif defined BSWM_STOP_SEC_CONST_8
    #define  BSW_STOP_SEC_CONST_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONST_8

#elif defined BSWM_START_SEC_CONST_16
    #define  BSW_START_SEC_CONST_16
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONST_16
#elif defined BSWM_STOP_SEC_CONST_16
    #define  BSW_STOP_SEC_CONST_16
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONST_16
	
#elif defined BSWM_START_SEC_CONST_32
    #define  BSW_START_SEC_CONST_32
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONST_32
#elif defined BSWM_STOP_SEC_CONST_32
    #define  BSW_STOP_SEC_CONST_32
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONST_32

#elif defined BSWM_START_SEC_CONST_UNSPECIFIED
    #define  BSW_START_SEC_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONST_UNSPECIFIED
#elif defined BSWM_STOP_SEC_CONST_UNSPECIFIED
    #define  BSW_STOP_SEC_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONST_UNSPECIFIED	

#elif defined BSWM_START_SEC_CONFIG_DATA_POSTBUILD_8
    #define  BSW_START_SEC_CONFIG_DATA_POSTBUILD_8
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONFIG_DATA_POSTBUILD_8
#elif defined BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_8
    #define  BSW_STOP_SEC_CONFIG_DATA_POSTBUILD_8
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_8

#elif defined BSWM_START_SEC_CONFIG_DATA_POSTBUILD_16
    #define  BSW_START_SEC_CONFIG_DATA_POSTBUILD_16
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONFIG_DATA_POSTBUILD_16
#elif defined BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_16
    #define  BSW_STOP_SEC_CONFIG_DATA_POSTBUILD_16
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_16
	
#elif defined BSWM_START_SEC_CONFIG_DATA_POSTBUILD_32
    #define  BSW_START_SEC_CONFIG_DATA_POSTBUILD_32
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONFIG_DATA_POSTBUILD_32
#elif defined BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_32
    #define  BSW_STOP_SEC_CONFIG_DATA_POSTBUILD_32
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_32

#elif defined BSWM_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #define  BSW_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
#elif defined BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #define  BSW_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_CONFIG_DATA_POSTBUILD_UNSPECIFIED	

#elif defined BSWM_START_SEC_VAR_SAVED_ZONE0_UNSPECIFIED
    #define  BSW_START_SEC_VAR_SAVED_ZONE_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_START_SEC_VAR_SAVED_ZONE0_UNSPECIFIED
#elif defined BSWM_STOP_SEC_VAR_SAVED_ZONE0_UNSPECIFIED
    #define  BSW_STOP_SEC_VAR_SAVED_ZONE_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef BSWM_STOP_SEC_VAR_SAVED_ZONE0_UNSPECIFIED		

#else    
    #error "BswM_MemMap.h, wrong #pragma command"    
#endif


/*
 **********************************************************************************************************************
 * End of header file *
 **********************************************************************************************************************
*/
