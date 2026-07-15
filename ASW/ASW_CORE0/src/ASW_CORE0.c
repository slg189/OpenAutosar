/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_CORE0
 * Description: Testcode for ASW_CORE0
 * Version         Author:       Date               Update information
 * 1.0             AGT1HC        12-Mar-2019        Create software
 * 1.1             AGT1HC        19-Jan-2021        Update software for new requirement OS_2_1
 * 1.2             HAD1HC        02-Feb-2021        Test code for transmission and reception 
 * 													data from other core
 * 1.3			   HAD1HC	     01-Mar-2021		Add function for fault reaction
 * 1.4			   HAD1HC	     15-Mar-2021		Update API call for stack utilization
 * 													Add function for handling IRV variable
 * 													Update function of getting CPU Load	
 * 1.5             HAD1HC	     13-Apr-2021	    Update Memmap				
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/
#define CPULOAD_MEASURE_TIME 1000u /* 1000u = 1 second */
#define ISR_ID 255
#define CORE0_ID 0
#define CORE1_ID 1
#define CORE2_ID 2
#define CORE3_ID 3
#define  CACUALTION_TIME  200

#include "Rte_ASW_CORE0.h"
#include "ASW_CORE0.h"
FUNC (void, ASW_CORE0_CODE) CallTestStack_func(void);
FUNC (void, ASW_CORE0_CODE) CallTestStack_func1(void);


#define ASW_CORE0_START_SEC_VAR_INIT_8
#include "ASW_CORE0_MemMap.h"

uint8 CPU0_StatusUtiliazation=0;
char Test_ErrorHook=0;
uint8 shutdown_b =0;
uint8 TestOverStack=0;
#define ASW_CORE0_STOP_SEC_VAR_INIT_8
#include "ASW_CORE0_MemMap.h"
Os_StackSizeType Stack_OsTask_ASW_100ms = {0,0};
#define ASW_CORE0_START_SEC_VAR_INIT_64
#include "ASW_CORE0_MemMap.h"
float64  CPU0_PercentUtilization=0.0;
float64 CPU0_MaximumUtilization = 0.0;
float64 Core0_MaxUserStackUtilization = 0.0;
#define ASW_CORE0_STOP_SEC_VAR_INIT_64
#include "ASW_CORE0_MemMap.h"

#define ASW_CORE0_START_SEC_CODE
#include "ASW_CORE0_MemMap.h"
FUNC (void, ASW_CORE0_CODE) RE_CORE0_SWC_100ms_func/* return value & FctID */
(
		void
)
{

	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */
	Rte_Call_RPort_CPUUtilizationCore0_CPU_Utilization(CPULOAD_MEASURE_TIME);
	
	Rte_Call_RPort_GetMaxUserStackUtilization_GetMaxUserStackUtilization(&Core0_MaxUserStackUtilization);

	/*------------------------------------ Test SenderRecivier Port on different core ------------------------------  */


	/*-------------------------------------- Test IRV Read -----------------------------------  */

	/*  -------------------------------------- Shutdown service -----------------------------------------  */

	if(shutdown_b)
	{
		Rte_Write_PPort_SwcModeRequest_AppMode(SWC_REQUEST_SHUTDOWN);
		shutdown_b=0;
	}
	/*------------------------------------Uncomment code for test over stack  --------------------------------------  */
	if(TestOverStack)  
	{
		TestOverStack = 0;
		CallTestStack_func();
	}

	/*-------------------------------------- Test ErrorHook service -----------------------------------  */
	if(Test_ErrorHook)
	{
		ActivateTask(Core0_OsTask_ASW_100ms);
		Test_ErrorHook = 0;
	}
}

FUNC (void, ASW_CORE0_CODE) RE_CORE0_SWC_10ms_func/* return value & FctID */
(
		void
)
{



	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :RE_CORE0_SWC_10ms_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */


	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :RE_CORE0_SWC_10ms_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

FUNC(void, ASW_CORE0_CODE) RE_Core0FaultReaction(VAR(StatusType, AUTOMATIC) Error)
{
	switch (Error)
	{
	case E_OS_LIMIT:
		Rte_Write_PPort_SwcModeRequest_AppMode(SWC_REQUEST_SHUTDOWN);
		break;
	case E_OS_STACKFAULT:
		Rte_Write_PPort_SwcModeRequest_AppMode(SWC_REQUEST_SHUTDOWN);
		break;
	default:
		break;
	}
	
}

FUNC(void, ASW_CORE0_CODE) RE_CORE0_CPU_Utilization_GetResult_func(void)
{
	Rte_Result_RPort_CPUUtilizationCore0_CPU_Utilization(&CPU0_PercentUtilization,&CPU0_StatusUtiliazation);
	if (CPU0_PercentUtilization > CPU0_MaximumUtilization)
	{
		CPU0_MaximumUtilization = CPU0_PercentUtilization;
	}
}




FUNC (void, ASW_CORE0_CODE) CallTestStack_func(void)
{
	uint16 i;
	uint16 test_stack[3000];
	for(i=0;i<3000;i++)
	{
		test_stack[i]=i;
	}
	CallTestStack_func1();
}

FUNC (void, ASW_CORE0_CODE) CallTestStack_func1(void)
{
	Stack_OsTask_ASW_100ms = Os_GetStackUsage();
}


#define ASW_CORE0_STOP_SEC_CODE
#include "ASW_CORE0_MemMap.h"
