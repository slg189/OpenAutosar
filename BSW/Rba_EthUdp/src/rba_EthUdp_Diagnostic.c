
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"
#include "rba_EthUdp_Cfg_SchM.h"
#include "TcpIp_Prv.h"
#include "rba_EthUdp_Prv.h"


/* check if diagnostic measurement data feature is enabled */
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)

/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */
#define RBA_ETHUDP_START_SEC_VAR_CLEARED_32
#include "rba_EthUdp_MemMap.h"

/* Counter to keep count of datagrams dropped for port not mapped to valid port*/
uint32 rba_EthUdp_InvalidIpPortDropCnt_u32 = 0UL;

#define RBA_ETHUDP_STOP_SEC_VAR_CLEARED_32
#include "rba_EthUdp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface Functions
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/********************************************************************************************************************/
/* rba_EthUdp_GetAndResetMeasurementData() This function allows to read and reset UDP measurement data for          */
/* diagnostic purposes.                                                                                             */
/*                                                                                                                  */
/* Synchronous, Reentrant (from [SWS_TcpIp_91006])                                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* MeasurementIdx             Data index of measurement data.                                                       */
/* MeasurementResetNeeded     Flag to trigger a reset of the measurement data.                                      */
/*                                                                                                                  */
/* Parameters Out:                                                                                                  */
/* MeasurementDataPtr         Reference to data buffer, where to copy measurement data.                             */
/*                                                                                                                  */
/* Return type :              Std_ReturnType {E_OK: successful; E_NOT_OK: failed}                                   */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_GetAndResetMeasurementData( TcpIp_MeasurementIdxType MeasurementIdx_u8,
                                                      boolean MeasurementResetNeeded_b,
                                                      uint32 * MeasurementDataPtr_pu32)
{
    /* Local variables declaration */
    Std_ReturnType         lStdRetVal_en;

    /* Initialize local variable. */
    lStdRetVal_en = E_NOT_OK;

    /* DET checks */
    /* check if EthUdp is initialised, otherwise raise DET */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (NULL_PTR == rba_EthUdp_Config_pco), RBA_ETHUDP_E_GETANDRESET_MEAS_DATA_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Check the measurement index provided by the input parameter. */
    switch( MeasurementIdx_u8 )
    {
        case TCPIP_MEAS_DROP_UDP :
        {
            /* Enter critical section */
            /* (The clearing of the counter value need to be done in a critical section to avoid race condition with the incrementation */
            /* which is done in the execution context of RxIndication) */
            /* (The saving of the measurement data in the out argument is also part of the critical section to avoid missing */
            /* otherwise the track of the dropped packets between reading the measurement data and the reset of the measurement data will be lost) */
            SchM_Enter_rba_EthUdp_ExclusiveAreaMeasurementData();

            /* If MeasurementDataPtr is not null, copy count value */
            if( MeasurementDataPtr_pu32 != NULL_PTR )
            {
                *MeasurementDataPtr_pu32 = rba_EthUdp_InvalidIpPortDropCnt_u32;
            }

            /* Clear the count value if MeasurementResetNeeded is true */
            if( MeasurementResetNeeded_b != FALSE )
            {
                /* Clear the count value */
                rba_EthUdp_InvalidIpPortDropCnt_u32 = 0UL;
            }

            /* Exit critical section */
            SchM_Exit_rba_EthUdp_ExclusiveAreaMeasurementData();

            lStdRetVal_en = E_OK;
        }
        break;

        case TCPIP_MEAS_ALL :
        {
            if ( ( MeasurementResetNeeded_b != FALSE ) && ( MeasurementDataPtr_pu32 == NULL_PTR ) )
            {
                /* Enter critical section */
                /* (The clearing of the counter value need to be done in a critical section to avoid race condition with the incrementation */
                /* which is done in the execution context of RxIndication) */
                SchM_Enter_rba_EthUdp_ExclusiveAreaMeasurementData();

                /* Clear the count value */
                rba_EthUdp_InvalidIpPortDropCnt_u32 = 0UL;

                /* Exit critical section */
                SchM_Exit_rba_EthUdp_ExclusiveAreaMeasurementData();

                lStdRetVal_en = E_OK;
            }
        }
        break;

        default:
        {
            /* Set return value to E_NOT_OK. */
            /* This line is required to avoid MISRA warning if development error detection is disabled (empty default case is not allowed) */
            lStdRetVal_en = E_NOT_OK;

            /* Report DET : Development Error: MeasurementIdx is invalid. */
            RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_GETANDRESET_MEAS_DATA_API_ID, RBA_ETHUDP_E_INV_ARG )
        }
        break;
    }

    return lStdRetVal_en;
}

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* TCPIP_ENABLE_GETANDRESET_MEAS_DATA */
#endif
