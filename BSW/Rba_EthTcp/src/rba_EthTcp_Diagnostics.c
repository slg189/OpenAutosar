

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"

/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */
#define RBA_ETHTCP_START_SEC_VAR_INIT_32
#include "rba_EthTcp_MemMap.h"
/* Total number of TCP datagrams which cannot be mapped to a valid local IP/Port */
uint32 rba_EthTcp_MeasurementData_u32;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_32
#include "rba_EthTcp_MemMap.h"
/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */


/*
 ***************************************************************************************************
 * Local functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_GetAndResetMeasurementData()- This function is used to read and reset detailed measurement data for              **/
/**                                          diagnostic purposes. It returns the number of all TCP datagrams which cannot       **/
/**                                          be mapped to a valid local IP/Port                                                **/
/** Parameters (in):                                                                                                            **/
/** MeasurementIdx          - Data index of measurement data                                                                    **/
/** MeasurementResetNeeded  - Flag to trigger a reset of the measurement data                                                   **/
/**                                                                                                                             **/
/** Parameters (inout):     - None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** MeasurementDataPtr      - Reference to data buffer, where to copy measurement data                                          **/
/**                                                                                                                             **/
/** Return value:                                                                                                               **/
/** Std_ReturnType          - E_OK: successful                                                                                  **/
/**                         - E_NOT_OK: failed                                                                                  **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/

Std_ReturnType rba_EthTcp_GetAndResetMeasurementData(TcpIp_MeasurementIdxType MeasurementIdx, \
                                                     boolean                  MeasurementResetNeeded, \
                                                     uint32 *                 MeasurementDataPtr)
{
    /* Declare local variables */
    Std_ReturnType lFunctionRetVal_en;

    /* Initialize the return value of the function to E_NOT_OK */
    lFunctionRetVal_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if rba_EthTcp module is uninitialized */
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_GETANDRESETMEASUREMENTDATA, RBA_ETHTCP_E_NOTINIT, E_NOT_OK );

    /* Report DET if invalid measurement index is passed*/
    RBA_ETHTCP_CHECK_DETERROR_RETVAL( ((TCPIP_MEAS_DROP_TCP != MeasurementIdx ) && (TCPIP_MEAS_ALL != MeasurementIdx)) , \
                                        RBA_ETHTCP_E_GETANDRESETMEASUREMENTDATA, RBA_ETHTCP_E_INV_ARG, E_NOT_OK );

    switch(MeasurementIdx)
    {
        /* if measurement index is TCPIP_MEAS_DROP_TCP */
        case TCPIP_MEAS_DROP_TCP:
        {
            /* if the received pointer is not null pointer */
            if(NULL_PTR != MeasurementDataPtr)
            {
                /* copy the measurement data count to recieved measurement pointer */
                *MeasurementDataPtr = rba_EthTcp_MeasurementData_u32;
            }
            /* if the MeasurementResetNeeded flag is true */
            if(TRUE == MeasurementResetNeeded)
            {
                /* reset the measurement data count */
                rba_EthTcp_MeasurementData_u32 = 0;
            }
            /* Update the return value with E_OK */
            lFunctionRetVal_en = E_OK;

            break;
        }

        /* if measurement index is TCPIP_MEAS_ALL  */
        case TCPIP_MEAS_ALL:
        {
            /* if the MeasurementResetNeeded flag is true */
            if(TRUE == MeasurementResetNeeded)
            {
                /* reset the measurement data count */
                rba_EthTcp_MeasurementData_u32 = 0;
                /* Update the return value with E_OK */
                lFunctionRetVal_en = E_OK;
            }
            break;
        }

        default :
        {
            /* do nothing */
        }
        break;

    }/* end of switch(MeasurementIdx) */

    return lFunctionRetVal_en;
}/* End of Std_ReturnType rba_EthTcp_GetAndResetMeasurementData */


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON) */
#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
