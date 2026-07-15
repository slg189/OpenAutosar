
/* MR12 DIR 4.10 VIOLATION: No protection against multiple inclusion intended */
#define MEMMAP_ERROR

/* MR12 RULE 20.5 VIOLATION: AUTOSAR MemMap concept requires #undef, AUTOSAR MemMap requirements are incompatible to MISRA */
/*----------------------------------------------------------------------------*/
/* To be used for global or static constants */
/* All generated constant variables */
/*----------------------------------------------------------------------------*/
#if defined ETHTSYN_START_SEC_CONST_UNSPECIFIED
    #define BSW_START_SEC_DEFAULT_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_START_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
#elif defined ETHTSYN_STOP_SEC_CONST_UNSPECIFIED
    #define BSW_STOP_SEC_DEFAULT_CONST_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_STOP_SEC_CONST_UNSPECIFIED
    #undef MEMMAP_ERROR
/*----------------------------------------------------------------------------*/
/* Used for global or static variables that are
    cleared to zero after every reset */
/*----------------------------------------------------------------------------*/
#elif defined ETHTSYN_START_SEC_VAR_CLEARED_UNSPECIFIED
    #define BSW_START_SEC_VAR_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_START_SEC_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
#elif defined ETHTSYN_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #define BSW_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_STOP_SEC_VAR_CLEARED_UNSPECIFIED
    #undef MEMMAP_ERROR
/*----------------------------------------------------------------------------*/
/* Used for global or static variables that are
    initialized to values after every reset */
/*----------------------------------------------------------------------------*/
#elif defined ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
    /* Unspecified size variable access */
     #define BSW_START_SEC_VAR_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
#elif defined ETHTSYN_STOP_SEC_VAR_INIT_UNSPECIFIED
    #define BSW_STOP_SEC_VAR_INIT_UNSPECIFIED
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_STOP_SEC_VAR_INIT_UNSPECIFIED
    #undef MEMMAP_ERROR
/*----------------------------------------------------------------------------*/
/* Used for code Section */
/*----------------------------------------------------------------------------*/
#elif defined ETHTSYN_START_SEC_CODE
    #define  BSW_START_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_START_SEC_CODE
    #undef MEMMAP_ERROR
#elif defined ETHTSYN_STOP_SEC_CODE
    #define  BSW_STOP_SEC_DEFAULT_CODE
    #include "Bsw_MemMap.h"
    #undef ETHTSYN_STOP_SEC_CODE
    #undef MEMMAP_ERROR

#elif defined MEMMAP_ERROR
    #error "EthTSyn_MemMap.h, wrong #pragma command"
#endif
