/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  CDD_CORE4
 * Description: Testcode for CDD_CORE4
 * Version         Author        Date               Update information
 * 1.0             HAD1HC        01-Feb-2021        Create software
 * 1.1			   HAD1HC        19-Mar-2021        Update function of calculating
 * 													CPU Load and stack utilization
 * 1.2             HAD1HC		 24-May-2021	    Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_CDD_CORE4.h"
#include "CDD_CORE4.h"

#define CDD_CORE4_START_SEC_VAR_INIT_UNSPECIFIED
#include "CDD_CORE4_MemMap.h"
static enum StateCaculation_type StateCaculationCore4 = PREPARE;
#define CDD_CORE4_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "CDD_CORE4_MemMap.h"

#define CDD_CORE4_START_SEC_VAR_INIT_8
#include "CDD_CORE4_MemMap.h"
static uint8 Status_global = 0;
#define CDD_CORE4_STOP_SEC_VAR_INIT_8
#include "CDD_CORE4_MemMap.h"

#define CDD_CORE4_START_SEC_VAR_INIT_32
#include "CDD_CORE4_MemMap.h"
static uint32 Duration_global = 0;  
#define CDD_CORE4_STOP_SEC_VAR_INIT_32
#include "CDD_CORE4_MemMap.h"

#define CDD_CORE4_START_SEC_VAR_INIT_64
#include "CDD_CORE4_MemMap.h"
static float64 CPU_PercentUtilization_global = 0.0;
static float64 MaxUserStackUtilization_f64 = 0.0;
#define CDD_CORE4_STOP_SEC_VAR_INIT_64
#include "CDD_CORE4_MemMap.h"


#define CDD_CORE4_START_SEC_CODE
#include "CDD_CORE4_MemMap.h"
/**********************************************************************************
  Function name		:	CPUUtilizationCore4_func
  Description		:	Fucntion called from client to get the core 4 utilization.
  Parameter	(in)	:	Duration_u32: duration in which CPU load is measured 
  Parameter	(inout)	:	None
  Parameter	(out)	:	CPU_PercentUtilization: percent of CPU load over measured time
						Status: status of the process of calculating CPU load
  Return value		:	None
  Remarks: 
***********************************************************************************/
FUNC (void, CDD_CORE4_CODE) CPUUtilizationCore4_func/* return value & FctID */
(
		VAR(uint32, AUTOMATIC) Duration_u32,
		P2VAR(float64, AUTOMATIC, RTE_APPL_DATA) CPU_PercentUtilization,
		P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Status
)
{
	Duration_global = Duration_u32;
	*CPU_PercentUtilization = CPU_PercentUtilization_global;
	*Status = Status_global;
}
#define CDD_CORE4_STOP_SEC_CODE
#include "CDD_CORE4_MemMap.h"

#define CDD_CORE4_START_SEC_CODE
#include "CDD_CORE4_MemMap.h"
/**********************************************************************************
  Function name		:	GetMaxUserStackUtilization_Core4_func
  Description		:	Fucntion called from client to get the maximum stack utilization of core 4
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	MaxUserStackUtilization: maximum percent of used stack over 
  						configured stack
  Return value		:	Std_ReturnType - E_OK: Request accepted, E_NOT_OK: Request not accepted
  Remarks: 
***********************************************************************************/
FUNC (Std_ReturnType, AUTOMATIC) GetMaxUserStackUtilization_Core4_func/* return value & FctID */
(
		P2VAR(float64, AUTOMATIC, RTE_APPL_DATA) MaxUserStackUtilization
)
{

	Std_ReturnType retValue = E_OK;

	if ((MaxUserStackUtilization_f64 >= 0) && (MaxUserStackUtilization_f64 <= 100))
	{
		*MaxUserStackUtilization = MaxUserStackUtilization_f64;
	}
	else
	{
		retValue = E_NOT_OK;
	}

	return retValue;
}
#define CDD_CORE4_STOP_SEC_CODE
#include "CDD_CORE4_MemMap.h"

#define CDD_CORE4_START_SEC_CODE
#include "CDD_CORE4_MemMap.h"
/**********************************************************************************
  Function name		:	CDD_CORE4_func
  Description		:	Main function to process the calculation of Cpu load
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	None
  Remarks: 
***********************************************************************************/
FUNC (void, CDD_CORE4_CODE) CDD_CORE4_func/* return value & FctID */
(
		void
)
{
	static uint32 	TimeSet;
	static uint32	BaseTime;
	static uint32 	CurTime;
	static uint32 	DTime;
	static uint32 	IdleElapsed;

	uint32     *stack_pos;
	uint32     stack_start;
	uint8      empty_accu = 0;
	uint32 UStack_Size = 0;
	float64 UserStackUtilization_f64 = 0.0;

	switch (StateCaculationCore4)
	{
	case PREPARE:
		Status_global = RTE_E_CPU_Utilization_ON_PROGRESS;
		TimeSet = Duration_global*OSSWTICKSPERMILLISECOND;
		StateCaculationCore4 = START_CACULATION;
		break;
	case START_CACULATION:
		/*prepare set time for caculation */
		Os_ResetIdleElapsedTime(OS_CORE_ID_4);
		BaseTime = Os_Cbk_GetStopwatch();
		StateCaculationCore4=WAIT_CACULATION;
		break;
	case WAIT_CACULATION:
		/*check the time is match */
		CurTime = Os_Cbk_GetStopwatch();
		if(CurTime > BaseTime) DTime = CurTime - BaseTime;
		else DTime = 0xFFFFFFFF + CurTime - BaseTime;

		if(DTime>TimeSet)
		{
			IdleElapsed = Os_GetIdleElapsedTime(OS_CORE_ID_4);
			StateCaculationCore4 = FINISH_START_CACULATION;
		}
		break;
	case FINISH_START_CACULATION:
		/*caculation the CPU load after match time*/
		CPU_PercentUtilization_global = 100.0*(float)(DTime -IdleElapsed)/DTime;
		StateCaculationCore4 = PREPARE;
		Status_global = RTE_E_CPU_Utilization_E_OK;
		break;
	default:
		break;
	}

	UStack_Size = (uint32)&__USTACK4- (uint32)&__USTACK4_END;
	stack_start = (uint32)&__USTACK4;
	stack_pos = stack_start  -  STACK_STARTUP_SIZE;
	do
	{
		if(STACK_EMPTY_VALUE == *stack_pos)
		{
			empty_accu += 1U;
			stack_pos = stack_pos - STACK_EMPTY_STEP;
		}
		else
		{
			empty_accu = 0;
			stack_pos = stack_pos - STACK_EMPTY_GRANULARITY_U32;
		}
	}while(STACK_EMPTY_GRANULARITY_U32 != empty_accu );
	
	UserStackUtilization_f64 = (float64)(stack_start - (uint32)(stack_pos) - STACK_EMPTY_GRANULARITY_U32*4)/UStack_Size;
	if (UserStackUtilization_f64 > MaxUserStackUtilization_f64)
	{
		MaxUserStackUtilization_f64 = UserStackUtilization_f64;
	}
}
#define CDD_CORE4_STOP_SEC_CODE
#include "CDD_CORE4_MemMap.h"

