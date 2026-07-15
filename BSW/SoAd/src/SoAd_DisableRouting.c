

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
 **********************************************************************************************************************
 * \Function Name : SoAd_DisableRouting()
 *
 * \Function description
 *  Disables routing of a group of PDUs in the SoAd related to the RoutingGroup specified by parameter id.
 *
 *  Parameters (in):
 * \param   SoAd_RoutingGroupIdType
 * \arg     id - Id of the soad routing group, for which the routing should be disabled
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
 **********************************************************************************************************************
 */
/* As the message transmission/reception to/from a remote node shall be stopped by two switches.
 * one is, related socket can be made to OFFLINE.
 * Second is, routing group linked with that pdu route dest can be disabled by upper layer.
 * This function allows to disable the routing group, which can stop tx and rx pdus linked with this routing group  even though its socket is ONLINE*/
Std_ReturnType SoAd_DisableRouting( SoAd_RoutingGroupIdType id )
{
#if (SOAD_ROUTING_GROUPS_ENABLE != STD_OFF)
    /* Local pointer to store the SoAd_RoutingGroupSoConStructMap_tst array for given index*/
    SoAd_RoutingGroupSoConStructMap_tst * lSoAdRoutingGroupSoConStructMap_pst;

    /* Variable to hold the return value */
    Std_ReturnType                        lFunctionRetVal_u8;

    /* Loop through for NumofSocket Mapping to RG Id */
    SoAd_SoConIdType                      lIdxSoCon_uo ;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_DISABLE_ROUTING_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether id is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noRoutingGrps_cuo <= id ), SOAD_DISABLE_ROUTING_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* The exclusive area SoAd_RoutingGroupStatus is used to avoid race condition when rg status flag is enabled / Disabled */
    SchM_Enter_SoAd_RoutingGroupStatus();

    /* Initialize the local variables that are declared above */
    /* copy the global array address to local pointer for given index */
    lSoAdRoutingGroupSoConStructMap_pst = &(SoAd_CurrConfig_cpst->SoAd_RoutingGroupSoConStructMap_pst[id]);

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00721] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /*** For RoutingGroups that are referenced by a SoAdPduRouteDest that refers to a SocketConnectionGroup SoAd  ***/
    /*** shall maintain independent states for each SocketConnection that is part of the referenced               ***/
    /*** SocketConnectionGroup and handle them as if they were separate RoutingGroups.                            ***/
    /****************************************************************************************************************/

    /* rg status flag is disabled*/
    lSoAdRoutingGroupSoConStructMap_pst->rgStatus_b = FALSE;
    /* Specific rg status check flag is disabled */
    lSoAdRoutingGroupSoConStructMap_pst->specificRgCheck_b = FALSE;

    /* Loop for each SoConId's whcih are mapped to given Routing Group Id */
    for(lIdxSoCon_uo = 0;
            lIdxSoCon_uo < lSoAdRoutingGroupSoConStructMap_pst->numOfSoConIdMapped_uo;
            lIdxSoCon_uo++)
    {
        /* Disable the routing group by updating its value in Dynamic table,
         * Once the routing group is disabled, there shall be no Tx/Rx for the PDUs linked with this routing group */
        lSoAdRoutingGroupSoConStructMap_pst->routingGrpSoConMap_pst[lIdxSoCon_uo].specificRgStatus_b= FALSE;
        lFunctionRetVal_u8 = E_OK;
    }

    SchM_Exit_SoAd_RoutingGroupStatus();

    /* Return lFunctionRetVal_u8 */
    return(lFunctionRetVal_u8);
#else
    (void)id;
    return(E_NOT_OK);
#endif /* SOAD_ROUTING_GROUPS_ENABLE != STD_OFF */

}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
