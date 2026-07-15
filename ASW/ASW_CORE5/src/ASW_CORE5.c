/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_CORE5
 * Description: Testcode for ASW_CORE5
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        01-Feb-2021        Create software
 * 1.1			   HAD1HC	     01-Mar-2021		Add function for fault reaction
 * 1.2			   HAD1HC	     16-Mar-2021		Add function call for getting 
 * 												    stack utilization
 * 1.3             HAD1HC	     24-May-2021		Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_CORE5.h"
#include "ASW_CORE5.h"

#define CPULOAD_MEASURE_TIME 1000u /* 1000u = 1 second */

#define ASW_CORE5_START_SEC_VAR_INIT_8
#include "ASW_CORE5_MemMap.h"
uint8 CPU5_StatusUtiliazation = 0;
#define ASW_CORE5_STOP_SEC_VAR_INIT_8
#include "ASW_CORE5_MemMap.h"

#define ASW_CORE5_START_SEC_VAR_INIT_64
#include "ASW_CORE5_MemMap.h"
float64  CPU5_PercentUtilization=0.0;
float64  CPU5_MaximumUtilization=0.0;
float64 Core5_MaxUserStackUtilization = 0.0;
#define ASW_CORE5_STOP_SEC_VAR_INIT_64
#include "ASW_CORE5_MemMap.h"

#define ASW_CORE5_START_SEC_CODE                   
#include "ASW_CORE5_MemMap.h"
FUNC (void, ASW_CORE5_CODE) RE_CORE5_SWC_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Rte_Call_RPort_CPUUtilizationCore5_CPU_Utilization(CPULOAD_MEASURE_TIME,&CPU5_PercentUtilization,&CPU5_StatusUtiliazation);
	if (CPU5_PercentUtilization > CPU5_MaximumUtilization)
	{
		CPU5_MaximumUtilization = CPU5_PercentUtilization;
	}
	
	Rte_Call_RPort_GetMaxUserStackUtilization_GetMaxUserStackUtilization(&Core5_MaxUserStackUtilization);
}

FUNC(void, ASW_CORE5_CODE) RE_Core5FaultReaction(VAR(StatusType, AUTOMATIC) Error)
{

}
#define ASW_CORE5_STOP_SEC_CODE                       
#include "ASW_CORE5_MemMap.h"
