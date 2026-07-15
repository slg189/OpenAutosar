/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_NM
 * Description: Testcode for ASW_NM
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        14-Jun-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_ASW_NM.h"
#include "ComStack_Types.h"
#include "NmStack_Types.h"
#include "Rte_ComM_Type.h"
#include "Nm.h"
#include "ASW_NM.h"
#include "EcuM.h"

#define FULFILLED 1

typedef enum{
	NO_REASON =0,
	KL15_WU,
	NMMSG_WU,
} WU_SRC_t;

#define ASW_NM_START_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_NM_MemMap.h"
static WU_SRC_t wakeup_reason = NO_REASON;
Nm_Test_t Nm_Test[NM_NUMBER_OF_CHANNELS] ={0};
#define ASW_NM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_NM_MemMap.h"

#define ASW_NM_START_SEC_VAR_INIT_8
#include "ASW_NM_MemMap.h"
static uint8 checkNMreceived =FALSE;
#define ASW_NM_STOP_SEC_VAR_INIT_8
#include "ASW_NM_MemMap.h"

#define ASW_NM_START_SEC_CODE
#include "ASW_NM_MemMap.h"
FUNC (void, ASW_NM_CODE) RE_ASW_NM_func/* return value & FctID */
(
		void
)
{
	/* Local Data Declaration */
    uint8 index_chanel =0;
	static volatile uint8 KL15_State = FALSE;	// The user shall implement the API to get KL15 state from CDD.
	static uint8 isFullComm = TRUE; //init value is full comm because wakeup will request network at startup
	static uint8 is_init=2;	//only check for the first 2 cycle to make sure NM message is received

	uint8 NetworkHandle =0; //only CANNM available

	//Set wakeup reason for byte2 of NM message
	if(is_init > 0)
	{
		if (EcuM_GetValidatedWakeupEvents() == EcuMConf_EcuMWakeupSource_ECUM_WKSOURCE_KL15)
		{
			wakeup_reason = KL15_WU;
			KL15_State = TRUE;
			Nm_Test[NetworkHandle].nm_tx_data[1]= 0x01;	//set the byte2 to 0x01 for local wakeup
		}
		else
		{		
			if (Nm_Test[NetworkHandle].IsNmReceived)
			{
				wakeup_reason = NMMSG_WU;
			}
			else
			{
				wakeup_reason = NO_REASON;
			}
			Nm_Test[NetworkHandle].nm_tx_data[1]= 0x02;	//set the byte2 to 0x02 for NM wakeup
		}
		is_init--;
	}
	else
	{
		if ((wakeup_reason == NO_REASON)&(Nm_Test[NetworkHandle].nm_CurrentState == NM_STATE_REPEAT_MESSAGE))
		{
			shutdown_b = 1;		//if wakeup by non NM msg, then, go to shut down
		}
	}
	// KL15 state shall be checked everytime
	for (index_chanel=0;index_chanel<NM_NUMBER_OF_CHANNELS;index_chanel++)
	{
		if ((Nm_Test[index_chanel].nm_CurrentState != NM_STATE_BUS_SLEEP) & (Nm_Test[index_chanel].nm_CurrentState != NM_STATE_UNINIT))
		{
			if ((KL15_State) & (!isFullComm))
			{
				Nm_Test[index_chanel].request_b=1;
			}
			else if ((!KL15_State) & (isFullComm))	
			{
				Nm_Test[index_chanel].release_b=1;
			}
			else
			{
					//do nothing
			}
		}
	}


	for(index_chanel=0;index_chanel<NM_NUMBER_OF_CHANNELS;index_chanel++)
	{
		if(Nm_Test[index_chanel].request_b)
		{
			Rte_Write_PP_BswMArbitration_BswM_MRP_Network_uint8(RTE_MODE_ComMMode_COMM_FULL_COMMUNICATION);
			isFullComm = TRUE;
			Nm_Test[index_chanel].request_b=0;
		}

        if(Nm_Test[index_chanel].release_b)
        {
            Rte_Write_PP_BswMArbitration_BswM_MRP_Network_uint8(RTE_MODE_ComMMode_COMM_NO_COMMUNICATION);
			isFullComm = FALSE;
            Nm_Test[index_chanel].release_b=0;
        }

		if(Nm_Test[index_chanel].IsNmReceived)
		{
            Nm_GetUserData(index_chanel,(uint8*)(Nm_Test[index_chanel].nm_rx_data));
            //Nm_Test[index_chanel].IsNmReceived = 0;
		}
	}
}
#define ASW_NM_STOP_SEC_CODE
#include "ASW_NM_MemMap.h"
#define ASW_NM_START_SEC_CODE
#include "ASW_NM_MemMap.h"
extern FUNC(void, NM_CODE) Nm_RxIndicationCallback(CONST(NetworkHandleType, AUTOMATIC)   NetworkHandle )
{
	if (NetworkHandle < NM_NUMBER_OF_CHANNELS)
	{
		Nm_Test[NetworkHandle].IsNmReceived = TRUE;
	}
}
#define ASW_NM_STOP_SEC_CODE
#include "ASW_NM_MemMap.h"
#define ASW_NM_START_SEC_CODE
#include "ASW_NM_MemMap.h"
FUNC(void, NM_CODE) Nm_StateChangeIndication(CONST(NetworkHandleType, AUTOMATIC)  NetworkHandle,
                                               CONST(Nm_StateType, AUTOMATIC)   nmPreviousState,
                                               CONST(Nm_StateType, AUTOMATIC)   nmCurrentState)
{

	if (NetworkHandle<NM_NUMBER_OF_CHANNELS)
	{
		Nm_Test[NetworkHandle].nm_PreviousState=nmPreviousState;
		Nm_Test[NetworkHandle].nm_CurrentState=nmCurrentState;
		if ((Nm_Test[NetworkHandle].nm_PreviousState == NM_STATE_PREPARE_BUS_SLEEP)&&(Nm_Test[NetworkHandle].nm_CurrentState == NM_STATE_BUS_SLEEP))
		{
			shutdown_b = 1;
		}
		if (Nm_Test[NetworkHandle].nm_CurrentState == NM_STATE_REPEAT_MESSAGE)
		{
			Nm_Test[NetworkHandle].nm_tx_data[0]=0xAA;
			Nm_SetUserData(NetworkHandle,Nm_Test[NetworkHandle].nm_tx_data);
		}

		if (Nm_Test[NetworkHandle].nm_CurrentState == NM_STATE_PREPARE_BUS_SLEEP)
		{
			Nm_Test[NetworkHandle].nm_tx_data[0]=0xCC;
			Nm_SetUserData(NetworkHandle,Nm_Test[NetworkHandle].nm_tx_data);
		}
		if (Nm_Test[NetworkHandle].nm_CurrentState == NM_STATE_NORMAL_OPERATION)
		{
			Nm_Test[NetworkHandle].nm_tx_data[0]=0xBB;
			Nm_SetUserData(NetworkHandle,Nm_Test[NetworkHandle].nm_tx_data);
		}

	}
	else
	{
		/*do nothing*/
	}
}
#define ASW_NM_STOP_SEC_CODE
#include "ASW_NM_MemMap.h"
#define ASW_NM_START_SEC_CODE
#include "ASW_NM_MemMap.h"
extern FUNC(void, NM_CODE) TestNm_RemoteSleepCancellation(CONST(NetworkHandleType, AUTOMATIC)   NetworkHandle)
{

}
#define ASW_NM_STOP_SEC_CODE
#include "ASW_NM_MemMap.h"
#define ASW_NM_START_SEC_CODE
#include "ASW_NM_MemMap.h"
extern FUNC(void, NM_CODE) TestNm_RemoteSleepIndication(CONST(NetworkHandleType, AUTOMATIC)   NetworkHandle)
{

}
#define ASW_NM_STOP_SEC_CODE
#include "ASW_NM_MemMap.h"
