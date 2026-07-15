/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_WDG
 * Description: Testcode for ASW_WDG
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        14-Jun-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        08-Apr-2021        Update test code for deadline
 *                                                  supervised entity
 * 1.3             HAD1HC        13-Apr-2021        Update the Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_WDG.h"
#include "ASW_WDG.h"
#include "WdgIf_Types.h"
#include "WdgIf_Cfg.h"
#include "WdgM.h"

#define ASW_WDG_START_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_WDG_MemMap.h"
Wdg_Test_t Wdg_TestCase_Local = WDG_NONE;
#define ASW_WDG_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_WDG_MemMap.h"

#define ASW_WDG_START_SEC_VAR_INIT_8
#include "ASW_WDG_MemMap.h"
uint8 start_Deadline = 0;
uint8 wait10ms = 0;
#define ASW_WDG_STOP_SEC_VAR_INIT_8
#include "ASW_WDG_MemMap.h"

#define ASW_WDG_START_SEC_VAR_INIT_32
#include "ASW_WDG_MemMap.h"
uint32  WdgDeadlineTimeBase=0;
#define ASW_WDG_STOP_SEC_VAR_INIT_32
#include "ASW_WDG_MemMap.h"

#define ASW_WDG_START_SEC_CODE
#include "ASW_WDG_MemMap.h"
FUNC (void, ASW_WDG_CODE) RE_ASW_WDG_Deadline_Supervision_Entity1_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType syncCall;
	
	wait10ms++;
	if (start_Deadline == 0)
	{
		syncCall = Rte_Call_RPort_Deadline_Supervision_Entity1_CheckpointReached();
		start_Deadline = 1;	
		wait10ms = 0;
	}

	switch (Wdg_TestCase_Local)
	{
	case DEADLINE_OK_ONTIME_CP_WDG_DEADLINE_ENTITY1:
		if ((start_Deadline == 1) && (wait10ms == 1))
		{
			syncCall = Rte_Call_RPort_Deadline_Supervision_Entity2_CheckpointReached();
			start_Deadline = 0;
			Wdg_TestCase_Local=WDG_NONE;
		}
		break;
	case DEADLINE_NOTOK_LATE_CP_WDG_DEADLINE_ENTITY1:
		if ((start_Deadline == 1) && (wait10ms == 2))
		{
			syncCall = Rte_Call_RPort_Deadline_Supervision_Entity2_CheckpointReached();
			start_Deadline = 0;
			Wdg_TestCase_Local=WDG_NONE;
		}
		break;
	default:
		if ((start_Deadline == 1) && (wait10ms == 1))
		{
			syncCall = Rte_Call_RPort_Deadline_Supervision_Entity2_CheckpointReached();
			start_Deadline = 0;
		}
		break;
	}
}
#define ASW_WDG_STOP_SEC_CODE
#include "ASW_WDG_MemMap.h"
#define ASW_WDG_START_SEC_CODE
#include "ASW_WDG_MemMap.h"

FUNC (void, ASW_WDG_CODE) RE_ASW_WDG_Alive_Supervision_Entity1_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType syncCall1;
	static uint16 Entity1_Count;
	switch (Wdg_TestCase_Local)
	{
		case ALIVE_OK_LACKOF_ONE_CP_ALIVE_SUPERVISION_ENTITY1:
			Wdg_TestCase_Local=WDG_NONE;
			break;

		case ALIVE_OK_LACKOF_TWO_ALIVE_SUPERVISION_ENTITY1:
			/*lack of 2 CP*/
			if(Entity1_Count>0)
			{
				Wdg_TestCase_Local=WDG_NONE;
				Entity1_Count=0;
			}
			else
			{
				Entity1_Count++;
			}
			break;
		case ALIVE_NOTOK_LACKOF_THREE_ALIVE_SUPERVISION_ENTITY1:
			/*lack of 3 CPs*/
			if(Entity1_Count>1)
			{
				Wdg_TestCase_Local=WDG_NONE;
				Entity1_Count=0;
			}
			else
			{
				Entity1_Count++;
			}
			break;

		case ALIVE_NOTOK_ADDMORE_ALIVE_SUPERVISION_ENTITY1:
			syncCall1 = Rte_Call_RPort_Alive_Supervision_Entity1_CheckpointReached();
			/*add more checkpoint*/
			syncCall1 = Rte_Call_RPort_Alive_Supervision_Entity1_CheckpointReached();
			Wdg_TestCase_Local=WDG_NONE;
			break;
		case FAST_MODE:
			WdgM_SetMode(WDGIF_FAST_MODE);
			Wdg_TestCase_Local=WDG_NONE;
			syncCall1 = Rte_Call_RPort_Alive_Supervision_Entity1_CheckpointReached();
			break;
		case SLOW_MODE:
			WdgM_SetMode(WDGIF_SLOW_MODE);
			Wdg_TestCase_Local=WDG_NONE;
			syncCall1 = Rte_Call_RPort_Alive_Supervision_Entity1_CheckpointReached();
			break;			
		default:
			syncCall1 = Rte_Call_RPort_Alive_Supervision_Entity1_CheckpointReached();
			break;
	}
}
#define ASW_WDG_STOP_SEC_CODE
#include "ASW_WDG_MemMap.h"
#define ASW_WDG_START_SEC_CODE
#include "ASW_WDG_MemMap.h"
/***************************************************************************************************
  Function name		:	WdgTest_GetTime
  Description		:	Calculate the current timing from base time.
  Parameter	(in)	:	baseTime
  Parameter	(inout)	:	None
  Parameter	(out)	:	None
  Return value		:	uint32 (ms)
  Remarks: This function will be included only when PLATFORM_TEST_WDGM_BUILD = ON.
           This function is not intended to be inline so that to be as less memory interferences as
		   the code for test.
***************************************************************************************************/
FUNC(uint32, WdgInternalTest_CODE)  WdgTest_GetTime(uint32 baseTime)
{
	uint32 CurrentStmValue;
	uint32 time=0u;

	CurrentStmValue = OS_Counter_1ms;

	if(CurrentStmValue > baseTime)
	{
		time = CurrentStmValue - baseTime;
	}
	else if(CurrentStmValue < baseTime)
	{
		time = 0xFFFFFFFF - baseTime + CurrentStmValue;
	}
	return time;
}
#define ASW_WDG_STOP_SEC_CODE
#include "ASW_WDG_MemMap.h"
#define ASW_WDG_START_SEC_CODE
#include "ASW_WDG_MemMap.h"
/***************************************************************************************************
  Function name		:	WdgTest_SetBaseTime
  Description		:	Get base timing form OS_Counter_1ms.
  Parameter	(in)	:	None
  Parameter	(inout)	:	None
  Parameter	(out)	:	baseTime
  Return value		:	None
  Remarks: This function will be included only when PLATFORM_TEST_WDGM_BUILD = ON.
           This function is not intended to be inline so that to be as less memory interferences as
		   the code for test.
***************************************************************************************************/
FUNC(void, WdgInternalTest_CODE)  WdgTest_SetBaseTime(uint32 * baseTime)
{
	*baseTime = OS_Counter_1ms;
}
#define ASW_WDG_STOP_SEC_CODE
#include "ASW_WDG_MemMap.h"
