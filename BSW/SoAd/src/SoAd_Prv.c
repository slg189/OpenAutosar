

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv_Domain.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

#ifdef SOAD_DEBUG_AND_TEST
#include "TestSoAdApplication.h"
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

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_IsMatchRxPduFound()
 *
 * \Function description
 * This service shall be invoked to identify whether Rx Pdu is mapped to given socket by matching Rx Pdu Header Id
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - static socket id
 * \param   uint32
 *          RxPduHdrId_32  - Rx Header id
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: boolean (TRUE or FALSE)
 *
 ********************************************************************************************************************
 */

boolean SoAd_IsMatchRxPduFound( SoAd_SoConIdType StaticSocketId_uo, \
                                uint32           RxPduHdrId_32 )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Index for looping across all Rx Pdu's afor particular socket */
    PduIdType                            lIdxRxPduLoop_uo ;

    /* Variable to store the status */
    boolean                              lSoConIdFound_b;

    /*Initialization of local variables */
    /* copy the global array index address to local pointer for further processing */
    lSoAdStaticSocConfig_cpst   = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Initialize the local variables */
    lSoConIdFound_b     = FALSE;

    for ( lIdxRxPduLoop_uo = 0; \
          lIdxRxPduLoop_uo < lSoAdStaticSocConfig_cpst->numberOfRxPdusConfigured_u8; \
          lIdxRxPduLoop_uo++ )
    {
        /* If Rx Pdu's header id is matched with received one */
        if(SoAd_CurrConfig_cpst->SoAd_RxPduConfig_cpst[lSoAdStaticSocConfig_cpst->rxPduIds_pcuo[lIdxRxPduLoop_uo]].rxPduHeaderId_u32 == RxPduHdrId_32)
        {
            lSoConIdFound_b = TRUE;
            break;
        }
    }

    return lSoConIdFound_b;
}

/****************************************************************************************************
 * \Function Name : SoAd_PerformPostPduTransmissionAction()
 *
 * \Function description
 *  This function shall do the activities which has to be done after successfull transmission
 *  of requested PDU's and if it is automatic socket and remote address are wild card.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo    - SoAd static Socket identifier table
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: None
 *
 ***************************************************************************************************
 */
