/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  SensorActuator_ICU
 * Description: Testcode for SensorActuator_ICU
 * Version         Author        Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_SensorActuator_ICU.h"

#define SENSORACTUATOR_ICU_START_SEC_VAR_INIT_16
#include "SensorActuator_ICU_MemMap.h"
uint16 IcuDuty =0;
#define SENSORACTUATOR_ICU_STOP_SEC_VAR_INIT_16
#include "SensorActuator_ICU_MemMap.h"

#define SENSORACTUATOR_ICU_START_SEC_CODE
#include "SensorActuator_ICU_MemMap.h"
FUNC (void, SensorActuator_ICU_CODE) ICU_PZ_1_Get_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType asyncCall1;
	Std_ReturnType retValue = RTE_E_OK;

	asyncCall1 = Rte_Call_RPort_ICU_PZ_1_IoHwAb_ICU_Get(&IcuDuty);
}

#define SENSORACTUATOR_ICU_STOP_SEC_CODE
#include "SensorActuator_ICU_MemMap.h"
