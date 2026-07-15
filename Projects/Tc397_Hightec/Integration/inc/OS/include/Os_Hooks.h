/*
 * Os_Hooks.h
 *
 *  Created on: Jul 23, 2019
 *      Author: AGT1HC
 */

#ifndef OS_HOOKS_H
#define OS_HOOKS_H

#include "Std_Types.h"
#include "Os.h"
// #include "Gpt.h"
#include "Ifx_reg.h"
#include "Rte_Const.h"
#include "ASW_CORE0.h"
#include "ASW_CORE1.h"
#include "ASW_CORE2.h"
#include "ASW_CORE3.h"
#include "ASW_CORE4.h"
#include "ASW_CORE5.h"

#define TIMER_RTE_TICK  OSCYCLESPERSECOND /* system tick per second */\
		                  /1000000 /* system tick per us */\
						  * RTE_PERIODIC_COUNTER_TICK_INTERVAL_US /* system tick per RTE counter tick */\
		                  /2 /* PIT tick per RTE counter tick, because fPIT = fsys / 2*/\

#define ERR_LOG_MAX 16

#define  REQUESTFAULTREACTION(x)      	{ if(GetCoreID() == 0) RE_Core0FaultReaction(x); \
											else if(GetCoreID() == 1) 	RE_Core1FaultReaction(x); \
											else if(GetCoreID() == 2) 	RE_Core2FaultReaction(x); \
											else if(GetCoreID() == 3) 	RE_Core3FaultReaction(x); \
											else if(GetCoreID() == 4) 	RE_Core4FaultReaction(x); \
											else RE_Core5FaultReaction(x);} 


typedef struct Os_ErrorLog{
	uint32 errCount;
	StatusType errType;
	ISRType	isr;
	TaskType task;
}OsTrace_ErrorHookLog_t;

#define OS_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "Os_MemMap.h"
OsTrace_ErrorHookLog_t 		err_log[ERR_LOG_MAX];
#define OS_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "Os_MemMap.h"


#endif /* OS_HOOKS_H_ */
