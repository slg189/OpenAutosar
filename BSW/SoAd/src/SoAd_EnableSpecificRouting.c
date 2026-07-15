

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "SoAd.h"

#ifdef SOAD_CONFIGURED

#include "SoAd_Prv.h"

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

/**
 ********************************************************************************************************************
 * \Function Name : SoAd_EnableSpecificRouting()
 *
 * \Function description
 *  Enables routing of a group of PDUs in the SoAd related to the RoutingGroup specified by parameter id
 *  only on the socket connection identified by SoConId.
 *
 *  Parameters (in):
 * \param   SoAd_RoutingGroupIdType
 * \arg     id - Id of the soad routing group, for which the routing should be enabled
 * \param   SoAd_SoConIdType
 * \arg     SoConId - Id of the socket connection index, for which the routing group shall be enabled
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request has been accepted
 *          E_NOT_OK: The request has not been accepted
 *
 *
 *********************************************************************************************************************
 */
/* As the message transmission/reception to/from a remote node shall be enabled by two switches.
 * one is, related socket can be made to ONLINE and .
 * Second is, routing group linked with that pdu route dest shall be enabled by upper layer or enabled at Init.
 * This function allows to enables routing of a group of PDUs in the SoAd related to the RoutingGroup specified by parameter id
 * only on the socket connection identified by SoConId*/
Std_ReturnType SoAd_EnableSpecificRouting( SoAd_RoutingGroupIdType id,
                                           SoAd_SoConIdType        SoConId)
{
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF )
    /* Local pointer to store the SoAd_RoutingGroupSoConStructMap_tst array for given index*/
    SoAd_RoutingGroupSoConStructMap_tst * lSoAdRoutingGroupSoConStructMap_pst;

    /* Variable to hold the return value */
    Std_ReturnType                        lFunctionRetVal_u8;

    /* Loop through for NumofSocket Mapping to RG Id */
    SoAd_SoConIdType                      lIdxSoCon_uo ;

    /* Flag to indicate if routing group has to be enabled */
    boolean                               lRgStatusToBeEnbled_b ;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /* Initialize local variable */
    lRgStatusToBeEnbled_b = TRUE;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_ENABLE_SPECIFIC_ROUTING_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether RgId_uo and SoConId_uo are valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_noRoutingGrps_cuo <= id ) ||
                                 ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ) ),
                                 SOAD_ENABLE_SPECIFIC_ROUTING_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    SchM_Enter_SoAd_RoutingGroupStatus();

    /* Initialize the local variables that are declared above */
    /* copy the global array address to local pointer for given index */
    lSoAdRoutingGroupSoConStructMap_pst = &(SoAd_CurrConfig_cpst->SoAd_RoutingGroupSoConStructMap_pst[id]);

    /* Specific rg status flag is enabled */
    lSoAdRoutingGroupSoConStructMap_pst->specificRgCheck_b = TRUE;

    /* Loop for each SoConId's whcih are mapped to given Routing Group Id */
    for(lIdxSoCon_uo = 0;
            lIdxSoCon_uo < lSoAdRoutingGroupSoConStructMap_pst->numOfSoConIdMapped_uo;
            lIdxSoCon_uo++)
    {
        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00721] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /*** For RoutingGroups that are referenced by a SoAdPduRouteDest that refers to a SocketConnectionGroup SoAd  ***/
        /*** shall maintain independent states for each SocketConnection that is part of the referenced               ***/
        /*** SocketConnectionGroup and handle them as if they were separate RoutingGroups.                            ***/
        /****************************************************************************************************************/

        if(lSoAdRoutingGroupSoConStructMap_pst->routingGrpSoConMap_pst[lIdxSoCon_uo].mappedSoConId_uo == SoConId)
        {
            /* Enable the routing group by updating its value in Dynamic table,
             * Once the routing group is enabled, there shall be Tx/Rx for the PDUs linked with this routing group and sockets, if socket is ONLINE  */
            /* Enable Specific Routing Group only for linked sockets mapped to Routing Group Id */
            lSoAdRoutingGroupSoConStructMap_pst->routingGrpSoConMap_pst[lIdxSoCon_uo].specificRgStatus_b = TRUE;
            lFunctionRetVal_u8 = E_OK;
        }
        else
        {
            /* Check if specific routing group is disabled*/
            if(FALSE == lSoAdRoutingGroupSoConStructMap_pst->routingGrpSoConMap_pst[lIdxSoCon_uo].specificRgStatus_b)
            {
                /* Set the lRgStatusToBeEnbled_b to false if routing group has to be disabled */
                lRgStatusToBeEnbled_b = FALSE;
            }
        }
        /* If SoConId mapped to given routing group is found and routing group is to be disabled */
        if( (E_OK == lFunctionRetVal_u8) && (FALSE == lRgStatusToBeEnbled_b) )
        {
            break;
        }
    }

    /* If routing group has to be enabled */
    if(TRUE == lRgStatusToBeEnbled_b)
    {
        /* rg status flag is enabled*/
        lSoAdRoutingGroupSoConStructMap_pst->rgStatus_b = TRUE;

        /* Specific rg status check is disabled */
        lSoAdRoutingGroupSoConStructMap_pst->specificRgCheck_b = FALSE;
    }

    SchM_Exit_SoAd_RoutingGroupStatus();


    return(lFunctionRetVal_u8);
#else
    (void)id;
    return(E_NOT_OK);
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"
#endif /* SOAD_CONFIGURED */
