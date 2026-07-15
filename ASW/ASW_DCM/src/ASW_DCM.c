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
 * 1.0             GBI81HC       31-May-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        04-Mar-2021        Update functions for new 
 * 													DIAG requirements
 * 1.3             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_DCM.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */
#define START_ROUTINE 1
#define STOP_ROUTINE 2
#define REQUEST_ROUTINE 3
#define RoutineServices_DriveVacuumPump_Start              (450U)

#define ASW_DCM_START_SEC_VAR_INIT_8
#include "ASW_DCM_MemMap.h"
static uint8 mode_Nrc0x72 = 1;
static uint8 mode_Nrc0x22 = 1;
static uint8 modeswitch_Nrc0x72_Start = 1;
static uint8 modeswitch_Nrc0x22_Start = 1;
static uint8 mode_routine= 0;
static Dcm_DataArrayTypeUint8_SystemFaultRankType DID_2311= {0x11,0x11,0x11,0x11};
static Dcm_DataArrayTypeUint8_VehicleSpeedType DID_2310 = {0x22,0x22,0x22,0x22};
static Dcm_DataArrayTypeUint8_VacuumPumpPresType VacuumPumpPressure_Data = {0xCC};
static Dcm_DataArrayTypeUint8_ECU_HW_Ref_NumType HwRef = {0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0};
static Dcm_DataArrayTypeUint8_ECU_SW_Ref_NumType SwRef = {0xA1,0xA1,0xA1,0xA1,0xA1,0xA1,0xA1,0xA1,0xA1,0xA1};
static Dcm_DataArrayTypeUint8_DcmDspData_VINType VIN = {0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2};
static Dcm_DataArrayTypeUint8_ECU_Serial_NumberType ECU_Serial_Number = {0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3,0xA3};
static Dcm_DataArrayTypeUint8_ECU_NCF_Ref_NumType NCF_Reference_Number = {0xA4,0xA4,0xA4,0xA4,0xA4,0xA4,0xA4,0xA4};
static Dcm_DataArrayTypeUint8_EngineTemperatureType EngineTemperature_Data = {0};
static Dcm_DataArrayTypeUint8_PowerModeType PowerMode_Data = {0};
#define ASW_DCM_STOP_SEC_VAR_INIT_8
#include "ASW_DCM_MemMap.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_VacuumPumpPressure_resetToDefault_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	VacuumPumpPressure_Data[0] = 0x10;
    *ErrorCode = 0;
    return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_PowerMode_resetToDefault_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_PowerMode_resetToDefault_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_PowerMode_resetToDefault_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	PowerMode_Data[0] = 0x20;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_VacuumPumpPressure_returnControlToECU_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_VacuumPumpPressure_returnControlToECU_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_VacuumPumpPressure_returnControlToECU_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	VacuumPumpPressure_Data[0] = 0xAA;
    *ErrorCode = 0;
    return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (void, ASW_DCM_CODE) ASW_DCM_ModeCondition_Nrc0x72_func/* return value & FctID */
(
		void
)
{

	Std_ReturnType e1;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_ModeCondition_Nrc0x72_func) ENABLED START */
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
	if (modeswitch_Nrc0x72_Start)
    {
		e1 = Rte_Switch_PPortPrototype_NRC72_MDGP_ModeDeclarationGroup_NRC72(mode_Nrc0x72);
		modeswitch_Nrc0x72_Start = 0;
	}

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DCM_ModeCondition_Nrc0x72_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_EngineTemperature_freezeCurrentState_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_EngineTemperature_freezeCurrentState_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_EngineTemperature_freezeCurrentState_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	EngineTemperature_Data[0] = 0xBB;
	*ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_PowerMode_shortTermAdjustment_func/* return value & FctID */
(
		P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) ControlStateInfo,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_PowerMode_shortTermAdjustment_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_PowerMode_shortTermAdjustment_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	PowerMode_Data[0] = *ControlStateInfo;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_RoutineServices_DriveVacuumPump_RequestRoutineResults_func/* return value & FctID */
