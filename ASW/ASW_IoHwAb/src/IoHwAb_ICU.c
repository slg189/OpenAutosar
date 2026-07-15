/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  IoHwAb_ICU
 * Description: Testcode for IoHwAb_ICU
 * Version         Author:       Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_IoHwAb_ICU.h"

#include "Icu_17_TimerIp.h"
#include "Icu_17_TimerIp_Cfg.h"
#include "IfxSrc_reg.h"
#define SignalMeas  IcuConf_IcuChannel_SignalMeas

#define IOHWAB_ICU_START_SEC_VAR_INIT_UNSPECIFIED
#include "IoHwAb_ICU_MemMap.h"

Icu_17_TimerIp_DutyCycleType DutyCycleValues={0,0};;

#define IOHWAB_ICU_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "IoHwAb_ICU_MemMap.h"
#define IOHWAB_ICU_START_SEC_CODE
#include "IoHwAb_ICU_MemMap.h"

FUNC (void, IoHwAb_ICU_CODE) IoHwAb_ICU_Init_func/* return value & FctID */
(
		void
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Icu_17_TimerIp_StartSignalMeasurement(SignalMeas);
}

#define IOHWAB_ICU_STOP_SEC_CODE
#include "IoHwAb_ICU_MemMap.h"
#define IOHWAB_ICU_START_SEC_CODE
#include "IoHwAb_ICU_MemMap.h"


FUNC (void , IoHwAb_ICU_CODE) IoHwAb_ICU_Get_PZ_1_func/* return value & FctID */
(
		P2VAR(uint16, AUTOMATIC, RTE_APPL_DATA) Duty
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Icu_17_TimerIp_GetDutyCycleValues(SignalMeas,&DutyCycleValues);
	if(DutyCycleValues.PeriodTime == 0)
	{
        DutyCycleValues.PeriodTime = 1;
	}
	*Duty = (uint32)((DutyCycleValues.ActiveTime * 200U) + (DutyCycleValues.PeriodTime)) / (uint32)(2U * DutyCycleValues.PeriodTime);
}

#define IOHWAB_ICU_STOP_SEC_CODE
#include "IoHwAb_ICU_MemMap.h"

