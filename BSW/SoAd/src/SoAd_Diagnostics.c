

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "SoAd_Prv.h"

#if( SOAD_ECUC_RB_RTE_IN_USE == STD_ON)
#include "SchM_SoAd.h"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */
/****************************************************************************************************
 * local functions
 ****************************************************************************************************/
/*
 ***************************************************************************************************
 * Prototype for Static functions
 ***************************************************************************************************
 */

#define SOAD_START_SEC_VAR_INIT_32
#include "SoAd_MemMap.h"

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
/* variable to hold dropped udp packets count at SoAd level */
uint32 SoAd_RxUdpDropCnt_u32;
/* variable to hold dropped tcp packets count at SoAd level */
uint32 SoAd_RxTcpDropCnt_u32;
#endif /* SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON */

#define SOAD_STOP_SEC_VAR_INIT_32
#include "SoAd_MemMap.h"

#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
/**
 ***************************************************************************************************
 * \Function Name : SoAd_GetAndResetMeasurementData()
 *
 * \Function description
 * This function allows to read and reset detailed measurement data for diagnostic purposes
 *
 * Parameters (in):
 * \param   SoAd_MeasurementIdxType - Data index of measurement data
 * \arg     MeasurementIdx   -
 * \param   boolean
 * \arg     MeasurementResetNeeded   - Flag to trigger a reset of the measurement data
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 * \param   uint32
 * \arg     MeasurementDataPtr   - Reference to data, where to copy measurement data
 * Return value: Std_ReturnType - E_OK : successful
 *                                E_NOT_OK : failed
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_GetAndResetMeasurementData(SoAd_MeasurementIdxType MeasurementIdx, \
                                               boolean                 MeasurementResetNeeded, \
                                               uint32 *                MeasurementDataPtr)
{
    /* variable to hold the return value*/
    Std_ReturnType lRetVal_u8;

    lRetVal_u8 = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Report the DET Error if SoAd Init was not called previosly as per Autosar [SWS_SoAd_00756] */
    SOAD_DET_CHECK_ERROR_RETVAL((FALSE == SoAd_InitFlag_b), SOAD_GETANDRESET_MEASUREMENTDATA_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* SOAD_MEAS_ALL shall only be used to reset all MeasurementIdx's at once. A NULL_PTR shall be provided
       for MeasurementDataPtr in this case */
    SOAD_DET_CHECK_ERROR_RETVAL(((SOAD_MEAS_ALL == MeasurementIdx ) && ((FALSE == MeasurementResetNeeded) || (NULL_PTR != MeasurementDataPtr))), SOAD_GETANDRESET_MEASUREMENTDATA_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    SOAD_DET_CHECK_ERROR_RETVAL((((SOAD_MEAS_DROP_TCP == MeasurementIdx ) || (SOAD_MEAS_DROP_UDP == MeasurementIdx )) && \
            ((FALSE == MeasurementResetNeeded) && (NULL_PTR == MeasurementDataPtr))), SOAD_GETANDRESET_MEASUREMENTDATA_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* based on mesurement index */
    switch(MeasurementIdx)
    {
        /* if measurement index is TCP */
        case SOAD_MEAS_DROP_TCP:
        {
            /* The exclusive area SoAd_MeasurementData is used to avoid race condition when measurement data is stored */
            SchM_Enter_SoAd_MeasurementData();
            /* if the received pointer is not null pointer */
            if(NULL_PTR != MeasurementDataPtr)
            {
                /* copy the Tcp dropped count to recieved measurement pointer */
                *MeasurementDataPtr = SoAd_RxTcpDropCnt_u32;
            }
            /* if the MeasurementResetNeeded flag is true */
            if(TRUE == MeasurementResetNeeded)
            {
                /* reset the dropped count */
                SoAd_RxTcpDropCnt_u32 = 0u;
            }
            SchM_Exit_SoAd_MeasurementData();
            /* Update the return value with E_OK */
            lRetVal_u8 = E_OK;

            break;
        }
        /* if measurement index is UDP  */
        case SOAD_MEAS_DROP_UDP:
        {
            /* The exclusive area SoAd_MeasurementData is used to avoid race condition when measurement data is stored */
            SchM_Enter_SoAd_MeasurementData();
            /* if the received pointer is not null pointer */
            if(NULL_PTR != MeasurementDataPtr)
            {
                /* copy the Udp dropped count to recieved measurement pointer */
                *MeasurementDataPtr = SoAd_RxUdpDropCnt_u32;
            }
            /* if the MeasurementResetNeeded flag is true */
            if(TRUE == MeasurementResetNeeded)
            {
                /* reset the dropped count */
                SoAd_RxUdpDropCnt_u32 = 0u;
            }
            SchM_Exit_SoAd_MeasurementData();
            /* Update the return value with E_OK */
            lRetVal_u8 = E_OK;

            break;
        }
        /* if measurement index is All  */
        case SOAD_MEAS_ALL:
        {
            /* The exclusive area SoAd_MeasurementData is used to avoid race condition when measurement data is stored */
            SchM_Enter_SoAd_MeasurementData();

            /* reset the both Tcp and Udp dropped counts */
            SoAd_RxTcpDropCnt_u32 = 0u;
            SoAd_RxUdpDropCnt_u32 = 0u;

            SchM_Exit_SoAd_MeasurementData();

            lRetVal_u8 = E_OK;

            break;
        }
        default :

            /* Report DET with Invalid argument */
            SOAD_DET_REPORT_ERROR(SOAD_GETANDRESET_MEASUREMENTDATA_API_ID, SOAD_E_INV_ARG)
            /* do nothing */
            break;
    }

    /* return value */
    return lRetVal_u8;
}/* end of Std_ReturnType SoAd_GetAndResetMeasurementData.. */

#endif /* ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON ) */

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
