/***************************************************************************************************
 * Component : ECU_Startup.c
 * Date      : Mar 15 2019
 * Version   : 1.0
 * Description  : This module implements ECU startup sequences of TC397
 *
 **************************************************************************************************/

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#define RTE_CORE

#include "Rte_Const.h"
#if !defined(RTE_VENDOR_MODE)
#error "Compiling an individual task file but RTE_VENDOR_MODE not defined. Compiling a stale file?"
#endif /* !defined(RTE_VENDOR_MODE) */
#if defined(RTE_USE_TASK_HEADER)
#include "ECU_StartupTask.h"
#else /* defined(RTE_USE_TASK_HEADER) */
#include "Os.h"
#endif /* defined(RTE_USE_TASK_HEADER) */
#if defined(RTE_REQUIRES_IOC)
#include "Ioc.h"
#endif /* defined(RTE_REQUIRES_IOC) */
#include "Rte.h"
#include "Rte_Intl.h"
#include "Rte_Type.h"
#include "Rte_DataHandleType.h"
/*==================================================================================================
*                                        LOCAL MACROS
==================================================================================================*/
/*==================================================================================================
*                                      FILE VERSION CHECKS
==================================================================================================*/
/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
*                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
extern void BswM_MainFunction(void);
extern FUNC(void, RTE_CODE) SchM_IModeInit(void);
/* Runnable entity prototypes */
#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h" /*lint !e537 permit multiple inclusion */
FUNC(void, ECUM_CODE) EcuM_StartupTwo(void);
#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h" /*lint !e537 permit multiple inclusion */
/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/* Function name :
* Description   :
* Parameter     :   None
* Return value  :   None
* Remarks       :
*/
#define RTE_START_SEC_EcucPartition_Core0_CODE
#include "Rte_MemMap.h"
TASK(Core0_ECU_StartupTask)
{
   /* Box: Implicit Buffer Initialization begin */
   /* Box: Implicit Buffer Initialization end */
   /* Box: Implicit Buffer Fill begin */
   /* Box: Implicit Buffer Fill end */
   {
      /* static char cTest[10] = {0}; */
      /* Box: BSWImpl3_EcuM begin */
      EcuM_StartupTwo();
      /* Box: BSWImpl3_EcuM end */

      /* ETAS: SchM was initialized before BswM,
       * so mode initialization didn't trigger the BswM_RequestMode
       * Need to call it again
       */
      SchM_IModeInit();

      /* ETAS: BswM_MainFunction is scheduled with RTE,
       * Need to call BswM_MainFunction to trigger the start machine execution
       */
      BswM_MainFunction();
   }
   /* Box: Implicit Buffer Flush begin */
   /* Box: Implicit Buffer Flush end */
   TerminateTask();
} /* ECU_StartupTask */
#define RTE_STOP_SEC_EcucPartition_Core0_CODE
#include "Rte_MemMap.h"
