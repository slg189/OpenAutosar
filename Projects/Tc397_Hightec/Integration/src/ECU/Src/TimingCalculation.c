/*
 **********************************************************************************************************************
 *
 * COPYRIGHT RESERVED, ETAS GmbH, 2017. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 * Component 	: TimingCalculation.c
 * Created on	: Jan 7, 2020
 * Version   	: 1.0
 * Description  : This module provides APIs to calculate the elapsed time based on software counter.
 * Author		: HAD1HC
 **********************************************************************************************************************
    This file contains sample code only. It is not part of the production code deliverables.

*/

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "TimingCalculation.h"
#if (OS_COUNTER_IS_USED == FALSE)
#include "Ifx_reg.h"
#endif
/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/
#define SHARED_START_SEC_VAR_INIT_32
#include "MemMap.h"
static uint32 CurrentTime = 0;
#define SHARED_STOP_SEC_VAR_INIT_32
#include "MemMap.h"

#define SHARED_START_SEC_CODE
#include "MemMap.h"
/**********************************************************************************
  Function name		:	IC_SetCurrentTime
  Description		:	Set the current time for timing calculation.
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/
FUNC(void, OS_INTEGRATION_CODE) IC_SetCurrentTime(void)
{
#if (OS_COUNTER_IS_USED == TRUE)
  GetCounterValue((CounterType)&(Os_const_counters[Counter_Index]),(TickRefType)&CurrentTime);
#else
  CurrentTime = (uint32)STM0_TIM0.U;
#endif
}


/**********************************************************************************
  Function name		:	IC_GetElapsedTime
  Description		:	Get elapsed time from base time.
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks			:   None.
***********************************************************************************/
#if (OS_COUNTER_IS_USED == TRUE)
FUNC(uint32, OS_INTEGRATION_CODE) IC_GetElapsedTime(void)
#else
FUNC(float32, OS_INTEGRATION_CODE) IC_GetElapsedTime(void)
#endif 
{
#if (OS_COUNTER_IS_USED == TRUE)
	uint32 ElapsedTime;
#else
  volatile uint32 currentTick;
  float32 ElapsedTime;
#endif  

#if (OS_COUNTER_IS_USED == TRUE)
	GetElapsedCounterValue((CounterType)&(Os_const_counters[Counter_Index]),(TickRefType)&CurrentTime,(TickRefType)&ElapsedTime);
#else
  currentTick =  (uint32)STM0_TIM0.U;
  if (currentTick >= CurrentTime)
  {
    ElapsedTime = (float32)(currentTick - CurrentTime)/TICK_PER_MS;
  }
  else{
    ElapsedTime = (float32)(0xFFFFFFFF - CurrentTime + currentTick + 1)/TICK_PER_MS;
  }  
#endif

	return ElapsedTime;
}

#define SHARED_STOP_SEC_CODE
#include "MemMap.h"

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */

