/*
 *
 * 	Clock initializzation Funcntion
 *
 *
 */

#include "McuFunc.h"
#include "Mcu.h"
#include "IfxSmu_reg.h"
#include "IfxScu_reg.h"

#define CPU0_START_SEC_CODE
#include "MemMap.h"
void McuFunc_InitializeClock(void)
{
	Std_ReturnType RetValue;
	/* Initialize MCU Clock*/
	/* parameter 0 is chosen here by default, the first clock configuration */
	RetValue = Mcu_InitClock(0);
	/* wait till PLL lock */
	while(Mcu_GetPllStatus() == MCU_PLL_UNLOCKED)
	{

	}
	/* distribute the clock */
	Mcu_DistributePllClock();

}
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"
