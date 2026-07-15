/*
 **********************************************************************************************************************
 *
 * COPYRIGHT RESERVED, ETAS GmbH, 2017. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 * Component 	: Os_Cbk_GetStopwatch.c
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

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h" 
/**********************************************************************************
  Function name		:	Os_Cbk_GetStopwatch
  Description		:	This is used to measure execution times of tasks and ISRs.
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	Current tick value of the stop watch tick timer.
  Remarks			:   Os_Cbk_GetStopwatch() implements if OS option ’Enable Time
                        Monitoring’ is enabled.
***********************************************************************************/
FUNC(Os_StopwatchTickType, OS_CALLOUT_CODE) Os_Cbk_GetStopwatch(void)
{
  /* Get current value of 32 bit timer */
  /* Note: If this function is called from within untrusted code
   * then ensure that untrusted access is allowed */
  //return(STM_0_CNT);
	return (Os_StopwatchTickType) STM0_TIM0.U;

}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"  


