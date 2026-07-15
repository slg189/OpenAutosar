/*
 * EcnM_integration.c
 *
 *  Created on: May 15, 2019
 *      Author: AGT1HC
 */


#include "EcuM_integration.h"
#include "Compiler.h"
#include "Ifx_Ssw_Infra.h"
#include "Platform_Types.h"
#include "VectorTable_Core0.h"
#include "VectorTable_Core1.h"
#include "VectorTable_Core2.h"
extern FUNC(void, ECUM_CODE) EcuM_MainFunction(void);
extern unsigned short Ifx_Ssw_getCpuWatchdogPasswordInline(Ifx_SCU_WDTCPU *watchdog);
extern unsigned short Ifx_Ssw_getSafetyWatchdogPasswordInline(void);
extern void Ifx_Ssw_clearCpuEndinitInline(Ifx_SCU_WDTCPU *watchdog, unsigned short password);

#define CPU0_START_SEC_CODE
#include "MemMap.h"
/**********************************************************************************
   Function name    :  Loop_EcuM_MainFunc
   Description      :   call Loop_EcuM_MainFunc in Shutdown sequence due to RTE was disable. And Core0 have
   	   	   	   	   	   	   to wait for Slave cores shutdown.
   Parameter  (in)  :
   Parameter  (inout)  :  None
   Parameter  (out)  :  None
   Return value    :  None
   Remarks      :
 ***********************************************************************************/
FUNC( void, ECUM_CALLOUT_CODE ) Loop_EcuM_MainFunc(void)
{
	uint8 timeout = 0;
	while (timeout < ECUM_MAINFUNCTION_TIMEOUT)
	{
		EcuM_MainFunction();
		timeout++;

	}
}
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"

#define SHARED_START_SEC_CODE
#include "MemMap.h"
/**********************************************************************************
  Function name    :  VectorTable_Init
  Description    :  Wrapper for Os_InitializeVectorTable
  Parameter  (in)  :  None
  Parameter  (inout)  :  None
  Parameter  (out)  :  None
  Return value    :  None
  Remarks      :
***********************************************************************************/
FUNC(void, INIT_CODE) VectorTable_Init(sint8 coreID)
{
  /* Update safety and cpu/safety watchdog reload values */
  /* Password value is read again, because there is chance that local variables may be overridden. */
  unsigned short cpuWdtPassword    = Ifx_Ssw_getCpuWatchdogPasswordInline(&MODULE_SCU.WDTCPU[coreID]);
  unsigned short safetyWdtPassword = Ifx_Ssw_getSafetyWatchdogPasswordInline();
  Ifx_Ssw_clearCpuEndinitInline(&MODULE_SCU.WDTCPU[coreID], cpuWdtPassword);

	Os_InitializeVectorTable();
  
  Ifx_Ssw_setCpuEndinitInline(&MODULE_SCU.WDTCPU[coreID], cpuWdtPassword);
	return;
}
#define SHARED_STOP_SEC_CODE
#include "MemMap.h"

#define CPU0_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core0_Init(void)
{
	VectorTable_Init(0);
}
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"

#define CPU1_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core1_Init(void)
{
	VectorTable_Init(1);
}
#define CPU1_STOP_SEC_CODE
#include "MemMap.h"

#define CPU2_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core2_Init(void)
{
	VectorTable_Init(2);
}
#define CPU2_STOP_SEC_CODE
#include "MemMap.h"

#define CPU3_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core3_Init(void)
{
	VectorTable_Init(3);
}
#define CPU3_STOP_SEC_CODE
#include "MemMap.h"

#define CPU4_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core4_Init(void)
{
	VectorTable_Init(4);
}
#define CPU4_STOP_SEC_CODE
#include "MemMap.h"

#define CPU5_START_SEC_CODE
#include "MemMap.h"
FUNC(void, INIT_CODE) VectorTable_Core5_Init(void)
{
	VectorTable_Init(5);
}
#define CPU5_STOP_SEC_CODE
#include "MemMap.h"

