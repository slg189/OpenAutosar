/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_COM
 * Description: Testcode for ASW_ETH
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        22-Mar-2021        Standardized the Banner
 * 													Update Memmap
 * 1.1             HAD1HC        13-May-2021        Update functions of DoIP
 * 1.2             HAD1HC        28-May-2021        Update Memmap															
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/
#include "Rte_ASW_ETH.h"

/* UDP socket with Payload size 508 bytes */
#define UDP_PAYLOAD	508

/**********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
/*==========================================================================
 *  UDP socket with Payload size 508 bytes
 ==========================================================================*/
#define ASW_ETH_START_SEC_VAR_INIT_32
#include "ASW_ETH_MemMap.h"
uint32 readSystemSignalUdpRx = 0;
volatile uint32 writeSystemSignalUdpTx = 0x11223344;
#define ASW_ETH_STOP_SEC_VAR_INIT_32
#include "ASW_ETH_MemMap.h"

/*==========================================================================
 *  TCP socket with Payload size
 *  - Tx: 4 bytes
 *  - Rx: 4 bytes
 ==========================================================================*/
#define ASW_ETH_START_SEC_VAR_INIT_32
#include "ASW_ETH_MemMap.h"
volatile uint32 writeSystemSignalTcpTx=0x41424344;
volatile uint32 readSystemSignalTcpRx = 0;
#define ASW_ETH_STOP_SEC_VAR_INIT_32
#include "ASW_ETH_MemMap.h"

#define ASW_ETH_START_SEC_VAR_INIT_8
#include "ASW_ETH_MemMap.h"
volatile uint8 sendUDP = 0;
volatile uint8 sendTCP=0;
#define ASW_ETH_STOP_SEC_VAR_INIT_8
#include "ASW_ETH_MemMap.h"

/*==========================================================================
 *  TCP socket with Payload size
 *        Signal_1    Signal_2
 *  - Tx: 4 bytes   | 4 bytes
 *  - Rx: 4 bytes   | 4 bytes
 ==========================================================================*/
#define ASW_ETH_START_SEC_VAR_INIT_8
#include "ASW_ETH_MemMap.h"
volatile uint8 sendTCP_1=0;
uint8 activeSwitch = 0;
#define ASW_ETH_STOP_SEC_VAR_INIT_8
#include "ASW_ETH_MemMap.h"

/**********************************************************************************************************************
 * Function implementation
 **********************************************************************************************************************/
#define ASW_ETH_START_SEC_CODE
#include "ASW_ETH_MemMap.h"
/***************************************************************************//*!
 *@brief		RE_ETH_SWC
 *
 *@param[in]  void
 *
 *@return	 void
 *
 *@details	This function is used to keep track the Eth data
 *
 *@note
 *
* *******************************************************************************/
FUNC (void, ETH_SWC_CODE) RE_ETH_SWC // return value & FctID 
(
		void
)
{
	/* Local Data Declaration */
	static uint8 isFirstCall_u8=1;
	uint32 cnt;
	Std_ReturnType retWrite3;
	Std_ReturnType retWrite4;

	/*
	 * Prepare the Tx buffer for UDP socket with payload size 'UDP_PAYLOAD'
	 * */


	/*PROTECTED REGION ID(UserVariables :RE_ETH_SWC) ENABLED START */
	/* Start of user variable defintions - Do not remove this comment  */
	/* End of user variable defintions - Do not remove this comment  */
	/*PROTECTED REGION END */

	/*  -------------------------------------- Data Read -----------------------------------------  */
	/*
	 * Receive UDP socket with big payload size
	 * */
		readSystemSignalUdpRx   = (uint32)Rte_IRead_RE_ETH_SWC_RPort_SystemSignal_UDP_RX_RPort_SystemSignal_UDP_RX();
	/*
	 * Receive normal TCP socket
	 * */
		readSystemSignalTcpRx   = Rte_IRead_RE_ETH_SWC_RPort_SystemSignal_TCP_RX_RPort_SystemSignal_TCP_RX();

	/*  -------------------------------------- Server Call Point  --------------------------------  */

	/*  -------------------------------------- CDATA ---------------------------------------------  */

	/*  -------------------------------------- Data Write ----------------------------------------  */
	/*
	 * Transmit UDP socket with big payload size
	 * */
	if (sendUDP==1)
	{
		retWrite3 = Rte_Write_ASW_ETH_PPort_SystemSignal_UDP_TX_PPort_SystemSignal_UDP_TX(writeSystemSignalUdpTx);
		sendUDP=0;
	}
	/*
	 * Transmit normal TCP socket
	 * */
	if (sendTCP==1)
	{
		retWrite4 = Rte_Write_ASW_ETH_PPort_SystemSignal_TCP_TX_PPort_SystemSignal_TCP_TX(writeSystemSignalTcpTx);
		sendTCP = 0;
	}

	/*  -------------------------------------- Trigger Interface ---------------------------------  */

	/*  -------------------------------------- Mode Management -----------------------------------  */

	if (activeSwitch == 1)
	{
		Rte_Switch_PPort_DoIPActivationLineSwitchNotification_currentDoIPActivationLineStatus(DOIP_ACTIVATION_LINE_ACTIVE);
		activeSwitch = 0;
	}
	/*  -------------------------------------- Port Handling -------------------------------------  */

	/*  -------------------------------------- Exclusive Area ------------------------------------  */

	/*  -------------------------------------- Multiple Instantiation ----------------------------  */

	/*PROTECTED REGION ID(User Logic :RE_ETH_SWC) ENABLED START */
	/* Start of user code - Do not remove this comment */
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */

}
#define ASW_ETH_STOP_SEC_CODE
#include "ASW_ETH_MemMap.h"

/***************************************************************************//*!
 *@brief		RE_ETH_CallbackGetPowerMode_func
 *
 *@param[in]  DoIP_PowerStateType
 *
 *@return	 Std_ReturnType
 *
 *@details	This function is used to provide the power mode to DoIP
 *
 *@note
 *
* *******************************************************************************/
#define ASW_ETH_START_SEC_CODE
#include "ASW_ETH_MemMap.h"
FUNC (Std_ReturnType, AUTOMATIC) RE_ETH_CallbackGetPowerMode_func/* return value & FctID */
(
		P2VAR(DoIP_PowerStateType, AUTOMATIC, RTE_APPL_DATA) PowerStateReady
)
{

	/* Local Data Declaration */

	/*PROTECTED REGION ID(UserVariables :RE_ETH_CallbackGetPowerMode_func) ENABLED START */
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

	/*PROTECTED REGION ID(User Logic :RE_ETH_CallbackGetPowerMode_func) ENABLED START */
	/* Start of user code - Do not remove this comment */
	*PowerStateReady = DOIP_READY;
	/* End of user code - Do not remove this comment */
	/*PROTECTED REGION END */
	return retValue;
}
#define ASW_ETH_STOP_SEC_CODE
#include "ASW_ETH_MemMap.h"

 /**********************************************************************************************************************
  * EOFs
  **********************************************************************************************************************/
