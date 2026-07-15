/*
 **********************************************************************************************************************
 *
 * COPYRIGHT RESERVED, ETAS GmbH, 2017. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 * Component 	: ProtectionHook.c
 * Created on	: Sep 23, 2019
 * Version   	: 1.0
 * Description  : This module provides ProtectionHook for Os
 * Author		 AGT1HC
 **********************************************************************************************************************
 */
/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Os.h"
#include "Ifx_reg.h"
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/

/*
 **********************************************************************************************************************
 * Function Definitions
 **********************************************************************************************************************
 */
#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h" /* [MISRA 2012 Dir 4.10] */ /*lint !e537 !e451 */
FUNC(ProtectionReturnType, OS_PROTECTIONHOOK_CODE) ProtectionHook(StatusType FatalError) {
  ISRType ISRInError;
  TaskType TaskInError;
  ProtectionReturnType action = PRO_SHUTDOWN;

  /*
   * This code sample shows how to determine which Task or
   * ISR was running when the fault occurred.
   */
  ISRInError = GetISRID();
  if (ISRInError != INVALID_ISR) {
    /* The fault occurred when ISR 'ISRInError' was running */
  } else {
    (void)GetTaskID(&TaskInError);
    if (TaskInError != INVALID_TASK) {
      /* The fault occurred when Task 'TaskInError' was running */
    } else {
      /* The fault occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
    }
  }

  switch (FatalError) {
    case E_OS_PROTECTION_MEMORY:
      /* A memory protection error has been detected */
      action = PRO_SHUTDOWN;
      break;

    case E_OS_PROTECTION_TIME:
      /* Task exceeds its execution time budget OR A Category 2 ISR exceeds its execution time */
      action = PRO_TERMINATETASKISR;
      break;

    case E_OS_PROTECTION_ARRIVAL:
      /* Task/Category 2 arrives before its timeframe has expired */
      action = PRO_IGNORE; /* This is the only place where PRO_IGNORE is legal */
      break;

    case E_OS_PROTECTION_LOCKED:
      /* Task/Category 2 ISR blocks for too long */
      action = PRO_TERMINATEAPPL;
      break;

    case E_OS_PROTECTION_EXCEPTION:
      /* Trap occurred */
      action = PRO_SHUTDOWN;
      break;

    default:
      /* Never here */
      break;
  }
  return action;
}
/* [MISRA 2012 Rule 20.1] */ /*lint -save -estring(9019, *) */
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h" /* [MISRA 2012 Dir 4.10] */ /*lint !e537 !e451 */
/*lint -restore */
/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
