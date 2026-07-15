/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_DCM
 * Description: Testcode for ASW_DCM
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        04-Mar-2021        Initial version
 * 1.1             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_DCM.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"

FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func/* return value & FctID */
(
		VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
		P2VAR(Dcm_RequestDataOut_DcmDspRoutine_CheckMemoryCRC_DcmDspRequestRoutineResultsOutSignal_0Type, AUTOMATIC, RTE_APPL_DATA) DataOut_DcmDspRequestRoutineResultsOutSignal_0,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DCM_RoutineServices_CheckMemoryCRC_RequestRoutineResults_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	return retValue;
}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