(
		VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
		P2VAR(Dcm_RequestDataOut_DcmDspRoutine_DriveVacuumPump_DcmDspRequestRoutineResultsOutSignal_0Type, AUTOMATIC, RTE_APPL_DATA) DataOut_DcmDspRequestRoutineResultsOutSignal_0,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_RoutineServices_DriveVacuumPump_RequestRoutineResults_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_RoutineServices_DriveVacuumPump_RequestRoutineResults_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	mode_routine = REQUEST_ROUTINE;
	return retValue;
}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_PowerMode_freezeCurrentState_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_PowerMode_freezeCurrentState_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_PowerMode_freezeCurrentState_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	PowerMode_Data[0] = 0xCC;
	*ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_RoutineServices_DriveVacuumPump_Stop_func/* return value & FctID */
(
		VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
		P2VAR(Dcm_StopDataOut_DcmDspRoutine_DriveVacuumPump_DcmDspStopRoutineOutSignalType, AUTOMATIC, RTE_APPL_DATA) DataOut_DcmDspStopRoutineOutSignal,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_RoutineServices_DriveVacuumPump_Stop_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_RoutineServices_DriveVacuumPump_Stop_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	mode_routine = STOP_ROUTINE;
	return retValue;
}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (void, ASW_DCM_CODE) ASW_DCM_PeriodicRunnable_10ms_func/* return value & FctID */
(
		void
)
{

	Std_ReturnType retRead1;
	Std_ReturnType retRead2;
	Std_ReturnType retRead3;
	Std_ReturnType retRead4;
	Std_ReturnType retWrite5;
	Std_ReturnType retWrite6;
	Std_ReturnType retWrite7;
	Std_ReturnType retWrite8;
	Std_ReturnType retWrite9;
	Std_ReturnType retWrite10;
	Dcm_DataArrayTypeUint8_ECU_NCF_Ref_NumType write11;
	Std_ReturnType retWrite11;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_PeriodicRunnable_10ms_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */
	retRead1 = Rte_Read_RPort_DataServices_DcmDspData_SystemFaultRank_Write_Data(&DID_2311);
	retRead2 = Rte_Read_RPort_DataServices_DcmDspData_VehicleSpeed_Write_Data(&DID_2310);
	retRead3 = Rte_Read_R_DataServices_DcmDspData_ECU_Serial_Number_Data(&ECU_Serial_Number);
	retRead4 = Rte_Read_R_DataServices_DcmDspData_VIN_Data(&VIN);

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */
	retWrite5 = Rte_Write_P_DataServices_DcmDspData_SystemFaultRank_Read_Data(&DID_2311);
	retWrite6 = Rte_Write_P_DataServices_DcmDspData_VehicleSpeed_Read_Data(&DID_2310);
	retWrite7 = Rte_Write_P_DataServices_DcmDspData_VIN_Data(&VIN);
	retWrite8 = Rte_Write_P_DataServices_DcmDspData_ECU_Serial_Number_Data(&ECU_Serial_Number);
	retWrite9 = Rte_Write_P_DataServices_DcmDspData_HaRef_Data(&HwRef);
	retWrite10 = Rte_Write_P_DataServices_DcmDspData_SwRef_Data(&SwRef);
	retWrite11 = Rte_Write_P_DataServices_DcmDspData_ECU_NCF_Reference_Number_Data(&NCF_Reference_Number);

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DCM_PeriodicRunnable_10ms_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_EngineTemperature_returnControlToECU_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_EngineTemperature_returnControlToECU_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_EngineTemperature_returnControlToECU_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	EngineTemperature_Data[0] = 0xAA;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_EngineTemperature_resetToDefault_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_EngineTemperature_resetToDefault_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_EngineTemperature_resetToDefault_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	EngineTemperature_Data[0] = 0x10;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_VacuumPumpPressure_ReadData_func/* return value & FctID */
(
		P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_VacuumPumpPressure_ReadData_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_VacuumPumpPressure_ReadData_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	*Data = 0xCC;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_EngineTemperature_shortTermAdjustment_func/* return value & FctID */
(
		P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) ControlStateInfo,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_EngineTemperature_shortTermAdjustment_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_EngineTemperature_shortTermAdjustment_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	EngineTemperature_Data[0] = *ControlStateInfo;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_PoweMode_ReadData_func/* return value & FctID */
(
		P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_PoweMode_ReadData_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_PoweMode_ReadData_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	*Data = PowerMode_Data[0];
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_RoutineServices_DriveVacuumPump_Start_func/* return value & FctID */
(
		VAR(Dcm_StartDataIn_DcmDspRoutine_DriveVacuumPump_DcmDspStartRoutineInSignalType, AUTOMATIC) DataIn_DcmDspStartRoutineInSignal,
		VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,
		P2VAR(Dcm_StartDataOut_DcmDspRoutine_DriveVacuumPump_DcmDspStartRoutineOutSignalType, AUTOMATIC, RTE_APPL_DATA) DataOut_DcmDspStartRoutineOutSignal,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */
	uint32 LoopCount;

	/*PROTECTED REGION ID(UserVariables :Diag_RoutineServices_DriveVacuumPump_Start_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :Diag_RoutineServices_DriveVacuumPump_Start_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
 	mode_routine = START_ROUTINE;
	// if (modeswitch_Nrc0x72 == 1)
	// {
	// 	Std_ReturnType retValue = RTE_E_INVALID;
	// }
	 /* Add some delay for RoutineServices_Start */
    for(LoopCount = 0U;LoopCount<(uint32)RoutineServices_DriveVacuumPump_Start;LoopCount++)
    {
      
    }
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
   	// modeswitch_Nrc0x72 == 2;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (void, ASW_DCM_CODE) ASW_DCM_ModeCondition_Nrc0x22_func/* return value & FctID */
(
		void
)
{

	Std_ReturnType e1;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_ModeCondition_Nrc0x22_func) ENABLED START */
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
	if (modeswitch_Nrc0x22_Start)
    {
		e1 = Rte_Switch_PPortPrototype_NRC22_MDGP_ModeDeclarationGroup_NRC22(mode_Nrc0x22);
		modeswitch_Nrc0x22_Start = 0;
	}
	
	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :ASW_DCM_ModeCondition_Nrc0x22_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_VacuumPumpPressure_shortTermAdjustment_func/* return value & FctID */
(
		P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) ControlStateInfo,
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_VacuumPumpPressure_shortTermAdjustment_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_VacuumPumpPressure_shortTermAdjustment_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	VacuumPumpPressure_Data[0] = *ControlStateInfo;
    *ErrorCode = 0;
    return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_PowerMode_returnControlToECU_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_PowerMode_returnControlToECU_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_PowerMode_returnControlToECU_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	PowerMode_Data[0] = 0xAA;
    *ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_VacuumPumpPressure_freezeCurrentState_func/* return value & FctID */
(
		P2VAR(Dcm_NegativeResponseCodeType, AUTOMATIC, RTE_APPL_DATA) ErrorCode
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_VacuumPumpPressure_freezeCurrentState_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_VacuumPumpPressure_freezeCurrentState_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	VacuumPumpPressure_Data[0] = 0xBB;
	*ErrorCode = 0;
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
#define ASW_DCM_START_SEC_CODE                   
#include "ASW_DCM_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) ASW_DCM_IOControl_EngineTemperature_ReadData_func/* return value & FctID */
(
		P2VAR(uint8, AUTOMATIC, RTE_APPL_DATA) Data
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :ASW_DCM_IOControl_EngineTemperature_ReadData_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :ASW_DCM_IOControl_EngineTemperature_ReadData_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	*Data = EngineTemperature_Data[0];
	return retValue;

}

#define ASW_DCM_STOP_SEC_CODE                       
#include "ASW_DCM_MemMap.h"
