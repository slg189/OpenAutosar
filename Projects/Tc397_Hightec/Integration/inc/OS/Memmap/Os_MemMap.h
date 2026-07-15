/*
 * This is Os_MemMap.h, auto-generated for:
 *   Project: TC397
 *   Target:  TC397
 *   Variant: ETAS Entry Platfrom
 *   Version:
 *   It should be either copied into MemMap.h by the integrator OR #included into it.
 */
/* Generator info: generate_always */
/* This file is a sample version of Os_MemMap.h that can be used
 * as a starting template.
 * [$UKS 652] [$UKS 653] [$UKS 655] [$UKS 656] [$UKS 657]
 */

#if defined (NOT_DEFINED)

/* OS Uninitialized RAM: trace data */
#elif defined (OS_START_SEC_VAR_TRACE)
  #undef      OS_START_SEC_VAR_TRACE
  #define SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED
#elif defined (OS_STOP_SEC_VAR_TRACE)
  #undef      OS_STOP_SEC_VAR_TRACE
  #define SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED

/* OS Uninitialized RAM: boolean */
#elif defined (OS_START_SEC_VAR_NO_INIT_BOOLEAN)
  #undef      OS_START_SEC_VAR_NO_INIT_BOOLEAN
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_NO_INIT_BOOLEAN)
  #undef      OS_STOP_SEC_VAR_NO_INIT_BOOLEAN
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS Uninitialized RAM: 8 bit values */
#elif defined (OS_START_SEC_VAR_NO_INIT_8)
  #undef      OS_START_SEC_VAR_NO_INIT_8
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_NO_INIT_8)
  #undef      OS_STOP_SEC_VAR_NO_INIT_8
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS Uninitialized RAM: 16 bit values */
#elif defined (OS_START_SEC_VAR_NO_INIT_16)
  #undef      OS_START_SEC_VAR_NO_INIT_16
  #define SHARED_START_SEC_VAR_CLEARED_16
#elif defined (OS_STOP_SEC_VAR_NO_INIT_16)
  #undef      OS_STOP_SEC_VAR_NO_INIT_16
  #define SHARED_STOP_SEC_VAR_CLEARED_16

/* OS Uninitialized RAM: 32 bit values */
#elif defined (OS_START_SEC_VAR_NO_INIT_32)
  #undef      OS_START_SEC_VAR_NO_INIT_32
  #define SHARED_START_SEC_VAR_CLEARED_32
#elif defined (OS_STOP_SEC_VAR_NO_INIT_32)
  #undef      OS_STOP_SEC_VAR_NO_INIT_32
  #define SHARED_STOP_SEC_VAR_CLEARED_32

/* OS Uninitialized RAM: unspecified size */
#elif defined (OS_START_SEC_VAR_NO_INIT_UNSPECIFIED)
  #undef      OS_START_SEC_VAR_NO_INIT_UNSPECIFIED
  #define SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED
  
#elif defined (OS_STOP_SEC_VAR_NO_INIT_UNSPECIFIED)
  #undef      OS_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
  #define SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED
  

/* OS reset cleared RAM: boolean */
#elif defined (OS_START_SEC_VAR_CLEARED_BOOLEAN)
  #undef      OS_START_SEC_VAR_CLEARED_BOOLEAN
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_CLEARED_BOOLEAN)
  #undef      OS_STOP_SEC_VAR_CLEARED_BOOLEAN
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS reset cleared RAM: 8 bit values */
#elif defined (OS_START_SEC_VAR_CLEARED_8)
  #undef      OS_START_SEC_VAR_CLEARED_8
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_CLEARED_8)
  #undef      OS_STOP_SEC_VAR_CLEARED_8
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS reset cleared RAM: 16 bit values */
#elif defined (OS_START_SEC_VAR_CLEARED_16)
  #undef      OS_START_SEC_VAR_CLEARED_16
  #define SHARED_START_SEC_VAR_CLEARED_16
#elif defined (OS_STOP_SEC_VAR_CLEARED_16)
  #undef      OS_STOP_SEC_VAR_CLEARED_16
  #define SHARED_STOP_SEC_VAR_CLEARED_16

/* OS reset cleared RAM: 32 bit values */
#elif defined (OS_START_SEC_VAR_CLEARED_32)
  #undef      OS_START_SEC_VAR_CLEARED_32
  #define SHARED_START_SEC_VAR_CLEARED_32
