/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  SensorActuator_PWM
 * Description: Testcode for SensorActuator_PWM
 * Version         Author        Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_SensorActuator_PWM.h"

#define SENSORACTUATOR_PWM_START_SEC_VAR_INIT_16
#include "SensorActuator_PWM_MemMap.h"
uint16 SetPWMDuty =1;
uint16 Pwm_duty =0x4000;  // duty_cycle == 50%
#define SENSORACTUATOR_PWM_STOP_SEC_VAR_INIT_16
#include "SensorActuator_PWM_MemMap.h"
#define SENSORACTUATOR_PWM_START_SEC_CODE
#include "SensorActuator_PWM_MemMap.h"
FUNC (void, SensorActuator_PWM_CODE) PWM_PG_9_Set_func/* return value & FctID */
(
		void
)
{

	uint16 pwm_duty1;
	Std_ReturnType asyncCall1;
	Std_ReturnType retValue = RTE_E_OK;

	if(SetPWMDuty==1)
	{
		SetPWMDuty =0;
		asyncCall1 = Rte_Call_RPort_PWM_PG_9_IoHwAb_PWM_Set(Pwm_duty);

	}
}

#define SENSORACTUATOR_PWM_STOP_SEC_CODE
#include "SensorActuator_PWM_MemMap.h"
