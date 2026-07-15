

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED

/* check if diagnostic measurement data feature is enabled */
#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6.h"
#endif

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface Functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_GetAndResetMeasurementData()                                                                               */
/*                             -By this API service the TCP/IP stack can read/reset the diagnostic measurement data.*/
/*                                                                                                                  */
/* Service ID           - 0x45                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant                                                                                 */
/*                                                                                                                  */
/* Parameter IN :                                                                                                   */
/*    MeasurementIdx           -Data index of measurement data                                                      */
/*    MeasurementResetNeeded   -Flag to trigger a reset of the measurement data                                     */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*    MeasurementDataPtr       -Reference to data buffer, where to copy measurement data                            */
/*                                                                                                                  */
/* Std_ReturnType  :           Result of operation                                                                  */
/*      E_OK                   -successful                                                                          */
/*      E_NOT_OK               -failed                                                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_GetAndResetMeasurementData( TcpIp_MeasurementIdxType MeasurementIdx,
                                                 boolean MeasurementResetNeeded,
                                                 uint32 * MeasurementDataPtr)
{
    /* Local variables declaration */
    Std_ReturnType         lStdFunctionRetVal_en;
    Std_ReturnType         lStdRetVal_en;

    /* Initialize local variable. */
    lStdFunctionRetVal_en = E_NOT_OK;

    /* DET Checks */
    /* Report DET : Development Error: Module not initialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET : Development Error: check if arguments are invalid, as TCPIP_MEAS_ALL is only for reset operation*/
    TCPIP_DET_REPORT_ERROR_RET_VALUE( (( FALSE == MeasurementResetNeeded ) && ( MeasurementIdx == TCPIP_MEAS_ALL )),
            TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET : Development Error: check if arguments are invalid, as read operation for TCPIP_MEAS_ALL is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( (( NULL_PTR != MeasurementDataPtr ) && ( MeasurementIdx == TCPIP_MEAS_ALL )),
            TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET : Development Error: check if arguments are invalid, as read operation with NULL pointer is not allowed. */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( (( FALSE == MeasurementResetNeeded ) && ( NULL_PTR == MeasurementDataPtr )),
            TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Check the measurement index provided by the input parameter. */
    switch( MeasurementIdx )
    {
        /*if measurement index is TCPIP_MEAS_DROP_TCP, return/reset the number of datagrams dropped for invalid tcp port*/
        case TCPIP_MEAS_DROP_TCP:
        {
             /* forward the call to submodule EthTcp API*/
#if ( TCPIP_TCP_ENABLED == STD_ON )
            lStdFunctionRetVal_en = rba_EthTcp_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
#else
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOPROTOOPT)
#endif
        }
        break;

        /*if measurement index is TCPIP_MEAS_DROP_UDP, return/reset the number of datagrams dropped for invalid udp port*/
        case TCPIP_MEAS_DROP_UDP:
        {
             /* forward the call to submodule EthUdp API*/
#if ( TCPIP_UDP_ENABLED == STD_ON )
            lStdFunctionRetVal_en = rba_EthUdp_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
#else
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOPROTOOPT)
#endif
        }
        break;

        /*if measurement index is TCPIP_MEAS_DROP_IPV4, return/reset the number of datagrams dropped for invalid ipv4 addr*/
        case TCPIP_MEAS_DROP_IPV4:
        {
              /* forward the call to submodule EthIpv4 API*/
#if ( TCPIP_IPV4_ENABLED == STD_ON )
            lStdFunctionRetVal_en = rba_EthIPv4_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
#else
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOPROTOOPT)
#endif
        }
        break;

        /*if measurement index is TCPIP_MEAS_DROP_IPV6, return/reset the number of datagrams dropped for invalid ipv6 addr*/
        case TCPIP_MEAS_DROP_IPV6:
        {
            /* forward the call to submodule EthIpv6 API*/
#if ( TCPIP_IPV6_ENABLED == STD_ON )
            lStdFunctionRetVal_en = rba_EthIPv6_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
#else
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOPROTOOPT)
#endif
        }
        break;

        /*if measurement index is TCPIP_MEAS_ALL, reset all the measurement data*/
        case TCPIP_MEAS_ALL:
        {
            /* If none of the sub-modules are configured, then throw DET and return E_NOT_OK*/
#if ( (TCPIP_TCP_ENABLED != STD_ON) && (TCPIP_UDP_ENABLED != STD_ON) && (TCPIP_IPV4_ENABLED != STD_ON) && (TCPIP_IPV6_ENABLED != STD_ON) )
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET(TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_NOPROTOOPT)
#else
            lStdFunctionRetVal_en = E_OK;
#endif

            /* forward the call to all submodule API, if any submodule api fails return E_NOT_OK*/
#if ( TCPIP_TCP_ENABLED == STD_ON )
            lStdRetVal_en = rba_EthTcp_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
            lStdFunctionRetVal_en = ( lStdRetVal_en != E_OK ) ? ( E_NOT_OK ) : ( lStdFunctionRetVal_en );
#endif
#if ( TCPIP_UDP_ENABLED == STD_ON )
            lStdRetVal_en = rba_EthUdp_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
            lStdFunctionRetVal_en = ( lStdRetVal_en != E_OK ) ? ( E_NOT_OK ) : ( lStdFunctionRetVal_en );
#endif
#if ( TCPIP_IPV4_ENABLED == STD_ON )
            lStdRetVal_en = rba_EthIPv4_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
            lStdFunctionRetVal_en = ( lStdRetVal_en != E_OK ) ? ( E_NOT_OK ) : ( lStdFunctionRetVal_en );
#endif
#if ( TCPIP_IPV6_ENABLED == STD_ON )
            lStdRetVal_en = rba_EthIPv6_GetAndResetMeasurementData(MeasurementIdx, MeasurementResetNeeded, MeasurementDataPtr);
            lStdFunctionRetVal_en = ( lStdRetVal_en != E_OK ) ? ( E_NOT_OK ) : ( lStdFunctionRetVal_en );
#endif

        }
        break;

        default:
        {
            /* Set return value to E_NOT_OK. */
            /* This line is required to avoid MISRA warning if development error detection is disabled (empty default case is not allowed) */
            lStdFunctionRetVal_en = E_NOT_OK;

            /* Report DET : Development Error: MeasurementIdx is invalid. */
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_GETANDRESET_MEAS_DATA_API_ID, TCPIP_E_INV_ARG )
        }
        break;
    }

    return lStdFunctionRetVal_en;
}


#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* TCPIP_ENABLE_GETANDRESET_MEAS_DATA */
#endif /* #ifdef TCPIP_CONFIGURED */
