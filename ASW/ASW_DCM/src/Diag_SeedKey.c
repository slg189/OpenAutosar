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
 * 1.0             HAD1HC        7-Nov-2018         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        04-Mar-2021        Update functions for new 
 * 													DIAG requirements
 * 1.3             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Std_Types.h"
#include "rte_Type.h"
#include "rte.h"
#include "Dcm_Lcfg_DspUds.h"

#define SWC_COMPARE_KEY_FAILED 11

#define ASW_DCM_START_SEC_CONST_8
#include "ASW_DCM_MemMap.h"
/* Seed values for Service 0x27 */
const uint8 DcmSeed[4] = {0x10,0x20,0x30,0x40};
/* Key values for Service 0x27 */
const uint8 DcmKey_L1[4] = {0x11,0x21,0x31,0x41};
const uint8 DcmKey_L2[4] = {0x12,0x22,0x32,0x42};
const uint8 DcmKey_L3[4] = {0x13,0x23,0x33,0x43};
const uint8 DcmKey_L5[4] = {0x14,0x24,0x34,0x44};
const uint8 DcmKey_L49[4] = {0x15,0x25,0x35,0x45};
#define ASW_DCM_STOP_SEC_CONST_8
#include "ASW_DCM_MemMap.h"

#define ASW_DCM_START_SEC_CODE
#include "ASW_DCM_MemMap.h"
FUNC(Std_ReturnType, ASW_DCM_CODE) CompareKey_L1(VAR(uint32,AUTOMATIC) KeyLen_32,P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) Key,VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
{

	Std_ReturnType retValue = RTE_E_OK;

	if ( ( Key[ 0 ] != DcmKey_L1[ 0 ] ) || ( Key[ 1 ] != DcmKey_L1[ 1 ] )||( Key[ 2 ] != DcmKey_L1[ 2 ] ) || ( Key[ 3 ] != DcmKey_L1[ 3 ] ) )
	{
		retValue = SWC_COMPARE_KEY_FAILED;
	}
	else{ /* Do nothing. Key is correct. */ }

	return ( retValue );
}

FUNC(Std_ReturnType, ASW_DCM_CODE) CompareKey_L2(VAR(uint32,AUTOMATIC) KeyLen_32,P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) Key,VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
{

	Std_ReturnType retValue = RTE_E_OK;

	if ( ( Key[ 0 ] != DcmKey_L2[ 0 ] ) || ( Key[ 1 ] != DcmKey_L2[ 1 ] )||( Key[ 2 ] != DcmKey_L2[ 2 ] ) || ( Key[ 3 ] != DcmKey_L2[ 3 ] ) )
	{
		retValue = SWC_COMPARE_KEY_FAILED;
	}
	else{ /* Do nothing. Key is correct. */ }

	return ( retValue );
}

FUNC(Std_ReturnType, ASW_DCM_CODE) CompareKey_L3(VAR(uint32,AUTOMATIC) KeyLen_32,P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) Key,VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
{

	Std_ReturnType retValue = RTE_E_OK;

	if ( ( Key[ 0 ] != DcmKey_L3[ 0 ] ) || ( Key[ 1 ] != DcmKey_L3[ 1 ] )||( Key[ 2 ] != DcmKey_L3[ 2 ] ) || ( Key[ 3 ] != DcmKey_L3[ 3 ] ) )
	{
		retValue = SWC_COMPARE_KEY_FAILED;
	}
	else{ /* Do nothing. Key is correct. */ }

	return ( retValue );
}

FUNC(Std_ReturnType, ASW_DCM_CODE) CompareKey_L5(VAR(uint32,AUTOMATIC) KeyLen_32,P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) Key,VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
{

	Std_ReturnType retValue = RTE_E_OK;

	if ( ( Key[ 0 ] != DcmKey_L5[ 0 ] ) || ( Key[ 1 ] != DcmKey_L5[ 1 ] )||( Key[ 2 ] != DcmKey_L5[ 2 ] ) || ( Key[ 3 ] != DcmKey_L5[ 3 ] ) )
	{
		retValue = SWC_COMPARE_KEY_FAILED;
	}
	else{ /* Do nothing. Key is correct. */ }

	return ( retValue );
}

