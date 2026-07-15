/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_WDG
 * Description: Testcode for ASW_WDG
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        14-Jun-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#ifndef APPLICATIONSOFTWARE_WDG_SWC_API_WDG_SWC_H
#define APPLICATIONSOFTWARE_WDG_SWC_API_WDG_SWC_H


#define     WDGM_DEADLINE_MAX_MS    10U

/*
 **********************************************************************************************************************
 * Type Define
 **********************************************************************************************************************
 */
typedef enum Wdg_Test_t{
	WDG_NONE,
	ALIVE_OK_LACKOF_ONE_CP_ALIVE_SUPERVISION_ENTITY1,
	ALIVE_OK_LACKOF_TWO_ALIVE_SUPERVISION_ENTITY1,
	ALIVE_NOTOK_LACKOF_THREE_ALIVE_SUPERVISION_ENTITY1,
	ALIVE_NOTOK_ADDMORE_ALIVE_SUPERVISION_ENTITY1,
	FAST_MODE,
	SLOW_MODE,
	DEADLINE_OK_ONTIME_CP_WDG_DEADLINE_ENTITY1,
	DEADLINE_NOTOK_LATE_CP_WDG_DEADLINE_ENTITY1,
}Wdg_Test_t;

extern uint32 OS_Counter_1ms;
FUNC(uint32, WdgInternalTest_CODE)  WdgTest_GetTime(uint32 baseTime);
FUNC(void, WdgInternalTest_CODE)  WdgTest_SetBaseTime(uint32 * baseTime);
#endif /* APPLICATIONSOFTWARE_WDG_SWC_API_WDG_SWC_H */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
