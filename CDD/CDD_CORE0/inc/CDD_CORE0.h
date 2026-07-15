/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  CDD_CORE0
 * Description: Testcode for CDD_CORE0
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        19-Nov-2021        Create software
 * 1.1             HAD1HC        15-Mar-2021        Update function of calculating
 * 													stack utilization and CPU Load
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/
#ifndef CDD_CORE0_H
#define CDD_CORE0_H

#include "Std_Types.h"
#include "Os.h"
#include "Rte.h"
#include "Rte_Type.h"
#include "Ifx_reg.h"
#include "EcuM_Cfg.h"


#define OSSWTICKSPERMILLISECOND (OSSWTICKSPERSECOND/1000UL)

enum StateCaculation_type
{
	PREPARE,
	START_CACULATION,
	WAIT_CACULATION,
	FINISH_START_CACULATION,
};

#define STACK_STARTUP_SIZE           0x02U       /* stack usage for start up */
#define STACK_EMPTY_STEP             0x01U       /* granularity for stack empty checking */
#define STACK_EMPTY_VALUE            0x00000000U
#define STACK_EMPTY_GRANULARITY_U32  0x08U       /* Number of 4Bytes for one funciton call */

extern void *__USTACK0;
extern void *__USTACK0_END;

extern FUNC(Os_StopwatchTickType, {memclass}) Os_Cbk_GetStopwatch(void);

#endif /* CDD_CORE0_H */
