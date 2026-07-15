/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  SensorActuator_ADC
 * Description: Testcode for SensorActuator_ADC
 * Version         Author        Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_SensorActuator_ADC.h"
#include "Adc.h"
#include "Adc_Cfg.h"

#define SENSORACTUATOR_ADC_START_SEC_VAR_INIT_32
#include "SensorActuator_ADC_MemMap.h"

float32 Adc0_RawBufferPyh[2]={0,0};
float32 Adc1_RawBufferPyh[1]={0};

#define SENSORACTUATOR_ADC_STOP_SEC_VAR_INIT_32
#include "SensorActuator_ADC_MemMap.h"
#define SENSORACTUATOR_ADC_START_SEC_CODE
#include "SensorActuator_ADC_MemMap.h"

FUNC (void, SensorActuator_ADC_CODE) ADC_Get_func/* return value & FctID */
(
		void
)
{

	Std_ReturnType asyncCall1;
	Std_ReturnType asyncCall2;
	Std_ReturnType asyncCall3;

	uint16 ADC_Value1 = 0;
	uint16 ADC_Value2 = 0;
	uint16 ADC_Value3 = 0;

	Std_ReturnType retValue = RTE_E_OK;

	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	asyncCall1 = Rte_Call_RPort_ADC_Get_SAR0_31_IoHwAb_ADC_Get(&ADC_Value1);

	asyncCall2 = Rte_Call_RPort_ADC_Get_SAR0_10_IoHwAb_ADC_Get(&ADC_Value2);

	asyncCall3 = Rte_Call_RPort_ADC_Get_SAR1_12_IoHwAb_ADC_Get(&ADC_Value3);

	Adc0_RawBufferPyh[0] = ADC_Value2*5.0/4096;
	Adc0_RawBufferPyh[1] = ADC_Value1*5.0/4096;
    Adc1_RawBufferPyh[0] = ADC_Value3*5.0/4096;

	Adc_StartGroupConversion(AdcConf_AdcGroup_AdcSWGroup_0);
	Adc_StartGroupConversion(AdcConf_AdcGroup_AdcSWGroup_1);
}

#define SENSORACTUATOR_ADC_STOP_SEC_CODE
#include "SensorActuator_ADC_MemMap.h"

