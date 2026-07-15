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

 * Project : BYD_TC397_AR422
 * Component: /ASW_StbM/ASW_StbM
 * Runnable : All Runnables in SwComponent
 *****************************************************************************
 * Tool Version: ISOLAR-A/B 9.1
 * Author: IQU1SGH
 * Date : Thu Aug 26 10:19:36 2021
 ****************************************************************************/

#include "Rte_ASW_StbM.h"
#include "Eth_17_GEthMac.h"
#include "TcpIp.h"

/*PROTECTED REGION ID(FileHeaderUserDefinedIncludes :RE_ASW_StbM_func) ENABLED START */
/* Start of user defined includes  - Do not remove this comment */
/* End of user defined includes - Do not remove this comment */
/*PROTECTED REGION END */

/*PROTECTED REGION ID(FileHeaderUserDefinedConstants :RE_ASW_StbM_func) ENABLED START */
/* Start of user defined constant definitions - Do not remove this comment */
/* End of user defined constant definitions - Do not remove this comment */
/*PROTECTED REGION END */
boolean bTransmit_EthTime = FALSE;
/*PROTECTED REGION ID(FileHeaderUserDefinedVariables :RE_ASW_StbM_func) ENABLED START */
/* Start of user variable defintions - Do not remove this comment  */
/* End of user variable defintions - Do not remove this comment  */
/*PROTECTED REGION END */
#define ASW_StbM_START_SEC_CODE                   
#include "ASW_StbM_MemMap.h"
FUNC (void, ASW_StbM_CODE) RE_ASW_StbM_func/* return value & FctID */
(
		void
)
{

	StbM_TimeStampType timeStamp3 = {0x08, 0xA, 0xA, 0x0};
	StbM_UserDataType userData4 = {0};
	Std_ReturnType syncCall2;
	Std_ReturnType syncCall3;

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :RE_ASW_StbM_func) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	if(bTransmit_EthTime == TRUE)
	{
		uint8 EthTsynMacAddr1[6] = {0x01,0x1b,0x19,0x00,0x00,0x00}; /* Defined for ptp Multicast group addresses IEEE 802.3 Ethernet */
		Eth_17_GEthMac_UpdatePhysAddrFilter(0,EthTsynMacAddr1,ETH_ADD_TO_FILTER);
		uint8 EthTsynMacAddr2[6] = {0x00U, 0x2bU, 0x67U, 0x0cU, 0x86U, 0xf2U};
		Eth_17_GEthMac_UpdatePhysAddrFilter(0,EthTsynMacAddr2,ETH_ADD_TO_FILTER);
		EthTSyn_TrcvLinkStateChg(0,ETHTRCV_LINK_STATE_ACTIVE);
		syncCall3 = Rte_Call_RP_GlobalTime_Master_StbMSynchronizedTimeBase_Eth_UpdateGlobalTime(&timeStamp3,&userData4);
		bTransmit_EthTime = FALSE;
	}
	

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :RE_ASW_StbM_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}
#define ASW_StbM_STOP_SEC_CODE  
#include "ASW_StbM_MemMap.h" 

/*PROTECTED REGION ID(FileHeaderUserDefinedFunctions :ASW_StbM) ENABLED START */
/* Start of user defined functions  - Do not remove this comment */
/* End of user defined functions - Do not remove this comment */
/*PROTECTED REGION END */

