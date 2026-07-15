/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

#include "rba_FeeFs1_SyncRead.h"
#include "rba_BswSrv.h"
#include "Fls.h"

/**
 *********************************************************************
 * Fee_Fls_SyncRead(): Service for synchronous data read
 *
 * This function performed synchronous read data from Data Flash.
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 *
 * \param    SourceAddress:     Relative address of the data to be read in flash memory.
 * 								This address offset will be added to the flash memory base address.
 * \param    TargetAddressPtr:  Pointer to target data buffer.
 * \param    Length:            Length in bytes to be read from Data Flash memory.
 * \return   Function success
 * \retval   E_OK:              Read command has been accepted.
 * \retval   E_NOT_OK:          Read command has not been accepted.
 * \seealso
 * \usedresources
 *********************************************************************
 */
#define BSW_START_SEC_CODE
#include "Bsw_MemMap.h"
Std_ReturnType Fee_Fls_SyncRead( Fls_AddressType SourceAddress,
                                uint8 *TargetAddressPtr,
                                Fls_LengthType Length
                               )
{

    Std_ReturnType RetVal = E_OK;

    if(TargetAddressPtr == NULL_PTR)
    {
        RetVal = E_NOT_OK;
    }else{
        rba_BswSrv_MemCopy8(TargetAddressPtr, FLS_17_DMU_BASE_ADDRESS + SourceAddress, Length);
    }

    return RetVal;
}
#define BSW_STOP_SEC_CODE
#include "Bsw_MemMap.h"