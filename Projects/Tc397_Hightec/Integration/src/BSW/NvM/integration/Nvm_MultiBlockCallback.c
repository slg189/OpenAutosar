#include "NvM.h"
#include "Rte_NvM_Type.h"
#include "TimingCalculation.h"
#include "MemIf.h"

#define BSW_START_SEC_VAR_CLEARED_16
#include "Bsw_Memmap.h"
uint16 ReadAllcounter,WriteAllcounter;
#define BSW_STOP_SEC_VAR_CLEARED_16
#include "Bsw_Memmap.h"

#define BSW_START_SEC_VAR_INIT_32
#include "Bsw_Memmap.h"
#if (OS_COUNTER_IS_USED == TRUE)
static uint32 NvM_WriteAll_ElapsedTime = 0;
static uint32 NvM_ReadAll_ElapsedTime = 0;
#else
static float32 NvM_WriteAll_ElapsedTime = 0;
static float32 NvM_ReadAll_ElapsedTime = 0;
#endif
#define BSW_STOP_SEC_VAR_INIT_32
#include "Bsw_Memmap.h"

#define BSW_START_SEC_CODE
#include "Bsw_Memmap.h"
void IC_BswM_NvM_ReadAll ( void )
{
	NvM_Rb_StatusType status_NvM;
	MemIf_StatusType stMemIf_en;
	uint32 *CRC_ptr;
	uint32 CRC_data;
	uint32 CRC_cal;

	ReadAllcounter=0;
	/* disable detection and report of timeout for FLS */
	Fls_ControlTimeoutDet(0);

	IC_SetCurrentTime();
	NvM_ReadAll();
	do
	{
		Wdg_17_Scu_SetTriggerCondition(2000);
		NvM_MainFunction();
		MemIf_Rb_MainFunction();
		NvM_Rb_GetStatus(&status_NvM);
		stMemIf_en = MemIf_Rb_GetStatus();
		ReadAllcounter++;
	} while ( (status_NvM == NVM_RB_STATUS_BUSY ) || (stMemIf_en == MEMIF_BUSY));
	NvM_ReadAll_ElapsedTime = IC_GetElapsedTime();

	/* enable detection and report of timeout for FLS */
	Fls_ControlTimeoutDet(1);

}


void IC_BswM_NvM_WriteAll ( void )
{
	NvM_Rb_StatusType status_NvM;
	MemIf_StatusType stMemIf_en;

	WriteAllcounter=0;

	/*
	 * Here RTE scheduling is stopped, so WdgM won't be triggered anymore.
	 * we need to prevent the Wdg from reseting the CPU.
	 * WDG_SLOW_MODE so Wdg increases the hardware timeout period
	 */

	
	/* disable detection and report of timeout for FLS */
	Fls_ControlTimeoutDet(0);

	IC_SetCurrentTime();
	NvM_WriteAll();

	do
	{
		/* Trigger watchdog to avoid timer's watchdog is overflow*/
		Wdg_17_Scu_SetTriggerCondition(2000);
		NvM_MainFunction();
		MemIf_Rb_MainFunction();

		NvM_Rb_GetStatus(&status_NvM);
		stMemIf_en = MemIf_Rb_GetStatus();

		WriteAllcounter++;
	} while ( (status_NvM == NVM_RB_STATUS_BUSY ) || (stMemIf_en == MEMIF_BUSY));
	NvM_WriteAll_ElapsedTime = IC_GetElapsedTime();

	/* enable detection and report of timeout for FLS */
	Fls_ControlTimeoutDet(1);

}
#define BSW_STOP_SEC_CODE
#include "Bsw_Memmap.h"