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
    This file contains sample code only. It is not part of the production code deliverables.

*/
#include "EcuM.h"
#include "Rte_Main.h"
#include "Ifx_Ssw_Infra.h"
#include "Os_Hooks.h"
#include "main.h"
#include "Irq.h"

#define OS_START_SEC_VAR_INIT_32
#include "Os_MemMap.h"
static uint32 	   err_cnt = 0;
#define OS_STOP_SEC_VAR_INIT_32
#include "Os_MemMap.h"

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
/**********************************************************************************
  Function name		:	ErrorHook
  Description		:	Called during Error detected in ENTENDED Build.
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/
FUNC(void, OS_ERRORHOOK_CODE) ErrorHook(StatusType Error)
{
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;
	OsTrace_ErrorHookLog_t* err;

	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
		/* The error occurred when Task 'TaskInError' was running */
		}
		else
		{
		/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}
	
	err =  &err_log[err_cnt % (uint8)ERR_LOG_MAX];
	err->errCount = err_cnt;
	err->errType = Error;
	err->isr = ISRInError;
	err->task = TaskInError;
	err_cnt++;

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	case E_OS_LIMIT:
		/* Terminate. */
		REQUESTFAULTREACTION(E_OS_LIMIT);
		// ShutdownOS(Error);
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"

#define OS_START_SEC_CODE_CORE0
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core0(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;
	
	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE0
#include "Os_MemMap.h"

#define OS_START_SEC_CODE_CORE1
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core1(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;
	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE1
#include "Os_MemMap.h"


#define OS_START_SEC_CODE_CORE2
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core2(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;

	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE2
#include "Os_MemMap.h"

#define OS_START_SEC_CODE_CORE3
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core3(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;

	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE3
#include "Os_MemMap.h"

#define OS_START_SEC_CODE_CORE4
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core4(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;

	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE4
#include "Os_MemMap.h"

#define OS_START_SEC_CODE_CORE5
#include "Os_MemMap.h"
FUNC(void, OS_CALLOUT_CODE) ErrorHook_OsApplication_Core5(StatusType Error){
	ISRType ISRInError = 0;
	TaskType TaskInError = 0;
	StatusType Status_Type;

	/*
	* This code sample shows how to determine which Task or
	* ISR was running when the error occurred.
	*/
	ISRInError = GetISRID();
	if (ISRInError != INVALID_ISR)
	{
		/* The error occurred when ISR 'ISRInError' was running */
	} 
	else
	{
		Status_Type = GetTaskID(&TaskInError);
		if (TaskInError != INVALID_TASK)
		{
			/* The error occurred when Task 'TaskInError' was running */
		} 
		else
		{
			/* The error occurred during StartOS, Os_Cbk_Idle or ShutdownOS */
		}
	}

	/*
	 * This code sample shows how to determine which error
	 * was detected.
	 */
	switch (Error){
	case E_OS_ID:
		/* Handle illegal identifier error */
		break;
	case E_OS_VALUE:
		/* Handle illegal value error */
		break;
	case E_OS_STATE:
		/* Handle illegal state error */
		break;
	default:
		/* Handle all other types of error */
		break;
	}
}
#define OS_STOP_SEC_CODE_CORE5
#include "Os_MemMap.h"

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
FUNC(void, OS_STARTUPHOOK_CODE) StartupHook(void)
{
	/* GTM register OCS sets the fiels SUS ad 2:
	 * 	Soft suspend (GTM Halt Mode). In Soft suspend registers could be read or written
	 *
	 * 	suspend the GTM counters during the debug phase
	 *
	 */
    unsigned short cpuWdtPassword;
    unsigned short safetyWdtPassword ;
    int coreID;
    coreID = GetCoreID();
    cpuWdtPassword    = Ifx_Ssw_getCpuWatchdogPasswordInline(&MODULE_SCU.WDTCPU[coreID]);
    safetyWdtPassword = Ifx_Ssw_getSafetyWatchdogPasswordInline();
    Ifx_Ssw_clearCpuEndinitInline(&MODULE_SCU.WDTCPU[coreID], cpuWdtPassword);
    
    if (coreID == 0)
	{
    	TargetEn_PeriodicInterrupt();
	}
    
    Ifx_Ssw_setCpuEndinitInline(&MODULE_SCU.WDTCPU[coreID], cpuWdtPassword);

}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
/**********************************************************************************
  Function name		:	ShutdownHook
  Description		:	Called during OS Shutdown by ShutdownOS.
  Parameter	(in)	:	Error - the error code passed which makes OS call ShutdownHook
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/
FUNC(void, OS_SHUTDOWNHOOK_CODE) ShutdownHook(StatusType Error)
{
    StatusType Status_Type;
    Status_Type = Error;
    EcuM_Shutdown();
}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
/**********************************************************************************
  Function name		:	Os_Cbk_CheckMemoryAccess
  Description		:	Check if a memory region is read/write/execute/stack
                        accessible by a specified OS-Application.
  Parameter	(in)	:	Application - The OS-Application to which the task or ISR belongs.
                        TaskID      - The task for which the memory access is being checked.
                        ISRID       - The ISR for which the memory access is being checked.
                        Address     - The start address of the memory area.
                        Size        - The size in bytes of the memory area.
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/

FUNC(AccessType, OS_OS_CBK_CHECKMEMORYACCESS_CODE) Os_Cbk_CheckMemoryAccess(ApplicationType Application, TaskType TaskID, ISRType ISRID, MemoryStartAddressType Address, MemorySizeType Size)
{
	return OS_ACCESS_EXECUTE;
}

#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"


#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
/**********************************************************************************
  Function name		:	Os_Cbk_StackOverrunHook
  Description		:	check the overun of stack
  Parameter	(in)	:	Overrun: infomtion of overun 
						Reason : the resone of overun
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/

FUNC(void, OS_CALLOUT_CODE) Os_Cbk_StackOverrunHook(Os_StackSizeType Overrun, Os_StackOverrunType Reason) 
{
	ISRType CurrentISRID;
	TaskType CurrentTaskID;
	uint8 Cnt;
	OsTrace_ErrorHookLog_t* err;

	/* Work out what has failed */
	CurrentISRID = GetISRID();

	if ( CurrentISRID != INVALID_ISR ) 
	{
		/* An ISR has overrun */
		/* Work out which ISR */
	} 
	else 
	{
		/* It must be a task that has overrun */
		GetTaskID(&CurrentTaskID);
		/* Work out which task */
	}
	
	err =  &err_log[err_cnt % (uint8)ERR_LOG_MAX];
	err->errCount = err_cnt;
	err->errType = E_OS_STACKFAULT;
	err->isr = CurrentISRID;
	err->task = CurrentTaskID;
	err_cnt++;

	/* Work out why */
	switch (Reason) {
	case OS_BUDGET:
		/* Problem: The task/ISR exceeded its stack allocation */
		REQUESTFAULTREACTION(E_OS_STACKFAULT);
	break;
		case OS_ECC_START:
		/* Problem: Some lower priority task on the stack has used
		too much stack space */
		/* Solution: Enable stack monitoring to find out which task */
		break;
	case OS_ECC_RESUME:
		/* Problem: Some lower priority task on the stack has used
		too much stack space */
		/* Solution: Enable stack monitoring to find out which task */
		break;
	case OS_ECC_WAIT:
		/* Problem: The extended task had consumed too much stack
		space then executing WaitEvent() */
		/* Solution: Add Overrun to the WaitEvent() stack allocation */
		break;
	}
}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h"
