/*
 * Contains code for the example application
 *
 * Copyright ETAS GmbH 2016
 */

 /*includes*/
#include <Os.h>
#include "Std_Types.h"
#include "Rte_UserCfg.h"
#include "Port.h"
#include "IfxPort_reg.h"
#include "IfxStm_reg.h"
#include "IfxSrc_reg.h"
#include "EcuM.h"
#include "BswM.h"
#include "SchM_BswM.h"
#include "main.h"
#include "Wdg_17_Scu_Cbk.h"
#include "IfxScu_reg.h"
#include "IfxCpu_reg.h"
#include "Ifx_Ssw_Infra.h"


/*Macro definitions*/
#define STM_CMP0 STM0_CMP0
#define STM_TIM0 STM0_TIM0
#define STM_SRC0 SRC_STM0SR0
#define CLEAR_PENDING_INTERRUPT() { STM_CMP0.U = STM_CMP0.U + TIMER_MILLISECOND; /* Increment from timer so that breakpoints won't cause an issue in samples */  STM_SRC0.B.CLRR = 1; }
#define TIMER_MILLISECOND (OSSWTICKSPERSECOND/1000UL)

/********************/
/* The main program */
/********************/
#define SHARED_START_SEC_CODE
#include "MemMap.h"
/*MISRA Dir-1.1, Rule 1.2, 10.4, 2.2, 8.5, 10.1 VIOLATION: This is a OS macro, This is a OS function*/
OS_MAIN()
{

	EcuM_Init();
	
	return;
}
#define SHARED_STOP_SEC_CODE
#include "MemMap.h"


/*Periodic interrupt definition*/
#define CPU0_START_SEC_CODE
#include "MemMap.h"
void TargetEn_PeriodicInterrupt(void)
{
	/* Initialize and enable STM CMP0 as the periodic interrupt source. */

	/* Configure 32 bit compares on the lowest 32 bits of the STM */
	STM0_CMCON.B.MSIZE0 = 31U;
	STM0_CMCON.B.MSTART0 = 0U;

	/* Set compare register to period value */
	// STM0_CMP0.U = STM0_TIM0.U + TIMER_MILLISECOND;

	/* Reset the interrupt pending flag */
	STM0_ISCR.B.CMP0IRR = 1;

	/* Enable compare interrupt */
	STM0_ICR.B.CMP0EN = 1;

	/* Enable suspend for system timer */
	/* Uncomment the below line of code when debugging */
	STM0_OCS.U = 0x12000000;
}

// #ifdef __GNUC__
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"
/******************************************************************/

#define CPU0_START_SEC_VAR_INIT_8
#include "MemMap.h"
char MyCounterIsRunning = FALSE;
#define CPU0_STOP_SEC_VAR_INIT_8
#include "MemMap.h"

#define CPU0_START_SEC_VAR_INIT_32
#include "MemMap.h"
uint32 OS_Counter_1ms = 0;
TickType Match_old = 0;
TickType Match_new = 0;
#define CPU0_STOP_SEC_VAR_INIT_32
#include "MemMap.h"

#define SHARED_START_SEC_CODE
#include "MemMap.h"
/* Default Interrupt handler for unknown IR source */
CAT1_ISR(DefaultInterruptHandler)
{
	for (;;)
	{
	/* Loop forever */
	}
}
#define SHARED_STOP_SEC_CODE
#include "MemMap.h"

 
// #define CPU0_START_SEC_VAR_INIT_32
// #include "MemMap.h"
// uint32 TIM0_Value = 0;
// uint32 CMP0_Value = 0;
// #define CPU0_STOP_SEC_VAR_INIT_32
// #include "MemMap.h" 
 


#define CPU0_START_SEC_CODE
#include "MemMap.h"
ISR(Millisecond)
{
	Os_AdvanceCounter_Rte_TickCounter();
	STM_SRC0.B.CLRR = 1;
	OS_Counter_1ms++;
}

FUNC(TickType, OS_CALLOUT_CODE) Os_Cbk_Now_Rte_TickCounter(void)
{
	return (*(Os_const_counters[1].dynamic)).type_dependent.hw.match;
}

FUNC(void, OS_CALLOUT_CODE) Os_Cbk_Set_Rte_TickCounter(TickType Match)
{
	TickType Detal_Match;
	Match_old = Match_new;
	Match_new = Match;

	if(Match_new < Match_old)
	{
		Detal_Match = 0xffff - (Match_old -Match_new) + 	1;
	}
	else
	{
		Detal_Match=(Match_new-Match_old);
	}
	if (MyCounterIsRunning == TRUE)
	{
		STM_CMP0.U += (Ifx_UReg_32Bit)(Detal_Match*TIMER_MILLISECOND);
	}
	else
	{
		STM_CMP0.U = STM0_TIM0.U + (Ifx_UReg_32Bit)(Detal_Match*TIMER_MILLISECOND);
	}
	// TIM0_Value = STM0_TIM0.U;
	// CMP0_Value = STM0_CMP0.U;
	MyCounterIsRunning = TRUE;
}
FUNC(void, OS_CALLOUT_CODE) Os_Cbk_State_Rte_TickCounter(Os_CounterStatusRefType State)
{
	State->Delay = (STM_CMP0.U - STM0_TIM0.U)/TIMER_MILLISECOND;
	State->Running = MyCounterIsRunning;
	if ((*(Os_const_counters[0].dynamic)).type_dependent.hw.running) 
	{
		State->Pending = TRUE;
	} 
	else {
		State->Pending = FALSE;
	}
}
FUNC(void, OS_CALLOUT_CODE) Os_Cbk_Cancel_Rte_TickCounter(void)
{
	MyCounterIsRunning = FALSE;

}
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"