void SoAd_PerformPostPduTransmissionAction( SoAd_SoConIdType StaticSocketId_uo )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst *         lSoAdDyncSocConfig_pst ;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType                     lIdxDynSocket_uo;

    /* Initialize the local variables which are declared above */
    /* copy the global array index address to local pointer for further processing */
    lSoAdStaticSocConfig_cpst   = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo]);

    /* Get the index of the dynamic soc table */
    lIdxDynSocket_uo         = (*lSoAdStaticSocConfig_cpst->SoAd_Prv_idxDynSocTable_puo);

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst   =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

    switch(SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo].soConGrpProtocol_en)
    {
       case TCPIP_IPPROTO_UDP:
       {
           /****************************************************************************************************************/
           /****************************************** [SWS_SoAd_00582] ****************************************************/
           /****************************************************************************************************************/
           /****************************************************************************************************************/
           /** If the transmission is successful and socket is Automatic (a) change the state of the socket connection to ***/
           /** SOAD_SOCON_RECONNECT (b) reset the remote address to the configured remote address after a PDU transmission */
           /****************************************************************************************************************/
           /****************************************************************************************************************/

           /* call a function which shall invoke the upper layer call back for mode change */
           SoAd_ModeChgCallbackToUl( StaticSocketId_uo, SOAD_SOCON_RECONNECT );

           break;
       }

       case TCPIP_IPPROTO_TCP:
       {

           /****************************************************************************************************************/
           /********************************[SWS_SoAd_00651], [SWS_SoAd_00644]: Step(a)*************************************/
           /****************************************************************************************************************/
           /****************************************************************************************************************/
           /****************** SoAd shall disable further transmission or reception for this socket connection *************/
           /****************************************************************************************************************/

           /* set the global variables to skip the further transmission and reception */
           lSoAdDyncSocConfig_pst->discardNewTxRxReq_b  = TRUE;

           /****************************************************************************************************************/
           /********************************[SWS_SoAd_00651], [SWS_SoAd_00644]: Step(b)*************************************/
           /****************************************************************************************************************/
           /****************************************************************************************************************/
           /****************** SoAd shall close the socket connection in the next SoAd_MainFunction() **********************/
           /****************************************************************************************************************/

           /* set the state such that, the socket shall be closed in next main function */
           lSoAdDyncSocConfig_pst->socConRequestedMode_en = SOAD_EVENT_OFFLINEREQ_SOAD ;

           break;
       }

       default:
       {
           /* Nothing to do */
           break;
       }
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_InsertPduHdrToBuffer()
 *
 * \Function description
 * This service shall insert the SoAd header(Pdu Header Id and Pdu length) to buffer
 *
 * Parameters (in):
 * \param   uint32
 * \arg     PduHeaderId_u32 - Pdu Header Id
 * \param   uint32
 * \arg     PduLength_u32 - Pdu length
 *
 * Parameters (inout): None
 *
 * Parameters (out):
 * \param   uint8*
 * \arg     Buffer_pu8 - buffer pointer to which pdu hesder id and length will be copied
 *
 * Return value: None
 *
 ********************************************************************************************************************
 */
void SoAd_InsertPduHdrToBuffer( uint32  PduHeaderId_u32, \
                                uint32  PduLength_u32, \
                                uint8 * Buffer_pu8 )
{
    /* Variable to hold the index in for loop*/
    uint8 lIdxForLoop_uo;

    /* copy the txPduHeaderId_u32 into dest buffer */
    for(lIdxForLoop_uo = 0; lIdxForLoop_uo < SOAD_PDUHDR_PDUID_LENGTH_SIZE ; lIdxForLoop_uo++)
    {
        /* Update the buffer pointer byte by byte for copying header  */
        ( *(Buffer_pu8 + (SOAD_PDUHDR_PDUID_LENGTH_SIZE - 1u - lIdxForLoop_uo)) ) = (uint8)( PduHeaderId_u32  >> (lIdxForLoop_uo*8));

        /* Update the buffer pointer byte by byte for copying Length information  */
        ( *(Buffer_pu8 + (SOAD_PDUHDR_SIZE - 1u - lIdxForLoop_uo)) ) = (uint8)( PduLength_u32  >> (lIdxForLoop_uo*8));

    }/* End of for loop to copy the Header info into the buffer */

}

#if ( (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF) )
/**
 ***********************************************************************************************************************
 * \Function Name : SoAd_GetOtherActiveDynSocketForGroup()
 *
 * \Function description
 * This service shall check whether any other socket is active for the same group
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     StaticSocketId_uo - static socket id whose Tcp config table or Udp config table is to be initiated
 *
 * Parameters (inout): None
 *
 * Parameters (out):
 * \param   SoAd_SoConIdType*
 * \arg     DynSocketId_puo - Dynamic socket id mapped to static socket id
 *
 * Return value: boolean (TRUE or FALSE)
 *
 ***********************************************************************************************************************
 */
boolean SoAd_GetOtherActiveDynSocketForGroup( SoAd_SoConIdType   StaticSocketId_uo,\
                                              SoAd_SoConIdType * DynSocketId_puo )
{
    /* variable to store index of the socket connection Group */
    SoAd_SoConIdType lIdxSoConGroup_uo;

    /* variable to store index of the static socket connection */
    SoAd_SoConIdType lStaticSocketId_uo ;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType lIdxDynSocket_uo;

    /* Variable to Loop through sockets in group */
    SoAd_SoConIdType lIdxForLoop_uo ;

    /* Variable to hold the other active socket availability */
    boolean          lOtherActiveSocketPresent;

    /*Initialization of local variables */
    lOtherActiveSocketPresent = FALSE;

    /* get the value of socket group Id*/
    lIdxSoConGroup_uo           = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[StaticSocketId_uo].soConGrpId_uo ;

    /* fetch the start socket id from Group table and loop through for max socket couent */
    lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].startSoConIdx_uo;

    for( lIdxForLoop_uo = 0; lIdxForLoop_uo < SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lIdxSoConGroup_uo].maxSoConChannel_uo; lIdxForLoop_uo++)
    {
        /* Get the index of the dynamic soc table */
        lIdxDynSocket_uo             = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

        /* Condition checks:
         * 1. Dynamic socket index is valid.
         * 2. Tcpip socket id mapped to dynamic socket index is valid, it means socket is active.
         * 3. should not be same socket
         */
        if( ( SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo > lIdxDynSocket_uo ) &&
            ( SOAD_NUM_TCPIP_SOCKETS > SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socketHandleTcpIp_uo ) &&
            ( StaticSocketId_uo != lStaticSocketId_uo ) )
        {
            /* assign the found dynamic scoket index to output parameter */
            (*DynSocketId_puo) = lIdxDynSocket_uo;

            /* set the return valud to success(TRUE) */
            lOtherActiveSocketPresent = TRUE;
            break;
        }

        /* increment the static socket id to move the next socket in the group */
        ++lStaticSocketId_uo;
    }

    return lOtherActiveSocketPresent;

}
#endif /* (SOAD_TCP_SERVER_GRP_PRESENT != STD_OFF) || (SOAD_UDP_GRP_PRESENT != STD_OFF) */

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
