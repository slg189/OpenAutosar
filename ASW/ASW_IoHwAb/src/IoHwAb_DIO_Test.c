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
 * Component: /IoHwAb_DIO_Test/IoHwAb_DIO_Test
 * Runnable : All Runnables in SwComponent
 *****************************************************************************
 * Tool Version: ISOLAR-A/B 9.1
 * Author: xxxxxx
 * Date : ������ ���� 08 10:53:43 2022
 ****************************************************************************/

#include "Rte_IoHwAb_DIO_Test.h"

#include "Std_Types.h"
#include "Dio_Cfg.h"
#include "Dio.h"
/* FUNC (void, IoHwAb_ICU_CODE) */void IoHwAb_DIO_Test_Init_func
(
		void
)
{
	Dio_WriteChannel(DioConf_DioChannel_DioChannel_P20_13, STD_HIGH);
	Dio_WriteChannel(DioConf_DioChannel_DioChannel_P20_13, STD_LOW);
	Dio_WriteChannel(DioConf_DioChannel_DioChannel_P20_13, STD_HIGH);
}

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :IoHwAb_DIO_Test_Set_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :IoHwAb_DIO_Test_Set_func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :IoHwAb_DIO_Test_Set_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define IoHwAb_DIO_Test_START_SEC_CODE                   
#include "IoHwAb_DIO_Test_MemMap.h"
FUNC (Std_ReturnType, IoHwAb_DIO_Test_CODE) IoHwAb_DIO_Test_Set_func/* return value & FctID */
(
		VAR(boolean, AUTOMATIC) DioLevel
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :IoHwAb_DIO_Test_Set_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :IoHwAb_DIO_Test_Set_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

	Dio_WriteChannel(DioConf_DioChannel_DioChannel_P20_13, DioLevel);

}
#define IoHwAb_DIO_Test_STOP_SEC_CODE  
#include "IoHwAb_DIO_Test_MemMap.h" 

/*PROTECTED REGION ID(FileHeaderUserDefinedFunctions :IoHwAb_DIO_Test) ENABLED START */
/* Start of user defined functions  - Do not remove this comment */
/* End of user defined functions - Do not remove this comment */
/*PROTECTED REGION END */

