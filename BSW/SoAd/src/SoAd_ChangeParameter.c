

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

#if (SOAD_CHANGEPARAMETER_ENABLED != STD_OFF)
LOCAL_INLINE Std_ReturnType SoAd_ConvertParamIdToTcpIpParamIdType( \
                                SoAd_ParamIdType    ParameterId_u8, \
                                TcpIp_ParamIdType * TcpIpParameterId_pen);


#endif /* SOAD_CHANGEPARAMETER_ENABLED != STD_OFF */

/**
 **********************************************************************************************************************
 * \Function Name : SoAd_ChangeParameter()
 *
 * \Function description
 * This API can be requested to control the connection parameter either by SoAd or TcpIp.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index specifying the socket connection which shall be closed
 * \param   SoAd_ParamIdType
 * \arg     ParameterId - Parameter which is requested by UL to get or set the values
 * \param   uint8*
 * \arg     ParameterValuePtr - Pointer to an address which contains the value for the corresponding ParameterId in
 *                              case of set request.
 *                              Else, this address will be filled by TcpIp module if its a Get request.
 *
 *  Parameters (inout): None
 *
 *  Parameters (out):   None
 *
 *  Return value:
 * \retval  E_OK :      The request was successful
 *          E_NOT_OK:   The request was not successful
 *
 **********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION:  Parameter is not const uint8* according to AR specification of this API */
Std_ReturnType SoAd_ChangeParameter( SoAd_SoConIdType SoConId,
                                     SoAd_ParamIdType ParameterId,
                                     uint8 *          ParameterValuePtr )
{
#if (SOAD_CHANGEPARAMETER_ENABLED != STD_OFF)

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Variable to store the return value */
    Std_ReturnType                       lFunctionRetVal_u8;

    /* Variable to store the internal function return value */
    Std_ReturnType                       lRetVal_u8;

    /* Index to store the dynamic socket connection */
    SoAd_SoConIdType                     lIdxDynSocket_uo;

    /* Variable to store the TcpIp socket Id */
    TcpIp_SocketIdType                   lTcpIpSocketId_uo;

    TcpIp_ParamIdType                    lTcpIpParameterId_en;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_CHANGE_PARAMETER_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId & ParameterId are valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ) || \
                               ( (SoAd_ParamIdType)(TCPIP_PARAMID_TCP_OUTOFORDER) < ParameterId ) ),    \
                                 SOAD_CHANGE_PARAMETER_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* Initialization of Variables*/
    lTcpIpSocketId_uo            = (TcpIp_SocketIdType)0xFFFF;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId]);

    /* Get the "dynamic socket connection index" from Static socket table */
    lIdxDynSocket_uo = (*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo);

    /* Condition check: the dynamic socket id should be valid  */
    if( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo )
    {
        /*Check if the SoAd socket is in the right state*/
        if( SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en >= SOAD_SOCON_TCPIP_ALLOCATED )
        {
            /* Get the TcpIp socket Id available in the dynamic table */
            lTcpIpSocketId_uo = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo;

            /*Check if the TcpIp socket is valid, then call the TcpIp_ChangeParameter function*/
            if( lTcpIpSocketId_uo < SOAD_NUM_TCPIP_SOCKETS )
            {
                lRetVal_u8 = SoAd_ConvertParamIdToTcpIpParamIdType(ParameterId, &lTcpIpParameterId_en);

                /* Check whether ParamId is valid or not */
                SOAD_DET_CHECK_ERROR_RETVAL( ( E_OK != lRetVal_u8 ), SOAD_CHANGE_PARAMETER_API_ID, SOAD_E_INV_ARG, E_NOT_OK );

				lFunctionRetVal_u8 = TcpIp_ChangeParameter(lTcpIpSocketId_uo,lTcpIpParameterId_en,ParameterValuePtr);

#if(SOAD_TCP_TLS_ENABLED != STD_OFF)
                if((lFunctionRetVal_u8 == E_OK) && (TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT == lTcpIpParameterId_en))
                {
                    /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                    SchM_Enter_SoAd_SocConModeProperties();

                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en = SOAD_SOCON_TCPWAITFORREMOTE;

                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en = SOAD_SOCON_RECONNECT;

                    SchM_Exit_SoAd_SocConModeProperties();

                    /* call a function which shall invoke the upper layer call back for mode change */
                    SoAd_ModeChgCallbackToUl( SoConId, SOAD_SOCON_RECONNECT );
                }
#endif/* SOAD_TCP_TLS_ENABLED != STD_OFF */

            }
        }
    }


    return(lFunctionRetVal_u8);

#else
    (void)SoConId;
    (void)ParameterId;
    (void)ParameterValuePtr;

    return(E_NOT_OK);

#endif /* SOAD_CHANGEPARAMETER_ENABLED != STD_OFF */
}

#if (SOAD_CHANGEPARAMETER_ENABLED != STD_OFF)
/**
 **************************************************************************************************************
 * \Function Name : SoAd_ConvertParamIdToTcpIpParamIdType()
 *
 * \function description
 * This function is called from SoAd_ChangeParameter to handle the DET error for function arguments.
 *
 * Parameters (in) :
 * \param   SoAd_ParamIdType
 * \arg     ParameterId_u8 - SoAd Parameter Id
 *
 * Parameters (inout): None
 * \param   TcpIp_ParamIdType
 * \arg     TcpIpParameterId_pen - TcpIp Parameter Id pointer in which TcpIp Param id would be copied
 *
 * Parameters (out):   None
 *
 * Return value:       None
 *
 *************************************************************************************************************
 */
LOCAL_INLINE Std_ReturnType SoAd_ConvertParamIdToTcpIpParamIdType( \
                                SoAd_ParamIdType    ParameterId_u8, \
                                TcpIp_ParamIdType * TcpIpParameterId_pen)
{
    Std_ReturnType lRetVal_u8;

    /* initialization of return variable to E_NOT_OK */
    lRetVal_u8 = E_OK;

    switch(ParameterId_u8)
    {
#if (SOAD_TCP_PRESENT != STD_OFF)
        case 0x00U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_RXWND_MAX;
            break;
        }

        case 0x02U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_NAGLE;
            break;
        }

        case 0x03U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_KEEPALIVE;
            break;
        }

        case 0x05U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_KEEPALIVE_TIME;
            break;
        }

        case 0x06U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_KEEPALIVE_PROBES_MAX;
            break;
        }

        case 0x07U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_KEEPALIVE_INTERVAL;
            break;
        }

#if(SOAD_TCP_TLS_ENABLED != STD_OFF)
        case 0x0DU:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TLS_CONNECTION_ASSIGNMENT;
            break;
        }
#endif/* SOAD_TCP_TLS_ENABLED != STD_OFF */

#endif /* SOAD_TCP_PRESENT != STD_OFF */
        case 0x01U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_FRAMEPRIO;
            break;
        }

        case 0x04U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TTL;
            break;
        }

#if (SOAD_IPv6_SUPPORT_ENABLE != STD_OFF)
        case 0x0AU:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_FLOWLABEL;
            break;
        }

        case 0x0BU:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_DSCP;
            break;
        }
#endif

        case 0x80U:
        {
            (*TcpIpParameterId_pen) = TCPIP_PARAMID_TCP_OUTOFORDER;
            break;
        }
        default:
            lRetVal_u8 = E_NOT_OK;
            break;
    }

    return (lRetVal_u8);
}

#endif /* (SOAD_CHANGEPARAMETER_ENABLED != STD_OFF) */

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