FUNC(Std_ReturnType, ASW_DCM_CODE) CompareKey_L49(VAR(uint32,AUTOMATIC) KeyLen_32,P2CONST(uint8, AUTOMATIC, RTE_APPL_DATA) Key,VAR(Dcm_OpStatusType, AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
{

	Std_ReturnType retValue = RTE_E_OK;

	if ( ( Key[ 0 ] != DcmKey_L49[ 0 ] ) || ( Key[ 1 ] != DcmKey_L49[ 1 ] )||( Key[ 2 ] != DcmKey_L49[ 2 ] ) || ( Key[ 3 ] != DcmKey_L49[ 3 ] ) )
	{
		retValue = SWC_COMPARE_KEY_FAILED;
	}
	else{ /* Do nothing. Key is correct. */ }

	return ( retValue );
}


FUNC(Std_ReturnType, ASW_DCM_CODE) GetSeed_L1(VAR(Dcm_SecLevelType,AUTOMATIC) SecLevel_u8,VAR(uint32,AUTOMATIC) Seedlen_u32,VAR(uint32,AUTOMATIC) AccDataRecsize_u32,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) SecurityAccessDataRecord,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) Seed,VAR(Dcm_OpStatusType,AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
		{
	Std_ReturnType retValue = RTE_E_OK;
	*Seed = DcmSeed[0];
	*(Seed+1) = DcmSeed[1];
	*(Seed+2) = DcmSeed[2];
	*(Seed+3) = DcmSeed[3];
	return (retValue);
}

FUNC(Std_ReturnType, ASW_DCM_CODE) GetSeed_L2(VAR(Dcm_SecLevelType,AUTOMATIC) SecLevel_u8,VAR(uint32,AUTOMATIC) Seedlen_u32,VAR(uint32,AUTOMATIC) AccDataRecsize_u32,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) SecurityAccessDataRecord,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) Seed,VAR(Dcm_OpStatusType,AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
		{
	Std_ReturnType retValue = RTE_E_OK;
	*Seed = DcmSeed[0];
	*(Seed+1) = DcmSeed[1];
	*(Seed+2) = DcmSeed[2];
	*(Seed+3) = DcmSeed[3];
	return (retValue);
}

FUNC(Std_ReturnType, ASW_DCM_CODE) GetSeed_L3(VAR(Dcm_SecLevelType,AUTOMATIC) SecLevel_u8,VAR(uint32,AUTOMATIC) Seedlen_u32,VAR(uint32,AUTOMATIC) AccDataRecsize_u32,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) SecurityAccessDataRecord,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) Seed,VAR(Dcm_OpStatusType,AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
		{
	Std_ReturnType retValue = RTE_E_OK;
	*Seed = DcmSeed[0];
	*(Seed+1) = DcmSeed[1];
	*(Seed+2) = DcmSeed[2];
	*(Seed+3) = DcmSeed[3];
	return (retValue);
}

FUNC(Std_ReturnType, ASW_DCM_CODE) GetSeed_L5(VAR(Dcm_SecLevelType,AUTOMATIC) SecLevel_u8,VAR(uint32,AUTOMATIC) Seedlen_u32,VAR(uint32,AUTOMATIC) AccDataRecsize_u32,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) SecurityAccessDataRecord,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) Seed,VAR(Dcm_OpStatusType,AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
		{
	Std_ReturnType retValue = RTE_E_OK;
	*Seed = DcmSeed[0];
	*(Seed+1) = DcmSeed[1];
	*(Seed+2) = DcmSeed[2];
	*(Seed+3) = DcmSeed[3];
	return (retValue);
}


FUNC(Std_ReturnType, ASW_DCM_CODE) GetSeed_L49(VAR(Dcm_SecLevelType,AUTOMATIC) SecLevel_u8,VAR(uint32,AUTOMATIC) Seedlen_u32,VAR(uint32,AUTOMATIC) AccDataRecsize_u32,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) SecurityAccessDataRecord,P2VAR(uint8,AUTOMATIC,DCM_INTERN_DATA) Seed,VAR(Dcm_OpStatusType,AUTOMATIC) OpStatus,P2VAR(Dcm_NegativeResponseCodeType,AUTOMATIC,DCM_INTERN_DATA) ErrorCode)
		{
	Std_ReturnType retValue = RTE_E_OK;
	*Seed = DcmSeed[0];
	*(Seed+1) = DcmSeed[1];
	*(Seed+2) = DcmSeed[2];
	*(Seed+3) = DcmSeed[3];
	return (retValue);
}



#define ASW_DCM_STOP_SEC_CODE
#include "ASW_DCM_MemMap.h"