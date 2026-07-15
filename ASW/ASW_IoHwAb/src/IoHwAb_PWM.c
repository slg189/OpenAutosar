/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  IoHwAb_PWM
 * Description: Testcode for IoHwAb_PWM
 * Version         Author        Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_IoHwAb_PWM.h"
#include "Pwm_17_GtmCcu6.h"
#define PWM_PERIOD                    (0x7999U)

#define IOHWAB_PWM_START_SEC_CODE
#include "IoHwAb_PWM_MemMap.h"

FUNC (void , IoHwAb_PWM_CODE) IoHwAb_PWM_PG_9_func/* return value & FctID */
(
		VAR(uint16, AUTOMATIC) pwm_duty
)
{

	Std_ReturnType retValue = RTE_E_OK;
	Pwm_17_GtmCcu6_SetPeriodAndDuty(Pwm_17_GtmCcu6Conf_PwmChannel_PwmChannel_0,PWM_PERIOD,pwm_duty);
}

#define IOHWAB_PWM_STOP_SEC_CODE
#include "IoHwAb_PWM_MemMap.h"
