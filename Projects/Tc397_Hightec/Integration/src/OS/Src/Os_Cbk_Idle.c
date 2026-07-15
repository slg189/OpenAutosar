/***************************************************************************************************
 * Component : Os_Cbk_Idle.c
 * Date      : Sep 23, 2019
 * Version   : 1.0
 * Description: This module implements the Os_Cbk_Idle
 * Author	: AGT1HC
 *
 **************************************************************************************************/
/**************************************************************/
/* Idle Task  								                  */
/**************************************************************/
/*Test for return of maximum stack usage for each task/ISR */
#include "Std_Types.h"
#include "Os.h"
// #include "Gpt.h"
#include "Rte_Const.h"

#define OS_START_SEC_VAR_INIT_32
#include "Os_MemMap.h" 
volatile uint32 Os_Task_Idle_Counter = 0;
#define OS_STOP_SEC_VAR_INIT_32
#include "Os_MemMap.h" 

#define OS_START_SEC_CALLOUT_CODE
#include "Os_MemMap.h" 
FUNC(boolean, OS_CALLOUT_CODE) Os_Cbk_Idle(void)
{
	Os_Task_Idle_Counter++;    
	return TRUE;
}
#define OS_STOP_SEC_CALLOUT_CODE
#include "Os_MemMap.h" 

