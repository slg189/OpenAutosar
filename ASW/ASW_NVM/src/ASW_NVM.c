/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_NVM
 * Description: Testcode for ASW_NVM
 * Version         Author        Date               Update information
 * 1.0             AGT1HC        14-Jun-2019        Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 * 1.3             HAD1HC        15-June-2021       Update test code of block 
 * 													corruption
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/
#include "Rte_ASW_NVM.h"
#include "NvM.h"
#include "ASW_NVM.h"
#include "NVM_types.h"
#include "Fee.h"
#include "TimingCalculation.h"
#include "rba_FeeFs1_Prv.h"

#define ASW_NVM_START_SEC_VAR_INIT_8
#include "ASW_NVM_MemMap.h"
uint8 TEST_DATA_NVM_NATIVE_1024_1[SIZE_BLOCK1]={0};
uint8 TEST_DATA_NVM_NATIVE_1024_2[SIZE_BLOCK2]={0};
static uint8 CorruptData = 0xCA;
static NvM_RequestResultType NvM_ErrorStatus[2] = {0,0};
static uint8 NvM_WriteFlag = 1;
uint8 CounterWriteBlock=0;
#define ASW_NVM_STOP_SEC_VAR_INIT_8
#include "ASW_NVM_MemMap.h"

#define ASW_NVM_START_SEC_VAR_INIT_32
#include "ASW_NVM_MemMap.h"
static uint32 modify_addr1 = 0x200;
static uint32 modify_addr2 = 0x300;
static uint32 NvM_Count_PENDING = 0;
#if (OS_COUNTER_IS_USED == TRUE)
static uint32 NvM_ElapsedTime = 0;
#else
static float32 NvM_ElapsedTime = 0;
#endif
uint32 MngData_Offset = 0;
#define ASW_NVM_STOP_SEC_VAR_INIT_32
#include "ASW_NVM_MemMap.h"

#define ASW_NVM_START_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_NVM_MemMap.h"
NvM_Rb_StatusType status_NvM_test=0;
NvM_Test_t NvM_Test = SWC_ZERO;
#define ASW_NVM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "ASW_NVM_MemMap.h"

#define ASW_NVM_START_SEC_VAR_INIT_16
#include "ASW_NVM_MemMap.h"
static uint8 InvalidData[1400]      = {0};
static uint8 MngData[MngData_Size]  = {0};
#define ASW_NVM_STOP_SEC_VAR_INIT_16
#include "ASW_NVM_MemMap.h"

#define ASW_NVM_START_SEC_CODE
#include "ASW_NVM_MemMap.h"