#elif defined (OS_STOP_SEC_VAR_CLEARED_32)
  #undef      OS_STOP_SEC_VAR_CLEARED_32
  #define SHARED_STOP_SEC_VAR_CLEARED_32

/* OS reset cleared RAM: unspecified size */
#elif defined (OS_START_SEC_VAR_CLEARED_UNSPECIFIED)
  #undef      OS_START_SEC_VAR_CLEARED_UNSPECIFIED
  #define SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED

#elif defined (OS_STOP_SEC_VAR_CLEARED_UNSPECIFIED)
  #undef      OS_STOP_SEC_VAR_CLEARED_UNSPECIFIED
  #define SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED

/* OS reset initialized RAM: boolean */
#elif defined (OS_START_SEC_VAR_INIT_BOOLEAN)
  #undef      OS_START_SEC_VAR_INIT_BOOLEAN
  #define SHARED_START_SEC_VAR_INIT_8
#elif defined (OS_STOP_SEC_VAR_INIT_BOOLEAN)
  #undef      OS_STOP_SEC_VAR_INIT_BOOLEAN
  #define SHARED_STOP_SEC_VAR_INIT_8

/* OS reset initialized RAM: 8 bit values */
#elif defined (OS_START_SEC_VAR_INIT_8)
  #undef      OS_START_SEC_VAR_INIT_8
  #define SHARED_START_SEC_VAR_INIT_8
#elif defined (OS_STOP_SEC_VAR_INIT_8)
  #undef      OS_STOP_SEC_VAR_INIT_8
  #define SHARED_STOP_SEC_VAR_INIT_8

/* OS reset initialized RAM: 16 bit values */
#elif defined (OS_START_SEC_VAR_INIT_16)
  #undef      OS_START_SEC_VAR_INIT_16
  #define SHARED_START_SEC_VAR_INIT_16
#elif defined (OS_STOP_SEC_VAR_INIT_16)
  #undef      OS_STOP_SEC_VAR_INIT_16
  #define SHARED_STOP_SEC_VAR_INIT_16

/* OS reset initialized RAM: 32 bit values */
#elif defined (OS_START_SEC_VAR_INIT_32)
  #undef      OS_START_SEC_VAR_INIT_32
  #define SHARED_START_SEC_VAR_INIT_32
#elif defined (OS_STOP_SEC_VAR_INIT_32)
  #undef      OS_STOP_SEC_VAR_INIT_32
  #define SHARED_STOP_SEC_VAR_INIT_32

/* OS reset initialized RAM: unspecified size */
#elif defined (OS_START_SEC_VAR_INIT_UNSPECIFIED)
  #undef      OS_START_SEC_VAR_INIT_UNSPECIFIED
  #define SHARED_START_SEC_VAR_INIT_UNSPECIFIED
#elif defined (OS_STOP_SEC_VAR_INIT_UNSPECIFIED)
  #undef      OS_STOP_SEC_VAR_INIT_UNSPECIFIED
  #define SHARED_STOP_SEC_VAR_INIT_UNSPECIFIED

/* OS Power-on cleared RAM: boolean */
#elif defined (OS_START_SEC_VAR_POWER_ON_CLEARED_BOOLEAN)
  #undef      OS_START_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_POWER_ON_CLEARED_BOOLEAN)
  #undef      OS_STOP_SEC_VAR_POWER_ON_CLEARED_BOOLEAN
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS Power-on cleared RAM: 8 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_CLEARED_8)
  #undef      OS_START_SEC_VAR_POWER_ON_CLEARED_8
  #define SHARED_START_SEC_VAR_CLEARED_8
#elif defined (OS_STOP_SEC_VAR_POWER_ON_CLEARED_8)
  #undef      OS_STOP_SEC_VAR_POWER_ON_CLEARED_8
  #define SHARED_STOP_SEC_VAR_CLEARED_8

/* OS Power-on cleared RAM: 16 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_CLEARED_16)
  #undef      OS_START_SEC_VAR_POWER_ON_CLEARED_16
  #define SHARED_START_SEC_VAR_CLEARED_16
#elif defined (OS_STOP_SEC_VAR_POWER_ON_CLEARED_16)
  #undef      OS_STOP_SEC_VAR_POWER_ON_CLEARED_16
  #define SHARED_STOP_SEC_VAR_CLEARED_16

/* OS Power-on cleared RAM: 32 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_CLEARED_32)
  #undef      OS_START_SEC_VAR_POWER_ON_CLEARED_32
  #define SHARED_START_SEC_VAR_CLEARED_32
#elif defined (OS_STOP_SEC_VAR_POWER_ON_CLEARED_32)
  #undef      OS_STOP_SEC_VAR_POWER_ON_CLEARED_32
  #define SHARED_STOP_SEC_VAR_CLEARED_16

/* OS Power-on cleared RAM: unspecified size */
#elif defined (OS_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED)
  #undef      OS_START_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
  #define SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED
