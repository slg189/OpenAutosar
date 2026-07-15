/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_CORE3
 * Description: Testcode for ASW_CORE3
 * Version         Author:       Date               Update information
 * 1.0             AGT1HC        12-Mar-2019        Create software
 * 1.1             AGT1HC        19-Jan-2021        Update software for new requirement OS_2_1
 * 1.2			   HAD1HC	     01-Mar-2021		Add function for fault reaction
 * 1.3			   HAD1HC	     15-Mar-2021		Update function of getting CPU Load
 * 													Add function call for getting stack utilization
 * 1.4             HAD1HC	     13-Apr-2021	    Update Memmap			
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_CORE3.h"
#include "ASW_CORE3.h"

#define CPULOAD_MEASURE_TIME 1000u /* 1000u = 1 second */

#define ASW_CORE3_START_SEC_VAR_INIT_8
#include "ASW_CORE3_MemMap.h"
uint8 CPU3_StatusUtiliazation=0;
#define ASW_CORE3_STOP_SEC_VAR_INIT_8
#include "ASW_CORE3_MemMap.h"

#define ASW_CORE3_START_SEC_VAR_INIT_64
#include "ASW_CORE3_MemMap.h"
float64  CPU3_PercentUtilization=0.0;
float64  CPU3_MaximumUtilization=0.0;
float64 Core3_MaxUserStackUtilization = 0.0;
#define ASW_CORE3_STOP_SEC_VAR_INIT_64
#include "ASW_CORE3_MemMap.h"

#define ASW_CORE3_START_SEC_CODE
#include "ASW_CORE3_MemMap.h"
FUNC (void, ASW_CORE3_CODE) RE_CORE3_SWC_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Rte_Call_RPort_CPUUtilizationCore3_CPU_Utilization(CPULOAD_MEASURE_TIME,&CPU3_PercentUtilization,&CPU3_StatusUtiliazation);
	if (CPU3_PercentUtilization > CPU3_MaximumUtilization)
	{
		CPU3_MaximumUtilization = CPU3_PercentUtilization;
	}

	Rte_Call_RPort_GetMaxUserStackUtilization_GetMaxUserStackUtilization(&Core3_MaxUserStackUtilization);
}

FUNC(void, ASW_CORE3_CODE) RE_Core3FaultReaction(VAR(StatusType, AUTOMATIC) Error)
{

}
#define ASW_CORE3_STOP_SEC_CODE
#include "ASW_CORE3_MemMap.h"
