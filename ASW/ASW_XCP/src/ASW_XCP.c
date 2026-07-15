/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_XCP
 * Description: Testcode for ASW_XCP
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        14-Jun-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/
#include "Overlaymanager.h"
#include "Rte_ASW_XCP.h"
#include "IfxScu_bf.h"
#include "IfxOvc_reg.h"

/*locate a memory area for calib */

/** \brief Mask values is used to invert the password value bits */
#define IFX_SSW_WDT_PASSWORD_INVERT_MSK      (0x003FU)



#define XCP_CALIB_START_SEC_CONST_32
#include "Xcp_Memmap.h"
const volatile uint32 test_calib[2] =  {0x01};   /*16 KB data for calib in maximum */
#define XCP_CALIB_STOP_SEC_CONST_32
#include "Xcp_Memmap.h"

#define ASW_XCP_START_SEC_VAR_INIT_8
#include "ASW_XCP_MemMap.h"
uint8 OverlayInitState=0;
#define ASW_XCP_STOP_SEC_VAR_INIT_8
#include "ASW_XCP_MemMap.h"

#define ASW_XCP_START_SEC_VAR_INIT_32
#include "ASW_XCP_MemMap.h"
volatile uint32 Measured_Var[20]={0};
#define ASW_XCP_STOP_SEC_VAR_INIT_32
#include "ASW_XCP_MemMap.h"

#define ASW_XCP_START_SEC_CODE
#include "ASW_XCP_MemMap.h"
FUNC (void, XCP_SWC_CODE) RE_XCP_SWC_func/* return value & FctID */
(
		void
)
{
    static unsigned short cpuWdtPassword = 0;
    static unsigned short safetyWdtPassword = 0;
    Ifx_SCU_WDTS  *watchdog;
    Ifx_SCU_WDTCPU *CpuWatchDog;
	Std_ReturnType retValue = RTE_E_OK;

	/*User code */
	Measured_Var[0] =   test_calib[0] + 1 ;
	Measured_Var[1] =   test_calib[1] + 1;
	if(OverlayInitState == 1u)
	{
		OverlayInitState = 0u;
		/* XcpApp_CurrentCalPage is WORKING_CAL_PAGE; */
	    SCU_OVCCON.U = 0x00050001U;//Overlay Start, OVSTRT = 0
    }
}
#define ASW_XCP_STOP_SEC_CODE
#include "ASW_XCP_MemMap.h"