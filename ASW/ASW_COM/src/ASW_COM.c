#include "Rte_ASW_COM.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :RE_ComSWC) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :RE_ComSWC) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :RE_ComSWC) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */


#define ASW_COM_START_SEC_VAR_INIT_8
#include "ASW_COM_MemMap.h"
uint8 MCU03_NRF_IdcSamp;    Std_ReturnType RetMCU03_NRF_IdcSamp;
uint8 MCU03_NRF_EMTempSamp;    Std_ReturnType RetMCU03_NRF_EMTempSamp;
uint8 MCU03_NRF_PhCSamp;    Std_ReturnType RetMCU03_NRF_PhCSamp;
uint8 MCU03_NRF_PhBSamp;    Std_ReturnType RetMCU03_NRF_PhBSamp;
uint8 MCU03_NRF_PhASamp;    Std_ReturnType RetMCU03_NRF_PhASamp;
uint8 MCU03_NRF_Resolver;    Std_ReturnType RetMCU03_NRF_Resolver;
uint8 MCU03_NRF_5VSupply;    Std_ReturnType RetMCU03_NRF_5VSupply;
uint8 MCU03_NRF_DESAT;    Std_ReturnType RetMCU03_NRF_DESAT;
uint8 MCU03_NRF_SysFailure;    Std_ReturnType RetMCU03_NRF_SysFailure;
uint8 MCU03_NRF_COMError;    Std_ReturnType RetMCU03_NRF_COMError;
uint8 MCU03_NRF_InvTempSamp;    Std_ReturnType RetMCU03_NRF_InvTempSamp;
uint8 MCU03_NRF_UdcSamp;    Std_ReturnType RetMCU03_NRF_UdcSamp;
uint8 MCU03_RF_PhaseImp;    Std_ReturnType RetMCU03_RF_PhaseImp;
uint8 MCU03_RF_IntError;    Std_ReturnType RetMCU03_RF_IntError;
uint8 MCU03_RF_UDCUnder;    Std_ReturnType RetMCU03_RF_UDCUnder;
uint8 MCU03_RF_OverTempINV;    Std_ReturnType RetMCU03_RF_OverTempINV;
uint8 MCU03_RF_ToqError;    Std_ReturnType RetMCU03_RF_ToqError;
uint8 MCU03_RF_UdcOver;    Std_ReturnType RetMCU03_RF_UdcOver;
uint8 MCU03_RF_IdcOver;    Std_ReturnType RetMCU03_RF_IdcOver;
uint8 MCU03_12VSupply;    Std_ReturnType RetMCU03_12VSupply;
uint8 MCU03_RF_PhCOver;    Std_ReturnType RetMCU03_RF_PhCOver;
uint8 MCU03_RF_PhBOver;    Std_ReturnType RetMCU03_RF_PhBOver;
uint8 MCU03_RF_PhAOver;    Std_ReturnType RetMCU03_RF_PhAOver;
uint8 MCU03_RF_HWOverCur;    Std_ReturnType RetMCU03_RF_HWOverCur;
uint8 MCU03_RF_MsgTO;    Std_ReturnType RetMCU03_RF_MsgTO;
uint8 MCU03_RF_BusOff;    Std_ReturnType RetMCU03_RF_BusOff;
uint8 MCU03_RF_OverSpeed;    Std_ReturnType RetMCU03_RF_OverSpeed;
uint8 MCU03_RF_OverTempEM;    Std_ReturnType RetMCU03_RF_OverTempEM;
uint8 MCU03_LI_UdcOver;    Std_ReturnType RetMCU03_LI_UdcOver;
uint8 MCU03_LI_OverSpeed;    Std_ReturnType RetMCU03_LI_OverSpeed;
uint8 MCU03_LI_OverTempEM;    Std_ReturnType RetMCU03_LI_OverTempEM;
uint8 MCU03_LI_OverTempINV;    Std_ReturnType RetMCU03_LI_OverTempINV;
uint8 MCU03_LI_UdcUnder;    Std_ReturnType RetMCU03_LI_UdcUnder;
uint8 MCU01_ShutDownAllowed;    Std_ReturnType RetMCU01_ShutDownAllowed;
uint8 MCU01_Counter;    Std_ReturnType RetMCU01_Counter;
uint8 MCU01_SelfCheck;    Std_ReturnType RetMCU01_SelfCheck;
uint8 MCU01_ActDischStatus;    Std_ReturnType RetMCU01_ActDischStatus;
uint8 MCU01_Status;    Std_ReturnType RetMCU01_Status;
uint8 MCU01_HighVolAllowed;    Std_ReturnType RetMCU01_HighVolAllowed;
uint8 MCU01_Ready;    Std_ReturnType RetMCU01_Ready;
uint8 MCU01_EmergShutDown;    Std_ReturnType RetMCU01_EmergShutDown;
uint8 MCU01_CheckSum;    Std_ReturnType RetMCU01_CheckSum;
uint8 HCU02_Poweroff;    Std_ReturnType RetHCU02_Poweroff;
uint8 HCU01_Shift;    Std_ReturnType RetHCU01_Shift;
uint8 HCU01_EnaMCU;    Std_ReturnType RetHCU01_EnaMCU;
uint8 HCU01_McuModeCmd;    Std_ReturnType RetHCU01_McuModeCmd;
uint8 HCU01_ActiveDischarge;    Std_ReturnType RetHCU01_ActiveDischarge;
uint8 HCU01_EmergShutDown;    Std_ReturnType RetHCU01_EmergShutDown;
uint8 HCU01_Counter;    Std_ReturnType RetHCU01_Counter;
uint8 HCU01_CheckSum;    Std_ReturnType RetHCU01_CheckSum;
Std_ReturnType RetMCU02_MaxTor;
Std_ReturnType RetMCU02_MinTor;
Std_ReturnType RetMCU02_Vdc;
Std_ReturnType RetMCU02_Idc;
Std_ReturnType RetMCU02_IGBTTmp;
Std_ReturnType RetMCU02_MotorTmp;
Std_ReturnType RetMCU01_Motorspd;
Std_ReturnType RetMCU01_CurCntTor;
Std_ReturnType RetHCU01_TqCmd;
Std_ReturnType RetHCU01_SpeedCmd;
#define ASW_COM_STOP_SEC_VAR_INIT_8
#include "ASW_COM_MemMap.h"

