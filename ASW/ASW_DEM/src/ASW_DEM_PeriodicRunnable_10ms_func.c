/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_DEM
 * Description: Testcode for ASW_DEM
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        04-Mar-2021        Initial version
 * 1.1             HAD1HC        13-Apr-2021        Update Memmap
 * 1.2             HAD1HC        11-June-2021       Add test case 10
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_DEM.h"
#include "ASW_DEM.h"
/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :ASW_DEM_PeriodicRunnable_10ms_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :ASW_DEM_PeriodicRunnable_10ms_func) ENABLED START */
/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :DemSwc_RE_Dem_SWC_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */

#define ASW_DEM_START_SEC_VAR_INIT_8
#include "ASW_DEM_MemMap.h"
uint8 Dem_Testcase = 0x0A, DemOperationCycleControl = 0;
uint8 isQualified = FALSE;
uint8 PfcQualified_Trigger = FALSE;
#define ASW_DEM_STOP_SEC_VAR_INIT_8
#include "ASW_DEM_MemMap.h"
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define ASW_DEM_START_SEC_CODE                   
#include "ASW_DEM_MemMap.h"

FUNC (void, ASW_DEM_CODE) ASW_DEM_PeriodicRunnable_10ms_func/* return value & FctID */
(
		void
)
{

	volatile static Dem_EventStatusType EventStatus1 = DEM_EVENT_STATUS_FAILED;
	Std_ReturnType syncCall1;
	Std_ReturnType syncCall2;
	Std_ReturnType syncCall3;
	Std_ReturnType syncCall4;
	Dem_DebounceResetStatusType DebounceResetStatus2;
	Std_ReturnType syncCall5;
	Dem_EventStatusType EventStatus3 = DEM_EVENT_STATUS_FAILED;
	Std_ReturnType syncCall6;
	Std_ReturnType syncCall7;
	Std_ReturnType syncCall8;
	Std_ReturnType syncCall9;
	Dem_DebounceResetStatusType DebounceResetStatus4;
	Std_ReturnType syncCall10;
	Dem_OperationCycleStateType CycleState5;
	Std_ReturnType syncCall11;
	Dem_OperationCycleStateType CycleState6;
	Std_ReturnType syncCall12;
	Dem_OperationCycleStateType CycleState7;
	Std_ReturnType syncCall13;
	Dem_OperationCycleStateType CycleState8;
	Std_ReturnType syncCall14;
	Dem_EventStatusType EventStatus9 = DEM_EVENT_STATUS_FAILED;
	Std_ReturnType syncCall15;
	Std_ReturnType syncCall16;
	Std_ReturnType syncCall17;
	Std_ReturnType syncCall18;
	Dem_DebounceResetStatusType DebounceResetStatus10;
	Std_ReturnType syncCall19;
	Std_ReturnType syncCall20;
	Dem_EventStatusType EventStatus11 = DEM_EVENT_STATUS_FAILED;
	Std_ReturnType syncCall21;
	Std_ReturnType syncCall22;
	Std_ReturnType syncCall23;
	Std_ReturnType syncCall24;
	Dem_DebounceResetStatusType DebounceResetStatus12;
	Std_ReturnType syncCall25;
	Std_ReturnType syncCall26;
	Std_ReturnType syncCall27;
	Std_ReturnType syncCall28;
	Std_ReturnType syncCall29;
	Std_ReturnType syncCall30;
	Std_ReturnType syncCall31;
	Std_ReturnType syncCall32;
	Std_ReturnType syncCall33;
	Std_ReturnType syncCall34;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DEM_PeriodicRunnable_10ms_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	if( DemOperationCycleControl == 0 )
    {
    	Rte_Call_RPort_OpCycle_DemOperationCycle_Power_SetOperationCycleState(DEM_CYCLE_STATE_START);
		DemOperationCycleControl = 255;
    }

    if( DemOperationCycleControl == 1 )
    {
    	Rte_Call_RPort_OpCycle_DemOperationCycle_Power_SetOperationCycleState(DEM_CYCLE_STATE_END);
		DemOperationCycleControl = 255;
    }

	switch (Dem_Testcase)
    {
    case 0x01:
        syncCall1 = Rte_Call_Event_DemEventParameter_0D01_SetEventStatus(EventStatus1);
        syncCall6 = Rte_Call_Event_DemEventParameter_0D02_SetEventStatus(EventStatus3);
        syncCall15 = Rte_Call_Event_DemEventParameter_0D06_Comb0_SetEventStatus(EventStatus9);
        syncCall21 = Rte_Call_Event_DemEventParameter_0D06_Comb1_SetEventStatus(EventStatus11);
		Dem_Testcase = 0;
        break;
    case 0x02:
        syncCall27 = Rte_Call_Event_DemEventParameter_0D01_SetEventStatus(DEM_EVENT_STATUS_PREPASSED);
		Dem_Testcase = 0;
        break;
    case 0x03:
        syncCall27 = Rte_Call_Event_DemEventParameter_0D01_SetEventStatus(DEM_EVENT_STATUS_PREFAILED);
		Dem_Testcase = 0;
        break;
    case 0x04:
        syncCall28 = Rte_Call_Event_DemEventParameter_0D02_SetEventStatus(DEM_EVENT_STATUS_PREFAILED);
        break;
    case 0x05:
        syncCall29 = Rte_Call_Event_DemEventParameter_0D02_SetEventStatus(DEM_EVENT_STATUS_PREPASSED);
        Dem_Testcase = 0;
        break;
    case 0x06:
        syncCall30 = Rte_Call_Event_DemEventParameter_0D06_Comb0_SetEventStatus(DEM_EVENT_STATUS_PREFAILED);
        Dem_Testcase = 0;
        break;
    case 0x07:
        syncCall31 = Rte_Call_Event_DemEventParameter_0D06_Comb0_SetEventStatus(DEM_EVENT_STATUS_PREPASSED);
        Dem_Testcase = 0;
        break;
    case 0x08:
        syncCall32 = Rte_Call_Event_DemEventParameter_0D06_Comb1_SetEventStatus(DEM_EVENT_STATUS_PREFAILED);
        Dem_Testcase = 0;
        break;
    case 0x09:
        syncCall33 = Rte_Call_Event_DemEventParameter_0D06_Comb1_SetEventStatus(DEM_EVENT_STATUS_PREPASSED);
        Dem_Testcase = 0;
        break;
	case 0x0A:
        syncCall34 = Rte_Call_Event_DemEventParameter_0D01_SetEventStatus(DEM_EVENT_STATUS_FAILED);
        Dem_Testcase = 0;
        break; 	    
    default:
        break;
    }

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DEM_PeriodicRunnable_10ms_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

#define ASW_DEM_STOP_SEC_CODE                       
#include "ASW_DEM_MemMap.h"