#elif defined (OS_STOP_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED)
  #undef      OS_STOP_SEC_VAR_POWER_ON_CLEARED_UNSPECIFIED
  #define SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED

/* OS Power-on initialized RAM: boolean */
#elif defined (OS_START_SEC_VAR_POWER_ON_INIT_BOOLEAN)
  #undef      OS_START_SEC_VAR_POWER_ON_INIT_BOOLEAN
  #define SHARED_START_SEC_VAR_INIT_8
#elif defined (OS_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN)
  #undef      OS_STOP_SEC_VAR_POWER_ON_INIT_BOOLEAN
  #define SHARED_STOP_SEC_VAR_INIT_8

/* OS Power-on initialized RAM: 8 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_INIT_8)
  #undef      OS_START_SEC_VAR_POWER_ON_INIT_8
  #define SHARED_START_SEC_VAR_INIT_8
#elif defined (OS_STOP_SEC_VAR_POWER_ON_INIT_8)
  #undef      OS_STOP_SEC_VAR_POWER_ON_INIT_8
  #define SHARED_STOP_SEC_VAR_INIT_8

/* OS Power-on initialized RAM: 16 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_INIT_16)
  #undef      OS_START_SEC_VAR_POWER_ON_INIT_16
  #define SHARED_START_SEC_VAR_INIT_16
#elif defined (OS_STOP_SEC_VAR_POWER_ON_INIT_16)
  #undef      OS_STOP_SEC_VAR_POWER_ON_INIT_16
  #define SHARED_STOP_SEC_VAR_INIT_16

/* OS Power-on initialized RAM: 32 bit values */
#elif defined (OS_START_SEC_VAR_POWER_ON_INIT_32)
  #undef      OS_START_SEC_VAR_POWER_ON_INIT_32
  #define SHARED_START_SEC_VAR_INIT_32
#elif defined (OS_STOP_SEC_VAR_POWER_ON_INIT_32)
  #undef      OS_STOP_SEC_VAR_POWER_ON_INIT_32
  #define SHARED_STOP_SEC_VAR_INIT_32

/* OS Power-on initialized RAM: unspecified size */
#elif defined (OS_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
  #undef      OS_START_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
  #define SHARED_START_SEC_VAR_INIT_UNSPECIFIED
#elif defined (OS_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED)
  #undef      OS_STOP_SEC_VAR_POWER_ON_INIT_UNSPECIFIED
  #define SHARED_STOP_SEC_VAR_INIT_UNSPECIFIED

/* OS Const (ROM): boolean */
#elif defined (OS_START_SEC_CONST_BOOLEAN)
  #undef      OS_START_SEC_CONST_BOOLEAN
  #define SHARED_START_SEC_CONST_8
#elif defined (OS_STOP_SEC_CONST_BOOLEAN)
  #undef      OS_STOP_SEC_CONST_BOOLEAN
  #define SHARED_STOP_SEC_CONST_8

/* OS Const (ROM): 8 bit values */
#elif defined (OS_START_SEC_CONST_8)
  #undef      OS_START_SEC_CONST_8
  #define SHARED_START_SEC_CONST_8
#elif defined (OS_STOP_SEC_CONST_8)
  #undef      OS_STOP_SEC_CONST_8
  #define SHARED_STOP_SEC_CONST_8

/* OS Const (ROM): 16 bit values */
#elif defined (OS_START_SEC_CONST_16)
  #undef      OS_START_SEC_CONST_16
  #define SHARED_START_SEC_CONST_16
#elif defined (OS_STOP_SEC_CONST_16)
  #undef      OS_STOP_SEC_CONST_16
  #define SHARED_STOP_SEC_CONST_16

/* OS Const (ROM): 32 bit values */
#elif defined (OS_START_SEC_CONST_32)
  #undef      OS_START_SEC_CONST_32
  #define SHARED_START_SEC_CONST_32
#elif defined (OS_STOP_SEC_CONST_32)
  #undef      OS_STOP_SEC_CONST_32
  #define SHARED_STOP_SEC_CONST_32