uint8 NvM_SwcICopyDataToRam(uint8 * dest_addr, uint8 * src_addr, uint32 size);
FUNC (void, ASW_NVM_CODE) RE_ASW_NVM_func/* return value & FctID */
(
		void
)
{
	boolean retVal 		= E_NOT_OK;

	switch (NvM_Test)
	{
	case ASW_WRITE_BLOCKNATIVE_1024_1:
		/*Update 1024 bytes of value 0x7E to test block*/
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,SIZE_BLOCK1,0x7E);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1);
		NvM_Test = SWC_ZERO;
		break;
	case ASW_READ_BLOCKNATIVE_1024_1:
		UpdatePIMWithValue(TEST_DATA_NVM_NATIVE_1024_1,SIZE_BLOCK1,0x00);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_ReadBlock(TEST_DATA_NVM_NATIVE_1024_1);
        NvM_Test = SWC_ZERO;
		break;
	case ASW_WRITE_BLOCKNATIVE_1024_2:
		/*Update 1024 bytes of value 0xCE to test block*/
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2,SIZE_BLOCK2,0xCE);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2);
		NvM_Test = SWC_ZERO;
		break;
	case ASW_READ_BLOCKNATIVE_1024_2:
		UpdatePIMWithValue(TEST_DATA_NVM_NATIVE_1024_2,SIZE_BLOCK2,0x00);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_ReadBlock(TEST_DATA_NVM_NATIVE_1024_2);
		NvM_Test = SWC_ZERO;
		break;
	case ASW_WRITE_ALL_BLOCK_1024:
		/*Update 1024 bytes of value 0xAB to test block*/
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,SIZE_BLOCK1,0xAB);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1);

		/*Update 1024 bytes of value 0xAB to test block*/
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2,SIZE_BLOCK2,0xBC);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2);
		NvM_Test = SWC_ZERO;
		break;
	case ASW_READ_ALL_BLOCK_1024:
		/*Erase PIM before read*/
        UpdatePIMWithValue(TEST_DATA_NVM_NATIVE_1024_1,1024u,0x00);
        Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_ReadBlock(TEST_DATA_NVM_NATIVE_1024_1);
		/*Erase PIM before read*/
        UpdatePIMWithValue(TEST_DATA_NVM_NATIVE_1024_2,1024u,0x00);
        Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_ReadBlock(TEST_DATA_NVM_NATIVE_1024_2);
		NvM_Test = SWC_ZERO;

		break;

	case ASW_CORRUPT_BLOCKNATIVE_1024_1:
		retVal = NvM_SwcIModifyNvBlock(modify_addr1);
		if(retVal == E_OK){
			NvM_Test = SWC_ZERO;
		}
		break;
	case ASW_CORRUPT_BLOCKNATIVE_1024_2:
		retVal = NvM_SwcIModifyNvBlock(modify_addr2);
		if(retVal == E_OK){
			NvM_Test = SWC_ZERO;
		}
		break;
	case SWITCH_PAGE:
		NvM_Rb_GetStatus(&status_NvM_test);
		if ( (status_NvM_test == NVM_RB_STATUS_BUSY ) || (status_NvM_test == MEMIF_BUSY))
		{

		}
		else
		{
			CounterWriteBlock++;
			UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,SIZE_BLOCK1,CounterWriteBlock);
			Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1);
			if(CounterWriteBlock==NUMBER_BLOCK_TO_WRITE_DATA_IN_PAGE1)
			{

			}
			else if (CounterWriteBlock ==NUMBER_BLOCK_TO_WRITE_DATA_IN_PAGE2)
			{
				NvM_Test = SWC_ZERO;
			}
		}

		break;
	case ASW_WRITE_ALL_BLKS_REQ:
		/*Update PIM with new value*/
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,1024,0x33);
		UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2,1024,0x34);
		/*Change block status*/
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_SetRamBlockStatus(TRUE);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_SetRamBlockStatus(TRUE);
		shutdown_b=1;

		NvM_Test = SWC_ZERO;
		break;
	case ERASE_BANK:
		if( Fls_GetJobResult() == MEMIF_JOB_OK)
		{
		Fls_Erase(FEE_PHYS_SEC_START0,FEE_PHYS_SEC_END1 - FEE_PHYS_SEC_START0 + 1);
		NvM_Test = SWC_ZERO;
		}
		break;
	case ERASE_SECTOR_0:
		if( Fls_GetJobResult() == MEMIF_JOB_OK)
		{
		Fls_Erase(FEE_PHYS_SEC_START0,FEE_PHYS_SEC_END0 - FEE_PHYS_SEC_START0 + 1);
		NvM_Test = SWC_ZERO;
		}

		break;
	case ERASE_SECTOR_1:
		if( Fls_GetJobResult() == MEMIF_JOB_OK)
		{
		Fls_Erase(FEE_PHYS_SEC_START1,FEE_PHYS_SEC_END1 - FEE_PHYS_SEC_START1 + 1);
		NvM_Test = SWC_ZERO;
		}
		break;

	case ASW_FILL_SECTOR_WITH_FULL_DATA:
		/*Write native block 1 and native block 2 until the first sector is full*/
		if (Fee_LLCheckReorganizationNeed(FEE_REQUIRED_FREE_SPACE_BEFORE_HARD_SR ,SIZE_BLOCK1 + 14) == 5)
		{
			NvM_Test = ASW_WAIT_FOR_BLOCK_REQUEST_FINISHED;
		}
		else
		{
			Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_GetErrorStatus(&NvM_ErrorStatus[0]);
			Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_GetErrorStatus(&NvM_ErrorStatus[1]);
			if (NvM_ErrorStatus[0] != NVM_REQ_PENDING)
			{
				if (NvM_WriteFlag == 1)
				{
					UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,SIZE_BLOCK1,CounterWriteBlock++);
					Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1);
					NvM_WriteFlag = 2;
				}
				else if (NvM_WriteFlag == 2)
				{
					NvM_WriteFlag = 3;
				}
			}
			if (NvM_ErrorStatus[1] != NVM_REQ_PENDING)
			{
				if (NvM_WriteFlag == 3)
				{
					UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2,SIZE_BLOCK2,CounterWriteBlock++);
					Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_2);
					NvM_WriteFlag = 4;
				}
				else if (NvM_WriteFlag == 4)
				{
					NvM_WriteFlag = 1;
				}
			}
		}
		break;

	case ASW_WAIT_FOR_BLOCK_REQUEST_FINISHED:
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_GetErrorStatus(&NvM_ErrorStatus[0]);
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_2_GetErrorStatus(&NvM_ErrorStatus[1]);
		if ((NvM_ErrorStatus[0] == NVM_REQ_OK) && (NvM_ErrorStatus[1] == NVM_REQ_OK))
		{
			NvM_Test = SWC_ZERO;
		}
		break;

	case ASW_WRITE_BLOCK_AND_CALCULATE_TIME:
		/*Write the native block and calculate the time to write this blocks*/
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_GetErrorStatus(&NvM_ErrorStatus[0]);

		if((NvM_ErrorStatus[0] != NVM_REQ_PENDING) && (NvM_Count_PENDING == 0))
		{
			UpdatePIMWithValue(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1,SIZE_BLOCK1,CounterWriteBlock++);
			IC_SetCurrentTime();
			Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_WriteBlock(Rte_CPim_ASW_NVM_ASW_NVM_BlockNative_1024_1);
		}
		else if((NvM_ErrorStatus[0] != NVM_REQ_PENDING) && (NvM_Count_PENDING > 0))
		{
			NvM_ElapsedTime = IC_GetElapsedTime();
			NvM_Test = SWC_ZERO;
		}
		else if (NvM_ErrorStatus[0] == NVM_REQ_PENDING)
		{
			NvM_Count_PENDING++;
		}
		else
		{
			/*do nothing*/
		}
		break;
	case ASW_READ_BLOCK_AND_CALCULATE_TIME:
		/*Read the native block and calculate the time to read this blocks*/
		Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_GetErrorStatus(&NvM_ErrorStatus[0]);
		if((NvM_ErrorStatus[0] != NVM_REQ_PENDING) && (NvM_Count_PENDING == 0))
		{
			UpdatePIMWithValue(TEST_DATA_NVM_NATIVE_1024_1,SIZE_BLOCK1,0x00);
			IC_SetCurrentTime();
			Rte_Call_RPort_ASW_NVM_BlockNative_1024_1_ReadBlock(TEST_DATA_NVM_NATIVE_1024_1);
		}
		else if((NvM_ErrorStatus[0] != NVM_REQ_PENDING) && (NvM_Count_PENDING > 0))
		{
			NvM_ElapsedTime = IC_GetElapsedTime();
			NvM_Test = SWC_ZERO;
		}
		else if (NvM_ErrorStatus[0] == NVM_REQ_PENDING)
		{
			NvM_Count_PENDING++;
		}
		else
		{
			/*do nothing*/
		}

		break;
	default:
		break;

	}
}
#define ASW_NVM_STOP_SEC_CODE
#include "ASW_NVM_MemMap.h"
#define ASW_NVM_START_SEC_CODE
#include "ASW_NVM_MemMap.h"
/***************************************************************************************************
 Function name    : UpdatePIMWithValue
 Syntax           : void  UpdatePIMWithValue(uint8 *src, uint32 length, uint8 Value)
 Description      : Update PIM's value
 Parameter        : uint8 *src, uint32 length, uint8 Value
 Return value     : none
 ***************************************************************************************************/

