/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  IoHwAb_ADC
 * Description: Testcode for IoHwAb_ADC
 * Version         Author:       Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_IoHwAb_ADC.h"
#include "Adc.h"
#include "Adc_Cfg.h"
#include "Irq.h"
#include "IfxSrc_reg.h"
#define AdcSWGroup_0 AdcConf_AdcGroup_AdcSWGroup_0
#define AdcSWGroup_1 AdcConf_AdcGroup_AdcSWGroup_1
#define SRE_ENABLE (1<<10)


#define IOHWAB_ADC_START_SEC_VAR_INIT_16
#include "IoHwAb_ADC_MemMap.h"

Adc_ValueGroupType Adc_Group0_Result[2]={0,0};
Adc_ValueGroupType Adc_Group1_Result[1]={0};

#define IOHWAB_ADC_STOP_SEC_VAR_INIT_16
#include "IoHwAb_ADC_MemMap.h"
#define IOHWAB_ADC_START_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"

FUNC (void, IoHwAb_ADC_CODE) IoHwAb_ADC_Init_func/* return value & FctID */
(
		void
)
{

	
	Adc_SetupResultBuffer(AdcSWGroup_0,Adc_Group0_Result); 
	Adc_SetupResultBuffer(AdcSWGroup_1,Adc_Group1_Result);

}

#define IOHWAB_ADC_STOP_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
#define IOHWAB_ADC_START_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
FUNC (void , IoHwAb_ADC_CODE) IoHwAb_ADC_Get_SAR0_31_func/* return value & FctID */
(
		P2VAR(uint16, AUTOMATIC, RTE_APPL_DATA) RawVoltage
)
{
	*RawVoltage = Adc_Group0_Result[1];
}

#define IOHWAB_ADC_STOP_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
#define IOHWAB_ADC_START_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
FUNC (void , IoHwAb_ADC_CODE) IoHwAb_ADC_Get_SAR1_12_func/* return value & FctID */
(
		P2VAR(uint16, AUTOMATIC, RTE_APPL_DATA) RawVoltage
)
{
	*RawVoltage = Adc_Group1_Result[0];
}

#define IOHWAB_ADC_STOP_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
#define IOHWAB_ADC_START_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
FUNC (void , IoHwAb_ADC_CODE) IoHwAb_ADC_Get_SAR0_10_func/* return value & FctID */
(
		P2VAR(uint16, AUTOMATIC, RTE_APPL_DATA) RawVoltage
)
{

	*RawVoltage = Adc_Group0_Result[0];
}

#define IOHWAB_ADC_STOP_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"

