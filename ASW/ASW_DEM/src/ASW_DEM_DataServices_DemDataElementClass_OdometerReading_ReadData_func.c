/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_DEM
 * Description: Testcode for ASW_DEM
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        04-Mar-2021        Initial version
 * 1.1             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_DEM.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */
#define ASW_DEM_START_SEC_VAR_INIT_8
#include "ASW_DEM_MemMap.h"
uint8 OdometerReading_data[3] = {0x00, 0x00, 0x5A};
#define ASW_DEM_STOP_SEC_VAR_INIT_8
#include "ASW_DEM_MemMap.h"
/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define ASW_DEM_START_SEC_CODE                   
#include "ASW_DEM_MemMap.h"

FUNC (Std_ReturnType, AUTOMATIC) ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func/* return value & FctID */
(
		P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */
		*Data = OdometerReading_data[0];
		*(Data + 1) = OdometerReading_data[1];
		*(Data + 2) = OdometerReading_data[2];
		return retValue;
	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DEM_DataServices_DemDataElementClass_OdometerReading_ReadData_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

#define ASW_DEM_STOP_SEC_CODE                       
#include "ASW_DEM_MemMap.h"