#define ASW_COM_START_SEC_VAR_INIT_16
#include "ASW_COM_MemMap.h"
uint16 MCU02_MaxTor;    
uint16 MCU02_MinTor;    
uint16 MCU02_Vdc;    
sint16 MCU02_Idc;    
sint16 MCU02_IGBTTmp;    
sint16 MCU02_MotorTmp; 
sint16 HCU01_TqCmd;   
sint16 HCU01_SpeedCmd;    
#define ASW_COM_STOP_SEC_VAR_INIT_16
#include "ASW_COM_MemMap.h"

#define ASW_COM_START_SEC_VAR_INIT_32
#include "ASW_COM_MemMap.h"
sint32 MCU01_Motorspd;   
sint32 MCU01_CurCntTor;
#define ASW_COM_STOP_SEC_VAR_INIT_32
#include "ASW_COM_MemMap.h"
   


#define ASW_COM_START_SEC_CODE                   
#include "ASW_COM_MemMap.h"

FUNC (void, ASW_COM_CODE) RE_COM_SWC_func/* return value & FctID */
(
		void
)
{
	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :RE_ComSWC) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */
	RetHCU02_Poweroff = Rte_Read_RPort_HCU02_Poweroff_HCU02_Poweroff(&HCU02_Poweroff);
    RetHCU01_TqCmd = Rte_Read_RPort_HCU01_TqCmd_HCU01_TqCmd(&HCU01_TqCmd);
    RetHCU01_Shift = Rte_Read_RPort_HCU01_Shift_HCU01_Shift(&HCU01_Shift);
    RetHCU01_SpeedCmd = Rte_Read_RPort_HCU01_SpeedCmd_HCU01_SpeedCmd(&HCU01_SpeedCmd);
    RetHCU01_EnaMCU = Rte_Read_RPort_HCU01_EnaMCU_HCU01_EnaMCU(&HCU01_EnaMCU);
    RetHCU01_McuModeCmd = Rte_Read_RPort_HCU01_McuModeCmd_HCU01_McuModeCmd(&HCU01_McuModeCmd);
    RetHCU01_ActiveDischarge = Rte_Read_RPort_HCU01_ActiveDischarge_HCU01_ActiveDischarge(&HCU01_ActiveDischarge);
    RetHCU01_EmergShutDown = Rte_Read_RPort_HCU01_EmergShutDown_HCU01_EmergShutDown(&HCU01_EmergShutDown);
    RetHCU01_Counter = Rte_Read_RPort_HCU01_Counter_HCU01_Counter(&HCU01_Counter);
    RetHCU01_CheckSum = Rte_Read_RPort_HCU01_CheckSum_HCU01_CheckSum(&HCU01_CheckSum);
	/*  -------------------------------------- Server Call Point  --------------------------------  */
	
	/*  -------------------------------------- CDATA ---------------------------------------------  */
	if (RetHCU01_ActiveDischarge != 0)
	{
		HCU01_ActiveDischarge = 1;
	}

	if (RetHCU01_CheckSum != 0)
	{
		HCU01_CheckSum = 255;
	}

	if (RetHCU01_Counter != 0)
	{
		HCU01_Counter = 15;
	}

	if (RetHCU01_EmergShutDown != 0)
	{
		HCU01_EmergShutDown = 1;
	}

	if (RetHCU01_EnaMCU != 0)
	{
		HCU01_EnaMCU = 1;
	}

	if (RetHCU01_McuModeCmd != 0)
	{
		HCU01_McuModeCmd = 3;
	}

	if (RetHCU01_Shift != 0)
	{
		HCU01_Shift = 3;
	}

	if (RetHCU01_SpeedCmd != 0)
	{
		HCU01_SpeedCmd = 16383;
	}	
	
	if (RetHCU01_TqCmd != 0)
	{
		HCU01_TqCmd = 2047;
	}	
	
	if (RetHCU02_Poweroff != 0)
	{
		HCU02_Poweroff = 1;
	}	
	
	
	
	if (MCU03_NRF_IdcSamp == 0)
    {
        MCU03_NRF_IdcSamp = 1;
    }
    else
    {
        MCU03_NRF_IdcSamp = 0;
    }
	/*  -------------------------------------- Data Write ----------------------------------------  */
	RetMCU03_NRF_IdcSamp = Rte_Write_PPort_MCU03_NRF_IdcSamp_MCU03_NRF_IdcSamp(MCU03_NRF_IdcSamp);
    RetMCU03_NRF_EMTempSamp = Rte_Write_PPort_MCU03_NRF_EMTempSamp_MCU03_NRF_EMTempSamp(MCU03_NRF_EMTempSamp);
    RetMCU03_NRF_PhCSamp = Rte_Write_PPort_MCU03_NRF_PhCSamp_MCU03_NRF_PhCSamp(MCU03_NRF_PhCSamp);
    RetMCU03_NRF_PhBSamp = Rte_Write_PPort_MCU03_NRF_PhBSamp_MCU03_NRF_PhBSamp(MCU03_NRF_PhBSamp);
    RetMCU03_NRF_PhASamp = Rte_Write_PPort_MCU03_NRF_PhASamp_MCU03_NRF_PhASamp(MCU03_NRF_PhASamp);
    RetMCU03_NRF_Resolver = Rte_Write_PPort_MCU03_NRF_Resolver_MCU03_NRF_Resolver(MCU03_NRF_Resolver);
    RetMCU03_NRF_5VSupply = Rte_Write_PPort_MCU03_NRF_5VSupply_MCU03_NRF_5VSupply(MCU03_NRF_5VSupply);
    RetMCU03_NRF_DESAT = Rte_Write_PPort_MCU03_NRF_DESAT_MCU03_NRF_DESAT(MCU03_NRF_DESAT);
    RetMCU03_NRF_SysFailure = Rte_Write_PPort_MCU03_NRF_SysFailure_MCU03_NRF_SysFailure(MCU03_NRF_SysFailure);
    RetMCU03_NRF_COMError = Rte_Write_PPort_MCU03_NRF_COMError_MCU03_NRF_COMError(MCU03_NRF_COMError);
    RetMCU03_NRF_InvTempSamp = Rte_Write_PPort_MCU03_NRF_InvTempSamp_MCU03_NRF_InvTempSamp(MCU03_NRF_InvTempSamp);
    RetMCU03_NRF_UdcSamp = Rte_Write_PPort_MCU03_NRF_UdcSamp_MCU03_NRF_UdcSamp(MCU03_NRF_UdcSamp);
    RetMCU03_RF_PhaseImp = Rte_Write_PPort_MCU03_RF_PhaseImp_MCU03_RF_PhaseImp(MCU03_RF_PhaseImp);
    RetMCU03_RF_IntError = Rte_Write_PPort_MCU03_RF_IntError_MCU03_RF_IntError(MCU03_RF_IntError);
    RetMCU03_RF_UDCUnder = Rte_Write_PPort_MCU03_RF_UDCUnder_MCU03_RF_UDCUnder(MCU03_RF_UDCUnder);
    RetMCU03_RF_OverTempINV = Rte_Write_PPort_MCU03_RF_OverTempINV_MCU03_RF_OverTempINV(MCU03_RF_OverTempINV);
    RetMCU03_RF_ToqError = Rte_Write_PPort_MCU03_RF_ToqError_MCU03_RF_ToqError(MCU03_RF_ToqError);
    RetMCU03_RF_UdcOver = Rte_Write_PPort_MCU03_RF_UdcOver_MCU03_RF_UdcOver(MCU03_RF_UdcOver);
    RetMCU03_RF_IdcOver = Rte_Write_PPort_MCU03_RF_IdcOver_MCU03_RF_IdcOver(MCU03_RF_IdcOver);
    RetMCU03_12VSupply = Rte_Write_PPort_MCU03_12VSupply_MCU03_12VSupply(MCU03_12VSupply);
    RetMCU03_RF_PhCOver = Rte_Write_PPort_MCU03_RF_PhCOver_MCU03_RF_PhCOver(MCU03_RF_PhCOver);
    RetMCU03_RF_PhBOver = Rte_Write_PPort_MCU03_RF_PhBOver_MCU03_RF_PhBOver(MCU03_RF_PhBOver);
    RetMCU03_RF_PhAOver = Rte_Write_PPort_MCU03_RF_PhAOver_MCU03_RF_PhAOver(MCU03_RF_PhAOver);
    RetMCU03_RF_HWOverCur = Rte_Write_PPort_MCU03_RF_HWOverCur_MCU03_RF_HWOverCur(MCU03_RF_HWOverCur);
    RetMCU03_RF_MsgTO = Rte_Write_PPort_MCU03_RF_MsgTO_MCU03_RF_MsgTO(MCU03_RF_MsgTO);
    RetMCU03_RF_BusOff = Rte_Write_PPort_MCU03_RF_BusOff_MCU03_RF_BusOff(MCU03_RF_BusOff);
    RetMCU03_RF_OverSpeed = Rte_Write_PPort_MCU03_RF_OverSpeed_MCU03_RF_OverSpeed(MCU03_RF_OverSpeed);
    RetMCU03_RF_OverTempEM = Rte_Write_PPort_MCU03_RF_OverTempEM_MCU03_RF_OverTempEM(MCU03_RF_OverTempEM);
    RetMCU03_LI_UdcOver = Rte_Write_PPort_MCU03_LI_UdcOver_MCU03_LI_UdcOver(MCU03_LI_UdcOver);
    RetMCU03_LI_OverSpeed = Rte_Write_PPort_MCU03_LI_OverSpeed_MCU03_LI_OverSpeed(MCU03_LI_OverSpeed);
    RetMCU03_LI_OverTempEM = Rte_Write_PPort_MCU03_LI_OverTempEM_MCU03_LI_OverTempEM(MCU03_LI_OverTempEM);
    RetMCU03_LI_OverTempINV = Rte_Write_PPort_MCU03_LI_OverTempINV_MCU03_LI_OverTempINV(MCU03_LI_OverTempINV);
    RetMCU03_LI_UdcUnder = Rte_Write_PPort_MCU03_LI_UdcUnder_MCU03_LI_UdcUnder(MCU03_LI_UdcUnder);
    RetMCU02_MaxTor = Rte_Write_PPort_MCU02_MaxTor_MCU02_MaxTor(MCU02_MaxTor);
    RetMCU02_MinTor = Rte_Write_PPort_MCU02_MinTor_MCU02_MinTor(MCU02_MinTor);
    RetMCU02_Vdc = Rte_Write_PPort_MCU02_Vdc_MCU02_Vdc(MCU02_Vdc);
    RetMCU02_Idc = Rte_Write_PPort_MCU02_Idc_MCU02_Idc(MCU02_Idc);
    RetMCU02_IGBTTmp = Rte_Write_PPort_MCU02_IGBTTmp_MCU02_IGBTTmp(MCU02_IGBTTmp);
    RetMCU02_MotorTmp = Rte_Write_PPort_MCU02_MotorTmp_MCU02_MotorTmp(MCU02_MotorTmp);
    RetMCU01_Motorspd = Rte_Write_PPort_MCU01_Motorspd_MCU01_Motorspd(MCU01_Motorspd);
    RetMCU01_CurCntTor = Rte_Write_PPort_MCU01_CurCntTor_MCU01_CurCntTor(MCU01_CurCntTor);
    RetMCU01_ShutDownAllowed = Rte_Write_PPort_MCU01_ShutDownAllowed_MCU01_ShutDownAllowed(MCU01_ShutDownAllowed);
    RetMCU01_Counter = Rte_Write_PPort_MCU01_Counter_MCU01_Counter(MCU01_Counter);
    RetMCU01_SelfCheck = Rte_Write_PPort_MCU01_SelfCheck_MCU01_SelfCheck(MCU01_SelfCheck);
    RetMCU01_ActDischStatus = Rte_Write_PPort_MCU01_ActDischStatus_MCU01_ActDischStatus(MCU01_ActDischStatus);
    RetMCU01_Status = Rte_Write_PPort_MCU01_Status_MCU01_Status(MCU01_Status);
    RetMCU01_HighVolAllowed = Rte_Write_PPort_MCU01_HighVolAllowed_MCU01_HighVolAllowed(MCU01_HighVolAllowed);
    RetMCU01_Ready = Rte_Write_PPort_MCU01_Ready_MCU01_Ready(MCU01_Ready);
    RetMCU01_EmergShutDown = Rte_Write_PPort_MCU01_EmergShutDown_MCU01_EmergShutDown(MCU01_EmergShutDown);
    RetMCU01_CheckSum = Rte_Write_PPort_MCU01_CheckSum_MCU01_CheckSum(MCU01_CheckSum);
	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

}


#define ASW_COM_STOP_SEC_CODE  
#include "ASW_COM_MemMap.h"

/*PROTECTED REGION ID(User Logic :RE_ComSWC) ENABLED START */
/* Start of user code - Do not remove this comment */
/* End of user code - Do not remove this comment */
/*PROTECTED REGION END */ 
