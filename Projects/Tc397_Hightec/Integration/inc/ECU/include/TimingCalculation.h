/*
 * TimingCalculation.h
 *
 *  Created on: Jan 7, 2020
 *      Author: HAD1HC
 */

#include "Std_Types.h"
#include "Os.h"

#ifndef INTEGRATION_ECU_INCLUDE_TIMINGCALCULATION_H_
#define INTEGRATION_ECU_INCLUDE_TIMINGCALCULATION_H_

#define OS_COUNTER_IS_USED FALSE

#define Counter_Index    1

#if (OS_COUNTER_IS_USED == FALSE)
#define TICK_PER_MS 100000
#endif

extern FUNC(void, OS_INTEGRATION_CODE) IC_SetCurrentTime(void);
#if (OS_COUNTER_IS_USED == TRUE)
extern FUNC(uint32, OS_INTEGRATION_CODE) IC_GetElapsedTime(void);
#else
extern FUNC(float32, OS_INTEGRATION_CODE) IC_GetElapsedTime(void);
#endif

#endif /* INTEGRATION_OS_INCLUDE_OS_CPUUTILIZATION_H_ */
