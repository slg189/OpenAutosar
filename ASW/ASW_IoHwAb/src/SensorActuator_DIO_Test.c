/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Name: 
 * Description:
 * Version: 1.0
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************

 * Project : TC397_Hightec_V1.3
 * Component: /SensorActuator_DIO_Test/SensorActuator_DIO_Test
 * Runnable : All Runnables in SwComponent
 *****************************************************************************
 * Tool Version: ISOLAR-A/B 9.1
 * Author: xxxxxx
 * Date : ������ ���� 08 10:53:24 2022
 ****************************************************************************/

#include "Rte_SensorActuator_DIO_Test.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :SensorActuator_DIO_Test_Set_Func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :SensorActuator_DIO_Test_Set_Func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :SensorActuator_DIO_Test_Set_Func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define SensorActuator_DIO_Test_START_SEC_CODE                   
#include "SensorActuator_DIO_Test_MemMap.h"
FUNC (void, SensorActuator_DIO_Test_CODE) SensorActuator_DIO_Test_Set_Func/* return value & FctID */
(
		void
)
{

	static boolean DioLevel1;
	Std_ReturnType syncCall1;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :SensorActuator_DIO_Test_Set_Func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	syncCall1 = Rte_Call_RPort_Dio_Set_Dio_Write(DioLevel1);
    DioLevel1 = ~DioLevel1;
	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :SensorActuator_DIO_Test_Set_Func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}
#define SensorActuator_DIO_Test_STOP_SEC_CODE  
#include "SensorActuator_DIO_Test_MemMap.h" 

/*PROTECTED REGION ID(FileHeaderUserDefinedFunctions :SensorActuator_DIO_Test) ENABLED START */
/* Start of user defined functions  - Do not remove this comment */
/* End of user defined functions - Do not remove this comment */
/*PROTECTED REGION END */

