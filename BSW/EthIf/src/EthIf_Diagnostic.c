

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

#include "EthIf_Cfg_SchM.h"
#include "EthIf_Prv.h"

/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"
/* Global variable to store the count of dropped frames due to invalid CtrlIdx/VlanId. */
uint32 EthIf_InvalidCtrlIdxDropCnt_u32   = 0UL;

/* Global variable to store the count of dropped frames due to invalid EtherType. */
uint32 EthIf_InvalidEtherTypeDropCnt_u32 = 0UL;
#define ETHIF_STOP_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
* Interface functions
 ***************************************************************************************************
*/

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 *******************************************************************************************************************************
 * \moduledescription
 * EthIf_GetAndResetMeasurementData:
 * ETHIF91011: Allows to read and reset detailed measurement data for diagnostic purposes.
 * Get all MeasurementIdx's at once is not supported. ETHIF_MEAS_ALL shall only be used to reset all MeasurementIdx's at once.
 * A NULL_PTR shall be provided for MeasurementDataPtr in this case.
 *
 * \par Service ID 0x45, Synchronous, Reentrant
 *
 * Parameter In:
 * \param MeasurementIdx              Data index of measurement data.
 * \param MeasurementResetNeeded      Flag to trigger a reset of the measurement data.
 *
 * Parameters Out:
 * \param MeasurementDataPtr          Reference to data buffer, where to copy measurement data.
 *
 * \return                  Std_ReturnType {E_OK: successful; E_NOT_OK: failed}
 *
 *******************************************************************************************************************************
 */
Std_ReturnType EthIf_GetAndResetMeasurementData( EthIf_MeasurementIdxType MeasurementIdx,
                                                 boolean MeasurementResetNeeded,
                                                 uint32 * MeasurementDataPtr )
{
    /* Local variables declaration */
    Std_ReturnType         lStdRetVal_en;

    /* Initialize local variable. */
    lStdRetVal_en = E_OK;

    /* DET Checks */
    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( ( EthIf_InitStatus_en != ETHCTRL_STATE_INIT ), ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: Parameters are invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (( FALSE == MeasurementResetNeeded ) && ( MeasurementIdx == ETHIF_MEAS_ALL )),
                                   ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (( NULL_PTR != MeasurementDataPtr ) && ( MeasurementIdx == ETHIF_MEAS_ALL )),
                                   ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (( FALSE == MeasurementResetNeeded ) && ( NULL_PTR == MeasurementDataPtr )),
                                  ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Check the measurement index provided by the input parameter. */
    switch( MeasurementIdx )
    {
        /* If the measurement index is ETHIF_MEAS_DROP_CRTLIDX, return the count and/or clear the number of dropped frames due to invalid CtrlIdx/Vlan Id. */
        case ETHIF_MEAS_DROP_CRTLIDX:
        {
            /* Enter critical section: Getting and Resetting the measurement data should be done in a critical section. The value can be incremented in a different execution context */
            /* and critical section is added here to prevent concurrency issues. If measurement data is GET and RESET and if a dropped packet is coming after the get and before the reset, */
            /*  then this dropped packet will not be counted. */
            SchM_Enter_EthIf_UpdateMeasurementData();

            /* Check if MeasurementDataPtr is not NULL_PTR. */
            if( MeasurementDataPtr != NULL_PTR )
            {
                *MeasurementDataPtr = EthIf_InvalidCtrlIdxDropCnt_u32;
            }

            /* Check if the reset needed flag is set to true. */
            if( MeasurementResetNeeded != FALSE )
            {
                /* Clear the counter if the Reset flag is set to true. */
                EthIf_InvalidCtrlIdxDropCnt_u32 = 0UL;
            }

            /* Exit critical section. */
            SchM_Exit_EthIf_UpdateMeasurementData();
        }
        break;

        /* If the measurement index is ETHIF_MEAS_DROP_ETHERTYPE, return the count and/or clear the number of dropped frames due to invalid ether type. */
        case ETHIF_MEAS_DROP_ETHERTYPE:
        {
            /* Enter critical section: Resetting the measurement data should be done in a critical section. The value can be incremented in a different execution context */
            /* and critical section is added here to prevent concurrency issues. */
            SchM_Enter_EthIf_UpdateMeasurementData();

            /* Check if MeasurementDataPtr is not NULL_PTR. */
            if( MeasurementDataPtr != NULL_PTR )
            {
                *MeasurementDataPtr = EthIf_InvalidEtherTypeDropCnt_u32;
            }

            /* Check if the reset needed flag is set to true. */
            if( MeasurementResetNeeded != FALSE )
            {
                /* Clear the counter if the Reset flag is set to true. */
                EthIf_InvalidEtherTypeDropCnt_u32 = 0UL;
            }

            /* Exit critical section. */
            SchM_Exit_EthIf_UpdateMeasurementData();
        }
        break;

        /* If the measurement index is ETHIF_MEAS_ALL, clear all measurement data counters. */
        case ETHIF_MEAS_ALL:
        {
            /* Enter critical section: Resetting the measurement data should be done in a critical section. The value can be incremented in a different execution context */
            /* and critical section is added here to prevent concurrency issues. */
            SchM_Enter_EthIf_UpdateMeasurementData();

            /* Clear counter to store dropped frames due to invalid CtrlIdx/Vlan Id. */
            EthIf_InvalidCtrlIdxDropCnt_u32 = 0UL;

            /* Clear counter to store dropped frames due to invalid ether type. */
            EthIf_InvalidEtherTypeDropCnt_u32 = 0UL;

            /* Exit critical section. */
            SchM_Exit_EthIf_UpdateMeasurementData();
        }
        break;

        default:
        {
            /* Set return value to E_NOT_OK. */
            lStdRetVal_en = E_NOT_OK;

            /* Report DET : Development Error: MeasurementIdx is invalid. */
            ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_GETANDRESET_MEAS_DATA, ETHIF_E_INV_PARAM )
        }
        break;

    }

    return (lStdRetVal_en);

}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif  /* #if(ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON) */

#endif /* ETHIF_CONFIGURED */