/* OS Const (ROM): Often accessed */
#elif defined (OS_START_SEC_CONST_FAST_UNSPECIFIED)
  #undef      OS_START_SEC_CONST_FAST_UNSPECIFIED
  #define SHARED_START_SEC_CONST_UNSPECIFIED
#elif defined (OS_STOP_SEC_CONST_FAST_UNSPECIFIED)
  #undef      OS_STOP_SEC_CONST_FAST_UNSPECIFIED
  #define SHARED_STOP_SEC_CONST_UNSPECIFIED
/* OS Const (ROM): unspecified size */
#elif defined (OS_START_SEC_CONST_UNSPECIFIED)
  #undef      OS_START_SEC_CONST_UNSPECIFIED
  #define SHARED_START_SEC_CONST_UNSPECIFIED
#elif defined (OS_STOP_SEC_CONST_UNSPECIFIED)
  #undef      OS_STOP_SEC_CONST_UNSPECIFIED
  #define SHARED_STOP_SEC_CONST_UNSPECIFIED

/* OS Code */
#elif defined (OS_START_SEC_CODE)
  #undef      OS_START_SEC_CODE
  #define SHARED_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE)
  #undef      OS_STOP_SEC_CODE
  #define SHARED_STOP_SEC_CODE

/* OS Fast Code */
#elif defined (OS_START_SEC_CODE_FAST)
  #undef      OS_START_SEC_CODE_FAST
  #define SHARED_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_FAST)
  #undef      OS_STOP_SEC_CODE_FAST
  #define SHARED_STOP_SEC_CODE

/* OS Slow Code */
#elif defined (OS_START_SEC_CODE_SLOW)
  #undef      OS_START_SEC_CODE_SLOW
  #define SHARED_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_SLOW)
  #undef      OS_STOP_SEC_CODE_SLOW
  #define SHARED_STOP_SEC_CODE

/* OS Library Code */
#elif defined (OS_START_SEC_CODE_LIB)
  #undef      OS_START_SEC_CODE_LIB
  #define SHARED_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_LIB)
  #undef      OS_STOP_SEC_CODE_LIB
  #define SHARED_STOP_SEC_CODE

/* Callouts from the OS. */
#elif defined (OS_START_SEC_CALLOUT_CODE)
  #undef      OS_START_SEC_CALLOUT_CODE
  #define SHARED_START_SEC_CODE
#elif defined (OS_STOP_SEC_CALLOUT_CODE)
  #undef      OS_STOP_SEC_CALLOUT_CODE
  #define SHARED_STOP_SEC_CODE

/* OS code called by core 0 */
#elif defined (OS_START_SEC_CODE_CORE0)
  #undef      OS_START_SEC_CODE_CORE0
  #define CPU0_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE0)
  #undef      OS_STOP_SEC_CODE_CORE0
  #define CPU0_STOP_SEC_CODE

/* OS code called by core 1 */
#elif defined (OS_START_SEC_CODE_CORE1)
  #undef      OS_START_SEC_CODE_CORE1
  #define CPU1_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE1)
  #undef      OS_STOP_SEC_CODE_CORE1
  #define CPU1_STOP_SEC_CODE

/* OS code called by core 2 */
#elif defined (OS_START_SEC_CODE_CORE2)
  #undef      OS_START_SEC_CODE_CORE2
  #define CPU2_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE2)
  #undef      OS_STOP_SEC_CODE_CORE2
  #define CPU2_STOP_SEC_CODE

/* OS code called by core 3 */
#elif defined (OS_START_SEC_CODE_CORE3)
  #undef      OS_START_SEC_CODE_CORE3
  #define CPU3_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE3)
  #undef      OS_STOP_SEC_CODE_CORE3
  #define CPU3_STOP_SEC_CODE

/* OS code called by core 4 */
#elif defined (OS_START_SEC_CODE_CORE4)
  #undef      OS_START_SEC_CODE_CORE4
  #define CPU4_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE4)
  #undef      OS_STOP_SEC_CODE_CORE4
  #define CPU4_STOP_SEC_CODE

/* OS code called by core 4 */
#elif defined (OS_START_SEC_CODE_CORE5)
  #undef      OS_START_SEC_CODE_CORE5
  #define CPU5_START_SEC_CODE
#elif defined (OS_STOP_SEC_CODE_CORE5)
  #undef      OS_STOP_SEC_CODE_CORE5
  #define CPU5_STOP_SEC_CODE

#else
  #error "Os_MemMap.h, the section is undefined"

#endif

#include "MemMap.h"