void UpdatePIMWithValue(uint8 *dst, uint32 length, uint8 Value)
{
	uint32 Cnt = 0;
	for(Cnt = 0; Cnt < length ; Cnt++)
	{
		*dst = Value;
		dst++;
	}

}

#define ASW_NVM_STOP_SEC_CODE
#include "ASW_NVM_MemMap.h"
#define ASW_NVM_START_SEC_CODE
#include "ASW_NVM_MemMap.h"
/***************************************************************************************************
 Function name    : NvM_SwcIModifyNvBlock
 Syntax           : void  NvM_SwcIModifyNvBlock(uint32 addr)
 Description      : Modify data on NV Block
 Parameter        : uint32 addr
 Return value     : none
 ***************************************************************************************************/
uint8 NvM_SwcIModifyNvBlock(uint32 addr)
{
	static MDF_State_t MDF_State = MDF_NONE;
	uint8 ret_val = E_NOT_OK;
	switch(MDF_State)
	{
	case MDF_NONE:
		MDF_State = MDF_INIT_STATE;
		break;
	case MDF_INIT_STATE: /*copy data flash to buffer ram*/;
		NvM_SwcICopyDataToRam(InvalidData, (uint8*)(FLS_17_DMU_BASE_ADDRESS + MngData_Offset), sizeof(InvalidData));
		NvM_SwcICopyDataToRam(MngData, (uint8*)FLS_17_DMU_BASE_ADDRESS, MngData_Offset);
		MDF_State = MDF_ERASE_BANK;
		break;
	case MDF_ERASE_BANK: /*erase bank 0 before write data*/
		if( Fls_GetJobResult() == MEMIF_JOB_OK)
		{
		Fls_Erase(FEE_PHYS_SEC_START0,FEE_PHYS_SEC_END1 - FEE_PHYS_SEC_START0 + 1);
		MDF_State = MDF_MODIFY_DATA;
		}
		break;
	case MDF_MODIFY_DATA:
		InvalidData[addr]= CorruptData;
		MDF_State = MDF_UPDATE_MANANGMENT_INFO;
		break;
	case MDF_UPDATE_MANANGMENT_INFO: /*write back  management data*/
		if( Fls_GetJobResult() == MEMIF_JOB_OK){
			Fls_Write((uint32)FEE_PHYS_SEC_START0, &MngData[0], MngData_Offset);
			MDF_State = MDF_UPDATE_DATA;
		}
		break;
	case MDF_UPDATE_DATA: /*write data back to bank 0*/
		if( Fls_GetJobResult() == MEMIF_JOB_OK){
			Fls_Write((uint32)FEE_PHYS_SEC_START0 + MngData_Offset, &InvalidData[0], sizeof(InvalidData));
			MDF_State = MDF_UPDATE_COMPLETE;
		}
		break;
	case MDF_UPDATE_COMPLETE:
		if( Fls_GetJobResult() == MEMIF_JOB_OK){
			MDF_State = MDF_NONE;
			ret_val = E_OK;
		}
		break;
	default :
		break;
	}
	return ret_val ;
}

#define ASW_NVM_STOP_SEC_CODE
#include "ASW_NVM_MemMap.h"
#define ASW_NVM_START_SEC_CODE
#include "ASW_NVM_MemMap.h"
/***************************************************************************************************
 Function name    : NvM_SwcICopyDataToRam
 Syntax           : static uint8 NvM_SwcICopyDataToRam(uint8 * dest_addr, uint8 * src_addr, size)
 Description      : Copy data from ROM to RAM for testing purpose
 Parameter        : uint8 * dest_addr, uint8 * src_addr, size
 Return value     : none
 ***************************************************************************************************/
uint8 NvM_SwcICopyDataToRam(uint8 * dest_addr, uint8 * src_addr, uint32 size)
{
	uint8 retVal = E_NOT_OK;
	if((dest_addr == NULL_PTR) || (NULL_PTR == src_addr) || (0u == size))
	{
		return retVal;
	}
	while(size > 0u)
	{
		*dest_addr = *src_addr;
		dest_addr++;
		src_addr++;
		size--;
	}
	retVal = E_OK;

	return retVal;
}
#define ASW_NVM_STOP_SEC_CODE
#include "ASW_NVM_MemMap.h"
